#include "GamePythonScriptWgt.h"
#include "../CGALib/gameinterface.h"
#include "CGFunction.h"
#include "MINT.h"
#include <intrin.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QTimer>
extern CGA::CGAInterface *g_CGAInterface;

GamePythonScriptWgt::GamePythonScriptWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl, &GameCtrl::NotifyAttachProcessOk, this, &GamePythonScriptWgt::OnNotifyAttachProcessOk, Qt::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::HttpLoadScript, this, &GamePythonScriptWgt::OnHttpLoadScript);
	connect(g_pGameCtrl, &GameCtrl::NotifyFillLoadScript, this, &GamePythonScriptWgt::OnNotifyFillLoadScript, Qt::QueuedConnection);

	m_output = new QPlainTextEdit(this);
	ui.verticalLayout_debug->addWidget(m_output);
	m_output->setReadOnly(true);
	m_output->show();

	m_bDebugging = false;
	m_bListening = false;
	m_bNavigating = false;
	m_bPathBegin = false;
	m_bSuspending = false;
	m_port = 0;
	m_ConsoleMaxLines = 100;
	m_output->setMaximumBlockCount(m_ConsoleMaxLines);
	m_python = new QProcess(this);

	connect(m_python, &QProcess::started, this, &GamePythonScriptWgt::OnNodeStarted);
	connect(m_python, &QProcess::errorOccurred, this, &GamePythonScriptWgt::OnNodeStartError);
	connect(m_python, SIGNAL(readyRead()), this, SLOT(OnNodeReadyRead()));
	connect(m_python, SIGNAL(readyReadStandardOutput()), this, SLOT(OnNodeReadyRead()));
	connect(m_python, SIGNAL(readyReadStandardError()), this, SLOT(OnNodeReadyRead()));
	connect(m_python, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnNodeFinish(int, QProcess::ExitStatus)));

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(OnAutoRestart()));
	timer->start(1000);
}

GamePythonScriptWgt::~GamePythonScriptWgt()
{
}

void GamePythonScriptWgt::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
	{
		event->acceptProposedAction();
	}
}

// 拖拽释放处理函数
void GamePythonScriptWgt::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
	{
		return;
	}

	QString filePath = urls.first().toLocalFile();
	if (filePath.isEmpty())
	{
		return;
	}

	if (!ui.pushButton_term->isEnabled())
	{
		QFile file(filePath);
		if (file.exists())
		{
			m_scriptPath = filePath;
			m_output->appendPlainText(tr("准备运行"));
			ui.pushButton_run->setEnabled(true);
			ui.pushButton_debug->setEnabled(true);
			ui.pushButton_term->setEnabled(false);
			ui.lineEdit_scriptPath->setText(filePath);
		}
		else
		{
			QMessageBox::critical(this, tr("错误"), tr("加载脚本文件失败.\n错误信息: %1").arg(file.errorString()), QMessageBox::Ok, 0);
		}
	}
}

void GamePythonScriptWgt::OnNodeStarted()
{
	if (m_bDebugging)
	{
		m_output->appendPlainText(tr("调试进程id %1...").arg(m_python->processId()));
	}
	else
	{
		m_output->appendPlainText(tr("运行进程id %1...").arg(m_python->processId()));
	}
	ui.pushButton_term->setEnabled(true);
}

void GamePythonScriptWgt::OnNodeStartError(QProcess::ProcessError error)
{
	m_output->appendPlainText(tr("启动Python失败,错误信息: %1").arg(m_python->errorString()));
	ui.pushButton_run->setEnabled(true);
	ui.pushButton_debug->setEnabled(true);
	ui.pushButton_term->setEnabled(false);
}

