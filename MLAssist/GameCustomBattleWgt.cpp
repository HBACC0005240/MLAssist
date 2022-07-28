#include "GameCustomBattleWgt.h"
#include <QFontMetrics>
#include <QListView>
#include <QMenu>
#include <QPainter>
#include <QToolTip>
Q_DECLARE_METATYPE(CBattleSettingList)

GameCustomBattleWgt::GameCustomBattleWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	qRegisterMetaType<CBattleSettingList>("CBattleSettingList");

	connect(g_pGameCtrl, &GameCtrl::NotifyGameSkillsInfo, this, &GameCustomBattleWgt::OnNotifyGetSkillsInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGameItemsInfo, this, &GameCustomBattleWgt::OnNotifyGetItemsInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGamePetsInfo, this, &GameCustomBattleWgt::OnNotifyGetPetsInfo, Qt::ConnectionType::QueuedConnection);
	init();
	initTransMap();
}

GameCustomBattleWgt::~GameCustomBattleWgt()
{
}

void GameCustomBattleWgt::init()
{
	ui.comboBox_condition_type->setView(new QListView());
	ui.comboBox_condition_relation->setView(new QListView());
	ui.comboBox_condition_value->setView(new QListView());
	ui.comboBox_condition2_type->setView(new QListView());
	ui.comboBox_condition2_relation->setView(new QListView());
	ui.comboBox_condition2_value->setView(new QListView());
	ui.comboBox_playerAction->setView(new QListView());
	ui.comboBox_playerActionValue->setView(new QListView());

	ui.comboBox_playerTarget->setView(new QListView());
	ui.comboBox_playerTargetSelect->setView(new QListView());

	ui.comboBox_petAction->setView(new QListView());
	ui.comboBox_petTarget->setView(new QListView());
	ui.comboBox_petTargetSelect->setView(new QListView());

	QList<int> condTypes;
	condTypes << dtCondition_Ignore << dtCondition_PlayerHp << dtCondition_PlayerMp
			  << dtCondition_PetHp << dtCondition_PetMp << dtCondition_InventoryItem
			  << dtCondition_TeammateHp
			  << dtCondition_TeammateMp << dtCondition_TeammateDebuff
			  << dtCondition_TeammateMultiTargetHp << dtCondition_TeammateCount
			  << dtCondition_TeammateAllHp
			  << dtCondition_Round << dtCondition_DoubleAction
			  << dtCondition_EnemyAllHp
			  << dtCondition_EnemyCount << dtCondition_EnemySingleRowCount << dtCondition_EnemyUnit
			  << dtCondition_EnemyMultiTargetHp << dtCondition_EnemyLevel
			  << dtCondition_EnemyLv1Hp << dtCondition_EnemyLv1MaxHp
			  << dtCondition_EnemyLv1MaxMp << dtCondition_EnemyType
			  << dtCondition_EnemyAvgLevel;

	m_targetSelect << dtTargetCondition_Random << dtTargetCondition_Front << dtTargetCondition_Back << dtTargetCondition_LowHP
				   << dtTargetCondition_HighHP << dtTargetCondition_LowHPPercent << dtTargetCondition_HighHPPercent
				   << dtTargetCondition_LowLv << dtTargetCondition_HighLv << dtTargetCondition_SingleDebuff
				   << dtTargetCondition_MulTDebuff << dtTargetCondition_Goatfarm << dtTargetCondition_Boomerang
				   << dtTargetCondition_Boomerang << dtTargetCondition_LessUnitRow << dtTargetCondition_MultiMagic
				   << dtTargetCondition_AimOrderUpDown << dtTargetCondition_AimOrderDownUp << dtTargetCondition_AimOrderCenter;
	m_targetTeammateSelect << dtTargetCondition_Random << dtTargetCondition_Front << dtTargetCondition_Back << dtTargetCondition_LowHP
						   << dtTargetCondition_HighHP << dtTargetCondition_LowHPPercent << dtTargetCondition_HighHPPercent
						   << dtTargetCondition_LowLv << dtTargetCondition_HighLv << dtTargetCondition_SingleDebuff
						   << dtTargetCondition_MulTDebuff;

	m_conditionCompare << dtCompare_GreaterEqual << dtCompare_LessEqual << dtCompare_Equal << dtCompare_NotEqual << dtCompare_GreaterThan << dtCompare_LessThan;
	m_conditionCompareString << dtCompare_Contain << dtCompare_NotContain;
	for (int i = 0; i < condTypes.size(); ++i)
	{
		QString sText = g_pAutoBattleCtrl->GetBattleTypeText(condTypes[i]);
		ui.comboBox_condition_type->addItem(sText, condTypes[i]);
		ui.comboBox_condition2_type->addItem(sText, condTypes[i]);
	}
	hBattlePlayerActionString.insert(dtAction_PlayerIgnore, "忽略");
	hBattlePlayerActionString.insert(dtAction_PlayerAttack, "攻击");
	hBattlePlayerActionString.insert(dtAction_PlayerGuard, "防御");
	hBattlePlayerActionString.insert(dtAction_PlayerEscape, "逃跑");
	hBattlePlayerActionString.insert(dtAction_PlayerExchangePosition, "换位");
	hBattlePlayerActionString.insert(dtAction_PlayerChangePet, "召唤宠物");
	hBattlePlayerActionString.insert(dtAction_PlayerUseItem, "使用物品");
	//hBattlePlayerActionString.insert(dtAction_PlayerSkillAttack, "技能攻击");
	hBattlePlayerActionString.insert(dtAction_PlayerLogBack, "登出");
	hBattlePlayerActionString.insert(dtAction_PlayerDoNothing, "什么也不做");

	hPetActionString.insert(dtAction_PetIgnore, "忽略");
	hPetActionString.insert(dtAction_PetDoNothing, "什么也不做");

	hBattleTargetString.insert(dtTarget_Enemy, "敌人");
	hBattleTargetString.insert(dtTarget_Teammate, "队友");
	hBattleTargetString.insert(dtTarget_Self, "人物");
	hBattleTargetString.insert(dtTarget_Pet, "宠物");
	hBattleTargetString.insert(dtTarget_Condition, "条件");

	//人物动作 技能
	for (auto it = hBattlePlayerActionString.begin(); it != hBattlePlayerActionString.end(); ++it)
	{
		if (it.value().isEmpty())
			continue;
		ui.comboBox_playerAction->addItem(it.value(), it.key());
	}

	//人宠攻击目标
	for (auto it = hBattleTargetString.begin(); it != hBattleTargetString.end(); ++it)
	{
		if (it.value().isEmpty())
			continue;
		ui.comboBox_playerTarget->addItem(it.value(), it.key());
		ui.comboBox_petTarget->addItem(it.value(), it.key());
	}

	//敌人优先规则
	for (auto it = hTargetCondString.begin(); it != hTargetCondString.end(); ++it)
	{
		if (it.value().isEmpty())
			continue;
		ui.comboBox_playerTargetSelect->addItem(it.value(), it.key());
		ui.comboBox_petTargetSelect->addItem(it.value(), it.key());
	}

	//宠物默认动作  什么也不做
	for (auto it = hPetActionString.begin(); it != hPetActionString.end(); ++it)
	{
		if (it.value().isEmpty())
			continue;
		ui.comboBox_petAction->addItem(it.value(), it.key());
	}

	m_model = new CBattleSettingModel(ui.tableView_settings);
	ui.tableView_settings->setModel(m_model);
	ui.tableView_settings->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
	ui.tableView_settings->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
	ui.tableView_settings->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);
	ui.tableView_settings->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeMode::Stretch);
	ui.tableView_settings->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeMode::Stretch);
	ui.tableView_settings->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeMode::Stretch);

	ui.tableView_settings->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.tableView_settings, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_table_customContextMenu(const QPoint &)));
	connect(m_model, SIGNAL(syncList(CBattleSettingList)), g_pAutoBattleCtrl, SLOT(OnSyncList(CBattleSettingList)), Qt::ConnectionType::QueuedConnection);
	ui.tableView_settings->setMouseTracking(true);
	connect(ui.tableView_settings, SIGNAL(entered(const QModelIndex &)), this, SLOT(showToolTip(const QModelIndex &)));
	connect(ui.tableView_settings, SIGNAL(clicked(const QModelIndex &)), this, SLOT(dealTableViewClicked(const QModelIndex &)));
}

void GameCustomBattleWgt::initTransMap()
{
	m_conditionMap.insert(0, dtCondition_Ignore);
	m_conditionMap.insert(1, dtCondition_PlayerHp);
	m_conditionMap.insert(2, dtCondition_PlayerMp);
	m_conditionMap.insert(3, dtCondition_PetHp);
	m_conditionMap.insert(4, dtCondition_PetMp);
	m_conditionMap.insert(5, dtCondition_TeammateHp);
	m_conditionMap.insert(6, dtCondition_TeammateMp);
	m_conditionMap.insert(7, dtCondition_TeammateDebuff);
	m_conditionMap.insert(8, dtCondition_TeammateMultiTargetHp);
	m_conditionMap.insert(9, dtCondition_TeammateAllHp);
	m_conditionMap.insert(10, dtCondition_EnemyCount);
	m_conditionMap.insert(11, dtCondition_EnemySingleRowCount);
	m_conditionMap.insert(12, dtCondition_EnemyUnit);
	m_conditionMap.insert(13, dtCondition_EnemyLevel);
	m_conditionMap.insert(14, dtCondition_EnemyAvgLevel);
	m_conditionMap.insert(15, dtCondition_EnemyMultiTargetHp);
	m_conditionMap.insert(16, dtCondition_EnemyAllHp);
	m_conditionMap.insert(17, dtCondition_Round);
	m_conditionMap.insert(18, dtCondition_DoubleAction);
	m_conditionMap.insert(19, dtCondition_InventoryItem);
	m_conditionMap.insert(20, dtCondition_TeammateCount);
	m_conditionMap.insert(21, dtCondition_TeammateUnit);
	m_conditionMap.insert(22, dtCondition_PlayerName);
	m_conditionMap.insert(23, dtCondition_PlayerJob);
	m_conditionMap.insert(24, dtCondition_PlayerGold);
	m_conditionMap.insert(25, dtCondition_PlayerBGM);
	m_actionMap.insert(0, dtAction_PlayerIgnore);
	m_actionMap.insert(1, dtAction_PlayerAttack);
	m_actionMap.insert(2, dtAction_PlayerGuard);
	m_actionMap.insert(3, dtAction_PlayerEscape);
	m_actionMap.insert(4, dtAction_PlayerExchangePosition);
	m_actionMap.insert(5, dtAction_PlayerChangePet);
	m_actionMap.insert(6, dtAction_PlayerUseItem);
	m_actionMap.insert(7, dtAction_PlayerLogBack);
	m_actionMap.insert(8, dtAction_PlayerRebirth);
	m_actionMap.insert(9, dtAction_PlayerDoNothing);
	m_actionMap.insert(10, dtAction_PlayerIgnore);
	m_actionMap.insert(100, dtAction_PlayerSkillAttack);
	m_targetConditionNumMap.insert(0, dtCompare_GreaterEqual);
	m_targetConditionNumMap.insert(1, dtCompare_GreaterThan);
	m_targetConditionNumMap.insert(2, dtCompare_LessEqual);
	m_targetConditionNumMap.insert(3, dtCompare_LessThan);
	m_targetConditionNumMap.insert(4, dtCompare_Equal);
	m_targetConditionNumMap.insert(5, dtCompare_NotEqual);
	m_petActionMap.insert(0, dtAction_PetIgnore);
	m_petActionMap.insert(1, dtAction_PetDoNothing);
	m_petActionMap.insert(100, dtAction_PetSkillAttack);
	m_targetMap.insert(0, dtTarget_Enemy);
	m_targetMap.insert(1, dtTarget_Teammate);
	m_targetMap.insert(2, dtTarget_Self);
	m_targetMap.insert(3, dtTarget_Pet);
	m_targetMap.insert(4, dtTarget_Condition);
	m_targetConditionMap.insert(0, dtTargetCondition_Random);
	m_targetConditionMap.insert(1, dtTargetCondition_Front);
	m_targetConditionMap.insert(2, dtTargetCondition_Back);
	m_targetConditionMap.insert(3, dtTargetCondition_LowHP);
	m_targetConditionMap.insert(4, dtTargetCondition_HighHP);
	m_targetConditionMap.insert(5, dtTargetCondition_LowHPPercent);
	m_targetConditionMap.insert(6, dtTargetCondition_HighHPPercent);
	m_targetConditionMap.insert(7, dtTargetCondition_LowLv);
	m_targetConditionMap.insert(8, dtTargetCondition_HighLv);
	m_targetConditionMap.insert(9, dtTargetCondition_SingleDebuff);
	m_targetConditionMap.insert(10, dtTargetCondition_MulTDebuff);
	m_targetConditionMap.insert(11, dtTargetCondition_Goatfarm);
	m_targetConditionMap.insert(12, dtTargetCondition_Boomerang);
	m_targetConditionMap.insert(13, dtTargetCondition_LessUnitRow);
	m_targetConditionMap.insert(14, dtTargetCondition_MultiMagic);
}

