#if _MSC_VER >= 1600					 ///1600为vs2010编译器版本
#pragma execution_character_set("utf-8") ///c++11特性  设置执行编码
#endif
#include "MLAssist.h"
#include "../include/ITPublic.h"
#include "../include/qxtglobalshortcut5/QxtGlobalShortcut.h"
#include "FZParseScript.h"
#include "GameCtrl.h"
#include <math.h>
#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QToolButton>
#include <QtConcurrent>
MLAssist::MLAssist(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_StyledBackground);
	m_bIsInLoginProgress = false;
	//setWindowFlags(Qt::Tool);
	//	setWindowFlags(Qt::FramelessWindowHint | Qt::Popup );//| Qt::Tool | Qt::WindowStaysOnTopHint
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	createSysTrayAction();
	connect(g_pGameCtrl, SIGNAL(signal_updateTrayToolTip(const QString &)), this, SLOT(onUpdateTrayToolTip(const QString &)));
	connect(g_pGameCtrl, SIGNAL(signal_loadUserConfig(const QString &)), this, SLOT(on_load_config(const QString &)));
	connect(g_pGameCtrl, SIGNAL(signal_saveUserConfig(const QString &)), this, SLOT(on_save_config(const QString &)));
	connect(g_pGameCtrl, SIGNAL(signal_saveLoginBat(int)), this, SLOT(SaveLoginBat(int)));
	connect(g_pGameCtrl, SIGNAL(signal_ctrl_app(int)), this, SLOT(on_ctrl_app(int)));
	connect(g_pGameCtrl, SIGNAL(signal_attachGame()), this, SLOT(on_switch_ui_tab()));
	connect(g_pGameCtrl, &GameCtrl::HttpGetSettings, this, &MLAssist::save_json_config, Qt::DirectConnection);
	connect(g_pGameCtrl, &GameCtrl::HttpLoadSettings, this, &MLAssist::OnHttpLoadSettings, Qt::DirectConnection);

	connect(g_pGameCtrl, SIGNAL(NotifyLoginProgressStart()), this, SLOT(OnNotifyLoginProgressStart()), Qt::QueuedConnection);
	connect(g_pGameCtrl, SIGNAL(NotifyLoginProgressEnd()), this, SLOT(OnNotifyLoginProgressEnd()), Qt::QueuedConnection);
	ui.widget_title->installEventFilter(this);
	//	QToolButton* m_tabClosebutton = new QToolButton(this);
	//	m_tabClosebutton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
	//ui.tabWidget->removeTab(13);
	//ui.tabWidget->removeTab(11);
	//ui.tabWidget->removeTab(10);
	//	ui.tabWidget->removeTab(12);
	/*ui.tabWidget->setTabVisible(10, false);
	ui.tabWidget->setTabVisible(11, false);
	ui.tabWidget->setTabVisible(13, false);*/

	auto pTabBar = ui.tabWidget->tabBar();

	m_pLoadBtn = new QPushButton("读取", this);
	m_pSaveBtn = new QPushButton("保存", this);
	connect(m_pLoadBtn, SIGNAL(clicked()), this, SLOT(on_pushButton_loadCfg_clicked()));
	connect(m_pSaveBtn, SIGNAL(clicked()), this, SLOT(on_pushButton_saveCfg_clicked()));
	m_pLoadBtn->setStyleSheet("color: rgb(255, 0, 0);");
	m_pSaveBtn->setStyleSheet("color: rgb(0,0,255);");

	m_pLoadBtn->setMaximumWidth(40);
	m_pSaveBtn->setMaximumWidth(40);
	QWidget *pWidget = new QWidget(this);
	QHBoxLayout *pHLayout = new QHBoxLayout(pWidget);
	pHLayout->addWidget(m_pLoadBtn);
	pHLayout->addWidget(m_pSaveBtn);
	pHLayout->setContentsMargins(0, 0, 0, 0);
	ui.tabWidget->setCornerWidget(pWidget);
	connect(ui.attachedWindget, SIGNAL(signal_followGameWnd(long, long, long, long)), this, SLOT(FollowGameWnd(long, long, long, long)));

	/*g_pGameCtrl->moveToThread(&m_gameCtrlThread);
	connect(&m_gameCtrlThread, SIGNAL(finished()), g_pGameCtrl, SLOT(deleteLater()));

	g_pAutoBattleCtrl->moveToThread(&m_battleWorkerThread);
	connect(&m_battleWorkerThread, SIGNAL(finished()), g_pAutoBattleCtrl, SLOT(deleteLater()));
	m_gameCtrlThread.start();
	m_battleWorkerThread.start();*/
	//	g_pGameFun->moveToThread(&m_funThread);
	//	m_funThread.start();
}

