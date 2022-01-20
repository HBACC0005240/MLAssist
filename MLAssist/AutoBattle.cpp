#include "AutoBattle.h"
#include "BattleModuleRegisty.h"
#include <QDateTime>
#include <QTimer>
extern CGA::CGAInterface *g_CGAInterface;

CBattleWorker::CBattleWorker()
{

	m_iDelayFrom = 0;
	m_iDelayTo = 0;
	m_LastWarpMap202 = 0;
	connect(g_pGameCtrl, &GameCtrl::NotifyGameSkillsInfo, this, &CBattleWorker::OnNotifyGetSkillsInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGameItemsInfo, this, &CBattleWorker::OnNotifyGetItemsInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGamePetsInfo, this, &CBattleWorker::OnNotifyGetPetsInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGameBattlesInfo, this, &CBattleWorker::OnNotifyGetBattleInfo, Qt::ConnectionType::QueuedConnection);
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(OnLockCountdown()));
	timer->start(500);
	init();
}

CBattleWorker *CBattleWorker::getInstace()
{
	static CBattleWorker ins;
	return &ins;
}
#include <QStandardItemModel>
void CBattleWorker::init()
{
	//初始化T字形站位数据
	/*  站位对应pos 
    14  12  10  11  13
    19  17  15  16  18
    9   7   5   6   8
    4   2   0   1   3
    */
	//int rowCount = 2;
	//int colCount = 5;
	//QStandardItemModel model;
	//for (int i=0;i<rowCount;++i)
	//{
	//    int nVal = colCount * i;    //行最小数0 5       10 15
	//    for (size_t n = 0; n < colCount; n++)
	//    {
	//        int nCenterIndex = colCount / 2;    //列除2
	//        int cellVal =
	//        QStandardItem* pItem = new QStandardItem(i,n);
	//        pItem->setData(1, Qt::UserRole);
	//    }
	//}
	m_battleTPos.insert(0, QList<int>({ 1, 2, 5 }));
	m_battleTPos.insert(1, QList<int>({ 0, 3, 6 }));
	m_battleTPos.insert(2, QList<int>({ 0, 4, 7 }));
	m_battleTPos.insert(3, QList<int>({ 1, 8 }));
	m_battleTPos.insert(4, QList<int>({ 2, 9 }));
	m_battleTPos.insert(5, QList<int>({ 0, 6, 7 }));
	m_battleTPos.insert(6, QList<int>({ 1, 5, 8 }));
	m_battleTPos.insert(7, QList<int>({ 2, 5, 9 }));
	m_battleTPos.insert(8, QList<int>({ 3, 6 }));
	m_battleTPos.insert(9, QList<int>({ 4, 7 }));
	m_battleTPos.insert(10, QList<int>({ 11, 12, 15 }));
	m_battleTPos.insert(11, QList<int>({ 10, 13, 16 }));
	m_battleTPos.insert(12, QList<int>({ 10, 14, 17 }));
	m_battleTPos.insert(13, QList<int>({ 11, 18 }));
	m_battleTPos.insert(14, QList<int>({ 12, 19 }));
	m_battleTPos.insert(15, QList<int>({ 10, 16, 17 }));
	m_battleTPos.insert(16, QList<int>({ 11, 15, 18 }));
	m_battleTPos.insert(17, QList<int>({ 12, 15, 19 }));
	m_battleTPos.insert(18, QList<int>({ 13, 16 }));
	m_battleTPos.insert(19, QList<int>({ 14, 17 }));

	//m_devTypeText.insert(dtCondition_Ignore, "忽略");
	//m_devTypeText.insert(dtCondition_EnemyCount, "敌人数");
	//m_devTypeText.insert(dtCondition_TeammateCount, "队伍数");
	//m_devTypeText.insert(dtCondition_EnemySingleRowCount, "每行敌人数");
	//m_devTypeText.insert(dtCondition_PlayerHp, "人血");
	//m_devTypeText.insert(dtCondition_PlayerMp, "人魔");
	//m_devTypeText.insert(dtCondition_PetHp, "宠血");
	//m_devTypeText.insert(dtCondition_PetMp, "宠魔");
	//m_devTypeText.insert(dtCondition_TeammateHp, "队友血");
	//m_devTypeText.insert(dtCondition_TeammateMp, "队友魔");
	//m_devTypeText.insert(dtCondition_EnemyMultiTargetHp, "地方T血量");
	//m_devTypeText.insert(dtCondition_TeammateMultiTargetHp, "己方T血量");
	//m_devTypeText.insert(dtCondition_EnemyAllHp, "地方全体血量");
	//m_devTypeText.insert(dtCondition_TeammateAllHp, "己方全体血量");
	//m_devTypeText.insert(dtCondition_EnemyUnit, "敌人单位");
	//m_devTypeText.insert(dtCondition_Round, "回合");
	//m_devTypeText.insert(dtCondition_DoubleAction, "二动");
	//m_devTypeText.insert(dtCondition_TeammateDebuff, "敌人减益");
	//m_devTypeText.insert(dtCondition_EnemyLevel, "敌人等级");
	//m_devTypeText.insert(dtCondition_EnemyAvgLevel, "敌人平均等级");
	//m_devTypeText.insert(dtCondition_InventoryItem, "忽略");

	//m_devTypeText.insert(dtAction_PlayerAttack, "攻击");
	//m_devTypeText.insert(dtAction_PlayerGuard, "防御");
	//m_devTypeText.insert(dtAction_PlayerEscape, "逃跑");
	//m_devTypeText.insert(dtAction_PlayerExchangePosition, "换位");
	//m_devTypeText.insert(dtAction_PlayerChangePet, "换宠");
	//m_devTypeText.insert(dtAction_PlayerUseItem, "使用物品");
	////m_devTypeText.insert(dtAction_PlayerSkillAttack, "技能攻击");
	//m_devTypeText.insert(dtAction_PlayerLogBack, "登出");
	//m_devTypeText.insert(dtAction_PlayerDoNothing, "什么也不做");

	//m_devTypeText.insert(dtAction_PetDoNothing, "什么也不做");

	//m_devTypeText.insert(dtTarget_Enemy, "敌人");
	//m_devTypeText.insert(dtTarget_Teammate, "队友");
	//m_devTypeText.insert(dtTarget_Self, "人物");
	//m_devTypeText.insert(dtTarget_Pet, "宠物");

	//m_devTypeText.insert(dtTargetCondition_Random, "随机");
	//m_devTypeText.insert(dtTargetCondition_Front, "前排");
	//m_devTypeText.insert(dtTargetCondition_Back, "后排");
	//m_devTypeText.insert(dtTargetCondition_LowHP, "血少优先");
	//m_devTypeText.insert(dtTargetCondition_HighHP, "血多优先");
	//m_devTypeText.insert(dtTargetCondition_LowHPPercent, "低血量比例");
	//m_devTypeText.insert(dtTargetCondition_HighHPPercent, "高血量比例");
	//m_devTypeText.insert(dtTargetCondition_LowLv, "级低优先");
	//m_devTypeText.insert(dtTargetCondition_HighLv, "级高优先");
	//m_devTypeText.insert(dtTargetCondition_SingleDebuff, "单体异常状态");
	//m_devTypeText.insert(dtTargetCondition_MulTDebuff, "T字形异常状态");
	//m_devTypeText.insert(dtTargetCondition_Goatfarm, "砍牛");
	//m_devTypeText.insert(dtTargetCondition_Boomerang, "回力标(人多的一排)");
	//m_devTypeText.insert(dtTargetCondition_LessUnitRow, "人少的一排");
	//m_devTypeText.insert(dtTargetCondition_MultiMagic, "强力魔法");
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_Ignore), NEW_MODULE_FACTORY(CBattleCondition_Ignore));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyCount), NEW_MODULE_FACTORY(CBattleCondition_EnemyCount));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_TeammateCount), NEW_MODULE_FACTORY(CBattleCondition_TeammateCount));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemySingleRowCount), NEW_MODULE_FACTORY(CBattleCondition_EnemySingleRowCount));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_PlayerHp), NEW_MODULE_FACTORY(CBattleCondition_PlayerHp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_PlayerMp), NEW_MODULE_FACTORY(CBattleCondition_PlayerMp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_PetHp), NEW_MODULE_FACTORY(CBattleCondition_PetHp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_PetMp), NEW_MODULE_FACTORY(CBattleCondition_PetMp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_TeammateHp), NEW_MODULE_FACTORY(CBattleCondition_TeammateHp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_TeammateMp), NEW_MODULE_FACTORY(CBattleCondition_TeammateMp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyMultiTargetHp), NEW_MODULE_FACTORY(CBattleCondition_EnemyMultiTargetHp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_TeammateMultiTargetHp), NEW_MODULE_FACTORY(CBattleCondition_TeammateMultiTargetHp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyAllHp), NEW_MODULE_FACTORY(CBattleCondition_EnemyAllHp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_TeammateAllHp), NEW_MODULE_FACTORY(CBattleCondition_TeammateAllHp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyUnit), NEW_MODULE_FACTORY(CBattleCondition_EnemyUnit));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_TeammateUnit), NEW_MODULE_FACTORY(CBattleCondition_TeammateUnit));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_Round), NEW_MODULE_FACTORY(CBattleCondition_Round));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_DoubleAction), NEW_MODULE_FACTORY(CBattleCondition_DoubleAction));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_TeammateDebuff), NEW_MODULE_FACTORY(CBattleCondition_TeammateDebuff));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyLevel), NEW_MODULE_FACTORY(CBattleCondition_EnemyLevel));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyLv1Hp), NEW_MODULE_FACTORY(CBattleCondition_EnemyLv1Hp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyLv1MaxHp), NEW_MODULE_FACTORY(CBattleCondition_EnemyLv1MaxHp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyLv1MaxMp), NEW_MODULE_FACTORY(CBattleCondition_EnemyLv1MaxMp));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyType), NEW_MODULE_FACTORY(CBattleCondition_EnemyType));

	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_EnemyAvgLevel), NEW_MODULE_FACTORY(CBattleCondition_EnemyAvgLevel));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_InventoryItem), NEW_MODULE_FACTORY(CBattleCondition_InventoryItem));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerAttack), NEW_MODULE_FACTORY(CBattleAction_PlayerAttack));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerGuard), NEW_MODULE_FACTORY(CBattleAction_PlayerGuard));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerEscape), NEW_MODULE_FACTORY(CBattleAction_PlayerEscape));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerExchangePosition), NEW_MODULE_FACTORY(CBattleAction_PlayerExchangePosition));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerChangePet), NEW_MODULE_FACTORY(CBattleAction_PlayerChangePet));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerSkillAttack), NEW_MODULE_FACTORY(CBattleAction_PlayerSkillAttack));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerUseItem), NEW_MODULE_FACTORY(CBattleAction_PlayerUseItem));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerLogBack), NEW_MODULE_FACTORY(CBattleAction_PlayerLogBack));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerRebirth), NEW_MODULE_FACTORY(CBattleAction_PlayerRebirth));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PlayerDoNothing), NEW_MODULE_FACTORY(CBattleAction_PlayerDoNothing));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PetDoNothing), NEW_MODULE_FACTORY(CBattleAction_PetDoNothing));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtAction_PetSkillAttack), NEW_MODULE_FACTORY(CBattleAction_PetSkillAttack));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtTarget_Self), NEW_MODULE_FACTORY(CBattleTarget_Self));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtTarget_Pet), NEW_MODULE_FACTORY(CBattleTarget_Pet));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtTarget_Enemy), NEW_MODULE_FACTORY(CBattleTarget_Enemy));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtTarget_Teammate), NEW_MODULE_FACTORY(CBattleTarget_Teammate));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtTarget_Condition), NEW_MODULE_FACTORY(CBattleTarget_Condition));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_PlayerName), NEW_MODULE_FACTORY(CBattleCondition_PlayerName));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_PlayerJob), NEW_MODULE_FACTORY(CBattleCondition_PlayerJob));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_PlayerGold), NEW_MODULE_FACTORY(CBattleCondition_PlayerGold));
	BattleModuleRegisty::GetInstance().RegisterModuleFactory(QString::number(dtCondition_PlayerBGM), NEW_MODULE_FACTORY(CBattleCondition_BattleBGM));

	m_battleMapText.insert(dtCondition_Ignore, "忽略");
	m_battleMapText.insert(dtCondition_EnemyCount, "敌人数");
	m_battleMapText.insert(dtCondition_TeammateCount, "队伍数");
	m_battleMapText.insert(dtCondition_EnemySingleRowCount, "每行敌人数");
	m_battleMapText.insert(dtCondition_PlayerHp, "人血");
	m_battleMapText.insert(dtCondition_PlayerMp, "人魔");
	m_battleMapText.insert(dtCondition_PetHp, "宠血");
	m_battleMapText.insert(dtCondition_PetMp, "宠魔");
	m_battleMapText.insert(dtCondition_TeammateHp, "队友血");
	m_battleMapText.insert(dtCondition_TeammateMp, "队友魔");
	m_battleMapText.insert(dtCondition_EnemyMultiTargetHp, "敌方T血量");
	m_battleMapText.insert(dtCondition_TeammateMultiTargetHp, "己方T血量");
	m_battleMapText.insert(dtCondition_EnemyAllHp, "敌方全体血量");
	m_battleMapText.insert(dtCondition_TeammateAllHp, "己方全体血量");
	m_battleMapText.insert(dtCondition_EnemyUnit, "敌方单位");
	m_battleMapText.insert(dtCondition_Round, "回合");
	m_battleMapText.insert(dtCondition_DoubleAction, "二动");
	m_battleMapText.insert(dtCondition_TeammateDebuff, "己方异常状态");
	m_battleMapText.insert(dtCondition_EnemyLevel, "敌人等级");
	m_battleMapText.insert(dtCondition_EnemyLv1Hp, "敌方1级单位个体血量");
	m_battleMapText.insert(dtCondition_EnemyLv1MaxHp, "敌方1级单位个体最大血量");
	m_battleMapText.insert(dtCondition_EnemyLv1MaxMp, "敌方1级单位个体最大魔量");
	m_battleMapText.insert(dtCondition_EnemyType, "敌方类型");

	m_battleMapText.insert(dtCondition_EnemyAvgLevel, "敌人平均等级");
	m_battleMapText.insert(dtCondition_InventoryItem, "物品");

	m_battleMapText.insert(dtCompare, "判断");
	m_battleMapText.insert(dtCompare_Equal, "等于");
	m_battleMapText.insert(dtCompare_NotEqual, "不等于");
	m_battleMapText.insert(dtCompare_GreaterThan, "大于");
	m_battleMapText.insert(dtCompare_LessThan, "小于");
	m_battleMapText.insert(dtCompare_GreaterEqual, "大于等于");
	m_battleMapText.insert(dtCompare_LessEqual, "小于等于");
	m_battleMapText.insert(dtCompare_Contain, "包含");
	m_battleMapText.insert(dtCompare_NotContain, "不包含");

	m_battleMapText.insert(dtAction_PlayerAttack, "人物攻击");
	m_battleMapText.insert(dtAction_PlayerGuard, "人物防御");
	m_battleMapText.insert(dtAction_PlayerEscape, "人物逃跑");
	m_battleMapText.insert(dtAction_PlayerExchangePosition, "人物交换位置");
	m_battleMapText.insert(dtAction_PlayerChangePet, "人物交换宠物");
	m_battleMapText.insert(dtAction_PlayerSkillAttack, "人物使用技能");
	m_battleMapText.insert(dtAction_PlayerUseItem, "人物使用物品");
	m_battleMapText.insert(dtAction_PlayerLogBack, "人物登出");
	m_battleMapText.insert(dtAction_PlayerRebirth, "人物气绝");
	m_battleMapText.insert(dtAction_PlayerDoNothing, "人物什么也不做");
	m_battleMapText.insert(dtAction_PetDoNothing, "宠物什么也不做");
	m_battleMapText.insert(dtAction_PetSkillAttack, "宠物使用技能");
	m_battleMapText.insert(dtTarget_Self, "人物");
	m_battleMapText.insert(dtTarget_Pet, "宠物");
	m_battleMapText.insert(dtTarget_Enemy, "敌方");
	m_battleMapText.insert(dtTarget_Teammate, "队友");
	m_battleMapText.insert(dtTarget_Condition, "条件");

	m_battleMapText.insert(dtTargetCondition_Random, "随机");
	m_battleMapText.insert(dtTargetCondition_Front, "前排");
	m_battleMapText.insert(dtTargetCondition_Back, "后排");
	m_battleMapText.insert(dtTargetCondition_LowHP, "血少优先");
	m_battleMapText.insert(dtTargetCondition_HighHP, "血多优先");
	m_battleMapText.insert(dtTargetCondition_LowHPPercent, "低血量比例");
	m_battleMapText.insert(dtTargetCondition_HighHPPercent, "高血量比例");
	m_battleMapText.insert(dtTargetCondition_LowLv, "级低优先");
	m_battleMapText.insert(dtTargetCondition_HighLv, "级高优先");
	m_battleMapText.insert(dtTargetCondition_SingleDebuff, "单体异常状态");
	m_battleMapText.insert(dtTargetCondition_MulTDebuff, "T字形异常状态");
	m_battleMapText.insert(dtTargetCondition_Goatfarm, "砍牛");
	m_battleMapText.insert(dtTargetCondition_Boomerang, "回力标(人多的一排)");
	m_battleMapText.insert(dtTargetCondition_LessUnitRow, "人少的一排");
	m_battleMapText.insert(dtTargetCondition_MultiMagic, "强力魔法");
	m_devTypeText = m_battleMapText;
}