void GameCustomBattleWgt::OnCloseWindow()
{
}

void GameCustomBattleWgt::on_comboBox_condition_type_currentIndexChanged(int index)
{
	ui.comboBox_condition_relation->clear();
	ui.comboBox_condition_value->clear();
	int itemData = ui.comboBox_condition_type->currentData().toInt();
	switch (itemData)
	{
		case dtCondition_Round:
		case dtCondition_TeammateCount:
		case dtCondition_EnemyCount:
		case dtCondition_EnemySingleRowCount:
		case dtCondition_PlayerHp:
		case dtCondition_PlayerMp:
		case dtCondition_PetHp:
		case dtCondition_PetMp:
		case dtCondition_TeammateHp:
		case dtCondition_TeammateMp:
		case dtCondition_EnemyMultiTargetHp:
		case dtCondition_TeammateMultiTargetHp:
		case dtCondition_EnemyAllHp:
		case dtCondition_TeammateAllHp:
		case dtCondition_PlayerGold:
		case dtCondition_PlayerBGM:
		{
			for (int i = 0; i < m_conditionCompare.size(); ++i)
				ui.comboBox_condition_relation->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_conditionCompare[i]), m_conditionCompare[i]);

			if (itemData == dtCondition_Round || itemData == dtCondition_EnemyCount || itemData == dtCondition_TeammateCount || itemData == dtCondition_EnemySingleRowCount)
			{
				for (int i = 1; i <= 10; ++i)
					ui.comboBox_condition_value->addItem(tr("%1").arg(i));
			}
			else
			{
				ui.comboBox_condition_value->addItem(QLatin1String("25%"));
				ui.comboBox_condition_value->addItem(QLatin1String("50%"));
				ui.comboBox_condition_value->addItem(QLatin1String("75%"));
				ui.comboBox_condition_value->addItem(QLatin1String("500"));
				ui.comboBox_condition_value->addItem(QLatin1String("1000"));
				ui.comboBox_condition_value->addItem(QLatin1String("1500"));
				ui.comboBox_condition_value->addItem(QLatin1String("2000"));
			}
			break;
		}
		case dtCondition_EnemyUnit:
		case dtCondition_InventoryItem:
		case dtCondition_TeammateUnit:
		case dtCondition_PlayerName:
		case dtCondition_PlayerJob:
		{
			for (int i = 0; i < m_conditionCompareString.size(); ++i)
				ui.comboBox_condition_relation->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_conditionCompareString[i]), m_conditionCompareString[i]);
			break;
		}
		case dtCondition_EnemyLevel:
		case dtCondition_EnemyAvgLevel:
		{
			for (int i = 0; i < m_conditionCompare.size(); ++i)
				ui.comboBox_condition_relation->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_conditionCompare[i]), m_conditionCompare[i]);

			for (int i = 0; i <= 160; i += 10)
				ui.comboBox_condition_value->addItem(tr("%1").arg(i == 0 ? 1 : i));
			break;
		}
		case dtCondition_TeammateDebuff:
		{
			for (int i = 0; i < m_conditionCompareString.size(); ++i)
				ui.comboBox_condition_relation->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_conditionCompareString[i]), m_conditionCompareString[i]);

			using namespace CGA;
			ui.comboBox_condition_value->addItem(tr("Sleep"), QVariant(FL_DEBUFF_SLEEP));
			ui.comboBox_condition_value->addItem(tr("Medusa"), QVariant(FL_DEBUFF_MEDUSA));
			ui.comboBox_condition_value->addItem(tr("Drunk"), QVariant(FL_DEBUFF_DRUNK));
			ui.comboBox_condition_value->addItem(tr("Chaos"), QVariant(FL_DEBUFF_CHAOS));
			ui.comboBox_condition_value->addItem(tr("Forget"), QVariant(FL_DEBUFF_FORGET));
			ui.comboBox_condition_value->addItem(tr("Poison"), QVariant(FL_DEBUFF_POISON));
			ui.comboBox_condition_value->addItem(tr("Any"), QVariant(FL_DEBUFF_ANY));
			break;
		}
	}
}

void GameCustomBattleWgt::on_comboBox_condition2_type_currentIndexChanged(int index)
{
	ui.comboBox_condition2_relation->clear();
	ui.comboBox_condition2_value->clear();
	int itemData = ui.comboBox_condition2_type->currentData().toInt();
	switch (itemData)
	{
		case dtCondition_Round:
		case dtCondition_EnemyCount:
		case dtCondition_TeammateCount:
		case dtCondition_EnemySingleRowCount:
		case dtCondition_PlayerHp:
		case dtCondition_PlayerMp:
		case dtCondition_PetHp:
		case dtCondition_PetMp:
		case dtCondition_TeammateHp:
		case dtCondition_TeammateMp:
		case dtCondition_EnemyMultiTargetHp:
		case dtCondition_TeammateMultiTargetHp:
		case dtCondition_EnemyAllHp:
		case dtCondition_TeammateAllHp:
		{
			for (int i = 0; i < m_conditionCompare.size(); ++i)
				ui.comboBox_condition2_relation->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_conditionCompare[i]), m_conditionCompare[i]);

			if (itemData == dtCondition_Round || itemData == dtCondition_EnemyCount || itemData == dtCondition_TeammateCount || itemData == dtCondition_EnemySingleRowCount)
			{
				for (int i = 1; i <= 10; ++i)
					ui.comboBox_condition2_value->addItem(tr("%1").arg(i));
			}
			else
			{
				ui.comboBox_condition2_value->addItem(QLatin1String("25%"));
				ui.comboBox_condition2_value->addItem(QLatin1String("50%"));
				ui.comboBox_condition2_value->addItem(QLatin1String("75%"));
				ui.comboBox_condition2_value->addItem(QLatin1String("500"));
				ui.comboBox_condition2_value->addItem(QLatin1String("1000"));
				ui.comboBox_condition2_value->addItem(QLatin1String("1500"));
				ui.comboBox_condition2_value->addItem(QLatin1String("2000"));
			}
			break;
		}
		case dtCondition_EnemyUnit:
		case dtCondition_InventoryItem:
		{
			for (int i = 0; i < m_conditionCompareString.size(); ++i)
				ui.comboBox_condition2_relation->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_conditionCompareString[i]), m_conditionCompareString[i]);
			break;
		}
		case dtCondition_EnemyLevel:
		case dtCondition_EnemyAvgLevel:
		{
			for (int i = 0; i < m_conditionCompare.size(); ++i)
				ui.comboBox_condition2_relation->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_conditionCompare[i]), m_conditionCompare[i]);

			for (int i = 0; i <= 160; i += 10)
				ui.comboBox_condition2_value->addItem(tr("%1").arg(i == 0 ? 1 : i));
			break;
		}
		case dtCondition_TeammateDebuff:
		{
			for (int i = 0; i < m_conditionCompareString.size(); ++i)
				ui.comboBox_condition2_relation->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_conditionCompareString[i]), m_conditionCompareString[i]);

			using namespace CGA;
			ui.comboBox_condition2_value->addItem(tr("Sleep"), QVariant(FL_DEBUFF_SLEEP));
			ui.comboBox_condition2_value->addItem(tr("Medusa"), QVariant(FL_DEBUFF_MEDUSA));
			ui.comboBox_condition2_value->addItem(tr("Drunk"), QVariant(FL_DEBUFF_DRUNK));
			ui.comboBox_condition2_value->addItem(tr("Chaos"), QVariant(FL_DEBUFF_CHAOS));
			ui.comboBox_condition2_value->addItem(tr("Forget"), QVariant(FL_DEBUFF_FORGET));
			ui.comboBox_condition2_value->addItem(tr("Poison"), QVariant(FL_DEBUFF_POISON));
			ui.comboBox_condition2_value->addItem(tr("Any"), QVariant(FL_DEBUFF_ANY));
			break;
		}
	}
}

