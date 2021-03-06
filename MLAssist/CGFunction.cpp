#include "CGFunction.h"
#include "../include/QtXlsx/xlsxcellrange.h"
#include "../include/QtXlsx/xlsxdocument.h"
#include "../include/QtXlsx/xlsxformat.h"
#include "../include/QtXlsx/xlsxworksheet.h"
#include "./AStar/AStar.h"
#include "GPCalc.h"
#include "GameCtrl.h"
#include "ITObjectDataMgr.h"
#include "QEventLoop.h"
#include "RpcSocketClient.h"
#include "Tsp.h"
#include "stdafx.h"
#include <QDebug>
#pragma comment(lib, "../lib/QtXlsx/Qt5Xlsx.lib")

//#include <opencv2/highgui.hpp>
//#include <opencv2/opencv.hpp>

//using namespace cv;
using namespace std;
using namespace QXlsx;
//#pragma comment(lib, "../lib/opencv453/opencv_world453.lib")
CGFunction *CGFunction::GetInstance()
{
	static CGFunction instance;
	return &instance;
}

CGFunction::CGFunction()
{
	m_pUserDlg = new UserDefDialog;
	m_pUserComboBoxDlg = new UserDefComboBoxDlg;
	m_pUserCheckBoxDlg = new UserDefCheckBoxDlg;
	connect(this, &CGFunction::signal_stopScript, m_pUserDlg, &QDialog::close);
	connect(this, &CGFunction::signal_stopScript, m_pUserComboBoxDlg, &QDialog::close);
	connect(this, &CGFunction::signal_stopScript, m_pUserCheckBoxDlg, &QDialog::close);

	connect(m_pUserDlg, &UserDefDialog::signal_input_val, this, &CGFunction::signal_returnUserInputVal);
	connect(m_pUserComboBoxDlg, &UserDefComboBoxDlg::signal_input_val, this, &CGFunction::signal_returnUserInputVal);
	connect(m_pUserCheckBoxDlg, &UserDefCheckBoxDlg::signal_input_val, this, &CGFunction::signal_returnUserInputVal);

	m_resetTimer.setSingleShot(true);
	connect(&m_resetTimer, SIGNAL(timeout()), this, SLOT(RestFun()));
	connect(this, SIGNAL(signal_startTimer()), this, SLOT(StartTimer()));
	connect(this, SIGNAL(signal_StartAutoEncounterEnemy()), this, SLOT(OnStartAutoEncounterEnemyThread()));
	connect(this, SIGNAL(signal_userInputDialog(const QString &, const QString &)), this, SLOT(OnPopupUserInputDialog(const QString &, const QString &)));
	connect(this, SIGNAL(signal_userCheckBoxDialog(const QString &, const QString &)), this, SLOT(OnPopupUserCheckBoxDialog(const QString &, const QString &)));
	connect(this, SIGNAL(signal_userComboBoxDialog(const QString &, const QStringList &, const QString &)), this,
			SLOT(OnPopupUserComboBoxDialog(const QString &, const QStringList &, const QString &)));
	connect(g_pGameCtrl, &GameCtrl::NotifyChatMsg, this, &CGFunction::OnNotifyChatMsg, Qt::ConnectionType::QueuedConnection);
	m_petRaceMap.insert(0, "?????????");
	m_petRaceMap.insert(1, "??????");
	m_petRaceMap.insert(2, "?????????");
	m_petRaceMap.insert(3, "?????????");
	m_petRaceMap.insert(4, "?????????");
	m_petRaceMap.insert(5, "?????????");
	m_petRaceMap.insert(6, "?????????");
	m_petRaceMap.insert(7, "?????????");
	m_petRaceMap.insert(8, "?????????");
	m_petRaceMap.insert(9, "?????????");

	m_tradeState.insert(TRADE_STATE_CANCEL, "????????????");  //??????????????????
	m_tradeState.insert(TRADE_STATE_READY, "????????????");	  //????????????
	m_tradeState.insert(TRADE_STATE_CONFIRM, "????????????"); //????????????
	m_tradeState.insert(TRADE_STATE_SUCCEED, "????????????");
	m_workTypeForText.insert(TWork_Identify, "??????");
	m_workTypeForText.insert(TWork_Digging, "??????");
	m_workTypeForText.insert(TWork_Hunt, "??????");
	m_workTypeForText.insert(TWork_Lumber, "??????");
	m_workTypeForText.insert(TWork_RepairWeapon, "????????????");
	m_workTypeForText.insert(TWork_RepairArmor, "????????????");
	m_workTypeForText.insert(TWork_Heal, "??????");
	m_workTypeForText.insert(TWork_FirstAid, "??????");

	m_characterMap.insert("???", CHARACTER_Battle);
	m_characterMap.insert("???", CHARACTER_Chat);
	m_characterMap.insert("???", CHARACTER_Troop);
	m_characterMap.insert("???", CHARACTER_Card);
	m_characterMap.insert("???", CHARACTER_Trade);
	m_characterMap.insert("???", CHARACTER_Family);
	m_characterMap.insert("??????", CHARACTER_Avatar_Public);
	m_characterMap.insert("??????", CHARACTER_Battle_Position);

	m_sysConfigMap.insert("timer", TSysConfigSet_Timer);
	m_sysConfigMap.insert("?????????", TSysConfigSet_AutoDrop);
	m_sysConfigMap.insert("?????????", TSysConfigSet_AutoPile);
	m_sysConfigMap.insert("?????????", TSysConfigSet_AutoPick);
	m_sysConfigMap.insert("????????????", TSysConfigSet_FollowPickItem);
	m_sysConfigMap.insert("?????????", TSysConfigSet_AutoSale);
	m_sysConfigMap.insert("????????????", TSysConfigSet_AutoSupply);
	m_sysConfigMap.insert("????????????", TSysConfigSet_AutoEncounter);
	m_sysConfigMap.insert("????????????", TSysConfigSet_EncounterType);
	m_sysConfigMap.insert("????????????", TSysConfigSet_EncounterSpeed);
	m_sysConfigMap.insert("????????????", TSysConfigSet_AutoBattle);
	m_sysConfigMap.insert("????????????", TSysConfigSet_HighSpeedBattle);
	m_sysConfigMap.insert("????????????", TSysConfigSet_HighSpeedDelay);
	m_sysConfigMap.insert("????????????", TSysConfigSet_BattleDelay);
	m_sysConfigMap.insert("????????????", TSysConfigSet_AllEncounterEscape);
	m_sysConfigMap.insert("???????????????", TSysConfigSet_NoLv1Escape);
	m_sysConfigMap.insert("????????????????????????", TSysConfigSet_EnemyAvgLvEscape);
	m_sysConfigMap.insert("?????????????????????", TSysConfigSet_EnemyCountEscape);
	m_sysConfigMap.insert("?????????????????????", TSysConfigSet_TeamCountEscape);
	m_sysConfigMap.insert("???????????????", TSysConfigSet_NoPetDoubleAction);
	m_sysConfigMap.insert("????????????", TSysConfigSet_NoPetAttack);
	m_sysConfigMap.insert("????????????", TSysConfigSet_NoPetGuard);
	m_sysConfigMap.insert("????????????", TSysConfigSet_CharacterSwitch);
	m_sysConfigMap.insert("????????????", TSysConfigSet_MoveSpeed);
	m_sysConfigMap.insert("????????????", TSysConfigSet_PlayerTitle);
	m_sysConfigMap.insert("???????????????", TSysConfigSet_AutoEatDeepBlue);
	m_sysConfigMap.insert("???????????????", TSysConfigSet_AutoEatDogFood);
	m_sysConfigMap.insert("???????????????", TSysConfigSet_AutoEatTimeCrystal);
	m_sysConfigMap.insert("????????????", TSysConfigSet_EquipProtect);
	m_sysConfigMap.insert("????????????", TSysConfigSet_AutoCure);
	m_sysConfigMap.insert("????????????", TSysConfigSet_AutoFirstAid);
	m_sysConfigMap.insert("????????????????????????", TSysConfigSet_ScriptStillRestart);
	m_sysConfigMap.insert("??????????????????", TSysConfigSet_ScriptStopRestart);
	m_sysConfigMap.insert("????????????????????????", TSysConfigSet_ScriptStopLogbackRestart);
	m_sysConfigMap.insert("????????????????????????", TSysConfigSet_ScriptStillLogout);

	m_returnGameDataHash.insert("gid", TRet_Game_Gid);
	m_returnGameDataHash.insert("hp", TRet_Game_Hp);
	m_returnGameDataHash.insert("???", TRet_Game_Hp);
	m_returnGameDataHash.insert("mp", TRet_Game_Mp);
	m_returnGameDataHash.insert("???", TRet_Game_Mp);
	m_returnGameDataHash.insert("maxhp", TRet_Game_MaxHp);
	m_returnGameDataHash.insert("?????????", TRet_Game_MaxHp);
	m_returnGameDataHash.insert("maxmp", TRet_Game_MaxMp);
	m_returnGameDataHash.insert("?????????", TRet_Game_MaxMp);
	m_returnGameDataHash.insert("level", TRet_Game_Level);
	m_returnGameDataHash.insert("??????", TRet_Game_Level);
	m_returnGameDataHash.insert("exp", TRet_Game_Exp);
	m_returnGameDataHash.insert("??????", TRet_Game_Exp);
	m_returnGameDataHash.insert("maxexp", TRet_Game_MaxExp);
	m_returnGameDataHash.insert("????????????", TRet_Game_MaxExp);
	m_returnGameDataHash.insert("health", TRet_Game_Health);
	m_returnGameDataHash.insert("??????", TRet_Game_Health);
	m_returnGameDataHash.insert("soul", TRet_Game_Soul);
	m_returnGameDataHash.insert("??????", TRet_Game_Soul);
	m_returnGameDataHash.insert("name", TRet_Game_Name);
	m_returnGameDataHash.insert("??????", TRet_Game_Name);
	m_returnGameDataHash.insert("??????", TRet_Game_Gold);
	m_returnGameDataHash.insert("gold", TRet_Game_Gold);
	m_returnGameDataHash.insert("bankgold", TRet_Game_BankGold);
	m_returnGameDataHash.insert("????????????", TRet_Game_BankGold);
	m_returnGameDataHash.insert("??????", TRet_Game_Punchclock);
	m_returnGameDataHash.insert("punchclock", TRet_Game_Punchclock);
	m_returnGameDataHash.insert("????????????", TRet_Game_PunchclockState);
	m_returnGameDataHash.insert("punchclockstate", TRet_Game_PunchclockState);
	m_returnGameDataHash.insert("??????", TRet_Game_Job);
	m_returnGameDataHash.insert("job", TRet_Game_Job);
	m_returnGameDataHash.insert("????????????", TRet_Game_PlayerRank);
	m_returnGameDataHash.insert("??????", TRet_Game_Profession);
	m_returnGameDataHash.insert("profession", TRet_Game_Profession);
	m_returnGameDataHash.insert("??????", TRet_Game_Prestige);
	m_returnGameDataHash.insert("prestige", TRet_Game_Prestige);
	m_returnGameDataHash.insert("??????", TRet_Game_Prestige);
	m_returnGameDataHash.insert("????????????", TRet_Game_PrestigeLv);
	m_returnGameDataHash.insert("????????????", TRet_Game_PrestigeLv);
	m_returnGameDataHash.insert("??????", TRet_Game_Loc);
	m_returnGameDataHash.insert("loc", TRet_Game_Loc);
	m_returnGameDataHash.insert("????????????", TRet_Game_PetCount);
	m_returnGameDataHash.insert("petcount", TRet_Game_PetCount);
	m_returnGameDataHash.insert("??????", TRet_Game_CurLine);
	m_returnGameDataHash.insert("line", TRet_Game_CurLine);
	m_returnGameDataHash.insert("??????", TRet_Game_CurServelLine);
	m_returnGameDataHash.insert("serverline", TRet_Game_CurServelLine);
	m_returnGameDataHash.insert("??????", TRet_Game_Loyality);
	m_returnGameDataHash.insert("loyality", TRet_Game_Loyality);
	m_returnGameDataHash.insert("??????", TRet_Game_PetGrade);
	m_returnGameDataHash.insert("grade", TRet_Game_PetGrade);
	m_returnGameDataHash.insert("??????", TRet_Game_PetState);
	m_returnGameDataHash.insert("petstate", TRet_Game_PetState);
	m_returnGameDataHash.insert("?????????", TRet_Game_PetChangeState);
	m_returnGameDataHash.insert("petchangestate", TRet_Game_PetChangeState);
	m_returnGameDataHash.insert("petchangename", TRet_Game_PetChangeName);
	m_returnGameDataHash.insert("??????", TRet_Game_PetChangeName);
	m_returnGameDataHash.insert("4?????????", TRet_Game_OathGroup);
	m_returnGameDataHash.insert("??????", TRet_Game_Sex);
	m_returnGameDataHash.insert("??????", TRet_Game_ImageID);
	m_returnGameDataHash.insert("????????????", TRet_Game_GamePID);
	m_returnGameDataHash.insert("????????????", TRet_Game_GamePort);
	m_returnGameDataHash.insert("??????", TRet_Game_PlayerRole);
	m_returnGameDataHash.insert("????????????", TRet_Game_PlayerRole);
	m_returnGameDataHash.insert("????????????", TRet_Game_CheckHavePetBook);
	m_returnGameDataHash.insert("????????????", TRet_Game_PetBookInfo);

	m_playerActionHash.insert("pk", TCharacter_Action_PK);
	m_playerActionHash.insert("????????????", TCharacter_Action_JOINTEAM);
	m_playerActionHash.insert("??????", TCharacter_Action_EXCAHNGECARD);
	m_playerActionHash.insert("??????", TCharacter_Action_TRADE);
	m_playerActionHash.insert("????????????", TCharacter_Action_KICKTEAM);
	m_playerActionHash.insert("????????????", TCharacter_Action_LEAVETEAM);
	m_playerActionHash.insert("????????????", TCharacter_Action_TRADE_CONFIRM);
	m_playerActionHash.insert("????????????", TCharacter_Action_TRADE_REFUSE);
	m_playerActionHash.insert("??????", TCharacter_Action_TEAM_CHAT);
	m_playerActionHash.insert("????????????", TCharacter_Action_REBIRTH_ON);
	m_playerActionHash.insert("????????????", TCharacter_Action_REBIRTH_OFF);
	m_playerActionHash.insert("??????", TCharacter_Action_Gesture);

	m_sPrestigeMap.insert("??????", -3);
	m_sPrestigeMap.insert("???????????????", -2);
	m_sPrestigeMap.insert("???????????????", -1);
	m_sPrestigeMap.insert("???????????????", 0);
	m_sPrestigeMap.insert("???????????????", 1);
	m_sPrestigeMap.insert("??????????????????", 2);
	m_sPrestigeMap.insert("???????????????", 3);
	m_sPrestigeMap.insert("???????????????", 4);
	m_sPrestigeMap.insert("???????????????", 5);
	m_sPrestigeMap.insert("????????????", 6);
	m_sPrestigeMap.insert("???????????????", 7);
	m_sPrestigeMap.insert("???????????????", 8);
	m_sPrestigeMap.insert("???????????????", 9);
	m_sPrestigeMap.insert("???????????????", 10);
	m_sPrestigeMap.insert("????????????", 11);
	m_sPrestigeMap.insert("???????????????", 1);
	m_sPrestigeMap.insert("??????????????????", 2);
	m_sPrestigeMap.insert("??????????????????", 3);
	m_sPrestigeMap.insert("???????????????", 4);
	m_sPrestigeMap.insert("???????????????", 5);
	m_sPrestigeMap.insert("??????????????????", 6);
	m_sPrestigeMap.insert("??????", 7);
	m_sPrestigeMap.insert("??????", 8);
	m_sPrestigeMap.insert("???????????????", 9);
	m_sPrestigeMap.insert("???????????????", 10);
	m_sPrestigeMap.insert("???????????????", 11);

	//readCreateRandomNameJson();
	readProfessionJson();
	readTitleJson();
}

CGFunction::~CGFunction()
{
	SafeDelete(m_pUserComboBoxDlg);
	SafeDelete(m_pUserDlg);
	SafeDelete(m_pUserCheckBoxDlg);
}

void CGFunction::StopFun()
{
	m_bStop = true;
	emit signal_stopScript();
	//	 QTimer::singleShot(3000, g_pGameFun, SLOT(RestFun()));
	//	 emit signal_startTimer();	//?????????????????????????????? ????????????????????????
}

void CGFunction::RestFun()
{
	m_bStop = false;
	qDebug() << "ResetFun";
	m_bMoveing = false;
	m_navigatorLoopCount = 0; //???????????????????????????
	m_bMapMoveing = false;
	m_bAutoEncounterEnemy = false;
	m_bWorking = false;
}

void CGFunction::StartTimer()
{
	m_resetTimer.start(3000);
}

void CGFunction::OnStartAutoEncounterEnemyThread()
{
	qDebug() << "????????????????????????";
	if (m_encounterFuture.isRunning())
	{
		qDebug() << "?????????????????????????????????????????????";
		return;
	}
	m_encounterFuture = QtConcurrent::run(AutoEncounterEnemyThread, this);
}

void CGFunction::OnPopupUserInputDialog(const QString &sMsg, const QString &sVal)
{
	m_pUserDlg->setLabelText(sMsg);
	if (m_scriptInputVar.contains(sMsg))
		m_pUserDlg->setDefaultVal(m_scriptInputVar.value(sMsg).toString());
	else
		m_pUserDlg->setDefaultVal(sVal);
	m_pUserDlg->show();
}

void CGFunction::OnPopupUserCheckBoxDialog(const QString &sMsg, const QString &sVal)
{
	m_pUserCheckBoxDlg->setLabelText(sMsg);
	if (m_scriptInputVar.contains(sMsg))
		m_pUserCheckBoxDlg->setDefaultVal(m_scriptInputVar.value(sMsg).toString());
	else
		m_pUserCheckBoxDlg->setDefaultVal(sVal);
	m_pUserCheckBoxDlg->show();
}

void CGFunction::OnPopupUserComboBoxDialog(const QString &sMsg, const QStringList &sVal, const QString &sDefVal)
{
	m_pUserComboBoxDlg->setLabelText(sMsg);
	m_pUserComboBoxDlg->setComboBoxItems(sVal);
	if (m_scriptInputVar.contains(sMsg))
		m_pUserComboBoxDlg->setCurrentSelect(m_scriptInputVar.value(sMsg).toString());
	else
		m_pUserComboBoxDlg->setCurrentSelect(sDefVal);
	m_pUserComboBoxDlg->show();
}

void CGFunction::OnNotifyChatMsg(int unitid, QString msg, int size, int color)
{
	QMutexLocker lock(&m_charMutex);
	if (unitid == -1) //<0??????  >0??????????????????
	{
		m_systemCueList.append(qMakePair(GetTickCount(), msg));
	}
	if (m_systemCueList.size() > 100)
	{
		m_systemCueList = m_systemCueList.mid(m_systemCueList.size() - 100);
	}
	if (m_chatMsgList.size() > 100)
	{
		m_chatMsgList = m_chatMsgList.mid(m_chatMsgList.size() - 100);
	}
	m_chatMsgList.append(qMakePair(GetTickCount(), QStringList() << QString::number(unitid) << msg));
}

void CGFunction::OnRecvTopicMsg(const QString &topicName, const QString &msg)
{
	QMutexLocker locker(&m_topicMutex);
	if (m_topicMsg.size() > 100)
	{
		m_topicMsg = m_topicMsg.mid(m_topicMsg.size() - 100);
	}
	m_topicMsg.append(qMakePair<QString, QString>(topicName, msg));
}

void CGFunction::Logout()
{
	g_CGAInterface->LogBack();
	Sleep(2000);
	WaitInNormalState();
}

void CGFunction::LogoutServer()
{
	g_CGAInterface->LogOut();
}

QVariant CGFunction::UserInputDialog(QString sMsg, QString sVal)
{
	if (m_bUserDlgUseDefault)
	{
		if (m_scriptInputVar.contains(sMsg))
			return m_scriptInputVar.value(sMsg);
		return sVal; //????????????int
	}
	QMutex mutex;
	emit g_pGameFun->signal_userInputDialog(sMsg, sVal);
	QVariant nRetVal = 0;
	QEventLoop loop;
	qDebug() << "WaitUserInputDialog";

	QTimer::singleShot(600000, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameFun, &CGFunction::signal_returnUserInputVal, [&](const QVariant &nVal)
			{
				if (mutex.tryLock())
				{
					nRetVal = nVal;
					if (loop.isRunning())
						loop.quit(); //????????????????????? loop.exec()????????????????????????????????? ???????????????
					mutex.unlock();
				}
			});
	loop.exec();
	QMutexLocker locker(&mutex);
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	qDebug() << "WaitUserInputDialog Succe" << nRetVal;
	return nRetVal;
}

QVariant CGFunction::UserCheckBoxDialog(QString sMsg, QString sVal)
{
	if (m_bUserDlgUseDefault)
	{
		if (m_scriptInputVar.contains(sMsg))
			return m_scriptInputVar.value(sMsg);
		return sVal; //????????????int
	}
	QMutex mutex;
	emit g_pGameFun->signal_userCheckBoxDialog(sMsg, sVal);
	QVariant nRetVal = 0;
	QEventLoop loop;
	qDebug() << "WaitUserInputDialog";

	QTimer::singleShot(600000, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameFun, &CGFunction::signal_returnUserInputVal, [&](const QVariant &nVal)
			{
				if (mutex.tryLock())
				{
					nRetVal = nVal;
					if (loop.isRunning())
						loop.quit(); //????????????????????? loop.exec()????????????????????????????????? ???????????????
					mutex.unlock();
				}
			});
	loop.exec();
	QMutexLocker locker(&mutex);
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	qDebug() << "WaitUserInputDialog Succe" << nRetVal;
	return nRetVal;
}

QVariant CGFunction::UserComboBoxDialog(QString sMsg, QStringList sVal, QString sDefVal)
{
	if (m_bUserDlgUseDefault)
	{
		if (m_scriptInputVar.contains(sMsg))
			return m_scriptInputVar.value(sMsg);
		else if (sDefVal.isEmpty())
			return sVal.size() > 0 ? sVal.at(0) : "";
		else
			return sDefVal;
	}
	QMutex mutex;
	emit g_pGameFun->signal_userComboBoxDialog(sMsg, sVal, sDefVal);
	QVariant nRetVal = 0;
	QEventLoop loop;
	qDebug() << "WaitUserInputDialog";

	QTimer::singleShot(600000, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameFun, &CGFunction::signal_returnUserInputVal, [&](const QVariant &nVal)
			{
				if (mutex.tryLock())
				{
					nRetVal = nVal;
					if (loop.isRunning())
						loop.quit(); //????????????????????? loop.exec()????????????????????????????????? ???????????????
					mutex.unlock();
				}
			});
	loop.exec();
	QMutexLocker locker(&mutex);
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	qDebug() << "WaitUserInputDialog Succe" << nRetVal;
	return nRetVal;
}

int CGFunction::FindItemPos(const QString &sItemName)
{
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{ //??????????????????
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7 && iteminfo.name == sItemName.toStdString())
			{
				return iteminfo.pos;
			}
		}
	}
	return -1;
}

int CGFunction::FindItemPosFromCode(int code)
{
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{ //??????????????????
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7 && iteminfo.itemid == code)
			{
				return iteminfo.pos;
			}
		}
	}
	return -1;
}

QSharedPointer<CGA::cga_item_info_t> CGFunction::FindBagItem(const QString &sItemName)
{
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{ //??????????????????
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7 && iteminfo.name == sItemName.toStdString())
			{
				QSharedPointer<CGA::cga_item_info_t> retItem(new CGA::cga_item_info_t);
				retItem->pos = iteminfo.pos;
				retItem->assessed = iteminfo.assessed;
				retItem->name = iteminfo.name;
				retItem->attr = iteminfo.attr;
				retItem->info = iteminfo.info;
				retItem->itemid = iteminfo.itemid;
				retItem->count = iteminfo.count;
				retItem->level = iteminfo.level;
				retItem->type = iteminfo.type;
				return retItem;
			}
		}
	}
	return nullptr;
}

QSharedPointer<CGA::cga_item_info_t> CGFunction::FindBagItemFromCode(int code)
{
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{ //??????????????????
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7 && iteminfo.itemid == code)
			{
				QSharedPointer<CGA::cga_item_info_t> retItem(new CGA::cga_item_info_t);
				retItem->pos = iteminfo.pos;
				retItem->assessed = iteminfo.assessed;
				retItem->name = iteminfo.name;
				retItem->attr = iteminfo.attr;
				retItem->info = iteminfo.info;
				retItem->itemid = iteminfo.itemid;
				retItem->count = iteminfo.count;
				retItem->level = iteminfo.level;
				retItem->type = iteminfo.type;
				return retItem;
			}
		}
	}
	return nullptr;
}

GameItemList CGFunction::GetGameItems()
{
	GameItemList newItemList;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			auto item = GameItemPtr(new GameItem);
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
	}
	return newItemList;
}

int CGFunction::FindEquipItemPos(const QString &sItemName)
{
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{ //??????????????????
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos < 8 && (iteminfo.name == sItemName.toStdString() || iteminfo.itemid == sItemName.toInt()))
			{
				return iteminfo.pos;
			}
		}
	}
	return -1;
}

GameItemPtr CGFunction::FindEquipItem(const QString &sItemName)
{
	GameItemList pItemList = GetGameItems();
	foreach (auto pItem, pItemList) //??????????????????
	{
		if (pItem->exist && pItem->pos >= 0 && pItem->pos < 8 && pItem->name == sItemName)
		{
			return pItem;
		}
	}
	return nullptr;
}

int CGFunction::FindEquipItemPosFromCode(int code)
{
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{ //??????????????????
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos < 8 && (iteminfo.itemid == code))
			{
				return iteminfo.pos;
			}
		}
	}
	return -1;
}

GameItemPtr CGFunction::FindEquipItemFromCode(int code)
{
	GameItemList pItemList = GetGameItems();
	foreach (auto pItem, pItemList) //??????????????????
	{
		if (pItem->exist && pItem->pos >= 0 && pItem->pos < 8 && pItem->id == code)
		{
			return pItem;
		}
	}
	return nullptr;
}

int CGFunction::FindBankEmptySlot(QString itemName, int maxCount)
{
	CGA::cga_items_info_t myinfos;
	if (!g_CGAInterface->GetBankItemsInfo(myinfos))
		return -1;
	//????????????pos???100??????
	QList<int> existUsePos;
	for (int i = 0; i < myinfos.size(); i++)
	{
		CGA::cga_item_info_t itemInfo = myinfos.at(i);
		if (maxCount > 0 && QString::fromStdString(itemInfo.name) == itemName && itemInfo.count < maxCount)
			return itemInfo.pos;
		existUsePos.append(itemInfo.pos - 100);
	}
	for (int i = 0; i < 80; ++i)
	{
		if (!existUsePos.contains(i))
		{
			return i + 100;
		}
	}
	return -1;
}

int CGFunction::FindInventoryEmptySlot(QString itemName /*= ""*/, int maxCount /*= 0*/)
{
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{

		if (itemName.isEmpty())
		{
			QVector<int> existPosList;
			for (size_t i = 0; i < itemsinfo.size(); ++i)
			{ //??????????????????
				const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
				if (iteminfo.pos >= 8)
				{
					existPosList.append(iteminfo.pos);
				}
			}
			for (int i = 8; i < 28; ++i) //??????????????????
			{
				if (!existPosList.contains(i))
				{
					return i;
				}
			}
		}
		else
		{
			for (size_t i = 0; i < itemsinfo.size(); ++i)
			{ //??????????????????
				const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
				if (iteminfo.pos >= 8 && iteminfo.name == itemName.toStdString() && iteminfo.count < maxCount)
				{
					return iteminfo.pos;
				}
			}
		}
	}
	return -1;
}

int CGFunction::GetInventoryEmptySlotCount()
{
	int nCount = 0;
	QList<int> posExist;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7)
			{
				posExist.push_back(iteminfo.pos);
			}
		}
	}
	for (size_t i = 8; i < 28; i++)
	{
		if (!posExist.contains(i))
		{
			nCount++;
		}
	}
	return nCount;
}

int CGFunction::GetBagUsedItemCount()
{
	int nCount = 0;
	QList<int> posExist;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7)
			{
				posExist.push_back(iteminfo.pos);
			}
		}
	}
	for (size_t i = 8; i < 28; i++)
	{
		if (posExist.contains(i))
		{
			nCount++;
		}
	}
	return nCount;
}

auto CGFunction::GetInventoryItems()
{
	CGA::cga_items_info_t bagItemInfos;

	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7)
			{
				bagItemInfos.push_back(iteminfo);
			}
		}
	}
	return bagItemInfos;
}

QVariant CGFunction::GetCharacterData(const QString &sType)
{
	CGA::cga_player_info_t playerinfo;
	g_CGAInterface->GetPlayerInfo(playerinfo);
	QString sLower = sType.toLower();
	int nType = m_returnGameDataHash.value(sLower);
	switch (nType)
	{
		case TRet_Game_Gid: return QString::fromStdString(playerinfo.gid);
		case TRet_Game_Hp: return playerinfo.hp;
		case TRet_Game_Mp: return playerinfo.mp;
		case TRet_Game_MaxHp: return playerinfo.maxhp;
		case TRet_Game_MaxMp: return playerinfo.maxmp;
		case TRet_Game_Level: return playerinfo.level;
		case TRet_Game_Exp: return playerinfo.xp;
		case TRet_Game_MaxExp: return playerinfo.maxxp;
		case TRet_Game_Health: return playerinfo.health;
		case TRet_Game_Soul: return playerinfo.souls;
		case TRet_Game_Name: return QString::fromStdString(playerinfo.name);
		case TRet_Game_Gold: return playerinfo.gold;
		case TRet_Game_BankGold:
		{
			int bankGold = 0;
			g_CGAInterface->GetBankGold(bankGold);
			return bankGold;
		}
		case TRet_Game_Punchclock: return playerinfo.punchclock;
		case TRet_Game_PunchclockState: return playerinfo.usingpunchclock;
		case TRet_Game_Job: return GetCharacterProfession();
		case TRet_Game_PlayerRank: return GetCharacterRank();
		case TRet_Game_Profession: return QString::fromStdString(playerinfo.job);
		case TRet_Game_OathGroup: return GetCharacterOathGroup();
		case TRet_Game_Sex: return GetCharacterSex(playerinfo.image_id);
		case TRet_Game_ImageID: return playerinfo.image_id;
		case TRet_Game_GamePID: return QVariant::fromValue(g_pGameCtrl->getGamePID());
		case TRet_Game_GamePort: return g_pGameCtrl->GetGamePort();
		case TRet_Game_PlayerRole: return playerinfo.player_index;
		case TRet_Game_Prestige:
		{
			auto playerTitles = playerinfo.titles;
			for (auto title : playerTitles)
			{
				if (m_sPrestigeMap.keys().contains(QString::fromStdString(title)))
				{
					return QString::fromStdString(title);
				}
			}
			return "";
		}
		case TRet_Game_PrestigeLv:
		{
			auto playerTitles = playerinfo.titles;
			for (auto title : playerTitles)
			{
				if (m_sPrestigeMap.keys().contains(QString::fromStdString(title)))
				{
					return m_sPrestigeMap.value(QString::fromStdString(title));
				}
			}
			return 0;
		}
		case TRet_Game_Loc:
		{
			QPoint curPos = GetMapCoordinate();
			return QString("%1,%2").arg(curPos.x()).arg(curPos.y());
		}
		case TRet_Game_PetCount:
		{
			CGA::cga_pets_info_t petsinfo;
			g_CGAInterface->GetPetsInfo(petsinfo);
			return petsinfo.size();
		}
		case TRet_Game_CurLine:
		{
			int index1, index2, index3;
			std::string filemap;
			g_CGAInterface->GetMapIndex(index1, index2, index3, filemap);
			return index2;
		}
		case TRet_Game_CurServelLine:
		{
			int index1, index2, index3;
			std::string filemap;
			g_CGAInterface->GetMapIndex(index1, index2, index3, filemap);
			return index1;
		}
		default:
			break;
	}

	return 0;
}

int CGFunction::GetBattlePetData(const QString &sType, QString val, QString val2)
{
	GamePetPtr pPet = nullptr;
	auto pPetList = GetGamePets();
	foreach (auto battlePet, pPetList)
	{
		if (battlePet && battlePet->exist && battlePet->battle_flags == TPET_STATE_BATTLE) //??????????????????
		{
			pPet = battlePet;
			break;
		}
	}
	//if (!pPet)
	//	return 0;

	QString sLower = sType.toLower();
	int nType = m_returnGameDataHash.value(sLower);
	switch (nType)
	{
		case TRet_Game_Hp: return pPet ? pPet->hp : 0;
		case TRet_Game_Mp: return pPet ? pPet->mp : 0;
		case TRet_Game_MaxHp: return pPet ? pPet->maxhp : 0;
		case TRet_Game_MaxMp: return pPet ? pPet->maxmp : 0;
		case TRet_Game_Level: return pPet ? pPet->level : 0;
		case TRet_Game_Exp: return pPet ? pPet->xp : 0;
		case TRet_Game_MaxExp: return pPet ? pPet->maxxp : 0;
		case TRet_Game_Health: return pPet ? pPet->health : 0;
		case TRet_Game_Loyality: return pPet ? pPet->loyality : 0;
		case TRet_Game_PetGrade: return pPet ? pPet->grade : 0;
		case TRet_Game_PetState: return pPet ? pPet->battle_flags : 0;
		case TRet_Game_PetChangeName:
		{
			int petIndex = -1;
			if (val2.isEmpty())
				petIndex = pPet ? pPet->index : -1;
			else
			{
				petIndex = val2.toInt();
			}
			bool bRes = false;
			g_CGAInterface->ChangePetName(petIndex, val.toStdString(), bRes);
			return bRes;
		}
		case TRet_Game_PetChangeState:
		{
			bool bRet = false;
			int nVal = 0;
			if (val == "??????")
				nVal = 1;
			else if (val == "??????")
				nVal = 2;
			else if (val == "??????")
				nVal = 3;
			else if (val == "??????")
				nVal = 16;
			else
				nVal = val.toInt();
			if (nVal != 1 && nVal != 2 && nVal != 3 && nVal != 16)
				return pPet ? pPet->battle_flags : 0;
			else
			{
				int petIndex = -1;
				if (val2.isEmpty())
					petIndex = pPet ? pPet->index : -1;
				else
				{
					petIndex = val2.toInt();
					if (petIndex == 5) //?????????
					{
						int nLv = 0;
						foreach (auto battlePet, pPetList)
						{
							if (battlePet && battlePet->exist && battlePet->level > nLv)
							{
								nLv = battlePet->level;
								pPet = battlePet;
							}
						}
						if (nLv != 0 && pPet)
						{
							petIndex = pPet->index;
						}
					}
					else if (petIndex == 6) //?????????
					{
						int nLv = 200;
						foreach (auto battlePet, pPetList)
						{
							if (battlePet && battlePet->exist && battlePet->level < nLv)
							{
								nLv = battlePet->level;
								pPet = battlePet;
							}
						}
						if (nLv != 200 && pPet)
						{
							petIndex = pPet->index;
						}
					}
				}
				if (petIndex < 0)
					return 0;
				if (nVal == TPET_STATE_BATTLE) //????????????????????????????????????????????????
				{
					foreach (auto battlePet, pPetList)
					{
						if (battlePet && battlePet->exist && battlePet->battle_flags == TPET_STATE_BATTLE) //??????????????????
						{
							g_CGAInterface->ChangePetState(battlePet->index, TPET_STATE_READY, bRet);
							Sleep(1000);
							break;
						}
					}
				}
				g_CGAInterface->ChangePetState(petIndex, nVal, bRet);
				return pPet ? pPet->battle_flags : 0;
			}
		}
	/*	case TRet_Game_PetBookInfo:		//??????????????????????????????????????????????????????????????????
		{
			std::string tgtPetName = val.toStdString();
			CGA::cga_picbooks_info_t books;
			g_CGAInterface->GetPicBooksInfo(books);
			for (auto &pet : books)
			{
				if (pet.name == tgtPetName)
				{
					return true;
				}
			}
			break;
		}*/
		case TRet_Game_CheckHavePetBook:
		{
			std::string tgtPetName = val.toStdString();
			CGA::cga_picbooks_info_t books;
			g_CGAInterface->GetPicBooksInfo(books);
			for (auto &pet : books)
			{
				if (pet.name == tgtPetName)
				{
					return true;
				}
			}
			break;
		}
		default:
			break;
	}
	return 0;
}

QVariantMap CGFunction::GetNextTitleData(const QString &sTitle, int grade)
{
	QMap<QString, QVariant> nextTitleData;

	if (!m_titles.contains(sTitle))
		return nextTitleData;
	auto titleObj = m_titles.value(sTitle).toObject();
	if (titleObj.isEmpty())
		return nextTitleData;
	if (!titleObj.contains(QString::number(grade)))
		return nextTitleData;
	auto nextTitleObj = titleObj.value(QString::number(grade)).toObject();
	return nextTitleObj.toVariantMap();
}

int CGFunction::BankOperation(const QString &sType)
{

	return 0;
}

QVariant CGFunction::GetTeamData(const QString &sType)
{
	auto teamPlayers = GetTeamPlayers();
	QString sLower = sType.toLower();
	if (sLower == "hp" || sLower == "???")
	{
		int hp = 0;
		for (auto teamPlayer : teamPlayers)
		{
			hp += teamPlayer->hp;
		}
		return hp;
	}
	else if (sLower == "avglv" || sLower == "????????????")
	{
		int val = 0;
		for (auto teamPlayer : teamPlayers)
		{
			val += teamPlayer->level;
		}
		if (teamPlayers.size() < 1) //??????????????????
		{
			return GetCharacterData("??????");
		}
		return val / teamPlayers.size();
	}
	else if (sLower == "count" || sLower == "??????")
	{
		if (teamPlayers.size() < 1) //??????????????????
			return 1;
		return teamPlayers.size();
	}

	return 0;
}

int CGFunction::SetPetData(const QString &sType, QVariant petData, int petIndex /*=-1*/)
{

	GamePetPtr pPet = nullptr;
	auto pPetList = GetGamePets();
	if (petIndex == -1)
	{
		pPet = GetBattlePet();
	}
	else
	{

		foreach (auto battlePet, pPetList)
		{
			if (battlePet && battlePet->exist && battlePet->index == petIndex) //????????????
			{
				pPet = battlePet;
				break;
			}
		}
	}

	//if (!pPet)
	//	return 0;
	QString sLower = sType.toLower();
	if (sLower == "changestate" || sLower == "?????????")
	{

		bool bRet = false;
		int nVal = 0;
		if (petData == "??????")
			nVal = TPET_STATE_READY;
		else if (petData == "??????")
			nVal = TPET_STATE_BATTLE;
		else if (petData == "??????")
			nVal = TPET_STATE_REST;
		else if (petData == "??????")
			nVal = TPET_STATE_WALK;
		else
			nVal = petData.toInt();
		if (nVal != TPET_STATE_READY && nVal != TPET_STATE_BATTLE && nVal != TPET_STATE_REST && nVal != TPET_STATE_WALK)
			return pPet ? pPet->battle_flags : 0;
		else
		{
			if (petIndex == -1)
			{
				petIndex = pPet ? pPet->index : 5;
			}
			if (petIndex == 5) //?????????
			{
				int nLv = 0;
				foreach (auto battlePet, pPetList)
				{
					if (battlePet && battlePet->exist && battlePet->level > nLv)
					{
						nLv = battlePet->level;
						pPet = battlePet;
					}
				}
				if (nLv != 0 && pPet)
				{
					petIndex = pPet->index;
				}
			}
			else if (petIndex == 6) //?????????
			{
				int nLv = 200;
				foreach (auto battlePet, pPetList)
				{
					if (battlePet && battlePet->exist && battlePet->level < nLv)
					{
						nLv = battlePet->level;
						pPet = battlePet;
					}
				}
				if (nLv != 200 && pPet)
				{
					petIndex = pPet->index;
				}
			}
			if (petIndex < 0)
				return 0;
			if (nVal == TPET_STATE_BATTLE) //????????????????????????????????????????????????
			{
				foreach (auto battlePet, pPetList)
				{
					if (battlePet && battlePet->exist && battlePet->battle_flags == TPET_STATE_BATTLE) //??????????????????
					{
						g_CGAInterface->ChangePetState(battlePet->index, TPET_STATE_READY, bRet);
						Sleep(1000);
						break;
					}
				}
			}
			g_CGAInterface->ChangePetState(petIndex, nVal, bRet);
			return pPet ? pPet->battle_flags : 0;
		}
	}
	return 0;
}

GamePetPtr CGFunction::GetBattlePet()
{
	GamePetPtr pPet = nullptr;
	auto pPetList = GetGamePets();
	foreach (auto battlePet, pPetList)
	{
		if (battlePet && battlePet->exist && battlePet->battle_flags == TPET_STATE_BATTLE) //??????????????????
		{
			pPet = battlePet;
			break;
		}
	}
	if (!pPet)
		return nullptr;
	return pPet;
}

GamePetList CGFunction::GetGamePets()
{
	GamePetList newPetList;
	CGA::cga_pets_info_t petsinfo;
	if (g_CGAInterface->GetPetsInfo(petsinfo))
	{
		for (size_t i = 0; i < petsinfo.size(); ++i)
		{
			const CGA::cga_pet_info_t &petinfo = petsinfo.at(i);
			auto pet = GamePetPtr(new GamePet);
			if (petinfo.level == 1)
			{
				QStringList inputData = g_pGameFun->GetPetCalcBpData(petinfo);
				auto pCalcData = g_pGamePetCalc->ParseLine(inputData);
				if (pCalcData)
				{

					pet->grade = pCalcData->lossMin;		//????????????
					pet->lossMinGrade = pCalcData->lossMin; //????????????
					pet->lossMaxGrade = pCalcData->lossMax; //????????????
				}
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
			pet->race = petinfo.race;
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
	}
	return newPetList;
}

QSharedPointer<CGA::cga_picbook_info_s> CGFunction::GetPetCard(QString name)
{
	CGA::cga_picbooks_info_t petBook;
	g_CGAInterface->GetPicBooksInfo(petBook);
	for (auto tmpPet : petBook)
	{
		if (QString::fromStdString(tmpPet.name) == name)
		{
			QSharedPointer<CGA::cga_picbook_info_s> newPet(new CGA::cga_picbook_info_s);
			newPet->can_catch = tmpPet.can_catch;
			newPet->card_type = tmpPet.card_type;			//0x0002 0=??????1=?????????2=??????
			newPet->race = tmpPet.race;						//0x0003 ??????
			newPet->index = tmpPet.index;					//0x0004
			newPet->image_id = tmpPet.image_id;				//0x0008
			newPet->rate_endurance = tmpPet.rate_endurance; //0x000C 0=????????????3=2?????????5=3?????????9=5??????
			newPet->rate_strength = tmpPet.rate_strength;	//0x0010
			newPet->rate_defense = tmpPet.rate_defense;		//0x0014
			newPet->rate_agility = tmpPet.rate_agility;		//0x0018
			newPet->rate_magical = tmpPet.rate_magical;		//0x001C
			newPet->element_earth = tmpPet.element_earth;	//0x0020
			newPet->element_water = tmpPet.element_water;	//0x0024
			newPet->element_fire = tmpPet.element_fire;		//0x0028
			newPet->element_wind = tmpPet.element_wind;		//0x002C
			newPet->skill_slots = tmpPet.skill_slots;		//0x0030 ?????????
			newPet->name = tmpPet.name;
			return newPet;
		}
	}
	return nullptr;
}

int CGFunction::GetGameServerLine()
{
	int index1, index2, index3;
	std::string filemap;
	g_CGAInterface->GetMapIndex(index1, index2, index3, filemap);
	return index2;
}

int CGFunction::GetFriendServerLine(QString name)
{
	CGA::cga_cards_info_t friendCards;
	g_CGAInterface->GetCardsInfo(friendCards);
	for (auto card : friendCards)
	{
		if (card.name == name.toStdString())
		{
			return card.server; //0-?????????1-10???????????????
		}
	}
	return -1; //-1-??????????????????
}

int CGFunction::GetItemCount(const QString &itemName)
{
	int nCount = 0;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7 && (iteminfo.name == itemName.toStdString() || iteminfo.itemid == itemName.toInt()))
			{
				nCount += 1;
			}
		}
	}
	return nCount;
}

int CGFunction::GetItemPileCount(const QString &itemName)
{
	int nCount = 0;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7 && (iteminfo.name == itemName.toStdString() || iteminfo.itemid == itemName.toInt()))
			{
				nCount += iteminfo.count;
			}
		}
	}
	return nCount;
}

int CGFunction::GetBankItemCount(const QString &itemName)
{
	int nCount = 0;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetBankItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.name == itemName.toStdString() /*|| iteminfo.itemid == itemName.toInt()*/)
			{
				nCount += 1;
			}
		}
	}
	return nCount;
}

int CGFunction::GetBankItemPileCount(const QString &itemName)
{
	int nCount = 0;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetBankItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.name == itemName.toStdString() /*|| iteminfo.itemid == itemName.toInt()*/) //????????????????????????id????????????????????????id??????
			{
				nCount += iteminfo.count;
			}
		}
	}
	return nCount;
}

int CGFunction::GetAllItemCount(const QString &itemName)
{
	int nCount = 0;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if ((iteminfo.name == itemName.toStdString() || iteminfo.itemid == itemName.toInt()))
			{
				nCount += 1;
			}
		}
	}
	return nCount;
}

int CGFunction::GetAllItemPileCount(const QString &itemName)
{
	int nCount = 0;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if ((iteminfo.name == itemName.toStdString() || iteminfo.itemid == itemName.toInt()))
			{
				nCount += iteminfo.count;
			}
		}
	}
	return nCount;
}

void CGFunction::Work(int index, int nSubIndex /*=0*/, int nDelayTime /*= 6000*/, int nImmediate)
{
	bool bResult = false;
	if (nImmediate >= 0)
		g_CGAInterface->SetImmediateDoneWork(nImmediate); //????????????????????????
	g_CGAInterface->SetWorkDelay(nDelayTime);			  //????????????
	qDebug() << "??????????????????:" << nDelayTime;
	//	g_CGAInterface->SetWorkAcceleration(100);	//????????????
	g_CGAInterface->StartWork(index, nSubIndex, bResult);
	//WaitRecvWorkResult(nDelayTime + 1000);
}

//??????????????? ????????????
void CGFunction::WorkEx(const QString &skillName, const QString &itemName, int nDelayTime /*= 6000*/, int nImmediate)
{
	auto pSkill = g_pGameFun->FindPlayerSkillEx(skillName);
	if (!pSkill)
	{
		qDebug() << QString("??????%1??????").arg(skillName);
		return;
	}
	int nSkillIndex = pSkill->index;
	int subIndex = pSkill->level;
	if (nSkillIndex < 0)
	{
		qDebug() << QString("??????%1??????").arg(skillName);
		return;
	}
	if (skillName == "??????")
	{
		if (nImmediate >= 0)
			g_CGAInterface->SetImmediateDoneWork(nImmediate);

		auto pSkill = FindPlayerSkillEx("??????");
		if (!pSkill)
		{
			qDebug() << "??????????????????";
			return;
		}
		bool bResult = false;
		bool assessedOnce = false;
		if (itemName.isEmpty())
		{
			auto pItemList = GetGameItems();
			for (auto pItem : pItemList)
			{
				//??????1??? ??????????????????1???1???  ??????????????????
				if (pItem->exist && !pItem->assessed && pItem->level <= pSkill->level)
				{
					g_CGAInterface->StartWork(pSkill->index, 0, bResult);
					qDebug() << "??????" << pItem->name << pItem->level << pItem->pos;
					if (GetCharacterData("mp") >= (pItem->level * 10) && g_CGAInterface->AssessItem(pSkill->index, pItem->pos, bResult))
					{
						int nTimeOut = assessedOnce ? 1000 : nDelayTime >= 0 ? nDelayTime :
																				 20000;
						WaitRecvWorkResult(nTimeOut);
						if (!assessedOnce)
						{
							assessedOnce = true;
							g_CGAInterface->SetImmediateDoneWork(true);
						}
					}
					Sleep(20);
				}
			}
		}
		else
		{
			auto pItemList = GetGameItems();
			for (auto pItem : pItemList)
			{
				//??????1??? ??????????????????1???1???  ??????????????????
				if (pItem->exist && pItem->name == itemName && !pItem->assessed && pItem->level <= pSkill->level)
				{
					g_CGAInterface->StartWork(pSkill->index, 0, bResult);
					qDebug() << "??????" << pItem->name << pSkill->id << pSkill->index << pItem->level << pItem->pos;
					if (GetCharacterData("mp") >= (pItem->level * 10) && g_CGAInterface->AssessItem(pSkill->index, pItem->pos, bResult))
					{
						int nTimeOut = assessedOnce ? 1000 : nDelayTime >= 0 ? nDelayTime :
																				 20000;
						WaitRecvWorkResult(nTimeOut);
						if (!assessedOnce)
						{
							assessedOnce = true;
							g_CGAInterface->SetImmediateDoneWork(true);
						}
					}
					Sleep(20);
					break;
				}
			}
		}
	}
	else
	{
		Work(nSkillIndex, subIndex - 1, nDelayTime);
	}
}

void CGFunction::AllCompound(QString name)
{
	g_CGAInterface->SetImmediateDoneWork(true);
	auto pRetSkill = GetItemCraftInfo(name);
	if (pRetSkill->index < 0)
	{
		qDebug() << "??????????????????";
		emit signal_workThreadEnd("??????????????????");
		return;
	}
	bool bResult = false;
	bool assessedOnce = false;
	while (!g_pGameFun->m_bStop)
	{
		if (GetCharacterData("mp") < pRetSkill->compound.cost)
		{
			qDebug() << "?????????????????????";
			emit signal_workThreadEnd("?????????????????????");
			return;
		}
		//8-27??????
		if (GetInventoryEmptySlotCount() < 1)
		{
			qDebug() << "??????????????????????????????";
			emit signal_workThreadEnd("???????????????????????????");
			return;
		}
		for (int nn = 0; nn < 5; ++nn)
		{
			pRetSkill->compound.materials[nn].pos = 0; //???????????????
			auto pItemList = GetInventoryItems();
			for (auto pItem : pItemList)
			{
				if (pRetSkill->compound.materials[nn].name.toStdString() == pItem.name && pItem.count >= pRetSkill->compound.materials[nn].count)
				{
					pRetSkill->compound.materials[nn].pos = pItem.pos;
					break;
				}
			}
		}
		//??????1??? ??????????????????1???1???  ??????????????????
		{
			CGA::cga_craft_item_t craftItem;
			craftItem.skill_index = pRetSkill->index;
			craftItem.subskill_index = pRetSkill->compound.index;
			craftItem.sub_type = 0;
			for (int nn = 0; nn < 5; ++nn) //5????????? ??????????????? ???????????????
			{
				if (!pRetSkill->compound.materials[nn].name.isEmpty() && pRetSkill->compound.materials[nn].pos == 0)
				{
					qDebug() << "??????????????????";
					emit signal_workThreadEnd("??????????????????");
					return;
				}
				qDebug() << "????????????" << pRetSkill->compound.materials[nn].name << " pos " << pRetSkill->compound.materials[nn].pos;

				craftItem.itempos[nn] = pRetSkill->compound.materials[nn].pos;
			}
			int doneWorkState = 0;
			//g_CGAInterface->GetImmediateDoneWorkState(doneWorkState);
			//qDebug() << "SetImmediateDoneWork State " << doneWorkState;
			g_CGAInterface->StartWork(pRetSkill->index, pRetSkill->compound.index, bResult);
			g_CGAInterface->CraftItem(craftItem, bResult);
			qDebug() << "?????????";
			//pThis->Work(pSkill->id, 0);
			emit signal_workStart();
			int nTimeOut = assessedOnce ? 2000 : 90000;
			WaitRecvWorkResult(nTimeOut);
			assessedOnce = true;
			emit signal_workEnd();
			//???????????? ?????????
			Sleep(500);
		}
	}
}

void CGFunction::StartWork(int workType, int nDelayTime /*=6000*/, QString compoundName)
{
	if (m_workFuture.isRunning())
	{
		qDebug() << "???????????????????????????????????????";
		return;
	}
	if (workType == TWork_Compound && compoundName.isEmpty())
	{
		qDebug() << "???????????????????????????" << compoundName;
		return;
	}
	m_nCurrentWorkType = workType;
	m_sCurrentCompoundName = compoundName;
	m_bWorking = true;
	if (workType != TWork_Compound)
	{
		QString skillName = m_workTypeForText.value(m_nCurrentWorkType);
		m_nCurrentWorkSkillIndex = g_pGameFun->FindPlayerSkill(skillName);
		if (m_nCurrentWorkSkillIndex < 0)
		{
			qDebug() << QString("??????%1??????").arg(skillName);
			return;
		}
	}

	m_nCurrentWorkDelayTime = nDelayTime;
	RestFun();
	//	g_pGameCtrl->OnSetWorkDelayTime(ntime);
	m_workFuture = QtConcurrent::run(WorkThread, g_pGameFun);
}

void CGFunction::StopWork()
{
	//g_CGAInterface->SetImmediateDoneWork(true);	//??????????????????????????? ????????????????????????
	m_bWorking = false;
	g_pGameFun->StopFun();
	//g_CGAInterface->SetImmediateDoneWork(false); //????????????????????????
}

void CGFunction::Compound(const QString &itemName, int nVal /*=0*/)
{
	bool result = false;
	CGA::cga_craft_item_t craft;
	//??????????????? ??????????????????????????????
	g_CGAInterface->CraftItem(craft, result);
}

void CGFunction::AssessAllItem()
{
	auto pSkill = FindPlayerSkillEx("??????");
	if (!pSkill)
	{
		qDebug() << "??????????????????";
		return;
	}
	bool bResult = false;
	auto pItemList = GetGameItems();
	for (auto pItem : pItemList)
	{
		if (pItem->exist && !pItem->assessed && pItem->level <= pSkill->level)
		{
			Work(pSkill->index, 0);
			if (GetCharacterData("mp") >= (pItem->level * 10) && g_CGAInterface->AssessItem(pSkill->index, pItem->pos, bResult))
			{
				WaitRecvWorkResult(9000);
			}
			Sleep(20);
		}
	}
}

int CGFunction::GetWorkState()
{
	int state = 0;
	if (!g_CGAInterface->GetImmediateDoneWorkState(state))
	{
		return 0;
	}
	return state;
}

int CGFunction::GetCompoundState()
{
	int status = 0;
	if (!g_CGAInterface->GetCraftStatus(status))
	{
		return 0;
	}
	return status;
}

QSharedPointer<GameCompoundSkill> CGFunction::GetItemCraftInfo(const QString &compoundName)
{
	qDebug() << "GetItemCraftInfo" << compoundName;
	QSharedPointer<GameCompoundSkill> pRetSkill(new GameCompoundSkill);
	pRetSkill->index = -1;
	GameSkillList pSkillList = g_pGameFun->GetGameSkills();
	for (auto pSkill : pSkillList)
	{
		/*if (!pSkill->available)
			continue;*/

		//????????????????????????
		CGA::cga_crafts_info_t myinfos;
		g_CGAInterface->GetCraftsInfo(pSkill->index, myinfos);
		for (size_t j = 0; j < myinfos.size(); ++j)
		{
			qDebug() << "GetItemCraftInfo" << QString::fromStdString(myinfos.at(j).name);

			if (myinfos.at(j).name != compoundName.toStdString())
			{
				continue;
			}
			pRetSkill->available = pSkill->available;
			pRetSkill->id = pSkill->id;
			pRetSkill->name = pSkill->name;
			pRetSkill->level = pSkill->level;
			pRetSkill->maxLevel = pSkill->maxLevel;
			pRetSkill->xp = pSkill->xp;
			pRetSkill->maxxp = pSkill->maxxp;
			pRetSkill->index = pSkill->index;
			pRetSkill->pos = pSkill->pos;

			GameCompound *pCompound = &(pRetSkill->compound);
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
		}
	}
	return pRetSkill;
}

GameSkillList CGFunction::GetGameSkills()
{
	GameSkillList newSkillList;
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
	}
	return newSkillList;
}

void CGFunction::ParseItemDurability(GameItemPtr pItem)
{
	if (!pItem || !pItem->exist || pItem->attr.isEmpty())
		return;
	QString sAttr = pItem->attr;
	if (!sAttr.contains("??????"))
		return;
	sAttr = sAttr.mid(sAttr.indexOf("??????"));
	//qDebug() << sAttr;

	sAttr = sAttr.mid(0, sAttr.indexOf("$"));
	//qDebug() << sAttr;

	sAttr.remove("??????");
	auto matchResult = sAttr.split("/");
	//qDebug() << matchResult;
	//qDebug() << pItem->attr << matchResult;
	if (matchResult.size() == 2)
	{
		pItem->nCurDurability = matchResult[0].toInt();
		pItem->nMaxDurability = matchResult[1].toInt();
		if (pItem->nMaxDurability > 0)
			pItem->nDurabilityRate = pItem->nCurDurability / pItem->nMaxDurability;
	}
}

void CGFunction::ParseItemDurabilityEx(QString sItemAttr, int &nCur, int &nMax)
{
	QString sAttr = sItemAttr;
	if (!sAttr.contains("??????"))
		return;
	sAttr = sAttr.mid(sAttr.indexOf("??????"));
	//qDebug() << sAttr;

	sAttr = sAttr.mid(0, sAttr.indexOf("$"));
	//qDebug() << sAttr;

	sAttr.remove("??????");
	auto matchResult = sAttr.split("/");
	//qDebug() << matchResult;
	//qDebug() << pItem->attr << matchResult;
	if (matchResult.size() == 2)
	{
		nCur = matchResult[0].toInt();
		nMax = matchResult[1].toInt();
	}
	return;
}

int CGFunction::GetItemDurability(const QString &name)
{
	auto pGameItems = GetGameItems();
	for (auto pItem : pGameItems) //??????????????????
	{
		//?????????????????????  ????????????????????????
		if (pItem->exist && pItem->pos >= 0 && pItem->pos < 8 && pItem->name == name)
		{
			return pItem->nCurDurability;
		}
	}
	return 0;
}

int CGFunction::GetPosItemDurability(int pos)
{
	auto pGameItems = GetGameItems();
	for (auto pItem : pGameItems) //??????????????????
	{
		//?????????????????????  ????????????????????????
		if (pItem->exist && pItem->pos == pos)
		{
			return pItem->nCurDurability;
		}
	}
	return 0;
}

QMap<QString, QVariant> CGFunction::ParseBuyStoreMsg(QString msg)
{
	QMap<QString, QVariant> obj;

	if (msg.isEmpty())
		return obj;

	//QRegExp reg("/([^|\n]+)/g");
	//auto match = msg.split(reg);
	auto match = msg.split("|");
	if (match.length() < 5)
		return obj;

	auto storeItemCount = (match.length() - 5) / 6;
	obj.insert("storeid", match[0]);
	obj.insert("name", match[1]);
	obj.insert("welcome", match[2]);
	obj.insert("insuff_funds", match[3]);
	obj.insert("insuff_inventory", match[4]);
	//obj.insert("items",match[4]);
	QList<QVariant> itemList;
	for (int i = 0; i < storeItemCount; ++i)
	{
		QMap<QString, QVariant> itemMap;
		itemMap.insert("name", match[5 + 6 * i + 0]);
		itemMap.insert("image_id", match[5 + 6 * i + 1]);
		itemMap.insert("cost", match[5 + 6 * i + 2]);
		itemMap.insert("attr", match[5 + 6 * i + 3]);
		itemMap.insert("unk1", match[5 + 6 * i + 4]);
		itemMap.insert("max_buy", match[5 + 6 * i + 5]);
		itemList.push_back(itemMap);
	}
	obj.insert("items", itemList);
	return obj;
}

void CGFunction::WorkThread(CGFunction *pThis)
{
	//g_CGAInterface->SetImmediateDoneWork(false);
	if (pThis->m_nCurrentWorkType == TWork_Identify) //??????all
	{
		auto pSkill = pThis->FindPlayerSkillEx("??????");
		if (!pSkill)
		{
			qDebug() << "??????????????????";
			emit pThis->signal_workThreadEnd("??????????????????");
			return;
		}
		bool bResult = false;
		bool assessedOnce = false;
		auto pItemList = pThis->GetGameItems();
		//g_CGAInterface->SetWorkDelay(8000);
		g_CGAInterface->SetWorkAcceleration(100);
		for (auto pItem : pItemList)
		{
			//??????1??? ??????????????????1???1???  ??????????????????
			if (pThis->m_bStop)
				break;
			int tryCount = 0;
			if (!pThis->m_bStop && pItem->exist && !pItem->assessed && pItem->level <= pSkill->level && tryCount < 10)
			{
				emit pThis->signal_workStart();
				g_CGAInterface->StartWork(pSkill->index, 0, bResult);
				qDebug() << "??????" << pSkill->index << pItem->name << pItem->level << pItem->pos;
				//pThis->Work(pSkill->id, 0);
				if (pThis->GetCharacterData("mp") >= (pItem->level * 10) && g_CGAInterface->AssessItem(pSkill->index, pItem->pos, bResult))
				{
					int nTimeOut = assessedOnce ? 1000 : 30000;
					auto workState = pThis->WaitRecvWorkResult(nTimeOut);
					if (!assessedOnce && workState && workState->success)
					{
						assessedOnce = true;
						g_CGAInterface->SetImmediateDoneWork(true);
					}
				}
				else
				{
					emit pThis->signal_workThreadEnd("?????????/??????????????????");
					return;
				}
				emit pThis->signal_workEnd();
				Sleep(20);
				tryCount += 1;
			}
		}
	}
	else if (pThis->m_nCurrentWorkType == TWork_Compound) //??????all
	{
		qDebug() << "WorkThread" << pThis->m_sCurrentCompoundName;
		pThis->AllCompound(pThis->m_sCurrentCompoundName);
	}
	else if (pThis->m_nCurrentWorkType == TWork_RepairWeapon || pThis->m_nCurrentWorkType == TWork_RepairArmor)
	{
		QString skillName;
		if (pThis->m_nCurrentWorkType == TWork_RepairWeapon)
			skillName = "????????????";
		else
			skillName = "????????????";

		auto pSkill = pThis->FindPlayerSkillEx(skillName);
		if (!pSkill)
		{
			qDebug() << "??????????????????";
			emit pThis->signal_workThreadEnd("??????????????????");
			return;
		}
		bool bResult = false;
		bool assessedOnce = false;
		auto pItemList = pThis->GetGameItems();
		for (auto pItem : pItemList)
		{
			if (!pItem->exist || pItem->pos < 8) //????????? ???????????????
				continue;
			if (pThis->m_bStop)
			{
				emit pThis->signal_workThreadEnd("");
				return;
			}
			//??????1??? ??????????????????1???1???  ??????????????????
			if (((pThis->m_nCurrentWorkType == TWork_RepairWeapon && pItem->type >= 0 && pItem->type <= 6) || (pThis->m_nCurrentWorkType == TWork_RepairArmor && pItem->type >= 7 && pItem->type <= 14)) && pItem->level <= pSkill->level && pItem->nCurDurability < pItem->nMaxDurability)
			{
				g_CGAInterface->StartWork(pSkill->index, 0, bResult);
				qDebug() << "??????" << pItem->name << pItem->level << pItem->pos << pItem->nCurDurability << pItem->nMaxDurability;
				//pThis->Work(pSkill->id, 0);
				emit pThis->signal_workStart();
				if (pThis->GetCharacterData("mp") >= (pItem->level * 10) && g_CGAInterface->AssessItem(pSkill->index, pItem->pos, bResult))
				{
					int nTimeOut = assessedOnce ? 2000 : 9000;
					auto workState = pThis->WaitRecvWorkResult(nTimeOut); //?????????????????????
					if (!assessedOnce && workState && workState->success)
					{
						assessedOnce = true;
						g_CGAInterface->SetImmediateDoneWork(true);
					}
				}
				emit pThis->signal_workEnd();
				Sleep(200);
			}
		}
	}
	else if (pThis->m_nCurrentWorkType == TWork_Heal || pThis->m_nCurrentWorkType == TWork_FirstAid)
	{
	}
	else //???????????????
	{
		while (!pThis->m_bStop)
		{
			pThis->Work(pThis->m_nCurrentWorkSkillIndex, 0, pThis->m_nCurrentWorkDelayTime);
			emit pThis->signal_workStart();
			pThis->WaitRecvWorkResult(pThis->m_nCurrentWorkDelayTime + 1000);
			emit pThis->signal_workEnd();
		}
		emit pThis->signal_workThreadEnd("");
		return;
	}
	emit pThis->signal_workThreadEnd("");
}

int CGFunction::FindPlayerSkill(const QString &skillName)
{
	auto pSkillList = g_pGameFun->GetGameSkills();
	for (auto pSkill : pSkillList)
	{
		if (pSkill->name == skillName)
		{
			qDebug() << "FindSkill" << skillName << pSkill->id << pSkill->index;
			return pSkill->index;
		}
	}
	return -1;
}
GameSkillPtr CGFunction::FindPlayerSkillEx(const QString &skillName)
{
	auto pSkillList = g_pGameFun->GetGameSkills();
	for (auto pSkill : pSkillList)
	{
		if (pSkill->name == skillName)
		{
			qDebug() << "FindSkill" << skillName << pSkill->id << pSkill->index;
			return pSkill;
		}
	}
	return nullptr;
}

bool CGFunction::LearnPlayerSkill(int x, int y)
{
	bool bResult = false;
	TurnAboutEx(x, y);
	WaitRecvNpcDialog();
	g_CGAInterface->ClickNPCDialog(-1, 0, bResult);
	WaitRecvNpcDialog();
	g_CGAInterface->ClickNPCDialog(0, 0, bResult);
	WaitRecvNpcDialog();
	g_CGAInterface->ClickNPCDialog(1, 0, bResult);
	return true;
}

bool CGFunction::ForgetPlayerSkill(int x, int y, QString skillName)
{
	int existSkill = FindPlayerSkill(skillName);
	if (existSkill < 0)
		return false;
	bool bResult = false;
	TurnAboutEx(x, y);
	auto dlg = WaitRecvNpcDialog();
	int count = 0; //10???
	while (dlg && count < 10)
	{
		if (dlg && dlg->type == 16)
		{
			g_CGAInterface->ClickNPCDialog(-1, 1, bResult);
			dlg = WaitRecvNpcDialog();
		}
		if (dlg && dlg->type == 18)
		{
			CGA::cga_skills_info_t skillsinfo;
			g_CGAInterface->GetSkillsInfo(skillsinfo);

			qSort(skillsinfo.begin(), skillsinfo.end(), [&](const CGA::cga_skill_info_t &a, const CGA::cga_skill_info_t &b)
					{ return a.pos < b.pos; });
			int index = -1;
			for (size_t i = 0; i < skillsinfo.size(); ++i)
			{
				const CGA::cga_skill_info_t &skinfo = skillsinfo.at(i);
				qDebug() << QString::fromStdString(skinfo.name) << skinfo.pos;
				if (skinfo.name == skillName.toStdString())
				{
					index = i;
					break;
				}
			}
			if (index < 0)
				return false;

			g_CGAInterface->ClickNPCDialog(0, index, bResult);
			dlg = WaitRecvNpcDialog();
		}
		if (dlg && dlg->options == 12)
		{
			g_CGAInterface->ClickNPCDialog(4, -1, bResult);
			dlg = WaitRecvNpcDialog();
		}
		if (dlg && dlg->message.contains("????????????"))
		{
			TurnAboutEx(x, y);
			WaitRecvNpcDialog();
			g_CGAInterface->ClickNPCDialog(0, 0, bResult);
			WaitRecvNpcDialog();
			g_CGAInterface->ClickNPCDialog(0, 0, bResult);
			return true;
		}
		count++;
	}
	return false;
}

bool CGFunction::ForgetPlayerSkillEx(int d, QString skillName)
{
	auto pos = GetDirectionPos(d, 1);
	return ForgetPlayerSkill(pos.x(), pos.y(), skillName);
}

void CGFunction::Npc(int option, int index)
{
	bool bResult = false;
	qDebug() << "Npc" << option << index;
	g_CGAInterface->ClickNPCDialog(option, index, bResult);
}

bool CGFunction::IsInBattle()
{
	int nWordStatus;
	g_CGAInterface->GetWorldStatus(nWordStatus);
	return (nWordStatus == 10) ? true : false;
}

QPoint CGFunction::GetMapCoordinate()
{
	QPoint pos;
	int x, y;
	g_CGAInterface->GetMapXY(x, y);
	pos.setX(x);
	pos.setY(y);
	return pos;
}

QString CGFunction::GetMapName()
{
	string name;
	g_CGAInterface->GetMapName(name);
	return QString::fromStdString(name);
}

QString CGFunction::GetMapFilePath()
{
	int index1, index2, index3;
	std::string filemap;
	g_CGAInterface->GetMapIndex(index1, index2, index3, filemap);
	return QString::fromStdString(filemap);
}

bool CGFunction::IsInRandomMap()
{
	//if (GetMapFilePath().contains("map\\0"))
	//	return false;
	//index1  0???????????? ??????????????????  index2 ???????????? index3???????????? filemap???????????????
	int index1, index2, index3;
	std::string filemap;
	g_CGAInterface->GetMapIndex(index1, index2, index3, filemap);
	if (index1 == 0)
		return false;
	return true;
}

int CGFunction::GetMapIndex()
{
	int index1, index2, index3;
	std::string filemap;
	g_CGAInterface->GetMapIndex(index1, index2, index3, filemap);
	return index3;
}

int CGFunction::GetTeammatesCount()
{
	CGA::cga_team_players_t plinfo;
	if (!g_CGAInterface->GetTeamPlayerInfo(plinfo))
	{
		return 0;
	}
	return plinfo.size();
}
QList<QPoint> CGFunction::GetUnmannedMapUnitPosList(QPoint tgtpos)
{
	//????????????????????????????????????
	auto character = g_pGameFun->GetGameCharacter();
	QString sName = character->name;

	int nFindRange = 5;
	auto fondPosList = GetMovablePointsEx(tgtpos, nFindRange);
	CGA::cga_map_units_t units;
	g_CGAInterface->GetMapUnits(units);

	QList<QPoint> posList;
	QMap<QString, CGA::cga_map_unit_t> playerList;
	for (size_t i = 0; i < units.size(); i++)
	{
		auto unit = units.at(i);
		if (unit.valid && unit.type == 8 && unit.model_id != 0 && (unit.flags & 256) != 0) //??????
		{
			if (unit.unit_name == sName.toStdString()) //???????????????
				continue;

			playerList.insert(QString("%1,%2").arg(unit.xpos).arg(unit.ypos), unit);
		}
	}
	QList<QPoint> lastUnmannedPos;
	auto findUnitList = playerList.values(QString("%1,%2").arg(tgtpos.x()).arg(tgtpos.y()));
	for (auto tmpPos : fondPosList)
	{
		if (!playerList.contains(QString("%1,%2").arg(tmpPos.x()).arg(tmpPos.y())))
		{
			lastUnmannedPos.append(tmpPos);
		}
	}
	QList<QPoint> sortUnmannedPos;
	if (lastUnmannedPos.contains(tgtpos))
	{
		sortUnmannedPos.append(tgtpos);
	}
	auto isFind = [&](QPoint tmPos)
	{
		if (lastUnmannedPos.contains(tmPos))
			sortUnmannedPos.append(tmPos);
	};
	//??????????????????  ?????????????????????????????????
	for (int i = 0; i < nFindRange; ++i)
	{
		//8??????
		QPoint tmPos(tgtpos.x() - i, tgtpos.y());
		isFind(tmPos);

		tmPos = QPoint(tgtpos.x() + i, tgtpos.y());
		isFind(tmPos);

		tmPos = QPoint(tgtpos.x(), tgtpos.y() - i);
		isFind(tmPos);

		tmPos = QPoint(tgtpos.x(), tgtpos.y() + i);
		isFind(tmPos);

		tmPos = QPoint(tgtpos.x() - i, tgtpos.y() - i);
		isFind(tmPos);

		tmPos = QPoint(tgtpos.x() + i, tgtpos.y() + i);
		isFind(tmPos);

		tmPos = QPoint(tgtpos.x() - i, tgtpos.y() + i);
		isFind(tmPos);

		tmPos = QPoint(tgtpos.x() + i, tgtpos.y() + i);
		isFind(tmPos);
	}
	return sortUnmannedPos;
}

bool CGFunction::AddTeammate(const QString &sName, int timeout)
{
	if (sName.isEmpty()) //?????????????????? ??????????????????
	{
		DoCharacterAction(TCharacter_Action_JOINTEAM);
		return true;
	}
	QPoint orgPos = GetMapCoordinate();
	DWORD dwStartTime = GetTickCount();
	while (!g_pGameFun->IsStop())
	{
		//3?????? ?????????????????? ??????
		if ((GetTickCount() - dwStartTime) > timeout)
		{
			return false;
		}
		auto teamLeaderUnit = FindPlayerUnit(sName);
		if (teamLeaderUnit == nullptr)
		{
			//?????????????????? ??????
			Sleep(2000);
			continue;
		}

		QPoint curPos = GetMapCoordinate();
		if (curPos.x() == teamLeaderUnit->xpos && curPos.y() == teamLeaderUnit->ypos)
		{
			//Sleep(2000); //????????????????????? ??????
			if (GetTeammatesCount() > 0 && IsTeamLeader(sName))
				return true;
			else
				DoCharacterAction(TCharacter_Action_LEAVETEAM); //??????
			MoveToNpcNear(teamLeaderUnit->xpos, teamLeaderUnit->ypos);
		}
		curPos = GetMapCoordinate();
		if (!IsNearTarget(curPos.x(), curPos.y(), teamLeaderUnit->xpos, teamLeaderUnit->ypos))
		{
			//Sleep(2000); //?????????????????? ??????
			if (GetTeammatesCount() > 0 && IsTeamLeader(sName))
				return true;
			else
				DoCharacterAction(TCharacter_Action_LEAVETEAM); //??????
			MoveToNpcNear(teamLeaderUnit->xpos, teamLeaderUnit->ypos);
		}
		//	g_CGAInterface->TurnTo(teamLeaderUnit->xpos, teamLeaderUnit->ypos);
		TurnAboutPointDir(teamLeaderUnit->xpos, teamLeaderUnit->ypos);
		//?????????????????? ????????????????????????????????????
		DoCharacterAction(TCharacter_Action_JOINTEAM);
		DoCharacterAction(TCharacter_Action_JOINTEAM);
		QMutex mutex;
		QEventLoop loop;
		QTimer::singleShot(5000, &loop, &QEventLoop::quit); //5???
		connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
		auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyNPCDialog, [&](const QSharedPointer<CGA_NPCDialog_t> &dlg)
				{
					if (mutex.tryLock())
					{
						QString stripper = "???????????????????????????";
						bool bResult = false;

						if (!dlg->message.isEmpty() && dlg->message.contains(stripper))
						{
							QString strip = dlg->message.mid(dlg->message.indexOf(stripper) + stripper.length());
							//	qDebug() << strip;
							QStringList match = strip.split("\n");
							//	qDebug() << match;
							match.removeAll("");
							//	qDebug() << match;

							for (int j = 0; j < match.size(); ++j)
							{
								if (match[j] == sName)
								{
									g_CGAInterface->ClickNPCDialog(0, j, bResult);
									break;
								}
							}
						}
						if (loop.isRunning())
							loop.quit();
						mutex.unlock();
					}
				});
		loop.exec();
		{
			QMutexLocker locker(&mutex);
			QObject::disconnect(connection); //??????Connection ??????lambda?????????
		}

		auto teamPlayers = GetTeamPlayers();
		if (!teamPlayers.isEmpty() && teamPlayers[0]->name == sName)
			return true;
		else
			DoCharacterAction(TCharacter_Action_LEAVETEAM); //??????
		Sleep(1000);
		//????????????
		AutoMoveTo(orgPos.x(), orgPos.y());
	}
	return false;
}

bool CGFunction::LeaveTeammate()
{
	auto teamPlayers = GetTeamPlayers();
	if (teamPlayers.size() > 0)
		return DoCharacterAction(TCharacter_Action_LEAVETEAM); //??????
	return false;
}

bool CGFunction::WaitTeammates(QStringList teammateNames)
{
	auto teamplayers = GetTeamPlayers();

	if (teammateNames.size() == 0 && teamplayers.size() == 0)
	{
		return false;
	}
	//????????????
	SetCharacterSwitch(CHARACTER_Troop, true);
	DWORD dwStartTime = GetTickCount();
	while (!g_pGameFun->IsStop())
	{
		//3?????? ?????????????????? ??????
		if ((GetTickCount() - dwStartTime) > 1000 * 60 * 3)
		{
			return false;
		}
		teamplayers = GetTeamPlayers();
		if (teamplayers.size() >= teammateNames.size())
		{
			for (int i = 0; i < teamplayers.size(); ++i)
			{
				if (!teammateNames.contains(teamplayers[i]->name))
				{
					//Unknown teammates, kick
					DoCharacterAction(TCharacter_Action_KICKTEAM);
					QMutex mutex;
					QEventLoop loop;
					QTimer::singleShot(5000, &loop, &QEventLoop::quit); //5???
					connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
					auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyNPCDialog, [&](const QSharedPointer<CGA_NPCDialog_t> &dlg)
							{
								if (mutex.tryLock())
								{
									QString stripper = "???????????????????????????";
									bool bResult = false;

									if (!dlg->message.isEmpty() && dlg->message.contains(stripper))
									{
										QString strip = dlg->message.mid(dlg->message.indexOf(stripper) + stripper.length());
										QStringList match = strip.split("\n");
										match.removeAll("");
										for (int j = 0; j < match.size(); ++j)
										{
											if (match[j] == teamplayers[i]->name)
											{
												g_CGAInterface->ClickNPCDialog(0, j / 2, bResult);
												break;
											}
										}
									}
									if (loop.isRunning())
										loop.quit();
									mutex.unlock();
								}
							});
					loop.exec();
					{
						QMutexLocker locker(&mutex);
						QObject::disconnect(connection); //??????Connection ??????lambda?????????
					}
				}
			}
			if (teamplayers.size() == teammateNames.size())
			{ //???????????? ???????????? ???????????? ??????
				bool bSuccess = true;
				for (int i = 0; i < teamplayers.size(); ++i)
				{
					if (!teammateNames.contains(teamplayers[i]->name))
					{
						bSuccess = false;
						break;
					}
				}
				if (bSuccess)
					return true;
			}
		}
		Sleep(2000);
	}
	return false;
}

bool CGFunction::WaitTeammatesEx(int nCount)
{
	//????????????
	SetCharacterSwitch(CHARACTER_Troop, true);
	auto teamplayers = GetTeamPlayers();
	DWORD dwStartTime = GetTickCount();
	while (!g_pGameFun->IsStop())
	{
		//3?????? ?????????????????? ??????
		if ((GetTickCount() - dwStartTime) > 1000 * 60 * 3)
		{
			return false;
		}
		teamplayers = GetTeamPlayers();
		if (teamplayers.size() >= nCount)
		{
			return true;
		}
		Sleep(2000);
	}
	return false;
}
//???????????? sName??????????????????
bool CGFunction::LaunchTrade(const QString &sName, const QString &myTradeData, const QString &sTradeData, int timeout)
{
	//????????????
	DoCharacterAction(TCharacter_Action_TRADE);
	bool bResult = false;
	bool bFind = false;
	//?????????????????????
	auto rcvMenu = WaitRecvPlayerMenu(timeout);
	if (!rcvMenu)
	{
		qDebug() << "??????????????????????????????";
		return false;
	}
	if (rcvMenu && rcvMenu->size() > 0)
	{
		if (sName.isEmpty())
		{
			bFind = true;

			/*QByteArray arr = g_codecGBK->fromUnicode(rcvMenu->at(1).name.c_str());
			g_CGAInterface->PlayerMenuSelect(rcvMenu->at(0).index, arr.toStdString(), bResult);*/
			g_CGAInterface->PlayerMenuSelect(rcvMenu->at(0).index, rcvMenu->at(0).name, bResult);
			qDebug() << "???????????????????????????????????????????????????" << sName << QString::fromStdString(rcvMenu->at(0).name);
		}
		else
		{
			for (int i = 0; i < rcvMenu->size(); ++i)
			{
				if (QString::fromStdString(rcvMenu->at(i).name) == sName /*|| rcvMenu->at(i).index == sName.toInt()*/)
				{
					bFind = true;
					//???????????????????????????dll?????????codec?????????????????????gbk????????????utf8?????????????????????????????????????????????gbk
					//??????PlayerMenuSelect???dll?????????????????????????????????????????????Dll??????????????????????????????
					/*	QByteArray arr = g_codecGBK->fromUnicode(sName);
					g_CGAInterface->PlayerMenuSelect(rcvMenu->at(i).index, arr.toStdString(), bResult);*/
					g_CGAInterface->PlayerMenuSelect(rcvMenu->at(i).index, rcvMenu->at(i).name, bResult);
					qDebug() << "???????????????" << sName << QString::fromStdString(rcvMenu->at(i).name);
					break;
				}
			}
		}
	}
	if (!bFind)
	{
		qDebug() << "???????????????????????????,????????????";
		DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
		return false;
	}

	auto tradeDlg = WaitTradeDialog(timeout);
	if (tradeDlg == nullptr)
	{
		qDebug() << "???????????????????????????,????????????";
		DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
		return false;
	}

	QString sTextName = QString::fromUtf8(tradeDlg->name.c_str());
	qDebug() << sTextName;
	if (tradeDlg->name != sName.toStdString() || tradeDlg->level <= 0)
	{
		qDebug() << "??????????????????,????????????,??????????????????:" << QString::fromStdString(tradeDlg->name) << " ??????????????????:" << sName;
		DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
		return false;
	}
	return TradeInternal(sName, myTradeData, sTradeData, timeout);
}

bool CGFunction::WaitTrade(const QString &sName /*= ""*/, const QString &myTradeData /*= ""*/, const QString &sTradeData /*= ""*/, int timeout /*= 3000*/)
{
	SetCharacterSwitch(CHARACTER_Trade, true);
	auto tradeDlg = WaitTradeDialog(timeout);
	if (tradeDlg == nullptr)
	{
		qDebug() << "???????????????????????????,????????????";
		DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
		return false;
	}
	if (sName.isEmpty())
	{
		if (tradeDlg->level <= 0)
		{
			qDebug() << "??????????????????,????????????";
			DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
			return false;
		}
	}
	else
	{
		if (tradeDlg->name != sName.toStdString() || tradeDlg->level <= 0)
		{
			qDebug() << "??????????????????,????????????";
			DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
			return false;
		}
	}

	return TradeInternal(sName, myTradeData, sTradeData, timeout);
}

QSharedPointer<CGA::cga_trade_dialog_t> CGFunction::WaitTradeDialog(int timeout)
{
	QSharedPointer<CGA::cga_trade_dialog_t> retDlg;
	qDebug() << "???????????????????????????";
	retDlg = g_pGameCtrl->GetLastTradeDialog();
	if (retDlg != nullptr)
		return retDlg;
	
	bool bSuccess = true;
	QEventLoop loop;
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyTradeDialog, &loop, &QEventLoop::quit);
	loop.exec();
	retDlg = g_pGameCtrl->GetLastTradeDialog();
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return retDlg;
}

bool CGFunction::TradeInternal(const QString &sName /*= ""*/, const QString &myTradeData /*= ""*/, const QString &sTradeData /*= ""*/, int timeout /*= 3000*/)
{
	//??????????????????????????????????????????????????????????????? ???????????????
	//????????????????????? ????????????????????????????????????

	bool bTradeDataOk = true;
	//???????????????
	//myTradeData
	CGA::cga_player_info_t playerinfo;
	g_CGAInterface->GetPlayerInfo(playerinfo);

	CGA::cga_items_info_t itemsinfo;
	g_CGAInterface->GetItemsInfo(itemsinfo);
	CGA::cga_sell_pets_t myPets;

	int myGold = 0;
	CGA::cga_sell_items_t cgaTradeItems;
	if (!myTradeData.isEmpty()) //???????????????????????????
	{
		QStringList tradeDataTypes = myTradeData.split(";");
		for (auto tmpText : tradeDataTypes)
		{
			if (tmpText.left(3) == "??????:")
			{
				QStringList petDatas = tmpText.mid(3).split("|");
				if (petDatas.size() % 2 != 0)
				{
					qDebug() << "?????????????????????????????????";
					DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
					return false;
				}
				//????????????????????????
				for (int i = 0; i < petDatas.size(); i += 2)
				{
					QString requestPetName = petDatas.at(i);
					int requestPetCount = petDatas.at(i + 1).toInt();
					if (requestPetCount > 0 && requestPetCount <= 5)
					{
						int nDstPetCount = 0;
						CGA::cga_pets_info_t petsinfo;
						g_CGAInterface->GetPetsInfo(petsinfo);
						//????????????????????????
						for (auto tmpPet : petsinfo)
						{
							if (tmpPet.realname == requestPetName.toStdString())
							{
								myPets.push_back(tmpPet.index);
								nDstPetCount++;
								if (nDstPetCount == requestPetCount)
									break;
							}
						}
						//?????????????????? ????????????
						if (nDstPetCount != requestPetCount)
						{
							bTradeDataOk = false;
							//break;
							qDebug() << "???????????????????????????????????????";
							DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
							return false;
						}
					}
				}
			}
			else if (tmpText.left(3) == "??????:")
			{

				//???????????????3????????? ???????????? ????????????
				QStringList itemDatas = tmpText.mid(3).split("|");
				if (itemDatas.size() % 3 != 0)
				{
					qDebug() << "???????????????????????????????????????";
					DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
					return false;
				}
				//??????????????????????????????
				QVector<int> existTradePos;
				for (int i = 0; i < itemDatas.size(); i += 3)
				{
					QString requestName = itemDatas.at(i);
					int maxPileCount = itemDatas.at(i + 1).toInt();
					int requestCount = itemDatas.at(i + 2).toInt();
					if (requestCount > 0)
					{
						int nDstReqCount = 0;
						for (size_t i = 0; i < itemsinfo.size(); ++i)
						{
							if (nDstReqCount == requestCount)
							{
								break;
							}
							const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
							if (iteminfo.pos > 7 && (iteminfo.name == requestName.toStdString() || iteminfo.itemid == requestName.toInt()))
							{
								if (iteminfo.count != 0 && iteminfo.count == maxPileCount)
								{
									if (existTradePos.contains(iteminfo.pos))
										continue;

									CGA::cga_sell_item_t cgaItem;
									cgaItem.itemid = iteminfo.itemid;
									cgaItem.itempos = iteminfo.pos;
									cgaItem.count = iteminfo.count == 0 ? 1 : iteminfo.count;
									cgaTradeItems.push_back(cgaItem);
									existTradePos.append(iteminfo.pos);
									nDstReqCount++;
								}
								else if (iteminfo.count == 0)
								{
									if (existTradePos.contains(iteminfo.pos))
										continue;
									CGA::cga_sell_item_t cgaItem;
									cgaItem.itemid = iteminfo.itemid;
									cgaItem.itempos = iteminfo.pos;
									cgaItem.count = iteminfo.count == 0 ? 1 : iteminfo.count;
									cgaTradeItems.push_back(cgaItem);
									existTradePos.append(iteminfo.pos);
									nDstReqCount++;
								}
							}
						}

						//?????????????????? ????????????
						if (nDstReqCount != requestCount)
						{
							bTradeDataOk = false;
							//							break;
							qDebug() << "???????????????????????????????????????????????????";
							DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
							return false;
						}
					}
				}
			}
			else if (tmpText.left(3) == "??????:")
			{
				QString goldDatas = tmpText.mid(3);
				bool bTrans = false;
				int goldNum = goldDatas.toInt(&bTrans);
				if (!bTrans)
				{
					qDebug() << "?????????????????????????????????";
					DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
					return false;
				}
				//??????????????????
				if (playerinfo.gold < goldNum)
				{
					bTradeDataOk = false;
					qDebug() << "??????????????????????????????????????????";
					DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
					return false;
				}
				myGold = goldNum;
			}
			//?????????????????????
		}
	}
	//?????????????????????????????????????????????
	g_CGAInterface->TradeAddStuffs(cgaTradeItems, myPets, myGold);
	//??????????????????
	bool bWaitTradeRet = false;
	auto tradeItems = WaitTradeStuffs(&bWaitTradeRet, 180000); //180???
	if (!bWaitTradeRet)
	{
		qDebug() << "??????????????????????????????????????????";
		return false;
	}
	//???????????? ?????????????????? ???????????????
	//?????????????????????:"??????:?????????|1|?????????|1;??????:????????????Lv9|3|??????|1;??????:150000"
	//?????????lua?????????????????????????????????????????????json?????????????????????????????????????????????????????????????????????
	if (!sTradeData.isEmpty()) //???????????????????????????
	{
		QStringList tradeDataTypes = sTradeData.split(";");
		for (auto tmpText : tradeDataTypes)
		{
			if (tmpText.left(3) == "??????:")
			{
				QStringList petDatas = tmpText.mid(3).split("|");
				if (petDatas.size() % 2 != 0)
				{
					qDebug() << "?????????????????????????????????";
					DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
					return false;
				}
				//????????????????????????
				for (int i = 0; i < petDatas.size(); i += 2)
				{
					QString requestPetName = petDatas.at(i);
					int requestPetCount = petDatas.at(i + 1).toInt();
					if (requestPetCount > 0 && requestPetCount <= 5)
					{
						int nDstPetCount = 0;
						for (auto tmpPet : tradeItems->pets)
						{
							if (tmpPet.realname == requestPetName.toStdString())
							{
								nDstPetCount++;
							}
						}
						//?????????????????? ????????????
						if (nDstPetCount != requestPetCount)
						{
							bTradeDataOk = false;
							//break;
							qDebug() << "???????????????????????????????????????";
							DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
							return false;
						}
					}
				}
			}
			else if (tmpText.left(3) == "??????:")
			{
				QStringList itemDatas = tmpText.mid(3).split("|");
				if (itemDatas.size() % 3 != 0)
				{
					qDebug() << "?????????????????????????????????";
					DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
					return false;
				}
				//??????????????????????????????
				for (int i = 0; i < itemDatas.size(); i += 3)
				{
					QString requestName = itemDatas.at(i);
					int maxPileCount = itemDatas.at(i + 1).toInt();
					int requestCount = itemDatas.at(i + 2).toInt();
					if (requestCount > 0)
					{
						int nDstReqCount = 0; //???
						for (auto tmpItem : tradeItems->items)
						{
							if (tmpItem.name == requestName.toStdString() || tmpItem.itemid == requestName.toInt())
							{
								if (tmpItem.count != 0 && tmpItem.count == maxPileCount)
								{
									nDstReqCount++;
								}
								else if (tmpItem.count == 0 || tmpItem.count == 1)
								{
									nDstReqCount++;
								}
							}
						}
						//?????????????????? ????????????
						if (nDstReqCount != requestCount)
						{
							bTradeDataOk = false;
							//							break;
							qDebug() << "???????????????????????????????????????";
							DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
							return false;
						}
					}
				}
			}
			else if (tmpText.left(3) == "??????:")
			{
				QString goldDatas = tmpText.mid(3);
				bool bTrans = false;
				int goldNum = goldDatas.toInt(&bTrans);
				if (!bTrans)
				{
					qDebug() << "???????????????????????????";
					DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
					return false;
				}
				//??????????????????
				if (tradeItems->gold != goldNum)
				{
					bTradeDataOk = false;
					qDebug() << "???????????????????????????????????????";
					DoCharacterAction(TCharacter_Action_TRADE_REFUSE);
					return false;
				}
			}
			//?????????????????????
		}
	}
	/*int tradeState = WaitTradeState();
	if (tradeState == -1)
	{
		qDebug() << "????????????????????????";
		return false;
	}
	if (tradeState == TRADE_STATE_READY)
	{
	}*/
	//???????????????????????????????????????????????????????????????????????????????????????????????????
	DoCharacterAction(TCharacter_Action_TRADE_CONFIRM);
	qDebug() << "?????????????????????";
	WaitTradeMsg();
}

bool CGFunction::WaitTradeMsg()
{
	std::tuple<int, QString> sMsgData = WaitSysMsg(15000); //15???????????? ?????? -1????????????
	QString sysMsg = std::get<1>(sMsgData);
	if (sysMsg.isEmpty())
	{
		return false;
	}
	if (sysMsg.contains("????????????"))
	{
		qDebug() << "???????????????";
		return false;
	}
	else if (sysMsg.contains("????????????") || sysMsg.contains("????????????????????????????????????"))
	{
		DoCharacterAction(TCharacter_Action_TRADE_REFUSE); //????????????
		qDebug() << sysMsg;
		return false;
	}
	else if (sysMsg.contains("????????????????????????"))
	{
		DoCharacterAction(TCharacter_Action_TRADE_REFUSE); //????????????
		qDebug() << sysMsg;
		return false;
	}
	return true;
}

//????????????????????? ????????????????????????????????????????????? ?????????
QSharedPointer<GameTradeStuff> CGFunction::WaitTradeStuffs(bool *bRet, int timeout /*=3000*/)
{
	bool bWaitRet = false;
	QMutex mutex;
	QSharedPointer<GameTradeStuff> tradeItems(new GameTradeStuff);
	QEventLoop loop;
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyTradeStuffs, [&](QSharedPointer<CGA::cga_trade_stuff_info_t> stuffInfo)
			{
				qDebug() << "Recv WaitTradeStuffs Data";
				//tradeItems = stuffInfo;
				if (stuffInfo->type == TRADE_STUFFS_ITEM)
				{
					tradeItems->items = stuffInfo->items;
				}
				else if (stuffInfo->type == TRADE_STUFFS_PET)
				{
					tradeItems->pets[stuffInfo->pet.index] = stuffInfo->pet;
				}
				else if (stuffInfo->type == TRADE_STUFFS_PETSKILL)
				{
					tradeItems->petskills[stuffInfo->petskills.index] = stuffInfo->petskills;
				}
				else if (stuffInfo->type == TRADE_STUFFS_GOLD)
				{
					tradeItems->gold = stuffInfo->gold;
				}
				//loop.quit();
			});
	auto connection2 = connect(g_pGameCtrl, &GameCtrl::NotifyTradeState, [&](int state)
			{
				QMutexLocker locker(&mutex);

				qDebug() << "?????????????????????:" + m_tradeState.value(state) << " ??????" << state;
				//if (state == TRADE_STATE_READY || state == TRADE_STATE_CONFIRM)//?????????????????? ??? ????????????
				//{
				//	loop.quit();
				//}
				//else if (state == TRADE_STATE_SUCCEED || state == TRADE_STATE_CANCEL)
				//{
				//	loop.quit();
				//}
				bWaitRet = true;
				if (loop.isRunning())
					loop.quit();
			});
	loop.exec();
	qDebug() << "WaitTradeStuffs End";
	QObject::disconnect(connection);
	QObject::disconnect(connection2);
	if (bRet != nullptr)
		*bRet = bWaitRet;
	QMutexLocker locker(&mutex);
	return tradeItems;
}

int CGFunction::WaitTradeState(int timeout)
{
	qDebug() << "??????????????????";
	int tradeState = -1;
	QEventLoop loop;
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyTradeState, [&](int state)
			{
				qDebug() << "?????????????????????:" + m_tradeState.value(state) << " ??????" << state;
				tradeState = state;
				if (loop.isRunning())
					loop.quit();
			});
	loop.exec();
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return tradeState;
}

bool CGFunction::IsInNormalState()
{
	int nWordStatus, nGameState;
	g_CGAInterface->GetWorldStatus(nWordStatus);
	g_CGAInterface->GetGameStatus(nGameState);
	return (nWordStatus == 9 && nGameState == 3) ? true : false;
}

int CGFunction::GetGameStatus()
{
	int nGameState;
	g_CGAInterface->GetGameStatus(nGameState);
	return nGameState;
}

int CGFunction::GetWorldStatus()
{
	int nWordStatus;
	g_CGAInterface->GetWorldStatus(nWordStatus);
	return nWordStatus;
}

void CGFunction::WaitTime(int timeout)
{
	//???????????? 1??????????????? ??????????????????????????????????????????????????????
	int timeoutNum = timeout / 1000; //10?????? 600??? ??????Sleep 1???
	for (int i = 0; i < timeoutNum; ++i)
	{
		if (m_bStop)
			return;
		//QApplication::processEvents();
		Sleep(1000);
	}
}

void CGFunction::WaitTimeEx(int timeout)
{
	Sleep(timeout);
}

bool CGFunction::IsOnline()
{
	int ingame = 0;
	if (g_CGAInterface->IsInGame(ingame) && ingame == 1)
		return true;
	return false;
}

bool CGFunction::UseItem(const QString &itemName)
{
	bool bRet = false;
	//GameItemList pItemList = GetGameItems();
	//foreach (auto pItem, pItemList) //??????????????????
	//{
	//	if (pItem->exist && (pItem->name == itemName || pItem->id == itemName.toInt()))
	//	{
	//		g_CGAInterface->UseItem(pItem->pos, bRet);
	//		break;
	//	}
	//}
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.name == itemName.toStdString() || iteminfo.itemid == itemName.toInt())
			{
				g_CGAInterface->UseItem(iteminfo.pos, bRet);
				break;
			}
		}
	}
	return bRet;
}

bool CGFunction::UseItemPos(int npos)
{
	if (npos < 0 || npos > 30)
		return false;

	bool bRet = false;
	g_CGAInterface->UseItem(npos, bRet);
	return bRet;
}

void CGFunction::Equip(QString sName, int pos)
{
	auto pItem = FindBagItem(sName);
	if (!pItem)
		return;
	bool bResult = false;

	if (pos < 0)
	{ //???????????? ??????????????? ?????????????????????????????????
		g_CGAInterface->UseItem(pItem->pos, bResult);
	}
	else
	{
		g_CGAInterface->MoveItem(pItem->pos, pos, -1, bResult);
	}
}

void CGFunction::EquipEx(int nCode, int pos)
{
	auto pItem = FindBagItemFromCode(nCode);
	if (!pItem)
		return;
	bool bResult = false;

	if (pos < 0)
	{ //???????????? ??????????????? ?????????????????????????????????
		g_CGAInterface->UseItem(pItem->pos, bResult);
	}
	else
	{
		g_CGAInterface->MoveItem(pItem->pos, pos, -1, bResult);
	}
}

void CGFunction::UnEquip(QString sName)
{
	int itemPos = FindEquipItemPos(sName);
	int bagIndex = FindInventoryEmptySlot();
	if (bagIndex >= 0 && itemPos >= 0)
	{
		bool bResult = false;
		g_CGAInterface->MoveItem(itemPos, bagIndex, -1, bResult);
	}
}

void CGFunction::UnEquipEx(int nCode)
{
	auto pItem = FindEquipItemFromCode(nCode);
	if (!pItem)
		return;
	int bagIndex = FindInventoryEmptySlot();
	if (bagIndex >= 0 && pItem->pos >= 0)
	{ //???????????? ??????????????? ?????????????????????????????????
		bool bResult = false;
		g_CGAInterface->MoveItem(pItem->pos, bagIndex, -1, bResult);
	}
}

void CGFunction::UnEquipPos(int pos)
{
	if (pos < 0 || pos > 7)
		return;
	int bagIndex = FindInventoryEmptySlot();
	if (bagIndex >= 0)
	{ //???????????? ??????????????? ?????????????????????????????????
		bool bResult = false;
		g_CGAInterface->MoveItem(pos, bagIndex, -1, bResult);
	}
}

bool CGFunction::Exchange(int x, int y, const QString &sName)
{
	if (sName.isEmpty())
		return false;
	bool bResult = false;
	TurnAboutEx(x, y);
	int nCount = GetItemPileCount(sName);
	CGA::cga_buy_items_t items; //vector  index count
	CGA::cga_buy_item_t subItem;
	subItem.index = 0; //????????????0??? ??????????????????????????????????????????????????? ????????????0
	subItem.count = nCount / 20;
	items.push_back(subItem);
	auto TalkNpcSelect = [&](QSharedPointer<CGA_NPCDialog_t> dlg)
	{
		if (!dlg)
			return;

		if (dlg && dlg->type == 27)
		{
			g_CGAInterface->ClickNPCDialog(-1, 0, bResult);
		}
		else if (dlg->type == 28)
		{
			g_CGAInterface->BuyNPCStore(items, bResult);
		}
	};
	auto dlg = g_pGameFun->WaitRecvNpcDialog();
	TalkNpcSelect(dlg);
	dlg = g_pGameFun->WaitRecvNpcDialog();
	TalkNpcSelect(dlg);
	return true;
}

bool CGFunction::ExchangeEx(int nDir, const QString &sName)
{
	QPoint pos = GetDirectionPos(nDir, 2);
	return Exchange(pos.x(), pos.y(), sName);
}

bool CGFunction::Shopping(int index, int count)
{
	//qDebug() << "Shopping" << index << count;
	bool bRet = false;
	CGA::cga_buy_items_t items; //vector  index count
	CGA::cga_buy_item_t subItem;
	subItem.index = index;
	subItem.count = count;
	items.push_back(subItem);
	g_CGAInterface->BuyNPCStore(items, bRet);
	return bRet;
}

bool CGFunction::DropGold(int nVal)
{
	int goldNum = nVal;
	bool bRes = false;
	if (nVal == 0) //?????????
	{
		goldNum = GetCharacterData("??????").toInt();
	}
	else if (nVal > 0) //?????????
	{
	}
	else //????????? ?????????
	{
		goldNum = GetCharacterData("??????").toInt();
		goldNum += nVal;
	}
	g_CGAInterface->MoveGold(goldNum, 3, bRes);
	return bRes;
}

bool CGFunction::DropPet(const QString &petName)
{
	CGA::cga_pets_info_t pets;
	g_CGAInterface->GetPetsInfo(pets);
	int petPos = -1;
	for (auto pet : pets)
	{
		if (pet.realname == petName.toStdString() || pet.name == petName.toStdString())
		{
			petPos = pet.index;
			break;
		}
	}
	if (petPos == -1)
	{
		return false;
	}
	bool bRes = false;
	g_CGAInterface->DropPet(petPos, bRes);
	return bRes;
}

bool CGFunction::DropPetEx(int pos)
{
	bool bRes = false;
	g_CGAInterface->DropPet(pos, bRes);
	return bRes;
}
//???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
//??????("??????", 100000)	//??????10????????????????????????????????????????????????
//??????("??????", -10000) //?????????1??????????????????????????????
bool CGFunction::DepositGold(int nVal)
{
	auto pCharacter = g_pGameFun->GetGameCharacter();
	int realGold = nVal; //???????????????

	if (realGold < 0)
	{
		if (pCharacter->gold > std::abs(realGold))
		{
			realGold = pCharacter->gold - std::abs(realGold);
		}
		else
		{
			qDebug() << "?????????????????????????????????????????????";
			return false;
		}
	}
	if (realGold > pCharacter->gold)
		realGold = pCharacter->gold;
	//??????????????????????????? ??????????????????????????????1000 ???
	bool bRes = false;
	g_CGAInterface->MoveGold(realGold, 1, bRes);
	return bRes;
}

bool CGFunction::DepositPet(const QString &petName, bool bSensitive)
{
	CGA::cga_pets_info_t bankPets;
	g_CGAInterface->GetBankPetsInfo(bankPets);
	if (bankPets.size() >= 5)
	{
		qDebug() << "?????????????????????";
		return false;
	}
	CGA::cga_pets_info_t pets;
	g_CGAInterface->GetPetsInfo(pets);
	int petPos = -1;
	if (bSensitive)
	{
		for (auto pet : pets)
		{
			if (pet.realname == petName.toStdString() || pet.name == petName.toStdString())
			{
				petPos = pet.index;
				break;
			}
		}
	}
	else
	{
		for (auto pet : pets)
		{
			if (QString::fromStdString(pet.realname).contains(petName) || QString::fromStdString(pet.name).contains(petName))
			{
				petPos = pet.index;
				break;
			}
		}
	}

	if (petPos == -1)
	{
		qDebug() << "???????????????" + petName + "?????????!";
		return false;
	}
	bool bRes = false;
	QList<int> bankPetIndex;
	for (auto bankPet : bankPets)
	{
		bankPetIndex.push_back(bankPet.index);
	}
	for (int i = 100; i < 105; ++i)
	{
		if (!bankPetIndex.contains(i))
		{
			g_CGAInterface->MovePet(petPos, i, bRes);
			return bRes;
		}
	}
	return false;
}

bool CGFunction::DepositPetPos(int pos)
{
	CGA::cga_pets_info_t bankPets;
	g_CGAInterface->GetBankPetsInfo(bankPets);
	if (bankPets.size() >= 5)
	{
		qDebug() << "?????????????????????";
		return false;
	}
	int petPos = pos;
	if (petPos == -1)
	{
		qDebug() << "??????????????????????????????!";
		return false;
	}
	bool bRes = false;
	QList<int> bankPetIndex;
	for (auto bankPet : bankPets)
	{
		bankPetIndex.push_back(bankPet.index);
	}
	for (int i = 100; i < 105; ++i)
	{
		if (!bankPetIndex.contains(i))
		{
			g_CGAInterface->MovePet(petPos, i, bRes);
			return bRes;
		}
	}
	return false;
}

bool CGFunction::DepositPetEx(int srcIndex, int bankIndex)
{
	bool bRes = false;
	g_CGAInterface->MovePet(srcIndex, bankIndex, bRes);
	return bRes;
}

//?????? 1??? 2??? 3???
//??????("??????", ??????)
//????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
//??????("??????", 100000) //??????10?????????????????????????????????????????????
//??????("??????", -10000) //????????????????????????1?????????
bool CGFunction::WithdrawGold(int nVal)
{
	int bankGold = 0;
	g_CGAInterface->GetBankGold(bankGold);
	if (bankGold <= 0)
	{
		qDebug() << "?????????????????????????????????";
		return false;
	}
	auto pCharacter = g_pGameFun->GetGameCharacter();
	int realGold = nVal; //???????????????

	if (realGold < 0)
	{
		if (std::abs(realGold) >= pCharacter->gold)
		{
			realGold = (std::abs(realGold) - pCharacter->gold);
		}
		else //??????????????????????????? ???????????????
		{
			qDebug() << "?????????????????????????????????????????????";
			return false;
		}
	}
	if ((realGold + pCharacter->gold) > 1000000) //????????????100???
	{
		realGold = (1000000 - pCharacter->gold);
	}
	if (realGold > bankGold)
		realGold = bankGold;
	bool bRes = false;
	g_CGAInterface->MoveGold(realGold, 2, bRes);
	return bRes;
}

bool CGFunction::WithdrawPet(const QString &petName, bool bSensitive)
{
	CGA::cga_pets_info_t bankPets;
	g_CGAInterface->GetBankPetsInfo(bankPets);
	if (bankPets.size() < 1)
	{
		qDebug() << "?????????????????????";
		return false;
	}
	CGA::cga_pets_info_t pets;
	g_CGAInterface->GetPetsInfo(pets);
	if (pets.size() >= 5)
	{
		qDebug() << "?????????????????????";
		return false;
	}

	int bankPos = -1;
	if (bSensitive)
	{
		for (auto bankPet : bankPets)
		{
			if (bankPet.realname == petName.toStdString() || bankPet.name == petName.toStdString())
			{
				bankPos = bankPet.index;
				break;
			}
		}
	}
	else
	{
		for (auto bankPet : bankPets)
		{
			if (QString::fromStdString(bankPet.realname).contains(petName) || QString::fromStdString(bankPet.name).contains(petName))
			{
				bankPos = bankPet.index;
				break;
			}
		}
	}

	QList<int> petIndex;
	for (auto pet : pets)
	{
		petIndex.push_back(pet.index);
	}
	if (bankPos == -1)
	{
		qDebug() << "???????????????" + petName + "?????????!";
		return false;
	}
	bool bRes = false;
	for (int i = 0; i < 5; ++i)
	{
		if (!petIndex.contains(i))
		{
			g_CGAInterface->MovePet(bankPos, i, bRes);
			return bRes;
		}
	}
	return false;
}

bool CGFunction::WithdrawPetEx(int srcIndex, int bankIndex)
{
	bool bRes = false;
	g_CGAInterface->MovePet(srcIndex, bankIndex, bRes);
	return bRes;
}

bool CGFunction::SaveToBankOnce(QString itemName, int maxCount)
{
	auto bagItem = FindBagItem(itemName);
	if (bagItem == nullptr)
	{
		qDebug() << "?????????????????????, ?????????????????????";
		return false;
	}
	int emptyslot = FindBankEmptySlot(itemName, maxCount);
	if (emptyslot == -1)
	{
		qDebug() << "??????????????????, ?????????????????????";
		return false;
	}

	bool bResult = false;
	g_CGAInterface->MoveItem(bagItem->pos, emptyslot, -1, bResult);
	//qDebug() << "?????????" << itemName << maxCount << " pos:" << itemPos << "bankPos:" << emptyslot << " Success:" << bResult;
	Sleep(800);

	CGA::cga_item_info_t info;
	g_CGAInterface->GetItemInfo(bagItem->pos, info);
	if (info.name.empty() || info.count != bagItem->count)
	{
		//	qDebug() << "??????????????????" << itemName << "???????????????" << emptyslot;
		return true;
	}
	//qDebug() << "??????????????????????????????????????????" << emptyslot << " Info:"<<QString::fromStdString(info.name);
	return false;
}

bool CGFunction::SaveToBankAll(QString itemName, int maxCount)
{
	bool bRet = false;
	int nTryNum = 0;
	do
	{
		bRet = SaveToBankOnce(itemName, maxCount);
		nTryNum += 1;
	} while (bRet && nTryNum < 100);
	return false;
}

bool CGFunction::SavePosToBank(int pos)
{
	CGA::cga_items_info_t myinfos;
	if (!g_CGAInterface->GetBankItemsInfo(myinfos))
		return false;
	//????????????pos???100??????
	QList<int> existUsePos;
	for (int i = 0; i < myinfos.size(); i++)
	{
		CGA::cga_item_info_t itemInfo = myinfos.at(i);
		existUsePos.append(itemInfo.pos - 100);
	}
	QList<int> emptyPos;
	for (int i = 0; i < 80; ++i)
	{
		if (!existUsePos.contains(i))
		{
			emptyPos.append(i + 100);
		}
	}
	if (emptyPos.size() < 1)
	{
		qDebug() << "??????????????????, ?????????????????????";
		return false;
	}
	bool bResult = false;
	g_CGAInterface->MoveItem(pos, emptyPos[0], -1, bResult);
	return bResult;
}
//???????????? ???????????????????????????
bool CGFunction::SaveDstItemCountToBank(QString itemName, int maxCount /*= 0*/)
{
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{ //??????????????????
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7 && iteminfo.name == itemName.toStdString() && iteminfo.count == maxCount)
			{
				int emptyslot = FindBankEmptySlot(itemName, maxCount);
				if (emptyslot == -1)
				{
					qDebug() << "??????????????????, ?????????????????????";
					return false;
				}
				bool bResult = false;
				g_CGAInterface->MoveItem(iteminfo.pos, emptyslot, -1, bResult);
				return bResult;
			}
		}
	}
	qDebug() << "?????????????????????, ?????????????????????";
	return false;
}

bool CGFunction::SaveBagAllItemsToBank()
{
	CGA::cga_items_info_t myinfos;
	if (!g_CGAInterface->GetBankItemsInfo(myinfos))
		return false;
	//????????????pos???100??????
	QList<int> existUsePos;
	for (int i = 0; i < myinfos.size(); i++)
	{
		CGA::cga_item_info_t itemInfo = myinfos.at(i);
		existUsePos.append(itemInfo.pos - 100);
	}
	QList<int> emptyPos;
	for (int i = 0; i < 80; ++i)
	{
		if (!existUsePos.contains(i))
		{
			emptyPos.append(i + 100);
		}
	}
	if (emptyPos.size() < 1)
	{
		qDebug() << "??????????????????, ?????????????????????";
		return false;
	}
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		int bankIndex = 0;
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			if (iteminfo.pos > 7 && ((iteminfo.assess_flags & 1) == 1 || iteminfo.assess_flags == 24))
			{
				if (bankIndex > (emptyPos.size() - 1)) //????????????
				{
					qDebug() << "??????????????????, ?????????????????????";
					return false;
				}
				int emptyslot = emptyPos.at(bankIndex);
				bool bResult = false;
				g_CGAInterface->MoveItem(iteminfo.pos, emptyslot, -1, bResult);
				//	qDebug() << "?????????" << QString::fromStdString(iteminfo.name) << iteminfo.count << " pos:" << iteminfo.pos << "bankPos:" << emptyslot << " Success:" << bResult;
				/*	Sleep(800);
				CGA::cga_item_info_t info;
				g_CGAInterface->GetItemInfo(emptyslot, info);
				if (!info.name.empty())
				{
					qDebug() << "??????????????????" << QString::fromStdString(iteminfo.name) << "???????????????" << emptyslot;
					
				}else
					qDebug() << "??????????????????????????????????????????" << emptyslot << " Info:" << QString::fromStdString(iteminfo.name);		*/
				bankIndex++;
			}
		}
	}
}

bool CGFunction::WithdrawItem(const QString &itemName, int count)
{
	QList<CGA::cga_item_info_t> filterBankInfos;
	CGA::cga_items_info_t bankInfos;
	g_CGAInterface->GetBankItemsInfo(bankInfos);
	bool bTrans = false;
	int nitemCode = itemName.toInt(&bTrans); //??????????????????id  ?????????id???
	for (int i = 0; i < bankInfos.size(); i++)
	{
		CGA::cga_item_info_t itemInfo = bankInfos.at(i);
		if (itemInfo.name == itemName.toStdString() /*|| (bTrans && itemInfo.itemid == nitemCode)*/)
		{
			filterBankInfos.push_back(itemInfo);
		}
	}
	//?????????????????? ??????
	if (filterBankInfos.size() > 0)
	{
		CGA::cga_items_info_t itemsinfo;
		g_CGAInterface->GetItemsInfo(itemsinfo);
		QList<int> existPosList;
		for (int i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			existPosList.append(iteminfo.pos);
		}
		int nCount = 0;
		int bankIndex = 0;
		bool bRes = false;
		for (int i = 8; i < 28; ++i)
		{
			if (!existPosList.contains(i))
			{
				if (bankIndex < count && bankIndex < filterBankInfos.size())
				{
					g_CGAInterface->MoveItem(filterBankInfos.at(bankIndex).pos, i, -1, bRes);
					bankIndex++;
					//	qDebug() << filterBankInfos.at(bankIndex).pos << i;
				}
			}
		}
	}
	return true;
}

bool CGFunction::WithdrawItemAll(const QString &itemName, int count)
{
	QList<CGA::cga_item_info_t> filterBankInfos;
	CGA::cga_items_info_t bankInfos;
	g_CGAInterface->GetBankItemsInfo(bankInfos);
	bool bTrans = false;
	int nitemCode = itemName.toInt(&bTrans);
	for (int i = 0; i < bankInfos.size(); i++)
	{
		CGA::cga_item_info_t itemInfo = bankInfos.at(i);
		if (itemInfo.name == itemName.toStdString() /*|| (bTrans && itemInfo.itemid == nitemCode)*/) //??????????????????id
		{
			filterBankInfos.push_back(itemInfo);
		}
	}
	//?????????????????? ??????
	if (filterBankInfos.size() > 0)
	{
		CGA::cga_items_info_t itemsinfo;
		g_CGAInterface->GetItemsInfo(itemsinfo);
		QList<int> existPosList;
		for (int i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			existPosList.append(iteminfo.pos);
		}
		int nCount = 0;
		int bankIndex = 0;
		bool bRes = false;
		for (int i = 8; i < 28; ++i)
		{
			if (!existPosList.contains(i))
			{
				if (bankIndex < count && bankIndex < filterBankInfos.size())
				{
					g_CGAInterface->MoveItem(filterBankInfos.at(bankIndex).pos, i, -1, bRes);
					bankIndex++;
				}
			}
		}
	}
	return true;
}

bool CGFunction::WithdrawItemAllEx()
{
	QList<CGA::cga_item_info_t> filterBankInfos;
	CGA::cga_items_info_t bankInfos;
	g_CGAInterface->GetBankItemsInfo(bankInfos);
	for (int i = 0; i < bankInfos.size(); i++)
	{
		CGA::cga_item_info_t itemInfo = bankInfos.at(i);
		//if (itemInfo.name == itemName.toStdString() /*|| itemInfo.itemid == itemName.toInt()*/)
		{
			filterBankInfos.push_back(itemInfo);
		}
	}
	//?????????????????? ??????
	int count = 20; //???????????????20??? ??????????????????20
	if (filterBankInfos.size() > 0)
	{
		QList<int> posExist;
		CGA::cga_items_info_t itemsinfo;
		if (g_CGAInterface->GetItemsInfo(itemsinfo))
		{
			for (size_t i = 0; i < itemsinfo.size(); ++i)
			{
				const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
				if (iteminfo.pos > 7)
				{
					posExist.push_back(iteminfo.pos);
				}
			}
		}
		int bankIndex = 0;
		bool bRes = false;
		int nCount = 0;
		for (size_t i = 8; i < 28; i++)
		{
			if (!posExist.contains(i))
			{
				if (bankIndex < count && bankIndex < filterBankInfos.size())
				{
					g_CGAInterface->MoveItem(filterBankInfos.at(bankIndex).pos, i, -1, bRes);
					bankIndex++;
				}
			}
		}
	}
	return true;
}
//??????????????????
void CGFunction::SortBagItems(bool bFront /*=false*/)
{
	auto pItemList = g_pGameFun->GetGameItems();
	QList<QSharedPointer<GameItem> > sortItemList;
	QList<int> sortIdList;
	QMap<int, QList<QSharedPointer<GameItem> > > idMapItem;
	for (auto pItem : pItemList)
	{
		if (pItem->exist && pItem->pos >= 8)
		{
			QSharedPointer<GameItem> newItem(new GameItem);
			newItem->id = pItem->id;
			newItem->pos = pItem->pos;
			sortItemList.append(newItem);
			if (!sortIdList.contains(pItem->id))
			{
				sortIdList.append(pItem->id);
				QList<QSharedPointer<GameItem> > tmpSortItemList;
				tmpSortItemList.append(newItem);
				idMapItem.insert(pItem->id, tmpSortItemList);
			}
			else
			{
				auto tmpSortItemList = idMapItem.value(pItem->id);
				tmpSortItemList.append(newItem);
				idMapItem.insert(pItem->id, tmpSortItemList);
			}
		}
	}
	qSort(sortIdList.begin(), sortIdList.end(), [&](const int &a, const int &b)
			{ return a > b; });
	if (bFront)
	{
		QList<int> itemIndexMgr; //????????????
		bool bRet = false;
		for (int i = 0; i < sortIdList.size(); ++i)
		{
			auto tmpSortItemList = idMapItem.value(sortIdList[i]);
			for (auto tmpSortItem : tmpSortItemList)
			{
				for (int n = 8; n < 28; ++n)
				{
					if (!itemIndexMgr.contains(n))
					{ //????????????
						g_CGAInterface->MoveItem(tmpSortItem->pos, n, -1, bRet);
						itemIndexMgr.append(n);
						//?????????????????????
						for (auto pTmpSortItem : sortItemList)
						{
							if (pTmpSortItem->pos == n)
							{
								pTmpSortItem->pos = tmpSortItem->pos;
								break;
							}
						}
						tmpSortItem->pos = n;
						break;
					}
				}
			}
		}
	}
	else
	{
		QList<int> itemIndexMgr; //????????????
		bool bRet = false;
		for (int i = 0; i < sortIdList.size(); ++i)
		{
			auto tmpSortItemList = idMapItem.value(sortIdList[i]);
			for (auto tmpSortItem : tmpSortItemList)
			{
				//for (int n = 8; n < 28; ++n)
				for (int n = 27; n >= 8; --n)
				{
					if (!itemIndexMgr.contains(n))
					{ //????????????
						g_CGAInterface->MoveItem(tmpSortItem->pos, n, -1, bRet);
						itemIndexMgr.append(n);
						//?????????????????????
						for (auto pTmpSortItem : sortItemList)
						{
							if (pTmpSortItem->pos == n)
							{
								pTmpSortItem->pos = tmpSortItem->pos;
								break;
							}
						}
						tmpSortItem->pos = n;
						break;
					}
				}
			}
		}
	}
}
//?????????  ??????????????????????????? ???????????????
//????????????id ???????????????
void CGFunction::SortBankItems(bool bFront /*= false*/)
{
	CGA::cga_items_info_t myinfos;
	g_CGAInterface->GetBankItemsInfo(myinfos);
	QList<QSharedPointer<GameItem> > sortItemList;
	QList<QString> sortIdList;
	QMap<QString, QList<QSharedPointer<GameItem> > > idMapItem;
	for (int i = 0; i < myinfos.size(); i++)
	{
		CGA::cga_item_info_t itemInfo = myinfos.at(i);
		qDebug() << itemInfo.image_id << itemInfo.itemid << itemInfo.pos << QString::fromStdString(itemInfo.name);
		QSharedPointer<GameItem> newItem(new GameItem);
		newItem->id = itemInfo.itemid;
		newItem->pos = itemInfo.pos;
		newItem->name = QString::fromStdString(itemInfo.name);
		sortItemList.append(newItem);
		if (!sortIdList.contains(newItem->name))
		{
			sortIdList.append(newItem->name);
			QList<QSharedPointer<GameItem> > tmpSortItemList;
			tmpSortItemList.append(newItem);
			idMapItem.insert(newItem->name, tmpSortItemList);
		}
		else
		{
			auto tmpSortItemList = idMapItem.value(newItem->name);
			tmpSortItemList.append(newItem);
			idMapItem.insert(newItem->name, tmpSortItemList);
		}
	}
	qSort(sortIdList.begin(), sortIdList.end(), [&](const QString &a, const QString &b)
			{ return a < b; });
	if (bFront)
	{
		QList<int> itemIndexMgr; //????????????
		bool bRet = false;
		for (int i = 0; i < sortIdList.size(); ++i)
		{
			auto tmpSortItemList = idMapItem.value(sortIdList[i]);
			for (auto tmpSortItem : tmpSortItemList)
			{
				for (int n = 100; n < 180; ++n)
				{
					if (!itemIndexMgr.contains(n))
					{ //????????????
						g_CGAInterface->MoveItem(tmpSortItem->pos, n, -1, bRet);
						itemIndexMgr.append(n);
						//?????????????????????
						for (auto pTmpSortItem : sortItemList)
						{
							if (pTmpSortItem->pos == n)
							{
								pTmpSortItem->pos = tmpSortItem->pos;
								break;
							}
						}
						tmpSortItem->pos = n;
						break;
					}
				}
			}
		}
	}
	else
	{
		QList<int> itemIndexMgr; //????????????
		bool bRet = false;
		for (int i = 0; i < sortIdList.size(); ++i)
		{
			auto tmpSortItemList = idMapItem.value(sortIdList[i]);
			for (auto tmpSortItem : tmpSortItemList)
			{
				//for (int n = 8; n < 28; ++n)
				for (int n = 179; n >= 100; --n)
				{
					if (!itemIndexMgr.contains(n))
					{ //????????????
						g_CGAInterface->MoveItem(tmpSortItem->pos, n, -1, bRet);
						itemIndexMgr.append(n);
						//?????????????????????
						for (auto pTmpSortItem : sortItemList)
						{
							if (pTmpSortItem->pos == n)
							{
								pTmpSortItem->pos = tmpSortItem->pos;
								break;
							}
						}
						tmpSortItem->pos = n;
						break;
					}
				}
			}
		}
	}
}

GameItemList CGFunction::GetMagicFoodItems()
{
	GameItemList pMagicFoodList;
	auto pItemList = GetGameItems();
	for (auto pItem : pItemList)
	{
		if (pItem && pItem->exist && pItem->type == 23) //??????
		{
			pMagicFoodList.append(pItem);
		}
	}
	return pMagicFoodList;
}

GameItemList CGFunction::GetMedicamentFoodItems()
{
	GameItemList pMedicamentList;
	auto pItemList = GetGameItems();
	for (auto pItem : pItemList)
	{
		if (pItem && pItem->exist && pItem->type == 43) //??????
		{
			pMedicamentList.append(pItem);
		}
	}
	return pMedicamentList;
}

QString CGFunction::GetCharacterProfession(QString sJob)
{
	if (sJob.isEmpty())
	{
		sJob = GetCharacterData("??????").toString();
	}
	if (m_professions.isEmpty())
		return sJob;
	for (auto tmpProfession : m_professions)
	{
		QJsonObject tp = tmpProfession.toObject();

		auto titles = tp.value("titles").toArray();
		if (titles.contains(sJob))
		{
			return tp.value("name").toString();
		}
	}
	return "";
}

int CGFunction::GetCharacterRank(QString sJob /*= ""*/)
{
	if (sJob.isEmpty())
	{
		sJob = GetCharacterData("??????").toString();
	}
	if (m_professions.isEmpty())
		return 0;
	for (auto tmpProfession : m_professions)
	{
		QJsonObject tp = tmpProfession.toObject();

		auto titles = tp.value("titles").toArray();
		if (titles.contains(sJob))
		{
			auto tTitle = titles.toVariantList();
			return tTitle.indexOf(sJob);
		}
	}
	return 0;
}

QString CGFunction::GetCharacterOathGroup(QString sJob)
{
	if (sJob.isEmpty())
	{
		sJob = GetCharacterData("??????").toString();
	}
	if (m_professions.isEmpty())
		return sJob;
	for (auto tmpProfession : m_professions)
	{
		QJsonObject tp = tmpProfession.toObject();

		auto titles = tp.value("titles").toArray();
		if (titles.contains(sJob))
		{
			return tp.value("oathGroup").toString();
		}
	}
	return "";
}

QString CGFunction::GetCharacterSex(int image_id)
{
	//qDebug() << "????????????id:" << image_id;
	if (image_id >= 100000 && image_id < 100250)
	{
		return "???";
	}
	else if (image_id >= 100250 && image_id <= 100423)
	{
		return "???";
	}
	else if (image_id >= 100525 && image_id <= 100630)
	{
		return "???";
	}
	else if (image_id >= 100650 && image_id < 100800)
	{
		return "???";
	}
	else if (image_id >= 100800 && image_id < 105250)
	{
		return "???";
	}
	else if (image_id >= 105250 && image_id < 106000)
	{
		return "???";
	}
	else if (image_id >= 106000 && image_id < 106250)
	{
		return "???";
	}
	else if (image_id >= 106250 && image_id < 106400)
	{
		return "???";
	}
	else if (image_id >= 106400 && image_id < 106600)
	{
		return "???";
	}
	else if (image_id >= 106600 && image_id < 106730)
	{
		return "???";
	}
	return "";
}

CharacterPtr CGFunction::GetGameCharacter()
{
	CharacterPtr pNewChar = QSharedPointer<Character>(new Character);
	CGA::cga_player_info_t info;
	if (g_CGAInterface->GetPlayerInfo(info))
	{
		pNewChar->player_index = info.player_index;
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
		pNewChar->sGid = QString::fromStdString(info.gid);
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
	}
	return pNewChar;
}

// ????????????
//v1 = ????????????
//v2 = ????????????
//v3 = ??????
//v4 = ???????????? void
void CGFunction::Chat(const QString &sText, int v1, int v2, int v3)
{
	if (sText.isEmpty())
		return;
	qDebug() << sText;
	QString sLog = sText;
	//if (sLog.size() > 39) //????????????39  ????????????
	//{
	//	while ((sLog.size() - 39) > 0)
	//	{
	//		QString msg = sLog.left(39);
	//		sLog = sLog.mid(39);
	//		g_CGAInterface->SayWords(msg.toStdString(), v1, v2, v3);
	//	}
	//	g_CGAInterface->SayWords(sLog.toStdString(), v1, v2, v3);
	//}
	//else
	g_CGAInterface->SayWords(sLog.toStdString(), v1, v2, v3);
}

void CGFunction::TurnAbout(int nDir)
{
	int x, y;
	g_CGAInterface->GetMapXY(x, y);
	switch (nDir)
	{
		case MOVE_DIRECTION_East:
			x = x + 2;
			y = y;
			break;
		case MOVE_DIRECTION_SouthEast:
			x = x + 2;
			y = y + 2;
			break;
		case MOVE_DIRECTION_South:
			x = x;
			y = y + 2;
			break;
		case MOVE_DIRECTION_SouthWest:
			x = x - 2;
			y = y + 2;
			break;
		case MOVE_DIRECTION_West:
			x = x - 2;
			y = y;
			break;
		case MOVE_DIRECTION_NorthWest:
			x = x - 2;
			y = y - 2;
			break;
		case MOVE_DIRECTION_North:
			x = x;
			y = y - 2;
			break;
		case MOVE_DIRECTION_NorthEast:
			x = x + 2;
			y = y - 2;
			break;
		default: return;
	}
	g_pGameCtrl->ResetRecvNpcDlgState();
	g_CGAInterface->TurnTo(x, y);
}

void CGFunction::TurnAboutEx(int x, int y)
{
	g_pGameCtrl->ResetRecvNpcDlgState();
	g_CGAInterface->TurnTo(x, y);
}

void CGFunction::TurnAboutPointDir(int x, int y)
{
	int nDir = GetOrientation(x, y);
	TurnAbout(nDir);
}

void CGFunction::TurnAboutEx2(QString sDir)
{
	int x, y;
	g_CGAInterface->GetMapXY(x, y);
	if (sDir == "???" || sDir.toLower() == "north")
	{
		x = x;
		y = y - 2;
	}
	else if (sDir == "???" || sDir.toLower() == "east")
	{
		x = x + 2;
		y = y;
	}
	else if (sDir == "???" || sDir.toLower() == "south")
	{
		x = x;
		y = y + 2;
	}
	else if (sDir == "???" || sDir.toLower() == "west")
	{
		x = x - 2;
		y = y;
	}
	else if (sDir == "??????" || sDir.toLower() == "northeast")
	{
		x = x + 2;
		y = y - 2;
	}
	else if (sDir == "??????" || sDir.toLower() == "southeast")
	{
		x = x + 2;
		y = y + 2;
	}
	else if (sDir == "??????" || sDir.toLower() == "northwest")
	{
		x = x - 2;
		y = y - 2;
	}
	else if (sDir == "??????" || sDir.toLower() == "southwest")
	{
		x = x - 2;
		y = y + 2;
	}
	g_pGameCtrl->ResetRecvNpcDlgState();
	g_CGAInterface->TurnTo(x, y);
}

void CGFunction::PickupItem(int nDir)
{
	auto pickPos = GetDirectionPos(nDir, 1);
	g_pGameCtrl->ResetRecvNpcDlgState();
	g_CGAInterface->TurnTo(pickPos.x(), pickPos.y());
}

QPoint CGFunction::GetDirectionPos(int nDir, int nVal /*=1*/)
{
	int x, y;
	g_CGAInterface->GetMapXY(x, y);
	return GetCoordinateDirectionPos(x, y, nDir, nVal);
}

QPoint CGFunction::GetDirectionPosEx(QString sDir, int nVal /*= 1*/)
{
	int nDir = -1;
	if (sDir == "???" || sDir.toLower() == "north")
	{
		nDir = MOVE_DIRECTION_North;
	}
	else if (sDir == "???" || sDir.toLower() == "east")
	{
		nDir = MOVE_DIRECTION_East;
	}
	else if (sDir == "???" || sDir.toLower() == "south")
	{
		nDir = MOVE_DIRECTION_South;
	}
	else if (sDir == "???" || sDir.toLower() == "west")
	{
		nDir = MOVE_DIRECTION_West;
	}
	else if (sDir == "??????" || sDir.toLower() == "northeast")
	{
		nDir = MOVE_DIRECTION_NorthEast;
	}
	else if (sDir == "??????" || sDir.toLower() == "southeast")
	{
		nDir = MOVE_DIRECTION_SouthEast;
	}
	else if (sDir == "??????" || sDir.toLower() == "northwest")
	{
		nDir = MOVE_DIRECTION_NorthWest;
	}
	else if (sDir == "??????" || sDir.toLower() == "southwest")
	{
		nDir = MOVE_DIRECTION_SouthWest;
	}
	return GetDirectionPos(nDir, nVal);
}

QPoint CGFunction::GetCoordinateDirectionPos(int x, int y, int nDir, int nVal)
{
	switch (nDir)
	{
		case MOVE_DIRECTION_East:
			x = x + nVal;
			y = y;
			break;
		case MOVE_DIRECTION_SouthEast:
			x = x + nVal;
			y = y + nVal;
			break;
		case MOVE_DIRECTION_South:
			x = x;
			y = y + nVal;
			break;
		case MOVE_DIRECTION_SouthWest:
			x = x - nVal;
			y = y + nVal;
			break;
		case MOVE_DIRECTION_West:
			x = x - nVal;
			y = y;
			break;
		case MOVE_DIRECTION_NorthWest:
			x = x - nVal;
			y = y - nVal;
			break;
		case MOVE_DIRECTION_North:
			x = x;
			y = y - nVal;
			break;
		case MOVE_DIRECTION_NorthEast:
			x = x + nVal;
			y = y - nVal;
			break;
		default: return QPoint(x, y);
	}
	return QPoint(x, y);
}

bool CGFunction::IsTeamLeader(const QString &sName)
{
	QList<QSharedPointer<GameTeamPlayer> > teamInfos = GetTeamPlayers();
	if (sName.isEmpty())
	{
		if (teamInfos.size() > 0 && teamInfos[0]->is_me)
		{
			return true;
		}
	}
	else
	{
		if (teamInfos.size() > 0 && teamInfos[0]->name == sName)
		{
			return true;
		}
	}

	return false;
}

bool CGFunction::MoveTo(int x, int y, int timeout)
{
	if (m_bMoveing)
		return false;
	int curX, curY;
	g_CGAInterface->GetMapXY(curX, curY);

	//??????????????????????????????????????? ??????8???  ?????????
	if (GetDistance(x, y) > 8)
	{
		qDebug() << QString("??????x??????y????????????,????????????(%1,%2) ????????????(%3,%4)").arg(curX).arg(curY).arg(x).arg(y);
		return false;
	}
	bool bRet = g_CGAInterface->WalkTo(x, y);
	if (bRet == false)
	{
		m_bMoveing = false;
		return false;
	}
	if (timeout < 1000)
		timeout = 10000;
	timeout = timeout / 1000;
	for (int i = 0; i < timeout; ++i)
	{
		if (m_bStop)
			return false;
		g_CGAInterface->GetMapXY(curX, curY);
		if (curX == x && curY == y)
		{
			m_bMoveing = false;
			return true;
		}
		//QApplication::processEvents();
		//Sleep(100);
	}
	m_bMoveing = false;
	return false;
}

bool CGFunction::AutoNavigator(A_FIND_PATH &path, bool isSyncMap, bool isLoop)
{
	m_navigatorLoopCount += 1;
	auto backPath = path;
	int curX, curY;	  //???????????????
	int tarX, tarY;	  //?????????????????????
	int lastX, lastY; //?????????????????????
	lastX = lastY = 0;
	int curMapIndex = GetMapIndex();
	QString curMapName = GetMapName();
	DWORD dwLastTime = 0; //?????????????????????
	DWORD dwCurTime = dwLastTime;
	int dwTimeoutTryCount = 0; //?????? ????????????
	bool isNormal = true;
	auto warpPosList = GetMapEntranceList(); //?????????
	auto startPos = GetMapCoordinate();
	if (path.front().first == startPos.x() && path.front().second == startPos.y())
	{
		auto it = path.begin();
		path.erase(it);
	}
	QPoint walkprePos; //???????????????????????????
	foreach (auto coor, path)
	{
		if (g_pGameCtrl->GetExitGame() || m_bStop)
			break;
		tarX = coor.first;
		tarY = coor.second;
		qDebug() << "?????????" << tarX << "-" << tarY;

		//if (warpPosList.contains(QPoint(coor.first, coor.second)))
		//{
		//	g_CGAInterface->FixMapWarpStuck(1);//??????????????????
		//}
		walkprePos = GetMapCoordinate(); //????????????????????????
		g_CGAInterface->WalkTo(coor.first, coor.second);
		dwLastTime = 0;
		dwTimeoutTryCount = 0;
		while (!m_bStop) //for(int i = 0; i < 1000; ++i)
		{
			//1????????????????????????
			while (!IsInNormalState() && !m_bStop) //?????????????????? ????????????
			{
				/*qDebug() << "AutoNavigator"
						 << "??????????????????";*/
				isNormal = false;
				Sleep(1000); //????????????2??????  ????????????
			}
			//2????????????????????????????????? ???????????????????????????????????????????????????
			if (curMapIndex != GetMapIndex() || curMapName != GetMapName())
			{
				qDebug() << "????????????????????????????????????";
				int gameStatus = 0;
				int lastWarpMap202 = 0;
				//???????????? ????????????
				while (g_CGAInterface->GetGameStatus(gameStatus) && gameStatus == 202)
				{
					auto timestamp = QDateTime::currentDateTime().toTime_t();
					if (lastWarpMap202 == 0)
					{
						lastWarpMap202 = timestamp;
					}
					else if (timestamp - lastWarpMap202 >= 5)
					{
						g_CGAInterface->FixMapWarpStuck(0);
						lastWarpMap202 = timestamp + 8;
						qDebug("???????????? ?????? fix warp");
					}
					Sleep(1000);
				}
				WaitInNormalState();
				////???????????? ??????????????????????????????????????????
				//Sleep(2000);
				if (curMapIndex == GetMapIndex() && curMapName == GetMapName() && isLoop)
				{
					//g_CGAInterface->WalkTo(tarX, tarY); //????????????????????????
					//??????????????????????????????
					qDebug() << "????????????????????????????????? ????????????";
					auto tgtPos = backPath.end();
					return AutoMoveInternal(tgtPos->first, tgtPos->second, false);
				}
				else
				{
					qDebug() << "??????????????????????????????";
					if (IsInRandomMap())
						Sleep(m_mazeWaitTime);
					return false;
				}
			}
			//???????????? ?????????????????????????????????????????????????????? ????????? ????????????????????? ?????????????????????
			if (!isNormal) //???????????????????????? ?????????????????????????????????????????????
			{
				qDebug() << "??????/??????????????????????????????" << tarX << tarY;
				if (GetDistance(tarX, tarY) > 11)
				{
					qDebug() << "??????/??????????????????????????? ????????????11 ????????????????????? ?????????" << tarX << tarY;
					//??????????????????
					QPoint curPos = GetMapCoordinate();
					auto findPath = CalculatePath(curPos.x(), curPos.y(), tarX, tarY);
					if (findPath.size() > 0 && isLoop)
					{
						AutoNavigator(findPath, isSyncMap, false);
					}
					else
					{
						qDebug() << "??????/??????????????????????????? ?????????" << tarX << tarY;
						return false;
					}
				}
				else
				{
					if (!IsReachableTarget(walkprePos.x(), walkprePos.y())) //????????????????????? ????????? ??????????????????????????????ud?????????
					{
						qDebug() << "???????????????????????????????????????????????????????????????";
						WaitInNormalState();
						return true;
					}
					else
					{
						g_CGAInterface->WalkTo(tarX, tarY);
						//Sleep(2000);
					}
				}
				//???????????? ???????????? ?????????????????? ??????
				//if (warpPosList.contains(QPoint(tarX, tarY)))
				//{
				//	g_CGAInterface->FixMapWarpStuck(1); //??????????????????
				//}

				dwLastTime = 0;
				isNormal = true;
			}
			//3??????????????????????????????
			g_CGAInterface->GetMapXY(curX, curY);
			if (curX == tarX && curY == tarY) //????????????
			{
				/*	qDebug() << "??????????????? " << curX << ","
						 << curY << "?????????" << tarX << "," << tarY << "WorldStatus " << GetWorldStatus();*/
				//???????????? ??????mapName???mapNum?????????????????????walk?????????????????????????????????
				//?????????????????????
				if (warpPosList.contains(QPoint(tarX, tarY))) //????????? ?????????????????????
				{
					qDebug() << "??????????????????,?????????????????????" << tarX << "," << tarY;
					int tryNum = 0;
					DWORD fixWarpTime = GetTickCount(); //5?????????
					while (tryNum < 3)
					{
						if ((GetTickCount() - fixWarpTime) > 5000) //5????????? ????????????
						{
							int gameStatus = 0;
							int lastWarpMap202 = 0;
							//???????????? ???????????? ??????????????????????????????500????????????????????????????????????
							while (g_CGAInterface->GetGameStatus(gameStatus) && gameStatus == 202)
							{
								auto timestamp = QDateTime::currentDateTime().toTime_t();
								if (lastWarpMap202 == 0)
								{
									lastWarpMap202 = timestamp;
								}
								else if (timestamp - lastWarpMap202 >= 5)
								{
									g_CGAInterface->FixMapWarpStuck(0);
									lastWarpMap202 = timestamp + 8;
									qDebug("???????????? ?????? fix warp 0");
								}
								Sleep(1000);
							}
							WaitInNormalState();
							tryNum++;
							fixWarpTime = GetTickCount();
							//?????????????????????2??? ????????????????????? ????????????
							//Sleep(2000);
							//??????????????? ?????? ????????????????????????????????? ??????
							g_CGAInterface->GetMapXY(curX, curY);
							//?????????????????? ???????????????????????????????????? ??????????????????
							if (curMapIndex != GetMapIndex() || curMapName != GetMapName())
							{
								qDebug() << "????????????????????????????????????";
								WaitInNormalState();
								if (IsInRandomMap())
									Sleep(m_mazeWaitTime);
								return true;
							}
							else if (curMapIndex == GetMapIndex() && curMapName == GetMapName() && (curX != tarX || curY != tarY)) //????????? ????????????
							{
								qDebug() << "??????????????????????????????";
								WaitInNormalState();
								return true;
							}
							else if (curMapIndex == GetMapIndex() && curMapName == GetMapName() && curX == tarX && curY == tarY) //????????? ????????????
							{
								//???????????? ????????????????????????????????????	 ???????????????????????????
								if (!IsReachableTarget(walkprePos.x(), walkprePos.y())) //????????????????????? ????????? ??????????????????????????????ud?????????
								{
									qDebug() << "???????????????????????????????????????????????????????????????";
									WaitInNormalState();
									return true;
								}
								//else
								//{
								//	//}
								//	qDebug() << "????????????????????????????????????????????????????????????????????????????????????";
								//	g_CGAInterface->WalkTo(tarX, tarY); //????????????????????????
								//	WaitInNormalState();
								//	return true;
								//}

								qDebug() << "??????????????????????????????????????????????????????????????????,????????????" << tarX << tarY;
								g_CGAInterface->FixMapWarpStuck(1); //??????????????????
								QPoint curPos = GetMapCoordinate();
								if (!isLoop)
								{
									g_CGAInterface->WalkTo(tarX, tarY);
									return true;
								}
								else
								{
									bool bTryRet = false;
									if (tarX == curPos.x() && tarY == curPos.y())
									{
										auto tmpPos = GetRandomSpace(curPos.x(), curPos.y(), 1);
										AutoMoveInternal(tmpPos.x(), tmpPos.y(), false);
										bTryRet = AutoMoveInternal(tarX, tarY, false);
									}
									else
										bTryRet = AutoMoveInternal(tarX, tarY, false);
									return bTryRet;
								}
							}
							tryNum++;
						}
						else //??????5??? ???????????????????????? ???????????????????????????
						{
							//?????????????????? ???????????????????????????????????? ??????????????????
							g_CGAInterface->GetMapXY(curX, curY);
							if (curMapIndex != GetMapIndex() || curMapName != GetMapName()) //????????????
							{
								qDebug() << "????????????????????????????????????";
								WaitInNormalState();
								if (IsInRandomMap())
									Sleep(m_mazeWaitTime);
								return true;
							}
							else if (curMapIndex == GetMapIndex() && curMapName == GetMapName() && (curX != tarX || curY != tarY)) //????????? ????????????
							{
								qDebug() << "??????????????????????????????";
								WaitInNormalState();
								return true;
							}
						}
						Sleep(1000); //??????????????????
					}
					qDebug() << "?????????????????????,3??????????????????" << tarX << "," << tarY;
					if (!IsReachableTarget(walkprePos.x(), walkprePos.y())) //????????????????????? ????????? ??????????????????????????????ud?????????
					{
						qDebug() << "???????????????????????????????????????????????????????????????";
						WaitInNormalState();
						return true;
					}
					else
					{
						//}
						qDebug() << "?????????????????????????????????????????????????????????????????????????????????????????????????????????" << tarX << tarY;
						g_CGAInterface->WalkTo(tarX, tarY); //???????????????????????? ????????????  ???????????? ??????????????????
						WaitInNormalState();
						return true;
					}
				}
				break;
			}
			//4???????????????????????????????????????????????? ????????? ?????????????????????????????????????????????????????????
			dwCurTime = GetTickCount();
			if (lastX == curX && lastY == curY)
			{
				if (dwLastTime == 0)
				{
					dwLastTime = dwCurTime; //?????????????????????
				}
				else
				{
					if (dwCurTime - dwLastTime > 10000) //10??????????????????
					{
						dwTimeoutTryCount++;
						if (dwTimeoutTryCount >= 3)
						{
							qDebug() << "??????????????????????????????3???????????????" << tarX << tarY;
							return false;
						}
						dwLastTime = dwCurTime;
						qDebug() << "?????????????????????????????????" << tarX << tarY;
						if (isSyncMap) //?????????????????? ?????????????????????????????? ?????????????????????????????? 
						{
							qDebug() << "????????????????????????????????????????????????????????????????????????";						
							auto tmpPos = GetRandomSpace(curX, curY, 2);
							if (AutoMoveInternal(tmpPos.x(), tmpPos.y(), false) == false)
								return false;	
						}else
						{
							//g_CGAInterface->FixMapWarpStuck(1); //??????????????????
							WaitInNormalState();
							QPoint curPos = GetMapCoordinate();
							auto findPath = CalculatePath(curPos.x(), curPos.y(), tarX, tarY);
							if (findPath.size() == 1 || !isLoop)
							{
								qDebug() << "?????????WalkTo" << tarX << tarY;
								g_CGAInterface->WalkTo(tarX, tarY); //????????????????????????
							}
							else if (findPath.size() > 1 && isLoop)
							{
								qDebug() << "?????????AutoMoveInternal" << tarX << tarY;
								if (AutoMoveInternal(tarX, tarY, false) == false)
									return false;
								dwLastTime = dwCurTime;
							}
							else
							{
								qDebug() << "???????????????????????????" << tarX << tarY;
								return false;
							}
						}						
					}
					//else if (dwCurTime - dwLastTime > 5000) //5??? ????????????
					//{
					//	dwLastTime -= 5000; //???????????????
					//	///????????????????????????????????? ?????????????????????
					//	qDebug() << "5????????????FixMapWarpStuck 1 ????????????" << tarX << tarY;
					//	g_CGAInterface->FixMapWarpStuck(1); //??????????????????
					//	WaitInNormalState();
					//	QPoint curPos = GetMapCoordinate();
					//	auto findPath = CalculatePath(curPos.x(), curPos.y(), tarX, tarY);
					//	if (findPath.size() > 0) //???????????????
					//	{
					//		g_CGAInterface->WalkTo(tarX, tarY); //????????????????????????
					//	}
					//	else
					//	{
					//		qDebug() << "???????????????????????????" << tarX << tarY;
					//		return false;
					//	}
					//}
				}
			}
			lastX = curX;
			lastY = curY;
			//QApplication::processEvents();
			//Sleep(1000);
			Sleep(10);
		}
	}
	WaitInNormalState();
	return true;
}
//0???????????? 1????????????  2?????????????????? 3??????????????????
int CGFunction::AutoMoveTo(int x, int y, int timeout /*=100*/)
{
	if (m_bMoveing)
	{
		qDebug() << "?????????";
		return 0;
	}
	m_navigatorLoopCount = 0;
	if (GetTeammatesCount() > 0 && !IsTeamLeader()) //????????????>0 ????????????????????????  ??????
	{
		qDebug() << "AutoMoveTo ????????????>0 ????????????????????????";
		return 0;
	}
	bool bRet = AutoMoveInternal(x, y, timeout);
	qDebug() << "??????" << x << "," << y << " ???????????? ";
	return bRet;
}

int CGFunction::AutoMoveToOffLineMap(int x, int y, QImage &mapData, int timeout /*= 10000*/)
{
	if (m_bMoveing)
	{
		qDebug() << "?????????";
		return 0;
	}
	m_navigatorLoopCount = 0;
	if (GetTeammatesCount() > 0 && !IsTeamLeader()) //????????????>0 ????????????????????????  ??????
	{
		qDebug() << "AutoMoveTo ????????????>0 ????????????????????????";
		return 0;
	}
	bool bRet = AutoMoveInternalOffLineMap(x, y, mapData, timeout);
	qDebug() << "??????" << x << "," << y << " ???????????? ";
	return bRet;
}

int CGFunction::AutoMoveToEx(int x, int y, QString sMapName, int timeout /*= 100*/)
{
	if (sMapName.isEmpty())
	{
		return AutoMoveTo(x, y, timeout);
	}
	else
	{
		int tryNum = 0;
		while (tryNum < 3)
		{
			AutoMoveTo(x, y, timeout);
			auto curMapName = GetMapName();
			auto curMapNum = GetMapIndex();
			if (!sMapName.isEmpty())
			{
				if (IsInNormalState() && (curMapName == sMapName || curMapNum == sMapName.toInt()))
				{
					//??????????????? ??????1  ????????????3????????????0
					return 1;
				}
			}
			tryNum++;
		}
		qDebug() << "??????3?????????????????????????????????:" << sMapName << " " << x << "," << y;
	}
	return 0;
}

int CGFunction::AutoMoveToPath(std::vector<pair<int, int> > &findPath, int timeout /*=100*/)
{
	if (findPath.size() > 0)
		AutoNavigator(findPath);
	else
		qDebug() << "???????????????????????????";
	return 0;
}

int CGFunction::AutoMoveInternal(int x, int y, int timeout /*= 100*/, bool isLoop)
{
	if (m_navigatorLoopCount >= 20)
		return 0;
	QPoint curPoint = GetMapCoordinate();
	if (curPoint.x() == x && curPoint.y() == y)
	{
		auto warpPosList = GetMapEntranceList(); //?????????
		if (warpPosList.contains(QPoint(x, y)))	 //????????? ?????????????????????
		{
			qDebug() << "AutoMoveTo ???????????? ????????????????????????????????????";
			auto tmpPos = GetRandomSpace(x, y, 1);
			g_CGAInterface->WalkTo(tmpPos.x(), tmpPos.y());
			Sleep(2000);
			g_CGAInterface->WalkTo(x, y);
		}
		qDebug() << "AutoMoveTo ???????????? ?????????";
		return 1;
	}
	m_bMoveing = true;
	WaitInNormalState();
	//???????????? ?????????????????????????????????????????????????????????????????????
	bool isOffLineMap = false;
	QPoint curPos = GetMapCoordinate();
	auto findPath = CalculatePath(curPos.x(), curPos.y(), x, y);
	if (findPath.size() < 1) //????????????????????????
	{
		qDebug() << "??????????????????????????????????????????????????????" << curPos << "tgt:" << x << "," << y;
		QImage mapImage;
		if(LoadOffLineMapImageData(mapImage))
		{
			findPath = CalculatePathEx(mapImage, curPos.x(), curPos.y(), x, y);
			if (findPath.size() > 0)
			{

				qDebug() << "?????????????????????????????????????????????";
				isOffLineMap = true;
			}
		}
		else
		{
			qDebug() << "??????????????????????????????????????????";
		}
		
	}
	bool bRet = false;
	if (findPath.size() > 0)
	{
		bRet = AutoNavigator(findPath,isOffLineMap, isLoop);
	}
	else
	{
		qDebug() << "????????????????????????????????????" << curPos.x() << ","
				 << curPos.y() << "?????????" << x << "," << y << "Normal " << IsInNormalState();
	}
	m_bMoveing = false;
	return bRet;
}

int CGFunction::AutoMoveInternalOffLineMap(int x, int y, QImage &mapData, int timeout /*= 10000*/, bool isLoop /*= true*/)
{
	if (m_navigatorLoopCount >= 20)
		return 0;
	QPoint curPoint = GetMapCoordinate();
	if (curPoint.x() == x && curPoint.y() == y)
	{
		auto warpPosList = GetMapEntranceList(); //?????????
		if (warpPosList.contains(QPoint(x, y)))	 //????????? ?????????????????????
		{
			qDebug() << "AutoMoveTo ???????????? ????????????????????????????????????";
			auto tmpPos = GetRandomSpace(x, y, 1);
			g_CGAInterface->WalkTo(tmpPos.x(), tmpPos.y());
			Sleep(2000);
			g_CGAInterface->WalkTo(x, y);
		}
		qDebug() << "AutoMoveTo ???????????? ?????????";
		return 1;
	}
	m_bMoveing = true;
	WaitInNormalState();
	//???????????? ?????????????????????????????????????????????????????????????????????
	QPoint curPos = GetMapCoordinate();
	auto findPath = CalculatePath(curPos.x(), curPos.y(), x, y);
	if (findPath.size() < 1) //????????????????????????
	{
		qDebug() << "??????????????????????????????????????????????????????" << curPos << "tgt:" << x << "," << y;
		findPath = CalculatePathEx(mapData, curPos.x(), curPos.y(), x, y);
		if (findPath.size() > 0)
			qDebug() << "?????????????????????????????????????????????";
	}
	bool bRet = false;
	if (findPath.size() > 0)
	{
		bRet = AutoNavigator(findPath, true,isLoop);
	}
	else
	{
		qDebug() << "????????????????????????????????????" << curPos.x() << ","
				 << curPos.y() << "?????????" << x << "," << y << "Normal " << IsInNormalState();
	}
	m_bMoveing = false;
	return bRet;
}

int CGFunction::AutoMoveToTgtMap(int tx, int ty, int tgtMapIndex, int timeout /*=100*/)
{
	if (tgtMapIndex == GetMapIndex())
	{
		return AutoMoveTo(tx, ty, timeout);
	}
	if (m_bMapMoveing)
	{
		qDebug() << "?????????????????? ??????";
		return 0;
	}
	return 0;
	//auto pGateMapList = ITObjectDataMgr::getInstance().FindTargetNavigation(tgtMapIndex, QPoint(tx, ty));
	//if (pGateMapList.size() < 1)
	//{
	//	qDebug() << "???????????????????????????";
	//	emit signal_crossMapFini("???????????????????????????");
	//	return 0;
	//}
	//m_bMapMoveing = true;
	//bool bRes = false;
	//for (int i = 0; i < pGateMapList.size(); ++i)
	//{
	//	if (g_pGameCtrl->GetExitGame() || m_bStop)
	//	{
	//		qDebug() << "?????????????????? ?????????????????? m_bStop ??????";
	//		m_bMapMoveing = false;
	//		emit signal_crossMapFini("??????????????????");
	//		return 0;
	//	}
	//	auto pGateMap = pGateMapList.at(i);
	//	int curMapIndex = GetMapIndex();
	//	if (pGateMap->_mapNumber == curMapIndex)
	//	{
	//		qDebug() << "??????????????? ??????????????????" << pGateMap->_x << pGateMap->_y;
	//		int tmpTx = pGateMap->_x;
	//		int tmpTy = pGateMap->_y;
	//		if (pGateMap->_warpType != 0) //1 ??? 2
	//		{
	//			//??????npc????????????
	//			auto tmpTgtPos = GetRandomSpace(tmpTx, tmpTy, 1);
	//			AutoMoveTo(tmpTgtPos.x(), tmpTgtPos.y(), timeout);
	//			TurnAboutEx(tmpTx, tmpTy);
	//			WaitRecvNpcDialog();
	//			int npcSize = pGateMap->_npcSelect.size();
	//			for (int i = 0; i < npcSize; ++i)
	//			{
	//				auto npcText = pGateMap->_npcSelect.at(i);
	//				auto npcSelectOpe = npcText.split(",");
	//				int opetion = 0;
	//				int index = 0;
	//				if (npcSelectOpe.size() == 1)
	//				{
	//					opetion = npcSelectOpe.at(0).toInt();
	//				}
	//				else if (npcSelectOpe.size() == 2)
	//				{
	//					opetion = npcSelectOpe.at(0).toInt();
	//					index = npcSelectOpe.at(1).toInt();
	//				}
	//				else
	//					continue;
	//				g_CGAInterface->ClickNPCDialog(opetion, index, bRes);
	//				if (i == (npcSize - 1))
	//					continue;
	//				WaitRecvNpcDialog();
	//			}
	//		}
	//		else
	//			AutoMoveTo(tmpTx, tmpTy, timeout);
	//		NowhileMapEx(pGateMap->_targetMapNumber, pGateMap->_tx, pGateMap->_ty);
	//	}
	//	else //???????????????????????????
	//	{
	//		qDebug() << "??????????????? ?????????????????????" << curMapIndex << " ????????????" << pGateMap->_mapNumber << pGateMap->_x << pGateMap->_y;
	//		if (i > 0)
	//		{
	//			pGateMap = pGateMapList.at(i - 1);
	//			qDebug() << "??????????????? ?????????????????? ?????????" << curMapIndex << " ????????????" << pGateMap->_mapNumber << pGateMap->_x << pGateMap->_y;
	//			if (pGateMap->_mapNumber == curMapIndex)
	//			{
	//				AutoMoveTo(pGateMap->_x, pGateMap->_y, timeout);
	//				if (g_pGameCtrl->GetExitGame() || m_bStop)
	//				{
	//					qDebug() << "?????????????????? m_bStop ??????";
	//					m_bMapMoveing = false;
	//					emit signal_crossMapFini("??????????????????");
	//					return 0;
	//				}
	//				curMapIndex = GetMapIndex();
	//				if (pGateMap->_mapNumber == curMapIndex)
	//				{
	//					AutoMoveTo(pGateMap->_x, pGateMap->_y, timeout);
	//				}
	//			}
	//			else
	//			{
	//				m_bMapMoveing = false;
	//				qDebug() << "???????????????????????????????????????????????????????????????";
	//				emit signal_crossMapFini("????????????????????????????????????????????????????????????");
	//				return 0;
	//			}
	//		}
	//	}
	//	//1????????????????????????
	//	while (!IsInNormalState() && !m_bStop) //?????????????????? ????????????
	//	{
	//		qDebug() << "??????????????? ??????????????????";
	//		Sleep(1000);
	//	}
	//}
	////??????????????? ?????????????????????  ????????????????????????
	//int curMapIndex = GetMapIndex();
	//if (tgtMapIndex == curMapIndex)
	//{
	//	qDebug() << "??????????????? ??????????????????" << tx << ty;
	//	AutoMoveTo(tx, ty, timeout);
	//}
	//qDebug() << "???????????????????????????";
	//m_bMapMoveing = false;
	//emit signal_crossMapFini("");
}

QList<QPoint> CGFunction::FindRandomEntryEx(int x, int y, int w, int h, QString filterPosList)
{
	QStringList tmpFilterPosList = filterPosList.split(";");
	QList<QPoint> filterPointList;
	for (auto tmpPos : tmpFilterPosList)
	{
		QStringList tpXY = tmpPos.split(",");
		if (tpXY.size() > 1)
			filterPointList.append(QPoint(tpXY.at(0).toInt(), tpXY.at(1).toInt()));
	}
	return FindRandomEntry(x, y, w, h, filterPointList);
}

QList<QPoint> CGFunction::FindRandomEntry(int tgtx, int tgty, int w, int h, QList<QPoint> filterPosList)
{
	int xmax = tgtx + w;
	int ymax = tgty + h;
	DownloadMapEx(tgtx, tgty, w, h);

	QList<QPoint> findPoints;
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		CGA::cga_map_cells_t objCells;
		if (g_CGAInterface->GetMapObjectTable(true, objCells))
		{
			if (cells.x_size && cells.y_size)
			{
				for (int y = 0; y < cells.y_size; ++y)
				{
					for (int x = 0; x < cells.x_size; ++x)
					{
						if (x < objCells.x_size && y < objCells.y_size)
						{
							auto cellObject = objCells.cell.at((size_t)(x + y * objCells.x_size));
							if (cellObject & 0xff)
							{
								if ((cellObject & 0xff) == 3 && x >= tgtx && y >= tgty && x <= xmax && y <= ymax && !filterPosList.contains(QPoint(x, y)))
								{
									findPoints.append(QPoint(x, y));
									qDebug() << "?????????????????????????????????" << x << y;
								}
							}
						}
					}
				}
			}
		}
	}
	return findPoints;
}

QList<QPoint> CGFunction::FindRandomSearchPath(int tx, int ty)
{
	QList<QPoint> searchPath;
	QPoint curPos = GetMapCoordinate();
	QPoint tgtPos = QPoint(tx, ty);
	auto moveAblePosList = GetMovablePoints(curPos);

	auto searchList = MergePoint(moveAblePosList);
	QVector<short> mapData;
	int width = 0;
	int height = 0;
	CreateMapImage(mapData, width, height);
	//????????????????????? ???????????? ?????????100 100 ????????????????????? ????????????
	int nMaxWidth = 100;
	int nMaxHeight = 100;
	if (width < 1 || height < 1 || width > nMaxWidth || height > nMaxHeight)
	{
		qDebug() << "???????????????????????????!SearchMap CreateMapImage Ero!";
		return searchPath;
	}
	TSearchRectList tracePosList;
	//tsp??????
	if (searchList.size() > 1)
	{
		Tsp tsp;
		tsp.SetStart(curPos, tgtPos);
		QVector<QPoint> tmpPosList;
		for (auto pos : searchList)
		{
			if (pos->_rectPosList.contains(curPos) || pos->_rectPosList.contains(tgtPos))
				continue;
			tmpPosList.append(pos->_centrePos);
		}
		if (tmpPosList.size() <= 1 && tmpPosList.size() > 0)
		{
			tracePosList.append(searchList.at(0));
		}
		else if (tmpPosList.size() <= 0)
		{
			for (int i = 0; i < searchList.size(); ++i)
			{
				auto searPos = searchList.at(i);
				tracePosList.append(searPos);
			}
		}
		else
		{
			tsp.Input(tmpPosList, mapData, width, height);
			tsp.SA();
			//	tsp.Print(tsp.GetBestPath(),tmpPosList.size());
			auto bestPath = tsp.GetBestPath();
			for (int i = 0; i < bestPath.citys.size(); ++i)
			{
				auto searPos = searchList.at(bestPath.citys.at(i));
				tracePosList.append(searPos);
			}
		}
	}
	else
	{
		for (int i = 0; i < searchList.size(); ++i)
		{
			auto searPos = searchList.at(i);
			tracePosList.append(searPos);
		}
	}

	//????????????
	//	signal_load_navpath
	QVector<quint32> navpath;
	for (auto tgtPos : tracePosList)
	{
		navpath.push_back((tgtPos->_centrePos.x() & 0xFFFF) | ((tgtPos->_centrePos.y() & 0xFFFF) << 16));
	}
	//emit g_pGameCtrl->signal_load_navpath(navpath);
	int curMapIndex = GetMapIndex();
	//??????????????????
	for (auto tgtPos : tracePosList)
	{
		searchPath.append(tgtPos->_centrePos);
	}
	return searchPath;
}
QList<QPoint> CGFunction::MakeMapOpen()
{
	QList<QPoint> searchPath;
	if (IsInRandomMap() == false) //???????????? ??????
	{
		qDebug() << "????????????????????????????????????????????????";
		return searchPath;
	}

	auto entranceList = GetMazeEntranceList();
	QPoint curPos = GetMapCoordinate();
	QList<QPoint> allMoveAblePosList;
	SearchAroundMapOpen(allMoveAblePosList);
	QPoint inPos;
	if (entranceList.size() == 1)
	{
		inPos = entranceList[0];
	}
	entranceList = GetMazeEntranceList();
	QPoint nextPos;
	for (auto tPos : entranceList)
	{
		if (tPos != inPos)
		{
			nextPos = tPos;
			AutoMoveTo(nextPos.x(), nextPos.y());
			return searchPath;
		}
	}
	return searchPath;
}
//??????????????????????????????
void CGFunction::MakeMapOpenEx()
{
	CGA::cga_map_cells_t map;
	if (!g_CGAInterface->GetMapCollisionTable(true, map))
	{
		qDebug() << "????????????????????????!";
		return;
	}
	if (map.x_size == 0 || map.y_size == 0)
	{
		qDebug() << "??????????????????????????????????????????";
		return;
	}
	QPoint curPos = GetMapCoordinate();
	int defRectWidth = 20;	//???  ??????????????????26 ?????????????????????13*13  26
	int defRectHeight = 20; //???

	int nXCount = map.x_size / defRectWidth;
	int nYCount = map.y_size / defRectHeight;

	qDebug() << nXCount << nYCount;
	TSearchRectList searchRects;
	//?????? ??????bottom??????0  ???????????????????????????
	for (int i = 0; i < nXCount; i++) //???????????????
	{
		for (int j = 0; j < nYCount; j++)
		{
			TSearchRectPtr tSearchPtr(new TSearchRect);
			tSearchPtr->_rect = QRect(i * defRectWidth, j * defRectHeight, defRectWidth, defRectHeight);
			tSearchPtr->_centrePos = tSearchPtr->_rect.center();
			searchRects.append(tSearchPtr);
		}
	}
	int nSurplusXCount = map.x_size % defRectWidth;
	int nSurplusYCount = map.x_size % defRectWidth;
	nYCount = map.y_size % defRectHeight;
	if (nSurplusXCount > 0)
	{
		int tWidth = (map.x_size - nXCount * defRectWidth);
		for (int j = 0; j < nYCount; j++)
		{
			TSearchRectPtr tSearchPtr(new TSearchRect);
			tSearchPtr->_rect = QRect(nXCount * defRectWidth, j * defRectHeight, tWidth, defRectHeight);
			tSearchPtr->_centrePos = tSearchPtr->_rect.center();
			searchRects.append(tSearchPtr);
		}
	}
	if (nSurplusYCount > 0)
	{
		int tHeight = (map.y_size - nYCount * defRectHeight);
		for (int i = 0; i < nYCount; i++)
		{
			TSearchRectPtr tSearchPtr(new TSearchRect);
			tSearchPtr->_rect = QRect(i * defRectWidth, nYCount * defRectHeight, defRectWidth, tHeight);
			tSearchPtr->_centrePos = tSearchPtr->_rect.center();
			searchRects.append(tSearchPtr);
		}
	}
	for (auto tRect : searchRects)
	{
		if (tRect->_rect.contains(curPos))
		{
			qDebug() << tRect->_rect << curPos << searchRects.indexOf(tRect);
			break;
		}
	}
	return;

	QList<QPoint> searchPath;
	auto entranceList = GetMazeEntranceList();

	QList<QPoint> allMoveAblePosList;
	SearchAroundMapOpen(allMoveAblePosList);
	QPoint inPos;
	if (entranceList.size() == 1)
	{
		inPos = entranceList[0];
	}
	entranceList = GetMazeEntranceList();
	QPoint nextPos;
	for (auto tPos : entranceList)
	{
		if (tPos != inPos)
		{
			nextPos = tPos;
			AutoMoveTo(nextPos.x(), nextPos.y());
			return;
		}
	}
	return;
}

void CGFunction::MakeMapOpenContainNextEntrance(int isNearFar)
{
	int index1, index2, index3;
	std::string filemap;
	g_CGAInterface->GetMapIndex(index1, index2, index3, filemap);
	if (IsInRandomMap() == false) //???????????? ??????
	{
		qDebug() << "????????????????????????????????????????????????";
		return;
	}
	if (g_pGameCtrl->GetExitGame() || m_bStop)
		return;
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells) == false)
	{
		qDebug() << "???????????????????????????5???";
		Sleep(5000);
	}
	else
	{
		if (cells.x_size == 0 || cells.y_size == 0)
		{
			qDebug() << "???????????????????????????5???";
			Sleep(5000);
		}
	}
	if (cells.x_size >= 300 || cells.y_size >= 300)
	{
		qDebug() << "????????????300?????????????????????";
		return;
	}
	QPoint curPos = GetMapCoordinate();
	auto entranceList = GetMazeEntranceList();
	if (entranceList.size() >= 2)
	{
		bool bReachable = true;
		for (auto tEntrance : entranceList)
		{
			if (!IsReachableTarget(tEntrance.x(), tEntrance.y()))
			{
				bReachable = false;
				break;
			}
		}
		if (bReachable) //????????????????????? ?????? ??????????????????
		{
			///begin ???????????? ????????????????????????
			CheckUploadMapData();

			qSort(entranceList.begin(), entranceList.end(), [&](QPoint a, QPoint b)
					{
						auto ad = GetDistanceEx(curPos.x(), curPos.y(), a.x(), a.y());
						auto bd = GetDistanceEx(curPos.x(), curPos.y(), b.x(), b.y());
						return ad < bd;
					});
			if (isNearFar) //??????
				AutoMoveTo(entranceList[1].x(), entranceList[1].y());
			else
				AutoMoveTo(entranceList[0].x(), entranceList[0].y());
			return;
		}
	}
	///begin ???????????? ????????????????????????
	if (g_pGameCtrl->GetIsOpenNetToMLAssistTool() && g_pGameCtrl->GetIsOpenSyncMap())
	{
		QImage mapImage;
		bool bMapRes = RpcSocketClient::getInstance().DownloadMapData(mapImage);
		if (bMapRes && mapImage.width() > 0 && mapImage.height() > 0 && mapImage.width() == cells.x_size && mapImage.height() == cells.y_size)
		{
			auto warpList = GetMazeEntranceListFromOfflineMap(mapImage);
			if (warpList.size() >= 2)
			{
				QPoint curPos = GetMapCoordinate();
				bool bReachable = true;
				for (auto tEntrance : warpList)
				{
					auto findPath = CalculatePathEx(mapImage, curPos.x(), curPos.y(), tEntrance.x(), tEntrance.y());
					if (findPath.size() <= 0)
					{
						bReachable = false;
						break;
					}
				}
				if (bReachable) //????????????????????? ?????? ??????????????????
				{
					qSort(warpList.begin(), warpList.end(), [&](QPoint a, QPoint b)
							{
								auto ad = GetDistanceEx(curPos.x(), curPos.y(), a.x(), a.y());
								auto bd = GetDistanceEx(curPos.x(), curPos.y(), b.x(), b.y());
								return ad < bd;
							});
					if (isNearFar) //??????
						AutoMoveToOffLineMap(warpList[1].x(), warpList[1].y(), mapImage);
					else
						AutoMoveToOffLineMap(warpList[0].x(), warpList[0].y(), mapImage);
					return;
				}
			}
		}
	}
	///end

	QList<QPoint> allMoveAblePosList;
	SearchAroundMapOpen(allMoveAblePosList, 2);
	QPoint inPos;
	if (entranceList.size() >= 1)
	{
		inPos = entranceList[0];
	}
	entranceList = GetMazeEntranceList();
	QPoint nextPos;
	for (auto tPos : entranceList)
	{
		if (tPos != inPos)
		{
			nextPos = tPos;
			AutoMoveTo(nextPos.x(), nextPos.y());
			return;
		}
	}
	//???????????????????????????????????????????????????????????????????????????
	return;
}

bool CGFunction::SearchAroundMapOpen(QList<QPoint> &allMoveAblePosList, int type)
{
	if (g_pGameCtrl->GetExitGame() || m_bStop)
		return false;
	if (IsInRandomMap() == false) //???????????? ??????
	{
		qDebug() << "????????????????????????????????????????????????";
		return false;
	}
	QPoint curPos = GetMapCoordinate();
	//???????????????????????????????????????
	auto moveAblePosList = GetMovablePoints(curPos);
	if (moveAblePosList.size() < 1)
		return false;
	auto moveAbleRangePosList = GetMovablePointsEx(curPos, 13);
	auto clipMoveAblePosList = GetMovablePointsEx(curPos, 12);
	QList<QPoint> newMoveAblePosList = moveAbleRangePosList;
	//???????????????4???????????????
	for (int i = 0; i < clipMoveAblePosList.size(); ++i)
	{
		newMoveAblePosList.removeOne(clipMoveAblePosList[i]);
	}
	//?????????????????????????????????????????????????????????????????????????????????????????????????????????
	QList<QPoint> filterMoveAblePosList = newMoveAblePosList;
	for (auto tPos : newMoveAblePosList)
	{
		qDebug() << tPos;
		//AutoMoveTo(tPos.x(),tPos.y());
		if (allMoveAblePosList.contains(tPos))
			filterMoveAblePosList.removeOne(tPos);
	}
	//???????????????????????????
	auto tSearchList = MergePoint(filterMoveAblePosList);
	if (tSearchList.size() > 0)
	{

		//??????????????????????????????
		qSort(tSearchList.begin(), tSearchList.end(), [&](TSearchRectPtr a, TSearchRectPtr b)
				{
					auto ad = GetDistanceEx(curPos.x(), curPos.y(), a->_centrePos.x(), a->_centrePos.y());
					auto bd = GetDistanceEx(curPos.x(), curPos.y(), b->_centrePos.x(), b->_centrePos.y());
					return ad < bd;
				});
		allMoveAblePosList += clipMoveAblePosList;
		for (auto tSearchPos : tSearchList)
		{
			//auto tSearchPos = tSearchList[0];
			AutoMoveTo(tSearchPos->_centrePos.x(), tSearchPos->_centrePos.y());
			Sleep(m_mazeSearchWaitTime);	//??????????????? ??????????????????????????????????????????????????????????????????
			if (type == 1)
			{
				if (SearchAroundMapOpen(allMoveAblePosList, type))
					return true;
			}
			else if (type == 2)
			{
				auto entranceList = GetMazeEntranceList();
				if (entranceList.size() >= 2)
				{
					bool bReachable = true;
					for (auto tEntrance : entranceList)
					{
						if (!IsReachableTarget(tEntrance.x(), tEntrance.y()))
						{
							bReachable = false;
							break;
						}
					}
					CheckUploadMapData();
					if (bReachable) //????????????????????? ?????? ??????????????????
						return true;
				}
				if (SearchAroundMapOpen(allMoveAblePosList, type))
					return true;
			}
		}
	}
	//for (auto tSearchPos : tSearchList)
	//{
	//	//???????????????????????????
	//	AutoMoveTo(tSearchPos->_centrePos.x(), tSearchPos->_centrePos.y());
	//	allMoveAblePosList += moveAblePosList; //????????????????????????
	//	SearchAroundMapOpen(allMoveAblePosList);
	//}
	return false;
}

bool CGFunction::SearchAroundMapUnit(QList<QPoint> &allMoveAblePosList, QString name, QPoint &findPos, QPoint &enterPos,
		QPoint &nextPos, int searchType, QList<QPoint> filterPosList, std::function<QVariantList(QPoint findPos, QPoint nextPos)> callBack)
{
	if (g_pGameCtrl->GetExitGame() || m_bStop)
		return 4;
	if (IsInRandomMap() == false) //???????????? ??????
	{
		qDebug() << "????????????????????????????????????????????????";
		return 3;
	}
	QPoint curPos = GetMapCoordinate();
	//???????????????????????????????????????
	auto moveAblePosList = GetMovablePoints(curPos);
	if (moveAblePosList.size() < 1)
		return 0;

	auto moveAbleRangePosList = GetMovablePointsEx(curPos, 13);
	auto clipMoveAblePosList = GetMovablePointsEx(curPos, 12);
	QList<QPoint> newMoveAblePosList = moveAbleRangePosList;
	//???????????????4???????????????
	for (int i = 0; i < clipMoveAblePosList.size(); ++i)
	{
		newMoveAblePosList.removeOne(clipMoveAblePosList[i]);
	}
	//?????????????????????????????????????????????????????????????????????????????????????????????????????????
	QList<QPoint> filterMoveAblePosList = newMoveAblePosList;
	for (auto tPos : newMoveAblePosList)
	{
		qDebug() << tPos;
		//AutoMoveTo(tPos.x(),tPos.y());
		if (allMoveAblePosList.contains(tPos))
			filterMoveAblePosList.removeOne(tPos);
	}
	//???????????????????????????
	auto tSearchList = MergePoint(filterMoveAblePosList);
	if (tSearchList.size() > 0)
	{
		//??????????????????????????????
		qSort(tSearchList.begin(), tSearchList.end(), [&](TSearchRectPtr a, TSearchRectPtr b)
				{
					auto ad = GetDistanceEx(curPos.x(), curPos.y(), a->_centrePos.x(), a->_centrePos.y());
					auto bd = GetDistanceEx(curPos.x(), curPos.y(), b->_centrePos.x(), b->_centrePos.y());
					return ad < bd;
				});
		allMoveAblePosList += clipMoveAblePosList;
		for (auto tSearchPos : tSearchList)
		{
			//auto tSearchPos = tSearchList[0];
			AutoMoveTo(tSearchPos->_centrePos.x(), tSearchPos->_centrePos.y());
			Sleep(m_mazeSearchWaitTime); //??????????????? ??????????????????????????????????????????????????????????????????
			if (findPos == QPoint(0, 0))
			{
				auto mapUnit = FindMapUnit(name, searchType);
				if (mapUnit && filterPosList.contains(QPoint(mapUnit->xpos, mapUnit->ypos)) == false) //?????? ????????????
				{
					qDebug() << "???????????????" << name << " ??????:" << mapUnit->xpos << mapUnit->ypos << "ModelID???" << mapUnit->model_id << " Name:" << QString::fromStdString(mapUnit->unit_name)
							 << " Flags:" << mapUnit->flags << "Valid:" << mapUnit->valid;
					MoveToNpcNear(mapUnit->xpos, mapUnit->ypos, 1);
					findPos.setX(mapUnit->xpos);
					findPos.setY(mapUnit->ypos);
					if (callBack)
					{
						QVariantList luaRet = callBack(findPos, nextPos); //????????????true ????????? ???lua??????nextPos
						if (luaRet.size() >= 3)
						{
							if (luaRet[0].toBool())
								return 2;
							findPos.setX(luaRet[1].toInt());
							findPos.setY(luaRet[2].toInt());
						}
					}
				}
			}
			if (nextPos == QPoint(0, 0))
			{
				auto entranceList = GetMazeEntranceList();
				if (entranceList.size() >= 2)
				{
					bool bReachable = true;
					QPoint tNextPos;
					for (auto tEntrance : entranceList)
					{
						if (!IsReachableTarget(tEntrance.x(), tEntrance.y()))
						{
							bReachable = false;
							break;
						}
						if (tEntrance != enterPos)
							tNextPos = tEntrance;
					}
					if (bReachable) //????????????????????? ?????? ??????????????????
					{
						nextPos = tNextPos; //????????? ???????????????
						if (callBack)
						{
							QVariantList luaRet = callBack(findPos, nextPos); //????????????true ????????? ???lua??????nextPos
							if (luaRet.size() >= 3)
							{
								if (luaRet[0].toBool())
									return 2;
								findPos.setX(luaRet[1].toInt());
								findPos.setY(luaRet[2].toInt());
							}
						}
					}
				}
			}
			if (nextPos == QPoint(0, 0) || findPos == QPoint(0, 0))
			{
				int ret = SearchAroundMapUnit(allMoveAblePosList, name, findPos, enterPos, nextPos, searchType, filterPosList, callBack);
				if (ret)
					return ret;
			}
			else
				break;
		}
	}
	if (nextPos != QPoint(0, 0) && findPos != QPoint(0, 0))
		return 1;
	return 0;
}

//???????????? ????????????????????????
bool CGFunction::IsReachableTargetEx(int sx, int sy, int tx, int ty)
{
	auto findPath = CalculatePath(sx, sy, tx, ty);
	if (findPath.size() < 1) //????????????????????????
	{
		qDebug() << "?????????????????????????????????????????????";
		QImage mapImage;
		if (LoadOffLineMapImageData(mapImage))
		{
			findPath = CalculatePathEx(mapImage, sx, sy, tx, ty);
			if (findPath.size() > 0)
				qDebug() << "?????????????????????????????????????????????";
		}else
		{
			qDebug() << "????????????????????????????????????,??????";
		}
	
	}
	if (findPath.size() > 0)
	{
		return true;
	}
	return false;
}

bool CGFunction::IsReachableTarget(int tx, int ty)
{
	//???????????? ?????????????????????????????????????????????????????????????????????
	try
	{
		QPoint curPos = GetMapCoordinate();
		return IsReachableTargetEx(curPos.x(), curPos.y(), tx, ty);
	}
	catch (const std::exception &e)
	{
		qDebug() << "Try catch IsReachableTarget Ero" << e.what();
	}
	catch (...)
	{
		qDebug() << "Try Catch IsReachableTarget Ero";
	}
	return false;
}

bool CGFunction::FindToRandomEntry(int tgtx, int tgty, int w, int h, QList<QPoint> filterPosList)
{
	int xmax = tgtx + w;
	int ymax = tgty + h;
	DownloadMapEx(tgtx, tgty, w, h);
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		CGA::cga_map_cells_t objCells;
		if (g_CGAInterface->GetMapObjectTable(true, objCells))
		{
			if (cells.x_size && cells.y_size)
			{
				for (int y = 0; y < cells.y_size; ++y)
				{
					for (int x = 0; x < cells.x_size; ++x)
					{
						if (x < objCells.x_size && y < objCells.y_size)
						{
							auto cellObject = objCells.cell.at((size_t)(x + y * objCells.x_size));
							if (cellObject & 0xff)
							{
								if ((cellObject & 0xff) == 3 && x >= tgtx && y >= tgty && x <= xmax && y <= ymax && !filterPosList.contains(QPoint(x, y)))
								{
									qDebug() << "?????????????????????????????????" << x << y;
									QPoint curPos = GetMapCoordinate();
									if (x == curPos.x() && y == curPos.y())
									{
										auto tmpPos = GetRandomSpace(curPos.x(), curPos.y(), 1);
										AutoMoveTo(tmpPos.x(), tmpPos.y());
										AutoMoveTo(x, y);
									}
									else
										AutoMoveTo(x, y);
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}
//lua????????? ????????????????????????
bool CGFunction::FindToRandomEntryEx(int x, int y, int w, int h, QString filterPosList)
{
	QStringList tmpFilterPosList = filterPosList.split(";");
	QList<QPoint> filterPointList;
	for (auto tmpPos : tmpFilterPosList)
	{
		QStringList tpXY = tmpPos.split(",");
		if (tpXY.size() > 1)
			filterPointList.append(QPoint(tpXY.at(0).toInt(), tpXY.at(1).toInt()));
	}
	return FindToRandomEntry(x, y, w, h, filterPointList);
}

void CGFunction::SearchMaze(QString name)
{
	auto mazeEntranceList = GetMazeEntranceList();
	auto curPos = GetMapCoordinate();
}

void CGFunction::MoveToNpc(const QString &npcName)
{
	auto unit = FindPlayerUnit(npcName);
	if (unit)
	{
		auto pos = GetRandomSpace(unit->xpos, unit->ypos);
		AutoMoveTo(pos.x(), pos.y());
	}
}

void CGFunction::MoveToNpcEx(const QString &npcName, int nDir, int dis)
{
	auto unit = FindPlayerUnit(npcName);
	if (unit)
	{
		//????????????????????? ??????????????????????????? ?????????????????????
		auto pos = GetCoordinateDirectionPos(unit->xpos, unit->ypos, nDir, dis);
		if (IsTargetExistWall(pos.x(), pos.y()) != 0)
		{
			qDebug() << "????????????";
			return;
		}
		AutoMoveTo(pos.x(), pos.y());
	}
}

void CGFunction::MoveToNpcNear(int x, int y, int dis)
{
	dis = dis ? dis : 1;
	auto pos = GetRandomSpace(x, y, dis);
	if (IsTargetExistWall(pos.x(), pos.y()) != 0)
	{
		qDebug() << "????????????";
		return;
	}
	AutoMoveTo(pos.x(), pos.y());
}

QPoint CGFunction::GetRandomSpace(int x, int y, int distance, bool judgeReachTgt)
{
	int nTempX = 0;
	int nTempY = 0;
	auto warpPosList = GetMapEntranceList(); //?????????

	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		if (x > cells.x_size || y > cells.y_size)
		{
			return QPoint();
		}
		do
		{
			nTempX = x - distance;
			nTempY = y;
			auto FindFun = [&](int x, int y)
			{
				bool bAble = cells.cell.at((size_t)(x + y * cells.x_size)) == 0 && warpPosList.contains(QPoint(x, y)) == false;
				if (bAble && judgeReachTgt)
				{
					return IsReachableTarget(x, y); //
				}
				return bAble;
			}; //???????????????
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x + distance;
			nTempY = y;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x;
			nTempY = y - distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x;
			nTempY = y + distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x + distance;
			nTempY = y + distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x - distance;
			nTempY = y + distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x + distance;
			nTempY = y - distance;
			if (FindFun(nTempX, nTempY))
				break;
			nTempX = x - distance;
			nTempY = y - distance;
			if (FindFun(nTempX, nTempY))
				break;
		} while (0);
	}
	qDebug() << nTempX << nTempY;
	return QPoint(nTempX, nTempY);
}

QPoint CGFunction::GetRandomSpaceOffLine(QImage mapImage, int x, int y, int distance /*= 1*/, bool judgeReachTgt)
{
	int nTempX = 0;
	int nTempY = 0;
	int w = mapImage.width();
	int h = mapImage.height();
	QColor qRgb(0, 0, 0);
	int xtop = h;
	int ytop = w;
	if (x > w || y > h)
	{
		return QPoint();
	}
	do
	{
		nTempX = x - distance;
		nTempY = y;
		auto FindFun = [&](int x, int y)
		{
			bool bAble = mapImage.pixelColor(x, y) != qRgb;
			if (bAble && judgeReachTgt)
			{
				return IsReachableTarget(x, y); //
			}
			return bAble;
		};
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x + distance;
		nTempY = y;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x;
		nTempY = y - distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x;
		nTempY = y + distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x + distance;
		nTempY = y + distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x - distance;
		nTempY = y + distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x + distance;
		nTempY = y - distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x - distance;
		nTempY = y - distance;
		if (FindFun(nTempX, nTempY))
			break;
	} while (0);

	qDebug() << nTempX << nTempY;
	return QPoint(nTempX, nTempY);
}
//???????????????
bool CGFunction::SearchMap(QString name, QPoint &findPos, QPoint &nextPos, int searchType)
{
	//if (!IsMapDownload())
	{
		DownloadMap();
		Sleep(2000); //??????2???
	}
	QPoint curPos = GetMapCoordinate();
	auto mazeEntraceList = GetMazeEntranceList();
	//auto nextMazeEntPos = GetMazeEntrance();
	QPoint nextMazeEntPos;
	for (auto tmpPos : mazeEntraceList)
	{
		if (tmpPos != curPos)
		{
			nextMazeEntPos = tmpPos;
			break;
		}
	}
	qDebug() << "???????????????" << nextMazeEntPos.x() << nextMazeEntPos.y();
	auto mapUnit = FindMapUnit(name, searchType);
	if (mapUnit) //?????? ????????????
	{
		qDebug() << "???????????????" << name << " ??????:" << mapUnit->xpos << mapUnit->ypos << "ModelID???" << mapUnit->model_id << " Name:" << QString::fromStdString(mapUnit->unit_name)
				 << " Flags:" << mapUnit->flags << "Valid:" << mapUnit->valid;
		MoveToNpcNear(mapUnit->xpos, mapUnit->ypos, 1);
		findPos.setX(mapUnit->xpos);
		findPos.setY(mapUnit->ypos);
		nextPos.setX(nextMazeEntPos.x());
		nextPos.setY(nextMazeEntPos.y());
		return true;
	}
	else
	{
		auto moveAblePosList = GetMovablePoints(curPos);
		auto searchList = MergePoint(moveAblePosList);
		QVector<short> mapData;
		int width = 0;
		int height = 0;
		CreateMapImage(mapData, width, height);
		//????????????????????? ???????????? ?????????100 100 ????????????????????? ????????????
		int nMaxWidth = 100;
		int nMaxHeight = 100;
		if (width < 1 || height < 1 || width > nMaxWidth || height > nMaxHeight)
		{
			qDebug() << "???????????????????????????!SearchMap CreateMapImage Ero!";
			return false;
		}
		TSearchRectList tracePosList;
		//tsp??????
		if (searchList.size() > 1)
		{
			Tsp tsp;
			tsp.SetStart(curPos, nextMazeEntPos);
			QVector<QPoint> tmpPosList;
			TSearchRectList restoreSearchList; //????????? ????????????????????????????????????index???????????????
			for (auto pos : searchList)
			{
				/*if (pos->_rectPosList.contains(curPos) || pos->_rectPosList.contains(nextMazeEntPos))
					continue;*/
				restoreSearchList.append(pos);
				tmpPosList.append(pos->_centrePos);
			}
			if (tmpPosList.size() <= 1 && tmpPosList.size() > 0)
			{
				tracePosList.append(searchList.at(0));
			}
			else if (tmpPosList.size() <= 0)
			{
				for (int i = 0; i < searchList.size(); ++i)
				{
					auto searPos = searchList.at(i);
					tracePosList.append(searPos);
				}
			}
			else
			{
				tsp.Input(tmpPosList, mapData, width, height);
				tsp.SA();
				//	tsp.Print(tsp.GetBestPath(),tmpPosList.size());
				auto bestPath = tsp.GetBestPath();
				qDebug() << "BestPath" << bestPath.citys;
				for (int i = 0; i < bestPath.citys.size(); ++i)
				{
					auto searPos = searchList.at(bestPath.citys.at(i));
					tracePosList.append(searPos);
					qDebug() << "searPos" << searPos->_centrePos.x() << searPos->_centrePos.y();
				}
				qDebug() << "TracePosList" << tracePosList;
			}
		}
		else
		{
			for (int i = 0; i < searchList.size(); ++i)
			{
				auto searPos = searchList.at(i);
				tracePosList.append(searPos);
			}
		}
		//????????????
		//	signal_load_navpath
		QVector<quint32> navpath;
		for (auto tgtPos : tracePosList)
		{
			navpath.push_back((tgtPos->_centrePos.x() & 0xFFFF) | ((tgtPos->_centrePos.y() & 0xFFFF) << 16));
		}
		emit g_pGameCtrl->signal_load_navpath(navpath);
		for (auto tgtPos : tracePosList)
		{
			qDebug() << "LastTracePos:" << tgtPos->_centrePos.x() << tgtPos->_centrePos.y();
		}
		int curMapIndex = GetMapIndex();
		//??????????????????
		for (auto tgtPos : tracePosList)
		{
			if (g_pGameCtrl->GetExitGame() || g_pGameFun->m_bStop)
			{
				qDebug() << "???????????????";
				return false;
			}
			if (curMapIndex != GetMapIndex())
			{
				qDebug() << "Map Changed";
				return false;
			}
			//????????? ??????????????? ??????????????????????????????10  ?????????1???
			if (mazeEntraceList.contains(tgtPos->_centrePos))
			{
				auto lastPos = GetRandomSpace(nextMazeEntPos.x(), nextMazeEntPos.y(), 1);
				AutoMoveTo(lastPos.x(), lastPos.y());
			}
			else
				AutoMoveTo(tgtPos->_centrePos.x(), tgtPos->_centrePos.y());
			//??????
			mapUnit = FindMapUnit(name, searchType);
			if (mapUnit) //?????? ????????????
			{
				qDebug() << "???????????????" << name << " ??????:" << mapUnit->xpos << mapUnit->ypos << "ModelID???" << mapUnit->model_id << " Name:" << QString::fromStdString(mapUnit->unit_name)
						 << " Flags:" << mapUnit->flags << "Valid:" << mapUnit->valid;
				MoveToNpcNear(mapUnit->xpos, mapUnit->ypos, 1);
				findPos.setX(mapUnit->xpos);
				findPos.setY(mapUnit->ypos);
				nextPos.setX(nextMazeEntPos.x());
				nextPos.setY(nextMazeEntPos.y());
				return true;
			}
		}
	}
	////??????????????????????????? ????????????  ??????????????????????????????????????????
	//auto lastPos = GetRandomSpace(nextMazeEntPos.x(), nextMazeEntPos.y(),1);
	//AutoMoveTo(lastPos.x(), lastPos.y());
	////??????
	//mapUnit = FindMapUnit(name, searchType);
	//if (mapUnit) //?????? ????????????
	//{
	//	qDebug() << "???????????????" << name << " ??????:" << mapUnit->xpos << mapUnit->ypos << "ModelID???" << mapUnit->model_id << " Name:" << QString::fromStdString(mapUnit->unit_name)
	//			 << " Flags:" << mapUnit->flags << "Valid:" << mapUnit->valid;
	//	MoveToNpcNear(mapUnit->xpos, mapUnit->ypos, 1);
	//	findPos.setX(mapUnit->xpos);
	//	findPos.setY(mapUnit->ypos);
	//	nextPos = nextMazeEntPos;
	//	return true;
	//}
	//???????????? ???????????????
	qDebug() << "???????????? ??????????????????";
	AutoMoveTo(nextMazeEntPos.x(), nextMazeEntPos.y());
	return false;
}
int CGFunction::SearchMapEx(QString name, QPoint &findPos, QPoint &nextPos, int searchType, QString filterPosList,
		std::function<QVariantList(QPoint findPos, QPoint nextPos)> callBack)
{
	//???????????????????????????
	QStringList tmpFilterPosList = filterPosList.split(";");
	QList<QPoint> filterPointList;
	for (auto tmpPos : tmpFilterPosList)
	{
		QStringList tpXY = tmpPos.split(",");
		if (tpXY.size() > 1)
			filterPointList.append(QPoint(tpXY[0].toInt(), tpXY[1].toInt()));
	}
	if (IsInRandomMap() == false) //???????????? ??????
	{
		qDebug() << "??????????????????????????????????????????????????????";
		return 3;
	}
	if (g_pGameCtrl->GetExitGame() || m_bStop)
		return 4;

	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells) == false)
	{
		qDebug() << "???????????????????????????5???";
		Sleep(5000);
	}
	else
	{
		if (cells.x_size == 0 || cells.y_size == 0)
		{
			qDebug() << "???????????????????????????5???";
			Sleep(5000);
		}
	}
	if (cells.x_size >= 300 || cells.y_size >= 300)
	{
		qDebug() << "????????????300?????????????????????";
		return 3;
	}
	QPoint tEntracePos = GetMapCoordinate();
	QPoint tFindPos, tNextMazePos;
	//????????????lua??????
	//if (callBack)
	//{
	//	QVariantList luaRet = callBack(tFindPos, tNextMazePos); //????????????true ????????? ???lua??????nextPos
	//	if (luaRet.size() >= 3)
	//	{
	//		if (luaRet[0].toBool())
	//			return true;
	//		tFindPos.setX(luaRet[1].toInt());
	//		tFindPos.setY(luaRet[2].toInt());
	//	}
	//}
	auto mapUnit = FindMapUnit(name, searchType);
	if (mapUnit && filterPointList.contains(QPoint(mapUnit->xpos, mapUnit->ypos)) == false) //?????? ????????????
	{
		qDebug() << "???????????????" << name << " ??????:" << mapUnit->xpos << mapUnit->ypos << "ModelID???" << mapUnit->model_id << " Name:" << QString::fromStdString(mapUnit->unit_name)
				 << " Flags:" << mapUnit->flags << "Valid:" << mapUnit->valid;
		MoveToNpcNear(mapUnit->xpos, mapUnit->ypos, 1);
		tFindPos.setX(mapUnit->xpos);
		tFindPos.setY(mapUnit->ypos);
		if (callBack)
		{
			QVariantList luaRet = callBack(tFindPos, tNextMazePos); //????????????true ????????? ???lua??????nextPos
			if (luaRet.size() >= 3)
			{
				if (luaRet[0].toBool())
					return 2;
				tFindPos.setX(luaRet[1].toInt());
				tFindPos.setY(luaRet[2].toInt());
			}
		}
	}

	QPoint curPos = GetMapCoordinate();
	auto entranceList = GetMazeEntranceList();
	if (entranceList.size() >= 2)
	{
		bool bReachable = true;
		for (auto tEntrance : entranceList)
		{
			if (!IsReachableTarget(tEntrance.x(), tEntrance.y()))
			{
				bReachable = false;
				break;
			}
		}
		if (bReachable) //????????????????????? ?????? ??????????????????
		{
			qSort(entranceList.begin(), entranceList.end(), [&](QPoint a, QPoint b)
					{
						auto ad = GetDistanceEx(curPos.x(), curPos.y(), a.x(), a.y());
						auto bd = GetDistanceEx(curPos.x(), curPos.y(), b.x(), b.y());
						return ad < bd;
					});
			tNextMazePos = entranceList[1];
			if (callBack)
			{
				QVariantList luaRet = callBack(tFindPos, tNextMazePos); //????????????true ????????? ???lua??????nextPos
				if (luaRet.size() >= 3)
				{
					if (luaRet[0].toBool())
						return 2;
					tFindPos.setX(luaRet[1].toInt());
					tFindPos.setY(luaRet[2].toInt());
				}
			}
		}
	}
	if (tFindPos != QPoint() && tNextMazePos != QPoint())
	{
		qDebug() << "???????????????" << name << " ??????:" << mapUnit->xpos << mapUnit->ypos << "ModelID???" << mapUnit->model_id << " Name:" << QString::fromStdString(mapUnit->unit_name)
				 << " Flags:" << mapUnit->flags << "Valid:" << mapUnit->valid;
		findPos.setX(tFindPos.x());
		findPos.setY(tFindPos.y());
		nextPos.setX(tNextMazePos.x());
		nextPos.setY(tNextMazePos.y());
		MoveToNpcNear(findPos.x(), findPos.y(), 1);
		return 1;
	}
	QList<QPoint> allMoveAblePosList;
	if (SearchAroundMapUnit(allMoveAblePosList, name, tFindPos, tEntracePos, tNextMazePos, searchType, filterPointList, callBack) == 1)
	{
		qDebug() << "??????????????????????????????" << name << " ????????????:" << findPos << "???????????????" << nextPos << "????????????" << GetMapName();
		findPos.setX(tFindPos.x());
		findPos.setY(tFindPos.y());
		nextPos.setX(tNextMazePos.x());
		nextPos.setY(tNextMazePos.y());
		MoveToNpcNear(findPos.x(), findPos.y(), 1);
		return 1;
	}
	if (tNextMazePos == QPoint())
		return 0;
	else
	{
		qDebug() << "???????????? ??????????????????";
		AutoMoveTo(tNextMazePos.x(), tNextMazePos.y());
	}
	return 0;
}

TSearchRectList CGFunction::MergePoint(QList<QPoint> posList, int nDis /*= 10*/)
{
	if (posList.size() < 1)
		return TSearchRectList();
	qDebug() << posList;
	QList<QPoint> alreadyPosList; //??????????????????????????????
	QList<QPoint> tmpPosList = posList;
	TSearchRectList searchRectPosList; //???10?????????????????????????????????????????????
	//???????????????
	while (tmpPosList.size() > 0)
	{
		QPoint centrePos = tmpPosList.front();
		tmpPosList.pop_front();
		if (alreadyPosList.contains(centrePos))
			continue;
		if (g_pGameCtrl->GetExitGame() || g_pGameFun->m_bStop)
		{
			qDebug() << "??????????????? MergePoint";
			return searchRectPosList;
		}
		QPoint leftPos = (centrePos - QPoint(10, 10));
		QPoint rightPos = (centrePos + QPoint(10, 10));
		QRect tmpRect(leftPos, rightPos);
		alreadyPosList.push_back(centrePos);

		TSearchRectPtr tSearchPtr(new TSearchRect);
		tSearchPtr->_rect = tmpRect;
		tSearchPtr->_centrePos = centrePos;
		tSearchPtr->_rectPosList.append(centrePos);
		//tSearchPtr->_cvRectPosList.push_back(cv::Point(centrePos.x(), centrePos.y()));

		//	tSearchPtr->_distance = GetDistanceEx(curPos.x(), curPos.y(), centrePos.x(), centrePos.y());
		searchRectPosList.append(tSearchPtr);

		auto lastPosList = tmpPosList;
		for (QPoint tmpRectPos : lastPosList)
		{
			if (alreadyPosList.contains(tmpRectPos))
				continue;
			if (tmpRectPos == QPoint(52, 8))
				qDebug() << "52,8";
			if (tmpRect.contains(tmpRectPos))
			{
				tmpPosList.removeOne(tmpRectPos);
				alreadyPosList.push_back(tmpRectPos);
				tSearchPtr->_rectPosList.append(tmpRectPos);
				if (tmpRectPos == QPoint(52, 8))
				{
					qDebug() << "52,8" << tSearchPtr->_centrePos.x() << tSearchPtr->_centrePos.y() << tSearchPtr->_rect.topLeft() << tSearchPtr->_rect.bottomRight();
				}
				//				tSearchPtr->_cvRectPosList.push_back(cv::Point(tmpRectPos.x(), tmpRectPos.y()));
			}
		}
	}
	////???????????????
	//for (auto tSearchPtr : searchRectPosList)
	//{
	//	cv::RotatedRect tCvRect = cv::minAreaRect(tSearchPtr->_cvRectPosList);
	//	qDebug() << "Center Cv:" << tCvRect.center.x << "," << tCvRect.center.y;
	//	if (tSearchPtr->_rectPosList.contains(QPoint(tCvRect.center.x, tCvRect.center.y)))
	//	{
	//		//???????????????
	//		tSearchPtr->_centrePos = QPoint(tCvRect.center.x, tCvRect.center.y);
	//		continue;
	//	}
	//	else
	//	{ //???????????????????????????
	//		QPoint closePos;
	//		int nMinDis = 99999;
	//		int tCenterX = tCvRect.center.x;
	//		int tCenterY = tCvRect.center.y;
	//		for (auto tmpPos : tSearchPtr->_rectPosList)
	//		{
	//			double tDis = AStarUtil::manhattan(abs(tCenterX - tmpPos.x()), abs(tCenterY - tmpPos.y()));
	//			if (nMinDis > tDis)
	//			{
	//				nMinDis = tDis;
	//				closePos = tmpPos;
	//			}
	//		}
	//		//???????????????
	//		tSearchPtr->_centrePos = closePos;
	//	}
	//}
	return searchRectPosList;
}

bool CGFunction::FindByNextPoints(CGA::cga_map_cells_t &map, QPoint tmpCentre, QList<QPoint> &foundedPoints, QRect tRect)
{
	QList<QPoint> nextPoints;
	auto isMoveAble = [&](QPoint p)
	{
		if (p.x() > tRect.x() && p.x() < tRect.width() && p.y() > tRect.y() && p.y() < tRect.height())
		{
			if (map.cell.at((size_t)(p.x() + p.y() * map.x_size)) == 0)
			{
				if (!foundedPoints.contains(p))
				{
					foundedPoints.push_back(p);
					nextPoints.push_back(p);
				}
			}
		}
	};

	isMoveAble(QPoint(tmpCentre.x() + 1, tmpCentre.y()));
	isMoveAble(QPoint(tmpCentre.x(), tmpCentre.y() + 1));
	isMoveAble(QPoint(tmpCentre.x() - 1, tmpCentre.y()));
	isMoveAble(QPoint(tmpCentre.x(), tmpCentre.y() - 1));
	for (auto tmpPos : nextPoints)
	{
		FindByNextPoints(map, tmpPos, foundedPoints, tRect);
	}
	return true;
}

QSharedPointer<CGA::cga_map_unit_t> CGFunction::FindMapUnit(const QString &name, int type)
{
	CGA::cga_map_units_t units;
	g_CGAInterface->GetMapUnits(units);
	CGA::cga_map_unit_t mapUnit;
	bool bFind = false;
	if (units.size() > 0)
	{
		switch (type)
		{
			case 0: //??????
			{
				foreach (auto unit, units)
				{
					if (unit.valid && unit.type == 2 && unit.model_id != 0 && (unit.flags & 1024) != 0 && unit.item_name == name.toStdString()) //??????
					{
						mapUnit = unit;
						bFind = true;
						break;
					}
				}
				break;
			}
			case 1: //NPC
			{
				foreach (auto unit, units)
				{
					if (unit.valid && unit.type == 1 && unit.model_id != 0 && (unit.flags & 4096) != 0 && unit.unit_name == name.toStdString()) //NPC
					{
						mapUnit = unit;
						bFind = true;
						break;
					}
				}
				break;
			}
			case 2: //??????
			{
				foreach (auto unit, units)
				{
					if (unit.valid && unit.type == 2 && unit.model_id != 0 && (unit.flags & 1024) != 0 && unit.unit_name == name.toStdString()) //??????
					{
						mapUnit = unit;
						bFind = true;
						break;
					}
				}
				break;
			}
			default:
				break;
		}
		if (bFind)
		{
			QSharedPointer<CGA::cga_map_unit_t> newUnit(new CGA::cga_map_unit_t);
			newUnit->valid = mapUnit.valid;
			newUnit->type = mapUnit.type;
			newUnit->unit_id = mapUnit.unit_id;
			newUnit->model_id = mapUnit.model_id;
			newUnit->xpos = mapUnit.xpos;
			newUnit->ypos = mapUnit.ypos;
			newUnit->item_count = mapUnit.item_count;
			newUnit->injury = mapUnit.injury;
			newUnit->level = mapUnit.level;
			newUnit->flags = mapUnit.flags;
			newUnit->unit_name = mapUnit.unit_name;
			newUnit->nick_name = mapUnit.nick_name;
			newUnit->title_name = mapUnit.title_name;
			newUnit->item_name = mapUnit.item_name;
			return newUnit;
		}
	}
	return nullptr;
}

QList<QPoint> CGFunction::GetMovablePoints(QPoint start)
{
	QList<QPoint> foundedPoints;
	CGA::cga_map_cells_t map;
	QRect tRect;
	if (g_CGAInterface->GetMapCollisionTable(true, map))
	{
		if (map.x_size >= 300 || map.y_size >= 300)
		{
			qDebug() << "????????????300?????????????????????";
			return foundedPoints;
		}
		tRect.setX(map.x_bottom);
		tRect.setY(map.y_bottom);
		tRect.setWidth(map.x_size);
		tRect.setHeight(map.y_size);
		FindByNextPoints(map, start, foundedPoints, tRect);
	}
	return foundedPoints;
}

QList<QPoint> CGFunction::GetMovablePointsEx(QPoint start, int range)
{
	QList<QPoint> foundedPoints;
	CGA::cga_map_cells_t map;
	if (g_CGAInterface->GetMapCollisionTable(true, map))
	{
		int minx = start.x() - range;
		minx = minx < map.x_bottom ? map.x_bottom : minx;
		int miny = start.y() - range;
		miny = miny < map.y_bottom ? map.y_bottom : miny;
		int maxx = start.x() + range;
		maxx = maxx > map.x_size ? map.x_size : maxx;
		int maxy = start.y() + range;
		maxy = maxy > map.y_size ? map.y_size : maxy;
		/*	auto isMoveAble = [&](QPoint p)
		{
			if (p.x() >= minx && p.x() < maxx && p.y() >= miny && p.y() < maxy)
			{
				if (map.cell.at((size_t)(p.x() + p.y() * map.x_size)) == 0)
				{
					if (!foundedPoints.contains(p))
					{
						foundedPoints.push_back(p);
					}
				}
			}
		};*/
		//		isMoveAble(start);
		QRect tRect;

		tRect.setX(minx);
		tRect.setY(miny);
		tRect.setWidth(maxx);
		tRect.setHeight(maxy);
		FindByNextPoints(map, start, foundedPoints, tRect);
	}
	return foundedPoints;
}

int CGFunction::IsTargetExistWall(int x, int y)
{
	int nTempX = 0;
	int nTempY = 0;
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		if (x > cells.x_size || y > cells.y_size)
		{
			qDebug() << "????????????????????????";
			return -1;
		}
		if (cells.cell.at((size_t)(x + y * cells.x_size)) == 0)
			return 0; //?????????
		else
			return 1; //??????
	}
	qDebug() << "????????????????????????";
	return -1;
}

bool CGFunction::IsNearTarget(int x, int y, int dis)
{
	auto curPos = GetMapCoordinate();
	if (abs(curPos.x() - x) <= dis && abs(curPos.y() - y) <= dis)
	{
		return true;
	}
	return false;
}

bool CGFunction::IsNearTarget(int srcx, int srcy, int tgtx, int tgty, int dis)
{
	if (abs(srcx - tgtx) <= dis && abs(srcy - tgty) <= dis)
	{
		return true;
	}
	return false;
}

void CGFunction::MoveGo(int nDir)
{
	QPoint tgtPos = GetDirectionPos(nDir, 1);
	//??????????????????????????? ?????????
	g_CGAInterface->WalkTo(tgtPos.x(), tgtPos.y());
}

int CGFunction::GetRandomSpaceDir(int x, int y)
{
	QPoint spacePos = GetRandomSpace(x, y);
	return GetDirection(x, y, spacePos.x(), spacePos.y());
}
#include <algorithm>
#include <cmath>

int CGFunction::GetOrientation(int tx, int ty)
{
	QPoint p = GetMapCoordinate();
	return GetDirection(p.x(), p.y(), tx, ty);
}

//tx ty?????????????????????   ??????x,y
int CGFunction::GetDirection(int x, int y, int tx, int ty)
{
	if (x == tx && y == ty)
	{
		return MOVE_DIRECTION_Origin;
	}
	double dy = ty - y;
	double dx = tx - x;
	if ((x - tx) == 0 || (y - ty) == 0)
	{
		if ((x - tx) == 0)
		{
			if (ty > y) //???????????? ??????
				return MOVE_DIRECTION_South;
			else
				return MOVE_DIRECTION_North;
		}
		else
		{
			if (tx > x)
				return MOVE_DIRECTION_East;
			else
				return MOVE_DIRECTION_West;
		}
	}
	else
	{
		if (dy > 0 && dx > 0)
		{
			return MOVE_DIRECTION_SouthEast;
		}
		else if (dx > 0 && dy < 0) //????????????
		{
			return MOVE_DIRECTION_NorthEast;
		}
		else if (dx < 0 && dy < 0) //????????????
		{
			return MOVE_DIRECTION_NorthWest;
		}
		else if (dx < 0 && dy > 0) //????????????
		{
			return MOVE_DIRECTION_SouthWest;
		}

		//?????? ?????????x?????? y??????  y????????? ????????????y ???????????????????????????????????????????????????????????????????????????
		//double angle = atan2(ty - y,tx - x); ;//atan2(tx - x, ty - y)
		//double dAnale = angle * 180 / PI;
		//double   angle22d5 = PI / 8;
		//int nDirection = 0;
		//if (angle > -angle22d5 && angle < angle22d5)
		//	nDirection = MOVE_DIRECTION_East;	//??????
		//else if (angle > angle22d5 && angle < angle22d5 * 3)
		//	nDirection = MOVE_DIRECTION_NorthEast;	//??????
		//else if (angle > angle22d5 * 3 && angle < angle22d5 * 5)
		//	nDirection = MOVE_DIRECTION_North;		//???
		//else if (angle > angle22d5 * 5 && angle < angle22d5 * 7)
		//	nDirection = MOVE_DIRECTION_NorthWest;	//??????
		//else if (angle > angle22d5 * 7 || angle < -(angle22d5 * 7))
		//	nDirection = MOVE_DIRECTION_West;		//???
		//else if (angle > -angle22d5 * 3 && angle < -angle22d5)
		//	nDirection = MOVE_DIRECTION_SouthEast;//??????
		//else if (angle > -angle22d5 * 5 && angle < -angle22d5 * 3)
		//	nDirection = MOVE_DIRECTION_South;		//???
		//else if (angle > -angle22d5 * 7 && angle < -angle22d5 * 5)
		//	nDirection = MOVE_DIRECTION_SouthWest;		//??????
		//qDebug() << nDirection << (nDirection + 4) % 8;
		////y?????????
		/*	switch (nDirection)
		{
		case MOVE_DIRECTION_North:nDirection = MOVE_DIRECTION_South;break;
		case MOVE_DIRECTION_NorthEast:nDirection = MOVE_DIRECTION_NorthWest; break;
		case MOVE_DIRECTION_East:nDirection = MOVE_DIRECTION_West; break;
		case MOVE_DIRECTION_SouthEast:nDirection = MOVE_DIRECTION_SouthWest; break;
		case MOVE_DIRECTION_South:nDirection = MOVE_DIRECTION_North; break;
		case MOVE_DIRECTION_SouthWest:nDirection = MOVE_DIRECTION_SouthEast; break;
		case MOVE_DIRECTION_West:nDirection = MOVE_DIRECTION_East; break;
		case MOVE_DIRECTION_NorthWest:nDirection = MOVE_DIRECTION_NorthEast; break;
		default:
			break;
		}		*/
		//nDirection =( nDirection + 4) % 8;
		return 0;
	}
}

int CGFunction::GetDirectionEx(QPoint sPos, QPoint tPos)
{
	return GetDirection(sPos.x(), sPos.y(), tPos.x(), tPos.y());
}

QString CGFunction::GetDirectionText(int nDirection)
{
	//	nDirection += 1;	//?????????????????? -1
	switch (nDirection)
	{
		case MOVE_DIRECTION_South: return "???";
		case MOVE_DIRECTION_West: return "???";
		case MOVE_DIRECTION_North: return "???";
		case MOVE_DIRECTION_East: return "???";
		case MOVE_DIRECTION_NorthWest: return "???";
		case MOVE_DIRECTION_SouthWest: return "???";
		case MOVE_DIRECTION_NorthEast: return "???";
		case MOVE_DIRECTION_SouthEast:
			return "???";
			//	case MOVE_DIRECTION_Origin:return "???";
			//	case 0: return "???";	 //????????? ?????????
		case -1: return "???"; //??????
		default:
			break;
	}
	return "";
}

double CGFunction::GetDistance(int x, int y)
{
	int curX, curY;
	g_CGAInterface->GetMapXY(curX, curY);
	if (curY == y && curX == x)
	{
		return 0;
	}
	double fdistance = sqrt(pow(abs(curX - x), 2) + pow(abs(curY - y), 2));
	return fdistance;
}

double CGFunction::GetDistanceEx(int sx, int sy, int tx, int ty)
{
	if (sx == tx && sy == ty)
		return 0;
	double fdistance = sqrt(pow(abs(sx - tx), 2) + pow(abs(sy - ty), 2));
	return fdistance;
}

double CGFunction::GetAStarDistance(int sx, int sy, int tx, int ty)
{
	auto path = CalculatePath(sx, sy, tx, ty);
	return AStarUtil::pathLength(path);
}

QImage CGFunction::CreateMapImage()
{
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		CGA::cga_map_cells_t objCells;
		g_CGAInterface->GetMapObjectTable(true, objCells);
		qDebug() << "Cell Size:" << cells.cell.size();
		int pixelwidth = cells.x_size;
		int pixelheight = cells.y_size;
		QColor cBlack(0, 0, 0);
		QColor cRed(255, 0, 0);
		QColor cWhite(255, 255, 255);
		QImage image(pixelwidth, pixelheight, QImage::Format_ARGB32);
		for (int tmpy = 0; tmpy < pixelheight; ++tmpy) //
		{
			for (int tmpx = 0; tmpx < pixelwidth; ++tmpx) //
			{
				size_t dataSize = (tmpx + tmpy * cells.x_size);
				auto cellWall = cells.cell.at((size_t)(dataSize)); //?????????????????? ??????
				auto cellObject = objCells.cell.at((size_t)(dataSize));
				if (cellWall == 1) //???????????? 1
				{
					image.setPixelColor(tmpx, tmpy, cBlack);
				}
				else
				{
					if (cellObject & 0xff) //?????????
						image.setPixelColor(tmpx, tmpy, cRed);
					else
						image.setPixelColor(tmpx, tmpy, cWhite);
				}
			}
			//qDebug() << szDebug;
		}
		return image;
	}
	return QImage();
}

bool CGFunction::CreateMapImage(QVector<short> &map, int &widgth, int &height)
{
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		CGA::cga_map_cells_t objCells;
		g_CGAInterface->GetMapObjectTable(true, objCells);
		qDebug() << "Cell Size:" << cells.cell.size();
		int pixelwidth = cells.x_size;
		int pixelheight = cells.y_size;
		map = map.fromStdVector(cells.cell);
		widgth = pixelwidth;
		height = pixelheight;
		return true;
	}
	return false;
}

bool CGFunction::SaveCurrentMapImage(const QString &sTgtPath)
{
	QString sPath = QCoreApplication::applicationDirPath() + "//map//";
	if (sTgtPath.isEmpty())
	{
		int index1, index2, index3;
		std::string sfilemap;
		g_CGAInterface->GetMapIndex(index1, index2, index3, sfilemap);
		if (index1 == 0)
			sPath = QString("%1/%2/%3.bmp").arg(sPath).arg(index1).arg(index3);
		else
			sPath = QString("%1/%2/%3/%4.bmp").arg(sPath).arg(index1).arg(index2).arg(index3);
	}
	else
		sPath = sTgtPath;
	QImage image = g_pGameFun->CreateMapImage();
	return image.save(sPath);
}

void CGFunction::MoveToThread(CGFunction *pThis, int x, int y, int timeout)
{
	pThis->m_bMoveing = true;

	int curX, curY;
	g_CGAInterface->GetMapXY(curX, curY);

	//??????????????????????????????????????? ??????8???  ?????????
	if (abs(curX - x) > 8 || abs(curY - y) > 8)
	{
		qDebug() << QString("??????x??????y????????????,????????????(%1,%2) ????????????(%3,%4)").arg(curX).arg(curY).arg(x).arg(y);
		return;
	}
	bool bRet = g_CGAInterface->WalkTo(x, y);
	if (bRet == false)
	{
		pThis->m_bMoveing = false;
		return;
	}
	/*QtConcurrent::run([&x, &y,&timeout]()
		{
			int curX, curY;
			for (int i = 0; i < timeout; ++i)
			{
				if (g_pGameFun->m_bStop)
				{
					g_pGameFun->m_bMoveing = false;
					return false;
				}
				g_CGAInterface->GetMapXY(curX, curY);
				if (curX == x && curY == y)
				{
					g_pGameFun->m_bMoveing = false;
					return true;
				}
				Sleep(1000);
			}
			g_pGameFun->m_bMoveing = false;
		}
	);*/
	for (int i = 0; i < timeout; ++i)
	{
		if (pThis->m_bStop)
			return;
		g_CGAInterface->GetMapXY(curX, curY);
		if (curX == x && curY == y)
		{
			pThis->m_bMoveing = false;
			return;
		}
		Sleep(1000);
	}
	pThis->m_bMoveing = false;
}

QSharedPointer<CGA_MapUnit_t> CGFunction::FindPlayerUnit(const QString &szName)
{
	CGA::cga_map_units_t units;
	g_CGAInterface->GetMapUnits(units);
	if (units.size() > 0)
	{
		foreach (auto mapUnit, units)
		{
			if (mapUnit.valid && mapUnit.type == 8 && mapUnit.model_id != 0 && (mapUnit.flags & 256) != 0 && mapUnit.unit_name == szName.toStdString())
			{
				QSharedPointer<CGA_MapUnit_t> interMapUnit(new CGA_MapUnit_t);
				interMapUnit->valid = mapUnit.valid;
				interMapUnit->type = mapUnit.type;
				interMapUnit->unit_id = mapUnit.unit_id;
				interMapUnit->model_id = mapUnit.model_id;
				interMapUnit->xpos = mapUnit.xpos;
				interMapUnit->ypos = mapUnit.ypos;
				interMapUnit->item_count = mapUnit.item_count;
				interMapUnit->injury = mapUnit.injury;
				interMapUnit->level = mapUnit.level;
				interMapUnit->flags = mapUnit.flags;
				interMapUnit->unit_name = QString::fromStdString(mapUnit.unit_name);
				interMapUnit->nick_name = QString::fromStdString(mapUnit.nick_name);
				interMapUnit->title_name = QString::fromStdString(mapUnit.title_name);
				interMapUnit->item_name = QString::fromStdString(mapUnit.item_name);
				return interMapUnit;
			}
		}
	}
	return nullptr;
}

QList<QSharedPointer<GameTeamPlayer> > CGFunction::GetTeamPlayers()
{
	CGA::cga_team_players_t teaminfo;
	g_CGAInterface->GetTeamPlayerInfo(teaminfo);

	CGA::cga_map_units_t units;
	g_CGAInterface->GetMapUnits(units);

	CGA::cga_player_info_t playerinfo;
	g_CGAInterface->GetPlayerInfo(playerinfo);
	QList<QSharedPointer<GameTeamPlayer> > pTeamInfoList;
	for (auto tmpTeam : teaminfo)
	{
		QSharedPointer<GameTeamPlayer> newTeam(new GameTeamPlayer);
		newTeam->name = QString::fromStdString(tmpTeam.name);
		newTeam->hp = tmpTeam.hp;
		newTeam->maxhp = tmpTeam.maxhp;
		newTeam->mp = tmpTeam.mp;
		newTeam->maxmp = tmpTeam.maxmp;
		newTeam->x = tmpTeam.xpos;
		newTeam->y = tmpTeam.ypos;
		newTeam->unit_id = tmpTeam.unit_id;
		pTeamInfoList.append(newTeam);
	}
	for (auto tmpTeam : pTeamInfoList)
	{
		for (auto unit : units)
		{
			if (unit.type == 8 && unit.unit_id == tmpTeam->unit_id)
			{
				tmpTeam->name = QString::fromStdString(unit.unit_name);
				tmpTeam->nick_name = QString::fromStdString(unit.nick_name);
				tmpTeam->title_name = QString::fromStdString(unit.title_name);
				tmpTeam->x = unit.xpos;
				tmpTeam->y = unit.ypos;
				tmpTeam->injury = unit.injury;
				tmpTeam->level = unit.level;
				break;
			}
		}
		if (playerinfo.unitid == tmpTeam->unit_id)
		{
			tmpTeam->name = QString::fromStdString(playerinfo.name);
			tmpTeam->level = playerinfo.level;
			tmpTeam->injury = playerinfo.health > 0 ? 1 : 0;
			tmpTeam->is_me = true;
		}
	}
	return pTeamInfoList;
}

void CGFunction::WaitTeammateChatSay()
{
}

void CGFunction::Renew(int nDir)
{
	QPoint dirPos = GetDirectionPos(nDir, 2);
	RenewEx(dirPos.x(), dirPos.y());
}

void CGFunction::RenewEx(int x, int y)
{
	qDebug() << "Renew" << x << y;
	TurnAboutEx(x, y);
	//??????????????????
	int count = 10;
	bool bNeedRenew = true;
	while (count-- && !m_bStop && bNeedRenew)
	{
		auto dlg = WaitRecvNpcDialog();
		bNeedRenew = RenewNpcClicked(dlg);
	}
	return;
}

void CGFunction::RenewEx2(QString sDir)
{
	QPoint dirPos = GetDirectionPosEx(sDir, 2);
	RenewEx(dirPos.x(), dirPos.y());
}

//???????????????????????????
bool CGFunction::RenewNpcClicked(QSharedPointer<CGA_NPCDialog_t> dlg)
{
	bool result = false;
	if (dlg && dlg->type == 2 && dlg->message.indexOf("????????????") >= 0)
	{
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
				return true;
			}
			else if (bNeedMP && playerinfo.gold >= playerinfo.maxmp - playerinfo.mp) //???????????? ???????????????
			{
				g_CGAInterface->ClickNPCDialog(0, 0, result);
				return true;
			}
			//????????????????????? ???????????????
			if (!result && g_CGAInterface->GetPetsInfo(petsinfo))
			{
				if (g_pGameFun->NeedPetSupply(petsinfo)) //????????????
				{
					g_CGAInterface->ClickNPCDialog(0, 4, result);
					return true;
				}
			}
		}
	}
	else if (dlg && dlg->type == 0)
	{
		if (dlg->options == 12) //???
		{
			g_CGAInterface->ClickNPCDialog(4, -1, result); //4 ??? 8??? 32????????? 1??????
			return true;
		}
		if (dlg->options == 1) //??????
		{
			g_CGAInterface->ClickNPCDialog(1, -1, result);
			return true;
		}
	}
	return false;
}

bool CGFunction::AutoWalkMaze(int isDownMap, QString filterPosList, int isNearFar)
{
	MakeMapOpenContainNextEntrance(isNearFar);
	return false;
	if (isDownMap)
	{
		//if (!IsMapDownload())
		{
			DownloadMap();
			Sleep(2000); //??????2???
		}
	}
	QStringList tmpFilterPosList = filterPosList.split(";");
	QList<QPoint> filterPointList;
	for (auto tmpPos : tmpFilterPosList)
	{
		QStringList tpXY = tmpPos.split(",");
		if (tpXY.size() > 1)
			filterPointList.append(QPoint(tpXY.at(0).toInt(), tpXY.at(1).toInt()));
	}
	qDebug() << "????????????";
	QPoint nextPos;
	if (filterPointList.size() > 0)
	{
		auto mazeTgtList = GetMazeEntranceList();
		for (auto tmpMaze : mazeTgtList)
		{
			if (!filterPointList.contains(tmpMaze))
			{
				nextPos = tmpMaze;
				break;
			}
		}
	}
	else
	{
		nextPos = GetMazeEntrance(isNearFar);
	}
	if (nextPos.x() == 0 && nextPos.y() == 0)
		return false;
	QPoint curPoint = GetMapCoordinate();
	if (curPoint == nextPos)
	{
		qDebug() << "AutoWalkMaze ???????????? ?????????";
		return false;
	}
	return AutoMoveTo(nextPos.x(), nextPos.y());
}
//?????????????????? ???????????????????????????????????? ???????????????
bool CGFunction::AutoWalkRandomMaze()
{
	return AutoWalkRandomMazeEx();
	QStringList sFilterNameList = m_sTargetMazeName.split("|");
	bool bRet = false;
	QList<QPoint> enterPosList;								  //?????????
	int beginFloor = g_pGameFun->GetMapFloorNumberFromName(); //????????????
	int lastFloor = beginFloor;
	while (!m_bStop && !g_pGameCtrl->GetExitGame())
	{
		QString curMapName = GetMapName();
		int curMapNum = GetMapIndex();
		if (sFilterNameList.size() > 0) //????????????
		{
			if (sFilterNameList.contains(curMapName) || sFilterNameList.contains(QString::number(curMapNum)))
			{
				qDebug() << "???????????????" << curMapName << m_sTargetMazeName;
				return true;
			}
		}
		else //????????????
		{
			if (!m_sTargetMazeName.isEmpty() && (curMapName.contains(m_sTargetMazeName) || curMapNum == m_sTargetMazeName.toInt()))
			{
				qDebug() << "???????????????" << curMapName << m_sTargetMazeName;
				return true;
			}
		}
		//??????????????????
		int curFloor = g_pGameFun->GetMapFloorNumberFromName(); //????????????
		if (curFloor != 0)										//???????????? ???????????????
		{
			if (curFloor < lastFloor) //?????????????????? ?????????????????????
			{						  //??????????????? ?????? ????????????
				qDebug() << "???????????????????????????????????????????????????????????????";
				auto curPos = g_pGameFun->GetMapCoordinate();
				auto tmpPos = g_pGameFun->GetRandomSpace(curPos.x(), curPos.y(), 1);
				g_pGameFun->AutoMoveTo(tmpPos.x(), tmpPos.y());
				g_pGameFun->AutoMoveTo(curPos.x(), curPos.y());
			}
			WaitInNormalState();
			curFloor = g_pGameFun->GetMapFloorNumberFromName();
			if (curFloor != 0)
				lastFloor = curFloor;
		}

		//		if (!IsMapDownload())
		{
			DownloadMap();
			Sleep(2000); //??????2???
		}
		QPoint curPos = GetMapCoordinate(); //?????????

		auto tmpMazeTgtList = GetMazeEntranceList();
		qSort(tmpMazeTgtList.begin(), tmpMazeTgtList.end(), [&](auto a, auto b)
				{
					double aDistance = AStarUtil::octile(abs(a.x() - curPos.x()), abs(a.y() - curPos.y()));
					double bDistance = AStarUtil::octile(abs(b.x() - curPos.x()), abs(b.y() - curPos.y()));
					//qDebug() << aDistance << bDistance;
					return aDistance > bDistance;
				});
		//????????????????????? ?????????
		if (tmpMazeTgtList.size() > 2)
		{
			qDebug() << "??????????????????????????????";
			return false;
		}
		if (tmpMazeTgtList.size() < 2)
		{
			qDebug() << "AutoWalkMaze ????????????????????? ?????????";
			return false;
		}
		QPoint nextPos = tmpMazeTgtList[0]; //???????????????
		if (enterPosList.contains(nextPos)) //??????????????? ?????????????????????
		{
			nextPos = tmpMazeTgtList[1];
		}
		//??????????????? ?????????????????????????????????????????????????????? ????????? ???????????? ??????????????????????????????
		if (tmpMazeTgtList.contains(curPos) && !enterPosList.contains(curPos))
		{
			enterPosList.push_back(curPos);
		}
		if (nextPos.x() == 0 && nextPos.y() == 0)
		{
			qDebug() << "AutoWalkMaze ????????????????????? ?????????";
			return false;
		}
		if (curPos == nextPos)
		{
			qDebug() << "AutoWalkMaze ??????????????????????????????????????????????????????";
			return false;
		}
		if (!AutoMoveTo(nextPos.x(), nextPos.y()))
		{
			qDebug() << "AutoWalkMaze ????????????????????????";
			return false;
		}
	}

	return true;
}

bool CGFunction::AutoWalkRandomMazeEx()
{
	QStringList sFilterNameList = m_sTargetMazeName.split("|");
	bool bRet = false;
	QList<QPoint> enterPosList;								  //?????????
	int beginFloor = g_pGameFun->GetMapFloorNumberFromName(); //????????????
	int lastFloor = beginFloor;
	while (!m_bStop && !g_pGameCtrl->GetExitGame())
	{
		QString curMapName = GetMapName();
		int curMapNum = GetMapIndex();
		if (IsInRandomMap() == false) //???????????? ??????
		{
			qDebug() << "???????????????????????????????????????????????????";
			return false;
		}
		if (sFilterNameList.size() > 0) //????????????
		{
			if (sFilterNameList.contains(curMapName) || sFilterNameList.contains(QString::number(curMapNum)))
			{
				qDebug() << "???????????????" << curMapName << m_sTargetMazeName;
				return true;
			}
		}
		else //????????????
		{
			if (!m_sTargetMazeName.isEmpty() && (curMapName.contains(m_sTargetMazeName) || curMapNum == m_sTargetMazeName.toInt()))
			{
				qDebug() << "???????????????" << curMapName << m_sTargetMazeName;
				return true;
			}
		}
		//??????????????????
		int curFloor = g_pGameFun->GetMapFloorNumberFromName(); //????????????
		if (curFloor != 0)										//???????????? ???????????????
		{
			if (curFloor < lastFloor) //?????????????????? ?????????????????????
			{						  //??????????????? ?????? ????????????
				qDebug() << "???????????????????????????????????????????????????????????????";
				auto curPos = g_pGameFun->GetMapCoordinate();
				auto tmpPos = g_pGameFun->GetRandomSpace(curPos.x(), curPos.y(), 1);
				g_pGameFun->AutoMoveTo(tmpPos.x(), tmpPos.y());
				g_pGameFun->AutoMoveTo(curPos.x(), curPos.y());
			}
			WaitInNormalState();
			curFloor = g_pGameFun->GetMapFloorNumberFromName();
			if (curFloor != 0)
				lastFloor = curFloor;
		}

		//		if (!IsMapDownload())
		{
			MakeMapOpenContainNextEntrance(1);
			Sleep(2000); //??????2???
		}
	}

	return true;
}

//???????????????????????????????????????
QPoint CGFunction::GetMazeEntrance(bool bNearFar)
{
	QPoint current = GetMapCoordinate();
	auto enteranceList = GetMazeEntranceList();
	qSort(enteranceList.begin(), enteranceList.end(), [&](auto a, auto b)
			{
				double aDistance = AStarUtil::octile(abs(a.x() - current.x()), abs(a.y() - current.y()));
				double bDistance = AStarUtil::octile(abs(b.x() - current.x()), abs(b.y() - current.y()));
				qDebug() << aDistance << bDistance;
				return aDistance > bDistance;
			});
	//????????????????????? ?????????
	if (enteranceList.size() > 2)
		return QPoint();
	if (enteranceList.size() > 0)
	{
		if (bNearFar) //?????? ????????????
			return enteranceList[0];
		else if (enteranceList.size() == 1) //??????1??? ????????????
			return enteranceList[0];
		else if (enteranceList.size() > 1)
			return enteranceList[1];
	}
	return QPoint();
}

QList<QPoint> CGFunction::GetMazeEntranceList()
{
	QList<QPoint> enteranceList;
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		CGA::cga_map_cells_t objCells;
		if (g_CGAInterface->GetMapObjectTable(true, objCells))
		{
			if (cells.x_size && cells.y_size)
			{
				for (int y = 0; y < cells.y_size; ++y)
				{
					for (int x = 0; x < cells.x_size; ++x)
					{
						if (x < objCells.x_size && y < objCells.y_size)
						{
							auto cellObject = objCells.cell.at((size_t)(x + y * objCells.x_size));
							if (cellObject & 0xff)
							{
								if ((cellObject & 0xff) == 3)
								{
									enteranceList.append(QPoint(x, y)); //?????? ???????????????
								}
								else
								{
									//	enteranceList.append(QPoint(x, y));
									//?????? ??????????????? ?????????
								}
							}
						}
					}
				}
			}
		}
	}
	qDebug() << enteranceList;
	if (enteranceList.size() > 2)
	{
		qDebug() << "??????????????????????????????";
	}
	return enteranceList;
}

QList<QPoint> CGFunction::GetMazeEntranceListFromOfflineMap(QImage mapImage)
{
	QList<QPoint> warpList;
	if (mapImage.width() <= 0 || mapImage.height() <= 0)
		return warpList;

	int w = mapImage.width();
	int h = mapImage.height();
	std::vector<uchar> mapData; //????????????

	QColor qRgb(255, 0, 0);
	for (int tmpx = 0; tmpx < h; ++tmpx) //???
	{
		for (int tmpy = 0; tmpy < w; ++tmpy) //???
		{
			if (mapImage.pixelColor(tmpy, tmpx) == qRgb) //???????????? 1
			{
				warpList.append(QPoint(tmpy, tmpx));
			}
		}
	}
	return warpList;
}

QList<QPoint> CGFunction::GetMapEntranceList()
{
	QList<QPoint> enteranceList;
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		CGA::cga_map_cells_t objCells;
		if (g_CGAInterface->GetMapObjectTable(true, objCells))
		{
			if (cells.x_size && cells.y_size)
			{
				for (int y = 0; y < cells.y_size; ++y)
				{
					for (int x = 0; x < cells.x_size; ++x)
					{
						if (x < objCells.x_size && y < objCells.y_size)
						{
							auto cellObject = objCells.cell.at((size_t)(x + y * objCells.x_size));
							if (cellObject & 0xff)
							{
								enteranceList.append(QPoint(x, y)); //?????? ???????????????
							}
						}
					}
				}
			}
		}
	}
	qDebug() << enteranceList;
	if (enteranceList.size() > 2)
	{
		qDebug() << "????????????????????????";
	}
	return enteranceList;
}

//???????????????????????????x???y????????????????????????5???, 2??????????????????npc??????????????????????????????????????????
bool CGFunction::ForceMoveTo(int x, int y, bool bShow)
{
	int curX, curY;
	g_CGAInterface->GetMapXY(curX, curY);
	if (abs(curX - x) > 5 || abs(curY - y) > 5)
	{
		qDebug() << "forceMoveTo???x??????y?????????????????????5???";
		return false;
	}
	bool bResult = false;
	g_CGAInterface->ForceMoveTo(x, y, bShow, bResult);
	int curMapIndex = GetMapIndex();
	QString curMapName = GetMapName();
	DWORD dwLastTime = GetTickCount();
	bool isNormal = true;
	while (!m_bStop)
	{
		if (g_pGameCtrl->GetExitGame() || m_bStop)
			break;

		//1????????????????????????
		while (!IsInNormalState() && !m_bStop) //?????????????????? ????????????
		{
			isNormal = false;
			Sleep(1000);
		}
		//2????????????????????????????????? ???????????????????????????????????????????????????
		if (curMapIndex != GetMapIndex() || curMapName != GetMapName())
		{
			m_bMoveing = false;
			qDebug() << "????????????????????????????????????";
			return false;
		}
		//???????????? ??????????????????????????????????????????????????????
		if (!isNormal) //???????????????????????? ?????????????????????????????????????????????
		{
			qDebug() << "??????/??????????????????????????????";
			g_CGAInterface->ForceMoveTo(x, y, true, bResult);
			isNormal = true;
		}
		//3??????????????????????????????
		g_CGAInterface->GetMapXY(curX, curY);
		if (curX == x && curY == y)
		{
			break;
		}
		//4???????????????????????????????????????????????? ????????? ?????????????????????????????????????????????????????????
		if (GetTickCount() - dwLastTime > 3000) //3???????????????
		{
			break; //3???????????? ?????? 5????????? 3?????????
		}

		//QApplication::processEvents();
		Sleep(500);
	}
	return true;
}
int CGFunction::TransDirectionToCga(int nDir)
{
	switch (nDir)
	{
		case 0: return 6;
		case 1: return 7;
		case 2: return 0;
		case 3: return 1;
		case 4: return 2;
		case 5: return 3;
		case 6: return 4;
		case 7: return 5;
		default:
			break;
	}
	return nDir;
}
bool CGFunction::ForceMoveToEx(int nDirection, int nVal, bool bShow)
{
	int nDir = TransDirectionToCga(nDirection);
	qDebug() << nDirection << "Trans:" << nDir;
	bool bResult = false;
	while (nVal > 0)
	{
		g_CGAInterface->ForceMove(nDir, bShow, bResult);
		Sleep(500);
		g_pGameFun->WaitInNormalState();
		nVal--;
	}
	return true;
}

void CGFunction::AutoEncounterEnemyThread(CGFunction *pThis)
{
	qDebug() << "??????????????????";

	if (!pThis)
		return;
	bool bResult = false;
	QPoint startPoint = pThis->GetMapCoordinate();
	QString preMapName = pThis->GetMapName();
	int nDir = pThis->m_nAutoEncounterDir;
	QPoint targetPos = pThis->GetDirectionPos(nDir, 1);
	int nTgtCheck = pThis->IsTargetExistWall(targetPos.x(), targetPos.y());
	if (nTgtCheck == -1)
	{
		qDebug() << "?????????????????????????????????????????????????????????";
		nDir = TEncounter_Origin_Random;
	}
	else if (nTgtCheck == 1)
	{
		qDebug() << "??????????????????????????????????????????????????????";
		nDir = TEncounter_Origin_Random;
	}
	if (nDir == TEncounter_Origin_Random)
	{
		targetPos = pThis->GetRandomSpace(startPoint.x(), startPoint.y());
	}
	g_CGAInterface->WalkTo(targetPos.x(),targetPos.y());
	Sleep(1000);
	g_CGAInterface->WalkTo(startPoint.x(), startPoint.y());
	g_CGAInterface->SetGameTextUICurrentScript("????????????:???");
	QTime recordTime;
	recordTime.restart();
	emit pThis->signal_startAutoEncounterEnemySucess();
	bool bMoveNext = false;
	while (pThis->m_bAutoEncounterEnemy && !g_pGameCtrl->GetExitGame())
	{
		//		if (pThis->IsInNormalState())	//?????????????????????ForceMoveTo?????????????????????
		{
			if (pThis->IsNeedStopEncounter()) //?????????????????? ???????????????????????? ????????????????????????????????????????????????
			{
				pThis->updateEndAutoAction();
				emit pThis->signal_stopAutoEncounterEnemy();
				return;
			}
			if (recordTime.elapsed() > 5000)
			{
				recordTime.restart();
				if (preMapName != pThis->GetMapName()) //???????????? ??????
				{
					pThis->updateEndAutoAction();
					emit pThis->signal_stopAutoEncounterEnemy();
					return;
				}
			}		
			//	qDebug() << "??????" << pThis->m_nAutoEncounterDir <<"??????" << pThis->m_nAutoEncounterEnemyInterval << nDir;
			//QPoint curPoint = pThis->GetMapCoordinate();
			//if (curPoint.x() == startPoint.x() && curPoint.y() == startPoint.y())
			//{
			//	g_CGAInterface->ForceMoveTo(targetPos.x(), targetPos.y(), pThis->m_bIsShowAutoEncounterEnemy, bResult);
			//	/*if (pThis->m_bIsShowAutoEncounterEnemy)
			//		g_CGAInterface->WalkTo(targetPos.x(), targetPos.y());
			//	else
			//		g_CGAInterface->ForceMoveTo(targetPos.x(), targetPos.y(), false, bResult);*/
			//}
			//else
			//{
			//	g_CGAInterface->ForceMoveTo(startPoint.x(), startPoint.y(), pThis->m_bIsShowAutoEncounterEnemy, bResult);
			//	/*		if (pThis->m_bIsShowAutoEncounterEnemy)
			//		g_CGAInterface->WalkTo(startPoint.x(), startPoint.y());
			//	else
			//		g_CGAInterface->ForceMoveTo(startPoint.x(), startPoint.y(), false, bResult);*/
			//}
			if (bMoveNext)
				g_CGAInterface->ForceMoveTo(targetPos.x(), targetPos.y(), pThis->m_bIsShowAutoEncounterEnemy, bResult);
			else
				g_CGAInterface->ForceMoveTo(startPoint.x(), startPoint.y(), pThis->m_bIsShowAutoEncounterEnemy, bResult);

		}
		//?????????????????????  ????????? ???nDir???????????????
		Sleep(pThis->m_nAutoEncounterEnemyInterval);
		bMoveNext = !bMoveNext;
	}
	pThis->updateEndAutoAction();
	//??????????????????????????????????????????????????? ???????????????
	if (preMapName != pThis->GetMapName()) //???????????? ??????
	{
		emit pThis->signal_stopAutoEncounterEnemy();
		qDebug() << "?????????????????? ????????????";

		return;
	}
	QPoint curPoint = pThis->GetMapCoordinate();
	if (curPoint.x() == startPoint.x() && curPoint.y() == startPoint.y())
	{
		emit pThis->signal_stopAutoEncounterEnemy();
		qDebug() << "?????????????????? ???????????? ??????";
		return;
	}
	else //????????????
	{
		//pThis->WaitInNormalState(20 * 1000); //20???
		if (pThis->IsInNormalState())
		{
			if (pThis->m_bIsShowAutoEncounterEnemy)
				g_CGAInterface->WalkTo(startPoint.x(), startPoint.y());
			else
				g_CGAInterface->ForceMoveTo(startPoint.x(), startPoint.y(), false, bResult);
		}
	}
	emit pThis->signal_stopAutoEncounterEnemy();
	qDebug() << "??????????????????";
}

void CGFunction::begin_auto_action()
{
	qDebug() << "begin_auto_action";
	g_pGameFun->m_bAutoEncounterEnemy = true;
	emit g_pGameFun->signal_StartAutoEncounterEnemy();
	// 	QtConcurrent::run(AutoEncounterEnemyThread, this);
}

void CGFunction::end_auto_action()
{
	qDebug() << "end_auto_action";
	g_pGameFun->m_bAutoEncounterEnemy = false;
	while (m_encounterFuture.isRunning())
	{
		g_pGameFun->m_bAutoEncounterEnemy = false;
		qDebug() << "?????????????????? SetFalse";
		Sleep(1000);
	}
}

void CGFunction::updateEndAutoAction()
{
	g_pGameFun->m_bAutoEncounterEnemy = false;
	g_CGAInterface->SetGameTextUICurrentScript("????????????:???");
}

void CGFunction::waitEndAutoEncounterAction()
{
	qDebug() << "waitEndAutoEncounterAction";

	if (m_encounterFuture.isRunning())
	{
		qDebug() << "waitEndAutoEncounterAction isRunning";
		m_encounterFuture.waitForFinished();
	}
	qDebug() << "waitEndAutoEncounterAction return";
}

//????????????????????????????????????????????????????????????bug
A_FIND_PATH CGFunction::CalculatePath(int curX, int curY, int targetX, int targetY)
{
	A_FIND_PATH findPath;
	//1???????????????
	//if (!IsMapDownload())
	//{
	//	if (!DownloadMap()) //?????????????????????  ???????????? ?????????????????????????????????????????????
	//	{
	//		//return findPath;
	//	}
	//}
	//2???A*???????????? ??????????????????
	try
	{
		CGA::cga_map_cells_t cells;
		if (g_CGAInterface->GetMapCollisionTable(true, cells))
		{
			CGA::cga_map_cells_t objCells;
			g_CGAInterface->GetMapObjectTable(true, objCells);
			AStar aStarFindPath(true, true);
			//x??????  y??????
			AStarGrid grid(cells.x_size, cells.y_size);
			int xtop = cells.y_size;
			int ytop = cells.x_size;
			QString szDebug;
			for (int tmpx = 0; tmpx < xtop; ++tmpx) //???
			{
				szDebug.clear();
				for (int tmpy = 0; tmpy < ytop; ++tmpy) //???
				{
					auto cellWall = cells.cell.at((size_t)(tmpy + tmpx * cells.x_size)); //?????????????????? ??????
					auto cellObject = objCells.cell.at((size_t)(tmpy + tmpx * cells.x_size));
					if (cellWall == 1) //???????????? 1
					{
						grid.SetWalkableAt(tmpy, tmpx, false); //?????? ?????????
						szDebug += QString("%1 ").arg(0);
					}
					else
					{
						grid.SetWalkableAt(tmpy, tmpx, true);
						szDebug += QString("%1 ").arg(1);
						if (cellObject & 0xff) //?????????????????????????????? ?????????????????????????????? ??????
						{
							if (tmpy != targetX || tmpx != targetY)
							{
								//	qDebug() << tmpx << tmpy << (cellObject & 0xff) << targetX << targetY;
								grid.SetWalkableAt(tmpy, tmpx, false); //?????? ?????????
								szDebug += QString("%1 ").arg(0);
							}
						}
					}
				}
				//qDebug() << szDebug;
			}
			QPoint frompos(curX - cells.x_bottom, curY - cells.y_bottom);
			QPoint topos(targetX - cells.x_bottom, targetY - cells.y_bottom);
			auto path = aStarFindPath.FindPath(frompos.x(), frompos.y(), topos.x(), topos.y(), &grid);
			//qDebug() <<  path.size()<<path << "frompos" << frompos << "targetpos" << topos;
			findPath = AStarUtil::compressPath(path);
		}
		return findPath;
	}
	catch (const std::exception &e)
	{
		qDebug() << "Try catch CalculatePathEx Ero" << e.what();
	}
	catch (...)
	{
		qDebug() << "Try catch CalculatePathEx Ero";
	}
	return findPath;
}

A_FIND_PATH CGFunction::CalculatePathEx(QImage &mapImage, int sx, int sy, int tx, int ty)
{
	int w = mapImage.width();
	int h = mapImage.height();
	std::vector<uchar> mapData; //????????????
	std::vector<std::pair<int, int> > findPath;
	try
	{
		QColor qRgb(0, 0, 0);
		AStar aStarFindPath(true, true);
		//x??????  y??????
		AStarGrid grid(w, h);
		int xtop = h;
		int ytop = w;
		//QString szDebug;
		for (int tmpx = 0; tmpx < xtop; ++tmpx) //???
		{
			//szDebug.clear();
			for (int tmpy = 0; tmpy < ytop; ++tmpy) //???
			{
				if (mapImage.pixelColor(tmpy, tmpx) == qRgb) //???????????? 1
				{
					grid.SetWalkableAt(tmpy, tmpx, false); //?????? ?????????
														   //	szDebug += QString("%1 ").arg(0);
				}
				else
				{
					grid.SetWalkableAt(tmpy, tmpx, true);
					//	szDebug += QString("%1 ").arg(1);
				}
			}
			//		qDebug() << szDebug;
		}
		QPoint frompos(sx, sy);
		QPoint topos(tx, ty);
		auto path = aStarFindPath.FindPath(frompos.x(), frompos.y(), topos.x(), topos.y(), &grid);
		//??????????????????
		std::vector<std::pair<int, int> > findPath = AStarUtil::compressPath(path);
		return findPath;
	}
	catch (const std::exception &e)
	{
		qDebug() << "Try catch CalculatePathEx Ero" << e.what();
	}
	catch (...)
	{
		qDebug() << "Try catch CalculatePathEx Ero";
	}
	return findPath;
}

A_FIND_PATH CGFunction::CalculatePathEx2(QImage &mapImage, int sx, int sy, int tx, int ty)
{
	int w = mapImage.width();
	int h = mapImage.height();
	std::vector<uchar> mapData; //????????????

	QColor qRgb(0, 0, 0);
	for (int i = 0; i < h; ++i)
	{
		for (size_t n = 0; n < w; n++)
		{
			if (mapImage.pixelColor(n, i) == qRgb)
				mapData.push_back(0);
			else
				mapData.push_back(1);
		}
	}
	AStar aStarFindPath(true, true);
	//x??????  y??????
	AStarGrid grid(w, h);
	int xtop = h;
	int ytop = w;
	//QString szDebug;
	for (int tmpx = 0; tmpx < xtop; ++tmpx) //???
	{
		//szDebug.clear();
		for (int tmpy = 0; tmpy < ytop; ++tmpy) //???
		{
			auto cellWall = mapData.at((size_t)(tmpy + tmpx * w)); //?????????????????? ??????
			if (cellWall == 0)									   //???????????? 1
			{
				grid.SetWalkableAt(tmpy, tmpx, false); //?????? ?????????
													   //	szDebug += QString("%1 ").arg(0);
			}
			else
			{
				grid.SetWalkableAt(tmpy, tmpx, true);
				//	szDebug += QString("%1 ").arg(1);
			}
		}
		//		qDebug() << szDebug;
	}
	QPoint frompos(sx, sy);
	QPoint topos(tx, ty);
	auto path = aStarFindPath.FindPath(frompos.x(), frompos.y(), topos.x(), topos.y(), &grid);
	//??????????????????
	std::vector<std::pair<int, int> > findPath = AStarUtil::compressPath(path);
	return findPath;
}

bool CGFunction::IsMapDownload()
{
	QList<int> excludedMaps({ 1000, 27001, 61001, 43600, 43000, 15592, 15593, 15594, 15595, 15596, 11032, 11034, 11035, 11036, 11037, 15000, 15001, 15002, 15003, 15004, 15005, 15006, 14000, 14001, 14002, 14014, 4400, 5008, 11000, 11001, 11002, 11003, 11004, 11005, 59501, 2400 });
	int mapindex = GetMapIndex();
	if (excludedMaps.contains(mapindex))
	{
		return true;
	}
	//????????????????????????????????????????????????0????????????????????????
	CGA::cga_map_cells_s mapCells;
	g_CGAInterface->GetMapTileTable(true, mapCells);
	//????????????
	for (int y = 0; y < mapCells.y_size; ++y)
	{
		for (int x = 0; x < mapCells.x_size; ++x)
		{

			if (mapCells.cell[x + y * mapCells.x_size] == 0)
				return false;
		}
	}
	return true;
}

bool CGFunction::DownloadMap()
{
	CGA::cga_map_cells_s mapCells;
	g_CGAInterface->GetMapTileTable(true, mapCells);
	return DownloadMapEx(0, 0, mapCells.x_size, mapCells.y_size);
}

bool CGFunction::DownloadMapEx(int xfrom, int yfrom, int xsize, int ysize)
{
	////?????????????????? ?????????????????????????????????????????????
	//if (g_pGameCtrl->GetIsOpenNetToMLAssistTool())//?????? ???????????????
	//{
	//	//???????????????????????????????????? ??????????????????2???????????? ????????????????????????
	//	if( GetMazeEntranceList().size() < 2)
	//	{
	//
	//	}
	//}
	MakeMapOpen();
	return false;
	int last_index3 = GetMapIndex();
	int x = xfrom, y = yfrom;
	DWORD dwWaitTime = GetTickCount();
	while (1)
	{
		g_CGAInterface->RequestDownloadMap(x, y, x + 24, y + 24);
		x += 24;
		if (x > xsize)
		{
			y += 24;
			x = xfrom;
		}
		if (y - ysize >= 24)
		{
			auto mapRegion = WaitRefreshMapRegion();
			if (mapRegion.size() < 1 || mapRegion.size() != 5)
			{
				qDebug() << "?????????????????????????????????";
				return false;
			}
			if (mapRegion.at(4) != last_index3)
			{
				qDebug() << "?????????????????????????????????";
				return false;
			}
			if ((mapRegion.at(2) >= xsize && mapRegion.at(3) >= ysize) || (mapRegion.at(0) == 0 && mapRegion.at(1) == 0))
			{
				WaitRefreshMapRegion(500);
			}
			else
			{
				WaitRefreshMapRegion(3000);
			}
			return true;
		}
		//if ((GetTickCount() - dwWaitTime) > 30 * 1000) //30?????????????????????  ??????
		//{
		//	qDebug() << "??????????????????30??????????????????";
		//	return false;
		//}
	}
	qDebug() << "??????????????????";
	return true;
}

void CGFunction::JianDongXi(int nDirection)
{
}

void CGFunction::ThrowItemID(int id)
{
	GameItemList pItemList = GetGameItems();
	for (size_t i = 0; i < pItemList.size(); i++)
	{
		GameItemPtr pItem = pItemList.at(i);
		if (pItem && pItem->exist && pItem->id == id)
		{
			bool result = false;
			if (g_CGAInterface->DropItem(pItem->pos, result) && result)
				continue;
		}
	}
}

void CGFunction::ThrowItemName(const QString &name)
{
	GameItemList pItemList = GetGameItems();
	for (size_t i = 0; i < pItemList.size(); i++)
	{
		GameItemPtr pItem = pItemList.at(i);
		if (pItem && pItem->exist && (pItem->name == name || pItem->id == name.toInt()))
		{
			bool result = false;
			if (g_CGAInterface->DropItem(pItem->pos, result) && result)
				continue;
		}
	}
}

void CGFunction::ThrowItemPos(int pos)
{
	bool result = false;
	g_CGAInterface->DropItem(pos, result);
}

void CGFunction::ThrowNotFullItemName(const QString &name, int nCount)
{
	GameItemList pItemList = GetGameItems();
	for (size_t i = 0; i < pItemList.size(); i++)
	{
		GameItemPtr pItem = pItemList.at(i);
		if (pItem && pItem->exist && pItem->name == name && pItem->count < nCount)
		{
			bool result = false;
			if (g_CGAInterface->DropItem(pItem->pos, result) && result)
				continue;
		}
	}
}

void CGFunction::PileItem(QString name, int count)
{
	GameItemList pItemList = GetGameItems();
	for (size_t i = 8; i < pItemList.size(); i++)
	{
		GameItemPtr pItem = pItemList.at(i);
		if (pItem && pItem->exist && pItem->name == name && pItem->count < count)
		{
			for (size_t n = 0; n < pItemList.size(); n++)
			{
				GameItemPtr pOtherItem = pItemList.at(n);
				if (pOtherItem && pOtherItem->exist && pOtherItem->name == pItem->name && pOtherItem != pItem && pOtherItem->count < count)
				{
					bool result = false;
					if (g_CGAInterface->MoveItem(pItem->pos, pOtherItem->pos, -1, result) && result)
						break;
				}
			}
		}
	}
}

bool CGFunction::NeedHPSupply(CGA::cga_player_info_t &pl)
{
	return (pl.hp < pl.maxhp) ? true : false;
}

bool CGFunction::NeedMPSupply(CGA::cga_player_info_t &pl)
{
	return (pl.mp < pl.maxmp) ? true : false;
}

bool CGFunction::NeedPetSupply(CGA::cga_pets_info_t &pets)
{
	for (size_t i = 0; i < pets.size(); ++i)
	{
		if (pets.at(i).hp < pets.at(i).maxhp || pets.at(i).mp < pets.at(i).maxmp)
			return true;
	}
	return false;
}

bool CGFunction::WaitSupplyFini(int timeout)
{
	for (int i = 0; i < timeout; ++i)
	{
		if (m_bStop)
			return false;
		CGA::cga_player_info_t playerinfo;
		CGA::cga_pets_info_t petsinfo;
		if (g_CGAInterface->GetPlayerInfo(playerinfo))
		{
			bool bNeedHP = g_pGameFun->NeedHPSupply(playerinfo);
			bool bNeedMP = g_pGameFun->NeedMPSupply(playerinfo);
			g_CGAInterface->GetPetsInfo(petsinfo);
			bool bNeedPet = g_pGameFun->NeedPetSupply(petsinfo);
			if (bNeedHP && bNeedMP && bNeedPet)
			{
				return true;
			}
		}
		//QApplication::processEvents();
		Sleep(1000);
	}
	return true;
}

QList<int> CGFunction::WaitRefreshMapRegion(int timeout /*= 5000*/)
{
	QMutex mutex;
	QList<int> mapInfo;
	QEventLoop loop;
	//qDebug() << "WaitRecvHead Work Start";
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyRefreshMapRegion, [&](int xbase, int ybase, int xtop, int ytop, int index3)
			{
				QMutexLocker locker(&mutex);
				//		qDebug() << "WaitRefreshMapRegion dlg" ;
				mapInfo << xbase << ybase << xtop << ytop << index3;
				if (loop.isRunning())
					loop.quit();
			});
	loop.exec();
	QMutexLocker locker(&mutex);
	//qDebug() << "WaitRefreshMapRegion  End";
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return mapInfo;
}

bool CGFunction::NeedSale()
{
	return true;
}

QString CGFunction::GetJustSysChatMsg()
{
	if (m_systemCueList.size() < 1)
		return QString();
	QMutexLocker locker(&m_charMutex);
	auto lastData = m_systemCueList.last();
	if ((GetTickCount() - lastData.first) > 3000)
		return QString();
	else
		return lastData.second;
	return m_systemCueList.last().second;
}

bool CGFunction::SetCharacterSwitch(int nType, bool bState)
{
	bool bRet = false;
	//	g_CGAInterface->DoRequest(,bRet);
	g_CGAInterface->EnableFlags(nType, bState, bRet);
	return bRet;
}

bool CGFunction::SetCharacterSwitchEx(QString type, bool bState)
{
	int characterData = m_characterMap.value(type, -1);
	if (characterData != -1)
	{
		return SetCharacterSwitch(characterData, bState);
	}
	return false;
}

//CHARACTER_STATE
int CGFunction::GetCharacterSwitch(int nType)
{
	bool bRet = false;
	//	g_CGAInterface->DoRequest(,bRet);
	return bRet;
}
//TCharacter_Action
bool CGFunction::DoCharacterAction(int nType)
{
	bool bRet = false;
	g_CGAInterface->DoRequest(nType, bRet);
	return bRet;
}

bool CGFunction::SysConfig(QVariant type, QVariant data1, QVariant data2)
{
	bool bRet = false;
	switch (type.type())
	{
		case QVariant::Int:
		{

			break;
		}
		case QVariant::String:
		{
			QString sType = type.toString();
			int nAutoType = m_sysConfigMap.value(sType);
			switch (nAutoType)
			{
				case TSysConfigSet_Timer:
				{
					g_pGameCtrl->OnSetScirptDelayTime(data1.toInt());
					break;
				}
				case TSysConfigSet_AutoDrop:
				{
					SetDropItemState(data1.toInt(), data2.toString());
					break;
				}
				case TSysConfigSet_AutoPile:
				{
					SetPileItemState(data1.toInt(), data2.toString());
					break;
				}
				case TSysConfigSet_FollowPickItem:
				{
					emit g_pGameCtrl->signal_switchFoolowPickItemUI(data1.toBool());
					break;
				}
				case TSysConfigSet_AutoPick:
				{
					if (data2.isNull())
						emit g_pGameCtrl->signal_switchAutoPickItemUI(data1.toBool());
					else
						emit g_pGameCtrl->signal_addPickItemUI(data1.toBool(), data2.toString());
					break;
				}
				case TSysConfigSet_AutoSale:
				{
					if (data2.isNull())
						emit g_pGameCtrl->signal_switchAutoSaleUI(data1.toBool());
					else
						emit g_pGameCtrl->signal_addSaleItemUI(data1.toBool(), data2.toString());
					break;
				}
				case TSysConfigSet_AutoSupply:
				{
					emit g_pGameCtrl->signal_switchAutoSupplyUI(data1.toBool());
					break;
				}
				case TSysConfigSet_AutoEncounter:
				{
					if (data1.toBool())
						g_pGameFun->begin_auto_action();
					else
						g_pGameFun->end_auto_action();
					break;
				}
				case TSysConfigSet_EncounterType:
				{
					m_nAutoEncounterDir = data1.toInt();
					break;
				}
				case TSysConfigSet_EncounterSpeed:
				{
					m_nAutoEncounterEnemyInterval = data1.toInt();
					emit g_pGameCtrl->signal_switchEncounterSpeedUI(m_nAutoEncounterEnemyInterval);
					break;
				}
				case TSysConfigSet_AutoBattle:
				{
					emit g_pGameCtrl->signal_switchAutoBattleUI(data1.toBool());
					break;
				}
				case TSysConfigSet_HighSpeedBattle:
				{
					emit g_pGameCtrl->signal_switchAutoHightSpeedBattleUI(data1.toBool());
					break;
				}
				case TSysConfigSet_HighSpeedDelay:
				{
					emit g_pGameCtrl->signal_setHightSpeedBattleDelayUI(data1.toInt());
					break;
				}
				case TSysConfigSet_BattleDelay:
				{
					emit g_pGameCtrl->signal_setBattleDelayUI(data1.toInt());
					break;
				}
				case TSysConfigSet_AllEncounterEscape:
				{
					emit g_pGameCtrl->signal_switchAllEncounterEscapeUI(data1.toBool());
					break;
				}
				case TSysConfigSet_NoLv1Escape:
				{
					emit g_pGameCtrl->signal_switchNoLvlEncounterEscapeUI(data1.toBool());
					break;
				}
				case TSysConfigSet_EnemyAvgLvEscape:
				{
					emit g_pGameCtrl->signal_switchEscapeUI(nAutoType, data1.toBool(), data2.toString());
					break;
				}
				case TSysConfigSet_EnemyCountEscape:
				{
					emit g_pGameCtrl->signal_switchEscapeUI(nAutoType, data1.toBool(), data2.toString());
					break;
				}
				case TSysConfigSet_TeamCountEscape:
				{
					emit g_pGameCtrl->signal_switchEscapeUI(nAutoType, data1.toBool(), data2.toString());
					break;
				}
				case TSysConfigSet_NoPetDoubleAction:
				{
					emit g_pGameCtrl->signal_switchNoPetDoubleActionUI(data1.toBool());
					break;
				}
				case TSysConfigSet_NoPetAttack:
				{
					emit g_pGameCtrl->signal_switchNoPetActionUI(1, data1.toBool());
					break;
				}
				case TSysConfigSet_NoPetGuard:
				{
					emit g_pGameCtrl->signal_switchNoPetActionUI(0, data1.toBool());
					break;
				}
				case TSysConfigSet_AutoEatDeepBlue:
				case TSysConfigSet_AutoEatDogFood:
				case TSysConfigSet_AutoEatTimeCrystal:
				{
					emit g_pGameCtrl->signal_switchAutoEatUi(nAutoType, data1.toBool());
					break;
				}
				case TSysConfigSet_ScriptStillRestart:
				{
					emit g_pGameCtrl->signal_setScriptStillRestartUI(data1.toInt(), data2.toInt());
					break;
				}
				case TSysConfigSet_ScriptStopRestart:
				{
					emit g_pGameCtrl->signal_setScriptStopRestartUI(data1.toInt(), data2.toInt());
					break;
				}
				case TSysConfigSet_ScriptStopLogbackRestart:
				{
					emit g_pGameCtrl->signal_setScriptStopLogbackRestartUI(data1.toInt());
					break;
				}
				case TSysConfigSet_ScriptStillLogout:
				{
					emit g_pGameCtrl->signal_setScriptStillLogoutUI(data1.toInt(), data2.toInt());
					break;
				}
				case TSysConfigSet_PlayerTitle:
				{
					bool bRet = false;
					QString sTitle = data1.toString();
					int nTitleIndex = 0;
					if (data1.type() == QVariant::Int || data1.type() == QVariant::UInt)
					{
						nTitleIndex = data1.toInt();
					}
					else
					{
						CGA::cga_player_info_t playerinfo;
						g_CGAInterface->GetPlayerInfo(playerinfo);
						auto playerTitles = playerinfo.titles;
						for (int i = 0; i < playerTitles.size(); ++i)
						{
							auto title = playerTitles.at(i);
							if (m_sPrestigeMap.keys().contains(QString::fromStdString(title)))
							{
								nTitleIndex = i;
								break;
							}
						}
					}
					g_CGAInterface->ChangeTitleName(nTitleIndex, bRet);
					break;
				}

				case TSysConfigSet_CharacterSwitch:
				{
					if (data1.type() == QVariant::Int)
					{
						SetCharacterSwitch(data1.toInt(), data2.toBool());
					}
					else
					{
					}
					break;
				}
				case TSysConfigSet_MoveSpeed:
				{
					emit g_pGameCtrl->signal_setMoveSpeedUI(data1.toInt());
					break;
				}
				default:
					break;
			}
			break;
		}
		default:
			break;
	}
	return bRet;
}

QString CGFunction::getSysTimeEx()
{
	QStringList stages = { "??????", "??????", "??????", "??????" };
	CGA::cga_sys_time_t sysTime;
	g_CGAInterface->GetSysTime(sysTime);
	if (sysTime.hours < 4)
		return stages[3];
	else if (sysTime.hours <= 6)
		return stages[0];
	else if (sysTime.hours < 16)
		return stages[1];
	else if (sysTime.hours <= 18)
		return stages[2];
	else
		return stages[3];
}

QString CGFunction::getSystemTime()
{
	QStringList stages = { "??????", "??????", "??????", "??????" };
	CGA::cga_sys_time_t sysTime;
	g_CGAInterface->GetSysTime(sysTime);
	QString sDateTime = QDateTime::currentDateTime().toString("hh:mm:ss"); //QString("%1:%2:%3").arg(sysTime.hours, 2, 10, QLatin1Char('0')).arg(sysTime.mins, 2, 10, QLatin1Char('0')).arg(sysTime.secs, 2, 10, QLatin1Char('0'));
	QString timeStage;
	if (sysTime.hours < 4)
		timeStage = stages[3];
	else if (sysTime.hours <= 6)
		timeStage = stages[0];
	else if (sysTime.hours < 16)
		timeStage = stages[1];
	else if (sysTime.hours <= 18)
		timeStage = stages[2];
	else
		timeStage = stages[3];
	//?????????????????????
	return sDateTime + " " + timeStage;
}

//?????????
bool CGFunction::Sale(int x, int y, const QString &itemName)
{
	if (itemName.isEmpty())
		return false;
	//2?????? ????????????
	if (!IsNearTarget(x, y, 2))
		return false;

	QStringList sSaleItems;
	if (itemName.contains("|"))
	{
		sSaleItems = itemName.split("|");
	}
	else
		sSaleItems << itemName;
	GameItemList pItemList = GetGameItems();
	CGA::cga_sell_items_t cgaSaleItems;
	GameItemList pExistItemList;	//????????????
	foreach (auto pItem, pItemList) //??????????????????
	{
		if (pItem->exist && pItem->pos > 7 && sSaleItems.contains(pItem->name))
		{
			if (pExistItemList.contains(pItem))
				continue;
			CGA::cga_sell_item_t cgaItem;

			//	qDebug() << "SaleList" << pItem->id << pItem->count << pItem->pos;
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
				//cgaItem.count = (pItem->count < 1 ? 1 : pItem->count);
			}
			cgaItem.itemid = pItem->id;
			cgaItem.itempos = pItem->pos;
			//??????????????????????????????????????????????????????
			cgaSaleItems.push_back(cgaItem);
			pExistItemList.push_back(pItem);
		}
	}
	if (cgaSaleItems.size() < 1)
	{
		qDebug() << "CGFun ????????????????????????";
		return false;
	}
	bool bResult = false;
	TurnAboutEx(x, y);
	auto dlg = g_pGameFun->WaitRecvNpcDialog();
	if (dlg && dlg->type == 5)
	{
		g_CGAInterface->ClickNPCDialog(-1, dlg->message.at(dlg->message.size() - 1) == "3" ? 1 : 0, bResult);
	}

	dlg = g_pGameFun->WaitRecvNpcDialog();
	if (dlg && dlg->type == 7)
	{
		for (auto saleItem : cgaSaleItems)
		{
			qDebug() << "SaleItem" << saleItem.itempos << saleItem.count;
		}
		bool bCall = g_CGAInterface->SellNPCStore(cgaSaleItems, bResult);
	}

	g_pGameFun->WaitRecvNpcDialog();
	return true;
}

bool CGFunction::SaleEx(int nDir, const QString &itemName)
{
	QPoint dirPos = GetDirectionPos(nDir, 2);
	return Sale(dirPos.x(), dirPos.y(), itemName);
}

bool CGFunction::SaleEx2(QString sDir, const QString &itemName)
{
	QPoint dirPos = GetDirectionPosEx(sDir, 2);
	return Sale(dirPos.x(), dirPos.y(), itemName);
}

bool CGFunction::IdentifyItem(int x, int y, const QString &itemName)
{
	if (itemName.isEmpty())
		return false;
	//2?????? ????????????
	if (!IsNearTarget(x, y, 2))
		return false;

	QStringList sSaleItems;
	if (itemName.contains("|"))
	{
		sSaleItems = itemName.split("|");
	}
	else
		sSaleItems << itemName;
	GameItemList pItemList = GetGameItems();
	CGA::cga_sell_items_t cgaSaleItems;
	GameItemList pExistItemList;	//????????????
	foreach (auto pItem, pItemList) //??????????????????
	{
		if (pItem->exist && pItem->pos > 7 && sSaleItems.contains(pItem->name))
		{
			if (pExistItemList.contains(pItem))
				continue;
			CGA::cga_sell_item_t cgaItem;

			//	qDebug() << "SaleList" << pItem->id << pItem->count << pItem->pos;
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
				//(pItem->count < 1 ? 1 : pItem->count);
			}
			cgaItem.itemid = pItem->id;
			cgaItem.itempos = pItem->pos;
			//??????????????????????????????????????????????????????
			cgaSaleItems.push_back(cgaItem);
			pExistItemList.push_back(pItem);
		}
	}
	if (cgaSaleItems.size() < 1)
	{
		qDebug() << "CGFun ???????????????????????????";
		return false;
	}
	bool bResult = false;
	TurnAboutEx(x, y);
	auto dlg = g_pGameFun->WaitRecvNpcDialog();
	if (dlg)
	{
		for (auto saleItem : cgaSaleItems)
		{
			qDebug() << "IdentifyItem" << saleItem.itempos << saleItem.count;
		}
		bool bCall = g_CGAInterface->SellNPCStore(cgaSaleItems, bResult);
	}

	g_pGameFun->WaitRecvNpcDialog();
	return true;
}

bool CGFunction::IdentifyItemEx(int nDir, const QString &itemName)
{
	QPoint dirPos = GetDirectionPos(nDir, 2);
	return IdentifyItem(dirPos.x(), dirPos.y(), itemName);
}

bool CGFunction::IdentifyItemEx2(QString sDir, const QString &itemName)
{
	QPoint dirPos = GetDirectionPosEx(sDir, 2);
	return IdentifyItem(dirPos.x(), dirPos.y(), itemName);
}

//??????name???????????? subName?????? ???????????? ????????????
bool CGFunction::FirstAid(const QString &name, const QString &subName /*=""*/, int lv)
{
	qDebug() << name << subName << lv;
	if (name.isEmpty() || subName.isEmpty())
	{
		return false;
	}
	GameSkillList pSkillList = g_pGameFun->GetGameSkills();
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
		qDebug() << "??????????????????";
		return false;
	}
	bool bResult = false;
	QEventLoop loop;
	g_CGAInterface->StartWork(pFirstAidSkill->index, lv, bResult);
	//5???
	QTimer::singleShot(5000, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyPlayerMenu, [&](QSharedPointer<CGA::cga_player_menu_items_t> players)
			{
				if (players && players->size() > 0)
				{
					for (int i = 0; i < players->size(); ++i)
					{
						if (QString::fromStdString(players->at(i).name) == name)
						{
							g_CGAInterface->PlayerMenuSelect(players->at(i).index, players->at(i).name, bResult);
							qDebug() << QString::fromStdString(players->at(i).name);
						}
					}
				}
				//loop.quit();
				QTimer::singleShot(5000, &loop, &QEventLoop::quit);
			});
	auto connection2 = connect(g_pGameCtrl, &GameCtrl::NotifyUnitMenu, [&](QSharedPointer<CGA::cga_unit_menu_items_t> units)
			{
				qDebug() << units->size();
				if (units && units->size() > 0)
				{
					for (int i = 0; i < units->size(); ++i)
					{
						if (QString::fromStdString(units->at(i).name) == subName)
						{
							g_CGAInterface->UnitMenuSelect(units->at(i).index, bResult);
						}
					}
				}
				if (loop.isRunning())
					loop.quit();
			});

	loop.exec();
	QObject::disconnect(connection);  //??????Connection ??????lambda?????????
	QObject::disconnect(connection2); //??????Connection ??????lambda?????????
	return true;
}

GameConditionCfg *CGFunction::GetStopEncounter(int type)
{
	GameConditionCfg *pCfg = m_pStopEncounterJudge.value(type);
	if (pCfg == nullptr)
	{
		pCfg = new GameConditionCfg;
		pCfg->nType = type;
		m_pStopEncounterJudge.insert(type, pCfg);
	}
	return pCfg;
}

GameConditionCfg *CGFunction::GetStopScriptCfg(int type)
{
	GameConditionCfg *pCfg = m_pStopScriptJudge.value(type);
	if (pCfg == nullptr)
	{
		pCfg = new GameConditionCfg;
		pCfg->nType = type;
		m_pStopScriptJudge.insert(type, pCfg);
	}
	return pCfg;
}
//??????????????????  ??????????????????
bool CGFunction::IsNeedStopScript()
{
	if (m_pStopScriptJudge.size() < 1)
		return false;
	auto pCharacter = g_pGameFun->GetGameCharacter(); //????????????
	if (!pCharacter)
		return false;
	GamePetPtr pPet = GetBattlePet();
	auto pTeammateInfo = GetTeamPlayers();
	for (auto it = m_pStopScriptJudge.begin(); it != m_pStopScriptJudge.end(); ++it)
	{
		if (it.value() && it.value()->bChecked)
		{
			GameConditionCfg *pCfg = it.value();
			switch (pCfg->nType)
			{
				case TCondition_StopEncounterPlayerHp:
				{
					if ((pCharacter->hp < (pCharacter->maxhp * pCfg->dVal / 100)))
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("?????????????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_StopEncounterPlayerMp:
				{
					if ((pCharacter->mp < (pCharacter->maxmp * pCfg->dVal / 100)))
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("?????????????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_StopEncounterPetHp:
				{
					if (pPet && (pPet->hp < (pPet->maxhp * pCfg->dVal / 100)))
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("????????????????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_StopEncounterPetMp:
				{
					if (pPet && (pPet->mp < (pPet->maxmp * pCfg->dVal / 100)))
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("????????????????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_StopEncounterTeammateHp:
				{
					for (auto pTeamInfo : pTeammateInfo)
					{
						if ((pTeamInfo->hp < (pTeamInfo->maxhp * pCfg->dVal / 100)))
						{
							emit g_pGameCtrl->signal_addOneScriptLogMsg("???????????????????????????????????????");
							return true;
						}
					}
					break;
				}
				case TCondition_StopEncounterTroopCount:
				{
					if (GetTeammatesCount() < pCfg->dVal)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("???????????????????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_PlayerInjuredWhite:
				{
					if (pCharacter->health > 0 && pCharacter->health < 25)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("???????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_PlayerInjuredYellow:
				{
					if (pCharacter->health >= 25 && pCharacter->health < 50)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("???????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_PlayerInjuredPurple:
				{
					if (pCharacter->health >= 50 && pCharacter->health < 75)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("???????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_PlayerInjuredRed:
				{
					if (pCharacter->health >= 75 && pCharacter->health <= 100)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("???????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_PetCountFull:
				{
					if (GetPetCount() >= 5)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_ItemListFull:
				{
					if (GetInventoryEmptySlotCount() < 1)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_DontHaveMagicFood:
				{
					if (GetMagicFoodItems().size() < 1)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("?????????????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_DontHaveMedicament:
				{
					if (GetMedicamentFoodItems().size() < 1)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("?????????????????????????????????");
						return true;
					}
					break;
				}
				case TCondition_BattlePetLoyaltyTooLow:
				{
					if (pPet && pPet->loyality < 60)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("??????????????????60???????????????");
						return true;
					}
					break;
				}
				case TCondition_PlayeLossSoul:
				{
					if (pCharacter->souls > 0)
					{
						emit g_pGameCtrl->signal_addOneScriptLogMsg("???????????????????????????");
						return true;
					}
					break;
				}
				default:
					break;
			}
		}
	}
	return false;
}
bool CGFunction::IsNeedStopEncounter()
{
	if (m_pStopEncounterJudge.size() < 1)
		return false;
	auto pCharacter = GetGameCharacter(); //????????????
	if (!pCharacter)
		return false;
	GamePetPtr pPet = GetBattlePet();
	auto pTeammateInfo = GetTeamPlayers();
	for (auto it = m_pStopEncounterJudge.begin(); it != m_pStopEncounterJudge.end(); ++it)
	{
		if (it.value() && it.value()->bChecked)
		{
			GameConditionCfg *pCfg = it.value();
			switch (pCfg->nType)
			{
				case TCondition_StopEncounterPlayerHp:
				{
					if ((pCharacter->hp < (pCharacter->maxhp * pCfg->dVal / 100)))
						return true;
					break;
				}
				case TCondition_StopEncounterPlayerMp:
				{
					if ((pCharacter->mp < (pCharacter->maxmp * pCfg->dVal / 100)))
						return true;
					break;
				}
				case TCondition_StopEncounterPetHp:
				{
					if (pPet && (pPet->hp < (pPet->maxhp * pCfg->dVal / 100)))
						return true;
					break;
				}
				case TCondition_StopEncounterPetMp:
				{
					if (pPet && (pPet->mp < (pPet->maxmp * pCfg->dVal / 100)))
						return true;
					break;
				}
				case TCondition_StopEncounterTeammateHp:
				{
					for (auto pTeamInfo : pTeammateInfo)
					{
						if ((pTeamInfo->hp < (pTeamInfo->maxhp * pCfg->dVal / 100)))
						{
							return true;
						}
					}
					break;
				}
				case TCondition_StopEncounterTroopCount:
				{
					if (GetTeammatesCount() < pCfg->dVal)
						return true;
					break;
				}
				case TCondition_PlayerInjuredWhite:
				{
					if (pCharacter->health > 0 && pCharacter->health < 25)
						return true;
					break;
				}
				case TCondition_PlayerInjuredYellow:
				{
					if (pCharacter->health >= 25 && pCharacter->health < 50)
						return true;
					break;
				}
				case TCondition_PlayerInjuredPurple:
				{
					if (pCharacter->health >= 50 && pCharacter->health < 75)
						return true;
					break;
				}
				case TCondition_PlayerInjuredRed:
				{
					if (pCharacter->health >= 75 && pCharacter->health <= 100)
						return true;
					break;
				}
				case TCondition_PetCountFull:
				{
					if (GetPetCount() >= 5)
						return true;
					break;
				}
				case TCondition_ItemListFull:
				{
					if (GetInventoryEmptySlotCount() < 1)
						return true;
					break;
				}
				case TCondition_DontHaveMagicFood:
				{
					if (GetMagicFoodItems().size() < 1)
						return true;
					break;
				}
				case TCondition_DontHaveMedicament:
				{
					if (GetMedicamentFoodItems().size() < 1)
						return true;
					break;
				}
				case TCondition_BattlePetLoyaltyTooLow:
				{
					if (pPet && pPet->loyality < 60)
						return true;
					break;
				}
				case TCondition_PlayeLossSoul:
				{
					if (pCharacter->souls > 0)
						return true;
					break;
				}
				default:
					break;
			}
		}
	}
	return false;
}
//???????????????
//??????????????? ??????
//???????????????id ??????
//??????????????? 4 ?????? 40?????? 1??????  11?????? ????????????
//?????????????????? 1?????? 0????????? ??????????????????
//?????????????????? 1?????? 0????????? ??????????????????
bool CGFunction::LoginGameServerEx(QString account, QString pwd, QString gid /*= ""*/, int gametype /*= 4*/, int rungame /*= 1*/, int skipupdate /*= 1*/)
{
	//?????? ??????????????? ?????????????????? ????????????????????????
	emit signal_loginGameServerEx(account, pwd, gid, gametype, rungame, skipupdate);
	return true;
}

//???????????????
//gid??????id glt????????????????????? serverid?????????id
//bigServerIndex??????0  ??????2
//serverIndex 1-10??? ??????0-9???
//character??? ??? ?????? ??????0/1)
bool CGFunction::LoginGameServer(QString gid, QString glt, int serverid, int bigServerIndex, int serverIndex, int character)
{
	if (g_CGAInterface->IsConnected())
		g_CGAInterface->LoginGameServer(gid.toStdString(), glt.toStdString(), serverid, bigServerIndex, serverIndex, character);
	return true;
}

QStringList CGFunction::GetPetCalcBpData(GamePetPtr pet)
{
	if (!pet)
		return QStringList();
	QStringList calcData;
	calcData << pet->realname << QString::number(pet->maxhp) << QString::number(pet->maxmp)
			 << QString::number(pet->detail.value_attack) << QString::number(pet->detail.value_defensive)
			 << QString::number(pet->detail.value_agility) << QString::number(pet->detail.value_spirit)
			 << QString::number(pet->detail.value_recovery) << QString::number(pet->detail.points_endurance)
			 << QString::number(pet->detail.points_strength) << QString::number(pet->detail.points_defense)
			 << QString::number(pet->detail.points_agility) << QString::number(pet->detail.points_magical);
	return calcData;
}

QStringList CGFunction::GetPetCalcBpData(const CGA::cga_pet_info_t &pet)
{
	QStringList calcData;
	calcData << QString::fromStdString(pet.realname) << QString::number(pet.maxhp) << QString::number(pet.maxmp)
			 << QString::number(pet.detail.value_attack) << QString::number(pet.detail.value_defensive)
			 << QString::number(pet.detail.value_agility) << QString::number(pet.detail.value_spirit)
			 << QString::number(pet.detail.value_recovery) << QString::number(pet.detail.points_endurance)
			 << QString::number(pet.detail.points_strength) << QString::number(pet.detail.points_defense)
			 << QString::number(pet.detail.points_agility) << QString::number(pet.detail.points_magical);
	return calcData;
}

QString CGFunction::GetPetCalcFiveAttribute(const CGA::cga_pet_info_t &pet, QString splitData)
{
	QString petData;
	petData += QString::number(pet.maxhp);
	petData += splitData;
	petData += QString::number(pet.maxmp);
	petData += splitData;
	petData += QString::number(pet.detail.value_attack);
	petData += splitData;
	petData += QString::number(pet.detail.value_defensive);
	petData += splitData;
	petData += QString::number(pet.detail.value_agility);
	return petData;
}

QString CGFunction::GetLogPetAttribute(const CGA::cga_pet_info_t &pet, QString splitData /*= " "*/)
{
	QString petData;
	petData += "??????:";
	petData += QString::number(pet.maxhp);
	petData += splitData;
	petData += QString::number(pet.maxmp);
	petData += splitData;
	petData += QString::number(pet.detail.value_attack);
	petData += splitData;
	petData += QString::number(pet.detail.value_defensive);
	petData += splitData;
	petData += QString::number(pet.detail.value_agility);
	petData += splitData;
	petData += QString::number(pet.detail.value_spirit);
	petData += splitData;
	petData += QString::number(pet.detail.value_recovery);
	petData += " Bp:";
	petData += QString::number(pet.detail.points_endurance);
	petData += splitData;
	petData += QString::number(pet.detail.points_strength);
	petData += splitData;
	petData += QString::number(pet.detail.points_defense);
	petData += splitData;
	petData += QString::number(pet.detail.points_agility);
	petData += splitData;
	petData += QString::number(pet.detail.points_magical);
	return petData;
}

QString CGFunction::GetPetCalcBpAttribute(const CGA::cga_pet_info_t &pet, QString splitData)
{
	QString petData;
	petData += QString::number(pet.maxhp);
	petData += splitData;
	petData += QString::number(pet.maxmp);
	petData += splitData;
	petData += QString::number(pet.detail.value_attack);
	petData += splitData;
	petData += QString::number(pet.detail.value_defensive);
	petData += splitData;
	petData += QString::number(pet.detail.value_agility);
	petData += splitData;
	petData += QString::number(pet.detail.value_spirit);
	petData += splitData;
	petData += QString::number(pet.detail.value_recovery);
	petData += splitData;
	petData += QString::number(pet.detail.points_endurance);
	petData += splitData;
	petData += QString::number(pet.detail.points_strength);
	petData += splitData;
	petData += QString::number(pet.detail.points_defense);
	petData += splitData;
	petData += QString::number(pet.detail.points_agility);
	petData += splitData;
	petData += QString::number(pet.detail.points_magical);
	return petData;
}

QString CGFunction::GetPlayerAttribute(CGA::cga_player_info_t info, QString splitData)
{
	QString infoData;
	infoData += QString::number(info.maxhp);
	infoData += splitData;
	infoData += QString::number(info.maxmp);
	infoData += splitData;
	infoData += QString::number(info.detail.value_attack);
	infoData += splitData;
	infoData += QString::number(info.detail.value_defensive);
	infoData += splitData;
	infoData += QString::number(info.detail.value_agility);
	infoData += splitData;
	infoData += QString::number(info.detail.value_spirit);
	infoData += splitData;
	infoData += QString::number(info.detail.value_recovery);
	infoData += splitData;
	infoData += QString::number(info.detail.points_endurance);
	infoData += splitData;
	infoData += QString::number(info.detail.points_strength);
	infoData += splitData;
	infoData += QString::number(info.detail.points_defense);
	infoData += splitData;
	infoData += QString::number(info.detail.points_agility);
	infoData += splitData;
	infoData += QString::number(info.detail.points_magical);
	return infoData;
}

QString CGFunction::GetPetFixVal(const CGA::cga_pet_info_t &pet, QString splitData)
{
	QString petData;
	petData += QString("???:%1").arg(pet.detail.fix_critical);
	petData += splitData;
	petData += QString("???:%1").arg(pet.detail.fix_strikeback);
	petData += splitData;
	petData += QString("???:%1").arg(pet.detail.fix_accurancy);
	petData += splitData;
	petData += QString("???:%1").arg(pet.detail.fix_dodge);
	return petData;
}

QString CGFunction::GetPlayerFixVal(CGA::cga_player_info_t info, QString splitData)
{
	QString infoData;
	infoData += QString("???:%1").arg(info.detail.fix_critical);
	infoData += splitData;
	infoData += QString("???:%1").arg(info.detail.fix_strikeback);
	infoData += splitData;
	infoData += QString("???:%1").arg(info.detail.fix_accurancy);
	infoData += splitData;
	infoData += QString("???:%1").arg(info.detail.fix_dodge);
	return infoData;
}

bool CGFunction::RestPetState(int petindex, int petstate)
{
	bool bRet = false;
	int nVal = petstate;
	if (nVal != 1 && nVal != 2 && nVal != 3 && nVal != 16)
		return false;
	else
	{
		if (petindex < 0 || petindex >= 5)
			return false;
		if (nVal == TPET_STATE_BATTLE) //????????????????????????????????????????????????
		{
			auto pPetList = GetGamePets();
			foreach (auto battlePet, pPetList)
			{
				if (battlePet && battlePet->exist && battlePet->battle_flags == TPET_STATE_BATTLE) //??????????????????
				{
					g_CGAInterface->ChangePetState(battlePet->index, TPET_STATE_READY, bRet);
					Sleep(1000);
					break;
				}
			}
		}
		g_CGAInterface->ChangePetState(petindex, nVal, bRet);
		return bRet;
	}
}
int CGFunction::GetPetCount()
{
	auto pPetList = GetGamePets();
	int nCount = 0;
	for (size_t i = 0; i < pPetList.size(); i++)
	{
		if (pPetList[i]->exist)
		{
			++nCount;
		}
	}
	return nCount;
}

QList<CGPetPictorialBookPtr> CGFunction::GetAllPetPictorialBook()
{
	QList<CGPetPictorialBookPtr> petBookList;
	CGA::cga_picbooks_info_t picbooks;
	if (g_CGAInterface->GetPicBooksInfo(picbooks))
	{
		for (auto tPicBook : picbooks)
		{
			auto picBook = QSharedPointer<CGPetPictorialBook>(new CGPetPictorialBook);
			picBook->can_catch = tPicBook.can_catch;
			picBook->card_type = tPicBook.card_type;
			picBook->raceType = tPicBook.race;
			picBook->number = tPicBook.index;
			picBook->image_id = tPicBook.image_id;
			picBook->name = QString::fromStdString(tPicBook.name);
			picBook->skill_slots = tPicBook.skill_slots;
			picBook->rate_endurance = tPicBook.rate_endurance;
			picBook->rate_strength = tPicBook.rate_strength;
			picBook->rate_defense = tPicBook.rate_defense;
			picBook->rate_agility = tPicBook.rate_agility;
			picBook->rate_magical = tPicBook.rate_magical;
			picBook->element_earth = tPicBook.element_earth;
			picBook->element_water = tPicBook.element_water;
			picBook->element_fire = tPicBook.element_fire;
			picBook->element_wind = tPicBook.element_wind;
			petBookList.append(picBook);
		}
	}
	return petBookList;
}

bool CGFunction::SavePetPictorialBookToHtml(QString path, bool bInPic)
{
	QString sHtml;
	sHtml = QString("<html>\n <body>\n<table border=\"1\" cellspacing=\"0\" >");
	QStringList sTableTitle;
	sTableTitle << "??????"
				<< "??????"
				<< "??????"
				<< "?????????"
				<< "????????????"
				<< "?????????"
				<< " ??? "
				<< " ??? "
				<< " ??? "
				<< " ??? ";
	if (bInPic)
	{
		sTableTitle << "??????";
	}

	QMap<int, QString> cardType;
	cardType.insert(0, "???");
	cardType.insert(1, "??????");
	cardType.insert(2, "??????");
	CGA::cga_picbooks_info_t picbooks;
	if (g_CGAInterface->GetPicBooksInfo(picbooks))
	{
		//??????
		QString sTitleRow = ("<tr>");
		for (auto tTitle : sTableTitle)
		{
			sTitleRow += QString("<td align=\"center\">%1</td>").arg(tTitle);
		}

		sTitleRow += "</tr>";
		sHtml += sTitleRow;
		//?????????
		QString sImagePath = QApplication::applicationDirPath() + "//db//??????//";
		for (int i = 1; i <= 317; ++i)
		{
			bool bFind = false;
			for (auto tPicBook : picbooks)
			{
				if (tPicBook.index == i)
				{
					bFind = true;
					QString sRow = ("<tr>");
					sHtml += sRow;
					sRow = QString("<td align=\"center\">%1</td>\n").arg(tPicBook.index);
					sHtml += sRow;
					sRow = QString("<td align=\"center\">%1</td>\n").arg(QString::fromStdString(tPicBook.name));
					sHtml += sRow;
					sRow = QString("<td align=\"center\">%1</td>\n").arg(m_petRaceMap.value(tPicBook.race));
					sHtml += sRow;
					sRow = QString("<td align=\"center\">%1</td>\n").arg(tPicBook.skill_slots);
					sHtml += sRow;
					sRow = QString("<td align=\"center\">%1</td>\n").arg(tPicBook.can_catch ? "?????????" : "????????????");
					sHtml += sRow;
					sRow = QString("<td align=\"center\">%1</td>\n").arg(cardType.value(tPicBook.card_type));
					sHtml += sRow;
					sRow = QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"green\">%1</td>\n").arg(tPicBook.element_earth);
					sHtml += sRow;
					sRow = QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"#00a9fa\">%1</td>\n").arg(tPicBook.element_water);
					sHtml += sRow;
					sRow = QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"red\">%1</td>\n").arg(tPicBook.element_fire);
					sHtml += sRow;
					sRow = QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"#b18e00\">%1</td>\n").arg(tPicBook.element_wind);
					sHtml += sRow;
					if (bInPic)
					{
						QString imagePath = QString("%1/%2.gif").arg(sImagePath).arg(tPicBook.image_id);
						if (QFile::exists(imagePath))
						{
							sRow = QString("<td align=\"center\"><img src=\"%1\"></td>\n").arg(imagePath);
							sHtml += sRow;
						}
					}
					/*sRow += QString("<td align=\"center\">%1</td>\n<td align=\"center\">%2</td>\n\
			<td align=\"center\">%3</td>\n<td align=\"center\">%4</td>\n<td align=\"center\">%5</td>\n\
			<td align=\"center\">%6</td>\n<td  align=\"center\" bgcolor=\"green\">%7</td>\n\
			<td align=\"center\" bgcolor=\"#00a9fa\">%8</td>\n<td align=\"center\" bgcolor=\"red\">%9</td>\n\
			<td align=\"center\" bgcolor=\"#b18e00\">%10</td>")
				.arg(tPicBook.index).arg(QString::fromStdString(tPicBook.name)).arg(m_petRaceMap.value(tPicBook.race))\
				.arg(tPicBook.skill_slots).arg(tPicBook.can_catch ?"?????????" : "????????????")\
				.arg(cardType.value(tPicBook.card_type)).arg(tPicBook.element_earth)\
				.arg(tPicBook.element_water).arg(tPicBook.element_fire).arg(tPicBook.element_wind); */
					sRow += "< /tr >";
					//sHtml += sRow;
					break;
				}
			}
			if (!bFind)
			{
				auto tGamePet = ITObjectDataMgr::getInstance().GetGamePetFromNumber(i);
				if (!tGamePet)
					continue;

				QString sRow = ("<tr>");
				sHtml += sRow;
				sRow = QString("<td align=\"center\" bgcolor=\"yellow\">%1</td>\n").arg(i);
				sHtml += sRow;
				sRow = QString("<td align=\"center\" bgcolor=\"yellow\">%1</td>\n").arg(tGamePet->getObjectName());
				sHtml += sRow;
				sRow = QString("<td align=\"center\" bgcolor=\"yellow\">%1</td>\n").arg(tGamePet->_petRace);
				sHtml += sRow;
				sRow = QString("<td align=\"center\" bgcolor=\"yellow\">%1</td>\n").arg("--");
				sHtml += sRow;
				sRow = QString("<td align=\"center\" bgcolor=\"yellow\">%1</td>\n").arg("--");
				sHtml += sRow;
				sRow = QString("<td align=\"center\" bgcolor=\"yellow\">%1</td>\n").arg("--");
				sHtml += sRow;
				sRow = QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"green\">%1</td>\n").arg("--");
				sHtml += sRow;
				sRow = QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"#00a9fa\">%1</td>\n").arg("--");
				sHtml += sRow;
				sRow = QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"red\">%1</td>\n").arg("--");
				sHtml += sRow;
				sRow = QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"#b18e00\">%1</td>\n").arg("--");
				sHtml += sRow;
				if (bInPic)
				{
					QString imagePath = QString("%1/%2.gif").arg(sImagePath).arg(tGamePet->_imageId);
					if (QFile::exists(imagePath))
					{
						sRow = QString("<td align=\"center\"><img src=\"%1\"></td>\n").arg(imagePath);
						sHtml += sRow;
					}
				}
				sRow += "< /tr >";
			}
		}
	}
	sHtml += QString("</body >\n</html>");
	if (path.isEmpty())
	{
		path = QString("%1\\??????\\%2.html").arg(QApplication::applicationDirPath()).arg(g_pGameFun->GetGameCharacter()->name);
	}
	QFile file(path);
	if (file.open(QFile::ReadWrite))
		file.write(sHtml.toStdString().c_str());
	file.close();
	return true;
}

bool CGFunction::SetDropItemState(int state, QString name /*= ""*/)
{
	if (name.isEmpty())
	{
		emit g_pGameCtrl->signal_switchAutoDrop(state);
	}
	else
	{
		emit g_pGameCtrl->signal_addRenItemScript(name, state);
	}
	return true;
}
bool CGFunction::SetPileItemState(int state, QString name /*= ""*/)
{
	if (name.isEmpty())
	{
		emit g_pGameCtrl->signal_switchAutoPile(state);
	}
	else
	{
		emit g_pGameCtrl->signal_addDieItemScript(name, state);
	}
	return true;
}
//????????????????????????
QList<QStringList> CGFunction::GatherAccountInfo()
{
	QList<QStringList> characterInfos;
	//?????????????????????????????????????????????????????????????????????
	CGA::cga_player_info_t info;
	if (g_CGAInterface->GetPlayerInfo(info))
	{
		QStringList playerInfo;
		playerInfo << QString::fromStdString(info.name) << QString::fromStdString(info.job) << QString("Lv:%1").arg(info.level)
				   << QString("??????:%1").arg(info.gold);
		characterInfos << playerInfo;
		qDebug() << QString::fromStdString(info.name) << QString::fromStdString(info.job) << "Lv:" << info.level << " ??????:" << info.gold;
	}

	CGA::cga_pets_info_t petsinfo;
	if (g_CGAInterface->GetPetsInfo(petsinfo))
	{
		for (size_t i = 0; i < petsinfo.size(); ++i)
		{
			QStringList sInfo;
			const CGA::cga_pet_info_t &petinfo = petsinfo.at(i);
			qDebug() << QString::fromStdString(petinfo.realname) << "Lv:" << petinfo.level;
			sInfo << QString::fromStdString(petinfo.realname) << QString::number(petinfo.level);
			characterInfos << sInfo;
		}
	}
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			QStringList sInfo;
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			qDebug() << QString::fromStdString(iteminfo.name) << "Count:" << iteminfo.count << "Code:" << iteminfo.itemid;
			sInfo << QString::fromStdString(iteminfo.name) << QString::number(iteminfo.count) << QString::number(iteminfo.itemid);
			characterInfos << sInfo;
		}
	}

	int bankGold = 0;
	g_CGAInterface->GetBankGold(bankGold);
	qDebug() << "???????????????" << bankGold;
	QStringList sInfo;
	sInfo << QString::number(bankGold);
	characterInfos << sInfo;

	CGA::cga_items_info_t myinfos;
	g_CGAInterface->GetBankItemsInfo(myinfos);
	for (int i = 0; i < myinfos.size(); i++)
	{
		QStringList sInfo;
		CGA::cga_item_info_t itemInfo = myinfos.at(i);
		qDebug() << "??????" << QString::fromStdString(itemInfo.name) << "Count:" << itemInfo.count << "Code:" << itemInfo.itemid;
		sInfo << QString::fromStdString(itemInfo.name) << QString::number(itemInfo.count) << QString::number(itemInfo.itemid);
		characterInfos << sInfo;
	}
	CGA::cga_pets_info_t bankPets;
	g_CGAInterface->GetBankPetsInfo(bankPets);

	for (size_t i = 0; i < bankPets.size(); ++i)
	{
		const CGA::cga_pet_info_t &petinfo = bankPets.at(i);
		qDebug() << "??????" << QString::fromStdString(petinfo.realname) << "Lv:" << petinfo.level;
		QStringList sInfo;
		sInfo << QString::fromStdString(petinfo.realname) << QString::number(petinfo.level);
		characterInfos << sInfo;
	}
	qDebug() << "????????????" << bankGold;
	return characterInfos;
}
QString CGFunction::CreateHtmlTable(QVariantList sTextList)
{
	//?????????
	QString sTitleRow = ("<table border=\"1\" cellspacing=\"0\" >\n<tr>");
	for (auto tTitle : sTextList)
	{
		sTitleRow += QString("<td style=\"color:#FFFFFF\"align=\"center\" bgcolor=\"green\">%1</td>").arg(tTitle.toString());
	}
	sTitleRow += "</tr>";
	return sTitleRow;
}
QString CGFunction::CreateHtmlRow(QVariantList sTextList)
{
	//?????????
	QString sRow = ("<tr>");
	for (auto sText : sTextList)
	{
		sRow += QString("<td align=\"left\" style=\"white-space:pre-line;\">%1</td>\n").arg(sText.toString());
	}
	sRow += "</tr>";
	return sRow;
}

QString CGFunction::CreateHtmlRows(QList<QVariantList> sTextLists)
{
	QString sRow;
	for (auto sTextList : sTextLists)
	{
		sRow += CreateHtmlRow(sTextList);
	}
	return sRow;
}

bool CGFunction::SaveGatherAccountInfos()
{
	QString sHtml;
	sHtml = QString("<html>\n <body>\n");
	QVariantList sTableTitle;
	sTableTitle << "??????"
				<< "??????"
				<< "??????"
				<< "????????????"
				<< "??????"
				<< "????????????"
				<< "??????"
				<< "??????"
				<< "??????"
				<< "??? ??? ??? ??? ??? ??? ??? ??? ??? ??? ??? ???"
				<< "??????";

	//??????
	sHtml += CreateHtmlTable(sTableTitle);

	QList<QVariantList> tableRowData;
	//?????????????????????????????????????????????????????????????????????
	CGA::cga_player_info_t info;
	if (g_CGAInterface->GetPlayerInfo(info))
	{
		int bankGold = 0;
		g_CGAInterface->GetBankGold(bankGold);
		QVariantList playerInfo;
		//??????
		int pclock = info.punchclock;
		pclock /= 1000;
		int hours = pclock / 3600;
		int mins = (pclock - hours * 3600) / 60;
		QString sPClock = QString("%1:%2").arg(hours).arg(mins, 2, 10, QLatin1Char('0'));

		playerInfo << QString::fromStdString(info.name) << QString::fromStdString(info.job)
				   << QString("Lv:%1").arg(info.level)
				   << (info.maxxp == -1 ? "??????" : QString::number(info.maxxp - info.xp))
				   << QString::number(info.gold)
				   << QString::number(bankGold)
				   << info.health
				   << info.value_charisma
				   << sPClock
				   << GetPlayerAttribute(info)
				   << GetPlayerFixVal(info);
		tableRowData << playerInfo;
	}
	sHtml += CreateHtmlRows(tableRowData);
	sHtml += "</table>";
	sTableTitle.clear();
	sTableTitle << "??????"
				<< "????????????"
				<< "??????"
				<< "????????????"
				<< "????????????"
				<< "????????????"
				<< "????????????";
	sHtml += CreateHtmlTable(sTableTitle);
	tableRowData.clear();
	CGA::cga_skills_info_t skillsinfo;
	if (g_CGAInterface->GetSkillsInfo(skillsinfo))
	{
		for (size_t i = 0; i < skillsinfo.size(); ++i)
		{
			const CGA::cga_skill_info_t &skinfo = skillsinfo.at(i);
			QVariantList sInfo;
			sInfo << i + 1 << QString::fromStdString(skinfo.name) << skinfo.lv
				  << skinfo.maxlv
				  << skinfo.xp
				  << (skinfo.maxxp == 99999999 ? 0 : skinfo.maxxp)
				  << (skinfo.maxxp == 99999999 ? "??????" : QString::number(skinfo.maxxp - skinfo.xp));
			tableRowData << sInfo;
		}
	}
	sHtml += CreateHtmlRows(tableRowData);
	sHtml += "</table>\n";

	sTableTitle.clear();
	sTableTitle << "??????"
				<< "??????"
				<< "??????"
				<< "??????"
				<< "??????"
				<< "??????"
				<< "????????????"
				<< "???"
				<< "???"
				<< "???"
				<< "???"
				<< "??? ??? ??? ??? ??? ??? ??? ??? ??? ??? ??? ???"
				<< "??????";
	sHtml += CreateHtmlTable(sTableTitle);
	tableRowData.clear();

	//??????????????????
	auto createPetHtmlCellData = [&](int number, const CGA::cga_pet_info_t &petinfo)
	{
		QVariantList sInfo;
		QString petGrade = "-";
		if (petinfo.level == 1)
		{
			QStringList inputData = g_pGameFun->GetPetCalcBpData(petinfo);
			auto pCalcData = g_pGamePetCalc->ParseLine(inputData);
			if (pCalcData)
				petGrade = QString("%1-%2").arg(pCalcData->lossMin).arg(pCalcData->lossMax);
		}
		QString sPetName = QString::fromStdString(petinfo.name);
		if (petinfo.name.empty())
		{
			sPetName = QString::fromStdString(petinfo.realname);
		}
		sInfo << QString::number(number + 1) << QString::fromStdString(petinfo.realname)
			  << sPetName
			  << QString::number(petinfo.level)
			  << QString::number(petinfo.loyality) << petGrade
			  << (petinfo.maxxp == -1 ? "??????" : QString::number(petinfo.maxxp - petinfo.xp))
			  << petinfo.detail.element_earth << petinfo.detail.element_water
			  << petinfo.detail.element_fire << petinfo.detail.element_wind
			  << GetPetCalcBpAttribute(petinfo)
			  << GetPetFixVal(petinfo);
		return sInfo;
	};

	CGA::cga_pets_info_t petsinfo;
	if (g_CGAInterface->GetPetsInfo(petsinfo))
	{
		for (size_t i = 0; i < petsinfo.size(); ++i)
		{
			QVariantList sInfo;
			const CGA::cga_pet_info_t &petinfo = petsinfo.at(i);
			sInfo = createPetHtmlCellData(i, petinfo);
			tableRowData << sInfo;
		}
	}
	sHtml += CreateHtmlRows(tableRowData);
	sHtml += "</table>\n";

	sTableTitle[1] = QVariant("????????????");
	sHtml += CreateHtmlTable(sTableTitle);
	tableRowData.clear();
	CGA::cga_pets_info_t bankPets;
	g_CGAInterface->GetBankPetsInfo(bankPets);
	for (size_t i = 0; i < bankPets.size(); ++i)
	{
		const CGA::cga_pet_info_t &petinfo = bankPets.at(i);
		QVariantList sInfo;
		sInfo = createPetHtmlCellData(i, petinfo);
		tableRowData << sInfo;
	}
	sHtml += CreateHtmlRows(tableRowData);
	sHtml += "</table>\n";

	sTableTitle.clear();
	sTableTitle << "??????"
				<< "????????????"
				<< "????????????"
				<< "????????????"
				<< "????????????"
				<< "????????????"
				<< "????????????";
	sHtml += CreateHtmlTable(sTableTitle);
	tableRowData.clear();
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{

			QVariantList sInfo;
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);

			QString itemInfo = QString("%1 \n%2\n").arg(QString::fromStdString(iteminfo.attr)).arg(QString::fromStdString(iteminfo.info));
			for (int n = 0; n < 10; ++n)
			{
				itemInfo = itemInfo.remove(QString("$%1").arg(n));
			}
			sInfo << QString::number(i + 1) << QString::fromStdString(iteminfo.name) << QString::number(iteminfo.count ? iteminfo.count : 1) << QString::number(iteminfo.itemid)
				  << iteminfo.type << iteminfo.level << itemInfo;
			tableRowData << sInfo;
		}
	}
	sHtml += CreateHtmlRows(tableRowData);
	sHtml += "</table>\n";

	sTableTitle.clear();
	sTableTitle << "??????"
				<< "??????????????????"
				<< "??????????????????"
				<< "??????????????????"
				<< "??????????????????"
				<< "??????????????????"
				<< "??????????????????";
	sHtml += CreateHtmlTable(sTableTitle);
	tableRowData.clear();
	CGA::cga_items_info_t myinfos;
	g_CGAInterface->GetBankItemsInfo(myinfos);
	for (int i = 0; i < myinfos.size(); i++)
	{
		CGA::cga_item_info_t iteminfo = myinfos.at(i);
		QVariantList sInfo;
		QString itemInfo = QString("%1 \n%2\n").arg(QString::fromStdString(iteminfo.attr)).arg(QString::fromStdString(iteminfo.info));
		for (int n = 0; n < 10; ++n)
		{
			itemInfo = itemInfo.remove(QString("$%1").arg(n));
		}
		sInfo << QString::number(i + 1) << QString::fromStdString(iteminfo.name) << QString::number(iteminfo.count ? iteminfo.count : 1) << QString::number(iteminfo.itemid)
			  << iteminfo.type << iteminfo.level << itemInfo;
		tableRowData << sInfo;
	}
	sHtml += CreateHtmlRows(tableRowData);
	sHtml += "</table>\n";

	sHtml += QString("</body >\n</html>");
	QString path;
	if (path.isEmpty())
	{
		QString saveName = GetGameCharacter()->name + "-" + GetGameCharacter()->sGid;
		path = QString("%1\\??????\\%2-????????????.html").arg(QApplication::applicationDirPath()).arg(saveName);
	}
	QFile file(path);
	if (file.open(QFile::ReadWrite))
		file.write(sHtml.toStdString().c_str());
	file.close();
	return true;
}

bool CGFunction::SaveAccountInfoToExcel()
{
	QXlsx::Document xlsx;
	//![0]

	//![1]
	xlsx.write("A1", "Hello Qt!");
	xlsx.write("A2", 12345);
	xlsx.write("A3", "=44+33");
	xlsx.write("A4", true);
	xlsx.write("A5", "http://qt-project.org");
	xlsx.write("A6", QDate(2013, 12, 27));
	xlsx.write("A7", QTime(6, 30));
	//![1]

	//![2]
	xlsx.save();
	return true;
}

//???????????? ????????????
void CGFunction::GoBank()
{
	int mapNumber = GetMapIndex();
	if (mapNumber == 59520) //????????????
	{
		AutoMoveTo(157, 94);
		TurnAboutEx(158, 93);
		Nowhile("?????????");
		AutoMoveTo(114, 105);
		AutoMoveTo(114, 104);
		Nowhile("??????");
		AutoMoveTo(49, 30);
		TurnAbout(MOVE_DIRECTION_East);
		WaitRecvHead();
	}
}

void CGFunction::ReadUserConfig(QString cfg)
{
	emit g_pGameCtrl->signal_loadUserConfig(cfg);
}

void CGFunction::WriteUserConfig(QString cfg)
{
	emit g_pGameCtrl->signal_saveUserConfig(cfg);
}

QString CGFunction::CreateRandomName(int sex)
{
	switch (sex)
	{
		case 0:
		{
			return CreateBoyName();
			break;
		}
		case 1:
		{
			return CreateGirlName();
			break;
		}
		default:
			break;
	}
	return "";
}

QString CGFunction::CreateBoyName()
{
	std::srand(time(0));
	if (m_createRandomName.size() < 1)
		readCreateRandomNameJson();

	QVariantList xingArray = m_createRandomName.value("xing");

	QVariantList nameArray = m_createRandomName.value("boy");
	auto randomV = std::rand() % 10000 / 10000.0;
	auto xingIndex = std::floor(randomV * xingArray.size());
	auto index = std::floor(randomV * nameArray.size());
	auto xingJson = xingArray.at(xingIndex);
	auto nameJson = nameArray.at(index);
	return xingJson.toString() + nameJson.toString();
}

QString CGFunction::CreateGirlName()
{
	std::srand(time(0));
	if (m_createRandomName.size() < 1)
		readCreateRandomNameJson();
	QVariantList xingArray = m_createRandomName.value("xing");

	QVariantList nameArray = m_createRandomName.value("girl");

	auto xingIndex = std::floor(std::rand() % 10000 / 10000 * xingArray.size());
	auto index = std::floor(std::rand() % 10000 / 10000 * nameArray.size());
	auto xingJson = xingArray.at(xingIndex);
	auto nameJson = nameArray.at(index);
	return xingJson.toString() + nameJson.toString();
}

bool CGFunction::readCreateRandomNameJson()
{
	QString path = ":/Config/Resources/config/name.json"; //QApplication::applicationDirPath() + "//db//name.json";
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << path << "File open failed!";
		return false;
	}
	else
	{
		qDebug() << "File open successfully!";
	}
	QJsonParseError *error = new QJsonParseError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), error);
	if (!jsonDoc.isNull() && (error->error == QJsonParseError::NoError))
	{
		QJsonObject jsonObj = jsonDoc.object();
		if (!jsonObj.isEmpty())
		{
			auto jsonKeyList = jsonObj.keys();
			QVariantList boyArray, grilArray;
			for (auto jsonKey : jsonKeyList)
			{
				QJsonArray jsonArray = jsonObj.value(jsonKey).toArray();
				if (jsonKey.contains("boy"))
				{
					boyArray.append(jsonArray.toVariantList());
				}
				else if (jsonKey.contains("girl"))
				{
					grilArray.append(jsonArray.toVariantList());
				}
				else
					m_createRandomName.insert(jsonKey, jsonArray.toVariantList());
			}
			m_createRandomName.insert("boy", boyArray);
			m_createRandomName.insert("girl", grilArray);
		}
		else
			return false;
	}
	file.close();
	return true;
}

bool CGFunction::readProfessionJson()
{
	QString path = ":/Config/Resources/config/profession.json"; //QApplication::applicationDirPath() + "//db//profession.json";
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << path << "File open failed!";
		return false;
	}
	else
	{
		qDebug() << "File open successfully!";
	}
	QJsonParseError *error = new QJsonParseError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), error);
	if (!jsonDoc.isNull() && (error->error == QJsonParseError::NoError))
	{
		m_professions = jsonDoc.toVariant().toJsonArray();
		/*QJsonObject jsonObj = jsonDoc.object();
		if (!jsonObj.isEmpty())
		{
			m_professions = jsonObj.value("Professions").toArray();
		}*/
	}
	file.close();
	return true;
}

bool CGFunction::readTitleJson()
{
	QString path = ":/Config/Resources/config/title.json"; //QApplication::applicationDirPath() + "//db//title.json";
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << path << "File open failed!";
		return false;
	}
	else
	{
		qDebug() << "File open successfully!";
	}
	QJsonParseError *error = new QJsonParseError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), error);
	if (!jsonDoc.isNull() && (error->error == QJsonParseError::NoError))
	{
		m_titles = jsonDoc.object();
	}
	file.close();
	return true;
}

bool CGFunction::CheckUploadMapData()
{
	if (g_pGameCtrl->GetIsOpenNetToMLAssistTool())
	{
		RpcSocketClient::getInstance().UploadMapData();
		return true;
	}
	return false;
}

QSharedPointer<CGA_NPCDialog_t> CGFunction::WaitRecvHead(int timeout)
{ //600000
	//QEventLoop loop;
	//qDebug() << "WaitRecvHead Start";
	//QTimer timer;
	//timer.setSingleShot(true);
	//auto connection = connect(&timer, &QTimer::timeout, this, [&]()
	//		{
	//			qDebug() << "????????????";
	//			bRet = false;
	//			loop.quit();
	//		});
	//timer.start(timeout);
	//connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	//connect(g_pGameCtrl, &GameCtrl::NotifyNPCDialog, &loop, &QEventLoop::quit);
	//	loop.exec();
	//	disconnect(connection);
	//	qDebug() << "WaitRecvHead Succe";
	auto dlg = WaitRecvNpcDialog(timeout);
	return dlg;
}

//10??????
bool CGFunction::WaitInNormalState(int timeout /*=10000*/)
{
	timeout = timeout == 0 ? 10 * 60 * 1000 : timeout;
	int timeoutNum = timeout / 1000; //10?????? 600??? ??????Sleep 1???
	for (int i = 0; i < timeoutNum; ++i)
	{
		if (m_bStop)
			return false;
		if (IsInNormalState())
			return true;
		//QApplication::processEvents();
		Sleep(1000);
	}
	return false;
}

bool CGFunction::WaitBattleEnd(int timeout /*=30000*/)
{
	bool bEnd = false;
	QEventLoop loop;
	if (timeout > 0) //?????????????????????????????? ?????????????????????
		QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	qDebug() << "WaitBattleEnd Work Start";
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::signal_battleStateEnd, &loop, &QEventLoop::quit);
	loop.exec();
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	qDebug() << "WaitBattleEnd Work End";
	return bEnd;
}

//?????????Npc ????????????10 ???
bool CGFunction::TalkNpc(int x, int y)
{
	int count = 10; //10???
	QSharedPointer<CGA_NPCDialog_t> dlg;
	while (count && !m_bStop && !dlg)
	{
		TurnAboutEx(x, y);
		dlg = WaitRecvNpcDialog();
		count--;
	}
	return dlg ? true : false;
}

bool CGFunction::TalkNpcEx(int dir)
{
	QPoint dirPos = GetDirectionPos(dir, 2);
	return TalkNpc(dirPos.x(), dirPos.y());
}
//???????????? count???npc?????????????????? 2????????? ?????????true false????????????
bool CGFunction::TalkNpcSelectYes(int x, int y, int count)
{
	int talkCount = 3; //?????????????????? ???????????? ???????????? ??????????????????
	QSharedPointer<CGA_NPCDialog_t> dlg;
	while (talkCount && !m_bStop && !dlg)
	{
		TurnAboutEx(x, y);
		dlg = WaitRecvNpcDialog();
		talkCount--;
	}
	//????????????????????????????????????????????????????????????????????? ????????????????????????
	bool bTalkNpc = true;
	bTalkNpc = TalkNpcClicked(dlg, 4);
	count--;
	//count
	while (count && !m_bStop && bTalkNpc)
	{
		dlg = WaitRecvNpcDialog();
		bTalkNpc = TalkNpcClicked(dlg, 4);
		count--;
	}
	return bTalkNpc;
}

bool CGFunction::TalkNpcPosSelectYes(int x, int y, int count /*= 32*/)
{
	qDebug() << "TalkNpcPosSelectYes" << x << y;
	if (!IsNearTarget(x, y, 1))
	{
		MoveToNpcNear(x, y, 1);
	}
	return TalkNpcSelectYes(x, y, count);
}

bool CGFunction::TalkNpcSelectYesEx(int dir, int count /*= 32*/)
{
	QPoint dirPos = GetDirectionPos(dir, 2);
	return TalkNpcSelectYes(dirPos.x(), dirPos.y(), count);
}

bool CGFunction::TalkNpcSelectNo(int x, int y, int count /*= 32*/)
{
	qDebug() << "TalkNpcSelectYes" << x << y;
	//TurnAboutEx(x, y);
	int talkCount = 10; //??????????????????
	QSharedPointer<CGA_NPCDialog_t> dlg;
	while (talkCount && !m_bStop && !dlg)
	{
		TurnAboutEx(x, y);
		dlg = WaitRecvNpcDialog();
		talkCount--;
	}
	//????????????????????????????????????????????????????????????????????? ????????????????????????
	bool bTalkNpc = true;
	bTalkNpc = TalkNpcClicked(dlg, 8);
	count--;

	//count
	while (count && !m_bStop && bTalkNpc)
	{
		auto dlg = WaitRecvNpcDialog();
		bTalkNpc = TalkNpcClicked(dlg, 8);
		count--;
	}
	return bTalkNpc;
}

bool CGFunction::TalkNpcPosSelectNo(int x, int y, int count /*= 32*/)
{
	qDebug() << "TalkNpcPosSelectNo" << x << y;
	if (!IsNearTarget(x, y, 1))
	{
		MoveToNpcNear(x, y, 1);
	}
	return TalkNpcSelectNo(x, y, count);
}

bool CGFunction::TalkNpcSelectNoEx(int dir, int count /*= 32*/)
{
	QPoint dirPos = GetDirectionPos(dir, 2);
	return TalkNpcSelectNo(dirPos.x(), dirPos.y(), count);
}

//selectVal 4 ????????????
//selectVal 8 ????????????
bool CGFunction::TalkNpcClicked(QSharedPointer<CGA_NPCDialog_t> dlg, int selectVal)
{
	bool result = false;
	if (!dlg)
	{ //?????? ?????????????????????????????? ?????????????????????
		g_CGAInterface->ClickNPCDialog(selectVal, -1, result);
		return false;
	}
	switch (dlg->options)
	{
		case 12:
		{
			g_CGAInterface->ClickNPCDialog(selectVal, -1, result);
			return true;
		}
		case 32:
		{
			g_CGAInterface->ClickNPCDialog(32, -1, result);
			return true;
		}
		case 1:
		{
			g_CGAInterface->ClickNPCDialog(1, -1, result);
			return true;
		}
		case 2:
		{
			g_CGAInterface->ClickNPCDialog(2, -1, result);
			return true;
		}
		case 3:
		{
			g_CGAInterface->ClickNPCDialog(1, -1, result);
			return true;
		}
		case 4:
		{
			g_CGAInterface->ClickNPCDialog(4, -1, result);
			return true;
		}
		case 8:
		{
			g_CGAInterface->ClickNPCDialog(8, -1, result);
			return true;
		}
		case 0:
		{
			return true;
		}
		default:
			break;
	}
	return false;
}

std::tuple<int, QString> CGFunction::WaitSysMsg(int timeout)
{
	QString sysMsg;
	int retunitid = 0;
	QEventLoop loop;
	if (timeout > 0) //?????????????????????????????? ?????????????????????
		QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	connect(g_pGameCtrl, &GameCtrl::NotifyChatMsg, &loop, &QEventLoop::quit);
	loop.exec();
	auto retMsg = g_pGameFun->GetJustChatMsg();
	if (retMsg.size() >= 2)
	{
		retunitid = retMsg[0].toInt();
		sysMsg = retMsg[1];
	}
	return std::make_tuple(retunitid, sysMsg);
}

std::tuple<int, QString> CGFunction::WaitChatMsg(int timeout /*= 5000*/)
{
	QString sysMsg;
	int retunitid = 0;
	QMutex mutex;
	QEventLoop loop;
	if (timeout > 0) //?????????????????????????????? ?????????????????????
		QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	connect(g_pGameCtrl, &GameCtrl::NotifyChatMsg, &loop, &QEventLoop::quit);
	loop.exec();
	auto retMsg = g_pGameFun->GetJustChatMsg();
	if (retMsg.size() >= 2)
	{
		retunitid = retMsg[0].toInt();
		sysMsg = retMsg[1];
	}
	return std::make_tuple(retunitid, sysMsg);
}

std::tuple<int, QString> CGFunction::WaitSysAndChatMsg(int timeout /*= 5000*/)
{
	QString sysMsg;
	int retunitid = 0;
	QEventLoop loop;
	if (timeout > 0) //?????????????????????????????? ?????????????????????
		QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	connect(g_pGameCtrl, &GameCtrl::NotifyChatMsg, &loop, &QEventLoop::quit);
	loop.exec();
	auto retMsg = g_pGameFun->GetJustChatMsg();
	if (retMsg.size() >= 2)
	{
		retunitid = retMsg[0].toInt();
		sysMsg = retMsg[1];
	}
	return std::make_tuple(retunitid, sysMsg);
}
//??????????????????
std::tuple<QString, QString> CGFunction::WaitSubscribeMsg(int lastInterval,int timeout /*= 5000*/)
{
	QString retMsg;
	QString rettopic = 0;
	QEventLoop loop;
	if (timeout > 0) //?????????????????????????????? ?????????????????????
		QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	connect(&ITObjectDataMgr::getInstance(), &ITObjectDataMgr::signal_mqttMsg, &loop, &QEventLoop::quit);
	loop.exec();
	QStringList sRecvMsg = ITObjectDataMgr::getInstance().GetLastPublishMsg(lastInterval);
	if (sRecvMsg.size() >= 2)
	{
		retMsg = sRecvMsg[1];
		rettopic = sRecvMsg[0];
	}
	return std::make_tuple(rettopic, retMsg);
}

bool CGFunction::ContainChatMsg(const QString &cue)
{
	if (m_chatMsgList.size() > 0)
	{
		QStringList sMsg = m_chatMsgList.last().second;
		if (sMsg.size() < 2)
			return false;
		return sMsg[1].contains(cue);
	}
	return false;
}
//????????????????????????
QString CGFunction::GetSysChatMsg()
{
	if (m_systemCueList.size() > 0)
	{
		QString sMsg;
		QMutexLocker locker(&m_charMutex);
		for (int i = 0; i < m_systemCueList.size(); ++i)
		{
			sMsg.append(m_systemCueList[i].second);
			sMsg += ";";
		}
		return sMsg;
	}
	return "";
}

QString CGFunction::GetLastSysChatMsg()
{
	QMutexLocker lock(&m_charMutex);
	if (m_systemCueList.size() > 0)
		return m_systemCueList.last().second;
	return "";
}

QString CGFunction::GetAllChatMsg(int count)
{
	if (m_chatMsgList.size() < 1)
		return "";

	if (count == 0)
	{
		QString sMsg;
		QMutexLocker locker(&m_charMutex);
		for (int i = 0; i < m_chatMsgList.size(); ++i)
		{
			sMsg.append(m_chatMsgList[i].second[1]);
			sMsg += ";";
		}
		return sMsg;
	}
	else
	{
		int tmpCount = count;
		QString sMsg;
		if (m_chatMsgList.size() < tmpCount)
			tmpCount = m_chatMsgList.size();
		QMutexLocker locker(&m_charMutex);
		int i = m_chatMsgList.size() - tmpCount;
		for (; i < m_chatMsgList.size(); ++i)
		{
			sMsg.append(m_chatMsgList[i].second[1]);
			sMsg += ";";
		}
		return sMsg;
	}
}

QList<QPair<int, QString> > CGFunction::GetDetailAllChatMsg(int count /*= 0*/)
{
	QList<QPair<int, QString> > detailMsg;
	if (m_chatMsgList.size() < 1)
		return detailMsg;

	if (count == 0)
	{
		count = m_chatMsgList.size();
	}
	//else
	{
		QMutexLocker locker(&m_charMutex);
		int tmpCount = count;
		if (m_chatMsgList.size() < tmpCount)
			tmpCount = m_chatMsgList.size();
		int i = m_chatMsgList.size() - tmpCount;
		for (; i < m_chatMsgList.size(); ++i)
		{
			detailMsg.append(qMakePair<int, QString>(m_chatMsgList[i].second[0].toInt(), m_chatMsgList[i].second[1]));
		}
		return detailMsg;
	}
}

QStringList CGFunction::GetJustChatMsg()
{
	if (m_chatMsgList.size() < 1)
		return QStringList();
	QMutexLocker locker(&m_charMutex);
	for (int i = (m_chatMsgList.size() - 1); i > 0; i--)
	{
		auto lastData = m_chatMsgList[i];
		if ((GetTickCount() - lastData.first) > 3000)
			return QStringList();
		if (lastData.second[0] == -1) //???????????? ??????
			continue;
		else
			return lastData.second;
	}
	return QStringList();
}

QSharedPointer<CGA_NPCDialog_t> CGFunction::WaitRecvNpcDialog(int timeout /*=5000*/)
{
	QSharedPointer<CGA_NPCDialog_t> resNpcDialog;
	QEventLoop loop;
	qDebug() << "WaitRecvHead Work Start";
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyNPCDialog, &loop, &QEventLoop::quit);
	loop.exec();
	resNpcDialog = g_pGameCtrl->GetLastNpcDialog();
	qDebug() << "WaitRecvHead Work End";
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return resNpcDialog;
}

QSharedPointer<CGA::cga_working_result_t> CGFunction::WaitRecvWorkResult(int timeout /*= 9000*/)
{
	QSharedPointer<CGA::cga_working_result_t> recvReslut;
	QEventLoop loop;
	qDebug() << "WaitRecvWorkResult Work Start";
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyWorkingResult, &loop, &QEventLoop::quit);
	loop.exec();
	recvReslut = g_pGameCtrl->GetLastWorkResult();
	qDebug() << "WaitRecvWorkResult Work End";
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return recvReslut;
}

QSharedPointer<CGA::cga_player_menu_items_t> CGFunction::WaitRecvPlayerMenu(int timeout /*= 5000*/)
{
	QSharedPointer<CGA::cga_player_menu_items_t> reqMenu;
	QEventLoop loop;
	qDebug() << "WaitRecvPlayerMenu Start";
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyPlayerMenu, &loop, &QEventLoop::quit);
	loop.exec();
	reqMenu = g_pGameCtrl->GetLastPlayerMenuResult();
	qDebug() << "WaitRecvPlayerMenu End";
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return reqMenu;
}
QSharedPointer<CGA::cga_unit_menu_items_t> CGFunction::WaitRecvPlayerMenuUnit(int timeout /*= 5000*/)
{
	QSharedPointer<CGA::cga_unit_menu_items_t> reqMenu;
	QEventLoop loop;
	qDebug() << "WaitRecvPlayerMenuUnit Start";
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyUnitMenu, &loop, &QEventLoop::quit);
	loop.exec();
	reqMenu = g_pGameCtrl->GetLastUnitMenuResult();
	qDebug() << "WaitRecvPlayerMenuUnit End";
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return reqMenu;
}

int CGFunction::WaitRecvBattleAction(int timeout /*= 5000*/)
{
	int flags = 0;
	QEventLoop loop;
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyBattleAction, &loop, &QEventLoop::quit);
	loop.exec();
	flags = g_pGameCtrl->GetLastBattleActionResult();
	qDebug() << "WaitRecvBattleAction End";
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return flags;
}

int CGFunction::WaitRecvGameWndKeyDown(int timeout /*= 5000*/)
{
	int flags = 0;
	QEventLoop loop;
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyGameWndKeyDown, [&](unsigned int keyVal)
			{
				flags = keyVal;
				if (loop.isRunning())
					loop.quit();
			});
	loop.exec();
	qDebug() << "WaitRecvGameWndKeyDown End";
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return flags;
}

int CGFunction::WaitRecvConnectionState(int timeout /*= 5000*/)
{
	int flags = 0;
	QEventLoop loop;
	qDebug() << "WaitRecvConnectionState Start";
	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyConnectionState, &loop, &QEventLoop::quit);
	loop.exec();
	auto recvState = g_pGameCtrl->GetLastConnectStateResult();
	if (recvState)
		flags = recvState->state;
	qDebug() << "WaitRecvConnectionState End";
	QObject::disconnect(connection); //??????Connection ??????lambda?????????
	return flags;
}

//int CGFunction::WaitRecvServerShutdown(int timeout /*= 5000*/)
//{
//	int flags = 0;
//	QEventLoop loop;
//	QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
//	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
//	auto connection = connect(g_pGameCtrl, &GameCtrl::NotifyGameWndKeyDown, [&](unsigned int keyVal)
//			{
//				flags = keyVal;
//				loop.quit();
//			});
//	loop.exec();
//	qDebug() << "WaitRecvGameWndKeyDown End";
//	QObject::disconnect(connection); //??????Connection ??????lambda?????????
//	return flags;
//}

bool CGFunction::Nowhile(const QString &sName)
{
	auto curMapName = GetMapName();
	auto curMapNum = GetMapIndex();
	int timeout = 60; //10?????? 600??? ??????Sleep 1???	???60???
	for (int i = 0; i < timeout; ++i)
	{
		if (m_bStop)
			return false;
		curMapName = GetMapName();
		curMapNum = GetMapIndex();
		if (IsInNormalState() && (curMapName == sName || curMapNum == sName.toInt()))
		{
			return true;
		}
		//QApplication::processEvents();
		Sleep(1000);
	}
	return true;
}

bool CGFunction::NowhileEx(const QString &sName, int x, int y)
{
	auto curMapName = GetMapName();
	auto curPoint = GetMapCoordinate();
	int timeout = 60; //10?????? 600??? ??????Sleep 1???
	for (int i = 0; i < timeout; ++i)
	{
		if (m_bStop)
			return false;
		curMapName = GetMapName();
		curPoint = GetMapCoordinate();
		if (curMapName == sName && curPoint.x() == x && curPoint.y() == y)
		{
			return true;
		}
		//QApplication::processEvents();
		Sleep(1000);
	}
	return true;
}

bool CGFunction::NowhileMap(int mapNum, int timeout)
{
	if (timeout < 1000)
		timeout = 5000;
	timeout = timeout / 1000;
	auto curMapIndex = GetMapIndex();
	for (int i = 0; i < timeout; ++i)
	{
		if (m_bStop)
			return false;
		curMapIndex = GetMapIndex();
		if (IsInNormalState() && curMapIndex == mapNum)
		{
			return true;
		}
		//QApplication::processEvents();
		Sleep(1000);
	}
	return true;
}

bool CGFunction::NowhileMapEx(int mapNum, int x, int y, int timeout /*= 5*/)
{
	if (timeout < 1000)
		timeout = 5000;
	timeout = timeout / 1000;
	auto curMapIndex = GetMapIndex();
	auto curPoint = GetMapCoordinate();
	//int timeout = timeout; //5??? 10?????? 600??? ??????Sleep 1???
	for (int i = 0; i < timeout; ++i)
	{
		if (m_bStop)
			return false;
		curMapIndex = GetMapIndex();
		curPoint = GetMapCoordinate();
		if (curMapIndex == mapNum && curPoint.x() == x && curPoint.y() == y)
		{
			return true;
		}
		//QApplication::processEvents();
		Sleep(1000);
	}
	return true;
}
//???????????????????????????????????????????????????
/*
QString sText = "?????????????????????1???";
	int nFloor = g_pGameFun->GetMapFloorNumberFromName(sText);//1
	sText = "?????????????????????32???";
	nFloor = g_pGameFun->GetMapFloorNumberFromName(sText);//32
	sText = "???2??????????????????54???";
	nFloor = g_pGameFun->GetMapFloorNumberFromName(sText);//254
	sText = "???2??????????????????54???";
	nFloor = g_pGameFun->GetMapFloorNumberFromName(sText, true); //2
	sText = "???2??????????????????54???";
	nFloor = g_pGameFun->GetMapFloorNumberFromName(sText, true, true); //54
*/
int CGFunction::GetNumberFromName(const QString &sMapName, bool bSerial, bool bBack)
{
	QString sData;
	if (bSerial) //??????????????????  ?????????
	{
		bool bFind = false;
		QStringList sDigitList;
		foreach (QChar szChar, sMapName)
		{
			if (szChar.isDigit())
			{
				bFind = true;
				sData.append(szChar);
			}
			else
			{
				if (bFind) //???????????? ???????????????????????????????????????
				{
					sDigitList.append(sData);
					sData.clear();
					if (!bBack) //????????????????????????break	?????? ?????????????????????list
					{
						break;
					}
				}
			}
		}
		if (sDigitList.size() < 1)
			return sData.toInt();
		sData = sDigitList.last();
	}
	else
	{
		foreach (QChar szChar, sMapName)
		{
			if (szChar.isDigit())
				sData.append(szChar);
		}
	}
	return sData.toInt();
}

int CGFunction::GetMapFloorNumberFromName(bool bSerial /*= false*/, bool bBack /*= false*/)
{
	QString sMapName = GetMapName();
	return GetNumberFromName(sMapName, bSerial, bBack);
}

bool CGFunction::LoadOffLineMapImageData(int index1, int index2, int index3, QImage &mapImage)
{
	CGA::cga_map_cells_t cells;
	g_CGAInterface->GetMapCollisionTable(true, cells);
	QString sPath = QCoreApplication::applicationDirPath() + "//map//";
	if (index1 == 0)
		sPath = QString("%1/%2/%3.bmp").arg(sPath).arg(index1).arg(index3);
	else
	{
		if (!g_pGameCtrl->GetIsOpenNetToMLAssistTool() || !g_pGameCtrl->GetIsOpenSyncMap())
			return false;		
		sPath = QString("%1/%2/%3/%4.bmp").arg(sPath).arg(index1).arg(index2).arg(index3);
	}

	if (QFile::exists(sPath) == false)
	{
		qDebug() << "???????????????????????????????????????????????????";
		return false;
	}
	mapImage.load(sPath);
	if (mapImage.width() <= 0 || mapImage.height() <= 0)
	{
		qDebug() << "??????????????????????????????????????????";
		return false;
	}
	if (mapImage.width() != cells.x_size || mapImage.height() != cells.y_size)
	{
		qDebug() << "????????????????????????????????????????????????????????????";
		return false;
	}
	return true;
}

bool CGFunction::LoadOffLineMapImageData(QImage &mapImage)
{
	int index1, index2, index3;
	std::string sfilemap;
	g_CGAInterface->GetMapIndex(index1, index2, index3, sfilemap);
	return LoadOffLineMapImageData(index1, index2, index3, mapImage);
}

void CGFunction::Gesture(int index)
{
	if (IsInNormalState())
	{
		g_CGAInterface->PlayGesture(index);
	}
}
