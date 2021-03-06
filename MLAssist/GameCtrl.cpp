#include "GameCtrl.h"
#include "../include/ITPublic.h"
#include "CGFunction.h"
#include "GPCalc.h"
#include "ITLog.h"
#include "ITObjectDataMgr.h"
#include "MINT.h"
#include "RpcSocketClient.h"
#include "YunLai.h"
#include "stdafx.h"
#include <QSettings>
#include <QTextCodec>
#include <QtConcurrent>

#define GAME_SKILL_NUM 16
#define GAME_SUB_SKILL_NUM 10
#define GAME_ITEM_NUM 28
#define GAME_PET_NUM 5
#define GAME_PET_SKILL_NUM 10

Q_DECLARE_METATYPE(QSharedPointer<CGA_NPCDialog_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA::cga_player_menu_items_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA::cga_unit_menu_items_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA_MapCellData_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA_MapUnits_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA::cga_working_result_t>)
Q_DECLARE_METATYPE(QSharedPointer<CProcessItem>)
Q_DECLARE_METATYPE(QSharedPointer<GameTeamPlayer>)
Q_DECLARE_METATYPE(QList<QSharedPointer<GameTeamPlayer> >)
Q_DECLARE_METATYPE(QSharedPointer<CGA::cga_trade_stuff_info_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA::cga_trade_dialog_t>)

GameCtrl::GameCtrl()
{
	qRegisterMetaType<GameItemPtr>("GameItemPtr");
	qRegisterMetaType<GameSkillPtr>("GameSkillPtr");
	qRegisterMetaType<GamePetPtr>("GamePetPtr");
	qRegisterMetaType<GameSearchPtr>("GameSearchPtr");
	qRegisterMetaType<GameBattleUnitPtr>("GameBattleUnitPtr");
	qRegisterMetaType<GameCompoundPtr>("GameCompoundPtr");
	qRegisterMetaType<CharacterPtr>("CharacterPtr");

	qRegisterMetaType<QSharedPointer<CGA_NPCDialog_t> >("QSharedPointer<CGA_NPCDialog_t>");
	qRegisterMetaType<QSharedPointer<CGA::cga_player_menu_items_t> >("QSharedPointer<CGA::cga_player_menu_items_t>");
	qRegisterMetaType<QSharedPointer<CGA::cga_unit_menu_items_t> >("QSharedPointer<CGA::cga_unit_menu_items_t>");
	qRegisterMetaType<QSharedPointer<CGA::cga_trade_stuff_info_t> >("QSharedPointer<CGA::cga_trade_stuff_info_t>");
	qRegisterMetaType<QSharedPointer<CGA::cga_trade_dialog_t> >("QSharedPointer<CGA::cga_trade_dialog_t>");
	qRegisterMetaType<QSharedPointer<CGA_MapCellData_t> >("QSharedPointer<CGA_MapCellData_t>");
	qRegisterMetaType<QSharedPointer<CGA_MapUnits_t> >("QSharedPointer<CGA_MapUnits_t>");
	qRegisterMetaType<QSharedPointer<CGA::cga_working_result_t> >("QSharedPointer<CGA::cga_working_result_t>");
	qRegisterMetaType<QSharedPointer<CProcessItem> >("QSharedPointer<CProcessItem>");
	qRegisterMetaType<QSharedPointer<GameTeamPlayer> >("QSharedPointer<GameTeamPlayer>");
	qRegisterMetaType<QList<QSharedPointer<GameTeamPlayer> > >("QList<QSharedPointer<GameTeamPlayer>>");
	m_nLastAutoRenTime.start();
	m_lastUpdateTeamTime.start();
	m_pGameCharacter = QSharedPointer<Character>(new Character);
	m_pFirstAidCfg = new GameFirstAidCfg;
	m_pHealCfg = new GameHealCfg;
	m_pBattlePetCfg = new GameBattlePetCfg;
	m_pEatFoodCfg = new GameConditionCfg;
	m_pEquipProtectCfg = new GameEquipProtectCfg;
	m_pEquipProtectCfg->nType = TCondition_EquipProtcet;
	m_pTransformation = new GameCfgBase;
	m_pCosplay = new GameCfgBase;
	m_pAutoPetRiding = new GameCfgBase;
	m_upgradePetCfg = new GameUpgradeCfg;
	m_upgradePetCfg->bEnabled = true;
	m_upgradePetCfg->normalCfg.bEnabled = false;
	m_upgradePlayerCfg = new GamePlayerUpgradeCfg;
	m_upgradePlayerCfg->bEnabled = true;
	m_upgradePlayerCfg->normalCfg.vigorVal = 333;
	m_upgradePlayerCfg->normalCfg.strengthVal = 333;
	m_upgradePlayerCfg->normalCfg.enduranceVal = 333;
	m_upgradePlayerCfg->normalCfg.agilityVal = 333;
	m_upgradePlayerCfg->normalCfg.magicalVal = 333;
	m_pEquipProtectCfg->bPercentage = true;
	m_pEquipProtectCfg->bChecked = true;
	//m_pBattleCtrl = new CBattleWorker;
	connect(&m_characterTimer, SIGNAL(timeout()), this, SLOT(OnGetCharacterData()));
	//connect(&m_mapTimer, SIGNAL(timeout()), this, SLOT(OnGetMapData()));
	connect(this, &GameCtrl::NotifyBattleAction, this, &GameCtrl::OnNotifyBattleAction, Qt::ConnectionType::QueuedConnection);
	connect(this, &GameCtrl::NotifyGameWndKeyDown, this, &GameCtrl::OnNotifyGameWndKeyDown, Qt::ConnectionType::QueuedConnection);
	bool bRet = connect(this, &GameCtrl::NotifyNPCDialog, this, &GameCtrl::OnNotifyNPCDialog, Qt::QueuedConnection);
	connect(this, &GameCtrl::NotifyPlayerMenu, this, &GameCtrl::OnNotifyPlayerMenu, Qt::QueuedConnection);
	connect(this, &GameCtrl::NotifyUnitMenu, this, &GameCtrl::OnNotifyUnitMenu, Qt::QueuedConnection);
	connect(this, &GameCtrl::NotifyConnectionState, this, &GameCtrl::OnNotifyConnectionState, Qt::ConnectionType::QueuedConnection);
	connect(this, &GameCtrl::NotifyChatMsg, this, &GameCtrl::OnNotifyChatMsg, Qt::ConnectionType::QueuedConnection);

	const ushort nurse_message[] = { 35201, 22238, 22797, 21527, 65311, 0 }; //???????????????

	m_NurseMessage = QString::fromUtf16(nurse_message);
	//for (int i = 0; i < GAME_SKILL_NUM; ++i)
	//{
	//	GameSkillPtr pSkill =  QSharedPointer<GameSkill>(new GameSkill);
	//	m_pGameSkills.append(pSkill);
	//	for (int n = 0; n < GAME_SUB_SKILL_NUM; ++n)
	//	{
	//		GameSkillPtr pSubSkill = QSharedPointer<GameSkill>(new GameSkill);
	//		pSkill->subskills.append(pSubSkill);
	//	}
	//}
	//for (int i = 0; i < GAME_ITEM_NUM; ++i)
	//{
	//	GameItemPtr pItem = GameItemPtr(new GameItem);
	//	m_pGameItems.append(pItem);
	//}
	//for (int i = 0; i < GAME_PET_NUM; ++i)
	//{
	//	auto pPet =GamePetPtr( new GamePet);
	//	m_pGamePets.append(pPet);
	//	for (int n = 0; n < GAME_PET_SKILL_NUM; ++n)
	//	{
	//		GameSkillPtr pSkill = QSharedPointer<GameSkill>(new GameSkill);
	//		pPet->skills.append(pSkill);
	//	}
	//}
	////???????????????????????????
	/*for (int i = 0; i < 20; ++i)
	{
		GameBattleUnitPtr pUnit = GameBattleUnitPtr(new GameBattleUnit);
		pBattleUnits.append(pUnit);
	}*/
	////???????????????????????? ????????????????????? ???????????????????????? ?????????10??? ??????50??? ??????50??????????????????
	//for (int i = 0; i < 100; ++i)
	//{
	//	GameCompoundPtr pItem = GameCompoundPtr( new GameCompound);
	//	m_pCompoundList.append(pItem);
	//}
	m_quickKeyMap.insert(TDef_Quick_Key_Logback, VK_F1);
	m_quickKeyMap.insert(TDef_Quick_Key_Logout, VK_F2);
	m_quickKeyMap.insert(TDef_Quick_Key_Cross, VK_F3);
	m_quickKeyMap.insert(TDef_Quick_Key_Trade, VK_F4);
	m_quickKeyMap.insert(TDef_Quick_Key_TradeNoAccept, VK_F5);
	m_quickKeyMap.insert(TDef_Quick_Key_SaveAll, VK_F7);
	m_quickKeyMap.insert(TDef_Quick_Key_FetchAll, VK_F8);
	m_quickKeyMap.insert(TDef_Quick_Key_CallFz, VK_INSERT);
	m_quickKeyMap.insert(TDef_Quick_Key_Encounter, VK_DELETE);
	m_upgradeTypeText.insert(TPET_POINT_TYPE_None, "???");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Vigor, "??????");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Strength, "??????");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Endurance, "??????");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Agility, "??????");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Magical, "??????");
	m_sEquipProtectFilters << "???????????????"
						   << "???????????????"
						   << "????????????";
}

GameCtrl::~GameCtrl()
{
}

GameCtrl *GameCtrl::getInstace()
{
	static GameCtrl pGameCtrl;
	return &pGameCtrl;
}

void GameCtrl::InitCmdParse()
{
	QCommandLineOption *gameType = new QCommandLineOption("gametype", "", "gametype");
	QCommandLineOption *loginUser = new QCommandLineOption("loginuser", "", "loginuser");
	QCommandLineOption *loginPwd = new QCommandLineOption("loginpwd", "", "loginpwd");
	QCommandLineOption *gid = new QCommandLineOption("gid", "", "gid");
	QCommandLineOption *bigserver = new QCommandLineOption("bigserver", "", "bigserver");
	QCommandLineOption *server = new QCommandLineOption("server", "", "server");
	QCommandLineOption *character = new QCommandLineOption("character", "", "character");
	QCommandLineOption *autologin = new QCommandLineOption("autologin", "", "autologin");
	QCommandLineOption *skipupdate = new QCommandLineOption("skipupdate", "", "skipupdate");
	QCommandLineOption *autochangeserver = new QCommandLineOption("autochangeserver", "", "autochangeserver");
	QCommandLineOption *autokillgame = new QCommandLineOption("autokillgame", "", "autokillgame");
	QCommandLineOption *autocreatechara = new QCommandLineOption("autocreatechara", "", "autocreatechara");
	QCommandLineOption *createcharachara = new QCommandLineOption("createcharachara", "", "createcharachara");
	QCommandLineOption *createcharaeye = new QCommandLineOption("createcharaeye", "", "createcharaeye");
	QCommandLineOption *createcharamouth = new QCommandLineOption("createcharamouth", "", "createcharamouth");
	QCommandLineOption *createcharacolor = new QCommandLineOption("createcharacolor", "", "createcharacolor");
	QCommandLineOption *createcharapoints = new QCommandLineOption("createcharapoints", "", "createcharapoints");
	QCommandLineOption *createcharaelements = new QCommandLineOption("createcharaelements", "", "createcharaelements");
	QCommandLineOption *createcharaname = new QCommandLineOption("createcharaname", "", "createcharaname");
	QCommandLineOption *loadscript = new QCommandLineOption("loadscript", "", "loadscript");
	QCommandLineOption *scriptautorestart = new QCommandLineOption("scriptautorestart", "", "scriptautorestart");
	QCommandLineOption *scriptfreezestop = new QCommandLineOption("scriptfreezestop", "", "scriptfreezestop");
	QCommandLineOption *scriptautoterm = new QCommandLineOption("scriptautoterm", "", "scriptautoterm");
	QCommandLineOption *injuryprotect = new QCommandLineOption("injuryprotect", "", "injuryprotect");
	QCommandLineOption *soulprotect = new QCommandLineOption("soulprotect", "", "soulprotect");
	QCommandLineOption *logBackRestart = new QCommandLineOption("logbackrestart", "", "logbackrestart");
	QCommandLineOption *transUserInput = new QCommandLineOption("transuserinput", "", "transuserinput");

	QCommandLineOption *loadsettings = new QCommandLineOption("loadsettings", "", "loadsettings");
	QCommandLineOption *killfreeze = new QCommandLineOption("killfreeze", "", "killfreeze", "60");
	QCommandLineOption *consolemaxlines = new QCommandLineOption("consolemaxlines", "", "consolemaxlines", "100");
	QCommandLineOption *chatmaxlines = new QCommandLineOption("chatmaxlines", "", "chatmaxlines", "100");
	m_commandMap.insert(TCmdConfig_GameType, gameType);
	m_commandMap.insert(TCmdConfig_LoginUser, loginUser);
	m_commandMap.insert(TCmdConfig_LoginPwd, loginPwd);
	m_commandMap.insert(TCmdConfig_LoginGid, gid);
	m_commandMap.insert(TCmdConfig_BigServer, bigserver);
	m_commandMap.insert(TCmdConfig_Server, server);
	m_commandMap.insert(TCmdConfig_Character, character);
	m_commandMap.insert(TCmdConfig_AutoLogin, autologin);
	m_commandMap.insert(TCmdConfig_SkipUpdate, skipupdate);
	m_commandMap.insert(TCmdConfig_AutoChangeServer, autochangeserver);
	m_commandMap.insert(TCmdConfig_AutoKillGame, autokillgame);
	m_commandMap.insert(TCmdConfig_AutoCreateChara, autocreatechara);
	m_commandMap.insert(TCmdConfig_CreateCharaChara, createcharachara);
	m_commandMap.insert(TCmdConfig_CreateCharaEye, createcharaeye);
	m_commandMap.insert(TCmdConfig_CreateCharaMouth, createcharamouth);
	m_commandMap.insert(TCmdConfig_CreateCharaColor, createcharacolor);
	m_commandMap.insert(TCmdConfig_CreateCharaPoints, createcharapoints);
	m_commandMap.insert(TCmdConfig_CreateCharaElements, createcharaelements);
	m_commandMap.insert(TCmdConfig_CreateCharaName, createcharaname);
	m_commandMap.insert(TCmdConfig_LoadScript, loadscript);
	m_commandMap.insert(TCmdConfig_ScriptAutoRestart, scriptautorestart);
	m_commandMap.insert(TCmdConfig_ScriptFreezeStop, scriptfreezestop);
	m_commandMap.insert(TCmdConfig_ScriptAutoTerm, scriptautoterm);
	m_commandMap.insert(TCmdConfig_InjuryProtect, injuryprotect);
	m_commandMap.insert(TCmdConfig_SoulProtect, soulprotect);
	m_commandMap.insert(TCmdConfig_LoadSettings, loadsettings);
	m_commandMap.insert(TCmdConfig_KillFreeze, killfreeze);
	m_commandMap.insert(TCmdConfig_ConsoleMaxLines, consolemaxlines);
	m_commandMap.insert(TCmdConfig_ChatMaxLines, chatmaxlines);
	m_commandMap.insert(TCmdConfig_LogBackRestart, logBackRestart);
	m_commandMap.insert(TCmdConfig_TransUserInput, transUserInput);

	m_cmdParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
	m_cmdParser.addOption(*gameType);
	m_cmdParser.addOption(*loginUser);
	m_cmdParser.addOption(*loginPwd);
	m_cmdParser.addOption(*gid);
	m_cmdParser.addOption(*bigserver);
	m_cmdParser.addOption(*server);
	m_cmdParser.addOption(*character);
	m_cmdParser.addOption(*autologin);
	m_cmdParser.addOption(*skipupdate);
	m_cmdParser.addOption(*autochangeserver);
	m_cmdParser.addOption(*autokillgame);
	m_cmdParser.addOption(*autocreatechara);
	m_cmdParser.addOption(*createcharachara);
	m_cmdParser.addOption(*createcharaeye);
	m_cmdParser.addOption(*createcharamouth);
	m_cmdParser.addOption(*createcharacolor);
	m_cmdParser.addOption(*createcharapoints);
	m_cmdParser.addOption(*createcharaelements);
	m_cmdParser.addOption(*createcharaname);
	m_cmdParser.addOption(*loadscript);
	m_cmdParser.addOption(*scriptfreezestop);
	m_cmdParser.addOption(*scriptautorestart);
	m_cmdParser.addOption(*scriptautoterm);
	m_cmdParser.addOption(*injuryprotect);
	m_cmdParser.addOption(*soulprotect);
	m_cmdParser.addOption(*loadsettings);
	m_cmdParser.addOption(*killfreeze);
	m_cmdParser.addOption(*consolemaxlines);
	m_cmdParser.addOption(*chatmaxlines);
	m_cmdParser.addOption(*logBackRestart);
	m_cmdParser.addOption(*transUserInput);
	m_cmdParser.process(*qApp);
}

void GameCtrl::RunParseCmd()
{
	//qDebug() << m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoadSettings));
	//qDebug() << m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoadScript));
	//qDebug() << GBK2UTF8(m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoadSettings)));
	//qDebug() << GBK2UTF8(m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoadScript)));

	emit NotifyFillAutoLogin(m_cmdParser.value(*m_commandMap.value(TCmdConfig_GameType)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoginUser)),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoginPwd)),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoginGid)),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_BigServer)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_Server)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_Character)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_AutoLogin)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_SkipUpdate)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_AutoChangeServer)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_AutoKillGame)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_AutoCreateChara)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_CreateCharaChara)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_CreateCharaEye)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_CreateCharaMouth)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_CreateCharaColor)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_CreateCharaPoints)),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_CreateCharaElements)),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_CreateCharaName)));
	emit signal_loadUserConfig(m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoadSettings)));

	emit NotifyFillLoadScript(m_cmdParser.value(*m_commandMap.value(TCmdConfig_LoadScript)),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_ScriptAutoRestart)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_ScriptFreezeStop)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_InjuryProtect)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_SoulProtect)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_ConsoleMaxLines)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_LogBackRestart)).toInt(),
			m_cmdParser.value(*m_commandMap.value(TCmdConfig_TransUserInput)).toInt());

	emit NotifyFillStaticSettings(m_cmdParser.value(*m_commandMap.value(TCmdConfig_KillFreeze)).toInt(), m_cmdParser.value(*m_commandMap.value(TCmdConfig_ChatMaxLines)).toInt());
}

void GameCtrl::Disconnect()
{
	//???????????????????????????????????????????????????
	StopUpdateTimer();
	WaitThreadFini();
	qDebug() << "???????????????????????????????????????";
	g_CGAInterface->Disconnect();
	emit signal_clearUiInfo();
}

void GameCtrl::SetGameGid(const QString &gid)
{
	if (m_sGid != gid)
	{
		QMutexLocker locker(&m_gidMutex);
		m_sGid = gid;
	}
}

QString GameCtrl::GetGameGid()
{
	QMutexLocker locker(&m_gidMutex);
	return m_sGid;
}

void GameCtrl::HttpGetGameProcInfo(QJsonDocument *doc)
{
	QJsonObject obj;
	ULONG pid, tid;
	if (m_gameHwnd && IsWindow(m_gameHwnd) && (tid = GetWindowThreadProcessId(m_gameHwnd, &pid)) != 0)
	{
		obj.insert("errcode", 0);
		obj.insert("pid", (int)pid);
		obj.insert("tid", (int)tid);
		obj.insert("gameport", (int)m_nGamePort);
		obj.insert("hwnd", (int)m_gameHwnd);
	}
	else
	{
		obj.insert("errcode", 1);
		obj.insert("message", tr("not attached to any game process yet."));
	}
	doc->setObject(obj);
}