void GameCustomBattleWgt::on_comboBox_playerAction_currentIndexChanged(int index)
{
	ui.comboBox_playerActionValue->clear();
	int itemData = ui.comboBox_playerAction->currentData().toInt();
	switch (itemData)
	{
		case dtAction_PlayerIgnore:
		{
			//NO TARGET
			ui.comboBox_playerTarget->setEnabled(false);
			ui.comboBox_playerTargetSelect->setEnabled(false);
			ui.comboBox_playerActionValue->setEnabled(false);
			break;
		}
		case dtAction_PlayerAttack:
		{
			ui.comboBox_playerTarget->setEnabled(true);
			ui.comboBox_playerTargetSelect->setEnabled(true);
			ui.comboBox_playerActionValue->setEnabled(false);
			break;
		}
		case dtAction_PlayerGuard:
		{
			//NO TARGET
			ui.comboBox_playerTarget->setEnabled(false);
			ui.comboBox_playerTargetSelect->setEnabled(false);
			ui.comboBox_playerActionValue->setEnabled(false);
			break;
		}
		case dtAction_PlayerEscape:
		{
			ui.comboBox_playerTarget->setEnabled(false);
			ui.comboBox_playerTargetSelect->setEnabled(false);
			ui.comboBox_playerActionValue->setEnabled(false);
			break;
		}
		case dtAction_PlayerExchangePosition:
		{
			ui.comboBox_playerTarget->setEnabled(false);
			ui.comboBox_playerTargetSelect->setEnabled(false);
			ui.comboBox_playerActionValue->setEnabled(false);
			break;
		}
		case dtAction_PlayerChangePet:
		{
			//TODO:Fill item list here...
			ui.comboBox_playerTarget->setEnabled(false);
			ui.comboBox_playerTargetSelect->setEnabled(false);
			ui.comboBox_playerActionValue->setEnabled(true);
			ui.comboBox_playerActionValue->addItem("宠物收回", QVariant(TChangePet_Recall));
			ui.comboBox_playerActionValue->addItem("召唤最高等级宠", QVariant(TChangePet_HighestLv));
			ui.comboBox_playerActionValue->addItem("召唤最高血量宠", QVariant(TChangePet_HighestHp));
			ui.comboBox_playerActionValue->addItem("召唤最高魔量宠", QVariant(TChangePet_HighestMp));
			ui.comboBox_playerActionValue->addItem("召唤最低等级宠", QVariant(TChangePet_LowestLv));
			ui.comboBox_playerActionValue->addItem("召唤最低血量宠", QVariant(TChangePet_LowestHp));
			ui.comboBox_playerActionValue->addItem("召唤最低魔量宠", QVariant(TChangePet_LowestMp));
			ui.comboBox_playerActionValue->addItem("召唤最高忠诚宠", QVariant(TChangePet_HighestLoyalty));
			ui.comboBox_playerActionValue->addItem("召唤最低忠诚宠", QVariant(TChangePet_LowestLoyalty));
			for (int i = 0; i < m_pets.size(); ++i)
			{
				if (m_pets[i]->exist == false)
					continue;
				ui.comboBox_playerActionValue->addItem(m_pets[i]->showname, QVariant(TChangePet_Call));
			}
			break;
		}
		case dtAction_PlayerUseItem:
		{
			//TODO:Fill item list here...
			ui.comboBox_playerTarget->setEnabled(true);
			ui.comboBox_playerTargetSelect->setEnabled(true);
			ui.comboBox_playerActionValue->setEnabled(true);

			auto pItemList = m_items;
			for (int i = 0; i < pItemList.size(); ++i)
			{
				if (pItemList.at(i)->exist)
				{
					ui.comboBox_playerActionValue->addItem(pItemList.at(i)->name, QVariant(pItemList.at(i)->id));
				}
			}
			break;
		}
		case dtAction_PlayerLogBack:
		case dtAction_PlayerRebirth:
		case dtAction_PlayerDoNothing:
		{
			//NO TARGET
			ui.comboBox_playerTarget->setEnabled(false);
			ui.comboBox_playerTargetSelect->setEnabled(false);
			ui.comboBox_playerActionValue->setEnabled(false);
			break;
		}
		default: //Custom skills... dtAction_PlayerSkillAttack
		{
			ui.comboBox_playerTarget->setEnabled(true);
			ui.comboBox_playerTargetSelect->setEnabled(true);
			ui.comboBox_playerActionValue->setEnabled(true);
			bool bValue = false;
			int level = ui.comboBox_playerAction->itemData(index).toInt(&bValue);
			if (bValue && level > 0)
			{
				ui.comboBox_playerActionValue->addItem("最高等级", QVariant(0));
				for (int i = 1; i <= level; ++i)
					ui.comboBox_playerActionValue->addItem(tr("%1级").arg(i), QVariant(i));
			}
			break;
		}
	}
}

void GameCustomBattleWgt::on_comboBox_playerTarget_currentIndexChanged(int index)
{
	ui.comboBox_playerTargetSelect->clear();
	int itemData = ui.comboBox_playerTarget->currentData().toInt();
	switch (itemData)
	{
		case dtTarget_Enemy:
		{
			for (int i = 0; i < m_targetSelect.size(); ++i)
				ui.comboBox_playerTargetSelect->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_targetSelect[i]), m_targetSelect[i]);
			break;
		}
		case dtTarget_Teammate:
		{
			for (int i = 0; i < m_targetTeammateSelect.size(); ++i)
				ui.comboBox_playerTargetSelect->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_targetTeammateSelect[i]), m_targetTeammateSelect[i]);
			break;
		}
		case dtTarget_Self:
		{
			break;
		}
		case dtTarget_Pet:
		{
			break;
		}
		case dtTarget_Condition:
		{
			break;
		}
	}
}

void GameCustomBattleWgt::on_comboBox_petAction_currentIndexChanged(int index)
{
	int itemData = ui.comboBox_petAction->currentData().toInt();
	switch (itemData)
	{
		case dtAction_PetIgnore:
		case dtAction_PetDoNothing:
		{
			ui.comboBox_petTarget->setEnabled(false);
			ui.comboBox_petTargetSelect->setEnabled(false);
			break;
		}

		default: //Custom skills... dtAction_PetSkillAttack
		{
			ui.comboBox_petTarget->setEnabled(true);
			ui.comboBox_petTargetSelect->setEnabled(true);
			break;
		}
	}
}

void GameCustomBattleWgt::on_comboBox_petTarget_currentIndexChanged(int index)
{
	ui.comboBox_petTargetSelect->clear();
	int itemData = ui.comboBox_petTarget->currentData().toInt();
	switch (itemData)
	{
		case dtTarget_Enemy:
		{
			for (int i = 0; i < m_targetSelect.size(); ++i)
				ui.comboBox_petTargetSelect->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_targetSelect[i]), m_targetSelect[i]);
			break;
		}
		case dtTarget_Teammate:
		{
			for (int i = 0; i < m_targetTeammateSelect.size(); ++i)
				ui.comboBox_petTargetSelect->addItem(g_pAutoBattleCtrl->GetBattleTypeText(m_targetTeammateSelect[i]), m_targetTeammateSelect[i]);
			break;
		}
		case dtTarget_Self:
		{
			break;
		}
		case dtTarget_Pet:
		{
			break;
		}
		case dtTarget_Condition:
		{
			break;
		}
	}
}