MLAssist::~MLAssist()
{
	quitAndDeleteAllInfo();
}
void MLAssist::createSysTrayAction()
{
	/*m_pMinimizeAction = new QAction("最小化", this);		///托盘菜单项
	connect(m_pMinimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

	m_pMaximizeAction = new QAction("最大化", this);
	connect(m_pMaximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

	m_pRestoreAction = new QAction("恢复", this);
	connect(m_pRestoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));*/

	m_pQuitAction = new QAction("退出", this);
	connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(quitAndDeleteAllInfo()));
	m_pTrayIconMenu = new QMenu(this); ///托盘菜单
	//m_pTrayIconMenu->addAction(m_pMinimizeAction);
	//m_pTrayIconMenu->addAction(m_pMaximizeAction);
	//m_pTrayIconMenu->addAction(m_pRestoreAction);
	m_pTrayIconMenu->addSeparator();
	m_pTrayIconMenu->addAction(m_pQuitAction);

	m_pTrayIcon = new QSystemTrayIcon(this); ///托盘图标
	connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(doIconActivated(QSystemTrayIcon::ActivationReason)));
	m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
	m_pTrayIcon->setToolTip("魔力辅助");
	m_pTrayIcon->setIcon(QIcon(":/MLAssist/Resources/logo.png"));
	m_pTrayIcon->show();
	//	m_pTrayIcon->showMessage(QString("后台更新："), QString("正在检查最新版本信息，请稍后！"));	//显示的数据，自动几秒后消失
}
void MLAssist::onUpdateTrayToolTip(const QString &szText)
{
	QString sTitle = szText;
	if (m_bIsInLoginProgress)
		sTitle += "(自动登录游戏中...)";
	if (m_pTrayIcon->toolTip() != sTitle)
		m_pTrayIcon->setToolTip(sTitle);
	if (this->windowTitle() != sTitle)
		this->setWindowTitle(sTitle);
	if (ui.label_title->text() != sTitle)
		ui.label_title->setText(sTitle);
}
void MLAssist::closeEvent(QCloseEvent *event)
{
	if (QMessageBox::information(this, "提示：", "是否关闭辅助？", "是", "否") == QString("是").toInt())
	{
		event->accept();
	}
	else
	{
		event->ignore(); // event->accept();
		return;
	}
}