bool CBattleWorker::OnBattleAction(int flags)
{
	if ((flags & FL_BATTLE_ACTION_END) || (flags & FL_BATTLE_ACTION_BEGIN))
	{														//战斗开始或结束
		m_BattleContext.m_iLastRound = -1;					//当前回合数重置
		m_BattleContext.m_bRoundZeroNotified = false;		//是否第0回合 就是第1回合
		m_BattleContext.m_bIsPlayerActionPerformed = false; //是否该人物动作
		m_BattleContext.m_bIsPlayerEscaped = false;			//人物是否逃跑
		return true;
	}
	else
	{
		if (m_bWaitAfterBattle)
		{
			g_CGAInterface->SetSwitchAnimForceWait(1, m_nBattleDelay*1000);
		}
	}
	int gameStatus = 0;
	if (!g_CGAInterface->GetGameStatus(gameStatus) && gameStatus != 10)
		return false;

	CGA::cga_battle_context_t ctx;

	if (!g_CGAInterface->GetBattleContext(ctx))
		return false;

	m_BattleContext.m_iPlayerPosition = ctx.player_pos; //人物站位
	m_BattleContext.m_iPlayerStatus = ctx.player_status;
	m_BattleContext.m_iRoundCount = ctx.round_count; //回合数
	m_BattleContext.m_iPetId = ctx.petid;			 //宠物id
	m_BattleContext.m_iWeaponAllowBit = ctx.weapon_allowbit;
	m_BattleContext.m_iSkillAllowBit = ctx.skill_allowbit;
	m_BattleContext.m_iPetSkillAllowBit = ctx.petskill_allowbit; //
	m_BattleContext.m_nCurDoubleAction = ctx.skill_performed;	 //0一动 1二动 2等待动画
	//qDebug() << "player_pos" << ctx.player_pos << "player_status" << ctx.player_status << "round_count"
	//		 << ctx.round_count << "petid" << ctx.petid << "weapon_allowbit" << ctx.weapon_allowbit << "skill_allowbit"
	//		 << ctx.skill_allowbit << "petskill_allowbit" << ctx.petskill_allowbit; //
	if (m_BattleContext.m_iPlayerStatus != 1 && m_BattleContext.m_iPlayerStatus != 4)
	{
		//qDebug("m_iPlayerStatus =%d", m_BattleContext.m_iPlayerStatus);
		return false;
	}

	m_BattleContext.m_bIsPlayer = (flags & FL_BATTLE_ACTION_ISPLAYER) ? true : false; //是否人物
	m_BattleContext.m_bIsDouble = (flags & FL_BATTLE_ACTION_ISDOUBLE) ? true : false; //人物是否二动
	//只通过这个判断 不准  增加宠物判断
	if (ctx.petid < 0)
	{
		m_BattleContext.m_bIsDouble = true;
	}
	m_BattleContext.m_bIsSkillPerformed = true; //(flags & FL_BATTLE_ACTION_ISSKILLPERFORMED) ? true : false; //是否释放技能 这个判断也不准 通过外部设置
	if (m_BattleContext.m_bIsDouble)			//人物第一动使用攻防逃
	{
		if (m_BattleContext.m_bIsFirstDouble && m_BattleContext.m_nCurDoubleAction == 0) //第一动使用
		{
			m_BattleContext.m_bIsSkillPerformed = false; //屏蔽使用技能
			m_BattleContext.m_bIsDouble = true;			 //再次声明是二动
		}
		else if (!m_BattleContext.m_bIsFirstDouble && m_BattleContext.m_nCurDoubleAction == 1)
		{
			m_BattleContext.m_bIsSkillPerformed = false; //屏蔽使用技能
			m_BattleContext.m_bIsDouble = true;			 //再次声明是二动
		}
		else
		{
			m_BattleContext.m_bIsDouble = false;
			m_BattleContext.m_bIsSkillPerformed = true;
		}
	}
	//qDebug() << ("OnNotifyBattleAction.") << flags << "m_bIsSkillPerformed" << m_BattleContext.m_bIsSkillPerformed;

	//第一回合
	if (m_BattleContext.m_iRoundCount == 0 && m_BattleContext.m_iLastRound != m_BattleContext.m_iRoundCount)
		m_BattleContext.m_bRoundZeroNotified = true;

	m_BattleContext.m_bIsPetDoubleAction = m_bPetDoubleAction;	   //宠物二动
	m_BattleContext.m_bIsPlayerForceAction = m_bPlayerForceAction; //人物强制行动
	m_BattleContext.m_bIsHaveLv1Enemy = false;					   //重置1级敌人
	m_BattleContext.m_nLv1MaxHp = 0;							   //重置1级最大血
	m_BattleContext.m_nLv1MaxMp = 0;							   //重置1级最大魔
	m_BattleContext.m_bIsSealLv1Enemy = m_bHave1LvEnemy;		   //ui勾选1级宠 是否封印
	//分析战斗数据
	AnalysisBattleData();
	if (m_bAutoBattle)
	{
		//第1级：使用脚本时，脚本设置由脚本控制战斗 此处跳过
		//第2级：有1级敌人和Boos战停止战斗
		if (CheckProtect())
		{
			m_bPlayAlarm = true;
			emit PlayAlarmWav();
			//qDebug("Found Lv1 enemy, stopped.");
			return true;
		}
		if (m_bNoSwitchAnim && m_BattleContext.m_iLastRound != m_BattleContext.m_iRoundCount)
		{ //如果切图 并且回合数
			//			int randDelay = qrand() * (m_iDelayTo - m_iDelayFrom) / RAND_MAX + m_iDelayFrom;
			int randDelay = m_nHightSpeedDelay;

			if (randDelay < 1)
				randDelay = 1;
			if (randDelay > 10000)
				randDelay = 10000;

			if (!m_BattleContext.m_bRoundZeroNotified && m_BattleContext.m_iRoundCount == 1)
				randDelay += 4000;

			QTimer::singleShot(randDelay, this, SLOT(OnPerformanceBattle()));
		}
		else if (((m_BattleContext.m_iRoundCount == 0 && !m_bFirstRoundNoDelay) || m_BattleContext.m_iRoundCount > 0) && m_BattleContext.m_iLastRound != m_BattleContext.m_iRoundCount)
		{ //如果是第一回合 并且第一回合不加速，延迟指定时间  如果是第一回合以后，也进行延迟
			//int randDelay = qrand() * (m_iDelayTo - m_iDelayFrom) / RAND_MAX + m_iDelayFrom;
			int randDelay = m_nHightSpeedDelay; //后续回合延时
			if (randDelay < 1)
				randDelay = 1;
			if (randDelay > 10000)
				randDelay = 10000;
			//qDebug() << "自动战斗 延时" << randDelay;
			QTimer::singleShot(randDelay, this, SLOT(OnPerformanceBattle()));
		}
		else //不延时
		{
			//qDebug() << "自动战斗 不延时 默认延时500毫秒"; //不延时是真的掉线
			OnPerformanceBattle();
		}
		//	qDebug("m_iRoundCount %d m_iLastRound %d", m_BattleContext.m_iRoundCount, m_BattleContext.m_iLastRound);
	}

	if (m_BattleContext.m_iRoundCount != m_BattleContext.m_iLastRound)
	{
		m_BattleContext.m_iLastRound = m_BattleContext.m_iRoundCount;
		m_BattleContext.m_bIsPlayerActionPerformed = false;
		m_BattleContext.m_bIsPlayerEscaped = false;
	}
	if ((m_bBOSSProtect || m_bNoLv1Escape) && m_bPlayAlarm) //Boss战和1级如果勾选，停止播放音乐
	{
		m_bPlayAlarm = false;
		emit StopAlarmWav();
	}
	return true;
}