void GamePythonScriptWgt::OnNodeReadyRead()
{
	if (!m_bListening)
		return;

	if (!m_bDebugging)
	{
		QString data = m_python->readAll();

		m_output->appendPlainText(data);

		if (m_bNavigating)
		{
			if (!m_bPathBegin)
			{
				QLatin1String patternbegin("[PATH BEGIN]");
				int findStart = data.indexOf(patternbegin);
				if (findStart != -1)
				{
					findStart += patternbegin.size();
					m_bPathBegin = true;
					QLatin1String patternend("[PATH END]");
					int findEnd = data.indexOf(patternend, findStart);
					if (findEnd != -1)
					{
						m_PathString += data.mid(findStart, findEnd - findStart);
						m_bPathBegin = false;
						ReportNavigatorPath(m_PathString);
					}
					else
					{
						m_PathString += data.mid(findStart);
					}
				}
			}
			else
			{
				QLatin1String patternend("[PATH END]");
				int findEnd = data.indexOf(patternend);
				if (findEnd != -1)
				{
					m_PathString += data.mid(0, findEnd);
					m_bPathBegin = false;
					ReportNavigatorPath(m_PathString);
				}
				else
				{
					m_PathString += data;
				}
			}
		}
	}
	else
	{
		QByteArray data = m_python->readAll();
		QLatin1String pattern("Debugger listening on ws://");

		int findStart = data.indexOf(pattern);
		if (findStart != -1)
		{
			m_output->appendPlainText(data);
			m_output->appendPlainText(tr("\nCheck \"chrome://inspect\" in chrome to debug the node process."));

			m_bListening = false;
		}
	}
}

void GamePythonScriptWgt::OnNodeFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (m_bNavigating)
	{
		ReportNavigatorFinish(exitCode);
	}

	m_bListening = false;
	m_bNavigating = false;
	m_bPathBegin = false;

	m_output->appendPlainText(tr("Python脚本退出,退出码 %1").arg(exitCode));

	ui.pushButton_load->setEnabled(true);
	ui.pushButton_run->setEnabled(true);
	ui.pushButton_debug->setEnabled(true);
	ui.pushButton_term->setEnabled(false);
	ui.pushButton_suspend->setEnabled(false);

	UpdateGameTextUI(false);

	//restore everything...
	g_CGAInterface->FixMapWarpStuck(2);
}

void GamePythonScriptWgt::OnCloseWindow()
{
	if (m_python->state() == QProcess::Running)
	{
		m_python->kill();
		m_python->waitForFinished();
	}
}

void GamePythonScriptWgt::OnAutoRestart()
{
	if (g_CGAInterface->IsConnected())
	{
		int ingame = 0;
		if (g_CGAInterface->IsInGame(ingame) && ingame)
		{
			CGA::cga_player_info_t playerinfo;
			if (g_CGAInterface->GetPlayerInfo(playerinfo))
			{
				if (playerinfo.health != 0 && ui.checkBox_injuryProt->isChecked())
				{
					if (m_python->state() == QProcess::Running)
					{
						on_pushButton_term_clicked();
						return;
					}
					else
					{
						return;
					}
				}
				if (playerinfo.souls != 0 && ui.checkBox_soulProt->isChecked())
				{
					if (m_python->state() == QProcess::Running)
					{
						on_pushButton_term_clicked();
						return;
					}
					else
					{
						return;
					}
				}
			}

			if (ui.checkBox_freezestop->isChecked())
			{
				int x, y, index1, index2, index3;
				std::string filename;
				if (g_CGAInterface->GetMapXY(x, y) && g_CGAInterface->GetMapIndex(index1, index2, index3, filename))
				{
					if (x != m_LastMapX || y != m_LastMapY || index3 != m_LastMapIndex)
					{
						m_LastMapChange = QTime::currentTime();
					}
					else
					{
						if (m_LastMapChange.elapsed() > 60 * 1000)
						{
							on_pushButton_term_clicked();

							g_CGAInterface->LogBack();
							return;
						}
					}
				}
			}

			if (ui.checkBox_autorestart->isChecked() && m_python->state() != QProcess::Running && !m_scriptPath.isEmpty())
			{
				int worldStatus = 0, gameStatus = 0;
				if (g_CGAInterface->GetWorldStatus(worldStatus) && g_CGAInterface->GetGameStatus(gameStatus) && worldStatus == 9 && gameStatus == 3)
				{
					on_pushButton_run_clicked();
					return;
				}
			}
		}
		else
		{
			if (/*ui.checkBox_autoterm->isChecked() && */ m_python->state() == QProcess::Running)
			{
				on_pushButton_term_clicked();
				return;
			}
		}
	}
}