void GameCustomBattleWgt::on_pushButton_add_clicked()
{
	CBattleCondition *pCondition = NULL;
	CBattleCondition *pCondition2 = NULL;
	{
		int condType = ui.comboBox_condition_type->currentData().toInt();
		switch (condType)
		{
			case dtCondition_Ignore:
			{
				pCondition = new CBattleCondition_Ignore();
				break;
			}

			case dtCondition_EnemyCount:
			case dtCondition_TeammateCount:
			case dtCondition_EnemySingleRowCount:
			{
				int relation = ui.comboBox_condition_relation->currentData().toInt();
				{
					bool bValue = false;
					int value = ui.comboBox_condition_value->currentText().toInt(&bValue);
					if (bValue)
					{
						if (condType == dtCondition_EnemyCount && value >= 0 && value <= 10)
							pCondition = new CBattleCondition_EnemyCount(relation, value);
						else if (condType == dtCondition_TeammateCount && value >= 0 && value <= 10)
							pCondition = new CBattleCondition_TeammateCount(relation, value);
						else if (condType == dtCondition_EnemySingleRowCount && value >= 0 && value <= 10)
							pCondition = new CBattleCondition_EnemySingleRowCount(relation, value);
					}
				}
				break;
			}
			case dtCondition_PlayerHp:
			case dtCondition_PlayerMp:
			case dtCondition_PetHp:
			case dtCondition_PetMp:
			case dtCondition_TeammateHp:
			case dtCondition_TeammateMp:
			case dtCondition_EnemyMultiTargetHp:
			case dtCondition_TeammateMultiTargetHp:
			case dtCondition_EnemyAllHp:
			case dtCondition_TeammateAllHp:
			{
				int relation = ui.comboBox_condition_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompare.size())
				{
					QString str = ui.comboBox_condition_value->currentText();
					int per = str.indexOf(QChar('%'));
					bool bValue = false, bPercentage = false;
					int value = 0;
					if (per >= 1)
					{
						value = str.mid(0, per).toInt(&bValue);
						if (bValue && value >= 0 && value <= 100)
							bPercentage = true;
						else
							bValue = false;
					}
					else
					{
						value = str.toInt(&bValue);
						if (bValue && value >= 0)
							bPercentage = false;
						else
							bValue = false;
					}
					if (bValue)
					{
						if (condType == dtCondition_PlayerHp)
							pCondition = new CBattleCondition_PlayerHp(relation, value, bPercentage);
						else if (condType == dtCondition_PlayerMp)
							pCondition = new CBattleCondition_PlayerMp(relation, value, bPercentage);
						else if (condType == dtCondition_PetHp)
							pCondition = new CBattleCondition_PetHp(relation, value, bPercentage);
						else if (condType == dtCondition_PetMp)
							pCondition = new CBattleCondition_PetMp(relation, value, bPercentage);
						else if (condType == dtCondition_TeammateHp)
							pCondition = new CBattleCondition_TeammateHp(relation, value, bPercentage);
						else if (condType == dtCondition_TeammateMp)
							pCondition = new CBattleCondition_TeammateMp(relation, value, bPercentage);
						else if (condType == dtCondition_EnemyMultiTargetHp)
							pCondition = new CBattleCondition_EnemyMultiTargetHp(relation, value, bPercentage);
						else if (condType == dtCondition_TeammateMultiTargetHp)
							pCondition = new CBattleCondition_TeammateMultiTargetHp(relation, value, bPercentage);
						else if (condType == dtCondition_EnemyAllHp)
							pCondition = new CBattleCondition_EnemyAllHp(relation, value, bPercentage);
						else if (condType == dtCondition_TeammateAllHp)
							pCondition = new CBattleCondition_TeammateAllHp(relation, value, bPercentage);
					}
				}
				break;
			}
			case dtCondition_EnemyUnit:
			case dtCondition_TeammateUnit:
			case dtCondition_PlayerName:
			case dtCondition_PlayerJob:
			{
				int relation = ui.comboBox_condition_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompareString.size())
				{
					QString name = ui.comboBox_condition_value->currentText();
					if (!name.isEmpty())
					{
						switch (condType)
						{
							case dtCondition_EnemyUnit:
							{
								pCondition = new CBattleCondition_EnemyUnit(relation, name);
								break;
							}
							case dtCondition_TeammateUnit:
							{
								pCondition = new CBattleCondition_TeammateUnit(relation, name);
								break;
							}
							case dtCondition_PlayerName:
							{
								pCondition = new CBattleCondition_PlayerName(relation, name);
								break;
							}
							case dtCondition_PlayerJob:
							{
								pCondition = new CBattleCondition_PlayerJob(relation, name);
								break;
							}
							default:
								break;
						}
					}
				}
				break;
			}
			case dtCondition_InventoryItem:
			{
				int relation = ui.comboBox_condition_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompareString.size())
				{
					QString name = ui.comboBox_condition_value->currentText();
					if (!name.isEmpty())
						pCondition = new CBattleCondition_InventoryItem(relation, name);
				}
				break;
			}
			case dtCondition_Round:
			case dtCondition_PlayerGold:
			case dtCondition_PlayerBGM:
			{
				int relation = ui.comboBox_condition_relation->currentData().toInt();
				//	if (relation >= 0 && relation < m_conditionCompare.size())
				{
					bool bValue = false;
					int value = ui.comboBox_condition_value->currentText().toInt(&bValue);
					if (bValue && value >= 0 && value <= 10000000)
					{
						switch (condType)
						{
							case dtCondition_Round:
							{
								pCondition = new CBattleCondition_Round(relation, value);
								break;
							}
							case dtCondition_PlayerGold:
							{
								pCondition = new CBattleCondition_PlayerGold(relation, value);
								break;
							}
							case dtCondition_PlayerBGM:
							{
								pCondition = new CBattleCondition_BattleBGM(relation, value);
								break;
							}
							default: break;
						}
					}
				}
				break;
			}
			case dtCondition_EnemyLevel:
			case dtCondition_EnemyAvgLevel:
			{
				int relation = ui.comboBox_condition_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompare.size())
				{
					bool bValue = false;
					int value = ui.comboBox_condition_value->currentText().toInt(&bValue);
					if (bValue && value >= 1 && value <= 160)
					{
						if (condType == dtCondition_EnemyLevel)
							pCondition = new CBattleCondition_EnemyLevel(relation, value);
						else if (condType == dtCondition_EnemyAvgLevel)
							pCondition = new CBattleCondition_EnemyAvgLevel(relation, value);
					}
				}
				break;
			}

			case dtCondition_DoubleAction:
			{
				pCondition = new CBattleCondition_DoubleAction();
				break;
			}
			case dtCondition_TeammateDebuff:
			{
				int relation = ui.comboBox_condition_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompareString.size())
				{
					bool bValue = false;
					int value = ui.comboBox_condition_value->currentData().toInt(&bValue);
					if (bValue && value >= FL_DEBUFF_SLEEP && value <= FL_DEBUFF_ANY)
						pCondition = new CBattleCondition_TeammateDebuff(relation, value);
				}
				break;
			}
		}
	}

	{
		int condType = ui.comboBox_condition2_type->currentData().toInt();

		switch (condType)
		{
			case dtCondition_Ignore:
			{
				pCondition2 = new CBattleCondition_Ignore();
				break;
			}

			case dtCondition_EnemyCount:
			case dtCondition_TeammateCount:
			case dtCondition_EnemySingleRowCount:
			{
				int relation = ui.comboBox_condition2_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompare.size())
				{
					bool bValue = false;
					int value = ui.comboBox_condition2_value->currentText().toInt(&bValue);
					if (bValue)
					{
						if (condType == dtCondition_EnemyCount && value >= 0 && value <= 10)
							pCondition2 = new CBattleCondition_EnemyCount(relation, value);
						else if (condType == dtCondition_TeammateCount && value >= 0 && value <= 10)
							pCondition2 = new CBattleCondition_TeammateCount(relation, value);
						else if (condType == dtCondition_EnemySingleRowCount && value >= 0 && value <= 10)
							pCondition2 = new CBattleCondition_EnemySingleRowCount(relation, value);
					}
				}
				break;
			}
			case dtCondition_PlayerHp:
			case dtCondition_PlayerMp:
			case dtCondition_PetHp:
			case dtCondition_PetMp:
			case dtCondition_TeammateHp:
			case dtCondition_TeammateMp:
			case dtCondition_EnemyMultiTargetHp:
			case dtCondition_TeammateMultiTargetHp:
			case dtCondition_EnemyAllHp:
			case dtCondition_TeammateAllHp:
			{
				int relation = ui.comboBox_condition2_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompare.size())
				{
					QString str = ui.comboBox_condition2_value->currentText();
					int per = str.indexOf(QChar('%'));
					bool bValue = false, bPercentage = false;
					int value = 0;
					if (per >= 1)
					{
						value = str.mid(0, per).toInt(&bValue);
						if (bValue && value >= 0 && value <= 100)
							bPercentage = true;
						else
							bValue = false;
					}
					else
					{
						value = str.toInt(&bValue);
						if (bValue && value >= 0)
							bPercentage = false;
						else
							bValue = false;
					}
					if (bValue)
					{
						if (condType == dtCondition_PlayerHp)
							pCondition2 = new CBattleCondition_PlayerHp(relation, value, bPercentage);
						else if (condType == dtCondition_PlayerMp)
							pCondition2 = new CBattleCondition_PlayerMp(relation, value, bPercentage);
						else if (condType == dtCondition_PetHp)
							pCondition2 = new CBattleCondition_PetHp(relation, value, bPercentage);
						else if (condType == dtCondition_PetMp)
							pCondition2 = new CBattleCondition_PetMp(relation, value, bPercentage);
						else if (condType == dtCondition_TeammateHp)
							pCondition2 = new CBattleCondition_TeammateHp(relation, value, bPercentage);
						else if (condType == dtCondition_TeammateMp)
							pCondition2 = new CBattleCondition_TeammateMp(relation, value, bPercentage);
						else if (condType == dtCondition_EnemyMultiTargetHp)
							pCondition2 = new CBattleCondition_EnemyMultiTargetHp(relation, value, bPercentage);
						else if (condType == dtCondition_TeammateMultiTargetHp)
							pCondition2 = new CBattleCondition_TeammateMultiTargetHp(relation, value, bPercentage);
						else if (condType == dtCondition_EnemyAllHp)
							pCondition2 = new CBattleCondition_EnemyAllHp(relation, value, bPercentage);
						else if (condType == dtCondition_TeammateAllHp)
							pCondition2 = new CBattleCondition_TeammateAllHp(relation, value, bPercentage);
					}
				}
				break;
			}
			case dtCondition_EnemyUnit:
			case dtCondition_TeammateUnit:
			case dtCondition_PlayerName:
			case dtCondition_PlayerJob:
			{
				int relation = ui.comboBox_condition2_relation->currentData().toInt();
				//	if (relation >= 0 && relation < m_conditionCompareString.size())
				{
					QString name = ui.comboBox_condition2_value->currentText();
					if (!name.isEmpty())
					{
						if (condType == dtCondition_EnemyUnit)
							pCondition2 = new CBattleCondition_EnemyUnit(relation, name);
						else if (condType == dtCondition_TeammateUnit)
							pCondition2 = new CBattleCondition_TeammateUnit(relation, name);
						else if (condType == dtCondition_PlayerName)
							pCondition2 = new CBattleCondition_PlayerName(relation, name);
						else if (condType == dtCondition_PlayerJob)
							pCondition2 = new CBattleCondition_PlayerJob(relation, name);
					}
				}
				break;
			}
			case dtCondition_InventoryItem:
			{
				int relation = ui.comboBox_condition2_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompareString.size())
				{
					QString name = ui.comboBox_condition2_value->currentText();
					if (!name.isEmpty())
						pCondition2 = new CBattleCondition_InventoryItem(relation, name);
				}
				break;
			}
			case dtCondition_Round:
			case dtCondition_PlayerGold:
			case dtCondition_PlayerBGM:
			{
				int relation = ui.comboBox_condition2_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompare.size())
				{
					bool bValue = false;
					int value = ui.comboBox_condition2_value->currentText().toInt(&bValue);
					if (bValue && value >= 0 && value <= 10000000)
					{
						if (condType == dtCondition_Round)
							pCondition2 = new CBattleCondition_Round(relation, value);
						else if (condType == dtCondition_PlayerGold)
							pCondition2 = new CBattleCondition_PlayerGold(relation, value);
						else if (condType == dtCondition_PlayerBGM)
							pCondition2 = new CBattleCondition_BattleBGM(relation, value);
					}
				}
				break;
			}
			case dtCondition_EnemyLevel:
			case dtCondition_EnemyAvgLevel:
			{
				int relation = ui.comboBox_condition2_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompare.size())
				{
					bool bValue = false;
					int value = ui.comboBox_condition2_value->currentText().toInt(&bValue);
					if (bValue && value >= 1 && value <= 160)
					{
						if (condType == dtCondition_EnemyLevel)
							pCondition2 = new CBattleCondition_EnemyLevel(relation, value);
						else if (condType == dtCondition_EnemyAvgLevel)
							pCondition2 = new CBattleCondition_EnemyAvgLevel(relation, value);
					}
				}
				break;
			}

			case dtCondition_DoubleAction:
			{
				pCondition2 = new CBattleCondition_DoubleAction();
				break;
			}
			case dtCondition_TeammateDebuff:
			{
				int relation = ui.comboBox_condition2_relation->currentData().toInt();
				//if (relation >= 0 && relation < m_conditionCompareString.size())
				{
					bool bValue = false;
					int value = ui.comboBox_condition2_value->currentData().toInt(&bValue);
					if (bValue && value >= FL_DEBUFF_SLEEP && value <= FL_DEBUFF_ANY)
						pCondition2 = new CBattleCondition_TeammateDebuff(relation, value);
				}
				break;
			}
		}
	}

	CBattleAction *pPlayerAction = NULL;

	int playerActionData = ui.comboBox_playerAction->currentData().toInt();
	QString playerActionStr = ui.comboBox_playerAction->currentText();

	bool bHasPlayerTarget = ui.comboBox_playerTarget->isEnabled();
	switch (playerActionData)
	{
		case dtAction_PlayerIgnore:
		{
			pPlayerAction = NULL;
			break;
		}
		case dtAction_PlayerAttack:
		{
			pPlayerAction = new CBattleAction_PlayerAttack();
			break;
		}
		case dtAction_PlayerGuard:
		{
			pPlayerAction = new CBattleAction_PlayerGuard();
			break;
		}
		case dtAction_PlayerEscape:
		{
			pPlayerAction = new CBattleAction_PlayerEscape();
			break;
		}
		case dtAction_PlayerExchangePosition:
		{
			pPlayerAction = new CBattleAction_PlayerExchangePosition();
			break;
		}
		case dtAction_PlayerChangePet:
		{
			bool bValue = false;
			int type = ui.comboBox_playerActionValue->currentData().toInt(&bValue);
			if (bValue && type > 0)
			{
				pPlayerAction = new CBattleAction_PlayerChangePet(type);
			}
			else
			{
				QString petName = ui.comboBox_playerActionValue->currentText();
				if (!petName.isEmpty())
					pPlayerAction = new CBattleAction_PlayerChangePet(petName);
			}
			break;
		}
		case dtAction_PlayerUseItem:
		{
			QString itemName = ui.comboBox_playerActionValue->currentText();
			if (!itemName.isEmpty())
				pPlayerAction = new CBattleAction_PlayerUseItem(itemName);
			break;
		}
		case dtAction_PlayerLogBack:
		{
			pPlayerAction = new CBattleAction_PlayerLogBack();
			break;
		}
		case dtAction_PlayerRebirth:
		{
			pPlayerAction = new CBattleAction_PlayerRebirth();
			break;
		}
		case dtAction_PlayerDoNothing:
		{
			pPlayerAction = new CBattleAction_PlayerDoNothing();
			break;
		}
		default:
		{
			if (!playerActionStr.isEmpty())
			{
				bool bValue = false;
				int skillLevel = ui.comboBox_playerActionValue->currentData().toInt(&bValue);
				if (bValue && skillLevel >= 0)
					pPlayerAction = new CBattleAction_PlayerSkillAttack(playerActionStr, skillLevel);
			}
			break;
		}
	}

	CBattleTarget *pPlayerTarget = NULL;
	if (bHasPlayerTarget)
	{
		int playerTargetData = ui.comboBox_playerTarget->currentData().toInt();
		switch (playerTargetData)
		{
			case dtTarget_Enemy:
			{
				int playerTargetSelectData = ui.comboBox_playerTargetSelect->currentData().toInt();
				//if (playerTargetSelectIndex >= 0 && playerTargetSelectIndex < m_targetSelect.size())
				pPlayerTarget = new CBattleTarget_Enemy(playerTargetSelectData);
				break;
			}
			case dtTarget_Teammate:
			{
				int playerTargetSelectData = ui.comboBox_playerTargetSelect->currentData().toInt();
				//if (playerTargetSelectIndex >= 0 && playerTargetSelectIndex < m_targetTeammateSelect.size())
				pPlayerTarget = new CBattleTarget_Teammate(playerTargetSelectData);
				break;
			}
			case dtTarget_Self:
			{
				pPlayerTarget = new CBattleTarget_Self();
				break;
			}
			case dtTarget_Pet:
			{
				pPlayerTarget = new CBattleTarget_Pet();
				break;
			}
			case dtTarget_Condition:
			{
				pPlayerTarget = new CBattleTarget_Condition();
				break;
			}
		}
	}

	CBattleAction *pPetAction = NULL;

	int petActionData = ui.comboBox_petAction->currentData().toInt();
	QString petActionStr = ui.comboBox_petAction->currentText();
	bool bHasPetTarget = ui.comboBox_petTarget->isEnabled();

	switch (petActionData)
	{
		case dtAction_PetIgnore:
		{
			pPetAction = NULL;
			break;
		}
		case dtAction_PetDoNothing:
		{
			QString str = QObject::tr("Do Nothing");
			pPetAction = new CBattleAction_PetSkillAttack(str);
			break;
		}
		default: //dtAction_PetSkillAttack
		{
			if (!petActionStr.isEmpty())
			{
				pPetAction = new CBattleAction_PetSkillAttack(petActionStr);
			}
			break;
		}
	}

	CBattleTarget *pPetTarget = NULL;
	if (pPetAction && bHasPetTarget)
	{
		int petTargetData = ui.comboBox_petTarget->currentData().toInt();
		switch (petTargetData)
		{
			case dtTarget_Enemy:
			{
				int petTargetSelectData = ui.comboBox_petTargetSelect->currentData().toInt();
				//if (petTargetSelectIndex >= 0 && petTargetSelectIndex < m_targetSelect.size())
				pPetTarget = new CBattleTarget_Enemy(petTargetSelectData);
				break;
			}
			case dtTarget_Teammate:
			{
				int petTargetSelectData = ui.comboBox_petTargetSelect->currentData().toInt();
				//	if (petTargetSelectIndex >= 0 && petTargetSelectIndex < m_targetTeammateSelect.size())
				pPetTarget = new CBattleTarget_Teammate(petTargetSelectData);
				break;
			}
			case dtTarget_Self:
			{
				pPetTarget = new CBattleTarget_Self();
				break;
			}
			case dtTarget_Pet:
			{
				pPetTarget = new CBattleTarget_Pet();
				break;
			}
			case dtTarget_Condition:
			{
				pPetTarget = new CBattleTarget_Condition();
				break;
			}
			default:
			{
				//pPetTarget = new CBattleTarget_Condition();
				break;
			}
		}
	}

	CBattleSettingPtr ptr(new CBattleSetting(
			pCondition, pCondition2,
			pPlayerAction, pPlayerTarget,
			pPetAction, pPetTarget /*,
                              pPetAction2, pPetTarget2*/
			));
	if ((pCondition || pCondition2) && (pPlayerAction || pPetAction))
	{
		m_model->appendRow(ptr);
		ui.tableView_settings->resizeRowToContents(m_model->rowCount() - 1);
	}
}