void CBattleWorker::OnLockCountdown()
{
	int worldStatus = 0;
	int gameStatus = 0;

	g_CGAInterface->BattleSetHighSpeedEnabled(m_bHighSpeed);
	g_CGAInterface->SetGameTextUIEnabled(m_bShowHPMP);

	if (g_CGAInterface->GetGameStatus(gameStatus) && gameStatus == 202)
	{
		auto timestamp = QDateTime::currentDateTime().toTime_t();
		if (m_LastWarpMap202 == 0)
		{
			m_LastWarpMap202 = timestamp;
		}
		else if (timestamp - m_LastWarpMap202 >= 5)
		{
			g_CGAInterface->FixMapWarpStuck(0);
			m_LastWarpMap202 = timestamp + 8;
			qDebug("fix warp");
		}
	}
	else
	{
		m_LastWarpMap202 = 0;
	}

	if (g_CGAInterface->GetWorldStatus(worldStatus) && worldStatus != 10)
	{
		m_BattleContext.m_iLastRound = -1;
		m_BattleContext.m_bRoundZeroNotified = false;
		m_BattleContext.m_bIsPlayerActionPerformed = false;
		m_BattleContext.m_bIsPlayerEscaped = false;
		return;
	}

	if (m_bLockCountdown)
	{
		g_CGAInterface->SetBattleEndTick(1000 * 30);
	}
}