void GamePythonScriptWgt::on_pushButton_load_clicked()
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(tr("加载脚本"));
	fileDialog->setDirectory(".");
	fileDialog->setNameFilter(tr("Python脚本文件(*.py)"));
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	if (fileDialog->exec() == QDialog::Accepted)
	{
		QString filePath = fileDialog->selectedFiles()[0];
		QFile file(filePath);
		if (file.exists())
		{
			m_scriptPath = filePath;
			m_output->appendPlainText(tr("准备运行"));
			ui.pushButton_run->setEnabled(true);
			ui.pushButton_debug->setEnabled(true);
			ui.pushButton_term->setEnabled(false);
			ui.lineEdit_scriptPath->setText(filePath);
		}
		else
		{
			QMessageBox::critical(this, tr("错误"), tr("加载Python脚本错误.\n错误信息: %1").arg(file.errorString()), QMessageBox::Ok, 0);
		}
	}
}

void GamePythonScriptWgt::on_pushButton_run_clicked()
{
	int ingame = 0;
	if (m_port && ui.pushButton_run->isEnabled() && m_python->state() != QProcess::Running && g_CGAInterface->IsInGame(ingame) && ingame)
	{
		ui.pushButton_run->setEnabled(false);
		ui.pushButton_debug->setEnabled(false);
		ui.pushButton_term->setEnabled(false);
		ui.pushButton_load->setEnabled(false);
		ui.pushButton_suspend->setEnabled(true);
		ui.pushButton_suspend->setText(tr("暂停"));

		m_output->appendPlainText(tr("Starting python..."));

		m_bDebugging = false;
		m_bListening = true;
		m_bNavigating = false;
		m_bPathBegin = false;

		m_output->clear();

		QStringList args;
		QFileInfo fileInfo(m_scriptPath);

		args.append(fileInfo.fileName());
		args.append(QString::number(m_port));

		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

		env.insert("CGA_GAME_PORT", qgetenv("CGA_GAME_PORT"));
		env.insert("CGA_GUI_PORT", qgetenv("CGA_GUI_PORT"));
		env.insert("CGA_DIR_PATH", qgetenv("CGA_DIR_PATH"));
		env.insert("CGA_GUI_PID", QString("%1").arg(GetCurrentProcessId()));
		env.insert("NODE_SKIP_PLATFORM_CHECK", "1");
		env.insert("PYTHONIOENCODING", "utf-8");
		//env.insert("PATH", qgetenv("PATH")+";"+QDir::currentPath());
		m_python->setProcessEnvironment(env);

		m_python->setWorkingDirectory(fileInfo.dir().absolutePath());
		m_python->setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);
		m_python->start(".//python//python.exe", args);

		UpdateGameTextUI(true);
	}
}

void GamePythonScriptWgt::StopNavigatorScript()
{
	if (m_bNavigating)
	{
		on_pushButton_term_clicked();
	}
}

void GamePythonScriptWgt::UpdateGameTextUI(bool show)
{
	if (show && m_scriptPath.size())
	{
		QFileInfo fileInfo(m_scriptPath);
		QString fileName = tr("Executing: %1").arg(fileInfo.fileName());
		std::string path = fileName.toUtf8().toStdString();

		g_CGAInterface->SetGameTextUICurrentScript(path);
	}
	else
	{
		g_CGAInterface->SetGameTextUICurrentScript("");
	}
}

bool GamePythonScriptWgt::IsRuningScript()
{
	return ui.pushButton_run->isEnabled();
}

QString GamePythonScriptWgt::GetLoginScriptData()
{
	QString sLoginArg;
	sLoginArg = QString(" -loadscript=\"%2\" -scriptautorestart=%3 -scriptfreezestop=%4 -scriptautoterm=%5"
						" -injuryprotect=%6 -soulprotect=%7 -logbackrestart=%8 -transuserinput=%9 ")
						.arg(m_scriptPath)
						.arg(ui.checkBox_autorestart->isChecked())
						.arg(ui.checkBox_freezestop->isChecked())
						.arg(false)
						.arg(ui.checkBox_injuryProt->isChecked())
						.arg(ui.checkBox_soulProt->isChecked())
						.arg(false)
						.arg(false);
	return sLoginArg;
}

