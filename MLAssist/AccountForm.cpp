#include "accountform.h"
#include "GameCtrl.h"
#include "MINT.h"
#include "YunLai.h"
#include "ui_accountform.h"
#include <windows.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListView>
#include <QMenu>
#include <QTimer>
extern CGA::CGAInterface *g_CGAInterface;

bool IsProcessThreadPresent(quint32 ProcessId, quint32 ThreadId)
{
	bool bPresent = false;
	HANDLE ThreadHandle = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, ThreadId);
	if (ThreadHandle)
	{
		THREAD_BASIC_INFORMATION tbi;
		if (STATUS_SUCCESS == NtQueryInformationThread(ThreadHandle, ThreadBasicInformation, &tbi, sizeof(tbi), NULL))
		{
			if (tbi.ClientId.UniqueProcess == (HANDLE)ProcessId && tbi.ExitStatus == STATUS_PENDING)
			{
				bPresent = true;
			}
		}
		CloseHandle(ThreadHandle);
	}
	return bPresent;
}

typedef struct
{
	uint32_t cga_pid;
	uint32_t polcn_pid;
	char gid[128];
} polcn_view_t;

typedef struct
{
	char glt[128];
} glt_view_t;

AccountForm::AccountForm(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::AccountForm)
{
	ui->setupUi(this);

	m_polcn_lock = NULL;
	m_polcn_map = NULL;
	m_attachExistGameWndTime = QTime::currentTime().addSecs(-15);
	m_glt_lock = NULL;
	m_glt_map = NULL;
	m_loginInterval = g_pGameCtrl->GetAutoLoginInterval();
	m_POLCN = new QProcess();
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	m_POLCN->setProcessEnvironment(env);
	QMenu *saveOption = new QMenu;
	saveOption->addAction("保存登录", this, SLOT(OnSaveLoginBat()));
	saveOption->addAction("保存登录和配置", this, SLOT(OnSaveLoginConfigBat()));
	saveOption->addAction("保存登录配置和脚本(Lua)", this, SLOT(OnSaveLoginScriptCfgBat()));
	saveOption->addAction("保存登录配置和脚本(Js)", []()
			{ emit g_pGameCtrl->signal_saveLoginBat(3); });
	ui->pushButton_saveBat->setMenu(saveOption);
	connect(g_pGameCtrl, &GameCtrl::NotifyConnectionState, this, &AccountForm::OnNotifyConnectionState, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyAttachProcessOk, this, &AccountForm::OnNotifyAttachProcessOk, Qt::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyFillAutoLogin, this, &AccountForm::OnNotifyFillAutoLogin);
	connect(g_pGameCtrl, &GameCtrl::signal_createCharaData, this, &AccountForm::OnSetCreateCharaData);
	connect(g_pGameCtrl, &GameCtrl::signal_swicthAccountGid, this, &AccountForm::OnSwitchAccountGid);
	connect(g_pGameCtrl, &GameCtrl::signal_swicthCharacter, this, &AccountForm::OnSwitchCharacter);
	connect(g_pGameCtrl, &GameCtrl::signal_switchLoginData, this, &AccountForm::OnSwitchLoginData);
	connect(g_pGameCtrl, &GameCtrl::signal_loginGame, this, &AccountForm::on_pushButton_logingame_clicked);
	connect(g_pGameCtrl, &GameCtrl::signal_runGameWnd, this, &AccountForm::on_pushButton_getgid_clicked);
	connect(g_pGameCtrl, &GameCtrl::signal_gameWndList, this, &AccountForm::OnRecordGameWndProcess);
	connect(g_pGameCtrl, &GameCtrl::HttpLoadAccount, this, &AccountForm::OnHttpLoadAccount);
	//connect(g_pGameCtrl, &GameCtrl::NotifyFillLoadScript, this, &AccountForm::OnNotifyFillLoadScript);
	//connect(g_pGameCtrl, &GameCtrl::NotifyFillLoadSettings, this, &AccountForm::OnNotifyFillLoadSettings);
	//connect(g_pGameCtrl, &GameCtrl::NotifyFillStaticSettings, this, &AccountForm::OnNotifyFillStaticSettings);

	connect(this, SIGNAL(FetchPlayerData()), this, SLOT(OnFetchPlayerData()));
	connect(g_pGameFun, SIGNAL(signal_updateFetchGid()), this, SLOT(OnFetchPlayerData()));
	connect(m_POLCN, SIGNAL(started()), this, SLOT(OnPOLCNStarted()));
	connect(m_POLCN, SIGNAL(readyReadStandardOutput()), this, SLOT(OnPOLCNReadyReadStdOut()));
	connect(m_POLCN, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnPOLCNFinish(int, QProcess::ExitStatus)));
	connect(g_pGameFun, &CGFunction::signal_loginGameServerEx, this, &AccountForm::OnLoginGameServerEx);
	ui->comboBox_gameType->addItem(QObject::tr("道具 (电信)"), QVariant(4));
	ui->comboBox_gameType->addItem(QObject::tr("道具 (网通)"), QVariant(40));
	ui->comboBox_gameType->addItem(QObject::tr("cg_se_6000 (时长)"), QVariant(1));
	ui->comboBox_gameType->addItem(QObject::tr("cg_se_3000 (怀旧)"), QVariant(11));

	ui->comboBox_bigserver->addItem(QObject::tr("牧羊"), QVariant(0));
	ui->comboBox_bigserver->addItem(QObject::tr("金牛"), QVariant(2));

	for (int i = 0; i < 10; ++i)
		ui->comboBox_server->addItem(QObject::tr("%1线").arg(i + 1), QVariant(i));

	ui->comboBox_character->addItem(QObject::tr("左边人物"), QVariant(0));
	ui->comboBox_character->addItem(QObject::tr("右边人物"), QVariant(1));
	ui->comboBox_character->addItem(QObject::tr("最高等级"), QVariant(2));
	ui->comboBox_character->addItem(QObject::tr("最低等级"), QVariant(3));

	ui->comboBox_CharaEye->addItem(QObject::tr("1"), QVariant(0));
	ui->comboBox_CharaEye->addItem(QObject::tr("2"), QVariant(1));
	ui->comboBox_CharaEye->addItem(QObject::tr("3"), QVariant(2));
	ui->comboBox_CharaEye->addItem(QObject::tr("4"), QVariant(3));
	ui->comboBox_CharaEye->addItem(QObject::tr("5"), QVariant(4));

	ui->comboBox_CharaMou->addItem(QObject::tr("1"), QVariant(0));
	ui->comboBox_CharaMou->addItem(QObject::tr("2"), QVariant(1));
	ui->comboBox_CharaMou->addItem(QObject::tr("3"), QVariant(2));
	ui->comboBox_CharaMou->addItem(QObject::tr("4"), QVariant(3));
	ui->comboBox_CharaMou->addItem(QObject::tr("5"), QVariant(4));

	ui->comboBox_CharaColor->addItem(QObject::tr("1"), QVariant(0));
	ui->comboBox_CharaColor->addItem(QObject::tr("2"), QVariant(1));
	ui->comboBox_CharaColor->addItem(QObject::tr("3"), QVariant(2));
	ui->comboBox_CharaColor->addItem(QObject::tr("4"), QVariant(3));
	QListView *pListView = new QListView();
	pListView->setIconSize(QSize(20, 48));
	ui->comboBox_Chara->setView(pListView);
	QString sPath = QCoreApplication::applicationDirPath() + "//db//人物//";
	QStringList roleName;
	roleName << "巴乌"
			 << "卡兹"
			 << "辛"
			 << "托布"
			 << "凯"
			 << "菲特"
			 << "伯克"
			 << "乌噜"
			 << "萌子"
			 << "阿咪"
			 << "梅古"
			 << "丽"
			 << "卡伊"
			 << "艾露"
			 << "谢堤"
			 << "彼特"
			 << "左藏"
			 << "尼尔森"
			 << "贝堤特"
			 << "兰斯洛特"
			 << "威斯凯尔"
			 << "莎拉"
			 << "绫女"
			 << "福尔蒂雅"
			 << "夏菈"
			 << "萍萍"
			 << "葛蕾丝"
			 << "荷蜜";
	int baseNum = 100000;
	int midIndex = 0;

	for (int i = 0; i < roleName.size(); ++i)
	{
		if (i < 7)
		{
			m_roleMapSex.insert(i, 0);
			ui->comboBox_Chara->addItem(QIcon(sPath + QString::number(100000 + i * 25) + ".gif"), roleName[i], i);
		}
		else if (i >= 7 && i < 14)
		{
			m_roleMapSex.insert(i, 1);
			ui->comboBox_Chara->addItem(QIcon(sPath + QString::number(100250 + (i - 7) * 25) + ".gif"), roleName[i], i);
		}
		else if (i >= 14 && i < 21)
		{
			m_roleMapSex.insert(i, 0);
			ui->comboBox_Chara->addItem(QIcon(sPath + QString::number(106000 + (i - 14) * 25) + ".gif"), roleName[i], i);
		}
		else if (i >= 21)
		{
			m_roleMapSex.insert(i, 1);
			ui->comboBox_Chara->addItem(QIcon(sPath + QString::number(106250 + (i - 21) * 25) + ".gif"), roleName[i], i);
		}
	}

	/*ui->comboBox_Chara->addItem(QIcon(""), QObject::tr("巴乌"), QVariant(0));
	ui->comboBox_Chara->addItem(QObject::tr("卡兹"), QVariant(1));
	ui->comboBox_Chara->addItem(QObject::tr("辛"), QVariant(2));
	ui->comboBox_Chara->addItem(QObject::tr("托布"), QVariant(3));
	ui->comboBox_Chara->addItem(QObject::tr("凯"), QVariant(4));
	ui->comboBox_Chara->addItem(QObject::tr("菲特"), QVariant(5));
	ui->comboBox_Chara->addItem(QObject::tr("伯克"), QVariant(6));
	ui->comboBox_Chara->addItem(QObject::tr("乌噜"), QVariant(7));
	ui->comboBox_Chara->addItem(QObject::tr("萌子"), QVariant(8));
	ui->comboBox_Chara->addItem(QObject::tr("阿咪"), QVariant(9));
	ui->comboBox_Chara->addItem(QObject::tr("梅古"), QVariant(10));
	ui->comboBox_Chara->addItem(QObject::tr("丽"), QVariant(11));
	ui->comboBox_Chara->addItem(QObject::tr("卡伊"), QVariant(12));
	ui->comboBox_Chara->addItem(QObject::tr("艾露"), QVariant(13));

	ui->comboBox_Chara->addItem(QObject::tr("谢堤"), QVariant(14 + 0));
	ui->comboBox_Chara->addItem(QObject::tr("彼特"), QVariant(14 + 1));
	ui->comboBox_Chara->addItem(QObject::tr("左藏"), QVariant(14 + 2));
	ui->comboBox_Chara->addItem(QObject::tr("尼尔森"), QVariant(14 + 3));
	ui->comboBox_Chara->addItem(QObject::tr("贝堤特"), QVariant(14 + 4));
	ui->comboBox_Chara->addItem(QObject::tr("兰斯洛特"), QVariant(14 + 5));
	ui->comboBox_Chara->addItem(QObject::tr("威斯凯尔"), QVariant(14 + 6));
	ui->comboBox_Chara->addItem(QObject::tr("莎拉"), QVariant(14 + 7));
	ui->comboBox_Chara->addItem(QObject::tr("绫女"), QVariant(14 + 8));
	ui->comboBox_Chara->addItem(QObject::tr("福尔蒂雅"), QVariant(14 + 9));
	ui->comboBox_Chara->addItem(QObject::tr("夏菈"), QVariant(14 + 10));
	ui->comboBox_Chara->addItem(QObject::tr("萍萍"), QVariant(14 + 11));
	ui->comboBox_Chara->addItem(QObject::tr("葛蕾丝"), QVariant(14 + 12));
	ui->comboBox_Chara->addItem(QObject::tr("荷蜜"), QVariant(14 + 13));*/

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(OnAutoLogin()));
	timer->start(1000);

	m_game_pid = 0;
	m_game_tid = 0;
	m_serverid = 0;
}