void CBattleWorker::OnPerformanceBattle()
{

	//第3级：遇敌就逃跑；
	//第4级：无1级敌人时逃跑；
	//第5级：遇指定怪逃跑；
	if (m_noPetDoubleAction && m_noPetDoubleAction->bEnabled)
	{
		if (m_noPetDoubleAction->DoAction(m_BattleContext))
			return;
	}
	if (CheckEscape())
		return; // true;
	//1级设置
	//内置保护设置
	if (CheckInternalProtect())
		return; // true;
	//有1级怪时候 进行抓捕
	if (CheckHaveLv1())
		return;

	//第6级：使用人魔少保护技；
	//第7级：使用宠魔少保护技；
	/*
		第10级：对队友使用加血技；
		第11级：第二回合使用人血少保护技；
		第12级：使用人血少保护技；
		第13级：第二回合使用宠血少保护技；
		第14级：使用宠血少保护技；
		第15级：10种基本遇敌状态设置。*/
	for (int i = 0; i < m_SettingList.size(); ++i)
	{
		const CBattleSettingPtr &ptr = m_SettingList.at(i);
		if (ptr->DoAction(m_BattleContext))
			return;
	}
	//1-10回合配置调用
	if (OnSpecifNumRoundBattle())
		return;
	//1-10 敌人配置调用
	if (OnSpecifNumEnemyBattle())
		return;
}