void GameCtrl::KillGameWndProcess()
{
	auto attachHwnd = getGameHwnd();
	if (!attachHwnd)
		return;
	if (!IsWindow(attachHwnd))
		return;
	DWORD pid, tid;
	tid = GetWindowThreadProcessId(attachHwnd, &pid);
	if (!pid || !tid)
	{
		setGameHwnd(nullptr);
		return;
	}
	WCHAR szMutex[32];
	wsprintfW(szMutex, L"CGAAttachMutex_%d", pid);
	HANDLE hAttachMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, szMutex);
	if (hAttachMutex)
	{
		CloseHandle(hAttachMutex);
	}
	HANDLE ProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (ProcessHandle)
	{
		if (STATUS_SUCCESS == NtTerminateProcess(ProcessHandle, 0))
		{
			Disconnect();
		}
		CloseHandle(ProcessHandle);
	}
}

int GameCtrl::GetLastBattlePetAvgLv()
{
	//int level = 0;
	//int count = 0;
	//for (int i = 0xA; i < 20; ++i)
	//{
	//	if (!pBattleUnits[i]->exist)
	//		continue;
	//	level += pBattleUnits[i]->level;
	//	count += 1;
	//}
	//if (count <= 0) //???????????????
	//{
	//	auto pet=g_pGameFun->GetBattlePet();
	//	if (pet)
	//	{
	//		return pet->level;
	//	}
	//	return 0;
	//}
	//
	//float avgLv = (float)level / count;
	//return avgLv;
	if (m_lastBattleAvgTeamPetLv == 0)
	{
		auto pet = g_pGameFun->GetBattlePet();
		if (pet)
		{
			m_lastBattleAvgTeamPetLv = pet->level;
			return pet->level;
		}
		return 0;
	}
	return m_lastBattleAvgTeamPetLv;
	/*auto pet = g_pGameFun->GetBattlePet();
	if (pet)
	{
		return pet->level;
	}
	return 0;*/
}

void GameCtrl::RestLastBattlePetAvgLv()
{
	auto pet = g_pGameFun->GetBattlePet();
	if (pet)
	{
		m_lastBattleAvgTeamPetLv = pet->level;
	}
	m_lastBattleAvgTeamPetLv = 0;
}

int GameCtrl::GetLastBattlePetLv(int type)
{
	if (type == 1) //??????
	{
		if (m_lastBattleTroopPetMinLv == 0)
		{
			auto pet = g_pGameFun->GetBattlePet();
			if (pet)
			{
				m_lastBattleTroopPetMinLv = pet->level;
				return pet->level;
			}
			return 0;
		}
		return m_lastBattleTroopPetMinLv;
	}
	else if (type == 2) //??????
	{
	}
}

void GameCtrl::RestLastBattlePetLv(int type)
{
	if (type == 1) //??????
	{
		m_lastBattleTroopPetMinLv = 0;
	}
}

void GameCtrl::SetDropPetItemChecked(int nType, int state)
{
	m_bDropPetCheckItem.insert(nType, state ? true : false);
}

void GameCtrl::SetDropPetItemValue(int nType, QVariant nVal)
{
	m_nDropPetCheckVal.insert(nType, nVal);
}

bool GameCtrl::getGameConnectState()
{
	int ingame = 0;
	if (g_CGAInterface->IsInGame(ingame) && ingame)
	{
		return true;
	}
	return false;
}
/*GetWorldStatus
* 9 ?????? 10 ?????? 11 ???????????????????????????
* cga.GetGameStatus
* 202 | 205  ??????
* 1 ??????
* 2 ???????????? 4 ???????????? 5 ????????? 8 ?????????????????????????????? 11 ???????????????????????????????????????????????????????????????????????????3????????????
* 3 ??????
* 1 ????????????????????????????????????
* 2 ????????????????????????
* 6 ??????????????????
* ?????????3 ???????????????????????????3
*201  204 ??????????????????  ????????????
201 ??????
201 205
200 202 205??????
202???????????????????????????
201???????????????
204 205 ??????????????????

1    ??????
10 3 ??????????????????
6 ??????????????????

8???????????? ????????????
11 2????????????????????????
2????????????  ???????????????????????????*/

QString GameCtrl::getGameState()
{
	int gameStatus, worldStatus;
	if (g_CGAInterface->GetGameStatus(gameStatus) && g_CGAInterface->GetWorldStatus(worldStatus))
	{
		switch (gameStatus)
		{
			case 1: return "??????????????????"; //????????????
			case 2: return "????????????";	   //"????????????";
			case 3:
			{
				return worldStatus == 9 ? "??????" : "????????????"; //??????????????????
			}
			case 4: return "????????????";
			case 5: return "??????????????????"; //5???????????????????????????
			case 6: return "????????????";
			case 8: return "????????????"; //??????????????????
			case 11: return "????????????";
			case 200:
			case 201: return "????????????";
			case 202: return "?????????";
			case 203:
			case 204:
			case 205: return "????????????";
			default:
				break;
		}
	}
	return "??????";
}

void GameCtrl::LoadConfig(const QString &path)
{
	if (path.isEmpty())
		return;
	QSettings iniFile(path, QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("GB2312")); //??????????????????????????????????????????  ??????????????????????????????

	ClearRenItems();
	int renCount = iniFile.value("ren/count", 0).toInt();
	for (int i = 1; i <= renCount; ++i)
	{
		QString itemKeyName = QString("item%1").arg(i);
		QString itemKeyVal = QString("checked%1").arg(i);
		QString itemName = iniFile.value(QString("ren/%1").arg(itemKeyName), "").toString();
		int itemVal = iniFile.value(QString("ren/%1").arg(itemKeyVal), "").toInt();
		GameItemPtr pItem = GameItemPtr(new GameItem);
		pItem->name = itemName;
		pItem->isDrop = itemVal;
		m_pRenItemList.append(pItem);
	}

	ClearDieItems();
	int dieCount = iniFile.value("die/count", 0).toInt();
	for (int i = 1; i <= dieCount; ++i)
	{
		QString itemKeyName = QString("item%1").arg(i);
		QString itemKeyVal = QString("checked%1").arg(i);
		QString itemName = iniFile.value(QString("die/%1").arg(itemKeyName), "").toString();
		int itemVal = iniFile.value(QString("die/%1").arg(itemKeyVal), "").toInt();
		GameItemPtr pItem = GameItemPtr(new GameItem);
		pItem->name = itemName;
		pItem->isPile = itemVal;
		m_pPileItemList.append(pItem);
	}

	//emit signal_loadUserConfig();
}

void GameCtrl::SaveConfig(const QString &path)
{
	if (path.isEmpty())
		return;
	QSettings iniFile(path, QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("GB2312")); //??????????????????????????????????????????  ??????????????????????????????
}

void GameCtrl::StartUpdateTimer()
{
	m_bExit = false;
	m_updateTimer.start(5000); //2???????????????????????????
	//m_characterTimer.start(1000);	//1?????????1?????????
	m_lastUploadTime.start();
	m_mapTimer.start(500);																											 //???????????????????????????
	m_nLastOverTime.start();																										 //??????????????????
	m_uLastUseItemTime.start();																										 //??????????????????
	m_nLastAutoDropPetTime.start();																									 //??????????????????
	m_lastRecvNpcDlgTime.start();																									 //???????????????????????????
	m_lastNormalState.start();																										 //?????????????????????
	g_CGAInterface->RegisterGameWndKeyDownNotify(std::bind(&GameCtrl::NotifyGameWndKeyDown, this, std::placeholders::_1));			 //????????????
	g_CGAInterface->RegisterBattleActionNotify(std::bind(&GameCtrl::NotifyBattleActionCallBack, this, std::placeholders::_1));		 //????????????
	g_CGAInterface->RegisterChatMsgNotify(std::bind(&GameCtrl::NotifyChatMsgCallback, this, std::placeholders::_1));				 //??????????????????
	g_CGAInterface->RegisterNPCDialogNotify(std::bind(&GameCtrl::NotifyNPCDialogCallback, this, std::placeholders::_1));			 //npc???????????????
	g_CGAInterface->RegisterDownloadMapNotify(std::bind(&GameCtrl::NotifyDownloadMapCallback, this, std::placeholders::_1));		 //????????????
	g_CGAInterface->RegisterPlayerMenuNotify(std::bind(&GameCtrl::NotifyPlayerMenuCallback, this, std::placeholders::_1));			 //??????????????????
	g_CGAInterface->RegisterConnectionStateNotify(std::bind(&GameCtrl::NotifyConnectionStateCallback, this, std::placeholders::_1)); //????????????
	g_CGAInterface->RegisterUnitMenuNotify(std::bind(&GameCtrl::NotifyUnitMenuCallback, this, std::placeholders::_1));				 //????????????
	g_CGAInterface->RegisterWorkingResultNotify(std::bind(&GameCtrl::NotifyWorkingResultCallback, this, std::placeholders::_1));	 //??????????????????
	g_CGAInterface->RegisterServerShutdownNotify(std::bind(&GameCtrl::NotifyServerShutdown, this, std::placeholders::_1));			 //?????????????????????

	g_CGAInterface->RegisterTradeStuffsNotify(std::bind(&GameCtrl::NotifyTradeStuffsCallback, this, std::placeholders::_1)); //??????????????????
	g_CGAInterface->RegisterTradeDialogNotify(std::bind(&GameCtrl::NotifyTradeDialogCallback, this, std::placeholders::_1)); //?????????????????????
	g_CGAInterface->RegisterTradeStateNotify(std::bind(&GameCtrl::NotifyTradeStateCallback, this, std::placeholders::_1));	 //??????????????????

	emit NotifyAttachProcessOk(m_gameProcessID, m_nGameThreadID, m_nGamePort, (quint32)m_gameHwnd);
	m_characterFuture = QtConcurrent::run(GetCharacterDataThread, this);
	m_itemsFuture = QtConcurrent::run(GetItemDataThread, this);
	m_mapFuture = QtConcurrent::run(DownloadMapThread, this);
	m_normalFuture = QtConcurrent::run(NormalThread, this);
	m_notifyTimeoutFuture = QtConcurrent::run(NotifyTimeoutThread, this);
	emit signal_attachGame(); //
							  //qDebug() << "??????????????????????????????" << QThreadPool::globalInstance()->maxThreadCount();
							  ////???????????????????????????
							  //qDebug() << "??????????????????????????????" << QThreadPool::globalInstance()->activeThreadCount();
							  ////?????????????????????
							  //QThreadPool::globalInstance()->setMaxThreadCount(100);
							  //qDebug() << "??????????????????????????????" << QThreadPool::globalInstance()->maxThreadCount();
	g_pGameFun->RestFun();
	{
		if (m_bCreateLog)
		{
			QString sLogFileName;
			void *pBaseAddr = YunLai::GetProcessImageBase1(m_gameProcessID);
			QString sUserName = QString(" [%1] ").arg(QString::fromWCharArray(YunLai::ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, (ULONG_PTR)pBaseAddr + 0xE12D30, 17))));
			QString szLoginUserID = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, (ULONG_PTR)pBaseAddr + 0xBDB488, 100); //??????id
			sLogFileName = szLoginUserID + sUserName;
			sLogFileName = ConvertFileName(sLogFileName);
			LOGGER->setLogFileName(sLogFileName.toStdWString());
			LOGGER->open();
		}
	}
}

void GameCtrl::StopUpdateTimer()
{
	m_bExit = true; //????????????
	g_pGameFun->StopFun();
	m_gameProcessID = 0;
	m_gameHwnd = nullptr;
	m_nGamePort = 0;
	m_nGameThreadID = 0;
	m_gameBaseAddr = 0;
	if (m_hGameMutex != NULL)
	{
		CloseHandle(m_hGameMutex);
	}
	m_hGameMutex = nullptr;
	m_updateTimer.stop();	 //???????????????
	m_characterTimer.stop(); //???????????????
	m_mapTimer.stop();		 //???????????????
	if (m_bCreateLog)
		LOGGER->close();
}

void GameCtrl::ResetGameConnectState()
{
	m_lastGameConnMsg.clear();
	m_lastGameConnState = -1;
}

void GameCtrl::GetLoginGameConnectState(int &state, QString &msg)
{
	state = m_lastGameConnState;
	msg = m_lastGameConnMsg;
}

