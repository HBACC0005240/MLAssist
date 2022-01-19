#include "GameCtrl.h"
#include "../include/ITPublic.h"
#include "CGFunction.h"
#include "GPCalc.h"
#include "ITObjectDataMgr.h"
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

	const ushort nurse_message[] = { 35201, 22238, 22797, 21527, 65311, 0 }; //要回复吗？

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
	////初始化战斗单位信息
	for (int i = 0; i < 20; ++i)
	{
		GameBattleUnitPtr pUnit = GameBattleUnitPtr(new GameBattleUnit);
		m_pBattleUnits.append(pUnit);
	}
	////可以合成物品信息 也默认初始化下 生产技能栏占得多 主技能10级 当他50个 剩下50个给其他技能
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
	m_upgradeTypeText.insert(TPET_POINT_TYPE_None, "无");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Vigor, "体力");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Strength, "力量");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Endurance, "强度");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Agility, "敏捷");
	m_upgradeTypeText.insert(TPET_POINT_TYPE_Magical, "魔力");
	m_sEquipProtectFilters << "王者守护神"
						   << "猫头鹰头盔"
						   << "承认之戒";
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

int GameCtrl::GetLastBattlePetAvgLv()
{
	//int level = 0;
	//int count = 0;
	//for (int i = 0xA; i < 20; ++i)
	//{
	//	if (!m_pBattleUnits[i]->exist)
	//		continue;
	//	level += m_pBattleUnits[i]->level;
	//	count += 1;
	//}
	//if (count <= 0) //只算自己的
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
	if (type == 1) //最低
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
	else if (type == 2) //最高
	{
	}
}