void CBattleWorker::OnNotifyGetSkillsInfo(GameSkillList skills)
{
	m_BattleContext.m_PlayerSkills = skills;
}

void CBattleWorker::OnNotifyGetPetsInfo(GamePetList pets)
{
	m_BattleContext.m_Pets = pets;
}

void CBattleWorker::OnNotifyGetItemsInfo(GameItemList items)
{
	m_BattleContext.m_Items = items;
}

void CBattleWorker::OnNotifyGetBattleInfo(GameBattleUnitList units)
{
	m_BattleContext.m_UnitGroup = units;
}

void CBattleWorker::OnNotifyCharacterInfo(CharacterPtr chara)
{
	m_BattleContext.m_character = chara;
}

bool CBattleWorker::CheckProtect()
{
	if (m_bLevelOneProtect)
	{
		for (int i = 0xA; i < m_BattleContext.m_UnitGroup.size(); ++i)
		{
			if (m_BattleContext.m_UnitGroup[i]->exist && m_BattleContext.m_UnitGroup[i]->level == 1)
				return true;
		}
	}
	if (m_bBOSSProtect)
	{
		int bgm = 0;
		if (g_CGAInterface->GetBGMIndex(bgm) && bgm == 14)
		{
			return true;
		}
	}
	return false;
}

bool CBattleWorker::CheckInternalProtect()
{
	if (m_pIntProtSettings.size() < 1)
		return false;
	//人物魔  宠魔 队伍血 队友血 人物血 宠血
	QList<int> internalOrd = QList<int>({ TProtectSet_PlayerMp2, TProtectSet_PlayerMp1,
			TProtectSet_PetMp, TProtectSet_TroopHp, TProtectSet_TeammateHp, TProtectSet_PlayerHp2, TProtectSet_PlayerHp1, TProtectSet_PetHp });
	/*for (auto it= m_pIntProtSettings.begin();it!= m_pIntProtSettings.end();++it)
	{		
		if (it.value()->DoAction(m_BattleContext))
			return true;
	}*/
	for (int i = 0; i < internalOrd.size(); ++i)
	{
		auto it = m_pIntProtSettings.value(internalOrd[i]);
		if (it && it->bEnabled && it->DoAction(m_BattleContext))
			return true;
	}
	return false;
}
bool CBattleWorker::CheckEscape()
{
	if (m_pEscapeSettingList.size() < 1)
		return false;
	QList<int> internalOrd = QList<int>({ TEscapeSet_All, TEscapeSet_TeammateCount, TEscapeSet_No1Lv, TEscapeSet_NoBoss, TEscapeSet_SpecialEnemy, TEscapeSet_EnemyAvgLv, TEscapeSet_EnemyCount });
	for (int i = 0; i < internalOrd.size(); ++i)
	{
		auto escapeList = m_pEscapeSettingList.values(internalOrd[i]);
		foreach (CBattleSettingPtr setting, escapeList)
		{
			if (setting->DoAction(m_BattleContext))
				return true;
		}
	}
	return false;
}