void GameCtrl::setRenItemIsChecked(const QString &name, int nVal)
{
	bool bExist = false;
	foreach (auto pItem, m_pRenItemList)
	{
		if (pItem->name == name)
		{
			pItem->isDrop = nVal;
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		GameItemPtr pItem = GameItemPtr(new GameItem);
		pItem->name = name;
		pItem->isDrop = nVal;
		if (name.contains(","))
		{
			QStringList limitCode = name.split(",");
			if (limitCode.size() >= 2)
			{
				pItem->isDropInterval = true;
				pItem->dropMinCode = limitCode[0].toInt();
				pItem->dropMaxCode = limitCode[1].toInt();
			}
		}
		m_pRenItemList.append(pItem);
	}
}

void GameCtrl::setSaleItemIsChecked(const QString &name, int nVal)
{
	bool bExist = false;
	foreach (auto pItem, m_pSaleItemList)
	{
		if (pItem->name == name)
		{
			pItem->isSale = nVal;
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		GameItemPtr pItem = GameItemPtr(new GameItem);
		pItem->name = name;
		pItem->isSale = nVal;
		m_pSaleItemList.append(pItem);
	}
}

void GameCtrl::SetPickItemIsChecked(const QString &name, int nVal)
{
	bool bExist = false;
	foreach (auto pItem, m_pPickItemList)
	{
		if (pItem->name == name)
		{
			pItem->isPick = nVal;
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		GameItemPtr pItem = GameItemPtr(new GameItem);
		pItem->name = name;
		pItem->isPick = nVal;
		m_pPickItemList.append(pItem);
	}
}

GameItemPtr GameCtrl::setDieItemIsChecked(const QString &name, int nVal)
{
	GameItemPtr pExitItem = nullptr;
	bool bExist = false;
	foreach (auto pItem, m_pPileItemList)
	{
		if (pItem->name == name)
		{
			pItem->isPile = nVal;
			bExist = true;
			pExitItem = pItem;
			break;
		}
	}
	if (!bExist)
	{
		GameItemPtr pItem = GameItemPtr(new GameItem);
		pItem->name = name;
		pItem->isPile = nVal;
		m_pPileItemList.append(pItem);
		pExitItem = pItem;
	}
	return pExitItem;
}

void GameCtrl::setSearchItem(const QString &name, int nVal)
{
	bool bExist = false;
	foreach (auto pItem, m_pSearchList)
	{
		if (pItem->name == name)
		{
			pItem->bSearch = nVal;
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		GameSearchPtr pItem = GameSearchPtr(new GameSearch);
		pItem->name = name;
		pItem->bSearch = nVal;
		m_pSearchList.append(pItem);
	}
}

GameConditionCfg *GameCtrl::GetPlayerEatCfg(int type)
{
	GameConditionCfg *pCfg = m_pEatFoodJudge.value(type);
	if (pCfg == nullptr)
	{
		pCfg = new GameConditionCfg;
		pCfg->nType = type;
		m_pEatFoodJudge.insert(type, pCfg);
	}
	return pCfg;
}

GameEquipProtectCfg *GameCtrl::GetPlayerEquipProtectCfg()
{
	//if (!m_pEquipProtectCfg)
	//{
	//	m_pEquipProtectCfg = new GameEquipProtectCfg;
	//	m_pEquipProtectCfg->nType = TCondition_EquipProtcet;
	//	m_pEquipProtectCfg->bPercentage = true;
	//}
	return m_pEquipProtectCfg;
}

bool GameCtrl::RenItem(GameItemPtr pItem)
{
	if (pItem == nullptr)
		return false;
	bool result = false;
	if (g_CGAInterface->DropItem(pItem->pos, result) && result)
		return true;
	return false;
}

bool GameCtrl::RenItems()
{
	if (m_nLastAutoRenTime.elapsed() > 3000)
	{
		m_nLastAutoRenTime.restart();
		bool result = false;
		foreach (auto pRenItem, m_pRenItemList)
		{
			if (pRenItem->isDrop)
			{
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);
					if (pItem && pItem->exist)
					{
						if (!pRenItem->isDropInterval && (pItem->name == pRenItem->name || QString::number(pItem->id) == pRenItem->name))
						{
							if (g_CGAInterface->DropItem(pItem->pos, result) && result)
								continue;
						}
						else if (pRenItem->isDropInterval && (pItem->id >= pRenItem->dropMinCode && pItem->id <= pRenItem->dropMaxCode))
						{
							if (g_CGAInterface->DropItem(pItem->pos, result) && result)
								continue;
						}
					}
				}
			}
		}
		return true;
	}

	return false;
}
//????????????  ??????????????????
bool GameCtrl::DieItems()
{
	foreach (auto pDieItem, m_pPileItemList)
	{
		if (pDieItem->isPile)
		{
			for (size_t i = 0; i < m_pGameItems.size(); i++)
			{
				GameItemPtr pItem = m_pGameItems.at(i); //????????? ?????????????????????????????????
				if (pItem && pItem->exist && (pItem->name == pDieItem->name || pItem->id == pDieItem->name) && pItem->count < pDieItem->maxCount)
				{
					for (size_t n = 0; n < m_pGameItems.size(); n++)
					{
						GameItemPtr pOtherItem = m_pGameItems.at(n);
						//?????????id??????
						if (pOtherItem && pOtherItem->exist && pOtherItem->name == pItem->name && pOtherItem->id == pItem->id && pOtherItem != pItem && pOtherItem->count < pDieItem->maxCount)
						{
							bool result = false;
							if (g_CGAInterface->MoveItem(pItem->pos, pOtherItem->pos, -1, result) && result)
								return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool GameCtrl::OnEatItem()
{
	if (m_uLastUseItemTime.elapsed() < 5000) //??????????????????
		return false;
	bool bEat = false;
	for (auto it = m_pEatFoodJudge.begin(); it != m_pEatFoodJudge.end(); ++it)
	{
		if (it.value()->bChecked)
		{
			bEat = true;
			break;
		}
	}
	//bug?????? ?????????????????????????????????????????????????????????????????????????????????
	//??????????????????????????????????????????
	if (!bEat)
	{
		m_pEatFoodCfg->bChecked = false;
		return false;
	}

	//????????????????????? ??????????????????

	CGA::cga_player_info_t char_info;
	if (!g_CGAInterface->GetPlayerInfo(char_info))
	{
		return false;
	}

	GamePetPtr pPet = g_pGameFun->GetBattlePet(); //?????????????????????
	for (auto it = m_pEatFoodJudge.begin(); it != m_pEatFoodJudge.end(); ++it)
	{
		auto pCfg = it.value();
		if (pCfg->bChecked)
		{
			if (pCfg->nType == TCondition_PlayerEatMagic && (char_info.mp <= (char_info.maxmp * pCfg->dVal / 100)))
			{ //?????????
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);			//????????? ?????????????????????????????????
					if (pItem && pItem->exist && pItem->type == 23) //??????
					{
						bool result = false;
						
						{
							QMutexLocker locker(&m_pEatFoodCfg->cMutex);
							m_pEatFoodCfg->bChecked = true;
							m_pEatFoodCfg->selectName = QString::fromStdString(char_info.name);
							m_pEatFoodCfg->selectSubName = QString::fromStdString(char_info.name);
						}					
						if (g_CGAInterface->UseItem(pItem->pos, result) && result)
						{
							m_uLastUseItemTime.restart();
							return true;
						}
					}
				}
			}
			else if (pCfg->nType == TCondition_PetEatMagic && pPet && (pPet->mp <= (pPet->maxmp * pCfg->dVal / 100)))
			{ //?????????
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);			//????????? ?????????????????????????????????
					if (pItem && pItem->exist && pItem->type == 23) //??????
					{
						bool result = false;
						{
							QMutexLocker locker(&m_pEatFoodCfg->cMutex);
							m_pEatFoodCfg->bChecked = true;
							m_pEatFoodCfg->selectName = QString::fromStdString(char_info.name);
							if (pPet->name.isEmpty())
								m_pEatFoodCfg->selectSubName = pPet->realname;
							else
								m_pEatFoodCfg->selectSubName = pPet->name;
						}
						if (g_CGAInterface->UseItem(pItem->pos, result) && result)
						{
							m_uLastUseItemTime.restart();
							return true;
						}
					}
				}
			}
			else if (pCfg->nType == TCondition_PlayerEatMedicament && (char_info.hp <= (char_info.maxhp * pCfg->dVal / 100)))
			{ //?????????
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);			//????????? ?????????????????????????????????
					if (pItem && pItem->exist && pItem->type == 43) //??????
					{
						bool result = false;
						{
							QMutexLocker locker(&m_pEatFoodCfg->cMutex);
							m_pEatFoodCfg->bChecked = true;
							m_pEatFoodCfg->selectName = QString::fromStdString(char_info.name);
							m_pEatFoodCfg->selectSubName = QString::fromStdString(char_info.name);
						}	
						if (g_CGAInterface->UseItem(pItem->pos, result) && result)
						{
							m_uLastUseItemTime.restart();
							return true;
						}
					}
				}
			}
			else if (pCfg->nType == TCondition_PetEatMedicament && pPet && (pPet->hp <= (pPet->maxhp * pCfg->dVal / 100)))
			{ //?????????
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);			//????????? ?????????????????????????????????
					if (pItem && pItem->exist && pItem->type == 43) //?????????
					{
						bool result = false;
						{
							QMutexLocker locker(&m_pEatFoodCfg->cMutex);
							m_pEatFoodCfg->bChecked = true;
							m_pEatFoodCfg->selectName = QString::fromStdString(char_info.name);
							if (pPet->name.isEmpty())
								m_pEatFoodCfg->selectSubName = pPet->realname;
							else
								m_pEatFoodCfg->selectSubName = pPet->name;
						}
						if (g_CGAInterface->UseItem(pItem->pos, result) && result)
						{
							m_uLastUseItemTime.restart();
							return true;
						}
					}
				}
			}
		}
	}
	return true;
}

bool GameCtrl::DoSaleItems()
{

	return true;
}

void GameCtrl::ReadItemData()
{
	return;
	//	m_itemHash.clear();
	for (size_t i = 0; i < 28; i++)
	{
		GameItemPtr pItem = m_pGameItems.at(i);

		DWORD pAddress = 0x00E2A566;
		DWORD offset = i * 0x65C;
		pAddress += offset;
		DWORD pExistItemAdd = pAddress - 0x2;
		DWORD pItemCodeAddr = pAddress + 0x63E;
		DWORD pItemItemCount = pItemCodeAddr + 0x4;
		int isExist = YunLai::ReadMemoryWordFromProcessID(GameData::getInstance().getGamePID(), pExistItemAdd);
		QString itemName;
		int nCount = 0;
		int itemCode = 0;
		if (isExist)
		{
			itemName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 100)));
			nCount = YunLai::ReadMemoryWordFromProcessID(GameData::getInstance().getGamePID(), pItemItemCount);
			itemCode = YunLai::ReadMemoryWordFromProcessID(GameData::getInstance().getGamePID(), pItemCodeAddr);
			//			m_itemHash.insert(itemName, pItem);
		}
		else
			itemName = "";
		pItem->name = itemName;
		pItem->id = itemCode;
		pItem->count = nCount;
		pItem->pos = i;
	}
}
//????????????
bool GameCtrl::AutoFirstAid()
{
	do
	{
		if (!m_bAutoFirstAid)
			break;																		 //return false;
		if (QDateTime::currentMSecsSinceEpoch() < (m_pFirstAidCfg->lastUseSkill + 2000)) //2????????????
			break;																		 //return false;

		//1???????????? ??????
		if (!m_pFirstAidCfg->bSelf && !m_pFirstAidCfg->bPet && !m_pFirstAidCfg->bTeammate)
		{
			m_pFirstAidCfg->bChecked = false;
			break; //return false;
		}
		//????????????
		if (m_pFirstAidCfg->dFirstAidHp <= 0)
			break; //return false;
		GameSkillList pSkillList = m_pGameSkills;
		GameSkillPtr pFirstAidSkill = nullptr;
		foreach (auto pSkill, pSkillList)
		{
			if (pSkill->name == "??????")
			{
				pFirstAidSkill = pSkill;
				break;
			}
		}
		if (pFirstAidSkill == nullptr)
		{
			//	qDebug() << "??????????????????";
			break;
			//return false;
		}
		//qDebug() << pFirstAidSkill->level << pFirstAidSkill->maxLevel << pFirstAidSkill->cost;
		//?????????????????? level  ????????????????????????maxLevel   cost????????? ?????????0  ??????????????????
		if (m_pFirstAidCfg->nLv == -1) //??????
		{
			if (pFirstAidSkill->level > pFirstAidSkill->maxLevel)
			{ //?????????????????? maxLevel
				m_pFirstAidCfg->nLv = pFirstAidSkill->maxLevel - 1;
			}
			else
			{
				m_pFirstAidCfg->nLv = pFirstAidSkill->level - 1;
			}
			/*	GameSkillList subSkillList = pFirstAidSkill->subskills;
			foreach (auto subSkill, subSkillList)
			{
				qDebug() << subSkill->id << subSkill->index << subSkill->level << subSkill->maxLevel << subSkill->cost << subSkill->available;
				m_pFirstAidCfg->dCost = subSkill->cost;
			}*/
		}
		if (m_pFirstAidCfg->nLv > (pFirstAidSkill->maxLevel - 1)) //?????????????????? > ???????????????????????? ??????
		{														  //?????????????????? maxLevel
																  //		qDebug() << QString("?????????????????????%1").arg(pFirstAidSkill->maxLevel);
			break;
			//return false;
		}
		if (pFirstAidSkill->subskills.size() < 1)
			return false;
		if (pFirstAidSkill->subskills.size() < (m_pFirstAidCfg->nLv + 1))
			return false;
		m_pFirstAidCfg->dCost = pFirstAidSkill->subskills.at(m_pFirstAidCfg->nLv)->cost;
		//	qDebug() << QString("??????%1").arg(m_pFirstAidCfg->dCost);
		auto pNewChar = g_pGameFun->GetGameCharacter();
		if (!pNewChar) return false;
		if (pNewChar->mp < m_pFirstAidCfg->dCost)
		{
			//	qDebug() << "?????????";
			break; //return false;
		}
		//??????????????????
		//??????
		//??????
		QString selectName, selectSubName;

		int nFirstAidTarget = -1; //0?????? 1????????? 2??????
		//????????????
		if (m_pFirstAidCfg->bSelf)
		{
			double dFirstAidVal = pNewChar->maxhp * m_pFirstAidCfg->dFirstAidHp / 100; //?????????
			if (pNewChar->hp < dFirstAidVal)										   //????????????
			{
				nFirstAidTarget = 0;
				selectName = pNewChar->name;
				selectSubName = pNewChar->name;
			}
		}
		//??????
		if (nFirstAidTarget == -1 && m_pFirstAidCfg->bPet)
		{
			foreach (auto battlePet, m_pGamePets)
			{
				if (battlePet && battlePet->exist && battlePet->default_battle) //??????????????????
				{
					double dFirstAidVal = battlePet->maxhp * m_pFirstAidCfg->dFirstAidHp / 100; //?????????
					if (battlePet->hp < dFirstAidVal)											//????????????
					{
						nFirstAidTarget = 1;
						selectName = pNewChar->name;
						selectSubName = battlePet->name;
					}
					break;
				}
			}
		}
		//??????
		if (nFirstAidTarget == -1 && m_pFirstAidCfg->bTeammate)
		{
			CGA::cga_team_players_t teamPlayers;
			g_CGAInterface->GetTeamPlayerInfo(teamPlayers);
			foreach (auto teamPlayer, teamPlayers)
			{
				double dFirstAidVal = teamPlayer.maxhp * m_pFirstAidCfg->dFirstAidHp / 100; //?????????
				if (teamPlayer.hp < dFirstAidVal)											//????????????
				{
					nFirstAidTarget = 2;
					selectName = pNewChar->name;
					selectSubName = QString::fromStdString(teamPlayer.name);
					break;
				}
			}
		}
		if (nFirstAidTarget == -1)
		{
			//		qDebug() << "??????????????????????????????";
			break;
			; //
		}
		bool bResult = false;
		m_pFirstAidCfg->lastUseSkill = QDateTime::currentMSecsSinceEpoch();
		m_pFirstAidCfg->selectName = selectName;
		m_pFirstAidCfg->selectSubName = selectSubName;
		m_nWorkType = TWork_FirstAid;
		m_pFirstAidCfg->bChecked = true;
		g_CGAInterface->StartWork(pFirstAidSkill->index, m_pFirstAidCfg->nLv, bResult);
		return true;
	} while (0);
	return false;
}
//????????????
bool GameCtrl::AutoHeal()
{
	if (!m_bAutoHeal)
		return false;
	if (QDateTime::currentMSecsSinceEpoch() < (m_pHealCfg->lastUseSkill + 2000)) //2????????????
		return false;
	m_pHealCfg->bSelf = true; //????????????  ????????????true ????????????
	//1???????????? ??????
	//if (!m_pHealCfg->bSelf && !m_pHealCfg->bPet && !m_pHealCfg->bTeammate)
	//{
	//	return false;
	//}
	GameSkillList pSkillList = m_pGameSkills;
	GameSkillPtr pSelectSkill = nullptr;
	foreach (auto pSkill, pSkillList)
	{
		if (pSkill->name == "??????")
		{
			pSelectSkill = pSkill;
			break;
		}
	}
	if (pSelectSkill == nullptr)
	{
		//	qDebug() << "??????????????????";
		return false;
	}
	//?????????????????? level  ????????????????????????maxLevel   cost????????? ?????????0  ??????????????????
	if (m_pHealCfg->nLv == -1) //??????
	{
		if (pSelectSkill->level > pSelectSkill->maxLevel)
		{ //?????????????????? maxLevel
			m_pHealCfg->nLv = pSelectSkill->maxLevel - 1;
		}
		else
		{
			m_pHealCfg->nLv = pSelectSkill->level - 1;
		}
	}
	if (m_pHealCfg->nLv > (pSelectSkill->maxLevel - 1)) //?????????????????? > ???????????????????????? ??????
	{													//?????????????????? maxLevel
														//		qDebug() << QString("?????????????????????%1").arg(pSelectSkill->maxLevel);
		return false;
	}
	if (pSelectSkill->subskills.size() < 1)
		return false;
	if (pSelectSkill->subskills.size() < (m_pHealCfg->nLv + 1))
		return false;

	m_pHealCfg->dCost = pSelectSkill->subskills.at(m_pHealCfg->nLv)->cost;
	//	qDebug() << QString("??????%1").arg(m_pHealCfg->dCost);
	auto pNewChar = g_pGameFun->GetGameCharacter();
	if (!pNewChar) return false;
	if (pNewChar->mp < m_pHealCfg->dCost)
	{
		//		qDebug() << "?????????";
		return false;
	}
	//????????????
	//??????
	//??????
	QString selectName, selectSubName;

	int nHealTarget = -1; //0?????? 1????????? 2??????
	//???????????? ???????????????????????? ?????????????????????
	//if (m_pHealCfg->bSelf)
	{
		if (pNewChar->health > 0 && pNewChar->health < m_pHealCfg->nHurtVal) //????????????
		{
			nHealTarget = 0;
			selectName = pNewChar->name;
			selectSubName = pNewChar->name;
		}
	}
	//??????
	if (nHealTarget == -1 && m_pHealCfg->bPet)
	{
		foreach (auto battlePet, m_pGamePets)
		{
			if (battlePet && battlePet->exist && battlePet->default_battle) //??????????????????
			{
				if (battlePet->health > 0 && battlePet->health < m_pHealCfg->nHurtVal) //??????
				{
					nHealTarget = 1;
					selectName = pNewChar->name;
					selectSubName = battlePet->name;
				}
				break;
			}
		}
	}
	//??????
	if (nHealTarget == -1 && m_pHealCfg->bTeammate)
	{
		CGA::cga_team_players_t teamPlayers; //????????? ???????????????????????????????????????????????????
		g_CGAInterface->GetTeamPlayerInfo(teamPlayers);
		CGA::cga_map_units_t units;
		g_CGAInterface->GetMapUnits(units);
		if (units.size() > 0)
		{
			foreach (auto teamPlayer, teamPlayers)
			{
				foreach (auto mapUnit, units)
				{
					if (mapUnit.xpos == teamPlayer.xpos && mapUnit.ypos == teamPlayer.ypos && mapUnit.unit_name == teamPlayer.name)
					{
						qDebug() << "?????????" << QString::fromStdString(teamPlayer.name) << mapUnit.injury;
						//0????????? ????????? ??????8???????????? ??????9  ????????????  ?????????????????????????????????????????????????????????????????? ???????????????????????????
						if (mapUnit.injury == 1 || mapUnit.injury == 3 || mapUnit.injury == 9) //injury?????? ????????????????????????????????? ????????????????????????????????????????????????????????????????????????????????????
						{
							nHealTarget = 2;
							selectName = QString::fromStdString(teamPlayer.name);
							selectSubName = selectName;
							break;
						}
					}
				}
				if (nHealTarget == 2)
				{
					break;
				}
			}
		}
	}
	if (nHealTarget == -1)
	{
		//qDebug() << "??????????????????????????????";
		m_pHealCfg->bChecked = false;
		return false; //
	}
	bool bResult = false;
	m_pHealCfg->lastUseSkill = QDateTime::currentMSecsSinceEpoch();
	m_pHealCfg->selectName = selectName;
	m_pHealCfg->selectSubName = selectSubName;
	m_nWorkType = TWork_Heal;
	m_pHealCfg->bChecked = true;
	g_CGAInterface->StartWork(pSelectSkill->index, m_pHealCfg->nLv, bResult);
	return true;
}

bool GameCtrl::AutoTransformation()
{
	if (!m_pTransformation->bChecked)
		return false;
	if (QDateTime::currentMSecsSinceEpoch() < (m_pTransformation->lastUseSkill + 2000)) //1????????????
		return false;
	GameSkillPtr pSelectSkill = nullptr;
	foreach (auto pSkill, m_pGameSkills)
	{
		if (pSkill->name == "??????")
		{
			pSelectSkill = pSkill;
			break;
		}
	}
	if (pSelectSkill == nullptr)
	{
		//	qDebug() << "??????????????????";
		return false;
	}
	if (pSelectSkill->level > pSelectSkill->maxLevel)
	{ //?????????????????? maxLevel
		m_pTransformation->nLv = pSelectSkill->maxLevel - 1;
	}
	else
	{
		m_pTransformation->nLv = pSelectSkill->level - 1;
	}
	if (m_pTransformation->nLv > (pSelectSkill->maxLevel - 1)) //?????????????????? > ???????????????????????? ??????
	{														   //?????????????????? maxLevel
															   //		qDebug() << QString("?????????????????????%1").arg(pSelectSkill->maxLevel);
		return false;
	}
	auto pNewChar = g_pGameFun->GetGameCharacter();
	if (!pNewChar) return false;
	if (pNewChar->mp < 10)
	{
		//		qDebug() << "?????????";
		return false;
	}

	bool bResult = false;
	m_pTransformation->lastUseSkill = QDateTime::currentMSecsSinceEpoch();
	m_pTransformation->selectName = pNewChar->name; //????????????????????????
	if (m_pTransformation->selectSubName.isEmpty())
	{ //???????????????
		foreach (auto battlePet, m_pGamePets)
		{
			if (battlePet && battlePet->exist) //??????????????????
			{
				m_pTransformation->selectSubName = battlePet->name;
				break;
			}
		}
	}
	m_nWorkType = TWork_Transformation;
	m_pTransformation->bChecked = true;
	g_CGAInterface->StartWork(pSelectSkill->index, m_pTransformation->nLv, bResult);
}

bool GameCtrl::AutoCosplay()
{
	if (!m_pCosplay->bChecked)
		return false;
	if (QDateTime::currentMSecsSinceEpoch() < (m_pCosplay->lastUseSkill + 2000)) //1????????????
		return false;
	GameSkillPtr pSelectSkill = nullptr;
	foreach (auto pSkill, m_pGameSkills)
	{
		if (pSkill->name == "??????")
		{
			pSelectSkill = pSkill;
			break;
		}
	}
	if (pSelectSkill == nullptr)
	{
		//	qDebug() << "??????????????????";
		return false;
	}
	if (pSelectSkill->level > pSelectSkill->maxLevel)
	{ //?????????????????? maxLevel
		m_pCosplay->nLv = pSelectSkill->maxLevel - 1;
	}
	else
	{
		m_pCosplay->nLv = pSelectSkill->level - 1;
	}
	if (m_pCosplay->nLv > (pSelectSkill->maxLevel - 1)) //?????????????????? > ???????????????????????? ??????
	{													//?????????????????? maxLevel
														//		qDebug() << QString("?????????????????????%1").arg(pSelectSkill->maxLevel);
		return false;
	}
	if (m_pGameCharacter->mp < 10)
	{
		//		qDebug() << "?????????";
		return false;
	}
	bool bResult = false;
	m_pCosplay->lastUseSkill = QDateTime::currentMSecsSinceEpoch();
	m_nWorkType = TWork_Cosplay;
	m_pCosplay->bChecked = true;
	g_CGAInterface->StartWork(pSelectSkill->index, m_pCosplay->nLv, bResult);
}

bool GameCtrl::AutoPetRiding()
{
	if (!m_pAutoPetRiding->bChecked)
		return false;
	if (m_pGameCharacter->petriding) //????????? ??????
		return false;
	//?????????????????? ????????????????????? ????????????5???????????????
	if (QDateTime::currentMSecsSinceEpoch() < (m_pAutoPetRiding->lastUseSkill + 5000)) //1????????????
		return false;
	GameSkillPtr pSelectSkill = nullptr;
	foreach (auto pSkill, m_pGameSkills)
	{
		if (pSkill->name == "??????")
		{
			pSelectSkill = pSkill;
			break;
		}
	}
	if (pSelectSkill == nullptr)
	{
		//	qDebug() << "??????????????????";
		return false;
	}
	if (pSelectSkill->level > pSelectSkill->maxLevel)
	{ //?????????????????? maxLevel
		m_pAutoPetRiding->nLv = pSelectSkill->maxLevel - 1;
	}
	else
	{
		m_pAutoPetRiding->nLv = pSelectSkill->level - 1;
	}
	if (m_pAutoPetRiding->nLv > (pSelectSkill->maxLevel - 1)) //?????????????????? > ???????????????????????? ??????
	{														  //?????????????????? maxLevel
															  //		qDebug() << QString("?????????????????????%1").arg(pSelectSkill->maxLevel);
		return false;
	}

	bool bResult = false;
	m_pAutoPetRiding->lastUseSkill = QDateTime::currentMSecsSinceEpoch();
	m_nWorkType = TWork_PetRiding;
	m_pAutoPetRiding->bChecked = true;
	g_CGAInterface->StartWork(pSelectSkill->index, m_pAutoPetRiding->nLv, bResult);
}

void GameCtrl::OverTimeCheck()
{
	if (!m_bOverTimeT)
		return;
	//1??????1??? ???????????????
	if (m_nLastOverTime.elapsed() > 60000)
	{
		m_nLastOverTime.restart();
		g_CGAInterface->SayWords(string(""), 0, 3, 1);
	}
}