void GameCustomBattleWgt::on_pushButton_del_clicked()
{
	if (ui.tableView_settings->currentIndex().isValid())
	{
		m_model->removeRow(ui.tableView_settings->currentIndex().row());
	}
}

void GameCustomBattleWgt::on_pushButton_moveUp_clicked()
{
	if (ui.tableView_settings->currentIndex().isValid())
	{
		int row = ui.tableView_settings->currentIndex().row();
		if (row > 0)
		{
			m_model->swapRow(row, row - 1);
			ui.tableView_settings->selectRow(row - 1);
		}
	}
}

void GameCustomBattleWgt::on_pushButton_moveDown_clicked()
{
	if (ui.tableView_settings->currentIndex().isValid())
	{
		int row = ui.tableView_settings->currentIndex().row();
		if (row < m_model->rowCount() - 1)
		{
			m_model->swapRow(row, row + 1);
			ui.tableView_settings->selectRow(row + 1);
		}
	}
}

void GameCustomBattleWgt::on_table_customContextMenu(const QPoint &pos)
{
	auto modelIndex = ui.tableView_settings->indexAt(pos);
	if (modelIndex.isValid())
	{
		QMenu menu;
		menu.addAction(QString("上移"), this, SLOT(on_pushButton_moveUp_clicked()));
		menu.addAction(QString("下移"), this, SLOT(on_pushButton_moveDown_clicked()));
		menu.addAction(QString("删除"), this, SLOT(on_pushButton_del_clicked()));
		menu.exec(QCursor::pos());
	}
}

int GameCustomBattleWgt::transConditionCGAValToLocal(int nval)
{
	return m_conditionMap.value(nval);
}

int GameCustomBattleWgt::transActionCGAValToLocal(int nval)
{
	return m_actionMap.value(nval);
}

int GameCustomBattleWgt::transPetActionCGAValToLocal(int nval)
{
	return m_petActionMap.value(nval);
}

int GameCustomBattleWgt::transTargetCGAValToLocal(int nval)
{
	return m_targetMap.value(nval);
}

int GameCustomBattleWgt::transTargetSlectCGAValToLocal(int nval)
{
	return m_targetConditionMap.value(nval);
}

int GameCustomBattleWgt::transConNumRelCGAValToLocal(int nval)
{
	return m_targetConditionNumMap.value(nval);
}

int GameCustomBattleWgt::transConStrRelCGAValToLocal(int nval)
{
	m_targetConditionStrMap.insert(0, dtCompare_Contain);
	m_targetConditionStrMap.insert(1, dtCompare_NotContain);
	return m_targetConditionStrMap.value(nval);
}