void MLAssist::on_ctrl_app(int val)
{

	switch (val)
	{
		case 0:
		case 1:
		{
			if (this->isHidden())
			{
				this->show();
			}
			else
				this->hide();
			break;
		}
		case 3:
		{
			if (this->isHidden())
			{
				//	qDebug() << "show";
				HWND hwnd = g_pGameCtrl->getGameHwnd();
				if (hwnd == nullptr)
				{
					this->show();
					return;
				}
				RECT gameRect;
				GetWindowRect((HWND)hwnd, &gameRect);
				if (g_pGameCtrl->GetFollowGamePos() == 0)
					FollowGameWnd(gameRect.left, gameRect.top, gameRect.right, gameRect.bottom);
				else if (g_pGameCtrl->GetFollowGamePos() == 1)
					this->move(gameRect.left, gameRect.bottom - this->height());
				else if (g_pGameCtrl->GetFollowGamePos() == 1)
					this->move(gameRect.left, (gameRect.bottom - gameRect.top) / 2);
				//this->setWindowFlag(Qt::WindowStaysOnTopHint,true);
				//this->setVisible(true);
				//this->setWindowFlag(Qt::WindowStaysOnTopHint, false);
				showNormal();
				SetWindowPos((HWND)this->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				SetWindowPos((HWND)this->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				SetForegroundWindow((HWND)this->winId());
				SetForegroundWindow(hwnd);
			}
			else
			{
				//		qDebug() << "hide";
				this->setVisible(false);
			}
			break;
		}
		default:
			break;
	}
}

///处理托盘图标点击后的信号 槽函数
void MLAssist::doIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
		{
			showNormal(); ///恢复
			g_pGameCtrl->OnSyncGameWindow(true);
			break;
		}
		case QSystemTrayIcon::MiddleClick:
			break;
		default:;
	}
}
void MLAssist::quitAndDeleteAllInfo()
{
	killProcess();
	g_pGameCtrl->SetExitGame(true);
	g_pGameCtrl->StopUpdateTimer();
	m_funThread.quit();
	m_funThread.wait();
	m_gameCtrlThread.quit();
	m_gameCtrlThread.wait();

	m_battleWorkerThread.quit();
	m_battleWorkerThread.wait();
	g_pGameCtrl->WaitThreadFini();
	qApp->exit(0);
}

void MLAssist::on_pushButton_min_clicked()
{
	this->showMinimized();
}

void MLAssist::on_pushButton_close_clicked()
{
	close();
	//hide();
	//g_pGameCtrl->OnSyncGameWindow(false);
}
void MLAssist::killProcess()
{
	QString appPath = QCoreApplication::applicationFilePath();
	int pos = appPath.lastIndexOf("/");
	QString appName = appPath.mid(pos + 1, appPath.length() - pos);
	auto curPid = QCoreApplication::applicationPid();
	QString killProcess = QCoreApplication::applicationDirPath() + "\\ITKillProcess.exe";
	QString killTime = "3"; ///3秒后自动杀进程
	QStringList killProcessList(QStringList() << appName << QString::number(curPid) << killTime);
	QProcess::startDetached(killProcess, killProcessList);
}

void MLAssist::on_pushButton_loadCfg_clicked()
{
	QString szConfigName = QCoreApplication::applicationDirPath() + "//配置//";
	auto pGamePlayer = g_pGameCtrl->getGameCharacter();
	if (pGamePlayer)
	{
		szConfigName = szConfigName + QString("/%1.save").arg(pGamePlayer->name);
	}
	if (!m_lastOpenCfgPath.isEmpty())
		szConfigName = m_lastOpenCfgPath;

	QString path = QFileDialog::getOpenFileName(this, "选中配置", szConfigName, "*.save");
	if (path.isEmpty())
		return;
	m_lastOpenCfgPath = path;
	on_load_config(path);
}

void MLAssist::on_pushButton_saveCfg_clicked()
{
	QString szConfigName = QCoreApplication::applicationDirPath() + "//配置//";
	auto pGamePlayer = g_pGameCtrl->getGameCharacter();
	if (pGamePlayer)
	{
		szConfigName = szConfigName + QString("./%1.save").arg(pGamePlayer->name);
	}
	if (!m_lastOpenCfgPath.isEmpty())
		szConfigName = m_lastOpenCfgPath;

	QString path = QFileDialog::getSaveFileName(this, "保存配置", szConfigName, "*.save");
	if (path.isEmpty())
		return;
	//g_pGameCtrl->SaveConfig(path);
	//还是在这调用把
	m_lastOpenCfgPath = path;
	on_save_config(path);
}