bool GameCtrl::AutoSetBattlePet()
{
	bool bResult = false;
	bool bRecall = false;
	//??????????????????????????? ?????? 40
	int internalLoyalityVal = 35;
	for (size_t i = 0; i < m_pGamePets.size(); ++i)
	{
		auto pet = m_pGamePets.at(i);
		if (!pet->exist)
			continue;
		if (pet->battle_flags == TPET_STATE_BATTLE && pet->loyality < internalLoyalityVal)
		{
			qDebug() << "??????????????????35???????????????????????????????????????";
			g_CGAInterface->ChangePetState(i, TPET_STATE_NONE, bResult); //3?????? 2?????? 1?????? 16??????
			bRecall = true;
		}
	}
	for (size_t i = 0; i < m_pGamePets.size(); ++i)
	{
		auto pet = m_pGamePets.at(i);
		if (!pet->exist)
			continue;
		if (pet->battle_flags == TPET_STATE_BATTLE && m_pBattlePetCfg->bRecallLoyality && pet->loyality < m_pBattlePetCfg->nRecallLoyality)
		{
			g_CGAInterface->ChangePetState(i, TPET_STATE_NONE, bResult); //3?????? 2?????? 1?????? 16??????
			bRecall = true;
			continue;
		}
		if (pet->battle_flags == TPET_STATE_BATTLE && m_pBattlePetCfg->bRecallMp && pet->mp < m_pBattlePetCfg->nRecallMp)
		{
			g_CGAInterface->ChangePetState(i, TPET_STATE_READY, bResult); //3?????? 2?????? 1?????? 16??????
			continue;
		}
	}
	//????????????????????? ???????????????????????????????????????
	if (!bRecall && m_pBattlePetCfg->bSummonPet && m_pGameCharacter->petid == -1) //-1 ?????????
	{
		//?????? ??????index
		int nMaxVal = -1;
		int nIndex = -1;
		int nPetLoy = 0;
		if (m_pBattlePetCfg->nSummonPetType == TChangePet_HighestLv) //????????????????????????
		{
			for (int i = 0; i < m_pGamePets.size(); ++i)
			{
				auto pet = m_pGamePets.at(i);
				if (!pet->exist)
					continue;
				if (nMaxVal < pet->level)
				{
					nMaxVal = pet->level;
					nIndex = i;
					nPetLoy = pet->loyality;
				}
			}
		}
		else if (m_pBattlePetCfg->nSummonPetType == TChangePet_HighestHp)
		{
			for (int i = 0; i < m_pGamePets.size(); ++i)
			{
				auto pet = m_pGamePets.at(i);
				if (!pet->exist)
					continue;
				if (nMaxVal < pet->hp)
				{
					nMaxVal = pet->hp;
					nIndex = i;
					nPetLoy = pet->loyality;
				}
			}
		}
		else if (m_pBattlePetCfg->nSummonPetType == TChangePet_HighestMp)
		{
			for (int i = 0; i < m_pGamePets.size(); ++i)
			{
				auto pet = m_pGamePets.at(i);
				if (!pet->exist)
					continue;
				if (nMaxVal < pet->mp)
				{
					nMaxVal = pet->mp;
					nIndex = i;
					nPetLoy = pet->loyality;
				}
			}
		}
		else if (m_pBattlePetCfg->nSummonPetType == TChangePet_SpecialIndex0)
		{
			nIndex = 0;
			CGA::cga_pet_info_t spePet;
			g_CGAInterface->GetPetInfo(nIndex, spePet);
			nPetLoy = spePet.loyality;
		}
		else if (m_pBattlePetCfg->nSummonPetType == TChangePet_SpecialIndex1)
		{
			nIndex = 1;
			CGA::cga_pet_info_t spePet;
			g_CGAInterface->GetPetInfo(nIndex, spePet);
			nPetLoy = spePet.loyality;
		}
		else if (m_pBattlePetCfg->nSummonPetType == TChangePet_SpecialIndex2)
		{
			nIndex = 2;
			CGA::cga_pet_info_t spePet;
			g_CGAInterface->GetPetInfo(nIndex, spePet);
			nPetLoy = spePet.loyality;
		}
		else if (m_pBattlePetCfg->nSummonPetType == TChangePet_SpecialIndex3)
		{
			nIndex = 3;
			CGA::cga_pet_info_t spePet;
			g_CGAInterface->GetPetInfo(nIndex, spePet);
			nPetLoy = spePet.loyality;
		}
		else if (m_pBattlePetCfg->nSummonPetType == TChangePet_SpecialIndex4)
		{
			nIndex = 4;
			CGA::cga_pet_info_t spePet;
			g_CGAInterface->GetPetInfo(nIndex, spePet);
			nPetLoy = spePet.loyality;
		}
		if (nIndex != -1 && nPetLoy > internalLoyalityVal)
			g_CGAInterface->ChangePetState(nIndex, 2, bResult);
	}
	return true;
}

bool GameCtrl::AutoDropPet()
{
	if (!m_bAutoDropPet)
		return false;
	if (m_nLastAutoDropPetTime.elapsed() < 5000) return false;
	m_nLastAutoDropPetTime.restart();

	bool bResult = false;
	CGA::cga_pets_info_t petsinfo;
	if (!g_CGAInterface->GetPetsInfo(petsinfo))
		return false;
	bool bCheckRealName = m_bDropPetCheckItem.value(TDropPetType_RealName);
	QString sPetRealName = m_nDropPetCheckVal.value(TDropPetType_RealName).toString();
	for (auto it = m_bDropPetCheckItem.begin(); it != m_bDropPetCheckItem.end(); ++it)
	{
		if (it.value() && it.key() != TDropPetType_RealName) //?????? ??????  ??????????????? ?????????
		{
			for (size_t i = 0; i < petsinfo.size(); ++i)
			{
				CGA::cga_pet_info_t pet = petsinfo.at(i);
				if (bCheckRealName && pet.realname != sPetRealName.toStdString())
					continue;
				int nVal = m_nDropPetCheckVal.value(it.key()).toInt();

				//			qDebug() << QString("%1 %2 %3 %4 %5 %6").arg(i).arg(pet.flags).arg(pet.battle_flags).arg(QString::fromStdString(pet.name)).arg(pet.maxhp).arg(pet.index);

				bool bDrop = false;
				QString sDropPetText;
				if (it.key() == TDropPetType_Grade)
				{
					QList<int> nPetVal = GetPetVal(pet, it.key()).value<QList<int> >();
					if (nPetVal.size() == 3)
					{
						if (nPetVal[1] != nPetVal[2] && nPetVal[2] != -1)
							sDropPetText = QString("%1~%2").arg(nPetVal[1]).arg(nPetVal[2]);
						else
							sDropPetText = QString("%1").arg(nPetVal[0]);
						//20?????????
						if (nPetVal[0] > nVal && nPetVal[0] != 20 && pet.level == 1)
						{
							qDebug() << "??????:" << nPetVal[0] << nPetVal[1] << nPetVal[2] << " ????????????" << nVal;
							bDrop = true;
						}
					}
				}
				else
				{
					int nPetVal = GetPetVal(pet, it.key()).toInt();
					if (nPetVal >= 0 && nPetVal < nVal)
						bDrop = true;
				}
				if (bDrop)
				{
					QString sMsg = QString("??????Lv%1 %2 %3 ??????:%4").arg(pet.level).arg(QString::fromStdString(pet.realname)).arg(g_pGameFun->GetLogPetAttribute(pet)).arg(it.key() == TDropPetType_Grade ? sDropPetText : "--");
					//					qDebug() << QString("??????Lv:%0 %1 ???%2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 ?????????%14 %15").arg(pet.level).arg(QString::fromStdString(pet.realname)).arg(pet.maxhp).arg(pet.maxmp).arg(pet.detail.points_endurance).arg(pet.detail.points_strength).arg(pet.detail.points_defense).arg(pet.detail.points_agility).arg(pet.detail.points_magical).arg(pet.detail.value_attack).arg(pet.detail.value_defensive).arg(pet.detail.value_agility).arg(pet.detail.value_spirit).arg(pet.detail.value_recovery).arg(it.key() == TDropPetType_Grade ? nPetVal : "--");
					g_CGAInterface->ChangePetName(pet.index, sDropPetText.toStdString(), bResult);
					g_CGAInterface->DropPet(pet.index, bResult);
					emit g_pGameCtrl->signal_addOneLogMsg(sMsg);
					g_pGameFun->Chat(sMsg, 2, 3, 5);
				}
			}
		}
	}
	return true;
}

bool GameCtrl::AutoPickItems()
{
	if (!m_bAutoPickCfg.bPicked)
		return false;
	if (m_pPickItemList.size() < 1)
		return false;
	if (g_pGameFun->GetInventoryEmptySlotCount() < 1)
	{
		//	qDebug() << "??????????????????????????????";
		return false;
	}
	QStringList pickNameList;
	for (auto pItem : m_pPickItemList)
	{
		if (pItem->isPick)
		{
			pickNameList.append(pItem->name);
		}
	}
	QPoint curPos = g_pGameFun->GetMapCoordinate();
	CGA::cga_map_units_t units;
	if (g_CGAInterface->GetMapUnits(units))
	{
		for (auto unit : units)
		{
			//if (unit.valid && unit.type == 2 && unit.model_id != 0 && (unit.flags & 1024) != 0)
			if (unit.valid && unit.model_id != 0 && !(unit.flags & 4096) && !(unit.flags & 256))
			{
				QString sItemName = QString::fromStdString(unit.item_name);
				if (pickNameList.contains(sItemName) || sItemName.contains("??????"))
				{
					if (m_bAutoPickCfg.bFollow)
					{
						g_pGameFun->MoveToNpcNear(unit.xpos, unit.ypos);
					}
					curPos = g_pGameFun->GetMapCoordinate();
					if (g_pGameFun->IsNearTarget(curPos.x(), curPos.y(), unit.xpos, unit.ypos))
					{
						g_pGameFun->TurnAboutEx(unit.xpos, unit.ypos);
						m_bAutoPickCfg.lastPickTime = QDateTime::currentMSecsSinceEpoch();
					}
				}
			}
			if (g_pGameFun->GetInventoryEmptySlotCount() < 1)
			{
				//	qDebug() << "??????????????????????????????";
				return false;
			}
		}
	}
	if (m_bAutoPickCfg.bFixedPoint && m_bAutoPickCfg.east != 0 && m_bAutoPickCfg.south != 0)
	{
		if ((QDateTime::currentMSecsSinceEpoch() - m_bAutoPickCfg.lastPickTime) >= 10 * 1000)
		{
			//10???????????? ??????
			g_pGameFun->AutoMoveTo(m_bAutoPickCfg.east, m_bAutoPickCfg.south);
			m_bAutoPickCfg.lastPickTime = QDateTime::currentMSecsSinceEpoch();
		}
	}
}

bool GameCtrl::AutoUpgradeAddPoint()
{
	if (m_upgradePlayerCfg->bEnabled)
	{
		if (QDateTime::currentMSecsSinceEpoch() < (m_upgradePlayerCfg->lastCallTime + 2000)) //2????????????
			return false;
		CGA::cga_player_info_t info;
		if (g_CGAInterface->GetPlayerInfo(info))
		{
			if (info.detail.points_remain > 0)
			{
				int pointsRemain = info.detail.points_remain;
				//???????????????????????????????????????????????????
				GameUpgradeData upgradeCfg;
				if (m_upgradePlayerCfg->specialCfg.bEnabled)
				{
					memcpy(&upgradeCfg, &m_upgradePlayerCfg->specialCfg, sizeof(GameUpgradeData));
				}
				else if (m_upgradePlayerCfg->normalCfg.bEnabled)
				{
					memcpy(&upgradeCfg, &m_upgradePlayerCfg->normalCfg, sizeof(GameUpgradeData));
				}
				//?????????????????????????????????????????????????????????????????????????????????
				do
				{
					if (!upgradeCfg.bEnabled) //???????????? ??????
						break;
					//??????????????????????????????????????????333
					int maxPoint = 15 + (info.level - 1) * 2;
					if (upgradeCfg.bVigor) //???????????????
					{
						//??????????????????????????? ????????????
						if (info.detail.points_endurance < maxPoint && info.detail.points_endurance < upgradeCfg.vigorVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Vigor);
							break;
						}
					}
					if (upgradeCfg.bStrength) //????????????
					{
						//??????????????????????????? ????????????
						if (info.detail.points_strength < maxPoint && info.detail.points_strength < upgradeCfg.strengthVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Strength);
							break;
						}
					}
					if (upgradeCfg.bEndurance) //????????????
					{
						//??????????????????????????? ????????????
						if (info.detail.points_defense < maxPoint && info.detail.points_defense < upgradeCfg.enduranceVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Endurance);
							break;
						}
					}
					if (upgradeCfg.bAgility) //????????????
					{
						//??????????????????????????? ????????????
						if (info.detail.points_agility < maxPoint && info.detail.points_agility < upgradeCfg.agilityVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Agility);
							break;
						}
					}
					if (upgradeCfg.bMagical) //????????????
					{
						//??????????????????????????? ????????????
						if (info.detail.points_magical < maxPoint && info.detail.points_magical < upgradeCfg.magicalVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Magical);
							break;
						}
					}
				} while (0);
				m_upgradePlayerCfg->lastCallTime = QDateTime::currentMSecsSinceEpoch();
			}
		}
	}
	if (m_upgradePetCfg->bEnabled) //????????????????????????
	{
		if (QDateTime::currentMSecsSinceEpoch() < (m_upgradePetCfg->lastCallTime + 2000)) //2????????????
			return false;

		if (m_upgradePetCfg->normalCfg.bEnabled) //????????????
		{
			GamePetPtr battlePet = nullptr;
			if (m_upgradePetCfg->petName.isEmpty() && m_upgradePetCfg->petRealName.isEmpty()) //??????????????????
			{
				foreach (auto tmpPet, m_pGamePets)
				{
					if (tmpPet && tmpPet->exist && tmpPet->battle_flags == TPET_STATE_BATTLE) //??????????????????
					{
						battlePet = tmpPet;
						break;
					}
				}
			}
			else //??????????????????
			{
				foreach (auto tmpPet, m_pGamePets)
				{
					if (tmpPet && tmpPet->exist && tmpPet->battle_flags == TPET_STATE_BATTLE)
					{
						//????????????????????????????????????
						if (!m_upgradePetCfg->petName.isEmpty() && tmpPet->name != m_upgradePetCfg->petName)
							continue;
						if (!m_upgradePetCfg->petRealName.isEmpty() && tmpPet->realname != m_upgradePetCfg->petRealName)
							continue;
						//		qDebug() << m_upgradePetCfg->petName << m_upgradePetCfg->petRealName << tmpPet->index;
						battlePet = tmpPet;
						break;
					}
				}
			}
			if (!battlePet)
				return false;
			int pointVal = battlePet->detail.points_remain;
			if (pointVal <= 0) //???????????????????????????
				return false;
			//????????????????????????????????????????????????????????????????????????????????????????????????????????????
			int curVigor = battlePet->detail.points_endurance;
			int curStrength = battlePet->detail.points_strength;
			int curEndurance = battlePet->detail.points_defense;
			int curAgility = battlePet->detail.points_agility;
			int curMagical = battlePet->detail.points_magical;
			if (m_upgradePetCfg->normalCfg.addBpType == -1) //?????? ???????????? ????????????????????????
			{
				if (m_upgradePetCfg->normalCfg.vigorVal > 0 && battlePet->detail.points_endurance < m_upgradePetCfg->normalCfg.vigorVal && pointVal > 0)
				{
					qDebug() << "?????? ????????????-??????:";
					g_CGAInterface->UpgradePet(battlePet->index, 0); //??????
					pointVal--;
				}
				if (m_upgradePetCfg->normalCfg.strengthVal > 0 && battlePet->detail.points_strength < m_upgradePetCfg->normalCfg.strengthVal && pointVal > 0)
				{
					qDebug() << "?????? ????????????-??????:";
					g_CGAInterface->UpgradePet(battlePet->index, 1); //??????
					pointVal--;
				}
				if (m_upgradePetCfg->normalCfg.enduranceVal > 0 && battlePet->detail.points_defense < m_upgradePetCfg->normalCfg.enduranceVal && pointVal > 0)
				{
					qDebug() << "?????? ????????????-??????:";
					g_CGAInterface->UpgradePet(battlePet->index, 2); //??????
					pointVal--;
				}
				if (m_upgradePetCfg->normalCfg.agilityVal > 0 && battlePet->detail.points_agility < m_upgradePetCfg->normalCfg.agilityVal && pointVal > 0)
				{
					qDebug() << "?????? ????????????-??????:";
					g_CGAInterface->UpgradePet(battlePet->index, 3); //??????
					pointVal--;
				}
				if (m_upgradePetCfg->normalCfg.magicalVal > 0 && battlePet->detail.points_magical < m_upgradePetCfg->normalCfg.magicalVal && pointVal > 0)
				{
					qDebug() << "?????? ????????????-??????:";
					g_CGAInterface->UpgradePet(battlePet->index, 4); //??????
					pointVal--;
				}
			}
			else if (m_upgradePetCfg->normalCfg.addBpType >= 0 && m_upgradePetCfg->normalCfg.addBpType <= 4)
			{

				//???????????????????????? ??????2?????????
				qDebug() << "???????????????" << m_upgradePetCfg->normalCfg.addBpType << m_upgradeTypeText.value(m_upgradePetCfg->normalCfg.addBpType);
				g_CGAInterface->UpgradePet(battlePet->index, m_upgradePetCfg->normalCfg.addBpType);
				//?????????????????? ?????????????????????????????????????????????????????????????????????????????????????????????
				//??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
				if (m_upgradePetCfg->oldVigorVal == curVigor && m_upgradePetCfg->oldStrengthVal == curStrength && m_upgradePetCfg->oldEnduranceVal == curEndurance && m_upgradePetCfg->oldAgilityVal == curAgility && m_upgradePetCfg->oldMagicalVal == curMagical)
				{ //??????
					//???????????????
					m_upgradePetCfg->oldVigorVal = curVigor;
					m_upgradePetCfg->oldStrengthVal = curStrength;
					m_upgradePetCfg->oldEnduranceVal = curEndurance;
					m_upgradePetCfg->oldAgilityVal = curAgility;
					m_upgradePetCfg->oldMagicalVal = curMagical;
					if (m_upgradePetCfg->unnormalCfg.bEnabled) //????????????
					{
						qDebug() << "???????????????????????????" << m_upgradeTypeText.value(m_upgradePetCfg->unnormalCfg.addBpType);
						g_CGAInterface->UpgradePet(battlePet->index, m_upgradePetCfg->unnormalCfg.addBpType); //???????????????
					}
				}
				else
				{
					//???????????????
					m_upgradePetCfg->oldVigorVal = curVigor;
					m_upgradePetCfg->oldStrengthVal = curStrength;
					m_upgradePetCfg->oldEnduranceVal = curEndurance;
					m_upgradePetCfg->oldAgilityVal = curAgility;
					m_upgradePetCfg->oldMagicalVal = curMagical;
				}

				//
				//CGA::cga_pets_info_t petsinfo;
				//if (g_CGAInterface->GetPetsInfo(petsinfo))
				//{
				//	for (size_t i = 0; i < petsinfo.size(); ++i)
				//	{
				//		const CGA::cga_pet_info_t &petinfo = petsinfo.at(i);
				//		if (petinfo.index == battlePet->index)
				//		{
				//			if (petinfo.detail.points_remain > 0)
				//			{
				//				qDebug() << "???????????????????????????";
				//				if (m_upgradePetCfg->unnormalCfg.bEnabled) //????????????
				//				{
				//					g_CGAInterface->UpgradePet(battlePet->index, m_upgradePetCfg->unnormalCfg.addBpType); //???????????????
				//				}
				//			}
				//			break;
				//		}
				//	}
				//}
			}
		}
	}
	m_upgradePetCfg->lastCallTime = QDateTime::currentMSecsSinceEpoch();
	return true;
}

bool GameCtrl::IsRealUpdateUi()
{
	return m_bRealUpdateUi;
}

bool GameCtrl::IsEnabledDisplayDataUi()
{
	return m_bEnabledDataDisplayUi;
}
//??????????????? ??????
void GameCtrl::ResetRecvNpcDlgState()
{
	m_bHasNPCDlg = false;
}