//这部分解析 还得写个转换 把CGA值 转devtype
bool GameCustomBattleWgt::ParseBattleSettings(const QJsonValue &val)
{
	if (!val.isObject())
		return false;
	auto obj = val.toObject();
	if (obj.contains("list"))
	{
		m_model->removeRows(0, m_model->rowCount());
		auto list = obj.take("list");
		if (list.isArray())
		{
			auto arr = list.toArray();
			for (auto i = 0; i < arr.size(); ++i)
			{
				auto setting = arr[i].toObject();
				{
					auto conditionTypeId = transConditionCGAValToLocal(setting.take("condition").toInt());
					auto conditionRelId = transConNumRelCGAValToLocal(setting.take("conditionrel").toInt());
					auto conditionValue = setting.take("conditionval").toString();
					ui.comboBox_condition_type->setCurrentIndex(ui.comboBox_condition_type->findData(conditionTypeId));
					on_comboBox_condition_type_currentIndexChanged(ui.comboBox_condition_type->currentIndex());
					ui.comboBox_condition_relation->setCurrentIndex(ui.comboBox_condition_relation->findData(conditionRelId));

					ui.comboBox_condition_value->setCurrentText(conditionValue);
					if (ui.comboBox_condition_type->currentIndex() == dtCondition_TeammateDebuff)
					{
						using namespace CGA;
						if (conditionValue.toInt() == FL_DEBUFF_SLEEP)
							ui.comboBox_condition_value->setCurrentIndex(0);
						else if (conditionValue.toInt() == FL_DEBUFF_MEDUSA)
							ui.comboBox_condition_value->setCurrentIndex(1);
						else if (conditionValue.toInt() == FL_DEBUFF_DRUNK)
							ui.comboBox_condition_value->setCurrentIndex(2);
						else if (conditionValue.toInt() == FL_DEBUFF_CHAOS)
							ui.comboBox_condition_value->setCurrentIndex(3);
						else if (conditionValue.toInt() == FL_DEBUFF_FORGET)
							ui.comboBox_condition_value->setCurrentIndex(4);
						else if (conditionValue.toInt() == FL_DEBUFF_POISON)
							ui.comboBox_condition_value->setCurrentIndex(5);
						else if (conditionValue.toInt() == FL_DEBUFF_ANY)
							ui.comboBox_condition_value->setCurrentIndex(6);
					}
				}

				if (setting.find("condition2") != setting.end())
				{
					auto condition2TypeId = transConditionCGAValToLocal(setting.take("condition2").toInt());
					auto condition2RelId = transConNumRelCGAValToLocal(setting.take("condition2rel").toInt());
					auto condition2Value = setting.take("condition2val").toString();
					ui.comboBox_condition2_type->setCurrentIndex(ui.comboBox_condition2_type->findData(condition2TypeId));
					on_comboBox_condition2_type_currentIndexChanged(ui.comboBox_condition2_type->currentIndex());
					ui.comboBox_condition2_relation->setCurrentIndex(ui.comboBox_condition2_relation->findData(condition2RelId));

					ui.comboBox_condition2_value->setCurrentText(condition2Value);
					if (ui.comboBox_condition2_type->currentIndex() == dtCondition_TeammateDebuff)
					{
						using namespace CGA;
						if (condition2Value.toInt() == FL_DEBUFF_SLEEP)
							ui.comboBox_condition2_value->setCurrentIndex(0);
						else if (condition2Value.toInt() == FL_DEBUFF_MEDUSA)
							ui.comboBox_condition2_value->setCurrentIndex(1);
						else if (condition2Value.toInt() == FL_DEBUFF_DRUNK)
							ui.comboBox_condition2_value->setCurrentIndex(2);
						else if (condition2Value.toInt() == FL_DEBUFF_CHAOS)
							ui.comboBox_condition2_value->setCurrentIndex(3);
						else if (condition2Value.toInt() == FL_DEBUFF_FORGET)
							ui.comboBox_condition2_value->setCurrentIndex(4);
						else if (condition2Value.toInt() == FL_DEBUFF_POISON)
							ui.comboBox_condition2_value->setCurrentIndex(5);
						else if (condition2Value.toInt() == FL_DEBUFF_ANY)
							ui.comboBox_condition2_value->setCurrentIndex(6);
					}
				}

				auto playerActionId = transActionCGAValToLocal(setting.take("playeraction").toInt());
				if (playerActionId == dtAction_PlayerChangePet || playerActionId == dtAction_PlayerUseItem)
				{
					ui.comboBox_playerAction->setCurrentIndex(ui.comboBox_playerAction->findData(playerActionId));
					on_comboBox_playerAction_currentIndexChanged(ui.comboBox_playerAction->currentIndex());
					auto playerActionValue = setting.take("playeractionval").toString();
					ui.comboBox_playerActionValue->setCurrentText(playerActionValue);
				}
				else if (playerActionId == dtAction_PlayerSkillAttack)
				{
					auto playerSkillName = setting.take("playerskillname").toString();
					auto playerSkillLevel = setting.take("playerskilllevel").toInt();

					int playerActionIndex = ui.comboBox_playerAction->findText(playerSkillName);
					if (playerActionIndex == -1)
					{
						ui.comboBox_playerAction->addItem(playerSkillName, dtAction_PlayerSkillAttack);
						playerActionIndex = ui.comboBox_playerAction->findText(playerSkillName);
					}
					ui.comboBox_playerAction->setCurrentIndex(playerActionIndex);
					ui.comboBox_playerActionValue->clear();
					ui.comboBox_playerActionValue->addItem(tr("最高级"), QVariant(0));
					for (auto lv = 1; lv <= 10; ++lv)
						ui.comboBox_playerActionValue->addItem(tr("%1级").arg(lv), QVariant(lv));
					ui.comboBox_playerActionValue->setCurrentIndex(playerSkillLevel);
					ui.comboBox_playerTarget->setEnabled(true);
				}
				else
				{
					ui.comboBox_playerAction->setCurrentIndex(ui.comboBox_playerAction->findData(playerActionId));
					on_comboBox_playerAction_currentIndexChanged(ui.comboBox_playerAction->currentIndex());
				}

				auto playerTargetId = transTargetCGAValToLocal(setting.take("playertarget").toInt());
				auto playerTargetSel = transTargetSlectCGAValToLocal(setting.take("playertargetsel").toInt());
				ui.comboBox_playerTarget->setCurrentIndex(ui.comboBox_playerTarget->findData(playerTargetId));
				on_comboBox_playerTarget_currentIndexChanged(ui.comboBox_playerTarget->currentIndex());
				ui.comboBox_playerTargetSelect->setCurrentIndex(ui.comboBox_playerTargetSelect->findData(playerTargetSel));

				auto petActionId = transPetActionCGAValToLocal(setting.take("petaction").toInt());
				if (petActionId == dtAction_PetSkillAttack)
				{
					auto petSkillName = setting.take("petskillname").toString();
					int petActionIndex = ui.comboBox_petAction->findText(petSkillName);
					if (petActionIndex == -1)
					{
						ui.comboBox_petAction->addItem(petSkillName, dtAction_PetSkillAttack);
						petActionIndex = ui.comboBox_petAction->findText(petSkillName);
					}
					ui.comboBox_petAction->setCurrentIndex(petActionIndex);
					//	ui.comboBox_petAction->setCurrentText(petSkillName);
					//	on_comboBox_petAction_currentIndexChanged(dtAction_PetSkillAttack);
				}
				else
				{
					ui.comboBox_petAction->setCurrentIndex(petActionId);
					on_comboBox_petAction_currentIndexChanged(ui.comboBox_petAction->currentIndex());
				}

				auto petTargetId = transTargetCGAValToLocal(setting.take("pettarget").toInt());
				auto petTargetSel = transTargetSlectCGAValToLocal(setting.take("pettargetsel").toInt());
				ui.comboBox_petTarget->setCurrentIndex(ui.comboBox_petTarget->findData(petTargetId));
				on_comboBox_petTarget_currentIndexChanged(ui.comboBox_petTarget->currentIndex());
				ui.comboBox_petTargetSelect->setCurrentIndex(ui.comboBox_petTargetSelect->findData(petTargetSel));

				ui.pushButton_add->click();
			}
		}
	}

	//SyncAutoBattleWorker();
	return true;
}

void GameCustomBattleWgt::SaveBattleSettings(QJsonObject &obj)
{
	QJsonArray list;
	for (auto i = 0; i < m_model->rowCount(); ++i)
	{
		QJsonObject row;
		auto setting = m_model->BattleSettingFromIndex(m_model->index(i, 0));
		row.insert("index", i);

		row.insert("condition", setting->GetConditionTypeId());
		row.insert("conditionrel", setting->GetConditionRelId());
		QString conditionVauleStr;
		setting->GetConditionValue(conditionVauleStr);
		row.insert("conditionval", conditionVauleStr);

		row.insert("condition2", setting->GetCondition2TypeId());
		row.insert("condition2rel", setting->GetCondition2RelId());
		QString condition2VauleStr;
		setting->GetCondition2Value(condition2VauleStr);
		row.insert("condition2val", condition2VauleStr);

		row.insert("playeraction", setting->GetPlayerActionTypeId());
		if (setting->GetPlayerActionTypeId() == dtAction_PlayerChangePet || setting->GetPlayerActionTypeId() == dtAction_PlayerUseItem)
		{
			QString actionVauleStr;
			setting->GetPlayerActionName(actionVauleStr, true);
			row.insert("playeractionval", actionVauleStr);
		}
		else if (setting->GetPlayerActionTypeId() == dtAction_PlayerSkillAttack)
		{
			row.insert("playerskillname", setting->GetPlayerSkillName());
			row.insert("playerskilllevel", setting->GetPlayerSkillLevel());
		}
		row.insert("playertarget", setting->GetPlayerTargetTypeId());
		row.insert("playertargetsel", setting->GetPlayerTargetSelectId());

		row.insert("petaction", setting->GetPetActionTypeId());
		if (setting->GetPetActionTypeId() == dtAction_PetSkillAttack)
		{
			row.insert("petskillname", setting->GetPetSkillName());
		}
		row.insert("pettarget", setting->GetPetTargetTypeId());
		row.insert("pettargetsel", setting->GetPetTargetSelectId());

		/*row.insert("petaction2", setting->GetPetAction2TypeId());
        if(setting->GetPetAction2TypeId() == BattlePetAction_Skill){
            row.insert("petskill2name", setting->GetPetSkill2Name());
        }
        row.insert("pettarget2", setting->GetPetTarget2TypeId());
        row.insert("pettarget2sel", setting->GetPetTarget2SelectId());*/

		list.insert(i, row);
	}

	obj.insert("list", list);
}

void GameCustomBattleWgt::showToolTip(const QModelIndex &index)
{
	if (!index.isValid())
	{
		qDebug() << "Invalid index";
		return;
	}

	QToolTip::showText(QCursor::pos(), index.data().toString());
}

void GameCustomBattleWgt::OnNotifyGetSkillsInfo(GameSkillList skills)
{
	GameSkillList pSkillList = skills;
	for (int i = 0; i < pSkillList.size(); ++i)
	{
		GameSkillPtr pSkill = pSkillList.at(i);
		if (pSkill->name.isEmpty())
			continue;
		if ((hBattlePlayerActionString.size() + i) < ui.comboBox_playerAction->count())
		{
			ui.comboBox_playerAction->setItemText(hBattlePlayerActionString.size() + i, pSkill->name);
			ui.comboBox_playerAction->setItemData(hBattlePlayerActionString.size() + i, QVariant(pSkill->level));
		}
		else
			ui.comboBox_playerAction->addItem(pSkill->name, QVariant(pSkill->level));
	}

	for (int i = pSkillList.size() + hBattlePlayerActionString.size(); i < ui.comboBox_playerAction->count(); ++i)
	{
		ui.comboBox_playerAction->removeItem(i);
	}
}

void GameCustomBattleWgt::OnNotifyGetItemsInfo(GameItemList items)
{
	m_items = items;
}

void GameCustomBattleWgt::OnNotifyGetPetsInfo(GamePetList pets)
{
	for (int i = 0; i < pets.size(); ++i)
	{
		if (pets.at(i)->battle_flags & 2)
		{
			auto &skills = pets.at(i)->skills;
			for (int j = 0; j < skills.size(); ++j)
			{
				if (hPetActionString.size() + j < ui.comboBox_petAction->count())
				{
					ui.comboBox_petAction->setItemText(hPetActionString.size() + j, skills.at(j)->name);
				}
				else
				{
					ui.comboBox_petAction->addItem(skills.at(j)->name, dtAction_PlayerSkillAttack);
				}
			}

			for (int j = skills.size() + hPetActionString.size(); j < ui.comboBox_petAction->count(); ++j)
			{
				ui.comboBox_petAction->removeItem(j);
			}

			QString str = tr("宠物名称: %1").arg(pets.at(i)->showname);
			if (str != ui.label_petSkillsFrom->text())
			{
				ui.label_petSkillsFrom->setText(str);
			}
			return;
		}
	}
	//没有宠才走
	for (int j = hPetActionString.size(); j < ui.comboBox_petAction->count(); ++j)
	{
		ui.comboBox_petAction->removeItem(j);
	}
	QString str = tr("无宠物");
	if (str != ui.label_petSkillsFrom->text())
		ui.label_petSkillsFrom->setText(str);
	m_pets = pets;
}

void GameCustomBattleWgt::on_pushButton_float_clicked()
{
	emit signal_float_window();
}