AccountForm::~AccountForm()
{
	//释放 防止卡登录
	if (m_polcn_lock)
	{
		CloseHandle(m_polcn_lock);
		m_polcn_lock = NULL;
	}
	if (m_polcn_map)
	{
		CloseHandle(m_polcn_map);
		m_polcn_map = NULL;
	}
	delete ui;
}
//账号登录超时判断，超时重新登录，未超时直接连接
bool AccountForm::IsGltExpired()
{
	QString glt;

	if (m_glt.isEmpty())
		return true;

	if (m_loginresult.elapsed() > 1000 * 60)
		return true;
	if (m_lastGameAccount != ui->lineEdit_account->text())
		return true;
	if (m_glt_map && m_glt_lock)
	{
		WaitForSingleObject(m_glt_lock, INFINITE);
		auto glt_view = MapViewOfFile(m_glt_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000);
		if (glt_view)
		{
			glt_view_t *view = (glt_view_t *)glt_view;

			glt = QString(view->glt);

			UnmapViewOfFile(glt_view);
		}
		ReleaseMutex(m_glt_lock);
	}

	if (0 != glt.compare(m_glt))
		return true;

	return false;
}
bool AccountForm::IsProcessAttached(quint32 ProcessId)
{
	bool bAttached = false;
	WCHAR szMutex[32];
	wsprintfW(szMutex, L"CGAAttachMutex_%d", ProcessId);
	HANDLE hAttachMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, szMutex);
	if (hAttachMutex)
	{
		bAttached = true;
		CloseHandle(hAttachMutex);
	}
	return bAttached;
}