bool GameCtrl::AutoEquipProtect()
{
	if (!m_pEquipProtectCfg->bChecked)
		return false;
	if (QDateTime::currentMSecsSinceEpoch() < (m_pEquipProtectCfg->lastUseSkill + 5000)) //5???????????????
		return false;
	//qDebug() << m_pEquipProtectCfg->nSwapEquip << m_pEquipProtectCfg->dVal << m_pEquipProtectCfg->bRenEquip << m_pEquipProtectCfg->bNoEquipOffLine;

	bool bRes = false;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);

			int nCur = 0, nMax = 0;
			g_pGameFun->ParseItemDurabilityEx(QString::fromStdString(iteminfo.attr), nCur, nMax);
			/*qDebug() << "name" << QString::fromStdString(iteminfo.name)
					 << "pos:" << iteminfo.pos << "????????????:" << nCur << "????????????" << nMax << " ?????????:" << nMax * 0.01 * m_pEquipProtectCfg->dVal;*/
			if (iteminfo.pos >= 0 && iteminfo.pos < 8 && nCur < nMax * 0.01 * m_pEquipProtectCfg->dVal)
			{
				QString sItemName = QString::fromStdString(iteminfo.name);
				if (m_sEquipProtectFilters.contains(sItemName)) //??????
					continue;

				if (m_pEquipProtectCfg->nSwapEquip == 0) //????????????
				{
					for (size_t n = 0; n < itemsinfo.size(); ++n)
					{
						const CGA::cga_item_info_t &iteminfo2 = itemsinfo.at(n);
						int nCur2 = 0, nMax2 = 0;
						g_pGameFun->ParseItemDurabilityEx(QString::fromStdString(iteminfo2.attr), nCur2, nMax2);
						if (iteminfo2.pos > 7 && iteminfo2.name == iteminfo.name && nCur2 > nMax2 * 0.01 * m_pEquipProtectCfg->dVal)
						{ //?????????????????????????????? ?????????????????? ????????? ???????????????
							g_CGAInterface->MoveItem(iteminfo2.pos, iteminfo.pos, -1, bRes);
							if (m_pEquipProtectCfg->bRenEquip) //??????????????????
							{
								g_CGAInterface->DropItem(iteminfo2.pos, bRes);
							}
							break;
						}
					}
				}
				else if (m_pEquipProtectCfg->nSwapEquip == 1) //????????????
				{
					for (size_t n = 0; n < itemsinfo.size(); ++n)
					{
						const CGA::cga_item_info_t &iteminfo2 = itemsinfo.at(n);
						int nCur2 = 0, nMax2 = 0;
						g_pGameFun->ParseItemDurabilityEx(QString::fromStdString(iteminfo2.attr), nCur2, nMax2);
						if (iteminfo2.pos > 7 && iteminfo2.type == iteminfo.type && nCur2 > nMax2 * 0.01 * m_pEquipProtectCfg->dVal)
						{ //?????????????????????????????? ?????????????????? ????????? ???????????????
							g_CGAInterface->MoveItem(iteminfo2.pos, iteminfo.pos, -1, bRes);
							if (m_pEquipProtectCfg->bRenEquip) //??????????????????
							{
								g_CGAInterface->DropItem(iteminfo2.pos, bRes);
							}
							break;
						}
					}
				}
				else if (m_pEquipProtectCfg->nSwapEquip == 2) //??????
				{
				}
				else if (m_pEquipProtectCfg->nSwapEquip == 3) //???id
				{
					for (size_t n = 0; n < itemsinfo.size(); ++n)
					{
						const CGA::cga_item_info_t &iteminfo2 = itemsinfo.at(n);
						int nCur2 = 0, nMax2 = 0;
						g_pGameFun->ParseItemDurabilityEx(QString::fromStdString(iteminfo2.attr), nCur2, nMax2);
						if (iteminfo2.pos > 7 && iteminfo2.itemid == iteminfo.itemid && nCur2 > nMax2 * 0.01 * m_pEquipProtectCfg->dVal)
						{ //?????????????????????????????? ?????????????????? ????????? ???????????????
							g_CGAInterface->MoveItem(iteminfo2.pos, iteminfo.pos, -1, bRes);
							if (m_pEquipProtectCfg->bRenEquip) //??????????????????
							{
								g_CGAInterface->DropItem(iteminfo2.pos, bRes);
							}
							break;
						}
					}
				}
			}
		}
	}
	m_pEquipProtectCfg->lastUseSkill = QDateTime::currentMSecsSinceEpoch();
	return true;
}

QSharedPointer<CGA_NPCDialog_t> GameCtrl::GetLastNpcDialog()
{
	if (m_npcDlgCache.size() < 1)
		return nullptr;
	QMutexLocker locker(&m_npcDlgMutex);
	auto lastData = m_npcDlgCache.last();
	if ((GetTickCount() - lastData.first) > 3000)
		return nullptr;
	else
		return lastData.second;
}
QSharedPointer<CGA::cga_trade_dialog_t> GameCtrl::GetLastTradeDialog(int timeInteral)
{
	if (m_tradeDlgCache.size() < 1)
		return nullptr;
	QMutexLocker locker(&m_tradeDlgMutex);
	auto lastData = m_tradeDlgCache.last();
	if ((GetTickCount() - lastData.first) > timeInteral)
		return nullptr;
	else
		return lastData.second;
}

QSharedPointer<CGA::cga_working_result_t> GameCtrl::GetLastWorkResult()
{
	if (m_workResCache.size() < 1)
		return nullptr;
	QMutexLocker locker(&m_workResMutex);
	auto lastData = m_workResCache.last();
	if ((GetTickCount() - lastData.first) > 3000)
		return nullptr;
	else
		return lastData.second;
}

QSharedPointer<CGA::cga_player_menu_items_t> GameCtrl::GetLastPlayerMenuResult()
{
	if (m_playerMenuCache.size() < 1)
		return nullptr;
	QMutexLocker locker(&m_playerMenuResMutex);
	auto lastData = m_playerMenuCache.last();
	if ((GetTickCount() - lastData.first) > 3000)
		return nullptr;
	else
		return lastData.second;
}

QSharedPointer<CGA::cga_unit_menu_items_t> GameCtrl::GetLastUnitMenuResult()
{
	if (m_unitMenuCache.size() < 1)
		return nullptr;
	QMutexLocker locker(&m_unitMenuResMutex);
	auto lastData = m_unitMenuCache.last();
	if ((GetTickCount() - lastData.first) > 3000)
		return nullptr;
	else
		return lastData.second;
}

QSharedPointer<CGA::cga_conn_state_t> GameCtrl::GetLastConnectStateResult()
{
	if (m_connectStateCache.size() < 1)
		return nullptr;
	QMutexLocker locker(&m_connectResMutex);
	auto lastData = m_connectStateCache.last();
	if ((GetTickCount() - lastData.first) > 3000)
		return nullptr;
	else
		return lastData.second;
}

QList<QSharedPointer<CGA::cga_trade_dialog_t> > GameCtrl::GetAllRecvTopicMsgList()
{
	QList<QSharedPointer<CGA::cga_trade_dialog_t> > retTradeDlgs;
	if (m_tradeDlgCache.size() < 1)
		return retTradeDlgs;
	QMutexLocker locker(&m_tradeDlgMutex);
	auto lastData = m_tradeDlgCache.last();
	for (auto tmpDlg:m_tradeDlgCache)
	{
		retTradeDlgs.append(tmpDlg.second);
	}
	return retTradeDlgs;
}

int GameCtrl::GetLastBattleActionResult()
{
	if (m_battleActionCache.size() < 1)
		return 0;
	QMutexLocker locker(&m_battleResMutex);
	auto lastData = m_battleActionCache.last();
	if ((GetTickCount() - lastData.first) > 3000)
		return 0;
	else
		return lastData.second;
}

QVariant GameCtrl::GetPetVal(CGA::cga_pet_info_t &pPet, int nVal)
{
	switch (nVal)
	{
		case TDropPetType_PLevel:
			return pPet.level;
			break;
		case TDropPetType_PHp:
			return pPet.maxhp;
			break;
		case TDropPetType_PMp:
			return pPet.maxmp;
			break;
		case TDropPetType_PVigor:
			return pPet.detail.points_endurance;
			break;
		case TDropPetType_PStrength:
			return pPet.detail.points_strength;
			break;
		case TDropPetType_PEndurance:
			return pPet.detail.points_defense;
			break;
		case TDropPetType_PAgility:
			return pPet.detail.points_agility;
			break;
		case TDropPetType_PMagical:
			return pPet.detail.points_magical;
			break;
		case TDropPetType_VAttack:
			return pPet.detail.value_attack;
			break;
		case TDropPetType_VDefensive:
			return pPet.detail.value_defensive;
			break;
		case TDropPetType_VAgility:
			return pPet.detail.value_agility;
			break;
		case TDropPetType_VSpirit:
			return pPet.detail.value_spirit;
			break;
		case TDropPetType_VRecovery:
			return pPet.detail.value_recovery;
			break;
		case TDropPetType_Grade:
		{
			QList<int> grade;
			QStringList inputData = g_pGameFun->GetPetCalcBpData(pPet);
			auto pCalcData = g_pGamePetCalc->ParseLine(inputData);
			if (pCalcData)
			{
				grade << pCalcData->lossMin << pCalcData->lossMin << pCalcData->lossMax;
				return QVariant::fromValue<QList<int> >(grade);
			}
			break;
		}
		default:
			break;
	}
	return 0;
}

bool GameCtrl::WaitThreadFini()
{
	qDebug() << "??????????????????";
	qDebug() << "...????????????????????????????????????";
	if (m_characterFuture.isRunning())
	{
		m_characterFuture.waitForFinished();
	}
	qDebug() << "...??????????????????????????????";

	if (m_itemsFuture.isRunning())
	{
		m_itemsFuture.waitForFinished();
	}
	qDebug() << "...??????????????????????????????";

	if (m_mapFuture.isRunning())
		m_mapFuture.waitForFinished();
	qDebug() << "...????????????????????????????????????";
	if (m_normalFuture.isRunning())
		m_normalFuture.waitForFinished();
	return true;
}

void GameCtrl::GetCharacterDataThread(GameCtrl *pThis)
{
	if (!pThis)
		return;
	int ingame = 0;
	int lastGameState = 0;
	while (!pThis->m_bExit)
	{
		pThis->OnGetCharacterData();
		/*if (g_CGAInterface->IsInGame(ingame))
		{
			if (lastGameState != ingame)
			{
				lastGameState = ingame;
				emit pThis->signal_gameIsOnline(ingame);
			}
		}*/
		pThis->OnGetTeamData();
		//??????????????????????????????
		if (pThis->m_lastUploadTime.elapsed() > 10 * 1000)
		{
			RpcSocketClient::getInstance().UploadGidData();
			if (pThis->m_bNeedUploadBank) //?????????????????????????????????????????????????????????false
			{
				RpcSocketClient::getInstance().UploadGidBankData();
				pThis->m_bNeedUploadBank = false;
			}
			pThis->m_lastUploadTime.restart();
		}
		else
			QThread::msleep(1000);
	}
}

void GameCtrl::GetItemDataThread(GameCtrl *pThis)
{
	if (!pThis)
		return;
	while (!pThis->m_bExit)
	{
		pThis->OnGetItemsData();
		QThread::msleep(1000);
	}
}

void GameCtrl::DownloadMapThread(GameCtrl *pThis)
{
	if (!pThis)
		return;
	while (!pThis->m_bExit)
	{
		pThis->OnQueueDownloadMap();
		pThis->OnGetMapData();
		QThread::msleep(100);
	}
}

void GameCtrl::NormalThread(GameCtrl *pThis)
{
	if (!pThis)
		return;
	int ingame = 0;
	while (!pThis->m_bExit)
	{
		if (!g_CGAInterface->IsConnected() || !g_CGAInterface->IsInGame(ingame) || !ingame)
		{
			QThread::msleep(2000);
			continue;
		}
		if (!g_pGameFun->IsInNormalState()) //?????????????????????
		{
			//?????????????????????????????????????????????????????????????????????????????????
			if (pThis->m_lastNormalState.elapsed() > 30 * 60 * 1000) //30?????? ????????? ???????????????30??????????????????????????????
			{
				qDebug() << "????????????????????????30????????????????????????!";
				g_pGameFun->LogoutServer();
				pThis->m_lastNormalState.restart();
			}
			QThread::msleep(1000);
			continue;
		}
		pThis->m_lastNormalState.restart();
		//???????????? ???????????????  ?????????????????????????????? ?????????????????? ????????????  ???????????????  ?????????????????????
		//if (!pThis->m_bHasNPCDlg && pThis->m_lastRecvNpcDlgTime.elapsed() > 30 * 60 * 1000) //30?????? ?????????????????????????????? ???????????????30??????????????????????????????
		//{
		//	qDebug() << "???????????????????????????30????????????????????????!";
		//	g_pGameFun->LogoutServer();
		//	pThis->m_lastRecvNpcDlgTime.restart();
		//	pThis->m_bHasNPCDlg = true; //??????????????????
		//	continue;
		//}

		//????????????
		pThis->AutoSetBattlePet();
		//?????????
		pThis->AutoDropPet();
		//?????????T??????
		pThis->OverTimeCheck();
		//????????????
		if (pThis->AutoHeal()) //???????????? ????????????????????????
		{
			continue;
		}
		//????????????
		if (pThis->AutoFirstAid())
		{
			continue;
		}
		//????????????
		if (pThis->AutoEquipProtect())
		{
		}
		//????????????
		if (pThis->AutoTransformation())
		{
			continue;
		}
		//????????????
		if (pThis->AutoCosplay())
		{
			continue;
		} //????????????
		if (pThis->AutoPetRiding())
		{
			continue;
		}
		//????????????
		if (pThis->AutoPickItems())
		{
			continue;
		} //??????????????????
		if (pThis->AutoUpgradeAddPoint())
		{
			continue;
		}

		//????????????????????????
		if (pThis->m_nRunScriptState == SCRIPT_CTRL_RUN && g_pGameFun->IsNeedStopScript())
		{
			emit pThis->signal_stopScriptRun();
			continue;
		}
		QThread::msleep(500);
	}
}

void GameCtrl::NotifyTimeoutThread(GameCtrl *pThis)
{
	if (!pThis)
		return;
	int ingame = 0;
	while (!pThis->m_bExit)
	{
		pThis->RemoveTimeoutCache(pThis->m_npcDlgMutex, pThis->m_npcDlgCache);
		pThis->RemoveTimeoutCache(pThis->m_workResMutex, pThis->m_workResCache);
		pThis->RemoveTimeoutCache(pThis->m_tradeDlgMutex, pThis->m_tradeDlgCache);
		pThis->RemoveTimeoutCache(pThis->m_battleResMutex, pThis->m_battleActionCache);
		pThis->RemoveTimeoutCache(pThis->m_playerMenuResMutex, pThis->m_playerMenuCache);
		pThis->RemoveTimeoutCache(pThis->m_unitMenuResMutex, pThis->m_unitMenuCache);
		pThis->RemoveTimeoutCache(pThis->m_connectResMutex, pThis->m_connectStateCache);
		/*{
			QMutexLocker locker(&pThis->m_npcDlgMutex);
			quint64 curTime = GetTickCount();
			for (auto it = pThis->m_npcDlgCache.begin(); it != pThis->m_npcDlgCache.end();)
			{
				if ((curTime - it->first) > 30000)
				{
					it = pThis->m_npcDlgCache.erase(it);
				}
				else
					++it;
			}
		}*/
		QThread::msleep(500);
	}
}

//void GameCtrl::EncounterProtect(GameCtrl* pThis)
//{
//	if (!pThis)
//		return;
//	while (!pThis->m_bExit)
//	{
//		if (!g_pGameFun->IsInNormalState())
//		{
//			QThread::msleep(500);
//			continue;
//		}
//		if (pThis->AutoHeal())//???????????? ????????????????????????
//		{
//			continue;
//		}
//		if (pThis->AutoFirstAid())
//		{
//			continue;
//		}
//
//		QThread::msleep(500);
//	}
//}

void GameCtrl::OnQueueDownloadMap()
{
	if (!m_IsDownloadingMap)
		return;
	return; //?????????????????? ?????????
	//	qDebug() << "OnQueueDownloadMap";
	int ingame = 0;
	bool connected = g_CGAInterface->IsConnected();
	if (!connected || !g_CGAInterface->IsInGame(ingame) || !ingame)
		return;

	g_CGAInterface->RequestDownloadMap(m_DownloadMapX, m_DownloadMapY, m_DownloadMapX + 24, m_DownloadMapY + 24);

	m_DownloadMapX += 24;

	if (m_DownloadMapX > m_DownloadMapXSize)
	{
		m_DownloadMapX = 0;
		m_DownloadMapY += 24;
	}

	if (m_DownloadMapY > m_DownloadMapYSize)
	{
		m_IsDownloadingMap = false;
	}
}
void GameCtrl::ClearRenItems()
{
	/*foreach (auto pItem, m_pRenItemList)
	{
		SafeDelete(pItem);
	}*/
	m_pRenItemList.clear();
}

void GameCtrl::ClearDieItems()
{
	//foreach (auto pItem, m_pPileItemList)
	//{
	//	SafeDelete(pItem);
	//}
	m_pPileItemList.clear();
}
void GameCtrl::ClearSaleItems()
{
	//foreach (auto pItem, m_pSaleItemList)
	//{
	//	SafeDelete(pItem);
	//}
	m_pSaleItemList.clear();
}

void GameCtrl::OnGetItemsData()
{
	int ingame = 0;
	if (g_CGAInterface->IsConnected() && g_CGAInterface->IsInGame(ingame) && ingame)
	{
		GameItemList newItemList;

		CGA::cga_items_info_t itemsinfo;
		QList<int> allExistPosList;
		if (g_CGAInterface->GetItemsInfo(itemsinfo))
		{
			for (size_t i = 0; i < itemsinfo.size(); ++i)
			{
				const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);

				auto item = GameItemPtr(new GameItem);
				allExistPosList.push_back(iteminfo.pos);
				item->name = QString::fromStdString(iteminfo.name);
				item->attr = QString::fromStdString(iteminfo.attr);
				item->info = QString::fromStdString(iteminfo.info);
				item->id = iteminfo.itemid;
				item->image_id = iteminfo.image_id;
				item->type = iteminfo.type;
				item->count = iteminfo.count;
				item->pos = iteminfo.pos;
				item->assessed = iteminfo.assessed;
				item->level = iteminfo.level;
				item->assess_flags = iteminfo.assess_flags;
				item->exist = true;
				g_pGameFun->ParseItemDurability(item);
				ITObjectDataMgr::getInstance().StoreServerItemData(item);
				//qDebug() << QString::fromStdString(iteminfo.name) << QString::fromStdString(iteminfo.attr) << iteminfo.itemid << iteminfo.type << iteminfo.count << iteminfo.pos << iteminfo.assessed << iteminfo.level;
				newItemList.append(item);
			}
			m_pGameItems = newItemList;
		}
		//??????????????? ????????????
		if (!itemsinfo.empty()) //pExistItems
		{
			if (g_pGameFun->IsInNormalState())
			{
				if (m_bAutoRenItems)
					RenItems();
				//???????????????????????????????????????
				if (m_bAutoDieItems /*&& !g_pGameFun->isMoveing()*/)
					DieItems();
				OnEatItem();
			}
		}
		emit NotifyGameItemsInfo(newItemList);
	}
}