void GameCustomBattleWgt::dealTableViewClicked(const QModelIndex &index)
{
	if (!index.isValid())
	{
		qDebug() << "Invalid index";
		return;
	}
	auto pSettings = m_model->BattleSettingFromIndex(index);
	if (!pSettings)
	{
		return;
	}
	if (pSettings->m_condition)
	{
		int conditionTypeId = pSettings->m_condition->GetDevType();
		ui.comboBox_condition_type->setCurrentIndex(ui.comboBox_condition_type->findData(conditionTypeId));
		on_comboBox_condition_type_currentIndexChanged(ui.comboBox_condition_type->currentIndex());
		int conditionRelId = pSettings->m_condition->GetConditionRelId();
		ui.comboBox_condition_relation->setCurrentIndex(ui.comboBox_condition_relation->findData(conditionRelId));

		QString conditionValue;
		pSettings->m_condition->GetConditionValue(conditionValue);
		ui.comboBox_condition_value->setCurrentText(conditionValue);
		if (ui.comboBox_condition_type->currentIndex() == dtCondition_TeammateDebuff)
		{
			if (conditionValue.toInt() == FL_DEBUFF_SLEEP)
				ui.comboBox_condition_value->setCurrentIndex(0);
			else if (conditionValue.toInt() == FL_DEBUFF_MEDUSA)
				ui.comboBox_condition_value->setCurrentIndex(1);
			else if (conditionValue.toInt() == FL_DEBUFF_DRUNK)
				ui.comboBox_condition_value->setCurrentIndex(2);
			else if (conditionValue.toInt() == FL_DEBUFF_CHAOS)
				ui.comboBox_condition_value->setCurrentIndex(3);
			else if (conditionValue.toInt() == FL_DEBUFF_FORGET)
				ui.comboBox_condition_value->setCurrentIndex(4);
			else if (conditionValue.toInt() == FL_DEBUFF_POISON)
				ui.comboBox_condition_value->setCurrentIndex(5);
			else if (conditionValue.toInt() == FL_DEBUFF_ANY)
				ui.comboBox_condition_value->setCurrentIndex(6);
		}
	}
	
	if (pSettings->m_condition2)
	{
		auto condition2TypeId = pSettings->m_condition2->GetDevType();
		auto condition2RelId = pSettings->m_condition2->GetConditionRelId();
		QString condition2Value;
		pSettings->m_condition2->GetConditionValue(condition2Value);
		ui.comboBox_condition2_type->setCurrentIndex(ui.comboBox_condition2_type->findData(condition2TypeId));
		on_comboBox_condition2_type_currentIndexChanged(ui.comboBox_condition2_type->currentIndex());
		ui.comboBox_condition2_relation->setCurrentIndex(ui.comboBox_condition2_relation->findData(condition2RelId));

		ui.comboBox_condition2_value->setCurrentText(condition2Value);
		if (ui.comboBox_condition2_type->currentIndex() == dtCondition_TeammateDebuff)
		{
			if (condition2Value.toInt() == FL_DEBUFF_SLEEP)
				ui.comboBox_condition2_value->setCurrentIndex(0);
			else if (condition2Value.toInt() == FL_DEBUFF_MEDUSA)
				ui.comboBox_condition2_value->setCurrentIndex(1);
			else if (condition2Value.toInt() == FL_DEBUFF_DRUNK)
				ui.comboBox_condition2_value->setCurrentIndex(2);
			else if (condition2Value.toInt() == FL_DEBUFF_CHAOS)
				ui.comboBox_condition2_value->setCurrentIndex(3);
			else if (condition2Value.toInt() == FL_DEBUFF_FORGET)
				ui.comboBox_condition2_value->setCurrentIndex(4);
			else if (condition2Value.toInt() == FL_DEBUFF_POISON)
				ui.comboBox_condition2_value->setCurrentIndex(5);
			else if (condition2Value.toInt() == FL_DEBUFF_ANY)
				ui.comboBox_condition2_value->setCurrentIndex(6);
		}
	}
	
	if (pSettings->m_playerAction)
	{
		auto playerActionId = pSettings->m_playerAction->GetDevType();
		if (playerActionId == dtAction_PlayerChangePet || playerActionId == dtAction_PlayerUseItem)
		{
			ui.comboBox_playerAction->setCurrentIndex(ui.comboBox_playerAction->findData(playerActionId));
			on_comboBox_playerAction_currentIndexChanged(ui.comboBox_playerAction->currentIndex());
			QString playerActionValue;
			pSettings->m_playerAction->GetActionName(playerActionValue, true);
			ui.comboBox_playerActionValue->setCurrentText(playerActionValue);
		}
		else if (playerActionId == dtAction_PlayerSkillAttack)
		{
			auto playerSkillAction = (CBattleAction_PlayerSkillAttack *)pSettings->m_playerAction;

			QString playerSkillName = playerSkillAction->GetSkillName();
			auto playerSkillLevel = playerSkillAction->GetSkillLevel();

			int playerActionIndex = ui.comboBox_playerAction->findText(playerSkillName);
			if (playerActionIndex == -1)
			{
				ui.comboBox_playerAction->addItem(playerSkillName, dtAction_PlayerSkillAttack);
				playerActionIndex = ui.comboBox_playerAction->findText(playerSkillName);
			}
			ui.comboBox_playerAction->setCurrentIndex(playerActionIndex);
			ui.comboBox_playerActionValue->clear();
			ui.comboBox_playerActionValue->addItem(tr("最高级"), QVariant(0));
			for (auto lv = 1; lv <= 10; ++lv)
				ui.comboBox_playerActionValue->addItem(tr("%1级").arg(lv), QVariant(lv));
			ui.comboBox_playerActionValue->setCurrentIndex(playerSkillLevel);
			ui.comboBox_playerTarget->setEnabled(true);
		}
		else
		{
			ui.comboBox_playerAction->setCurrentIndex(ui.comboBox_playerAction->findData(playerActionId));
			on_comboBox_playerAction_currentIndexChanged(ui.comboBox_playerAction->currentIndex());
		}
	}
	
	if (pSettings->m_playerTarget)
	{
		auto playerTargetId = pSettings->m_playerTarget->GetDevType();
		auto playerTargetSel = pSettings->m_playerTarget->GetTargetSelectId();
		ui.comboBox_playerTarget->setCurrentIndex(ui.comboBox_playerTarget->findData(playerTargetId));
		on_comboBox_playerTarget_currentIndexChanged(ui.comboBox_playerTarget->currentIndex());
		ui.comboBox_playerTargetSelect->setCurrentIndex(ui.comboBox_playerTargetSelect->findData(playerTargetSel));
	}
	

	if (pSettings->m_petAction)
	{
		auto petActionId = pSettings->m_petAction->GetDevType();
		if (petActionId == dtAction_PetSkillAttack)
		{
			auto petSkillAction = (CBattleAction_PetSkillAttack *)pSettings->m_petAction;
			auto petSkillName = petSkillAction->GetSkillName();
			int petActionIndex = ui.comboBox_petAction->findText(petSkillName);
			if (petActionIndex == -1)
			{
				ui.comboBox_petAction->addItem(petSkillName, dtAction_PetSkillAttack);
				petActionIndex = ui.comboBox_petAction->findText(petSkillName);
			}
			ui.comboBox_petAction->setCurrentIndex(petActionIndex);
			//	ui.comboBox_petAction->setCurrentText(petSkillName);
			//	on_comboBox_petAction_currentIndexChanged(dtAction_PetSkillAttack);
		}
		else
		{
			ui.comboBox_petAction->setCurrentIndex(petActionId);
			on_comboBox_petAction_currentIndexChanged(ui.comboBox_petAction->currentIndex());
		}

	}
	if (pSettings->m_petTarget)
	{

		auto petTargetId = pSettings->m_petTarget->GetDevType();
		auto petTargetSel = pSettings->m_petTarget->GetTargetSelectId();
		ui.comboBox_petTarget->setCurrentIndex(ui.comboBox_petTarget->findData(petTargetId));
		on_comboBox_petTarget_currentIndexChanged(ui.comboBox_petTarget->currentIndex());
		ui.comboBox_petTargetSelect->setCurrentIndex(ui.comboBox_petTargetSelect->findData(petTargetSel));
	}
}