void GamePythonScriptWgt::RunNavigatorScript(int x, int y, int enter, QString *result)
{
	if (!m_bNavigating && !m_bListening && !m_bDebugging && m_python->state() != QProcess::Running)
	{
		QString filePath = QCoreApplication::applicationDirPath() + "/navigator.js";
		m_scriptPath = filePath;
		m_output->appendPlainText(tr("Ready to launch"));
		ui.lineEdit_scriptPath->setText(filePath);

		ui.pushButton_run->setEnabled(false);
		ui.pushButton_debug->setEnabled(false);
		ui.pushButton_term->setEnabled(false);
		ui.pushButton_load->setEnabled(false);
		ui.pushButton_suspend->setEnabled(false);
		ui.checkBox_autorestart->setChecked(false);

		m_output->appendPlainText(tr("Starting node..."));

		m_bDebugging = false;
		m_bListening = true;
		m_bNavigating = true;
		m_bPathBegin = false;
		m_PathString = QString();

		m_output->clear();

		QStringList args;
		QFileInfo fileInfo(m_scriptPath);

		args.append(fileInfo.fileName());
		args.append(QString::number(m_port));
		args.append(QString::number(x));
		args.append(QString::number(y));
		args.append(QString::number(enter));

		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

		env.insert("CGA_GAME_PORT", qgetenv("CGA_GAME_PORT"));
		env.insert("CGA_GUI_PORT", qgetenv("CGA_GUI_PORT"));
		env.insert("CGA_DIR_PATH", qgetenv("CGA_DIR_PATH"));
		env.insert("CGA_DIR_PATH_UTF8", qgetenv("CGA_DIR_PATH_UTF8"));
		env.insert("CGA_GUI_PID", QString("%1").arg(GetCurrentProcessId()));
		env.insert("NODE_SKIP_PLATFORM_CHECK", "1");
		env.insert("PYTHONIOENCODING", "utf-8");
		m_python->setProcessEnvironment(env);

		m_python->setWorkingDirectory(fileInfo.dir().absolutePath());
		m_python->setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);
		m_python->start(".//python//python.exe", args);

		UpdateGameTextUI(true);
	}
	else
	{
		if (result)
			*result = tr("Navigation is unavailable while running other scripts.");
	}
}

void GamePythonScriptWgt::on_pushButton_debug_clicked()
{
	QMessageBox::information(this, "提示：", "推荐用VsCode打开脚本文件后，直接断点调试，"
											 "也可以用windows自带Cmd命令行调试，格式：\n python.exe -m pdb test.py",
			"确定");
	return;
	if (m_port && ui.pushButton_debug->isEnabled() && m_python->state() != QProcess::Running)
	{
		ui.pushButton_run->setEnabled(false);
		ui.pushButton_debug->setEnabled(false);
		ui.pushButton_term->setEnabled(false);
		ui.pushButton_load->setEnabled(false);
		ui.pushButton_suspend->setEnabled(false);

		m_output->appendPlainText(tr("Starting node in debug mode..."));

		m_bDebugging = true;
		m_bListening = true;
		m_bNavigating = false;
		m_bPathBegin = false;

		QStringList args;
		QFileInfo fileInfo(m_scriptPath);

		args.append("--inspect");
		args.append(fileInfo.fileName());
		args.append(QString::number(m_port));

		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

		env.insert("CGA_GAME_PORT", qgetenv("CGA_GAME_PORT"));
		env.insert("CGA_GUI_PORT", qgetenv("CGA_GUI_PORT"));
		env.insert("CGA_DIR_PATH", qgetenv("CGA_DIR_PATH"));
		env.insert("CGA_DIR_PATH_UTF8", qgetenv("CGA_DIR_PATH_UTF8"));
		env.insert("CGA_GUI_PID", QString("%1").arg(GetCurrentProcessId()));
		env.insert("NODE_SKIP_PLATFORM_CHECK", "1");
		env.insert("PYTHONIOENCODING", "utf-8");
		m_python->setProcessEnvironment(env);

		m_python->setWorkingDirectory(fileInfo.dir().absolutePath());
		m_python->setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);
		m_python->start(".//python//python.exe", args);

		UpdateGameTextUI(true);
	}
}

void GamePythonScriptWgt::on_pushButton_term_clicked()
{
	if (ui.pushButton_term->isEnabled() && m_python->state() == QProcess::Running)
	{

		ui.pushButton_term->setEnabled(false);

		m_python->kill();

		if (m_bDebugging)
			m_output->clear();
	}
}

void GamePythonScriptWgt::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd)
{
	m_port = port;

	QByteArray qportString = QString("%1").arg(port).toLocal8Bit();
	qputenv("CGA_GAME_PORT", qportString);
}