void GameCtrl::OnNotifyGameWndKeyDown(unsigned int keyVal)
{
	//qDebug() << "????????????:" << keyVal;
	int mapKey = m_quickKeyMap.key(keyVal);
	bool bResult = false;
	switch (mapKey)
	{
		case TDef_Quick_Key_Logback:
			g_CGAInterface->LogBack();
			return;
		case TDef_Quick_Key_Logout:
			g_CGAInterface->LogOut();
			return;
		case TDef_Quick_Key_Cross:
		{
			int mouseDir = m_pGameCharacter->direction;
			g_CGAInterface->GetMouseOrientation(mouseDir);
			g_CGAInterface->ForceMove(mouseDir, true, bResult);
			return;
		}
		case TDef_Quick_Key_Trade: //???????????????????????? ?????????
		{
			g_CGAInterface->AddAllTradeItems(2); //???????????????
			/*	CGA::cga_sell_pets_t myPets;

			int myGold = 0;
			CGA::cga_sell_items_t cgaTradeItems;
			CGA::cga_items_info_t itemsinfo;
			if (g_CGAInterface->GetItemsInfo(itemsinfo))
			{
				for (size_t i = 0; i < itemsinfo.size(); ++i)
				{
					const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
					if (iteminfo.pos > 7 && (iteminfo.assess_flags & 1) == 1)
					{
						CGA::cga_sell_item_t tradeItem;
						tradeItem.count = iteminfo.count;
						tradeItem.itemid = iteminfo.itemid;
						tradeItem.itempos = iteminfo.pos;
						cgaTradeItems.push_back(tradeItem);
					}
				}
			}
			g_CGAInterface->TradeAddStuffs(cgaTradeItems, myPets, myGold);*/
			return;
		}
		case TDef_Quick_Key_TradeNoAccept: //???????????????????????? ?????????
		{
			g_CGAInterface->AddAllTradeItems(1);
			return;
		}
		case TDef_Quick_Key_SaveAll: //??????
		{
			g_pGameFun->SaveBagAllItemsToBank();
			return;
		}
		case TDef_Quick_Key_FetchAll: //?????? ???????????????20
		{
			g_pGameFun->WithdrawItemAllEx();
			return;
		}
		case TDef_Quick_Key_CallFz: //?????????????????????
		{
			emit signal_ctrl_app(3);
			break;
		}
		case TDef_Quick_Key_Encounter: //??????????????????
		{
			emit signal_switchAutoEncounterUI(!g_pGameFun->isEncounter());
			break;
		}
		default: break;
	}
}

quint32 GameCtrl::qNativeKeycode(quint32 key)
{
	switch (key)
	{
		case VK_ESCAPE:
			return Qt::Key_Escape;
		case VK_TAB:
		case Qt::Key_Backtab:
			return Qt::Key_Tab;
		case VK_BACK:
			return Qt::Key_Backspace;
		case VK_RETURN:
		case Qt::Key_Enter:
			return Qt::Key_Return;
		case VK_INSERT:
			return Qt::Key_Insert;
		case VK_DELETE:
			return Qt::Key_Delete;
		case VK_PAUSE:
			return Qt::Key_Pause;
		case VK_PRINT:
			return Qt::Key_Print;
		case VK_CLEAR:
			return Qt::Key_Clear;
		case VK_HOME:
			return Qt::Key_Home;
		case VK_END:
			return Qt::Key_End;
		case VK_LEFT:
			return Qt::Key_Left;
		case VK_UP:
			return Qt::Key_Up;
		case VK_RIGHT:
			return Qt::Key_Right;
		case VK_DOWN:
			return Qt::Key_Down;
		case VK_PRIOR:
			return Qt::Key_PageUp;
		case VK_NEXT:
			return Qt::Key_PageDown;
		case VK_F1:
			return Qt::Key_F1;
		case VK_F2:
			return Qt::Key_F2;
		case VK_F3:
			return Qt::Key_F3;
		case VK_F4:
			return Qt::Key_F4;
		case VK_F5:
			return Qt::Key_F5;
		case VK_F6:
			return Qt::Key_F6;
		case VK_F7:
			return Qt::Key_F7;
		case VK_F8:
			return Qt::Key_F8;
		case VK_F9:
			return Qt::Key_F9;
		case VK_F10:
			return Qt::Key_F10;
		case VK_F11:
			return Qt::Key_F11;
		case VK_F12:
			return Qt::Key_F12;
		case VK_F13:
			return Qt::Key_F13;
		case VK_F14:
			return Qt::Key_F14;
		case VK_F15:
			return Qt::Key_F15;
		case VK_F16:
			return Qt::Key_F16;
		case VK_F17:
			return Qt::Key_F17;
		case VK_F18:
			return Qt::Key_F18;
		case VK_F19:
			return Qt::Key_F19;
		case VK_F20:
			return Qt::Key_F20;
		case VK_F21:
			return Qt::Key_F21;
		case VK_F22:
			return Qt::Key_F22;
		case VK_F23:
			return Qt::Key_F23;
		case VK_F24:
			return Qt::Key_F24;
		case VK_SPACE:
			return Qt::Key_Space;
		case VK_MULTIPLY:
			return Qt::Key_Asterisk;
		case VK_ADD:
			return Qt::Key_Plus;
		case VK_SEPARATOR:
			return Qt::Key_Comma;
		case VK_SUBTRACT:
			return Qt::Key_Minus;
		case VK_DIVIDE:
			return Qt::Key_Slash;
		case VK_MEDIA_NEXT_TRACK:
			return Qt::Key_MediaNext;
		case VK_MEDIA_PREV_TRACK:
			return Qt::Key_MediaPrevious;
		case VK_MEDIA_PLAY_PAUSE:
			return Qt::Key_MediaPlay;
		case VK_MEDIA_STOP:
			return Qt::Key_MediaStop;
			// couldn't find those in VK_*
			//case Qt::Key_MediaLast:
			//case Qt::Key_MediaRecord:
		case VK_VOLUME_DOWN:
			return Qt::Key_VolumeDown;
		case VK_VOLUME_UP:
			return Qt::Key_VolumeUp;
		case VK_VOLUME_MUTE:
			return Qt::Key_VolumeMute;

			// numbers
		case Qt::Key_0:
		case Qt::Key_1:
		case Qt::Key_2:
		case Qt::Key_3:
		case Qt::Key_4:
		case Qt::Key_5:
		case Qt::Key_6:
		case Qt::Key_7:
		case Qt::Key_8:
		case Qt::Key_9:
			return key;

			// letters
		case Qt::Key_A:
		case Qt::Key_B:
		case Qt::Key_C:
		case Qt::Key_D:
		case Qt::Key_E:
		case Qt::Key_F:
		case Qt::Key_G:
		case Qt::Key_H:
		case Qt::Key_I:
		case Qt::Key_J:
		case Qt::Key_K:
		case Qt::Key_L:
		case Qt::Key_M:
		case Qt::Key_N:
		case Qt::Key_O:
		case Qt::Key_P:
		case Qt::Key_Q:
		case Qt::Key_R:
		case Qt::Key_S:
		case Qt::Key_T:
		case Qt::Key_U:
		case Qt::Key_V:
		case Qt::Key_W:
		case Qt::Key_X:
		case Qt::Key_Y:
		case Qt::Key_Z:
			return key;

		default:
			return 0;
	}
}

quint32 GameCtrl::nativeKeycode(Qt::Key key)
{
	switch (key)
	{
		case Qt::Key_Escape:
			return VK_ESCAPE;
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			return VK_TAB;
		case Qt::Key_Backspace:
			return VK_BACK;
		case Qt::Key_Return:
		case Qt::Key_Enter:
			return VK_RETURN;
		case Qt::Key_Insert:
			return VK_INSERT;
		case Qt::Key_Delete:
			return VK_DELETE;
		case Qt::Key_Pause:
			return VK_PAUSE;
		case Qt::Key_Print:
			return VK_PRINT;
		case Qt::Key_Clear:
			return VK_CLEAR;
		case Qt::Key_Home:
			return VK_HOME;
		case Qt::Key_End:
			return VK_END;
		case Qt::Key_Left:
			return VK_LEFT;
		case Qt::Key_Up:
			return VK_UP;
		case Qt::Key_Right:
			return VK_RIGHT;
		case Qt::Key_Down:
			return VK_DOWN;
		case Qt::Key_PageUp:
			return VK_PRIOR;
		case Qt::Key_PageDown:
			return VK_NEXT;
		case Qt::Key_F1:
			return VK_F1;
		case Qt::Key_F2:
			return VK_F2;
		case Qt::Key_F3:
			return VK_F3;
		case Qt::Key_F4:
			return VK_F4;
		case Qt::Key_F5:
			return VK_F5;
		case Qt::Key_F6:
			return VK_F6;
		case Qt::Key_F7:
			return VK_F7;
		case Qt::Key_F8:
			return VK_F8;
		case Qt::Key_F9:
			return VK_F9;
		case Qt::Key_F10:
			return VK_F10;
		case Qt::Key_F11:
			return VK_F11;
		case Qt::Key_F12:
			return VK_F12;
		case Qt::Key_F13:
			return VK_F13;
		case Qt::Key_F14:
			return VK_F14;
		case Qt::Key_F15:
			return VK_F15;
		case Qt::Key_F16:
			return VK_F16;
		case Qt::Key_F17:
			return VK_F17;
		case Qt::Key_F18:
			return VK_F18;
		case Qt::Key_F19:
			return VK_F19;
		case Qt::Key_F20:
			return VK_F20;
		case Qt::Key_F21:
			return VK_F21;
		case Qt::Key_F22:
			return VK_F22;
		case Qt::Key_F23:
			return VK_F23;
		case Qt::Key_F24:
			return VK_F24;
		case Qt::Key_Space:
			return VK_SPACE;
		case Qt::Key_Asterisk:
			return VK_MULTIPLY;
		case Qt::Key_Plus:
			return VK_ADD;
		case Qt::Key_Comma:
			return VK_SEPARATOR;
		case Qt::Key_Minus:
			return VK_SUBTRACT;
		case Qt::Key_Slash:
			return VK_DIVIDE;
		case Qt::Key_MediaNext:
			return VK_MEDIA_NEXT_TRACK;
		case Qt::Key_MediaPrevious:
			return VK_MEDIA_PREV_TRACK;
		case Qt::Key_MediaPlay:
			return VK_MEDIA_PLAY_PAUSE;
		case Qt::Key_MediaStop:
			return VK_MEDIA_STOP;
			// couldn't find those in VK_*
			//case Qt::Key_MediaLast:
			//case Qt::Key_MediaRecord:
		case Qt::Key_VolumeDown:
			return VK_VOLUME_DOWN;
		case Qt::Key_VolumeUp:
			return VK_VOLUME_UP;
		case Qt::Key_VolumeMute:
			return VK_VOLUME_MUTE;

			// numbers
		case Qt::Key_0:
		case Qt::Key_1:
		case Qt::Key_2:
		case Qt::Key_3:
		case Qt::Key_4:
		case Qt::Key_5:
		case Qt::Key_6:
		case Qt::Key_7:
		case Qt::Key_8:
		case Qt::Key_9:
			return key;

			// letters
		case Qt::Key_A:
		case Qt::Key_B:
		case Qt::Key_C:
		case Qt::Key_D:
		case Qt::Key_E:
		case Qt::Key_F:
		case Qt::Key_G:
		case Qt::Key_H:
		case Qt::Key_I:
		case Qt::Key_J:
		case Qt::Key_K:
		case Qt::Key_L:
		case Qt::Key_M:
		case Qt::Key_N:
		case Qt::Key_O:
		case Qt::Key_P:
		case Qt::Key_Q:
		case Qt::Key_R:
		case Qt::Key_S:
		case Qt::Key_T:
		case Qt::Key_U:
		case Qt::Key_V:
		case Qt::Key_W:
		case Qt::Key_X:
		case Qt::Key_Y:
		case Qt::Key_Z:
			return key;
		default:
			return 0;
	}
}

void GameCtrl::SetQuickKey(int val, QString sInputKey)
{
	QKeySequence shortcut(sInputKey);
	Qt::KeyboardModifiers allMods = Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier;
	Qt::Key key = shortcut.isEmpty() ? Qt::Key(0) : Qt::Key((shortcut[0] ^ allMods) & shortcut[0]);
	quint32 windowsKey = nativeKeycode(key);
	m_quickKeyMap.insert(val, windowsKey);
}

void GameCtrl::OnGetCharacterData()
{
	int ingame = 0;
	auto pNewChar = QSharedPointer<Character>(new Character);
	GamePetList newPetList;
	GameSkillList newSkillList;
	GameCompoundList newCompoundList;

	if (g_CGAInterface->IsConnected() && g_CGAInterface->IsInGame(ingame) && ingame)
	{
		//syncronize value
		g_CGAInterface->SetMoveSpeed(m_nMoveSpeed);
		//g_CGAInterface->SetWorkDelay(m_nWorkDelay);
		g_CGAInterface->SetWorkAcceleration(m_nWorkAcc);
		//g_CGAInterface->SetNoSwitchAnim(m_bNoSwitchAnim);

		CGA::cga_player_info_t info;
		if (g_CGAInterface->GetPlayerInfo(info))
		{
			pNewChar->name = QString::fromStdString(info.name);
			pNewChar->sGid = QString::fromStdString(info.gid);
			pNewChar->job = QString::fromStdString(info.job);
			pNewChar->level = info.level;
			pNewChar->player_index = info.player_index;
			pNewChar->gold = info.gold;
			pNewChar->petid = info.petid;
			if (pNewChar->hp == 0)
			{
				pNewChar->hp = info.hp;
				pNewChar->maxhp = info.maxhp;
				pNewChar->mp = info.mp;
				pNewChar->maxmp = info.maxmp;
			}
			else
			{
				if (!g_pGameFun->IsInBattle())
				{
					pNewChar->hp = info.hp;
					pNewChar->maxhp = info.maxhp;
					pNewChar->mp = info.mp;
					pNewChar->maxmp = info.maxmp;
				}
			}
			pNewChar->xp = info.xp;
			pNewChar->maxxp = info.maxxp;
			pNewChar->default_petid = info.petid;
			pNewChar->unitid = info.unitid;
			pNewChar->image_id = info.image_id;
			//	qDebug() << "Plyer Image iD" << info.image_id;
			pNewChar->petriding = info.petriding;
			//	qDebug() << "petriding" << info.petriding;

			pNewChar->score = info.score;
			pNewChar->skillslots = info.skillslots;
			pNewChar->use_title = info.use_title;
			pNewChar->avatar_id = info.avatar_id;
			pNewChar->punchclock = info.punchclock;
			pNewChar->usingpunchclock = info.usingpunchclock;
			pNewChar->health = info.health;
			pNewChar->souls = info.souls;
			pNewChar->direction = info.direction;
			pNewChar->manu_endurance = info.manu_endurance;
			pNewChar->manu_skillful = info.manu_skillful;
			pNewChar->manu_intelligence = info.manu_intelligence;
			pNewChar->value_charisma = info.value_charisma;
			pNewChar->titles.clear(); //??????????????? ?????????
			for (auto tmpTitle : info.titles)
			{
				pNewChar->titles.append(QString::fromStdString(tmpTitle));
			}
			for (auto title : pNewChar->titles)
			{
				if (g_pGameFun->m_sPrestigeMap.keys().contains(title))
				{
					pNewChar->prestige = title;
					break;
				}
			}
			memcpy(&pNewChar->detail, &info.detail, sizeof(GameAttrBase));
			pNewChar->persdesc.changeBits = info.persdesc.changeBits;
			pNewChar->persdesc.sellIcon = info.persdesc.sellIcon;
			pNewChar->persdesc.sellString = info.persdesc.sellString;
			pNewChar->persdesc.buyIcon = info.persdesc.buyIcon;
			pNewChar->persdesc.buyString = info.persdesc.buyString;
			pNewChar->persdesc.wantIcon = info.persdesc.wantIcon;
			pNewChar->persdesc.wantString = info.persdesc.wantString;
			pNewChar->persdesc.descString = info.persdesc.descString;
		}

		QList<int> existPetIndexs;
		CGA::cga_pets_info_t petsinfo;
		if (g_CGAInterface->GetPetsInfo(petsinfo))
		{
			for (size_t i = 0; i < petsinfo.size(); ++i)
			{
				const CGA::cga_pet_info_t &petinfo = petsinfo.at(i);
				auto pet = GamePetPtr(new GamePet);
				//auto pet = m_pGamePets.at(petinfo.index);

				existPetIndexs.append(petinfo.index);
				if (pet->level == 1 && (pet->maxhp != petinfo.maxhp || pet->maxmp != petinfo.maxmp || pet->realname != QString::fromStdString(petinfo.realname)))
				{
					pet->bCalcGrade = true;
				}
				pet->exist = true;
				pet->index = petinfo.index;
				pet->id = petinfo.index;
				pet->level = petinfo.level;
				pet->hp = petinfo.hp;
				pet->maxhp = petinfo.maxhp;
				pet->mp = petinfo.mp;
				pet->maxmp = petinfo.maxmp;
				pet->xp = petinfo.xp;
				pet->maxxp = petinfo.maxxp;
				pet->flags = petinfo.flags;
				pet->battle_flags = petinfo.battle_flags;
				pet->loyality = petinfo.loyality;
				pet->name = QString::fromStdString(petinfo.name);
				pet->realname = QString::fromStdString(petinfo.realname);
				pet->showname = pet->name.isEmpty() ? pet->realname : pet->name;
				pet->health = petinfo.health;
				pet->state = petinfo.state;

				if ((int)i == m_pGameCharacter->default_petid)
				{
					pet->default_battle = true;
				}
				memcpy(&pet->detail, &petinfo.detail, sizeof(GameAttrBase));

				CGA::cga_pet_skills_info_t skillsinfo;
				if (g_CGAInterface->GetPetSkillsInfo(pet->id, skillsinfo))
				{
					for (size_t j = 0; j < skillsinfo.size(); ++j)
					{
						auto skill = GameSkillPtr(new GameSkill);
						//auto skill = pet->skills.at(j);
						const CGA::cga_pet_skill_info_t &skinfo = skillsinfo.at(j);
						skill->index = (int)skinfo.index;
						skill->name = QString::fromStdString(skinfo.name);
						skill->info = QString::fromStdString(skinfo.info);
						skill->cost = skinfo.cost;
						skill->flags = skinfo.flags;
						skill->exist = true;
						pet->skills.append(skill);
					}
				}
				newPetList.append(pet);
			}
			m_pGamePets = newPetList;
		}
		QList<int> pSkillIndexList;
		CGA::cga_skills_info_t skillsinfo;
		if (g_CGAInterface->GetSkillsInfo(skillsinfo))
		{
			for (size_t i = 0; i < skillsinfo.size(); ++i)
			{
				const CGA::cga_skill_info_t &skinfo = skillsinfo.at(i);
				auto skill = GameSkillPtr(new GameSkill);
				//auto skill = m_pGameSkills.at(i);
				skill->id = skinfo.skill_id;
				skill->name = QString::fromStdString(skinfo.name);
				skill->level = skinfo.lv;
				skill->maxLevel = skinfo.maxlv;
				skill->xp = skinfo.xp;
				skill->maxxp = skinfo.maxxp;
				skill->index = skinfo.index;
				skill->pos = skinfo.pos;
				skill->exist = true;
				pSkillIndexList.append(skill->index);

				CGA::cga_subskills_info_t subskillsinfo;
				//????????????????????????
				CGA::cga_craft_info_t craftInfo;
				if (g_CGAInterface->GetSubSkillsInfo(skill->index, subskillsinfo))
				{
					for (size_t j = 0; j < subskillsinfo.size(); ++j)
					{
						auto subsk = GameSkillPtr(new GameSkill);
						//	auto subsk = skill->subskills.at(j);
						const CGA::cga_subskill_info_t &subskinfo = subskillsinfo.at(j);
						subsk->name = QString::fromStdString(subskinfo.name);
						subsk->info = QString::fromStdString(subskinfo.info);
						subsk->level = subskinfo.level;
						subsk->cost = subskinfo.cost;
						subsk->flags = subskinfo.flags;
						subsk->available = subskinfo.available;
						subsk->exist = true;
						skill->subskills.append(subsk);
					}
				}
				newSkillList.append(skill);
			}
			m_pGameSkills = newSkillList;
		}

		//????????????????????????
		CGA::cga_crafts_info_t myinfos;
		for (int i = 0; i < pSkillIndexList.size(); ++i)
		{
			g_CGAInterface->GetCraftsInfo(pSkillIndexList.at(i), myinfos);
			for (size_t j = 0; j < myinfos.size(); ++j)
			{
				auto pCompound = GameCompoundPtr(new GameCompound);
				//auto pCompound = m_pCompoundList.at(j);
				const CGA::cga_craft_info_t &craftInfo = myinfos.at(j);
				pCompound->id = craftInfo.id;
				pCompound->cost = craftInfo.cost;
				pCompound->level = craftInfo.level;
				pCompound->itemid = craftInfo.itemid;
				pCompound->index = craftInfo.index;
				pCompound->name = QString::fromStdString(craftInfo.name);
				pCompound->info = QString::fromStdString(craftInfo.info);
				pCompound->available = craftInfo.available;
				for (size_t n = 0; n < 5; ++n)
				{
					const CGA::cga_craft_material_t &mymat = craftInfo.materials[n];
					if (!mymat.name.empty())
					{
						pCompound->materials[n].count = mymat.count;
						pCompound->materials[n].itemid = mymat.itemid;
						pCompound->materials[n].name = QString::fromStdString(mymat.name);
					}
				}
				newCompoundList.append(pCompound);
			}
			m_pCompoundList = newCompoundList;
		}
		m_pGameCharacter = pNewChar; //??????????????? ???????????????????????????????????? ???gid
	}

	if (ingame)
	{
		emit NotifyGameCharacterInfo(pNewChar);
		emit signal_updateTrayToolTip(QString("%1 %2???").arg(pNewChar->name).arg(g_pGameFun->GetGameServerLine()));
	}
	else
	{
		emit NotifyGameCharacterInfo(m_pGameCharacter);
		emit signal_updateTrayToolTip(QString("%1 ??????").arg(m_pGameCharacter->name));
	}

	//emit NotifyGameItemsInfo(new);
	emit NotifyGamePetsInfo(newPetList);
	emit NotifyGameSkillsInfo(newSkillList);
	emit NotifyGameCompoundInfo(newCompoundList);
}