void MLAssist::on_load_config(const QString &sPath)
{
	QString sCfgPath = sPath;

	if (sCfgPath.isEmpty())
		return;
	if (!QFile::exists(sCfgPath))
		sCfgPath = QCoreApplication::applicationDirPath() + "//" + sPath;
	if (!QFile::exists(sCfgPath))
		return;
	m_lastOpenCfgPath = sCfgPath;

	qDebug() << "Load Config" << sCfgPath;
	//	g_pGameCtrl->LoadConfig(path);
	QSettings iniFile(sCfgPath, QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("GB2312")); //这样分组下的键值可以读取中文  下面的是读取段的中文
	ui.gameChatWgt->doLoadUserConfig(iniFile);
	ui.gameDataWgt->doLoadUserConfig(iniFile);
	ui.gameBattleWgt->doLoadUserConfig(iniFile);
	ui.gamePostwarWgt->doLoadUserConfig(iniFile);
	ui.gamePetWgt->doLoadUserConfig(iniFile);
	ui.gameFuncWgt->doLoadUserConfig(iniFile);
	ui.gameBattleSetWgt->doLoadUserConfig(iniFile);
	ui.gameWorkWgt->doLoadUserConfig(iniFile);
	ui.gameLuaWgt->doLoadUserConfig(iniFile);
}

void MLAssist::on_save_config(const QString &sPath)
{
	qDebug() << "Save Config" << sPath;
	QSettings iniFile(sPath, QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("GB2312")); //这样分组下的键值可以读取中文  下面的是读取段的中文
	ui.gameChatWgt->doSaveUserConfig(iniFile);
	ui.gameDataWgt->doSaveUserConfig(iniFile);
	ui.gameBattleWgt->doSaveUserConfig(iniFile);
	ui.gamePostwarWgt->doSaveUserConfig(iniFile);
	ui.gamePetWgt->doSaveUserConfig(iniFile);
	ui.gameBattleSetWgt->doSaveUserConfig(iniFile);
	ui.gameFuncWgt->doSaveUserConfig(iniFile);
	ui.gameWorkWgt->doSaveUserConfig(iniFile);
	ui.gameLuaWgt->doSaveUserConfig(iniFile);
}

void MLAssist::load_json_config(QJsonDocument &doc)
{
	//ui.gameChatWgt->doLoadJsConfig(doc);
	//ui.gameDataWgt->doLoadJsConfig(doc);
	//ui.gameBattleWgt->doLoadJsConfig(doc);
	//ui.gamePostwarWgt->doLoadJsConfig(doc);
	/*ui.gamePetWgt->doLoadJsConfig(doc);
	ui.gameFuncWgt->doLoadJsConfig(doc);
	ui.gameBattleSetWgt->doLoadJsConfig(doc);
	ui.gameWorkWgt->doLoadJsConfig(doc);
	ui.gameLuaWgt->doLoadJsConfig(doc);*/
}
void MLAssist::OnHttpLoadSettings(QString query, QByteArray postdata, QJsonDocument *doc)
{
	QJsonObject obj;

	QJsonDocument newdoc;
	if (ParseSettings(postdata, newdoc))
	{
		obj.insert("errcode", 0);
	}
	else
	{
		obj.insert("errcode", 1);
		obj.insert("message", tr("json parse error"));
	}

	doc->setObject(obj);
}

bool MLAssist::ParseSettings(const QByteArray &data, QJsonDocument &doc)
{
	QJsonParseError err;
	doc = QJsonDocument::fromJson(data, &err);
	if (err.error != QJsonParseError::NoError)
		return false;

	if (!doc.isObject())
		return false;

	QJsonObject obj = doc.object();

	ui.gameBattleWgt->doLoadJsConfig(obj);
	ui.gamePostwarWgt->doLoadJsConfig(obj);
	ui.gameFuncWgt->doLoadJsConfig(obj);	
	
	if (obj.contains("chat"))
	{
		//ParseChatSettings(obj.take("chat"));
	}
	return true;
}
void MLAssist::save_json_config(QJsonDocument &doc)
{
	QJsonObject obj;
	//ui.gameChatWgt->doSaveJsConfig(obj);
	//ui.gameDataWgt->doSaveJsConfig(obj);
	ui.gameBattleWgt->doSaveJsConfig(obj);
	ui.gamePostwarWgt->doSaveJsConfig(obj);
	//ui.gamePetWgt->doSaveJsConfig(obj);
	//ui.gameBattleSetWgt->doSaveJsConfig(obj);
	ui.gameFuncWgt->doSaveJsConfig(obj);
	//ui.gameWorkWgt->doSaveJsConfig(obj);
	//ui.gameLuaWgt->doSaveJsConfig(obj);
	QJsonObject chat;
	obj.insert("chat", chat);
	doc.setObject(obj);
}