bool CBattleWorker::CheckHaveLv1()
{
	bool bHave = false;
	int nMaxHp = 0;
	int nMaxMp = 0;
	//启用1级抓宠设置
	if (!m_bHave1LvEnemy)
		return false;
	//身上宠物大于5只 返回
	if (g_pGameFun->GetPetCount() >= 5)
		return false;
	bHave = m_BattleContext.m_bIsHaveLv1Enemy;
	nMaxHp = m_BattleContext.m_nLv1MaxHp;
	nMaxMp = m_BattleContext.m_nLv1MaxMp;
	if (bHave == false)
		return false;
	if (m_pLv1SettingList.size() < 1)
		return false;

	//过滤血检查
	bool bSkip = false;
	if (m_bCheckLv1MaxHp)
	{
		if (nMaxHp < m_nCheckLv1MaxHpVal) //1级怪不达标 执行其他操作
		{
			bSkip = true;
		}
	}
	if (m_bCheckLv1MaxMp)
	{
		if (nMaxMp < m_nCheckLv1MaxMpVal) //1级怪不达标 执行其他操作
		{
			bSkip = true;
		}
	}
	if (bSkip)
	{
		//如果过滤勾选了，并且1级怪不达标，并且勾选了无1级逃跑，则逃跑处理，否则按默认1-10怪攻击
		if (m_bNoLv1Escape) //无1级怪逃跑
		{
			qDebug() << QString("1级怪不达标：Hp-%1 Mp-%2").arg(nMaxHp).arg(nMaxMp);
			auto escapeList = m_pEscapeSettingList.values(TEscapeSet_No1Lv);
			foreach (CBattleSettingPtr setting, escapeList)
			{
				if (setting->DoAction(m_BattleContext))
					return true;
			}
		}
		//跳过此次1判断 执行后续判断
		return false;
	}
	QList<int> internalOrd = QList<int>({ /*TLv1EnemySet_FilterHP, TLv1EnemySet_FilterMP,*/ TLv1EnemySet_RecallPet, TLv1EnemySet_RoundOne, TLv1EnemySet_Lv1HpAction,
			TLv1EnemySet_CleanNoLv1, TLv1EnemySet_LastSet });
	for (int i = 0; i < internalOrd.size(); ++i)
	{
		auto it = m_pLv1SettingList.value(internalOrd[i]);
		if (it && it->bEnabled && it->DoAction(m_BattleContext))
			return true;
	}
	return false;
}

void CBattleWorker::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd)
{
	g_CGAInterface->RegisterBattleActionNotify(std::bind(&CBattleWorker::NotifyBattleAction, this, std::placeholders::_1));
}

void CBattleWorker::OnSetAutoBattle(int state)
{
	m_bAutoBattle = state ? true : false;
}

void CBattleWorker::OnSetHighSpeed(int state)
{
	m_bHighSpeed = state ? true : false;
}

void CBattleWorker::OnSetFRND(int state)
{
	m_bFirstRoundNoDelay = state ? true : false;
}

void CBattleWorker::OnSetAllEscape(int state)
{
	m_bAllEscape = state ? true : false;
}

void CBattleWorker::OnSetNoLv1Escape(int state)
{
	m_bNoLv1Escape = state ? true : false;
}

void CBattleWorker::OnSetSpecialEscape(int state)
{
	m_bSpecialEnemyEscape = state ? true : false;
}

void CBattleWorker::OnSetLv1Protect(int state)
{
	m_bLevelOneProtect = state ? true : false;
}

void CBattleWorker::OnSetBOSSProtect(int state)
{
	m_bBOSSProtect = state ? true : false;
}

void CBattleWorker::OnSetLockCountdown(int state)
{
	m_bLockCountdown = state ? true : false;
}

void CBattleWorker::OnSetNoSwitchAnim(int state)
{
	m_bNoSwitchAnim = state ? true : false;
	int ingame = 0;
	if (g_CGAInterface->IsConnected() && g_CGAInterface->IsInGame(ingame) && ingame)
	{
		g_CGAInterface->SetNoSwitchAnim(m_bNoSwitchAnim);
	}
}

void CBattleWorker::OnSetPetDoubleAction(int state)
{
	m_bPetDoubleAction = state ? true : false;
}

void CBattleWorker::OnSetPlayerForceAction(int state)
{
	m_bPlayerForceAction = state ? true : false;
}

void CBattleWorker::OnSetShowHPMPEnabled(int state)
{
	m_bShowHPMP = state ? true : false;
	/*int ingame = 0;
	if (g_CGAInterface->IsConnected() && g_CGAInterface->IsInGame(ingame) && ingame)*/
	{
		g_CGAInterface->SetGameTextUIEnabled(m_bShowHPMP);
	}
}

void CBattleWorker::OnSetHave1LvEnemy(int state)
{
	m_bHave1LvEnemy = state ? true : false;
}

void CBattleWorker::OnSetHave1LvAction(int nAction, bool bEnabled)
{
	if (nAction == TLv1EnemySet_FilterHP)
	{
		m_bCheckLv1MaxHp = bEnabled;
	}
	else if (nAction == TLv1EnemySet_FilterMP)
	{
		m_bCheckLv1MaxMp = bEnabled;
	}
	else
	{
		if (m_pLv1SettingList.value(nAction) != nullptr)
			m_pLv1SettingList.value(nAction)->bEnabled = bEnabled;
	}
}

void CBattleWorker::OnSetNoPetFirstAction(int state)
{
	m_BattleContext.m_bIsFirstDouble = state ? true : false;
}

void CBattleWorker::OnSetHightSpeedDelayVal(const QString &speed)
{
	//	qDebug() << speed;
	bool bFlag = false;
	int val = speed.toInt(&bFlag);
	if (bFlag == false)
	{
		val = 4; //默认4
	}
	val *= 1000; //转换秒
	m_iDelayFrom = val;
	m_iDelayTo = val;
	m_nHightSpeedDelay = val;
}

void CBattleWorker::OnSetHightSpeedDelayVal(int speed)
{
	int val = speed;
	val *= 1000; //转换秒
	m_iDelayFrom = val;
	m_iDelayTo = val;
	m_nHightSpeedDelay = val;
}

void CBattleWorker::OnSetDelayFrom(int val)
{
	m_iDelayFrom = val;
}

void CBattleWorker::OnSetDelayTo(int val)
{
	m_iDelayTo = val;
}

void CBattleWorker::OnSyncList(CBattleSettingList list)
{
	m_SettingList = list;
}
void CBattleWorker::AddOneCustomSetting(CBattleSettingPtr &item)
{
	m_SettingList.append(item);
}
void CBattleWorker::ClearAllCustomSetting()
{
	m_SettingList.clear();
}