void GameCtrl::OnGetTeamData()
{
	if (m_isInBattle)
		return;
	if (!m_bRealUpdatePlayerUi)
	{
		if (m_lastUpdateTeamTime.elapsed() < 5000) //5?????????
			return;
	}
	auto teamPlayerDatas = g_pGameFun->GetTeamPlayers();
	emit NotifyTeamInfo(teamPlayerDatas);
	m_lastUpdateTeamTime.restart();
}
//???????????????msgpack???QSharePtr??????
void GameCtrl::OnGetMapData()
{
	int ingame = 0;
	//qDebug("OnNotifyBattleAction.");
	if (!g_CGAInterface->IsConnected() || !g_CGAInterface->IsInGame(ingame) || !ingame)
	{
		g_CGAInterface->SetImmediateDoneWork(false); //?????????????????? ??????????????? ????????????????????????
		return;
	}

	std::string mapName;
	float x = 0, y = 0;
	int index1 = 0, index2 = 0, mapIndex = 0;
	g_CGAInterface->GetMapXYFloat(x, y);
	std::string filemap;
	if (g_CGAInterface->GetMapIndex(index1, index2, mapIndex, filemap))
	{

		//???????????? ?????? ???????????????????????????????????? ?????????????????????
		g_CGAInterface->GetMapName(mapName);
		if (g_pGameFun->IsInNormalState() && index1 == 0) //0???????????? ??????????????? ???????????????
		{
			ITObjectDataMgr::getInstance().StoreServerMapData(QString::fromStdString(mapName), mapIndex);
		}
	}
	//??????????????? ??????????????????????????????????????????label?????????
	emit NotifyGetMapInfo(QString::fromStdString(mapName), index1, index2, mapIndex, (int)(x / 64.0f), (int)(y / 64.0f));
	if (!m_bMapIsVisible && !m_bEnabledDataDisplayUi) //????????? ???????????????????????????????????????????????? ????????????????????????
		return;

	QSharedPointer<CGA_MapCellData_t> mapcollision(new CGA_MapCellData_t);
	QSharedPointer<CGA_MapCellData_t> mapobject(new CGA_MapCellData_t);
	QSharedPointer<CGA_MapUnits_t> mapunits(new CGA_MapUnits_t);
	int st;
	if (g_CGAInterface->GetGameStatus(st) && st == 3 /*&& m_tabindex == 5*/)
	{
		CGA::cga_map_cells_t cells;
		if (g_CGAInterface->GetMapCollisionTable(true, cells))
		{
			mapcollision->xbottom = cells.x_bottom;
			mapcollision->ybottom = cells.y_bottom;
			mapcollision->xsize = cells.x_size;
			mapcollision->ysize = cells.y_size;
			mapcollision->cells = cells.cell;
			mapcollision->mapindex = mapIndex;
		}

		if (g_CGAInterface->GetMapObjectTable(true, cells))
		{
			mapobject->xbottom = cells.x_bottom;
			mapobject->ybottom = cells.y_bottom;
			mapobject->xsize = cells.x_size;
			mapobject->ysize = cells.y_size;
			mapobject->cells = cells.cell;
		}

		CGA::cga_map_units_t units;
		if (g_CGAInterface->GetMapUnits(units))
		{
			mapunits->resize((int)units.size());
			for (int i = 0; i < mapunits->size(); ++i)
			{
				(*mapunits)[i].valid = units[i].valid;
				(*mapunits)[i].type = units[i].type;
				(*mapunits)[i].unit_id = units[i].unit_id;
				(*mapunits)[i].model_id = units[i].model_id;
				(*mapunits)[i].xpos = units[i].xpos;
				(*mapunits)[i].ypos = units[i].ypos;
				(*mapunits)[i].item_count = units[i].item_count;
				(*mapunits)[i].injury = units[i].injury;
				(*mapunits)[i].level = units[i].level;
				(*mapunits)[i].flags = units[i].flags;
				(*mapunits)[i].icon = units[i].icon;
				(*mapunits)[i].unit_name = QString::fromStdString(units[i].unit_name);
				(*mapunits)[i].nick_name = QString::fromStdString(units[i].nick_name);
				(*mapunits)[i].title_name = QString::fromStdString(units[i].title_name);
				(*mapunits)[i].item_name = QString::fromStdString(units[i].item_name);
			}
		}
	}
	emit NotifyGetMapCellInfo(mapcollision, mapobject, mapunits);
}

void GameCtrl::OnNotifyBattleAction(int flags)
{
	qDebug() << ("OnNotifyBattleAction.") << flags << "m_bIsSkillPerformed";
	if (flags & FL_BATTLE_ACTION_BEGIN)
		m_isInBattle = true;
	else if (flags & FL_BATTLE_ACTION_END)
	{
		emit signal_battleStateEnd();
		m_isInBattle = false;
	}
	//QTime espTime;
	//espTime = QTime::currentTime();
	//espTime.restart();
	//qDebug() << espTime.currentTime();
	int ingame = 0;
	if (!g_CGAInterface->IsConnected() || !g_CGAInterface->IsInGame(ingame) || !ingame)
		return;

	GetBattleUnits();

	//CBattleWorker::getInstace()->OnNotifyGetSkillsInfo(m_pGameSkills);	//?????????????????? ?????????????????????
	//CBattleWorker::getInstace()->OnNotifyGetPetsInfo(m_pGamePets);		//?????????????????? ?????????????????????
	//CBattleWorker::getInstace()->OnNotifyGetItemsInfo(m_pGameItems);	//?????????????????? ?????????????????????
	//CBattleWorker::getInstace()->OnNotifyGetItemsInfo(m_pGameItems);	//?????????????????? ?????????????????????
	CBattleWorker::getInstace()->OnBattleAction(flags);

	/*  ???1????????????????????????????????????????????????????????????
			???2?????????1????????????Boos??????????????????
			???3????????????????????????
			???4?????????1?????????????????????
			???5???????????????????????????
			???6?????????????????????????????????
			???7?????????????????????????????????
			???8????????????1???????????????????????????
			???9????????????1??????????????????????????????
			???10?????????????????????????????????
			???11?????????????????????????????????????????????
			???12?????????????????????????????????
			???13?????????????????????????????????????????????
			???14?????????????????????????????????
			???15??????10??????????????????????????????*/
	//qDebug() << "?????????????????????" <<  espTime.elapsed();
}
//???????????? 1 2???????????? 10000???NewUI_ShowLostConnectionDialog??? 0????????????????????? ???????????? 3????????????
void GameCtrl::OnNotifyConnectionState(int state, QString msg)
{
	m_lastGameConnState = state;
	m_lastGameConnMsg = msg;

	qDebug() << state << msg;
	if ((state == 10000 || state == 0) && !msg.isEmpty())
	{
		if (msg.contains("????????????????????????")) //??????id??????
			qDebug() << "???????????????????????????????????????????????????";
		m_isInBattle = false;
	}
	else if (state == 1 || state == 2)
	{
		//????????????
		qDebug() << "????????????";
		m_isInBattle = false;
	}
	else if (state == 3)
	{
		qDebug() << "?????????????????????";
	}
}

void GameCtrl::NotifyChatMsgCallback(CGA::cga_chat_msg_t msg)
{
	//unitid == -1  ????????????
	NotifyChatMsg(msg.unitid, QString::fromStdString(msg.msg), msg.size, msg.color);
}

void GameCtrl::NotifyNPCDialogCallback(CGA::cga_npc_dialog_t npcdlg)
{
	qDebug() << "NPCDlg Type:" << npcdlg.type << " Msg:" << QString::fromStdString(npcdlg.message) << "options:" << npcdlg.options;
	QSharedPointer<CGA_NPCDialog_t> dlg = QSharedPointer<CGA_NPCDialog_t>(new CGA_NPCDialog_t);
	m_bHasNPCDlg = true;
	dlg->type = npcdlg.type;
	dlg->options = npcdlg.options;
	dlg->dialog_id = npcdlg.dialog_id;
	dlg->npc_id = npcdlg.npc_id;
	dlg->message = QString::fromStdString(npcdlg.message);
	QMutexLocker locker(&m_npcDlgMutex);
	m_npcDlgCache.append(qMakePair(GetTickCount(), dlg));
	//????????????????????????????????????????????????????????????????????????????????????????????????????????????
	if (m_nRunScriptState == SCRIPT_CTRL_RUN)
	{
		//	emit g_pGameCtrl->DelaySendNpcDialog(dlg);

		//		qDebug() << "NotifyNPCDialogCallback";
		Sleep(m_nScriptDelayTime + 100);
		emit g_pGameCtrl->NotifyNPCDialog(dlg); //?????????????????????
												/*QTimer *timer = new QTimer(this);
		timer->setSingleShot(true);
		connect(timer, &QTimer::timeout, [&]() {
			emit NotifyNPCDialog(dlg);
			timer->deleteLater();
		});			
		timer->start(1000);*/
												/*QTimer::singleShot(	m_nScriptDelayTime, [&]() 
		{
			emit NotifyNPCDialog(dlg);
		});		*/
	}
	else
		emit g_pGameCtrl->NotifyNPCDialog(dlg); //?????????????????????
}

void GameCtrl::NotifyPlayerMenuCallback(CGA::cga_player_menu_items_t items)
{
	QSharedPointer<CGA::cga_player_menu_items_t> menu(new CGA::cga_player_menu_items_t);
	for (int i = 0; i < items.size(); ++i)
	{
		menu->push_back(items[i]);
		qDebug() << QString::fromStdString(items[i].name) << items[i].index << items[i].color;
	}
	//*menu = items;
	QMutexLocker locker(&m_playerMenuResMutex);
	m_playerMenuCache.append(qMakePair(GetTickCount(), menu));
	emit NotifyPlayerMenu(menu);
}
//??????????????????
void GameCtrl::NotifyWorkingResultCallback(CGA::cga_working_result_t msg)
{
	QSharedPointer<CGA::cga_working_result_t> working(new CGA::cga_working_result_t);
	*working = msg;
	//working->count = msg.count;
	//working->type=msg.type;
	//working->success=msg.success;
	//working->levelup=msg.levelup;
	//working->xp=msg.xp;
	//working->count=msg.count;
	//working->endurance=msg.endurance;
	//working->skillful=msg.skillful;
	//working->intelligence=msg.intelligence;
	//working->name=msg.name;

	QMutexLocker locker(&m_workResMutex);
	m_workResCache.append(qMakePair(GetTickCount(), working));
	//qDebug() << "NotifyWorkingResultCallback" << working->count << working->endurance << working->imgid
	//		 << working->intelligence << working->levelup << working->name.c_str() << working->skillful << working->status
	//		 << working->success << working->type << working->unk << working->value << working->value2 << working->xp;
	////????????????working->success ????????????type ????????????name ??????????????????levelup ???????????????imgid status
	NotifyWorkingResult(working);
}

void GameCtrl::NotifyServerShutdown(int port)
{
	qDebug() << "NotifyServerShutdown" << port;
	Disconnect();
}

void GameCtrl::NotifyTradeStuffsCallback(CGA::cga_trade_stuff_info_t tradeInfo)
{
	QSharedPointer<CGA::cga_trade_stuff_info_t> notifyItems(new CGA::cga_trade_stuff_info_t);
	notifyItems->gold = tradeInfo.gold;
	notifyItems->type = tradeInfo.type;
	notifyItems->pet = tradeInfo.pet;
	notifyItems->petskills = tradeInfo.petskills;
	notifyItems->items = tradeInfo.items;
	//????????????????????????????????????????????????????????????????????????????????????????????????????????????
	if (m_nRunScriptState == SCRIPT_CTRL_RUN)
	{
		Sleep(m_nScriptDelayTime + 100);
		emit NotifyTradeStuffs(notifyItems); //?????????????????????
	}
	else
		emit NotifyTradeStuffs(notifyItems); //?????????????????????
	//test
	qDebug() << "?????????" << tradeInfo.gold << "type" << tradeInfo.type << "pet" << QString::fromStdString(tradeInfo.pet.realname) << " name:" << QString::fromStdString(tradeInfo.pet.name);
	qDebug() << "???????????????indx " << tradeInfo.petskills.index;
	for (auto skillName : tradeInfo.petskills.skills)
	{
		qDebug() << "?????????" << QString::fromStdString(skillName);
	}

	for (auto item : tradeInfo.items)
	{
		qDebug() << "???????????????" << QString::fromStdString(item.name) << " ?????????" << item.count;
	}
}

void GameCtrl::NotifyTradeDialogCallback(CGA::cga_trade_dialog_t dlg)
{
	qDebug() << "NotifyTradeDialogCallback" << QString::fromStdString(dlg.name) << dlg.level;
	QSharedPointer<CGA::cga_trade_dialog_t> notifydlg(new CGA::cga_trade_dialog_t);
	notifydlg->name = dlg.name;
	notifydlg->level = dlg.level;
	QMutexLocker locker(&m_tradeDlgMutex);
	m_tradeDlgCache.append(qMakePair(GetTickCount(), notifydlg));
	//????????????????????????????????????????????????????????????????????????????????????????????????????????????
	if (m_nRunScriptState == SCRIPT_CTRL_RUN)
	{
		Sleep(m_nScriptDelayTime + 200);
		emit NotifyTradeDialog(notifydlg); //?????????????????????
	}
	else
		emit NotifyTradeDialog(notifydlg); //?????????????????????
}

void GameCtrl::NotifyTradeStateCallback(int state)
{
	qDebug() << "???????????? " << state;
	if (m_nRunScriptState == SCRIPT_CTRL_RUN)
	{
		Sleep(m_nScriptDelayTime + 100);
		emit NotifyTradeState(state); //?????????????????????
	}
	else
		emit NotifyTradeState(state); //?????????????????????
}

void GameCtrl::NotifyBattleActionCallBack(int flags)
{
	{
		QMutexLocker locker(&m_battleResMutex);
		m_battleActionCache.append(qMakePair(GetTickCount(), flags));
	}
	emit NotifyBattleAction(flags);
}

void GameCtrl::NotifyUnitMenuCallback(CGA::cga_unit_menu_items_t items)
{
	QSharedPointer<CGA::cga_unit_menu_items_t> menu(new CGA::cga_unit_menu_items_t);
	for (int i = 0; i < items.size(); ++i)
	{
		menu->push_back(items[i]);
	}
	//*menu = items;

	//	qDebug() << "NotifyUnitMenuCallback" << items.size() << menu->size();
	QMutexLocker locker(&m_unitMenuResMutex);
	m_unitMenuCache.append(qMakePair(GetTickCount(), menu));
	emit NotifyUnitMenu(menu);
}

void GameCtrl::NotifyConnectionStateCallback(CGA::cga_conn_state_t msg)
{
	QSharedPointer<CGA::cga_conn_state_t> connState(new CGA::cga_conn_state_t);
	connState->msg = msg.msg;
	connState->state = msg.state;
	QMutexLocker locker(&m_connectResMutex);
	m_connectStateCache.append(qMakePair(GetTickCount(), connState));
	NotifyConnectionState(msg.state, QString::fromStdString(msg.msg));
}

void GameCtrl::NotifyDownloadMapCallback(CGA::cga_download_map_t down)
{
	int st;
	if (g_CGAInterface->GetGameStatus(st) && st == 3 /*&& m_tabindex == 5*/)
	{
		QSharedPointer<CGA_MapCellData_t> mapcollision(new CGA_MapCellData_t);
		QSharedPointer<CGA_MapCellData_t> mapobject(new CGA_MapCellData_t);
		QSharedPointer<CGA_MapUnits_t> mapunits(new CGA_MapUnits_t);

		CGA::cga_map_cells_t cells;
		if (g_CGAInterface->GetMapCollisionTable(true, cells))
		{
			mapcollision->xbottom = cells.x_bottom;
			mapcollision->ybottom = cells.y_bottom;
			mapcollision->xsize = cells.x_size;
			mapcollision->ysize = cells.y_size;
			mapcollision->cells = cells.cell;
			int index1, index2;
			std::string filemap;
			g_CGAInterface->GetMapIndex(index1, index2, mapcollision->mapindex, filemap);
		}

		if (g_CGAInterface->GetMapObjectTable(true, cells))
		{
			mapobject->xbottom = cells.x_bottom;
			mapobject->ybottom = cells.y_bottom;
			mapobject->xsize = cells.x_size;
			mapobject->ysize = cells.y_size;
			mapobject->cells = cells.cell;
		}

		CGA::cga_map_units_t units;
		if (g_CGAInterface->GetMapUnits(units))
		{
			mapunits->resize((int)units.size());
			for (int i = 0; i < mapunits->size(); ++i)
			{
				(*mapunits)[i].valid = units[i].valid;
				(*mapunits)[i].type = units[i].type;
				(*mapunits)[i].unit_id = units[i].unit_id;
				(*mapunits)[i].model_id = units[i].model_id;
				(*mapunits)[i].xpos = units[i].xpos;
				(*mapunits)[i].ypos = units[i].ypos;
				(*mapunits)[i].item_count = units[i].item_count;
				(*mapunits)[i].injury = units[i].injury;
				(*mapunits)[i].level = units[i].level;
				(*mapunits)[i].flags = units[i].flags;
				(*mapunits)[i].unit_name = QString::fromStdString(units[i].unit_name);
				(*mapunits)[i].nick_name = QString::fromStdString(units[i].nick_name);
				(*mapunits)[i].title_name = QString::fromStdString(units[i].title_name);
				(*mapunits)[i].item_name = QString::fromStdString(units[i].item_name);
			}
		}

		NotifyGetMapCellInfo(mapcollision, mapobject, mapunits);
		NotifyRefreshMapRegion(down.xbase, down.ybase, down.xtop, down.ytop, down.index3);
	}
}