void GameCustomBattleWgt::doLoadUserConfig(QSettings &iniFile)
{
	//战斗设置读取
	auto LoadSettingCfg = [&](int i)
	{
		bool bTrans = false;

		QString sDevType = iniFile.value(QString("ActionCondType%1").arg(i)).toString();
		int nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		CBattleCondition *pCond = (CBattleCondition *)g_battleModuleReg.CreateNewBattleObj(nDevType);
		int nTextCondCount = iniFile.value(QString("ActionCondTextValCount%1").arg(i)).toInt();
		QStringList textCondData;
		for (int n = 0; n < nTextCondCount; ++n)
		{
			textCondData.append(iniFile.value(QString("ActionCondTextVal%1-%2").arg(i).arg(n)).toString());
		}
		pCond->SetConditionTextData(textCondData);

		int nCondDataCount = iniFile.value(QString("ActionCondValCount%1").arg(i)).toInt();
		QList<int> condData;
		for (int n = 0; n < nCondDataCount; ++n)
		{
			QVariant tmpVal = iniFile.value(QString("ActionCondVal%1-%2").arg(i).arg(n));
			if (n == 0)
			{
				if (g_pAutoBattleCtrl->GetBattleTypeFromText(tmpVal.toString()) == 0)
					condData.append(tmpVal.toInt());
				else
					condData.append(g_pAutoBattleCtrl->GetBattleTypeFromText(tmpVal.toString()));
			}
			else
				condData.append(tmpVal.toInt());
		}
		pCond->SetConditionData(condData);
		
		CBattleCondition *pCond2 = nullptr;
		QString sDevType2 = iniFile.value(QString("ActionCondType2-%1").arg(i)).toString();
		nDevType = sDevType2.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType2);
		if (nDevType!=0)
		{
			pCond2 = (CBattleCondition *)g_battleModuleReg.CreateNewBattleObj(nDevType);
			if (pCond2)
			{
				nTextCondCount = iniFile.value(QString("ActionCondTextValCount2-%1").arg(i)).toInt();
				QStringList textCondData2;
				for (int n = 0; n < nTextCondCount; ++n)
				{
					textCondData2.append(iniFile.value(QString("ActionCondTextVal2-%1-%2").arg(i).arg(n)).toString());
				}
				pCond2->SetConditionTextData(textCondData2);

				nCondDataCount = iniFile.value(QString("ActionCondValCount2-%1").arg(i)).toInt();
				QList<int> condData2;
				for (int n = 0; n < nCondDataCount; ++n)
				{
					QVariant tmpVal = iniFile.value(QString("ActionCondVal2-%1-%2").arg(i).arg(n));
					if (n == 0)
					{
						if (g_pAutoBattleCtrl->GetBattleTypeFromText(tmpVal.toString()) == 0)
							condData2.append(tmpVal.toInt());
						else
							condData2.append(g_pAutoBattleCtrl->GetBattleTypeFromText(tmpVal.toString()));
					}
					else
						condData2.append(tmpVal.toInt());
				}
				pCond2->SetConditionData(condData2);
			}			
		}

		//	nDevType = iniFile.value(QString("ActionType%1").arg(i)).toInt();
		sDevType = iniFile.value(QString("ActionType%1").arg(i)).toString();
		nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		CBattleAction *pAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(nDevType);
		int nActionTextValCount = 0;
		int nActionValCount = 0;
		QStringList textAction;
		QList<int> actionData;
		if (pAction)
		{
			int nActionTextValCount = iniFile.value(QString("ActionTextValCount%1").arg(i)).toInt();
			for (int n = 0; n < nActionTextValCount; ++n)
			{
				textAction.append(iniFile.value(QString("ActionTextVal%1-%2").arg(i).arg(n)).toString());
			}
			pAction->SetActionTextData(textAction);

			int nActionValCount = iniFile.value(QString("ActionValCount%1").arg(i)).toInt();

			for (int n = 0; n < nActionValCount; ++n)
			{
				QVariant tmpVal = iniFile.value(QString("ActionVal%1-%2").arg(i).arg(n));
				actionData.append(tmpVal.toInt());
			}
			pAction->SetActionData(actionData);
		}
		sDevType = iniFile.value(QString("PetActionType%1").arg(i)).toString();
		nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		CBattleAction *pPetAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(nDevType);
		if (pPetAction)
		{
			nActionTextValCount = iniFile.value(QString("PetActionTextValCount%1").arg(i)).toInt();
			textAction.clear();
			for (int n = 0; n < nActionTextValCount; ++n)
			{
				textAction.append(iniFile.value(QString("PetActionTextVal%1-%2").arg(i).arg(n)).toString());
			}
			pPetAction->SetActionTextData(textAction);

			nActionValCount = iniFile.value(QString("PetActionValCount%1").arg(i)).toInt();
			actionData.clear();
			for (int n = 0; n < nActionValCount; ++n)
			{
				QVariant tmpVal = iniFile.value(QString("PetActionVal%1-%2").arg(i).arg(n));
				actionData.append(tmpVal.toInt());
			}
			pPetAction->SetActionData(actionData);
		}

		//	nDevType = iniFile.value(QString("TargetType%1").arg(i)).toInt();
		sDevType = iniFile.value(QString("TargetType%1").arg(i)).toString();
		nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		int tgtVal = 0;
		QString sTgtVal;
		CBattleTarget *pTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(nDevType);
		if (pTarget)
		{
			sTgtVal = iniFile.value(QString("TargetVal%1").arg(i)).toString();
			tgtVal = sTgtVal.toInt(&bTrans);
			if (!bTrans) //字符串 转为int
				tgtVal = g_pAutoBattleCtrl->GetBattleTypeFromText(sTgtVal);
			pTarget->SetTargetVal(tgtVal);
			pTarget->SetTargetSpecialName(iniFile.value(QString("TargetSpcName%1").arg(i)).toString());
		}

		//nDevType = iniFile.value(QString("PetTargetType%1").arg(i)).toInt();
		sDevType = iniFile.value(QString("PetTargetType%1").arg(i)).toString();
		nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		CBattleTarget *pPetTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(nDevType);
		if (pPetTarget)
		{
			sTgtVal = iniFile.value(QString("PetTargetVal%1").arg(i)).toString();
			tgtVal = sTgtVal.toInt(&bTrans);
			if (!bTrans) //字符串 转为int
				tgtVal = g_pAutoBattleCtrl->GetBattleTypeFromText(sTgtVal);
			pPetTarget->SetTargetVal(tgtVal);
			pPetTarget->SetTargetSpecialName(iniFile.value(QString("PetTargetSpcName%1").arg(i)).toString());
		}
		if (pCond2==nullptr)
			pCond2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCond, pCond2, pAction, pTarget, pPetAction, pPetTarget));
		return ptr;
	};
	//战斗设置

	iniFile.beginGroup("AutoCustomAction");
	g_pAutoBattleCtrl->ClearAllCustomSetting();
	m_model->clearAllSetting();
	//1-10敌人设置

	int nSettingCount = iniFile.value("count").toInt();
	for (int i = 0; i < nSettingCount; ++i)
	{
		auto ptr = LoadSettingCfg(i);
		if (ptr)
		{
			auto pCond = ptr->m_condition;
			QString szVal;
			pCond->GetConditionValue(szVal);
			g_pAutoBattleCtrl->AddOneCustomSetting(ptr);
			m_model->appendRow(ptr);
			ui.tableView_settings->resizeRowToContents(m_model->rowCount() - 1);
		}
	}

	iniFile.endGroup();
}

int GameCustomBattleWgt::calcTextRow(QFont &font, QString sText, int totalWidth)
{
	//计算文字所占行数
	QFontMetrics fm(font);
	int nWidth = fm.width(sText);
	QString sCalcText = sText + "  ";
	int pixWidth = fm.width(sCalcText);
	int pixHeight = fm.height();
	if (pixWidth % totalWidth == 0)
		return pixWidth / totalWidth;
	else
		return pixWidth / totalWidth + 1;
}

void GameCustomBattleWgt::showEvent(QShowEvent *event)
{
	ui.tableView_settings->resizeRowsToContents();
}

void GameCustomBattleWgt::doSaveUserConfig(QSettings &iniFile)
{
	auto SaveOneSettingFun = [&](CBattleSettingPtr pSetting, int index)
	{
		CBattleCondition *pCond = pSetting->m_condition;
		iniFile.setValue(QString("ActionCondType%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pCond->GetDevType()));
		QStringList textCondData = pCond->GetConditionTextData();
		for (int i = 0; i < textCondData.size(); ++i)
		{
			iniFile.setValue(QString("ActionCondTextVal%1-%2").arg(index).arg(i), textCondData.at(i));
		}
		iniFile.setValue(QString("ActionCondTextValCount%1").arg(index), textCondData.size());
		QList<int> condData = pCond->GetConditionData();
		for (int i = 0; i < condData.size(); ++i)
		{
			if (i == 0)
				iniFile.setValue(QString("ActionCondVal%1-%2").arg(index).arg(i), g_pAutoBattleCtrl->GetBattleTypeText(condData.at(i)));
			else
				iniFile.setValue(QString("ActionCondVal%1-%2").arg(index).arg(i), condData.at(i));
		}
		iniFile.setValue(QString("ActionCondValCount%1").arg(index), condData.size());
		
		CBattleCondition *pCond2 = pSetting->m_condition2;
		iniFile.setValue(QString("ActionCondType2-%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pCond2->GetDevType()));
		QStringList textCondData2 = pCond2->GetConditionTextData();
		for (int i = 0; i < textCondData2.size(); ++i)
		{
			iniFile.setValue(QString("ActionCondTextVal2-%1-%2").arg(index).arg(i), textCondData2.at(i));
		}
		iniFile.setValue(QString("ActionCondTextValCount2-%1").arg(index), textCondData2.size());
		QList<int> condData2 = pCond2->GetConditionData();
		for (int i = 0; i < condData2.size(); ++i)
		{
			if (i == 0)
				iniFile.setValue(QString("ActionCondVal2-%1-%2").arg(index).arg(i), g_pAutoBattleCtrl->GetBattleTypeText(condData2.at(i)));
			else
				iniFile.setValue(QString("ActionCondVal2-%1-%2").arg(index).arg(i), condData2.at(i));
		}
		iniFile.setValue(QString("ActionCondValCount2-%1").arg(index), condData2.size());

		CBattleAction *pAction = pSetting->m_playerAction;
		QStringList textAction;
		QList<int> actionData;
		if (pAction)
		{
			iniFile.setValue(QString("ActionType%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pAction->GetDevType()));
			textAction = pAction->GetActionTextData();
			for (int i = 0; i < textAction.size(); ++i)
			{
				iniFile.setValue(QString("ActionTextVal%1-%2").arg(index).arg(i), textAction.at(i));
			}
			iniFile.setValue(QString("ActionTextValCount%1").arg(index), textAction.size());

			actionData = pAction->GetActionData();
			for (int i = 0; i < actionData.size(); ++i)
			{
				iniFile.setValue(QString("ActionVal%1-%2").arg(index).arg(i), actionData.at(i));
			}
			iniFile.setValue(QString("ActionValCount%1").arg(index), actionData.size());
		}
		CBattleAction *pPetAction = pSetting->m_petAction;
		if (pPetAction)
		{
			iniFile.setValue(QString("PetActionType%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pPetAction->GetDevType()));
			textAction = pPetAction->GetActionTextData();
			for (int i = 0; i < textAction.size(); ++i)
			{
				iniFile.setValue(QString("PetActionTextVal%1-%2").arg(index).arg(i), textAction.at(i));
			}
			iniFile.setValue(QString("PetActionTextValCount%1").arg(index), textAction.size());

			actionData = pPetAction->GetActionData();
			for (int i = 0; i < actionData.size(); ++i)
			{
				iniFile.setValue(QString("PetActionVal%1-%2").arg(index).arg(i), actionData.at(i));
			}
			iniFile.setValue(QString("PetActionValCount%1").arg(index), actionData.size());
		}

		int targetID = 0;
		CBattleTarget *pTarget = pSetting->m_playerTarget;
		if (pTarget)
		{
			targetID = pTarget->GetTargetSelectId();
			iniFile.setValue(QString("TargetVal%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(targetID));
			iniFile.setValue(QString("TargetType%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pTarget->GetDevType()));
			iniFile.setValue(QString("TargetSpcName%1").arg(index), pTarget->GetTargetSpecialName());
		}

		CBattleTarget *pPetTarget = pSetting->m_petTarget;
		if (pPetTarget)
		{
			targetID = pPetTarget->GetTargetSelectId();
			iniFile.setValue(QString("PetTargetVal%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(targetID));
			iniFile.setValue(QString("PetTargetType%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pPetTarget->GetDevType()));
			iniFile.setValue(QString("PetTargetSpcName%1").arg(index), pPetTarget->GetTargetSpecialName());
		}
	};
	auto SaveSettingFun = [&](CBattleSettingList pSettingList)
	{
		int nSettingCount = 0;
		foreach (auto pSetting, pSettingList)
		{
			SaveOneSettingFun(pSetting, nSettingCount);
			nSettingCount++;
		}
		return nSettingCount;
	};
	iniFile.beginGroup("AutoCustomAction");
	//自定义配置
	CBattleSettingList pSettingList = g_pAutoBattleCtrl->GetCustomSettings();
	int nSettingCount = SaveSettingFun(pSettingList);
	iniFile.setValue("count", nSettingCount);
	iniFile.endGroup();
}