void MLAssist::FollowGameWnd(long left, long top, long right, long bottom)
{
	qDebug() << left << bottom;
	if (left < 0 || bottom < 0)
		return;

	this->move(left, bottom);
}
//保存登录脚本0
void MLAssist::SaveLoginBat(int type)
{
	QString saveDir = QCoreApplication::applicationDirPath() + "//启动配置//";
	QDir dir(saveDir);
	if (!dir.exists())
	{
		dir.mkdir(saveDir);
	}
	QString savePath = QString("%1\\%2.bat").arg(saveDir).arg(g_pGameCtrl->getGameCharacter()->name);

	QString path = QFileDialog::getSaveFileName(this, "保存启动批处理", savePath, "*.bat");
	if (path.isEmpty())
		return;
	QString sBat;
	//sBat = QString("start \"\" \"%1\"").arg(QCoreApplication::applicationDirPath() + "//MLAssist.exe");
	sBat = QString("start \"\" \"%1\"").arg("../MLAssist.exe");
	QString sLoginArgs = ui.attachedWindget->GetLoginGameData();
	QString sOpenCfgPath = m_lastOpenCfgPath;
	//配置路径和脚本路径 用相对路径，方便移植到其他电脑
	sOpenCfgPath = sOpenCfgPath.replace(QCoreApplication::applicationDirPath(), ".");
	sOpenCfgPath = sOpenCfgPath.replace(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()), ".");
	QString sReadCfgArgs = QString(" -loadsettings=\"%1\"")
								   .arg(sOpenCfgPath);
	QString sScriptArgs = ui.gameLuaWgt->GetLoginScriptData();
	sScriptArgs = sScriptArgs.replace(QCoreApplication::applicationDirPath(), ".");
	sScriptArgs = sScriptArgs.replace(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()), ".");
	switch (type)
	{
		case 0:
		{
			sBat += sLoginArgs;
			break;
		}
		case 1:
		{
			sBat += sLoginArgs;
			sBat += sReadCfgArgs;
			break;
		}
		case 2:
		{
			sBat += sLoginArgs;
			sBat += sReadCfgArgs;
			sBat += sScriptArgs;
			break;
		}
		default:
			break;
	}

	QFile file(path);
	if (file.open(QFile::ReadWrite))
	{
		QTextStream filestream(&file);							//构建文本流
		QTextCodec *codec = QTextCodec::codecForName("GB2312"); //设置编码
		filestream.setCodec(codec);
		filestream << sBat;
		//file.write(sBat.toStdString().c_str());
	}
	file.close();
}

bool MLAssist::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == ui.widget_title)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			if (((QMouseEvent *)event)->button() == Qt::LeftButton)
			{
				m_bmove = true;
				m_pressedpoint = ((QMouseEvent *)event)->globalPos(); //鼠标点击桌面位置
				m_windowspoint = this->pos();						  //界面所在桌面位置
				m_oldmousepoint = m_pressedpoint - m_windowspoint;	  //鼠标点击位置 在界面起始点的比较
			}
		}
		else if (event->type() == QEvent::MouseMove)
		{
			if ((((QMouseEvent *)event)->buttons() == Qt::LeftButton) && m_bmove)
			{
				move(((QMouseEvent *)event)->globalPos() - m_oldmousepoint); //move移动的是界面坐标 新的鼠标位置减去以前鼠标在界面的位置差 就是要移动的目标点 移动的是界面的起始坐标
			}
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			m_bmove = false;
		}
	}
	return QWidget::eventFilter(obj, event);
}
void MLAssist::OnNotifyLoginProgressStart()
{
	m_bIsInLoginProgress = true;
}

void MLAssist::OnNotifyLoginProgressEnd()
{
	m_bIsInLoginProgress = false;
}

void MLAssist::on_switch_ui_tab()
{
	ui.tabWidget->setCurrentIndex(1);
}