void GameCtrl::OnNotifyNPCDialog(const QSharedPointer<CGA_NPCDialog_t> &dlg)
{
	//???????????????
	if (m_bAutoSupply)
	{
		if (dlg->type == 2 && dlg->message.indexOf(m_NurseMessage) >= 0)
		{
			m_NurseNPCId = dlg->npc_id;
			bool result = false;
			CGA::cga_player_info_t playerinfo;
			CGA::cga_pets_info_t petsinfo;
			if (g_CGAInterface->GetPlayerInfo(playerinfo))
			{
				bool bNeedHP = g_pGameFun->NeedHPSupply(playerinfo);
				bool bNeedMP = g_pGameFun->NeedMPSupply(playerinfo);
				//??????????????????<????????? ?????????
				if (bNeedHP && (!bNeedMP || playerinfo.gold < playerinfo.maxmp - playerinfo.mp))
				{
					g_CGAInterface->ClickNPCDialog(0, 2, result);
				}
				else if (bNeedMP && playerinfo.gold >= playerinfo.maxmp - playerinfo.mp) //???????????? ???????????????
				{
					g_CGAInterface->ClickNPCDialog(0, 0, result);
				}
			}
			if (!result && g_CGAInterface->GetPetsInfo(petsinfo))
			{
				if (g_pGameFun->NeedPetSupply(petsinfo)) //????????????
				{
					g_CGAInterface->ClickNPCDialog(0, 4, result);
				}
			}
		}
		else if (dlg->npc_id == m_NurseNPCId && dlg->type == 0)
		{
			if (dlg->options == 12)
			{
				bool result = false;
				g_CGAInterface->ClickNPCDialog(4, -1, result); //4 ??? 8??? 32????????? 1??????
			}
			if (dlg->options == 1)
			{
				bool result = false;
				g_CGAInterface->ClickNPCDialog(1, -1, result);
			}
		}
	}
	if (m_bAutoSale)
	{
		if (dlg->type == 5 || dlg->type == 7)
		{
			// ??????????????? ??????NPCID=7962 DLGID=333 334 --1112
			//	qDebug() <<  dlg->npc_id << dlg->dialog_id << dlg->options << dlg->type << dlg->message;
			GameItemList pSaleItemList = g_pGameCtrl->GetSaleItemList();
			GameItemList pItemList = m_pGameItems;
			CGA::cga_sell_items_t cgaSaleItems;
			GameItemList pExistItemList; //????????????
			foreach (auto pSaleItem, pSaleItemList)
			{
				if (pSaleItem->isSale)
				{
					foreach (auto pItem, pItemList) //??????????????????
					{
						if (pItem->exist && pItem->pos > 7 && pItem->name == pSaleItem->name)
						{
							if (pExistItemList.contains(pItem))
								continue;
							CGA::cga_sell_item_t cgaItem;
							auto pDBItem = ITObjectDataMgr::getInstance().FindItemFromCode(pItem->id);
							if (pDBItem)
							{
								int nMinNum = pDBItem->_sellMinCount;
								if (nMinNum <= 1)
									cgaItem.count = 1;
								else
								{
									int tmpCount = pItem->count / nMinNum;
									if (tmpCount < 1) //???????????? ???????????????????????? ???????????? ??????
										continue;
									cgaItem.count = tmpCount;
								}
							}
							else
							{
								cgaItem.count = 1;
							}
							cgaItem.itemid = pItem->id;
							cgaItem.itempos = pItem->pos;
							cgaSaleItems.push_back(cgaItem);
							pExistItemList.push_back(pItem);
						}
					}
				}
			}
			//qDebug() << cgaSaleItems.size();
			if (cgaSaleItems.size() < 1)
			{
				//		qDebug() << "GameCtrl ????????????????????????";
				return;
			}
			if (dlg->type == 5 /*&& dlg->message.indexOf("???") >= 0*/)
			{
				m_saleNpcID = dlg->npc_id;
				bool bResult = false;
				g_CGAInterface->ClickNPCDialog(-1, dlg->message.at(dlg->message.size() - 1) == "3" ? 1 : 0, bResult);
			}
			else if (dlg->npc_id == m_saleNpcID && dlg->type == 7)
			{
				bool bResult = false;
				g_CGAInterface->SellNPCStore(cgaSaleItems, bResult);
			}
		}
	}
	if (dlg->type == 0 && m_bAutoClickNpc)
	{
		int selectVal = 4;
		if (m_bAutoTalkNpcYesOrNo)
			selectVal = 4;
		else
			selectVal = 8;
		bool result = false;
		switch (dlg->options)
		{
			case 12: g_CGAInterface->ClickNPCDialog(selectVal, -1, result); break;
			case 32: g_CGAInterface->ClickNPCDialog(32, -1, result); break;
			case 1: g_CGAInterface->ClickNPCDialog(1, -1, result); break;
			case 2: g_CGAInterface->ClickNPCDialog(2, -1, result); break;
			case 3: g_CGAInterface->ClickNPCDialog(1, -1, result); break;
			case 4: g_CGAInterface->ClickNPCDialog(4, -1, result); break;
			case 8: g_CGAInterface->ClickNPCDialog(8, -1, result); break;
			case 0: break;
			default: break;
		}
	}
	if (dlg->type == 23 /* && m_bAutoUpLoadBankData*/) //??????????????????
	{
		m_bNeedUploadBank = true; //????????? ????????????
								  //	RpcSocketClient::getInstance().UploadGidBankData();
	}
}

void GameCtrl::OnNotifyPlayerMenu(QSharedPointer<CGA::cga_player_menu_items_t> menu)
{
	bool bResult = false;
	do
	{
		if (m_nWorkType == TWork_Heal || m_nWorkType == TWork_FirstAid) //?????? ????????????
		{
			QString szSelectName;
			if (m_bAutoHeal && m_nWorkType == TWork_Heal && m_pHealCfg->bChecked)
				szSelectName = m_pHealCfg->selectName;
			else if (m_bAutoFirstAid && m_nWorkType == TWork_FirstAid && m_pFirstAidCfg->bChecked)
				szSelectName = m_pFirstAidCfg->selectName;
			else
				break;
			if (menu && menu->size() > 0)
			{
				for (int i = 0; i < menu->size(); ++i)
				{
					if (QString::fromStdString(menu->at(i).name) == szSelectName)
					{
						std::string menuText;
						g_CGAInterface->PlayerMenuSelect(menu->at(i).index, menuText, bResult);
						//					qDebug() << QString::fromStdString(menuText);
						return;
					}
				}
			}
		}
		else if (m_nWorkType == TWork_Transformation && m_pTransformation->bChecked)
		{
			if (menu && menu->size() > 0)
			{
				for (int i = 0; i < menu->size(); ++i)
				{
					if (QString::fromStdString(menu->at(i).name) == m_pTransformation->selectName)
					{
						std::string menuText;
						g_CGAInterface->PlayerMenuSelect(menu->at(i).index, menuText, bResult);

						return;
					}
				}
			}
		}
		else if (m_nWorkType == TWork_Cosplay && m_pCosplay->bChecked)
		{
			if (menu && menu->size() > 0)
			{
				if (m_pCosplay->selectName.isEmpty())
				{
					std::string menuText;
					g_CGAInterface->PlayerMenuSelect(menu->at(0).index, menuText, bResult); //?????????
					//					qDebug() << QString::fromStdString(menuText);
					return;
				}
				for (int i = 0; i < menu->size(); ++i)
				{
					if (QString::fromStdString(menu->at(i).name) == m_pCosplay->selectName)
					{
						std::string menuText;
						g_CGAInterface->PlayerMenuSelect(menu->at(i).index, menuText, bResult);
						//					qDebug() << QString::fromStdString(menuText);
						return;
					}
				}
			}
		}
	} while (0);

	if (m_pEatFoodCfg->bChecked)
	{
		for (int i = 0; i < menu->size(); ++i)
		{
			if (QString::fromStdString(menu->at(i).name) == m_pEatFoodCfg->selectName)
			{
				std::string menuText;
				g_CGAInterface->PlayerMenuSelect(menu->at(i).index, menuText, bResult);
				//				qDebug() << QString::fromStdString(menuText);
				return;
			}
		}
	}
}

void GameCtrl::OnNotifyUnitMenu(QSharedPointer<CGA::cga_unit_menu_items_t> menu)
{
	bool bResult = false;
	do
	{
		if (m_nWorkType == TWork_Heal || m_nWorkType == TWork_FirstAid) //?????? ????????????
		{
			QString selectSubName;
			if (m_bAutoHeal && m_nWorkType == TWork_Heal && m_pHealCfg->bChecked)
				selectSubName = m_pHealCfg->selectSubName;
			else if (m_bAutoFirstAid && m_nWorkType == TWork_FirstAid && m_pFirstAidCfg->bChecked)
				selectSubName = m_pFirstAidCfg->selectSubName;
			else
				break;

			if (menu && menu->size() > 0)
			{
				//?????? ??????????????????
				if (m_nWorkType == TWork_Heal)
				{
					for (int i = 0; i < menu->size(); ++i)
					{
						if (QString::fromStdString(menu->at(i).name) == selectSubName && menu->at(i).health != 0)
						{
							g_CGAInterface->UnitMenuSelect(menu->at(i).index, bResult);
							return;
						}
					}
				}
				else
				{
					for (int i = 0; i < menu->size(); ++i)
					{
						if (QString::fromStdString(menu->at(i).name) == selectSubName)
						{
							g_CGAInterface->UnitMenuSelect(menu->at(i).index, bResult);
							return;
						}
					}
				}
			}
		}
		else if (m_nWorkType == TWork_Transformation && m_pTransformation->bChecked)
		{
			if (menu && menu->size() > 0)
			{
				//?????? ??????????????????
				if (m_pTransformation->selectSubName.isEmpty())
				{
					g_CGAInterface->UnitMenuSelect(menu->at(0).index, bResult);
					return;
				}
				else
				{

					for (int i = 0; i < menu->size(); ++i)
					{
						if (QString::fromStdString(menu->at(i).name) == m_pTransformation->selectSubName)
						{
							g_CGAInterface->UnitMenuSelect(menu->at(i).index, bResult);
							return;
						}
					}
				}
			}
		}
	} while (0);

	if (m_pEatFoodCfg->bChecked)
	{
		for (int i = 0; i < menu->size(); ++i)
		{
			if (QString::fromStdString(menu->at(i).name) == m_pEatFoodCfg->selectSubName)
			{
				g_CGAInterface->UnitMenuSelect(menu->at(i).index, bResult);
				{
					QMutexLocker locker(&m_pEatFoodCfg->cMutex);
					//??????????????? ?????????????????????
					m_pEatFoodCfg->selectName = ""; //???????????????
					m_pEatFoodCfg->selectSubName = "";
				}
				return;
			}
		}
	}
}

void GameCtrl::OnNotifyChatMsg(int unitid, QString msg, int size, int color)
{
	//???????????????????????????  ???????????????????????????
	/*if (m_uLastUseItemTime.elapsed() < 3000)
		return;
	if (!g_pGameFun->IsInNormalState())
		return;*/
	//?????????
	if (m_bAutoEatDeepBlue)
	{
		if (unitid == -1 && msg.contains("????????????????????????")) //????????????
		{
			foreach (auto pItem, m_pGameItems) //??????????????????
			{
				if (pItem->exist && pItem->pos >= 8 && pItem->id == 18526)
				{
					bool bRes = false;
					g_pGameFun->WaitInNormalState();
					g_CGAInterface->UseItem(pItem->pos, bRes);
					m_uLastUseItemTime.restart();
					break;
				}
			}
			return;
		}
	}
	if (m_bAutoEatDogFood) //?????????
	{
		//if (unitid == -1 && msg.contains("????????????????????????")) //????????????
		//{
		//	foreach (auto pItem, m_pGameItems) //??????????????????
		//	{
		//		if (pItem->exist && pItem->pos >= 8 && pItem->id == 18526)
		//		{
		//			bool bRes = false;
		// 			g_pGameFun->WaitInNormalState();
		//			g_CGAInterface->UseItem(pItem->pos, bRes);
		//			m_uLastUseItemTime.restart();
		//			break;
		//		}
		//	}
		//	return;
		//}
	}
	if (m_bAutoEatTimeCrystal && g_pGameFun->IsInNormalState()) //?????????
	{
		CGA::cga_player_info_t info;
		if (g_CGAInterface->GetPlayerInfo(info))
		{
			int hours = (info.punchclock / 1000 / 3600);
			int mins = (info.punchclock / 1000 - hours * 3600) / 60;
			//			auto clock = std::floor(info.punchclock / 1000 / 3600);	//??????
			if (mins < 2) //1?????? ???
			{
				foreach (auto pItem, m_pGameItems) //??????????????????
				{
					if (pItem->exist && pItem->pos >= 8 && pItem->name.contains("????????????Lv"))
					{
						bool bRes = false;
						g_CGAInterface->UseItem(pItem->pos, bRes); //?????????????????????
						g_CGAInterface->ClickNPCDialog(4, 0, bRes);
						m_uLastUseItemTime.restart();
						break;
					}
				}
			}
		}
	}
}

void GameCtrl::OnSetSyncGameWindowFlag(int state)
{
	m_bSyncGameWindow = state ? true : false;
}

void GameCtrl::OnSyncGameWindow(bool bShow)
{
	if (m_bSyncGameWindow)
	{
		if (bShow)
		{
			ShowWindow(m_gameHwnd, SW_SHOWNORMAL);
		}
		else
		{
			ShowWindow(m_gameHwnd, SW_MINIMIZE);
		}
	}
}

void GameCtrl::OnSetAutoSupply(int state)
{
	m_bAutoSupply = state ? true : false;
}

void GameCtrl::OnSetAutoSale(int state)
{
	m_bAutoSale = state ? true : false;
}

void GameCtrl::OnSetOverTimeT(int state)
{
	m_bOverTimeT = state ? true : false;
}

void GameCtrl::OnSetAutoRenItems(bool state)
{
	m_bAutoRenItems = state;
}

void GameCtrl::OnSetAutoDieItems(bool state)
{
	m_bAutoDieItems = state;
}

void GameCtrl::OnSetAutoFirstAid(bool state)
{
	m_bAutoFirstAid = state;
}

void GameCtrl::OnSetAutoHeal(bool state)
{
	m_bAutoHeal = state;
}

void GameCtrl::OnDownloadMap(int xsize, int ysize)
{
	m_DownloadMapX = 0;
	m_DownloadMapY = 0;
	m_DownloadMapXSize = xsize;
	m_DownloadMapYSize = ysize;
	m_IsDownloadingMap = true;
	qDebug() << "OnDownloadMap";
}

void GameCtrl::OnSetAutoDropPet(int state)
{
	m_bAutoDropPet = state ? true : false;
}

void GameCtrl::OnSetScirptDelayTime(int nTime)
{
	if (nTime < 0)
		m_nScriptDelayTime = 0;
	else
		m_nScriptDelayTime = nTime;
}

void GameCtrl::OnSetWorkDelayTime(int nTime)
{
	m_nWorkDelay = nTime;
}

void GameCtrl::OnSetFollowPickItem(int state)
{
	m_bAutoPickCfg.bFollow = (state == Qt::Checked ? true : false);
}

void GameCtrl::OnSetAutoPickItems(bool bFlag)
{
	m_bAutoPickCfg.bPicked = bFlag;
}

void GameCtrl::OnSetAutoTalkNpc(bool state)
{
	m_bAutoClickNpc = state;
}

void GameCtrl::OnSetAutoTalkNpcYesOrNo(bool state)
{
	m_bAutoTalkNpcYesOrNo = state;
}

void GameCtrl::OnSetRealUpdateUi(int state)
{
	m_bRealUpdateUi = (state == Qt::Checked ? true : false);
}

void GameCtrl::OnSetRealUpdatePlayerUi(int state)
{
	m_bRealUpdatePlayerUi = (state == Qt::Checked ? true : false);
}

void GameCtrl::OnEnableDataDisplayUi(int state)
{
	m_bEnabledDataDisplayUi = (state == Qt::Checked ? true : false);
}

void GameCtrl::OnSetAutoEatDeepBlue(int state)
{
	m_bAutoEatDeepBlue = (state == Qt::Checked ? true : false);
}

void GameCtrl::OnSetAutoEatTimeCrystal(int state)
{
	m_bAutoEatTimeCrystal = (state == Qt::Checked ? true : false);
}

void GameCtrl::OnSetAutoEatDogFood(int state)
{
	m_bAutoEatDogFood = (state == Qt::Checked ? true : false);
}

void GameCtrl::OnSetAutoUploadBankData(int state)
{
	m_bAutoUpLoadBankData = (state == Qt::Checked ? true : false);
}

void GameCtrl::GetBattleUnits()
{
	g_pAutoBattleCtrl->ResetBattleAnalysisData();
	GameBattleUnitList pBattleUnits;
	//??????????????????????????????
	for (int i = 0; i < 20; ++i)
	{
		GameBattleUnitPtr pUnit = GameBattleUnitPtr(new GameBattleUnit);
		pUnit->exist = false;
		pUnit->isback = false;
		pUnit->debuff = 0;
		pUnit->petriding_modelid = 0;
		pBattleUnits.append(pUnit);
	}

	//?????????????????????
	int nCount = 0;
	int nLv = 0;
	int minLv = 888;
	QList<int> frontPetPosList = { 5, 6, 7, 8, 9 };
	QList<int> backPetPosList = { 0, 1, 2, 3, 4 };

	int playerPos = -1;
	CGA::cga_battle_context_t ctx;
	if (g_CGAInterface->GetBattleContext(ctx))
		playerPos = ctx.player_pos;
	QList<int> petPosList;
	//int petPos = -1;
	if (playerPos != -1)
	{
		if (frontPetPosList.contains(playerPos)) //?????????
		{
			petPosList = backPetPosList;
			//	petPos = backPetPosList[frontPetPosList.indexOf(playerPos)];
		}
		else if (backPetPosList.contains(playerPos))
		{
			petPosList = frontPetPosList;
			//	petPos = frontPetPosList[backPetPosList.indexOf(playerPos)];
		}
	}
	CGA::cga_battle_units_t us;
	if (g_CGAInterface->GetBattleUnits(us))
	{
		for (size_t i = 0; i < us.size(); ++i)
		{
			const CGA::cga_battle_unit_t &u = us.at(i);
			pBattleUnits[u.pos]->exist = true;
			pBattleUnits[u.pos]->name = QString::fromStdString(u.name);
			pBattleUnits[u.pos]->hp = u.curhp;
			pBattleUnits[u.pos]->maxhp = u.maxhp;
			pBattleUnits[u.pos]->mp = u.curmp;
			pBattleUnits[u.pos]->maxmp = u.maxmp;
			pBattleUnits[u.pos]->level = u.level;
			pBattleUnits[u.pos]->flags = u.flags;
			pBattleUnits[u.pos]->pos = u.pos;
			pBattleUnits[u.pos]->petriding_modelid = u.petriding_modelid;

			pBattleUnits[u.pos]->debuff = 0;
			pBattleUnits[u.pos]->multi_hp = 0;
			pBattleUnits[u.pos]->multi_maxhp = 0;
			if (petPosList.contains(u.pos))
			{
				nLv += u.level;
				nCount += 1;
				minLv = (minLv > u.level ? u.level : minLv);
			}
			if (u.pos == playerPos)
			{
				/*	m_pGameCharacter->hp = u.curhp;
				m_pGameCharacter->maxhp = u.maxhp;
				m_pGameCharacter->mp = u.curmp;
				m_pGameCharacter->maxmp = u.maxmp;*/
				emit signal_updateBattleHpMp(u.curhp, u.maxhp, u.curmp, u.maxmp);
			}
			/*	if (u.pos == petPos && pet)	//?????????????????? ??????????????????????????????
			{
				pet->hp = u.curhp;
				pet->maxhp = u.maxhp;
				pet->mp = u.curmp;
				pet->maxmp = u.maxmp;
			}*/
		}

		if (nCount == 0)
		{
			auto pet = g_pGameFun->GetBattlePet();
			if (pet)
			{
				m_lastBattleTroopPetMinLv = pet->level;
				m_lastBattleAvgTeamPetLv = pet->level;
			}
		}
		else
		{
			m_lastBattleTroopPetMinLv = minLv;
			m_lastBattleAvgTeamPetLv = nLv / nCount;
		}
	}
	//???????????????????????????????????????????????????
	CBattleWorker::getInstace()->OnNotifyGetBattleInfo(pBattleUnits); //????????????????????????
	emit NotifyGameBattlesInfo(pBattleUnits);
}

void GameCtrl::SetScriptRunState(int state)
{
	qDebug() << "SetScriptRunState" << state;
	m_nRunScriptState = state;
}