void CBattleWorker::AddSpecifNumEnemySetting(CBattleSettingPtr &item, int num)
{
	if (num < 0 || num > 10)
		return;
	for (size_t i = 0; i < m_pSpecNumEnemySettingList.size(); i++)
	{
		CBattleSettingPtr setPtr = m_pSpecNumEnemySettingList.at(i);
		QString szVal;
		if (setPtr)
		{
			setPtr->GetConditionValue(szVal);
			if (szVal.toInt() == num)
			{
				m_pSpecNumEnemySettingList.erase(m_pSpecNumEnemySettingList.begin() + i);
				break;
			}
		}
	}
	m_pSpecNumEnemySettingList.append(item);
	//    m_pSpecNumEnemySettingList[num]=item;
}

void CBattleWorker::AddSpecifNumRoundSetting(CBattleSettingPtr &item, int num /*= -1*/)
{
	if (num < 0 || num > 10)
		return;
	for (size_t i = 0; i < m_pSpecNumRoundSettingList.size(); i++)
	{
		CBattleSettingPtr setPtr = m_pSpecNumRoundSettingList.at(i);
		QString szVal;
		if (setPtr)
		{
			setPtr->GetConditionValue(szVal);
			if (szVal.toInt() == num)
			{
				m_pSpecNumRoundSettingList.erase(m_pSpecNumRoundSettingList.begin() + i);
				break;
			}
		}
	}
	m_pSpecNumRoundSettingList.append(item);
	//    m_pSpecNumRoundSettingList[num]=item;
}

void CBattleWorker::RemoveSpecifNumEnemySetting(int num)
{
	if (num < 0 || num > 10)
		return;
	for (size_t i = 0; i < m_pSpecNumEnemySettingList.size(); i++)
	{
		CBattleSettingPtr setPtr = m_pSpecNumEnemySettingList.at(i);
		QString szVal;
		if (setPtr)
		{
			setPtr->GetConditionValue(szVal);
			if (szVal.toInt() == num)
			{
				m_pSpecNumEnemySettingList.erase(m_pSpecNumEnemySettingList.begin() + i);
				break;
			}
		}
	}
}

void CBattleWorker::RemoveSpecifNumRoundSetting(int num)
{
	if (num < 0 || num > 10)
		return;
	for (size_t i = 0; i < m_pSpecNumRoundSettingList.size(); i++)
	{
		CBattleSettingPtr setPtr = m_pSpecNumRoundSettingList.at(i);
		QString szVal;
		if (setPtr)
		{
			setPtr->GetConditionValue(szVal);
			if (szVal.toInt() == num)
			{
				m_pSpecNumRoundSettingList.erase(m_pSpecNumRoundSettingList.begin() + i);
				break;
			}
		}
	}
}

void CBattleWorker::ClearSpecifNumEnemySetting()
{
	m_pSpecNumEnemySettingList.clear();
}

void CBattleWorker::ClearSpecifNumRoundSetting()
{
	m_pSpecNumRoundSettingList.clear();
}

bool CBattleWorker::OnSpecifNumEnemyBattle()
{
	for (int i = 0; i < m_pSpecNumEnemySettingList.size(); ++i)
	{
		const CBattleSettingPtr &ptr = m_pSpecNumEnemySettingList[i];
		if (ptr->HasPlayerAction())
		{
			if (ptr->DoAction(m_BattleContext))
				return true;
		}
	}
	return false;
}

bool CBattleWorker::OnSpecifNumRoundBattle()
{
	for (int i = 0; i < m_pSpecNumRoundSettingList.size(); ++i)
	{
		const CBattleSettingPtr &ptr = m_pSpecNumRoundSettingList[i];
		if (ptr->HasPlayerAction())
		{
			if (ptr->DoAction(m_BattleContext))
				return true;
		}
	}
	return false;
}

CBattleSettingPtr CBattleWorker::GetNoPetDoubleAction()
{
	return m_noPetDoubleAction;
}

void CBattleWorker::SetNoPetDoubleAction(CBattleSettingPtr set)
{
	m_noPetDoubleAction = set;
}

//重置战斗计算的信息
void CBattleWorker::ResetBattleAnalysisData()
{
	m_BattleContext.m_iFrontCount = 0;
	m_BattleContext.m_iBackCount = 0;
	m_BattleContext.m_iEnemyCount = 0;
	m_BattleContext.m_iTeammateCount = 0;
	m_BattleContext.m_iPetPosition = -1;
	m_BattleContext.m_bIsPetRiding = false;
}