bool AccountForm::QueryAttachGameWnd()
{
	QString sGid = ui->comboBox_gid->currentText();
	if (sGid.isEmpty())
	{
		//	qDebug() << sGid << "游戏id为空";
		return false;
	}
	//	qDebug() << "当前游戏ID" << sGid;
	if (m_gameWndList.size() < 1)
	{
		const wchar_t szFindGameClass[] = { 39764, 21147, 23453, 36125, 0 };
		HWND hWnd = NULL;
		DWORD pid, tid;
		WCHAR szText[256];
		while ((hWnd = FindWindowExW(NULL, hWnd, szFindGameClass, NULL)) != NULL)
		{
			if ((tid = GetWindowThreadProcessId(hWnd, (LPDWORD)&pid)) != 0 && pid != GetCurrentProcessId())
			{
				if (GetWindowTextW(hWnd, szText, 256))
				{
					auto wndTitle = QString::fromWCharArray(szText);
					bool attached = IsProcessAttached(pid);
					//				qDebug() << wndTitle << "附加" << attached;
					//if (!attached) //已经附加过 则退出当前
					{
						//登录成功后 读取用户账号
						void *pBaseAddr = YunLai::GetProcessImageBase1(pid);
						//	qDebug() << pBaseAddr;
						QString sUserName = QString(" [%1] ").arg(QString::fromWCharArray(YunLai::ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xE12D30, 17))));
						wndTitle += sUserName;
						QString szLoginUserID = YunLai::ReadMemoryStrFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xBDB488, 100); //游戏id
						//这里没有附加才进行附加辅助 已经附加辅助的，直接返回
						//					qDebug() << "Read账号" <<szLoginUserID;
						if (szLoginUserID == sGid)
						{
							if (!attached) //没有附加  进行附加
							{
								//					qDebug() << sGid << "找到指定id 并且空闲中";
								void *pBaseAddr = YunLai::GetProcessImageBase1(pid);
								//int gameStatus = YunLai::ReadMemoryIntFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xE1DFFC);	//游戏状态
								//int worldStatus = YunLai::ReadMemoryIntFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xE1E000); //游戏状态
								//					qDebug() << "Find状态:world:" << worldStatus << "game:" << gameStatus;
								//	if (gameStatus == 3 && worldStatus == 9)													//状态值不对的 可以kill掉													 //游戏状态正常
								{
									//						qDebug() << "游戏状态正常，开始附加窗口";
									emit g_pGameCtrl->NotifyAutoAttachProcess(pid, tid);
									return true;
								}
								break;
							}
							else
							{
								auto attachPid = g_pGameCtrl->getGamePID();
								if (pid != 0 && pid != attachPid)
								{
									if (g_pGameCtrl->GetStartGameRepeatedGidExit())
									{
										qDebug() << "该ID已附加，退出！";
										qApp->exit(0);
										return true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		auto wndList = m_gameWndList;
		for (auto wnd : wndList)
		{
			//		qDebug() << wnd->m_sLoginUserID << "附加" << wnd->m_bAttached ;
			//这里没有附加才进行附加辅助 已经附加辅助的，直接返回
			if (wnd->m_sLoginUserID == sGid)
			{
				if (wnd->m_bAttached == false)
				{
					void *pBaseAddr = YunLai::GetProcessImageBase1(wnd->m_ProcessId);
					//int gameStatus = YunLai::ReadMemoryIntFromProcessID(wnd->m_ProcessId, (ULONG_PTR)pBaseAddr + 0xE1DFFC);	 //游戏状态
					//int worldStatus = YunLai::ReadMemoryIntFromProcessID(wnd->m_ProcessId, (ULONG_PTR)pBaseAddr + 0xE1E000); //游戏状态
					//																										 //			qDebug() << "状态:world:" << worldStatus << "game:"<< gameStatus;
					//if (gameStatus == 3 && worldStatus == 9)																 //游戏状态正常
					{
						//		qDebug() << "游戏状态正常，开始附加窗口";
						emit g_pGameCtrl->NotifyAutoAttachProcess(wnd->m_ProcessId, wnd->m_ThreadId);
						return true;
					}
					break;
				}
				else
				{

					auto attachPid = g_pGameCtrl->getGamePID();
					if (wnd->m_ProcessId != 0 && wnd->m_ProcessId != attachPid)
					{
						if (g_pGameCtrl->GetStartGameRepeatedGidExit())
						{
							qDebug() << "该ID已附加，退出！";
							qApp->exit(0);
							return true;
						}
					}
				}
			}
		}
	}
	//qDebug() << "没有找到指定游戏id窗口";
	return false;
}

void AccountForm::OnAutoLogin()
{
	//运行易玩通登录超时判断 超过15秒 如果还是运行中 则杀掉进程 返回，如果不是运行中进入下一步
	if (m_loginquery.elapsed() > 15 * 1000)
	{
		if (m_POLCN->state() == QProcess::ProcessState::Running)
		{
			m_POLCN->kill();
			return;
		}
	}

	if (!ui->groupBox_createChara->isChecked()) //不创建人物
	{
		CGA::cga_create_chara_t req;
		g_CGAInterface->CreateCharacter(req); //通知不创建人物
	}
	//不自动登录 直接返回
	if (!ui->checkBox_autoLogin->isChecked())
	{
		if (g_CGAInterface->IsConnected())
		{
			g_CGAInterface->LoginGameServer("", "", 0, 0, 0, 0); //通知远端 不自动登录
		}

		ui->label_status->setText(tr("自动登录功能已关闭"));

		if (m_polcn_lock)
		{
			CloseHandle(m_polcn_lock);
			m_polcn_lock = NULL;
		}
		if (m_polcn_map)
		{
			CloseHandle(m_polcn_map);
			m_polcn_map = NULL;
		}
		emit g_pGameCtrl->NotifyLoginProgressEnd();
		return;
	}
	//需要自动登录 先获取账号 再挂接游戏  最后登录游戏
	if (g_CGAInterface->IsConnected()) //如果已经连接
	{
		m_bFindNormalWnd = false;
		int ingame = 0;
		if (g_CGAInterface->IsInGame(ingame) && ingame == 1)
		{
			m_lastGameWndConnTime.restart();
			ui->textEdit_output->setText(tr("游戏登录成功.\n"));
			ui->label_status->setText(tr("游戏登录成功."));
			//qDebug() << "AutoLogin 游戏登录成功！";

			if (m_polcn_lock)
			{
				CloseHandle(m_polcn_lock);
				m_polcn_lock = NULL;
			}
			if (m_polcn_map)
			{
				CloseHandle(m_polcn_map);
				m_polcn_map = NULL;
			}
			//运行抓取判断
			if (m_bStatistics)
			{
				emit FetchPlayerData();
			}
			emit g_pGameCtrl->NotifyLoginProgressEnd();
			return;
		}
		//没有进入游戏 则进行登录
		int gameStatus = 0;
		int worldStatus = 0;
		if (g_CGAInterface->GetGameStatus(gameStatus) && g_CGAInterface->GetWorldStatus(worldStatus))
		{
			if ((worldStatus == 2 && gameStatus == 1) || (worldStatus == 3 && gameStatus == 11))
			{
				qDebug() << "AutoLogin 状态！" << worldStatus << " " << gameStatus;

				if (IsGltExpired())
				{
					if (!m_bFirstGltExpired)
					{
						m_bFirstGltExpired = true;
						m_gltExpiredTime.restart(); //记录第一次
					}
					if (m_bFirstGltExpired)
					{
						//10分钟没获取成功 杀掉
						if (m_gltExpiredTime.elapsed() > 10 * 60 * 1000)
						{
							qDebug() << "AutoLogin 10分钟持续登录过期，重新启动游戏！";
							emit g_pGameCtrl->NotifyKillProcess();
							return;
						}
					}
					qDebug() << "AutoLogin 登录过期！";
					on_pushButton_getgid_clicked();
					return;
				}
				else
				{
					if (m_lastGameWndConnTime.elapsed() > 30 * 10000) //300秒
					{
						qDebug() << "300秒登录失败，干掉游戏窗口，重新加载！";
						emit g_pGameCtrl->NotifyKillProcess();
					}
					//增加等待
					if (m_loginInterval > 0)
					{
						QEventLoop loop;
						QTimer::singleShot(m_loginInterval, &loop, SLOT(quit()));
						loop.exec();
					}
					on_pushButton_logingame_clicked();
					return;
				}
			}
		}
	}
	else if (!m_game_pid || !IsProcessThreadPresent(m_game_pid, m_game_tid))
	{
		m_lastGameWndConnTime.restart();
		//附加失败 以及游戏尚未启动 走这个

		////先屏蔽下面重复附加判断代码;
		//上次找到指定窗口 并且时间在5秒内  则不进行重新附加
		if (m_bFindNormalWnd)
		{
			//			qDebug() << "附加成功 15秒内 返回";
			return;
		}
		if (m_attachExistGameWndTime.elapsed() < 15 * 1000)
			return;
		//上次找到指定窗口 或没找到  时间也超过15秒 则重新进行操作
		//增加附加已有游戏窗口功能
		if (QueryAttachGameWnd()) //等待附加成功
		{
			qDebug() << "存在空闲窗口，进行附加！";
			m_attachExistGameWndTime.start();
			m_bFindNormalWnd = true;
		}
		else
		{
			qDebug() << "AutoLogin 获取通行证游戏ID！";
			on_pushButton_getgid_clicked();
		}
	}
}

void AccountForm::OnPOLCNReadyReadStdOut()
{
	m_StdOut += m_POLCN->readAllStandardOutput();
}

void AccountForm::OnPOLCNStarted()
{
	ui->label_status->setText(tr("运行易玩通登录器，登录进程 %1...").arg(m_POLCN->processId()));
	ui->textEdit_output->setText(tr("正在请求子账号列表..."));
	emit g_pGameCtrl->NotifyLoginProgressStart();
}

void AccountForm::OnPOLCNFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
	m_StdOut += m_POLCN->readAllStandardOutput();

	QJsonParseError err;
	auto doc = QJsonDocument::fromJson(m_StdOut, &err);
	if (err.error == QJsonParseError::NoError && doc.isObject())
	{
		QJsonObject obj = doc.object();
		if (obj.contains("result"))
		{
			auto result = obj.take("result");
			if (result == 0)
			{
				auto lastgid = ui->comboBox_gid->currentText();
				ui->comboBox_gid->clear();
				auto gid_array = obj.take("gid").toArray();
				QStringList gidList;
				for (auto i = 0; i < gid_array.count(); ++i)
				{
					auto gid_object = gid_array[i].toObject();
					auto gidName = gid_object.take("name").toString();
					ui->comboBox_gid->addItem(gidName);
					gidList.append(gidName);
					if (m_bStatistics && !m_gidDatas.contains(gidName))
					{
						auto fetchList = m_gidDatas.value(gidName);

						GameGoodsFetchPtr tmpFetch(new GameGoodsFetch);
						tmpFetch->sGID = gidName;
						tmpFetch->character = 0; //左边
						GameGoodsFetchPtr tmpFetch2(new GameGoodsFetch);
						tmpFetch2->sGID = gidName;
						tmpFetch2->character = 1; //右边
						fetchList.append(tmpFetch);
						fetchList.append(tmpFetch2);
						m_pFetchLoginList.append(fetchList);

						m_gidDatas.insert(gidName, fetchList);
					}
				}
				g_pGameFun->SetAccountGIDs(gidList);
				if (!lastgid.isEmpty())
				{
					for (auto i = 0; i < ui->comboBox_gid->count(); ++i)
					{
						if (lastgid == ui->comboBox_gid->itemText(i))
							ui->comboBox_gid->setCurrentIndex(i);
					}
				}

				m_serverid = obj.take("serverid").toInt();
				m_glt = obj.take("glt").toString();
				m_loginresult.start();
				m_bFirstGltExpired = false; //登录成功
				if (!m_glt_lock)
				{
					m_glt_lock = CreateMutexW(NULL, FALSE, L"CGAGltLock");
				}

				if (!m_glt_map)
				{
					m_glt_map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"CGAGltMap");
				}

				if (m_glt_map && m_glt_lock)
				{
					WaitForSingleObject(m_glt_lock, INFINITE);
					auto glt_view = MapViewOfFile(m_glt_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000);
					if (glt_view)
					{
						glt_view_t *view = (glt_view_t *)glt_view;

						memset(view->glt, 0, sizeof(view->glt));

						strncpy(view->glt, m_glt.toLocal8Bit().data(), 127);
						view->glt[127] = 0;

						UnmapViewOfFile(glt_view);
					}
					ReleaseMutex(m_glt_lock);
				}

				ui->textEdit_output->setText(tr("获取子账号成功.\n"));
				ui->textEdit_output->append(m_StdOut);

				if (obj.find("game_pid") != obj.end())
				{
					m_game_pid = (quint32)obj.take("game_pid").toInt();
					m_game_tid = (quint32)obj.take("game_tid").toInt();
					emit g_pGameCtrl->NotifyAutoAttachProcess(m_game_pid, m_game_tid);

					ui->textEdit_output->append(tr("等待游戏进程登录中...\n"));
				}

				if (ui->checkBox_autoLogin->isChecked())
				{
					on_pushButton_logingame_clicked();
				}
			}
			else
			{
				ui->textEdit_output->setText(tr("登录被服务器拒绝。\n"));
				ui->textEdit_output->append(m_StdOut);
			}
		}
		else
		{
			ui->textEdit_output->setText(tr("解析登录器返回结果失败，缺少result字段。\n"));
			ui->textEdit_output->append(m_StdOut);
		}
	}
	else
	{
		ui->textEdit_output->setText(tr("解析登录器返回结果失败。\n"));
		ui->textEdit_output->append(m_StdOut);
	}

	ui->label_status->setText(tr("登录器进程结束，退出代码 %1").arg(exitCode));
}

bool AccountForm::QueryAccount(QString &label, QString &errorMsg)
{
	if (m_POLCN->state() == QProcess::ProcessState::Running)
	{
		//qDebug("running");
		return false;
	}

	if (!m_polcn_lock)
	{
		m_polcn_lock = CreateMutexW(NULL, TRUE, L"CGAPolcnLock");
		if (m_polcn_lock && GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// qDebug("lock1");
			auto polcn_map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"CGAPPolcnMap");
			if (polcn_map)
			{
				auto polcn_view = MapViewOfFile(polcn_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000);
				if (polcn_view)
				{
					polcn_view_t *view = (polcn_view_t *)polcn_view;

					label = tr("正在等待其他CGA进程登录...");
					errorMsg = tr("正在等待其他CGA进程登录...\nMLAssist_PID=#%1, POLCN_PID=#%2, GID=%3")
									   .arg(view->cga_pid)
									   .arg(view->polcn_pid)
									   .arg(QString(view->gid));

					UnmapViewOfFile(polcn_view);
				}
				else
				{
					label = tr("正在等待其他CGA进程释放登录互斥锁...");
					errorMsg = tr("正在等待其他CGA进程释放登录互斥锁...");
				}
				CloseHandle(polcn_map);
			}
			else
			{
				label = tr("正在等待其他CGA进程释放登录互斥锁...");
				errorMsg = tr("正在等待其他CGA进程释放登录互斥锁...");
			}
			CloseHandle(m_polcn_lock);
			m_polcn_lock = NULL;
			return false;
		}
		else if (!m_polcn_lock)
		{
			//qDebug("lock2");

			label = tr("获取登录互斥锁失败...");
			errorMsg = tr("获取登录互斥锁失败，错误码 %1...").arg(GetLastError());
			return false;
		}
	}

	//qDebug("query");

	m_loginquery = QTime::currentTime();
	m_lastGameAccount = ui->lineEdit_account->text();
	m_StdOut.clear();

	QString argstr = QString("-account %1 -pwd %2 -gametype %3 -rungame %4 -skipupdate %5")
							 .arg(ui->lineEdit_account->text())
							 .arg(ui->lineEdit_password->text())
							 .arg(ui->comboBox_gameType->currentData().toInt())
							 .arg(((IsProcessThreadPresent(m_game_pid, m_game_tid) || g_CGAInterface->IsConnected())) ? "0" : "1")
							 .arg(ui->checkBox_disableUpdater->isChecked() ? "1" : "0");

	m_POLCN->setNativeArguments(argstr);
	m_POLCN->start("POLCN_Launcher.exe");

	if (!m_polcn_map)
	{
		m_polcn_map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"CGAPPolcnMap");
	}

	if (m_polcn_map)
	{
		auto polcn_view = MapViewOfFile(m_polcn_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000);
		if (polcn_view)
		{
			polcn_view_t *view = (polcn_view_t *)polcn_view;

			view->cga_pid = (uint32_t)QApplication::applicationPid();
			view->polcn_pid = (uint32_t)m_POLCN->processId();

			memset(view->gid, 0, sizeof(view->gid));

			strncpy(view->gid, ui->comboBox_gid->currentText().toLocal8Bit().data(), 127);
			view->gid[127] = 0;

			UnmapViewOfFile(polcn_view);
		}
	}

	return true;
}

void AccountForm::on_pushButton_getgid_clicked()
{
	QString label, errorMsg;
	QueryAccount(label, errorMsg);

	if (!label.isEmpty())
		ui->label_status->setText(label);

	if (!errorMsg.isEmpty())
		ui->textEdit_output->setText(errorMsg);
}

void AccountForm::on_pushButton_logingame_clicked()
{
	if (g_CGAInterface->IsConnected())
	{
		if (ui->groupBox_createChara->isChecked())
		{
			CGA::cga_create_chara_t req;
			int gameStatus = g_pGameFun->GetGameStatus();
			int worldStatus = g_pGameFun->GetWorldStatus();
			if (gameStatus == 101 && worldStatus == 4 && ui->radioButton_randomName->isChecked())
			{
				QString sName = g_pGameFun->CreateRandomName(m_roleMapSex.value(ui->comboBox_Chara->currentData().toInt()));
				ui->lineEdit_CharaName->setText(sName);
			}
			req.name = ui->lineEdit_CharaName->text().toStdString();
			req.character = ui->comboBox_Chara->currentData().toInt();
			req.eye = ui->comboBox_CharaEye->currentData().toInt();
			req.mouth = ui->comboBox_CharaMou->currentData().toInt();
			req.color = ui->comboBox_CharaColor->currentData().toInt();

			QStringList points = ui->lineEdit_CharaPoints->text().split(",");

			if (points.size() == 5)
			{
				req.endurance = points[0].toInt();
				req.strength = points[1].toInt();
				req.defense = points[2].toInt();
				req.agility = points[3].toInt();
				req.magical = points[4].toInt();
			}

			QStringList elements = ui->lineEdit_CharaElements->text().split(",");

			if (elements.size() == 4)
			{
				req.earth = elements[0].toInt();
				req.water = elements[1].toInt();
				req.fire = elements[2].toInt();
				req.wind = elements[3].toInt();
			}

			g_CGAInterface->CreateCharacter(req);
		}
		else
		{
			CGA::cga_create_chara_t req;
			g_CGAInterface->CreateCharacter(req);
		}

		IsFetchGameData();
		g_CGAInterface->LoginGameServer(ui->comboBox_gid->currentText().toStdString(),
				m_glt.toStdString(),
				m_serverid,
				ui->comboBox_bigserver->currentData().toInt(),
				ui->comboBox_server->currentData().toInt(),
				ui->comboBox_character->currentData().toInt());
	}
}
//连接状态 1 2登录成功 10000（NewUI_ShowLostConnectionDialog） 0（貌似断线？） 登录失败 3没有人物
void AccountForm::OnNotifyConnectionState(int state, QString msg)
{
	qDebug() << state << msg;
	if ((state == 10000 || state == 0) && !msg.isEmpty())
	{ // 0 "断线后在限定时间内不能登录！"    0 "此服务器很拥挤。"  0分多种情况
		if (m_bStatistics)
		{
			if (msg.contains("角色数据读取失败")) //这个id跳过
			{
				QString curGID = ui->comboBox_gid->currentText();

				auto pGidList = m_gidDatas.value(curGID);
				for (auto pFetchGid : pGidList)
				{
					if (pFetchGid)
					{
						pFetchGid->bFetched = true;
						pFetchGid->erMsg = "角色数据读取失败，其他窗口已登录！";
						qDebug() << "角色数据读取失败" << pFetchGid->sGID;
					}
				}
				////重新登录 下次判断会跳过这个
				//on_pushButton_logingame_clicked();
			}
		}
		if (msg.contains("非法登录或登录已超时"))
		{
			on_pushButton_getgid_clicked();
		}
		m_login_failure++;
		if (m_login_failure > 10)
		{
			qDebug() << "登录失败次数超10次，干掉游戏窗口，重新加载！";
			emit g_pGameCtrl->NotifyKillProcess();
			m_login_failure = 0;
			//YunLai::KillProcess(g_pGameCtrl->getGameHwnd());
			//10次以后，再进行换线操作，否则太频繁
			if (ui->checkBox_autoChangeServer->isChecked() && ui->checkBox_autoLogin->isChecked())
			{
				if (ui->comboBox_server->currentIndex() + 1 >= 10)
					ui->comboBox_server->setCurrentIndex(0);
				else
					ui->comboBox_server->setCurrentIndex(ui->comboBox_server->currentIndex() + 1);
			}
		}
	}
	else if (state == 1 || state == 2)
	{
		m_login_failure = 0;
	}
	else if (state == 3)
	{
		qDebug() << "没有游戏角色！";
		//登录游戏判断 是否有角色，没有的话，进行切换，还要测试下没有角色时，返回到服务器接口是否好使
		if (m_bStatistics)
		{
			//切换数据
			//0左边 1右边
			auto pFetchGid = GetCurrentLoginGid();
			if (pFetchGid)
			{
				pFetchGid->bFetched = true;
				pFetchGid->erMsg = "没有游戏角色";
				qDebug() << "没有游戏角色" << pFetchGid->sGID;
			}
			//没有游戏角色 回上页按钮没找到 暂时只能抓有数据的游戏
			g_CGAInterface->BackSelectServer();
			//g_CGAInterface->LogOut();
			//POINT points;
			//points.x = 550;
			//points.y = 450;
			////ClientToScreen(m_gameHwnd, &points);
			//YunLai::SetFocusToWnd(g_pGameCtrl->getGameHwnd()); //已经是焦点了 不需要
			//LPARAM newl = MAKELPARAM(points.x, points.y);
			//auto gameHwd = g_pGameCtrl->getGameHwnd();
			//SendMessage(gameHwd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
			//SendMessage(gameHwd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
			//SendMessage(gameHwd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
			return;
			////重新登录 下次判断会跳过这个
			//on_pushButton_logingame_clicked();
		}
		//还不知道创建人物时候 名字重复报什么错
		if (ui->radioButton_randomName->isChecked() /*&& ui->lineEdit_CharaName->text().isEmpty()*/)
		{
			//随机获取名称
			QString sName = g_pGameFun->CreateRandomName(m_roleMapSex.value(ui->comboBox_Chara->currentData().toInt()));
			ui->lineEdit_CharaName->setText(sName);
		}
		else if (ui->radioButton_nameIncrease->isChecked() && !ui->lineEdit_CharaName->text().isEmpty())
		{
			QString sName = ui->lineEdit_CharaName->text();
			QString srcName;
			QString nameNum;
			for (int i = (sName.size() - 1); i >= 0; i--)
			{
				if (!sName.at(i).isDigit())
				{
					srcName = sName.mid(0, i + 1);
					nameNum = sName.mid(i + 1);
					break;
				}
			}

			int endNum = 1;
			if (!nameNum.isEmpty())
				endNum = nameNum.toInt();

			ui->lineEdit_CharaName->setText(QString("%1%2").arg(srcName).arg(endNum));
		}
	}
}
//切换游戏账号
bool AccountForm::IsFetchGameData()
{
	if (m_bStatistics)
	{
		//切换数据
		//0左边 1右边
		bool bFetchFini = true;
		for (auto pFetchGid : m_pFetchLoginList)
		{
			if (pFetchGid->bFetched == false)
			{ //登录

				int nIndex = ui->comboBox_gid->findText(pFetchGid->sGID);
				if (nIndex < 0)
				{
					ResetFetchStatus();
					return false;
				}
				ui->comboBox_gid->setCurrentIndex(nIndex);
				ui->comboBox_character->setCurrentIndex(pFetchGid->character);
				bFetchFini = false;
				return true;
			}
		}
		//仓库获取完成 终止登出和脚本
		if (m_pFetchLoginList.size() > 0 && bFetchFini)
		{
			g_pGameCtrl->signal_stopScriptRun();
		}
	}
	return false;
}

void AccountForm::ResetFetchStatus()
{
	m_bStatistics = false;
	m_bStatisticsRunning = false;
	//m_gidDatas.clear();
}

GameGoodsFetchPtr AccountForm::GetCurrentLoginGid()
{
	if (m_bStatistics)
	{
		QString curGID = ui->comboBox_gid->currentText();
		int nCharacter = ui->comboBox_character->currentIndex();
		//切换数据
		//0左边 1右边
		for (auto pFetchGid : m_pFetchLoginList)
		{
			if (pFetchGid->sGID == curGID && pFetchGid->character == nCharacter)
			{
				return pFetchGid;
			}
		}
	}
	return nullptr;
}

void AccountForm::OnOpenAutoLogin()
{
	ui->checkBox_autoLogin->setChecked(true);
}
//
void AccountForm::OnLoginGameServerEx(QString account, QString pwd, QString gid /*= ""*/, int gametype /*= 4*/, int rungame /*= 1*/, int skipupdate /*= 1*/)
{
	if (gametype == 4)
		ui->comboBox_gameType->setCurrentIndex(0);
	if (gametype == 40)
		ui->comboBox_gameType->setCurrentIndex(1);
	if (gametype == 1)
		ui->comboBox_gameType->setCurrentIndex(2);
	if (gametype == 11)
		ui->comboBox_gameType->setCurrentIndex(3);

	if (!account.isEmpty())
		ui->lineEdit_account->setText(account);
	if (!pwd.isEmpty())
		ui->lineEdit_password->setText(pwd);
	if (!gid.isEmpty())
		ui->comboBox_gid->setEditText(gid);

	if (skipupdate)
		ui->checkBox_disableUpdater->setChecked(true);
}
//生成一键启动脚本
void AccountForm::on_pushButton_saveBat_clicked()
{
}

void AccountForm::OnSaveLoginBat()
{
	//QString sBat;
	//sBat = QString("start %1").arg(QCoreApplication::applicationDirPath() + "//MLAssist.exe");
	emit g_pGameCtrl->signal_saveLoginBat(0);
}

void AccountForm::OnSaveLoginConfigBat()
{
	emit g_pGameCtrl->signal_saveLoginBat(1);
}

void AccountForm::OnSaveLoginScriptCfgBat()
{
	emit g_pGameCtrl->signal_saveLoginBat(2);
}

QString AccountForm::GetLoginArgs()
{
	QString sLoginArg;
	sLoginArg = QString(" -gametype=%2 -loginuser=%3 -loginpwd=%4 -gid=%5"
						" -bigserver=%6 -server=%7 -character=%8 -autologin=%9"
						" -skipupdate=%10 -autochangeserver=%11 -autokillgame=%12 -autocreatechara=%13 -createcharachara=%14  -createcharaeye=%15 -createcharamouth=%16"
						" -createcharacolor=%17 -createcharapoints=%18 -createcharaelements=%19 -createcharaname=%20 ")
						.arg(ui->comboBox_gameType->currentData(Qt::UserRole).toInt())
						.arg(ui->lineEdit_account->text())
						.arg(ui->lineEdit_password->text())
						.arg(ui->comboBox_gid->currentText())
						.arg(ui->comboBox_bigserver->currentIndex() + 1)
						.arg(ui->comboBox_server->currentIndex() + 1)
						.arg(ui->comboBox_character->currentIndex() + 1)
						.arg(ui->checkBox_autoLogin->isChecked())
						.arg(ui->checkBox_disableUpdater->isChecked())
						.arg(ui->checkBox_autoChangeServer->isChecked())
						.arg(true) //killGame
						.arg(ui->groupBox_createChara->isChecked())
						.arg(ui->comboBox_Chara->currentIndex() + 1)
						.arg(ui->comboBox_CharaEye->currentIndex() + 1)
						.arg(ui->comboBox_CharaMou->currentIndex() + 1)
						.arg(ui->comboBox_CharaColor->currentIndex() + 1)
						.arg(ui->lineEdit_CharaPoints->text())
						.arg(ui->lineEdit_CharaElements->text())
						.arg(ui->lineEdit_CharaName->text());
	return sLoginArg;
}

void AccountForm::OnSwitchAccountGid(const QString &gid)
{
	int nIndex = ui->comboBox_gid->findText(gid);
	if (nIndex < 0)
	{
		ResetFetchStatus();
		return;
	}
	ui->comboBox_gid->setCurrentIndex(nIndex);
}

void AccountForm::OnSwitchCharacter(int index)
{
	ui->comboBox_character->setCurrentIndex(index);
}

void AccountForm::OnSwitchLoginData(QString gid /*= ""*/, int servr /*= 0*/, int line /*= 0*/, int character /*= 0*/)
{
	if (!gid.isEmpty())
		ui->comboBox_gid->setEditText(gid);

	if (servr >= 1 && servr <= 2)
		ui->comboBox_bigserver->setCurrentIndex(servr - 1);

	if (line >= 1 && line <= 10)
		ui->comboBox_server->setCurrentIndex(line - 1);

	if (character >= 1 && character <= 4)
		ui->comboBox_character->setCurrentIndex(character - 1);
}

void AccountForm::on_pushButton_logout_clicked()
{
	g_CGAInterface->LogOut();
}

void AccountForm::on_pushButton_logback_clicked()
{
	g_CGAInterface->LogBack();
}

void AccountForm::OnNotifyFillAutoLogin(int game, QString user, QString pwd, QString gid,
		int bigserver, int server, int character,
		bool autologin, bool skipupdate, bool autochangeserver, bool autokillgame,
		bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color,
		QString create_chara_points, QString create_chara_elements, QString create_chara_name)
{
	if (game == 4)
		ui->comboBox_gameType->setCurrentIndex(0);
	if (game == 40)
		ui->comboBox_gameType->setCurrentIndex(1);
	if (game == 1)
		ui->comboBox_gameType->setCurrentIndex(2);
	if (game == 11)
		ui->comboBox_gameType->setCurrentIndex(3);

	if (!user.isEmpty())
		ui->lineEdit_account->setText(user);
	if (!pwd.isEmpty())
		ui->lineEdit_password->setText(pwd);
	if (!gid.isEmpty())
		ui->comboBox_gid->setEditText(gid);

	if (bigserver >= 1 && bigserver <= 2)
		ui->comboBox_bigserver->setCurrentIndex(bigserver - 1);

	if (server >= 1 && server <= 10)
		ui->comboBox_server->setCurrentIndex(server - 1);

	if (character >= 1 && character <= 4)
		ui->comboBox_character->setCurrentIndex(character - 1);

	if (autologin)
		ui->checkBox_autoLogin->setChecked(true);

	if (skipupdate)
		ui->checkBox_disableUpdater->setChecked(true);

	if (autochangeserver)
		ui->checkBox_autoChangeServer->setChecked(true);
	/*if (autokillgame)
		ui->checkBox_autoKillGame->setChecked(true);*/
	if (create_chara)
	{
		ui->groupBox_createChara->setChecked(true);
		on_groupBox_createChara_clicked(1);
	}

	if (create_chara_chara)
		ui->comboBox_Chara->setCurrentIndex(create_chara_chara - 1);

	if (create_chara_eye)
		ui->comboBox_CharaEye->setCurrentIndex(create_chara_eye - 1);
	if (create_chara_mou)
		ui->comboBox_CharaMou->setCurrentIndex(create_chara_mou - 1);
	if (create_chara_color)
		ui->comboBox_CharaColor->setCurrentIndex(create_chara_color - 1);

	if (!create_chara_points.isEmpty())
		ui->lineEdit_CharaPoints->setText(create_chara_points);

	if (!create_chara_elements.isEmpty())
		ui->lineEdit_CharaElements->setText(create_chara_elements);

	if (!create_chara_name.isEmpty())
	{
		ui->lineEdit_CharaName->setText(create_chara_name);
	}
}

void AccountForm::OnSetCreateCharaData(bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color, QString create_chara_points, QString create_chara_elements, QString create_chara_name)
{
	if (create_chara)
	{
		ui->groupBox_createChara->setChecked(true);
		on_groupBox_createChara_clicked(1);
	}

	if (create_chara_chara)
		ui->comboBox_Chara->setCurrentIndex(create_chara_chara - 1);

	if (create_chara_eye)
		ui->comboBox_CharaEye->setCurrentIndex(create_chara_eye - 1);
	if (create_chara_mou)
		ui->comboBox_CharaMou->setCurrentIndex(create_chara_mou - 1);
	if (create_chara_color)
		ui->comboBox_CharaColor->setCurrentIndex(create_chara_color - 1);

	if (!create_chara_points.isEmpty())
		ui->lineEdit_CharaPoints->setText(create_chara_points);

	if (!create_chara_elements.isEmpty())
		ui->lineEdit_CharaElements->setText(create_chara_elements);

	if (!create_chara_name.isEmpty())
	{
		ui->lineEdit_CharaName->setText(create_chara_name);
	}
}

void AccountForm::OnHttpLoadAccount(QString query, QByteArray postdata, QJsonDocument *doc)
{
	QJsonObject obj;

	QJsonParseError err;
	auto newdoc = QJsonDocument::fromJson(postdata, &err);
	if (err.error == QJsonParseError::NoError && newdoc.isObject())
	{
		obj.insert("errcode", 0);
		auto newobj = newdoc.object();
		if (newobj.contains("user"))
		{
			auto quser = newobj.take("user");
			if (quser.isString())
			{
				auto user = quser.toString();
				if (!user.isEmpty())
				{
					ui->lineEdit_account->setText(user);
				}
			}
		}
		if (newobj.contains("pwd"))
		{
			auto qpwd = newobj.take("pwd");
			if (qpwd.isString())
			{
				auto pwd = qpwd.toString();
				if (!pwd.isEmpty())
				{
					ui->lineEdit_password->setText(pwd);
				}
			}
		}
		if (newobj.contains("gid"))
		{
			auto qgid = newobj.take("gid");
			if (qgid.isString())
			{
				auto gid = qgid.toString();
				if (!gid.isEmpty())
				{
					ui->comboBox_gid->setCurrentText(gid);
				}
			}
		}
		if (newobj.contains("game"))
		{
			auto qgame = newobj.take("game");

			int game = qgame.toInt();
			if (game == 4)
				ui->comboBox_gameType->setCurrentIndex(0);
			if (game == 40)
				ui->comboBox_gameType->setCurrentIndex(1);
			if (game == 1)
				ui->comboBox_gameType->setCurrentIndex(2);
			if (game == 11)
				ui->comboBox_gameType->setCurrentIndex(3);
		}
		if (newobj.contains("bigserver"))
		{
			auto qbigserver = newobj.take("bigserver");

			int bigserver = qbigserver.toInt();
			if (bigserver >= 1 && bigserver <= 2)
				ui->comboBox_bigserver->setCurrentIndex(bigserver - 1);
		}
		if (newobj.contains("server"))
		{
			auto qserver = newobj.take("server");

			int server = qserver.toInt();
			if (server >= 1 && server <= 10)
				ui->comboBox_server->setCurrentIndex(server - 1);
		}
		if (newobj.contains("character"))
		{
			auto qcharacter = newobj.take("character");

			int character = qcharacter.toInt();
			if (character >= 1 && character <= 2)
				ui->comboBox_character->setCurrentIndex(character - 1);
		}
		if (newobj.contains("autologin"))
		{
			auto qautologin = newobj.take("autologin");
			if (qautologin.isBool())
			{
				ui->checkBox_autoLogin->setChecked(qautologin.toBool());
			}
		}
		if (newobj.contains("skipupdate"))
		{
			auto qskipupdate = newobj.take("skipupdate");
			if (qskipupdate.isBool())
			{
				ui->checkBox_disableUpdater->setChecked(qskipupdate.toBool());
			}
		}
		if (newobj.contains("autochangeserver"))
		{
			auto qautochangeserver = newobj.take("autochangeserver");
			if (qautochangeserver.isBool())
			{
				ui->checkBox_autoChangeServer->setChecked(qautochangeserver.toBool());
			}
		}
		if (newobj.contains("autocreatechara"))
		{
			auto qautocreatechara = newobj.take("autocreatechara");
			if (qautocreatechara.isBool())
			{
				ui->groupBox_createChara->setChecked(qautocreatechara.toBool());
				on_groupBox_createChara_clicked(qautocreatechara.toBool() ? 1 : 0);
			}
		}
		if (newobj.contains("createcharachara"))
		{
			auto qcreatecharachara = newobj.take("createcharachara");

			int createcharachara = qcreatecharachara.toInt();
			if (createcharachara >= 1 && createcharachara <= 28)
				ui->comboBox_Chara->setCurrentIndex(createcharachara - 1);
		}
		if (newobj.contains("createcharaeye"))
		{
			auto qcreatecharaeye = newobj.take("createcharaeye");

			int createcharaeye = qcreatecharaeye.toInt();
			if (createcharaeye >= 1 && createcharaeye <= 5)
				ui->comboBox_CharaEye->setCurrentIndex(createcharaeye - 1);
		}
		if (newobj.contains("createcharamouth"))
		{
			auto qcreatecharamouth = newobj.take("createcharamouth");

			int createcharamouth = qcreatecharamouth.toInt();
			if (createcharamouth >= 1 && createcharamouth <= 5)
				ui->comboBox_CharaMou->setCurrentIndex(createcharamouth - 1);
		}
		if (newobj.contains("createcharacolor"))
		{
			auto qcreatecharacolor = newobj.take("createcharacolor");

			int createcharacolor = qcreatecharacolor.toInt();
			if (createcharacolor >= 1 && createcharacolor <= 5)
				ui->comboBox_CharaColor->setCurrentIndex(createcharacolor - 1);
		}
		if (newobj.contains("createcharapoints"))
		{
			auto qcreatecharapoints = newobj.take("createcharapoints");
			if (qcreatecharapoints.isString())
			{
				auto points = qcreatecharapoints.toString();
				if (!points.isEmpty())
				{
					ui->lineEdit_CharaPoints->setText(points);
				}
			}
		}
		if (newobj.contains("createcharaelements"))
		{
			auto qcreatecharaelements = newobj.take("createcharaelements");
			if (qcreatecharaelements.isString())
			{
				auto elements = qcreatecharaelements.toString();
				if (!elements.isEmpty())
				{
					ui->lineEdit_CharaElements->setText(elements);
				}
			}
		}
		if (newobj.contains("createcharaname"))
		{
			auto qcreatecharaname = newobj.take("createcharaname");
			if (qcreatecharaname.isString())
			{
				auto name = qcreatecharaname.toString();
				if (!name.isEmpty())
				{
					ui->lineEdit_CharaName->setText(name);
				}
			}
		}
	}
	else
	{
		obj.insert("errcode", 1);
		obj.insert("message", tr("json解析失败"));
	}
end:
	doc->setObject(obj);
}

void AccountForm::on_groupBox_createChara_clicked(bool bflag)
{
	if (bflag)
	{
		ui->comboBox_Chara->setEnabled(true);
		ui->comboBox_CharaEye->setEnabled(true);
		ui->comboBox_CharaMou->setEnabled(true);
		ui->comboBox_CharaColor->setEnabled(true);
		ui->lineEdit_CharaName->setEnabled(true);
		ui->lineEdit_CharaPoints->setEnabled(true);
		ui->lineEdit_CharaElements->setEnabled(true);
	}
	else
	{
		ui->comboBox_Chara->setEnabled(false);
		ui->comboBox_CharaEye->setEnabled(false);
		ui->comboBox_CharaMou->setEnabled(false);
		ui->comboBox_CharaColor->setEnabled(false);
		ui->lineEdit_CharaName->setEnabled(false);
		ui->lineEdit_CharaPoints->setEnabled(false);
		ui->lineEdit_CharaElements->setEnabled(false);
	}
}

void AccountForm::on_pushButton_Statistics_clicked()
{
	/*if (m_bStatistics)
		return;*/
	m_bStatistics = !m_bStatistics;
	//不用勾选自动登录？
	//1、打开窗口后，在账号界面，用户填写账号密码，点击自动获取账号物品信息按钮
	//2、自动获取所有子id
	//3、登录窗口，通过账号左边人物，右边人物，依次遍历，直至结束
	//4、每次登录成功游戏后，判断人物位置，暂定法兰和新城，运行脚本，去银行获取信息，然后把信息写到excel文件中
	g_pGameCtrl->signal_loadScript("脚本/仓库/游戏信息存储.lua");
	on_pushButton_getgid_clicked();
	//	m_bStatistics = false;
}

void AccountForm::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd)
{
	if (!m_bStatistics)
		return;
	//附加成功后，和切换线路走一个代码
}
//抓取人物信息
void AccountForm::OnFetchPlayerData()
{
	if (m_bStatisticsRunning)
		return;
	if (!g_CGAInterface->IsConnected())
		return;
	m_bStatisticsRunning = true;
	qDebug() << "AutoLogin OnFetchPlayerData！";
	auto pFetchGID = GetCurrentLoginGid();
	if (pFetchGID && pFetchGID->bFetched == false)
	{
		pFetchGID->bFetched = true;
	}
	//登录成功后 去读取信息
	m_bStatisticsRunning = false;
}

void AccountForm::on_checkBox_randomName_stateChanged(int state)
{
	if (ui->radioButton_randomName->isChecked())
	{
		//随机获取名称
		QString sName = g_pGameFun->CreateRandomName(0);
		ui->lineEdit_CharaName->setText(sName);
	}
}