void GamePythonScriptWgt::OnNotifyFillLoadScript(QString path, bool autorestart, bool freezestop, bool injuryprot, bool soulprot, int consolemaxlines, int logBackRestart, int transInput)
{
	if (path.endsWith("lua") || path.endsWith("luae"))
		return;
	if (!path.isEmpty())
	{
		if (QFile::exists(path) == false)
		{
			path = QApplication::applicationDirPath() + "//" + path;
		}
		QFile file(path);
		if (file.exists())
		{
			m_scriptPath = path;
			m_output->appendPlainText(tr("Ready to launch"));
			ui.pushButton_run->setEnabled(true);
			ui.pushButton_debug->setEnabled(true);
			ui.pushButton_term->setEnabled(false);
			ui.lineEdit_scriptPath->setText(path);
			on_pushButton_run_clicked();
		}
	}

	if (autorestart)
		ui.checkBox_autorestart->setChecked(true);
	if (injuryprot)
		ui.checkBox_injuryProt->setChecked(true);
	if (soulprot)
		ui.checkBox_soulProt->setChecked(true);
	if (freezestop)
		ui.checkBox_freezestop->setChecked(true);

	m_ConsoleMaxLines = consolemaxlines;
	m_output->setMaximumBlockCount(m_ConsoleMaxLines);
}

void GamePythonScriptWgt::on_pushButton_suspend_clicked()
{
	if (ui.pushButton_suspend->isEnabled() && m_python->state() == QProcess::Running)
	{

		ui.pushButton_suspend->setEnabled(false);

		bool bSuccess = false;

		if (!m_bSuspending)
		{
			HANDLE ProcessHandle = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, (DWORD)m_python->processId());
			if (ProcessHandle)
			{
				if (STATUS_SUCCESS == NtSuspendProcess(ProcessHandle))
				{
					UpdateGameTextUI(false);
					m_bSuspending = true;
					ui.pushButton_suspend->setText(tr("继续"));
					ui.pushButton_suspend->setEnabled(true);
					bSuccess = true;

					g_CGAInterface->FixMapWarpStuck(2);
				}
				CloseHandle(ProcessHandle);
			}
		}
		else
		{
			HANDLE ProcessHandle = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, (DWORD)m_python->processId());
			if (ProcessHandle)
			{
				if (STATUS_SUCCESS == NtResumeProcess(ProcessHandle))
				{
					UpdateGameTextUI(true);
					m_bSuspending = false;
					ui.pushButton_suspend->setText(tr("暂停"));
					ui.pushButton_suspend->setEnabled(true);
					bSuccess = true;
				}
				CloseHandle(ProcessHandle);
			}
		}

		if (!bSuccess)
		{
			ui.pushButton_suspend->setEnabled(true);
		}
	}
}

void GamePythonScriptWgt::OnHttpLoadScript(QString query, QByteArray postdata, QJsonDocument *doc)
{
	QJsonObject obj;

	QJsonParseError err;
	auto newdoc = QJsonDocument::fromJson(postdata, &err);
	if (err.error == QJsonParseError::NoError && newdoc.isObject())
	{
		obj.insert("errcode", 0);
		auto newobj = newdoc.object();
		if (newobj.contains("path"))
		{
			auto qpath = newobj.take("path");
			if (qpath.isString())
			{
				auto path = qpath.toString();
				if (!path.isEmpty())
				{
					QFile file(path);
					if (file.exists())
					{
						m_scriptPath = path;
						ui.lineEdit_scriptPath->setText(path);
					}
					else
					{
						obj.insert("errcode", 3);
						obj.insert("message", tr("script file not exists."));
						goto end;
					}
				}
				else
				{
					obj.insert("errcode", 4);
					obj.insert("message", tr("script path cannot be empty."));
					goto end;
				}
			}
		}
		if (newobj.contains("autorestart"))
		{
			auto qautorestart = newobj.take("autorestart");
			if (qautorestart.isBool())
			{
				ui.checkBox_autorestart->setChecked(qautorestart.toBool());
			}
		}
		if (newobj.contains("injuryprot"))
		{
			auto qinjuryprot = newobj.take("injuryprot");
			if (qinjuryprot.isBool())
			{
				ui.checkBox_injuryProt->setChecked(qinjuryprot.toBool());
			}
		}
		if (newobj.contains("soulprot"))
		{
			auto qsoulprot = newobj.take("soulprot");
			if (qsoulprot.isBool())
			{
				ui.checkBox_soulProt->setChecked(qsoulprot.toBool());
			}
		}
	}
	else
	{
		obj.insert("errcode", 1);
		obj.insert("message", tr("json parse error"));
	}
end:
	doc->setObject(obj);
}