void GameCtrl::RestLastBattlePetLv(int type)
{
	if (type == 1) //最低
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
* 9 平时 10 战斗 11 与服务器联机被切断
* cga.GetGameStatus
* 202 | 205  切图
* 1 未知
* 2 卡住战斗 4 战斗选择 5 战斗中 8 战斗结束一瞬间的状态 11 战斗切图，不能用来判断战斗，因为战斗中会有小瞬间是3空闲状态
* 3 空闲
* 1 一般是战斗开局画布打开中
* 2 算是在战斗计算中
* 6 播放战斗画面
* 战斗中3 怪物奔向人物动画是3
*201  204 切换地图画面  对话切图
201 切图
201 205
200 202 205切图
202一般是切图中黑屏值
201是开始切图
204 205 都是切图结束

1    遇敌
10 3 遇敌开场动画
6 遇敌战斗动画

8战斗完毕 关闭动画
11 2都算战斗结束切图
2战斗结算  应该算弹出结算画面*/

QString GameCtrl::getGameState()
{
	int gameStatus, worldStatus;
	if (g_CGAInterface->GetGameStatus(gameStatus) && g_CGAInterface->GetWorldStatus(worldStatus))
	{
		switch (gameStatus)
		{
			case 1: return "战斗开场动画"; //开合动画
			case 2: return "战斗结算";	   //"卡住战斗";
			case 3:
			{
				return worldStatus == 9 ? "空闲" : "战斗开始"; //战斗开场动画
			}
			case 4: return "战斗选择";
			case 5: return "遇敌过场动画"; //5遇敌开始和结束都有
			case 6: return "战斗动画";
			case 8: return "战斗结束"; //战斗结束动画
			case 11: return "战斗切图";
			case 200:
			case 201: return "开始切图";
			case 202: return "切图中";
			case 203:
			case 204:
			case 205: return "切图结束";
			default:
				break;
		}
	}
	return "未知";
}

void GameCtrl::LoadConfig(const QString &path)
{
	if (path.isEmpty())
		return;
	QSettings iniFile(path, QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("GB2312")); //这样分组下的键值可以读取中文  下面的是读取段的中文

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
	iniFile.setIniCodec(QTextCodec::codecForName("GB2312")); //这样分组下的键值可以读取中文  下面的是读取段的中文
}

void GameCtrl::StartUpdateTimer()
{
	m_bExit = false;
	m_updateTimer.start(5000); //2秒获取一次战斗信息
	//m_characterTimer.start(1000);	//1秒获取1次数据
	m_lastUploadTime.start();
	m_mapTimer.start(500);																											 //半秒更新坐标和地图
	m_nLastOverTime.start();																										 //道具超时检测
	m_uLastUseItemTime.start();																										 //使用物品时间
	m_nLastAutoDropPetTime.start();																									 //自动扔宠判断
	m_lastRecvNpcDlgTime.start();																									 //接收对话框时间判断
	m_lastNormalState.start();																										 //非正常状态判断
	g_CGAInterface->RegisterGameWndKeyDownNotify(std::bind(&GameCtrl::NotifyGameWndKeyDown, this, std::placeholders::_1));			 //按键回调
	g_CGAInterface->RegisterBattleActionNotify(std::bind(&GameCtrl::NotifyBattleAction, this, std::placeholders::_1));				 //战斗回调
	g_CGAInterface->RegisterChatMsgNotify(std::bind(&GameCtrl::NotifyChatMsgCallback, this, std::placeholders::_1));				 //聊天信息回调
	g_CGAInterface->RegisterNPCDialogNotify(std::bind(&GameCtrl::NotifyNPCDialogCallback, this, std::placeholders::_1));			 //npc对话框回调
	g_CGAInterface->RegisterDownloadMapNotify(std::bind(&GameCtrl::NotifyDownloadMapCallback, this, std::placeholders::_1));		 //下载地图
	g_CGAInterface->RegisterPlayerMenuNotify(std::bind(&GameCtrl::NotifyPlayerMenuCallback, this, std::placeholders::_1));			 //人物选择菜单
	g_CGAInterface->RegisterConnectionStateNotify(std::bind(&GameCtrl::NotifyConnectionStateCallback, this, std::placeholders::_1)); //游戏状态
	g_CGAInterface->RegisterUnitMenuNotify(std::bind(&GameCtrl::NotifyUnitMenuCallback, this, std::placeholders::_1));				 //菜单选项
	g_CGAInterface->RegisterWorkingResultNotify(std::bind(&GameCtrl::NotifyWorkingResultCallback, this, std::placeholders::_1));	 //工作结果回调
	g_CGAInterface->RegisterServerShutdownNotify(std::bind(&GameCtrl::NotifyServerShutdown, this, std::placeholders::_1));			 //服务器连接状态

	g_CGAInterface->RegisterTradeStuffsNotify(std::bind(&GameCtrl::NotifyTradeStuffsCallback, this, std::placeholders::_1)); //交易物品回调
	g_CGAInterface->RegisterTradeDialogNotify(std::bind(&GameCtrl::NotifyTradeDialogCallback, this, std::placeholders::_1)); //交易对话框回调
	g_CGAInterface->RegisterTradeStateNotify(std::bind(&GameCtrl::NotifyTradeStateCallback, this, std::placeholders::_1));	 //交易状态回调

	emit NotifyAttachProcessOk(m_gameProcessID, m_nGameThreadID, m_nGamePort, (quint32)m_gameHwnd);
	m_characterFuture = QtConcurrent::run(GetCharacterDataThread, this);
	m_itemsFuture = QtConcurrent::run(GetItemDataThread, this);
	m_mapFuture = QtConcurrent::run(DownloadMapThread, this);
	m_normalFuture = QtConcurrent::run(NormalThread, this);
	m_notifyTimeoutFuture = QtConcurrent::run(NotifyTimeoutThread, this);
	emit signal_attachGame(); //
							  //qDebug() << "线程池最大线程个数：" << QThreadPool::globalInstance()->maxThreadCount();
							  ////当前活动的线程个数
							  //qDebug() << "当前活动的线程个数：" << QThreadPool::globalInstance()->activeThreadCount();
							  ////设置最大线程数
							  //QThreadPool::globalInstance()->setMaxThreadCount(100);
							  //qDebug() << "线程池最大线程个数：" << QThreadPool::globalInstance()->maxThreadCount();
	g_pGameFun->RestFun();
}

void GameCtrl::StopUpdateTimer()
{
	m_bExit = true; //退出线程
	g_pGameFun->StopFun();

	m_updateTimer.stop();	 //更新定时器
	m_characterTimer.stop(); //人物定时器
	m_mapTimer.stop();		 //地图定时器
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
		foreach (auto pRenItem, m_pRenItemList)
		{
			if (pRenItem->isDrop)
			{
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);
					if (pItem && pItem->exist && (pItem->name == pRenItem->name || QString::number(pItem->id) == pRenItem->name))
					{
						bool result = false;
						if (g_CGAInterface->DropItem(pItem->pos, result) && result)
							continue;
					}
				}
			}
		}
		return true;
	}

	return false;
}
//堆叠物品  不加数量判断
bool GameCtrl::DieItems()
{
	foreach (auto pDieItem, m_pPileItemList)
	{
		if (pDieItem->isPile)
		{
			for (size_t i = 0; i < m_pGameItems.size(); i++)
			{
				GameItemPtr pItem = m_pGameItems.at(i); //最大值 到时候通过外置配置获取
				if (pItem && pItem->exist && pItem->name == pDieItem->name && pItem->count < pDieItem->maxCount)
				{

					for (size_t n = 0; n < m_pGameItems.size(); n++)
					{
						GameItemPtr pOtherItem = m_pGameItems.at(n);
						if (pOtherItem && pOtherItem->exist && pOtherItem->name == pItem->name && pOtherItem != pItem && pOtherItem->count < pDieItem->maxCount)
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
	if (m_uLastUseItemTime.elapsed() < 5000) //使用物品间隔
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
	//bug就是 如果满足吃的条件，但人物想给其他人吃，会自动给自己吃了
	//需要先关闭自动吃才能给别人吃
	if (!bEat)
	{
		m_pEatFoodCfg->bChecked = false;
		return false;
	}

	//获取最新的判断 否则会有问题

	CGA::cga_player_info_t char_info;
	if (g_CGAInterface->GetPlayerInfo(char_info))
	{
		return false;
	}

	GamePetPtr pPet = g_pGameFun->GetBattlePet(); //获取实时的宠物
	for (auto it = m_pEatFoodJudge.begin(); it != m_pEatFoodJudge.end(); ++it)
	{
		auto pCfg = it.value();
		if (pCfg->bChecked)
		{
			if (pCfg->nType == TCondition_PlayerEatMagic && (char_info.mp <= (char_info.maxmp * pCfg->dVal / 100)))
			{ //吃料理
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);			//最大值 到时候通过外置配置获取
					if (pItem && pItem->exist && pItem->type == 23) //料理
					{
						bool result = false;
						m_pEatFoodCfg->bChecked = true;
						m_pEatFoodCfg->selectName = QString::fromStdString(char_info.name);
						m_pEatFoodCfg->selectSubName = QString::fromStdString(char_info.name);
						if (g_CGAInterface->UseItem(pItem->pos, result) && result)
						{
							m_uLastUseItemTime.restart();
							return true;
						}
					}
				}
			}
			else if (pCfg->nType == TCondition_PetEatMagic && pPet && (pPet->mp <= (pPet->maxmp * pCfg->dVal / 100)))
			{ //吃料理
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);			//最大值 到时候通过外置配置获取
					if (pItem && pItem->exist && pItem->type == 23) //料理
					{
						bool result = false;
						m_pEatFoodCfg->bChecked = true;
						m_pEatFoodCfg->selectName = QString::fromStdString(char_info.name);
						m_pEatFoodCfg->selectSubName = pPet->name;
						if (g_CGAInterface->UseItem(pItem->pos, result) && result)
						{
							m_uLastUseItemTime.restart();
							return true;
						}
					}
				}
			}
			else if (pCfg->nType == TCondition_PlayerEatMedicament && (char_info.hp <= (char_info.maxhp * pCfg->dVal / 100)))
			{ //吃血瓶
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);			//最大值 到时候通过外置配置获取
					if (pItem && pItem->exist && pItem->type == 43) //血瓶
					{
						bool result = false;
						m_pEatFoodCfg->bChecked = true;
						m_pEatFoodCfg->selectName = QString::fromStdString(char_info.name);
						m_pEatFoodCfg->selectSubName = QString::fromStdString(char_info.name);
						if (g_CGAInterface->UseItem(pItem->pos, result) && result)
						{
							m_uLastUseItemTime.restart();
							return true;
						}
					}
				}
			}
			else if (pCfg->nType == TCondition_PetEatMedicament && pPet && (pPet->hp <= (pPet->maxhp * pCfg->dVal / 100)))
			{ //吃血瓶
				for (size_t i = 0; i < m_pGameItems.size(); i++)
				{
					GameItemPtr pItem = m_pGameItems.at(i);			//最大值 到时候通过外置配置获取
					if (pItem && pItem->exist && pItem->type == 43) //吃血瓶
					{
						bool result = false;
						m_pEatFoodCfg->bChecked = true;
						m_pEatFoodCfg->selectName = QString::fromStdString(char_info.name);
						m_pEatFoodCfg->selectSubName = pPet->name;
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
//自动急救
bool GameCtrl::AutoFirstAid()
{
	do
	{
		if (!m_bAutoFirstAid)
			break;																		 //return false;
		if (QDateTime::currentMSecsSinceEpoch() < (m_pFirstAidCfg->lastUseSkill + 2000)) //2秒用一次
			break;																		 //return false;

		//1个没勾选 返回
		if (!m_pFirstAidCfg->bSelf && !m_pFirstAidCfg->bPet && !m_pFirstAidCfg->bTeammate)
		{
			m_pFirstAidCfg->bChecked = false;
			break; //return false;
		}
		//急救比例
		if (m_pFirstAidCfg->dFirstAidHp <= 0)
			break; //return false;
		GameSkillList pSkillList = m_pGameSkills;
		GameSkillPtr pFirstAidSkill = nullptr;
		foreach (auto pSkill, pSkillList)
		{
			if (pSkill->name == "急救")
			{
				pFirstAidSkill = pSkill;
				break;
			}
		}
		if (pFirstAidSkill == nullptr)
		{
			//	qDebug() << "没有急救技能";
			break;
			//return false;
		}
		//qDebug() << pFirstAidSkill->level << pFirstAidSkill->maxLevel << pFirstAidSkill->cost;
		//当前技能等级 level  最高能使用的等级maxLevel   cost有问题 一直是0  需要取子技能
		if (m_pFirstAidCfg->nLv == -1) //最高
		{
			if (pFirstAidSkill->level > pFirstAidSkill->maxLevel)
			{ //最高使用等级 maxLevel
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
		if (m_pFirstAidCfg->nLv > (pFirstAidSkill->maxLevel - 1)) //选的技能等级 > 最高用的技能等级 返回
		{														  //最高使用等级 maxLevel
																  //		qDebug() << QString("最高可使用等级%1").arg(pFirstAidSkill->maxLevel);
			break;
			//return false;
		}
		m_pFirstAidCfg->dCost = pFirstAidSkill->subskills.at(m_pFirstAidCfg->nLv)->cost;
		//	qDebug() << QString("耗魔%1").arg(m_pFirstAidCfg->dCost);
		if (m_pGameCharacter->mp < m_pFirstAidCfg->dCost)
		{
			//	qDebug() << "魔不够";
			break; //return false;
		}
		//优先急救自己
		//宠物
		//队友
		QString selectName, selectSubName;

		int nFirstAidTarget = -1; //0自己 1自己宠 2队友
		//优先自己
		if (m_pFirstAidCfg->bSelf)
		{
			double dFirstAidVal = m_pGameCharacter->maxhp * m_pFirstAidCfg->dFirstAidHp / 100; //急救值
			if (m_pGameCharacter->hp < dFirstAidVal)										   //急救自己
			{
				nFirstAidTarget = 0;
				selectName = m_pGameCharacter->name;
				selectSubName = m_pGameCharacter->name;
			}
		}
		//宠物
		if (nFirstAidTarget == -1 && m_pFirstAidCfg->bPet)
		{
			foreach (auto battlePet, m_pGamePets)
			{
				if (battlePet && battlePet->exist && battlePet->default_battle) //默认出战宠物
				{
					double dFirstAidVal = battlePet->maxhp * m_pFirstAidCfg->dFirstAidHp / 100; //急救值
					if (battlePet->hp < dFirstAidVal)											//急救宠物
					{
						nFirstAidTarget = 1;
						selectName = m_pGameCharacter->name;
						selectSubName = battlePet->name;
					}
					break;
				}
			}
		}
		//队友
		if (nFirstAidTarget == -1 && m_pFirstAidCfg->bTeammate)
		{
			CGA::cga_team_players_t teamPlayers;
			g_CGAInterface->GetTeamPlayerInfo(teamPlayers);
			foreach (auto teamPlayer, teamPlayers)
			{
				double dFirstAidVal = teamPlayer.maxhp * m_pFirstAidCfg->dFirstAidHp / 100; //急救值
				if (teamPlayer.hp < dFirstAidVal)											//急救队友
				{
					nFirstAidTarget = 2;
					selectName = m_pGameCharacter->name;
					selectSubName = QString::fromStdString(teamPlayer.name);
					break;
				}
			}
		}
		if (nFirstAidTarget == -1)
		{
			//		qDebug() << "状态完美，不需要急救";
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
//自动治疗
bool GameCtrl::AutoHeal()
{
	if (!m_bAutoHeal)
		return false;
	if (QDateTime::currentMSecsSinceEpoch() < (m_pHealCfg->lastUseSkill + 2000)) //2秒用一次
		return false;
	m_pHealCfg->bSelf = true; //上面勾了  这个默认true 治疗自己
	//1个没勾选 返回
	//if (!m_pHealCfg->bSelf && !m_pHealCfg->bPet && !m_pHealCfg->bTeammate)
	//{
	//	return false;
	//}
	GameSkillList pSkillList = m_pGameSkills;
	GameSkillPtr pSelectSkill = nullptr;
	foreach (auto pSkill, pSkillList)
	{
		if (pSkill->name == "治疗")
		{
			pSelectSkill = pSkill;
			break;
		}
	}
	if (pSelectSkill == nullptr)
	{
		//	qDebug() << "没有治疗技能";
		return false;
	}
	//当前技能等级 level  最高能使用的等级maxLevel   cost有问题 一直是0  需要取子技能
	if (m_pHealCfg->nLv == -1) //最高
	{
		if (pSelectSkill->level > pSelectSkill->maxLevel)
		{ //最高使用等级 maxLevel
			m_pHealCfg->nLv = pSelectSkill->maxLevel - 1;
		}
		else
		{
			m_pHealCfg->nLv = pSelectSkill->level - 1;
		}
	}
	if (m_pHealCfg->nLv > (pSelectSkill->maxLevel - 1)) //选的技能等级 > 最高用的技能等级 返回
	{													//最高使用等级 maxLevel
														//		qDebug() << QString("最高可使用等级%1").arg(pSelectSkill->maxLevel);
		return false;
	}
	m_pHealCfg->dCost = pSelectSkill->subskills.at(m_pHealCfg->nLv)->cost;
	//	qDebug() << QString("耗魔%1").arg(m_pHealCfg->dCost);
	if (m_pGameCharacter->mp < m_pHealCfg->dCost)
	{
		//		qDebug() << "魔不够";
		return false;
	}
	//优先自己
	//宠物
	//队友
	QString selectName, selectSubName;

	int nHealTarget = -1; //0自己 1自己宠 2队友
	//优先自己 默认打开自动治疗 就优先治疗自己
	//if (m_pHealCfg->bSelf)
	{
		if (m_pGameCharacter->health > 0 && m_pGameCharacter->health < m_pHealCfg->nHurtVal) //治疗自己
		{
			nHealTarget = 0;
			selectName = m_pGameCharacter->name;
			selectSubName = m_pGameCharacter->name;
		}
	}
	//宠物
	if (nHealTarget == -1 && m_pHealCfg->bPet)
	{
		foreach (auto battlePet, m_pGamePets)
		{
			if (battlePet && battlePet->exist && battlePet->default_battle) //默认出战宠物
			{
				if (battlePet->health > 0 && battlePet->health < m_pHealCfg->nHurtVal) //宠物
				{
					nHealTarget = 1;
					selectName = m_pGameCharacter->name;
					selectSubName = battlePet->name;
				}
				break;
			}
		}
	}
	//队友
	if (nHealTarget == -1 && m_pHealCfg->bTeammate)
	{
		CGA::cga_team_players_t teamPlayers; //队伍里 获取不到队友是否受伤，通过地图判断
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
						qDebug() << "受伤：" << QString::fromStdString(teamPlayer.name) << mapUnit.injury;
						//0是正常 其他值 上次8也是正常 还有9  也是受伤  还有治疗完后，通过地图获取的，不刷新治疗结果 需要远离再过来激活
						if (mapUnit.injury == 1 || mapUnit.injury == 3 || mapUnit.injury == 9) //injury受伤 不知道能否判断受伤等级 不能判断的话，每次一直治疗同一个人，不成功不能进行下一个
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
		//qDebug() << "状态完美，不需要治疗";
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
	if (QDateTime::currentMSecsSinceEpoch() < (m_pTransformation->lastUseSkill + 2000)) //1秒用一次
		return false;
	GameSkillPtr pSelectSkill = nullptr;
	foreach (auto pSkill, m_pGameSkills)
	{
		if (pSkill->name == "变身")
		{
			pSelectSkill = pSkill;
			break;
		}
	}
	if (pSelectSkill == nullptr)
	{
		//	qDebug() << "没有变身技能";
		return false;
	}
	if (pSelectSkill->level > pSelectSkill->maxLevel)
	{ //最高使用等级 maxLevel
		m_pTransformation->nLv = pSelectSkill->maxLevel - 1;
	}
	else
	{
		m_pTransformation->nLv = pSelectSkill->level - 1;
	}
	if (m_pTransformation->nLv > (pSelectSkill->maxLevel - 1)) //选的技能等级 > 最高用的技能等级 返回
	{														   //最高使用等级 maxLevel
															   //		qDebug() << QString("最高可使用等级%1").arg(pSelectSkill->maxLevel);
		return false;
	}
	if (m_pGameCharacter->mp < 10)
	{
		//		qDebug() << "魔不够";
		return false;
	}
	bool bResult = false;
	m_pTransformation->lastUseSkill = QDateTime::currentMSecsSinceEpoch();
	m_pTransformation->selectName = m_pGameCharacter->name; //当前角色人物名称
	if (m_pTransformation->selectSubName.isEmpty())
	{ //默认第一个
		foreach (auto battlePet, m_pGamePets)
		{
			if (battlePet && battlePet->exist) //默认出战宠物
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
	if (QDateTime::currentMSecsSinceEpoch() < (m_pCosplay->lastUseSkill + 2000)) //1秒用一次
		return false;
	GameSkillPtr pSelectSkill = nullptr;
	foreach (auto pSkill, m_pGameSkills)
	{
		if (pSkill->name == "变装")
		{
			pSelectSkill = pSkill;
			break;
		}
	}
	if (pSelectSkill == nullptr)
	{
		//	qDebug() << "没有变身技能";
		return false;
	}
	if (pSelectSkill->level > pSelectSkill->maxLevel)
	{ //最高使用等级 maxLevel
		m_pCosplay->nLv = pSelectSkill->maxLevel - 1;
	}
	else
	{
		m_pCosplay->nLv = pSelectSkill->level - 1;
	}
	if (m_pCosplay->nLv > (pSelectSkill->maxLevel - 1)) //选的技能等级 > 最高用的技能等级 返回
	{													//最高使用等级 maxLevel
														//		qDebug() << QString("最高可使用等级%1").arg(pSelectSkill->maxLevel);
		return false;
	}
	if (m_pGameCharacter->mp < 10)
	{
		//		qDebug() << "魔不够";
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
	if (m_pGameCharacter->petriding) //骑乘中 返回
		return false;
	//判断骑乘水晶？
	if (QDateTime::currentMSecsSinceEpoch() < (m_pAutoPetRiding->lastUseSkill + 2000)) //1秒用一次
		return false;
	GameSkillPtr pSelectSkill = nullptr;
	foreach (auto pSkill, m_pGameSkills)
	{
		if (pSkill->name == "骑乘")
		{
			pSelectSkill = pSkill;
			break;
		}
	}
	if (pSelectSkill == nullptr)
	{
		//	qDebug() << "没有变身技能";
		return false;
	}
	if (pSelectSkill->level > pSelectSkill->maxLevel)
	{ //最高使用等级 maxLevel
		m_pAutoPetRiding->nLv = pSelectSkill->maxLevel - 1;
	}
	else
	{
		m_pAutoPetRiding->nLv = pSelectSkill->level - 1;
	}
	if (m_pAutoPetRiding->nLv > (pSelectSkill->maxLevel - 1)) //选的技能等级 > 最高用的技能等级 返回
	{														  //最高使用等级 maxLevel
															  //		qDebug() << QString("最高可使用等级%1").arg(pSelectSkill->maxLevel);
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
	//1分钟1次 说话防掉线
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
	//强制内置宠物忠诚低 收回 40
	int internalLoyalityVal = 35;
	for (size_t i = 0; i < m_pGamePets.size(); ++i)
	{
		auto pet = m_pGamePets.at(i);
		if (!pet->exist)
			continue;
		if (pet->battle_flags == TPET_STATE_BATTLE && pet->loyality < internalLoyalityVal)
		{
			qDebug() << "宠物忠诚低于35，内置保护启动，收回宠物！";
			g_CGAInterface->ChangePetState(i, TPET_STATE_NONE, bResult); //3休息 2战斗 1待命 16散步
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
			g_CGAInterface->ChangePetState(i, TPET_STATE_NONE, bResult); //3休息 2战斗 1待命 16散步
			bRecall = true;
			continue;
		}
		if (pet->battle_flags == TPET_STATE_BATTLE && m_pBattlePetCfg->bRecallMp && pet->mp < m_pBattlePetCfg->nRecallMp)
		{
			g_CGAInterface->ChangePetState(i, TPET_STATE_READY, bResult); //3休息 2战斗 1待命 16散步
			continue;
		}
	}
	//不是忠诚低召回 并且勾选了招出，则进行招出
	if (!bRecall && m_pBattlePetCfg->bSummonPet && m_pGameCharacter->petid == -1) //-1 没有宠
	{
		//排序 获取index
		int nMaxVal = -1;
		int nIndex = -1;
		int nPetLoy = 0;
		if (m_pBattlePetCfg->nSummonPetType == TChangePet_HighestLv) //取第一个最高等级
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
	for (auto it = m_bDropPetCheckItem.begin(); it != m_bDropPetCheckItem.end(); ++it)
	{
		if (it.value()) //勾选 判断
		{
			for (size_t i = 0; i < petsinfo.size(); ++i)
			{
				int nVal = m_nDropPetCheckVal.value(it.key()).toInt();
				CGA::cga_pet_info_t pet = petsinfo.at(i);
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
						//20档不丢
						if (nPetVal[0] > nVal && nPetVal[0] != 20 && pet.level == 1)
						{
							qDebug() << "档次:" << nPetVal[0] << nPetVal[1] << nPetVal[2] << " 指定值：" << nVal;
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
					QString sMsg = QString("放下Lv%1 %2 %3 档次:%4").arg(pet.level).arg(QString::fromStdString(pet.realname)).arg(g_pGameFun->GetLogPetAttribute(pet)).arg(it.key() == TDropPetType_Grade ? sDropPetText : "--");
					//					qDebug() << QString("放下Lv:%0 %1 【%2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 档次：%14 %15").arg(pet.level).arg(QString::fromStdString(pet.realname)).arg(pet.maxhp).arg(pet.maxmp).arg(pet.detail.points_endurance).arg(pet.detail.points_strength).arg(pet.detail.points_defense).arg(pet.detail.points_agility).arg(pet.detail.points_magical).arg(pet.detail.value_attack).arg(pet.detail.value_defensive).arg(pet.detail.value_agility).arg(pet.detail.value_spirit).arg(pet.detail.value_recovery).arg(it.key() == TDropPetType_Grade ? nPetVal : "--");
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
		//	qDebug() << "包裹满了，不自动捡咯";
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
				if (pickNameList.contains(sItemName) || sItemName.contains("金币"))
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
				//	qDebug() << "包裹满了，不自动捡咯";
				return false;
			}
		}
	}
	if (m_bAutoPickCfg.bFixedPoint && m_bAutoPickCfg.east != 0 && m_bAutoPickCfg.south != 0)
	{
		if ((QDateTime::currentMSecsSinceEpoch() - m_bAutoPickCfg.lastPickTime) >= 10 * 1000)
		{
			//10秒未捡物 归位
			g_pGameFun->AutoMoveTo(m_bAutoPickCfg.east, m_bAutoPickCfg.south);
			m_bAutoPickCfg.lastPickTime = QDateTime::currentMSecsSinceEpoch();
		}
	}
}

bool GameCtrl::AutoUpgradeAddPoint()
{
	if (m_upgradePlayerCfg->bEnabled)
	{
		if (QDateTime::currentMSecsSinceEpoch() < (m_upgradePlayerCfg->lastCallTime + 2000)) //2秒用一次
			return false;
		CGA::cga_player_info_t info;
		if (g_CGAInterface->GetPlayerInfo(info))
		{
			if (info.detail.points_remain > 0)
			{
				int pointsRemain = info.detail.points_remain;
				//先判断是否有指定加点，有就指定加点
				GameUpgradeData upgradeCfg;
				if (m_upgradePlayerCfg->specialCfg.bEnabled)
				{
					memcpy(&upgradeCfg, &m_upgradePlayerCfg->specialCfg, sizeof(GameUpgradeData));
				}
				else if (m_upgradePlayerCfg->normalCfg.bEnabled)
				{
					memcpy(&upgradeCfg, &m_upgradePlayerCfg->normalCfg, sizeof(GameUpgradeData));
				}
				//还是每次加一点吧，按顺序优先加点，所以血永远第一个加的
				do
				{
					if (!upgradeCfg.bEnabled) //没有加点 跳过
						break;
					//正常加点，默认最大值初始化为333
					int maxPoint = 15 + (info.level - 1) * 2;
					if (upgradeCfg.bVigor) //检查体力点
					{
						//小于最大值和指定值 进行加点
						if (info.detail.points_endurance < maxPoint && info.detail.points_endurance < upgradeCfg.vigorVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Vigor);
							break;
						}
					}
					if (upgradeCfg.bStrength) //检查力量
					{
						//小于最大值和指定值 进行加点
						if (info.detail.points_strength < maxPoint && info.detail.points_strength < upgradeCfg.strengthVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Strength);
							break;
						}
					}
					if (upgradeCfg.bEndurance) //检查强度
					{
						//小于最大值和指定值 进行加点
						if (info.detail.points_defense < maxPoint && info.detail.points_defense < upgradeCfg.enduranceVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Endurance);
							break;
						}
					}
					if (upgradeCfg.bAgility) //检查敏捷
					{
						//小于最大值和指定值 进行加点
						if (info.detail.points_agility < maxPoint && info.detail.points_agility < upgradeCfg.agilityVal)
						{
							g_CGAInterface->UpgradePlayer(TPET_POINT_TYPE_Agility);
							break;
						}
					}
					if (upgradeCfg.bMagical) //检查魔法
					{
						//小于最大值和指定值 进行加点
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
	if (m_upgradePetCfg->bEnabled) //是否启用自动加点
	{
		if (QDateTime::currentMSecsSinceEpoch() < (m_upgradePetCfg->lastCallTime + 2000)) //2秒用一次
			return false;

		if (m_upgradePetCfg->normalCfg.bEnabled) //正常加点
		{
			GamePetPtr battlePet = nullptr;
			if (m_upgradePetCfg->petName.isEmpty() && m_upgradePetCfg->petRealName.isEmpty()) //默认出战宠物
			{
				foreach (auto tmpPet, m_pGamePets)
				{
					if (tmpPet && tmpPet->exist && tmpPet->battle_flags == TPET_STATE_BATTLE) //默认出战宠物
					{
						battlePet = tmpPet;
						break;
					}
				}
			}
			else //查找指定宠物
			{
				foreach (auto tmpPet, m_pGamePets)
				{
					if (tmpPet && tmpPet->exist && tmpPet->battle_flags == TPET_STATE_BATTLE)
					{
						//如果宠物名称不一致，调过
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
			if (pointVal <= 0) //没有需要加点的点数
				return false;
			//定义这个变量，主要在加点前记录下，然后赋值用，防止真的加点后，刷新了数据
			int curVigor = battlePet->detail.points_endurance;
			int curStrength = battlePet->detail.points_strength;
			int curEndurance = battlePet->detail.points_defense;
			int curAgility = battlePet->detail.points_agility;
			int curMagical = battlePet->detail.points_magical;
			if (m_upgradePetCfg->normalCfg.addBpType == -1) //混加 忽略类型 以此加指定部分点
			{
				if (m_upgradePetCfg->normalCfg.vigorVal > 0 && battlePet->detail.points_endurance < m_upgradePetCfg->normalCfg.vigorVal && pointVal > 0)
				{
					qDebug() << "混加 宠物加点-体力:";
					g_CGAInterface->UpgradePet(battlePet->index, 0); //体力
					pointVal--;
				}
				if (m_upgradePetCfg->normalCfg.strengthVal > 0 && battlePet->detail.points_strength < m_upgradePetCfg->normalCfg.strengthVal && pointVal > 0)
				{
					qDebug() << "混加 宠物加点-力量:";
					g_CGAInterface->UpgradePet(battlePet->index, 1); //力量
					pointVal--;
				}
				if (m_upgradePetCfg->normalCfg.enduranceVal > 0 && battlePet->detail.points_defense < m_upgradePetCfg->normalCfg.enduranceVal && pointVal > 0)
				{
					qDebug() << "混加 宠物加点-强度:";
					g_CGAInterface->UpgradePet(battlePet->index, 2); //强度
					pointVal--;
				}
				if (m_upgradePetCfg->normalCfg.agilityVal > 0 && battlePet->detail.points_agility < m_upgradePetCfg->normalCfg.agilityVal && pointVal > 0)
				{
					qDebug() << "混加 宠物加点-敏捷:";
					g_CGAInterface->UpgradePet(battlePet->index, 3); //敏捷
					pointVal--;
				}
				if (m_upgradePetCfg->normalCfg.magicalVal > 0 && battlePet->detail.points_magical < m_upgradePetCfg->normalCfg.magicalVal && pointVal > 0)
				{
					qDebug() << "混加 宠物加点-魔法:";
					g_CGAInterface->UpgradePet(battlePet->index, 4); //魔法
					pointVal--;
				}
			}
			else if (m_upgradePetCfg->normalCfg.addBpType >= 0 && m_upgradePetCfg->normalCfg.addBpType <= 4)
			{

				//还是不循环加点了 延迟2秒加点
				qDebug() << "宠物加点：" << m_upgradePetCfg->normalCfg.addBpType << m_upgradeTypeText.value(m_upgradePetCfg->normalCfg.addBpType);
				g_CGAInterface->UpgradePet(battlePet->index, m_upgradePetCfg->normalCfg.addBpType);
				//爆点还是放这 优先先走一轮正常加点，防止加点错误，上面自主分配，不会出现爆点
				//先保存加点前值，下次来先比对，一致，则认为爆点了，因为如果加点成功，记录的点和现在肯定不一样
				if (m_upgradePetCfg->oldVigorVal == curVigor && m_upgradePetCfg->oldStrengthVal == curStrength && m_upgradePetCfg->oldEnduranceVal == curEndurance && m_upgradePetCfg->oldAgilityVal == curAgility && m_upgradePetCfg->oldMagicalVal == curMagical)
				{ //爆点
					//保存当前的
					m_upgradePetCfg->oldVigorVal = curVigor;
					m_upgradePetCfg->oldStrengthVal = curStrength;
					m_upgradePetCfg->oldEnduranceVal = curEndurance;
					m_upgradePetCfg->oldAgilityVal = curAgility;
					m_upgradePetCfg->oldMagicalVal = curMagical;
					if (m_upgradePetCfg->unnormalCfg.bEnabled) //爆点加点
					{
						qDebug() << "宠物爆点，爆点加点" << m_upgradeTypeText.value(m_upgradePetCfg->unnormalCfg.addBpType);
						g_CGAInterface->UpgradePet(battlePet->index, m_upgradePetCfg->unnormalCfg.addBpType); //和实际对应
					}
				}
				else
				{
					//保存当前的
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
				//				qDebug() << "宠物爆点，爆点加点";
				//				if (m_upgradePetCfg->unnormalCfg.bEnabled) //爆点加点
				//				{
				//					g_CGAInterface->UpgradePet(battlePet->index, m_upgradePetCfg->unnormalCfg.addBpType); //和实际对应
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
//调用转向时 重置
void GameCtrl::ResetRecvNpcDlgState()
{
	m_bHasNPCDlg = false;
}

bool GameCtrl::AutoEquipProtect()
{
	if (!m_pEquipProtectCfg->bChecked)
		return false;
	if (QDateTime::currentMSecsSinceEpoch() < (m_pEquipProtectCfg->lastUseSkill + 5000)) //5秒判断一次
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
					 << "pos:" << iteminfo.pos << "当前耐久:" << nCur << "最大耐久" << nMax << " 判断值:" << nMax * 0.01 * m_pEquipProtectCfg->dVal;*/
			if (iteminfo.pos >= 0 && iteminfo.pos < 8 && nCur < nMax * 0.01 * m_pEquipProtectCfg->dVal)
			{
				QString sItemName = QString::fromStdString(iteminfo.name);
				if (m_sEquipProtectFilters.contains(sItemName)) //过滤
					continue;

				if (m_pEquipProtectCfg->nSwapEquip == 0) //同名装备
				{
					for (size_t n = 0; n < itemsinfo.size(); ++n)
					{
						const CGA::cga_item_info_t &iteminfo2 = itemsinfo.at(n);
						int nCur2 = 0, nMax2 = 0;
						g_pGameFun->ParseItemDurabilityEx(QString::fromStdString(iteminfo2.attr), nCur2, nMax2);
						if (iteminfo2.pos > 7 && iteminfo2.name == iteminfo.name && nCur2 > nMax2 * 0.01 * m_pEquipProtectCfg->dVal)
						{ //判断耐久是否符合要求 符合的话就换 不符合 判断下一个
							g_CGAInterface->MoveItem(iteminfo2.pos, iteminfo.pos, -1, bRes);
							if (m_pEquipProtectCfg->bRenEquip) //扔无耐久装备
							{
								g_CGAInterface->DropItem(iteminfo2.pos, bRes);
							}
							break;
						}
					}
				}
				else if (m_pEquipProtectCfg->nSwapEquip == 1) //同类装备
				{
					for (size_t n = 0; n < itemsinfo.size(); ++n)
					{
						const CGA::cga_item_info_t &iteminfo2 = itemsinfo.at(n);
						int nCur2 = 0, nMax2 = 0;
						g_pGameFun->ParseItemDurabilityEx(QString::fromStdString(iteminfo2.attr), nCur2, nMax2);
						if (iteminfo2.pos > 7 && iteminfo2.type == iteminfo.type && nCur2 > nMax2 * 0.01 * m_pEquipProtectCfg->dVal)
						{ //判断耐久是否符合要求 符合的话就换 不符合 判断下一个
							g_CGAInterface->MoveItem(iteminfo2.pos, iteminfo.pos, -1, bRes);
							if (m_pEquipProtectCfg->bRenEquip) //扔无耐久装备
							{
								g_CGAInterface->DropItem(iteminfo2.pos, bRes);
							}
							break;
						}
					}
				}
				else if (m_pEquipProtectCfg->nSwapEquip == 2) //下线
				{
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
	qDebug() << "等待线程结束";
	qDebug() << "...等待人物信息更新线程结束";
	if (m_characterFuture.isRunning())
	{
		m_characterFuture.waitForFinished();
	}
	qDebug() << "...等待物品处理线程结束";

	if (m_itemsFuture.isRunning())
	{
		m_itemsFuture.waitForFinished();
	}
	qDebug() << "...等待地图处理线程结束";

	if (m_mapFuture.isRunning())
		m_mapFuture.waitForFinished();
	qDebug() << "...等待逻辑事件处理线程结束";
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
		//上报角色信息到服务端
		if (pThis->m_lastUploadTime.elapsed() > 10 * 1000)
		{
			RpcSocketClient::getInstance().UploadGidData();
			RpcSocketClient::getInstance().UploadGidBankData();
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
		if (!g_pGameFun->IsInNormalState()) //解决卡战斗问题
		{
			//先不重置最后时间，上面有连接判断，在游戏才会进入此状态
			if (pThis->m_lastNormalState.elapsed() > 30 * 60 * 1000) //30分钟 在游戏 状态不正常30分钟，回到服务器界面
			{
				qDebug() << "状态不正常，超过30分钟，登出服务器!";
				g_pGameFun->LogoutServer();
				pThis->m_lastNormalState.restart();
			}
			QThread::msleep(1000);
			continue;
		}
		pThis->m_lastNormalState.restart();
		//增加一种 对话框时间  超过一小时没有对话框 认为卡对话框 ？挂机的  是不会对话  累计转向次数？
		//if (!pThis->m_bHasNPCDlg && pThis->m_lastRecvNpcDlgTime.elapsed() > 30 * 60 * 1000) //30分钟 在游戏没有返回对话框 状态不正常30分钟，回到服务器界面
		//{
		//	qDebug() << "未返回对话框，超过30分钟，登出服务器!";
		//	g_pGameFun->LogoutServer();
		//	pThis->m_lastRecvNpcDlgTime.restart();
		//	pThis->m_bHasNPCDlg = true; //恢复有对话框
		//	continue;
		//}

		//切换宠物
		pThis->AutoSetBattlePet();
		//扔宠物
		pThis->AutoDropPet();
		//超时被T检测
		pThis->OverTimeCheck();
		//自动治疗
		if (pThis->AutoHeal()) //优先治疗 没有治疗在下一步
		{
			continue;
		}
		//自动急救
		if (pThis->AutoFirstAid())
		{
			continue;
		}
		//装备保护
		if (pThis->AutoEquipProtect())
		{
		}
		//自动变身
		if (pThis->AutoTransformation())
		{
			continue;
		}
		//自动变装
		if (pThis->AutoCosplay())
		{
			continue;
		} //自动骑乘
		if (pThis->AutoPetRiding())
		{
			continue;
		}
		//自动捡物
		if (pThis->AutoPickItems())
		{
			continue;
		} //升级自动加点
		if (pThis->AutoUpgradeAddPoint())
		{
			continue;
		}

		//加个脚本停止判断
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
		{
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
		}
		{
			QMutexLocker locker(&pThis->m_workResMutex);
			quint64 curTime = GetTickCount();
			for (auto it = pThis->m_workResCache.begin(); it != pThis->m_workResCache.end();)
			{
				if ((curTime - it->first) > 30000)
				{
					it = pThis->m_workResCache.erase(it);
				}
				else
					++it;
			}
		}
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
//		if (pThis->AutoHeal())//优先治疗 没有治疗在下一步
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
	return;	//下载地图封了 先屏蔽
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
				item->exist = true;
				g_pGameFun->ParseItemDurability(item);
				ITObjectDataMgr::getInstance().StoreServerItemData(item);
				//qDebug() << QString::fromStdString(iteminfo.name) << QString::fromStdString(iteminfo.attr) << iteminfo.itemid << iteminfo.type << iteminfo.count << iteminfo.pos << iteminfo.assessed << iteminfo.level;
				newItemList.append(item);
			}
			m_pGameItems = newItemList;
		}
		//物品不为空 进行判断
		if (!itemsinfo.empty()) //pExistItems
		{
			if (g_pGameFun->IsInNormalState())
			{
				if (m_bAutoRenItems)
					RenItems();
				//移动限制丢物，防止高速移动
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
	//qDebug() << "按键事件:" << keyVal;
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
		case TDef_Quick_Key_Trade: //交易取出全部物品 并确定
		{
			g_CGAInterface->AddAllTradeItems(2); //只添加道具
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
		case TDef_Quick_Key_TradeNoAccept: //交易取出全部物品 不确定
		{
			g_CGAInterface->AddAllTradeItems(1);
			return;
		}
		case TDef_Quick_Key_SaveAll: //全存
		{
			g_pGameFun->SaveBagAllItemsToBank();
			return;
		}
		case TDef_Quick_Key_FetchAll: //全取 默认银行前20
		{
			g_pGameFun->WithdrawItemAllEx();
			return;
		}
		case TDef_Quick_Key_CallFz: //呼出辅助快捷键
		{
			emit signal_ctrl_app(3);
			break;
		}
		case TDef_Quick_Key_Encounter: //高速遇敌开关
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
		g_CGAInterface->SetWorkDelay(m_nWorkDelay);
		g_CGAInterface->SetWorkAcceleration(m_nWorkAcc);
		//g_CGAInterface->SetNoSwitchAnim(m_bNoSwitchAnim);

		CGA::cga_player_info_t info;
		if (g_CGAInterface->GetPlayerInfo(info))
		{
			pNewChar->name = QString::fromStdString(info.name);
			pNewChar->job = QString::fromStdString(info.job);
			pNewChar->level = info.level;
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
			pNewChar->titles.clear(); //那边在调用 只能读
			for (auto tmpTitle : info.titles)
			{
				pNewChar->titles.append(QString::fromStdString(tmpTitle));
			}
			for (auto title : pNewChar->titles)
			{
				if (g_pGameFun->m_sPrestigeList.contains(title))
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
				//获取合成物品信息
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

		//获取合成物品信息
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
	}
	m_pGameCharacter = pNewChar;

	if (ingame)
		emit signal_updateTrayToolTip(QString("%1 %2线").arg(m_pGameCharacter->name).arg(g_pGameFun->GetGameServerLine()));
	else
		emit signal_updateTrayToolTip(QString("%1 离线").arg(m_pGameCharacter->name));
	emit NotifyGameCharacterInfo(pNewChar);
	//emit NotifyGameItemsInfo(new);
	emit NotifyGamePetsInfo(newPetList);
	emit NotifyGameSkillsInfo(newSkillList);
	emit NotifyGameCompoundInfo(newCompoundList);
}

void GameCtrl::OnGetTeamData()
{
	if (m_isInBattle)
		return;
	if (m_lastUpdateTeamTime.elapsed() < 5000) //5秒刷新
		return;
	auto teamPlayerDatas = g_pGameFun->GetTeamPlayers();
	emit NotifyTeamInfo(teamPlayerDatas);
	m_lastUpdateTeamTime.restart();
}
//太占资源，msgpack和QSharePtr都占
void GameCtrl::OnGetMapData()
{
	int ingame = 0;
	//qDebug("OnNotifyBattleAction.");
	if (!g_CGAInterface->IsConnected() || !g_CGAInterface->IsInGame(ingame) || !ingame)
		return;
	std::string mapName;
	float x = 0, y = 0;
	int index1 = 0, index2 = 0, mapIndex = 0;
	g_CGAInterface->GetMapXYFloat(x, y);
	std::string filemap;
	if (g_CGAInterface->GetMapIndex(index1, index2, mapIndex, filemap))
	{

		//放入下面 同步 否则地图编号和名称不同步 一直刷新数据库
		g_CGAInterface->GetMapName(mapName);
		QString fileMapPath = QString::fromStdString(filemap);
		if (g_pGameFun->IsInNormalState() && fileMapPath.contains("map\\0")) //0下面地图 才进行缓存 迷宫不缓存
		{
			ITObjectDataMgr::getInstance().StoreServerMapData(QString::fromStdString(mapName), mapIndex);
		}
	}
	//这部分信号 目前也只有地图用了，用来刷新label信息的
	emit NotifyGetMapInfo(QString::fromStdString(mapName), index1, index2, mapIndex, (int)(x / 64.0f), (int)(y / 64.0f));
	if (!m_bMapIsVisible && !m_bEnabledDataDisplayUi) //不可见 并且没有启用实时刷新周边地图数据 则不进行地图下载
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
		m_isInBattle = false;
	//QTime espTime;
	//espTime = QTime::currentTime();
	//espTime.restart();
	//qDebug() << espTime.currentTime();
	int ingame = 0;
	if (!g_CGAInterface->IsConnected() || !g_CGAInterface->IsInGame(ingame) || !ingame)
		return;

	GetBattleUnits();
	//CBattleWorker::getInstace()->OnNotifyGetBattleInfo(m_pBattleUnits); //更新战斗单位信息
	//CBattleWorker::getInstace()->OnNotifyGetSkillsInfo(m_pGameSkills);	//更新技能信息 平常不进行更新
	//CBattleWorker::getInstace()->OnNotifyGetPetsInfo(m_pGamePets);		//更新技能信息 平常不进行更新
	//CBattleWorker::getInstace()->OnNotifyGetItemsInfo(m_pGameItems);	//更新技能信息 平常不进行更新
	//CBattleWorker::getInstace()->OnNotifyGetItemsInfo(m_pGameItems);	//更新技能信息 平常不进行更新
	CBattleWorker::getInstace()->OnBattleAction(flags);

	/*  第1级：使用脚本时，脚本设置由脚本控制战斗；
			第2级：有1级敌人和Boos战停止战斗；
			第3级：遇敌就逃跑；
			第4级：无1级敌人时逃跑；
			第5级：遇指定怪逃跑；
			第6级：使用人魔少保护技；
			第7级：使用宠魔少保护技；
			第8级：对有1级宠时的第一回合；
			第9级：对有1级宠时的非第一回合；
			第10级：对队友使用加血技；
			第11级：第二回合使用人血少保护技；
			第12级：使用人血少保护技；
			第13级：第二回合使用宠血少保护技；
			第14级：使用宠血少保护技；
			第15级：10种基本遇敌状态设置。*/
	//qDebug() << "战斗过去的时间" <<  espTime.elapsed();
}
//连接状态 1 2登录成功 10000（NewUI_ShowLostConnectionDialog） 0（貌似断线？） 登录失败 3没有人物
void GameCtrl::OnNotifyConnectionState(int state, QString msg)
{
	m_lastGameConnState = state;
	m_lastGameConnMsg = msg;

	qDebug() << state << msg;
	if ((state == 10000 || state == 0) && !msg.isEmpty())
	{
		if (msg.contains("角色数据读取失败")) //这个id跳过
			"角色数据读取失败，其他窗口已登录！";
	}
	else if (state == 1 || state == 2)
	{
		//登录成功
		qDebug() << "登录成功";
	}
	else if (state == 3)
	{
		qDebug() << "没有游戏角色！";
	}
}

void GameCtrl::NotifyChatMsgCallback(CGA::cga_chat_msg_t msg)
{
	//unitid == -1  系统消息
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
	//有脚本执行情况下，防止用户过快调用函数，延时发送，没有脚本执行，立即发送
	if (m_nRunScriptState == SCRIPT_CTRL_RUN)
	{
		//	emit g_pGameCtrl->DelaySendNpcDialog(dlg);

		//		qDebug() << "NotifyNPCDialogCallback";
		Sleep(m_nScriptDelayTime + 100);
		emit g_pGameCtrl->NotifyNPCDialog(dlg); //程序内部使用的
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
		emit g_pGameCtrl->NotifyNPCDialog(dlg); //程序内部使用的
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
	emit NotifyPlayerMenu(menu);
}
//工作状态回调
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
	////是否成功working->success 技能类型type 获得物品name 技能是否升级levelup 物品类型？imgid status
	NotifyWorkingResult(working);
}

void GameCtrl::NotifyServerShutdown(int port)
{
	qDebug() << "NotifyServerShutdown" << port;
	g_pGameCtrl->StopUpdateTimer();
}

void GameCtrl::NotifyTradeStuffsCallback(CGA::cga_trade_stuff_info_t tradeInfo)
{
	QSharedPointer<CGA::cga_trade_stuff_info_t> notifyItems(new CGA::cga_trade_stuff_info_t);
	notifyItems->gold = tradeInfo.gold;
	notifyItems->type = tradeInfo.type;
	notifyItems->pet = tradeInfo.pet;
	notifyItems->petskills = tradeInfo.petskills;
	notifyItems->items = tradeInfo.items;
	//有脚本执行情况下，防止用户过快调用函数，延时发送，没有脚本执行，立即发送
	if (m_nRunScriptState == SCRIPT_CTRL_RUN)
	{
		Sleep(m_nScriptDelayTime + 100);
		emit NotifyTradeStuffs(notifyItems); //程序内部使用的
	}
	else
		emit NotifyTradeStuffs(notifyItems); //程序内部使用的
	//test
	qDebug() << "金币：" << tradeInfo.gold << "type" << tradeInfo.type << "pet" << QString::fromStdString(tradeInfo.pet.realname) << " name:" << QString::fromStdString(tradeInfo.pet.name);
	qDebug() << "宠物技能：indx " << tradeInfo.petskills.index;
	for (auto skillName : tradeInfo.petskills.skills)
	{
		qDebug() << "名称：" << QString::fromStdString(skillName);
	}

	for (auto item : tradeInfo.items)
	{
		qDebug() << "物品名称：" << QString::fromStdString(item.name) << " 数量：" << item.count;
	}
}

void GameCtrl::NotifyTradeDialogCallback(CGA::cga_trade_dialog_t dlg)
{
	qDebug() << "NotifyTradeDialogCallback" << QString::fromStdString(dlg.name) << dlg.level;
	QSharedPointer<CGA::cga_trade_dialog_t> notifydlg(new CGA::cga_trade_dialog_t);
	notifydlg->name = dlg.name;
	notifydlg->level = dlg.level;
	//有脚本执行情况下，防止用户过快调用函数，延时发送，没有脚本执行，立即发送
	if (m_nRunScriptState == SCRIPT_CTRL_RUN)
	{
		Sleep(m_nScriptDelayTime + 200);
		emit NotifyTradeDialog(notifydlg); //程序内部使用的
	}
	else
		emit NotifyTradeDialog(notifydlg); //程序内部使用的
}

void GameCtrl::NotifyTradeStateCallback(int state)
{
	qDebug() << "交易状态 " << state;
	if (m_nRunScriptState == SCRIPT_CTRL_RUN)
	{
		Sleep(m_nScriptDelayTime + 100);
		emit NotifyTradeState(state); //程序内部使用的
	}
	else
		emit NotifyTradeState(state); //程序内部使用的
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

	emit NotifyUnitMenu(menu);
}

void GameCtrl::NotifyConnectionStateCallback(CGA::cga_conn_state_t msg)
{
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
	//自动补血魔
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
				//如果身上金钱<加魔钱 只加血
				if (bNeedHP && (!bNeedMP || playerinfo.gold < playerinfo.maxmp - playerinfo.mp))
				{
					g_CGAInterface->ClickNPCDialog(0, 2, result);
				}
				else if (bNeedMP && playerinfo.gold >= playerinfo.maxmp - playerinfo.mp) //加魔钱够 回复魔和血
				{
					g_CGAInterface->ClickNPCDialog(0, 0, result);
				}
			}
			if (!result && g_CGAInterface->GetPetsInfo(petsinfo))
			{
				if (g_pGameFun->NeedPetSupply(petsinfo)) //回复宠物
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
				g_CGAInterface->ClickNPCDialog(4, -1, result); //4 是 8否 32下一步 1确定
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
			// 里谢里雅堡 商人NPCID=7962 DLGID=333 334 --1112
			//	qDebug() <<  dlg->npc_id << dlg->dialog_id << dlg->options << dlg->type << dlg->message;
			GameItemList pSaleItemList = g_pGameCtrl->GetSaleItemList();
			GameItemList pItemList = m_pGameItems;
			CGA::cga_sell_items_t cgaSaleItems;
			GameItemList pExistItemList; //重复判断
			foreach (auto pSaleItem, pSaleItemList)
			{
				if (pSaleItem->isSale)
				{
					foreach (auto pItem, pItemList) //获取身上物品
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
									if (tmpCount < 1) //当前数量 除以贩卖最小数量 如果不足 跳过
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
				//		qDebug() << "GameCtrl 没有要卖的物品！";
				return;
			}
			if (dlg->type == 5 /*&& dlg->message.indexOf("卖") >= 0*/)
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
}

void GameCtrl::OnNotifyPlayerMenu(QSharedPointer<CGA::cga_player_menu_items_t> menu)
{
	bool bResult = false;
	do
	{
		if (m_nWorkType == TWork_Heal || m_nWorkType == TWork_FirstAid) //治疗 或者急救
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
					g_CGAInterface->PlayerMenuSelect(menu->at(0).index, menuText, bResult); //第一个
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
		if (m_nWorkType == TWork_Heal || m_nWorkType == TWork_FirstAid) //治疗 或者急救
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
				//分开 治疗增加判断
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
				//分开 治疗增加判断
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
				return;
			}
		}
	}
}

void GameCtrl::OnNotifyChatMsg(int unitid, QString msg, int size, int color)
{
	if (m_uLastUseItemTime.elapsed() < 3000)
		return;
	if (!g_pGameFun->IsInNormalState())
		return;
	if (m_bAutoEatDeepBlue)
	{
		if (unitid == -1 && msg.contains("道具的效果消失了")) //系统消息
		{
			foreach (auto pItem, m_pGameItems) //获取身上物品
			{
				if (pItem->exist && pItem->pos >= 8 && pItem->id == 18526)
				{
					bool bRes = false;
					g_CGAInterface->UseItem(pItem->pos, bRes);
					m_uLastUseItemTime.restart();
					break;
				}
			}
			return;
		}
	}
	if (m_bAutoEatDogFood) //吃狗粮
	{
		//if (unitid == -1 && msg.contains("道具的效果消失了")) //系统消息
		//{
		//	foreach (auto pItem, m_pGameItems) //获取身上物品
		//	{
		//		if (pItem->exist && pItem->pos >= 8 && pItem->id == 18526)
		//		{
		//			bool bRes = false;
		//			g_CGAInterface->UseItem(pItem->pos, bRes);
		//			m_uLastUseItemTime.restart();
		//			break;
		//		}
		//	}
		//	return;
		//}
	}
	if (m_bAutoEatTimeCrystal) //吃时水
	{
		CGA::cga_player_info_t info;
		if (g_CGAInterface->GetPlayerInfo(info))
		{
			int hours = (info.punchclock / 1000 / 3600);
			int mins = (info.punchclock / 1000 - hours * 3600) / 60;
			//			auto clock = std::floor(info.punchclock / 1000 / 3600);	//小时
			if (mins < 2) //1分钟 吃
			{
				foreach (auto pItem, m_pGameItems) //获取身上物品
				{
					if (pItem->exist && pItem->pos >= 8 && pItem->name.contains("时间水晶Lv"))
					{
						bool bRes = false;
						g_CGAInterface->UseItem(pItem->pos, bRes); //没有等待对话框
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
			ShowWindow(m_hGameHwnd, SW_SHOWNORMAL);
		}
		else
		{
			ShowWindow(m_hGameHwnd, SW_MINIMIZE);
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

void GameCtrl::GetBattleUnits()
{
	g_pAutoBattleCtrl->ResetBattleAnalysisData();
	//重置战斗中各单位信息
	for (int i = 0; i < 20; ++i)
	{
		m_pBattleUnits[i]->exist = false;
		m_pBattleUnits[i]->isback = false;
		m_pBattleUnits[i]->debuff = 0;
		m_pBattleUnits[i]->petriding_modelid = 0;
	}

	//计算宠物等级用
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
		if (frontPetPosList.contains(playerPos)) //反着来
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
			m_pBattleUnits[u.pos]->exist = true;
			m_pBattleUnits[u.pos]->name = QString::fromStdString(u.name);
			m_pBattleUnits[u.pos]->hp = u.curhp;
			m_pBattleUnits[u.pos]->maxhp = u.maxhp;
			m_pBattleUnits[u.pos]->mp = u.curmp;
			m_pBattleUnits[u.pos]->maxmp = u.maxmp;
			m_pBattleUnits[u.pos]->level = u.level;
			m_pBattleUnits[u.pos]->flags = u.flags;
			m_pBattleUnits[u.pos]->pos = u.pos;
			m_pBattleUnits[u.pos]->petriding_modelid = u.petriding_modelid;

			m_pBattleUnits[u.pos]->debuff = 0;
			m_pBattleUnits[u.pos]->multi_hp = 0;
			m_pBattleUnits[u.pos]->multi_maxhp = 0;
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
			/*	if (u.pos == petPos && pet)	//宠物不用更新 默认获取的就是更新的
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
	emit NotifyGameBattlesInfo(m_pBattleUnits);
}

void GameCtrl::SetScriptRunState(int state)
{
	qDebug() << "SetScriptRunState" << state;
	m_nRunScriptState = state;
}