void CBattleWorker::AnalysisBattleData()
{
	GameBattleUnitList pBattleUnitList = m_BattleContext.m_UnitGroup;
	for (size_t i = 0; i < pBattleUnitList.size(); ++i)
	{
		if (pBattleUnitList[i]->exist == false)
			continue;
		//自动战斗 判断用
		if (pBattleUnitList[i]->pos >= 0xF && pBattleUnitList[i]->pos <= 0x13)
			g_pAutoBattleCtrl->m_BattleContext.m_iFrontCount++; //前排数
		else if (pBattleUnitList[i]->pos >= 0xA && pBattleUnitList[i]->pos <= 0xE)
			g_pAutoBattleCtrl->m_BattleContext.m_iBackCount++; //后排数

		if (pBattleUnitList[i]->pos >= 0xA)
			g_pAutoBattleCtrl->m_BattleContext.m_iEnemyCount++; //敌总数
		else
			g_pAutoBattleCtrl->m_BattleContext.m_iTeammateCount++; //己方总数

		if (pBattleUnitList[i]->pos == g_pAutoBattleCtrl->m_BattleContext.m_iPlayerPosition)
		{
			if (pBattleUnitList[i]->petriding_modelid != 0)
			{
				g_pAutoBattleCtrl->m_BattleContext.m_iPetPosition = pBattleUnitList[i]->pos; //自己宠位置
				g_pAutoBattleCtrl->m_BattleContext.m_bIsPetRiding = true;
			}
			//if (pBattleUnitList[i]->flags == 251658248)//骑乘 F000008  不骑乘 1000004
			//{
			//	g_pAutoBattleCtrl->m_BattleContext.m_iPetPosition = pBattleUnitList[i]->pos; //自己宠位置
			//}
		}
		if (pBattleUnitList[i]->pos == GetPetPosition(g_pAutoBattleCtrl->m_BattleContext.m_iPlayerPosition))
			g_pAutoBattleCtrl->m_BattleContext.m_iPetPosition = pBattleUnitList[i]->pos; //自己宠位置

		if (i >= 0 && i <= 4 && pBattleUnitList[i + 5]->exist)
		{
			pBattleUnitList[i]->isback = true; //后排
		}
		else if (i >= 10 && i <= 14 && pBattleUnitList[5 + i]->exist)
		{
			pBattleUnitList[i]->isback = true; //后排
		}
		if ((pBattleUnitList[i]->flags & FL_DEBUFF_ANY))
			pBattleUnitList[i]->debuff++; //战斗单位是否中了buff

		pBattleUnitList[i]->multi_hp += pBattleUnitList[i]->hp;		  //t血 算上自己的
		pBattleUnitList[i]->multi_maxhp += pBattleUnitList[i]->maxhp; //t魔 算上自己的  下面for算旁边人的

		QList<int> tPosList = m_battleTPos.value(i);
		int tPos = 0; //计算T字形站位血量 魔量 以及debuff
		for (size_t n = 0; n < tPosList.size(); n++)
		{
			tPos = tPosList[n];
			if (pBattleUnitList[tPos]->exist && (pBattleUnitList[tPos]->flags & FL_DEBUFF_ANY))
				pBattleUnitList[i]->debuff++;

			if (pBattleUnitList[tPos]->exist)
				pBattleUnitList[i]->multi_hp += pBattleUnitList[tPos]->hp;

			if (pBattleUnitList[tPos]->exist)
				pBattleUnitList[i]->multi_maxhp += pBattleUnitList[tPos]->maxhp;
		}
		if (pBattleUnitList[i]->level == 1)
		{
			g_pAutoBattleCtrl->m_BattleContext.m_bIsHaveLv1Enemy = true;
			g_pAutoBattleCtrl->m_BattleContext.m_nLv1MaxHp = pBattleUnitList[i]->maxhp;
			g_pAutoBattleCtrl->m_BattleContext.m_nLv1MaxMp = pBattleUnitList[i]->maxmp;
			g_pGameCtrl->signal_addOneChat(QString("发现：%1 1级宠").arg(pBattleUnitList[i]->name));
			//qDebug() << pBattleUnitList[i]->level;
		}
		//qDebug("pos %d debuff %X", i, group[i]->debuff);
	}
}
int CBattleWorker::GetPetPosition(int playerPos)
{
	//if (g_pGameCtrl->getGameCharacter()->petriding)
	//{
	//	qDebug() << "骑乘中" << playerPos;
	//	return playerPos;
	//}

	if (playerPos >= 0 && playerPos <= 4)
		return playerPos + 5;
	if (playerPos >= 5 && playerPos <= 9)
		return playerPos - 5;
	return -1;
}

QString CBattleWorker::GetDevTypeText(int ntype)
{
	return m_devTypeText.value(ntype);
}

CBattleSettingList CBattleWorker::GetSpecNumEnemySettings()
{
	return m_pSpecNumEnemySettingList;
}

CBattleSettingList CBattleWorker::GetSpecNumRoundSettings()
{
	return m_pSpecNumRoundSettingList;
}

CBattleSettingList CBattleWorker::GetCustomSettings()
{
	return m_SettingList;
}

QHash<int, CBattleSettingPtr> CBattleWorker::GetInternalProtectSettings()
{
	return m_pIntProtSettings;
}

CBattleSettingPtr CBattleWorker::GetInternalProtectSetting(int ntype)
{
	return m_pIntProtSettings.value(ntype);
}

void CBattleWorker::AddInternalProtectSetting(int nType, CBattleSettingPtr setting)
{
	if (!m_pIntProtSettings.value(nType).isNull())
	{
		RemoveInternalProtectSetting(nType);
	}
	m_pIntProtSettings.insert(nType, setting);
}

void CBattleWorker::RemoveInternalProtectSetting(int nType)
{
	m_pIntProtSettings.remove(nType);
}

void CBattleWorker::SetInternalProtectSettingEnabled(int nType, bool bFlag)
{
	if (m_pIntProtSettings.value(nType) != nullptr)
		m_pIntProtSettings.value(nType)->bEnabled = bFlag;
}

void CBattleWorker::ClearInternalProtectSetting()
{
	m_pIntProtSettings.clear();
}

CBattleSettingPtr CBattleWorker::GetEscapeSetting(int ntype)
{
	return m_pEscapeSettingList.value(ntype);
}

void CBattleWorker::AddEscapeSetting(int nType, CBattleSettingPtr setting)
{
	//if (!m_pEscapeSettingList.value(nType).isNull())
	//{
	//	RemoveEscapeSetting(nType);
	//}
	m_pEscapeSettingList.insert(nType, setting);
}

void CBattleWorker::RemoveEscapeSetting(int nType)
{
	m_pEscapeSettingList.remove(nType);
}

void CBattleWorker::RemoveEscapeSetting(int nType, const QString &name)
{
	auto specialSettingList = m_pEscapeSettingList.values(nType);
	foreach (CBattleSettingPtr setting, specialSettingList)
	{
		CBattleCondition_EnemyUnit *pCondotopm = (CBattleCondition_EnemyUnit *)setting->m_condition;
		if (pCondotopm->GetUnitName() == name)
		{
			for (auto it = m_pEscapeSettingList.begin(); it != m_pEscapeSettingList.end(); ++it)
			{
				if (it.value() == setting)
				{
					m_pEscapeSettingList.erase(it);
					return;
				}
			}
		}
	}
}

void CBattleWorker::ClearEscapeSetting()
{
	m_pEscapeSettingList.clear();
}

CBattleSettingPtr CBattleWorker::GetHaveLv1Setting(int ntype)
{
	return m_pLv1SettingList.value(ntype);
}

void CBattleWorker::addHaveLv1Setting(int nType, CBattleSettingPtr setting)
{
	m_pLv1SettingList.insert(nType, setting);
}

QHash<int, CBattleSettingPtr> CBattleWorker::GetInternalLv1Settings()
{
	return m_pLv1SettingList;
}

void CBattleWorker::ClearHaveLv1Setting()
{
	m_pLv1SettingList.clear();
}

void CBattleWorker::SetBattleDelay(bool bEnabled, int delay)
{
	if (m_bWaitAfterBattle && !bEnabled)
	{
		g_CGAInterface->SetSwitchAnimForceWait(0, 0);
	}
	m_bWaitAfterBattle = bEnabled;
	m_nBattleDelay = delay;
}
