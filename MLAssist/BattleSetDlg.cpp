#include "BattleSetDlg.h"
#include "GameCtrl.h"
#include "constDef.h"
#include "stdafx.h"
BattleSetDlg::BattleSetDlg(QWidget *parent) :
		QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	m_sCopyToMultiName = "个敌";
	ui.comboBox_playerAction->setView(new QListView());
	ui.comboBox_playerTarget->setView(new QListView());
	ui.comboBox_playerSkillLv->setView(new QListView());
	ui.comboBox_useItem->setView(new QListView());
	ui.comboBox_ChangePet->setView(new QListView());
	ui.comboBox_playerTargetSelect->setView(new QListView());

	ui.comboBox_petAction->setView(new QListView());
	ui.comboBox_petTarget->setView(new QListView());
	ui.comboBox_playerSkillLv->setView(new QListView());
	ui.comboBox_useItem->setView(new QListView());
	ui.comboBox_ChangePet->setView(new QListView());
	ui.comboBox_petTargetSelect->setView(new QListView());
}

BattleSetDlg::~BattleSetDlg()
{
}
//初始化下拉项
void BattleSetDlg::init()
{
	QMap<int, QString> hBattlePlayerActionString; //人物动作
	QMap<int, QString> hBattleTargetString;		  //目标
	QMap<int, QString> hPetActionString;		  //宠物动作
	QMap<int, QString> hTargetCondString;		  //优先规则

	hBattlePlayerActionString.insert(dtAction_PlayerAttack, "攻击");
	hBattlePlayerActionString.insert(dtAction_PlayerGuard, "防御");
	hBattlePlayerActionString.insert(dtAction_PlayerEscape, "逃跑");
	hBattlePlayerActionString.insert(dtAction_PlayerExchangePosition, "换位");
	hBattlePlayerActionString.insert(dtAction_PlayerChangePet, "召唤宠物");
	hBattlePlayerActionString.insert(dtAction_PlayerUseItem, "使用物品");
	//hBattlePlayerActionString.insert(dtAction_PlayerSkillAttack, "技能攻击");
	hBattlePlayerActionString.insert(dtAction_PlayerLogBack, "登出");
	hBattlePlayerActionString.insert(dtAction_PlayerDoNothing, "什么也不做");

	hPetActionString.insert(dtAction_PetDoNothing, "什么也不做");

	hBattleTargetString.insert(dtTarget_Enemy, "敌人");
	hBattleTargetString.insert(dtTarget_Teammate, "队友");
	hBattleTargetString.insert(dtTarget_Self, "人物");
	hBattleTargetString.insert(dtTarget_Pet, "宠物");

	hTargetCondString.insert(dtTargetCondition_Random, "随机");
	hTargetCondString.insert(dtTargetCondition_Front, "前排");
	hTargetCondString.insert(dtTargetCondition_Back, "后排");
	hTargetCondString.insert(dtTargetCondition_LowHP, "血少优先");
	hTargetCondString.insert(dtTargetCondition_HighHP, "血多优先");
	hTargetCondString.insert(dtTargetCondition_LowHPPercent, "低血量比例");
	hTargetCondString.insert(dtTargetCondition_HighHPPercent, "高血量比例");
	hTargetCondString.insert(dtTargetCondition_LowLv, "级低优先");
	hTargetCondString.insert(dtTargetCondition_HighLv, "级高优先");
	hTargetCondString.insert(dtTargetCondition_SingleDebuff, "单体异常状态");
	hTargetCondString.insert(dtTargetCondition_MulTDebuff, "T字形异常状态");
	hTargetCondString.insert(dtTargetCondition_Goatfarm, "砍牛");
	hTargetCondString.insert(dtTargetCondition_Boomerang, "回力标(人多的一排)");
	hTargetCondString.insert(dtTargetCondition_LessUnitRow, "人少的一排");
	hTargetCondString.insert(dtTargetCondition_MultiMagic, "强力魔法");
	hTargetCondString.insert(dtTargetCondition_AimOrderUpDown, "上到下");
	hTargetCondString.insert(dtTargetCondition_AimOrderDownUp, "下到上");
	hTargetCondString.insert(dtTargetCondition_AimOrderCenter, "中间");

	//人物动作 技能
	for (auto it = hBattlePlayerActionString.begin(); it != hBattlePlayerActionString.end(); ++it)
	{
		if (it.value().isEmpty())
			continue;
		ui.comboBox_playerAction->addItem(it.value(), it.key());
	}
	GameSkillList pSkillList = g_pGameCtrl->getGameSkills(); // g_pGameFun->GetGameSkills();
	for (int i = 0; i < pSkillList.size(); ++i)
	{
		GameSkillPtr pSkill = pSkillList.at(i);
		if (pSkill->name.isEmpty())
			continue;
		ui.comboBox_playerAction->addItem(pSkill->name, QVariant(pSkill->index));
	}
	//人宠攻击目标
	for (auto it = hBattleTargetString.begin(); it != hBattleTargetString.end(); ++it)
	{
		if (it.value().isEmpty())
			continue;
		ui.comboBox_playerTarget->addItem(it.value(), it.key());
		ui.comboBox_petTarget->addItem(it.value(), it.key());
	}

	//人物技能等级
	ui.comboBox_playerSkillLv->addItem("最高级", 0);
	for (int i = 1; i <= 10; ++i)
	{
		ui.comboBox_playerSkillLv->addItem(QString("%1级").arg(i), i);
	}

	//敌人优先规则
	for (auto it = hTargetCondString.begin(); it != hTargetCondString.end(); ++it)
	{
		if (it.value().isEmpty())
			continue;
		ui.comboBox_playerTargetSelect->addItem(it.value(), it.key());
		ui.comboBox_petTargetSelect->addItem(it.value(), it.key());
	}
	//宠物技能动作
	GamePetList pPetList = g_pGameCtrl->getGamePets(); //g_pGameFun->GetGamePets();
	for (int i = 0; i < pPetList.size(); ++i)
	{
		if (pPetList[i]->exist == false)
			continue;
		ui.comboBox_ChangePet->addItem(pPetList[i]->showname, QVariant(TChangePet_Call));
		if (pPetList.at(i)->battle_flags & 2)
		{
			GameSkillList pPetSkillList = pPetList.at(i)->skills;
			for (int j = 0; j < pPetSkillList.size(); ++j)
			{
				if (pPetSkillList.at(j)->name.isEmpty())
					continue;
				ui.comboBox_petAction->addItem(pPetSkillList.at(j)->name, pPetSkillList.at(j)->index);
			}
		}
	}
	ui.comboBox_ChangePet->addItem("宠物收回", QVariant(TChangePet_Recall));
	ui.comboBox_ChangePet->addItem("召唤最高等级宠", QVariant(TChangePet_HighestLv));
	ui.comboBox_ChangePet->addItem("召唤最高血量宠", QVariant(TChangePet_HighestHp));
	ui.comboBox_ChangePet->addItem("召唤最高魔量宠", QVariant(TChangePet_HighestMp));
	ui.comboBox_ChangePet->addItem("召唤最低等级宠", QVariant(TChangePet_LowestLv));
	ui.comboBox_ChangePet->addItem("召唤最低血量宠", QVariant(TChangePet_LowestHp));
	ui.comboBox_ChangePet->addItem("召唤最低魔量宠", QVariant(TChangePet_LowestMp));
	ui.comboBox_ChangePet->addItem("召唤最高忠诚宠", QVariant(TChangePet_HighestLoyalty));
	ui.comboBox_ChangePet->addItem("召唤最低忠诚宠", QVariant(TChangePet_LowestLoyalty));

	//宠物默认动作  什么也不做
	for (auto it = hPetActionString.begin(); it != hPetActionString.end(); ++it)
	{
		if (it.value().isEmpty())
			continue;
		ui.comboBox_petAction->addItem(it.value(), it.key());
	}

	auto pItemList = g_pGameCtrl->getGameItems(); // g_pGameFun->GetGameItems();
	for (int i = 0; i < pItemList.size(); ++i)
	{
		if (pItemList.at(i)->exist)
		{
			ui.comboBox_useItem->addItem(pItemList.at(i)->name, QVariant(pItemList.at(i)->id));
		}
	}
	//复制到其他敌人
	m_pAllItem = new QListWidgetItem("全部");
	m_pAllItem->setCheckState(Qt::Unchecked);
	ui.listWidget_copySet->addItem(m_pAllItem);
	for (int i = 0; i < 10; ++i)
	{
		QListWidgetItem *pItem = new QListWidgetItem(QString("%1%2").arg(i + 1).arg(m_sCopyToMultiName));
		pItem->setData(Qt::UserRole, i + 1);
		pItem->setCheckState(Qt::Unchecked);
		ui.listWidget_copySet->addItem(pItem);
	}
	connect(ui.listWidget_copySet, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(doItemClicked(QListWidgetItem *)));
	setBattleSetting(m_pSetting);
}

void BattleSetDlg::setBattleSetting(CBattleSettingPtr &pSetting)
{
	m_pSetting = pSetting;
	if (m_pSetting)
	{
		//这里只用来预置，默认不能修改，只能删除原有再创建
		CBattleAction *pPlayerAction = m_pSetting->m_playerAction;
		if (pPlayerAction)
		{
			int nDevType = pPlayerAction->GetDevType();
			int index = ui.comboBox_playerAction->findData(nDevType);
			ui.comboBox_playerAction->setCurrentIndex(index);
			if (pPlayerAction->GetDevType() == dtAction_PlayerSkillAttack)
			{
				int nLv = ((CBattleAction_PlayerSkillAttack *)pPlayerAction)->GetSkillLevel();
				QString szkillName = ((CBattleAction_PlayerSkillAttack *)pPlayerAction)->GetSkillName();
				g_pGameCtrl->signal_addOneChat(QString("%1-%2-%3").arg(szkillName).arg(nLv).arg(nDevType));
				ui.comboBox_playerAction->setCurrentIndex(ui.comboBox_playerAction->findText(szkillName));
				ui.comboBox_playerSkillLv->setCurrentIndex(ui.comboBox_playerSkillLv->findData(nLv));
			}
			if (pPlayerAction->GetDevType() == dtAction_PlayerUseItem)
			{
				QStringList actionTextList = ((CBattleAction_PlayerUseItem *)pPlayerAction)->GetActionTextData();
				if (actionTextList.size() > 0)
				{
					QString sItemName = actionTextList.at(0);
					index = ui.comboBox_useItem->findText(sItemName);
					ui.comboBox_useItem->setCurrentIndex(index);
				}
			}
		}
		CBattleTarget *pPlayerTarget = m_pSetting->m_playerTarget;
		if (pPlayerTarget)
		{
			int nDevType = pPlayerTarget->GetDevType();
			int index = ui.comboBox_playerTarget->findData(nDevType);
			ui.comboBox_playerTarget->setCurrentIndex(index);

			int nSelectDevType = pPlayerTarget->GetTargetSelectId();
			index = ui.comboBox_playerTargetSelect->findData(nSelectDevType);
			ui.comboBox_playerTargetSelect->setCurrentIndex(index);
			QString specialName = pPlayerTarget->GetTargetSpecialName();
			if (!specialName.isEmpty())
				ui.checkBox_playerSpecial->setChecked(true);
			ui.lineEdit_playerSpecialName->setText(specialName);
		}
		CBattleAction *pPetAction = m_pSetting->m_petAction;
		if (pPetAction)
		{
			int nDevType = pPetAction->GetDevType();
			int index = ui.comboBox_petAction->findData(nDevType);
			ui.comboBox_petAction->setCurrentIndex(index);
			if (nDevType == dtAction_PetSkillAttack)
			{
				QString petSkillName = ((CBattleAction_PetSkillAttack *)pPetAction)->GetSkillName();
				ui.comboBox_petAction->setCurrentIndex(ui.comboBox_petAction->findText(petSkillName));
			}
		}
		CBattleTarget *pPetTarget = m_pSetting->m_petTarget;
		if (pPetTarget)
		{
			int nDevType = pPetTarget->GetDevType();
			int index = ui.comboBox_petTarget->findData(nDevType);
			ui.comboBox_petTarget->setCurrentIndex(index);

			int nSelectDevType = pPetTarget->GetTargetSelectId();
			index = ui.comboBox_petTargetSelect->findData(nSelectDevType);
			ui.comboBox_petTargetSelect->setCurrentIndex(index);
			QString specialName = pPetTarget->GetTargetSpecialName();
			if (!specialName.isEmpty())
				ui.checkBox_petSpecial->setChecked(true);
			ui.lineEdit_petSpecialName->setText(specialName);
		}
	}
}

void BattleSetDlg::SetAllListWidgetItemCheckState(Qt::CheckState nCheckState)
{
	int ncount = ui.listWidget_copySet->count();
	for (int i = 0; i < ncount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget_copySet->item(i);
		if (pItem == m_pAllItem)
			continue;
		pItem->setCheckState(nCheckState);
	}
}

void BattleSetDlg::SetCopyToMultiBattleGroupName(const QString &name)
{
	ui.groupBox_copyToMult->setTitle(name);
}

void BattleSetDlg::SetCopyToMultiBattleListName(const QString &name)
{
	m_sCopyToMultiName = name;
}

//人物动作
void BattleSetDlg::on_comboBox_playerAction_currentIndexChanged(int index)
{
	int battleType = ui.comboBox_playerAction->itemData(index, Qt::UserRole).toInt();
	if (battleType == dtAction_PlayerChangePet)
	{
		ui.comboBox_ChangePet->setEnabled(true);
		ui.comboBox_useItem->setEnabled(false);
		ui.label_ChangePet->setEnabled(true);
		ui.label_useItem->setEnabled(false);
	}
	else if (battleType == dtAction_PlayerUseItem)
	{
		ui.comboBox_useItem->setEnabled(true);
		ui.comboBox_ChangePet->setEnabled(false);
		ui.label_ChangePet->setEnabled(false);
		ui.label_useItem->setEnabled(true);
	}
	else
	{
		ui.comboBox_useItem->setEnabled(false);
		ui.comboBox_ChangePet->setEnabled(false);
		ui.label_ChangePet->setEnabled(false);
		ui.label_useItem->setEnabled(false);
	}
}

void BattleSetDlg::on_pushButton_ok_clicked()
{
	//只生成操作  判断不在此
	CBattleAction *pPlayerAction = NULL;

	QString playerActionStr = ui.comboBox_playerAction->currentText();
	int playerActionIndex = ui.comboBox_playerAction->currentIndex();
	playerActionIndex = ui.comboBox_playerAction->itemData(playerActionIndex, Qt::UserRole).toInt();
	bool bHasPlayerTarget = ui.comboBox_playerTarget->isEnabled();
	switch (playerActionIndex)
	{
		case dtAction_PlayerAttack:
		case dtAction_PlayerGuard:
		case dtAction_PlayerEscape:
		case dtAction_PlayerExchangePosition:
		case dtAction_PlayerLogBack:
		case dtAction_PlayerRebirth:
		case dtAction_PlayerDoNothing:
		{
			pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(playerActionIndex);
			break;
		}
		case dtAction_PlayerChangePet:
		{
			int nChangePetType = ui.comboBox_ChangePet->currentData().toInt();
			if (nChangePetType == TChangePet_Call)
			{
				QString petName = ui.comboBox_ChangePet->currentText();
				if (!petName.isEmpty())
					pPlayerAction = new CBattleAction_PlayerChangePet(petName);
			}
			else
			{
				pPlayerAction = new CBattleAction_PlayerChangePet(nChangePetType);
			}
			break;
		}
		case dtAction_PlayerUseItem:
		{
			QString itemName = ui.comboBox_useItem->currentText();
			if (!itemName.isEmpty())
				pPlayerAction = new CBattleAction_PlayerUseItem(itemName);
			else
			{
				QMessageBox::information(this, "错误:", "物品为空!", "确定");
				return;
			}
			break;
		}
		default:
		{ //使用技能
			if (!playerActionStr.isEmpty())
			{
				bool bValue = false;
				int skillLevel = ui.comboBox_playerSkillLv->currentData().toInt(&bValue);
				g_pGameCtrl->signal_addOneChat(QString("%1-%2").arg(playerActionStr).arg(skillLevel));

				if (bValue && skillLevel >= 0)
					pPlayerAction = new CBattleAction_PlayerSkillAttack(playerActionStr, skillLevel);
			}
			else
			{
				QMessageBox::information(this, "错误:", "技能为空!", "确定");
				return;
			}
			break;
		}
	}
	//攻击目标
	CBattleTarget *pPlayerTarget = NULL;
	if (bHasPlayerTarget)
	{
		int playerTargetIndex = ui.comboBox_playerTarget->currentIndex();
		playerTargetIndex = ui.comboBox_playerTarget->itemData(playerTargetIndex, Qt::UserRole).toInt();
		switch (playerTargetIndex)
		{
			case dtTarget_Enemy:
			{
				QString specialName;
				if (ui.checkBox_playerSpecial->isChecked())
					specialName = ui.lineEdit_playerSpecialName->text();
				int playerTargetSelectIndex = ui.comboBox_playerTargetSelect->currentIndex();
				playerTargetSelectIndex = ui.comboBox_playerTargetSelect->itemData(playerTargetSelectIndex, Qt::UserRole).toInt();

				if (playerTargetSelectIndex >= 0)
					pPlayerTarget = new CBattleTarget_Enemy(playerTargetSelectIndex, specialName);
				else
				{
					QMessageBox::information(this, "错误:", "指定目标错误!", "确定");
					return;
				}
				break;
			}
			case dtTarget_Teammate:
			{
				int playerTargetSelectIndex = ui.comboBox_playerTargetSelect->currentIndex();
				playerTargetSelectIndex = ui.comboBox_playerTargetSelect->itemData(playerTargetSelectIndex, Qt::UserRole).toInt();

				if (playerTargetSelectIndex >= 0)
					pPlayerTarget = new CBattleTarget_Teammate(playerTargetSelectIndex);
				else
				{
					QMessageBox::information(this, "错误:", "指定目标错误!", "确定");
					return;
				}
				break;
			}
			case dtTarget_Self:
			case dtTarget_Pet:
			case dtTarget_Condition:
			{
				pPlayerTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(playerTargetIndex);
				break;
			}
		}
	}

	CBattleAction *pPetAction = NULL;

	int petActionIndex = ui.comboBox_petAction->currentIndex();
	QString petActionStr = ui.comboBox_petAction->currentText();
	bool bHasPetTarget = ui.comboBox_petTarget->isEnabled();

	if (ui.comboBox_petAction->itemText(petActionIndex) != petActionStr)
		petActionIndex = -1;
	petActionIndex = ui.comboBox_petAction->itemData(petActionIndex, Qt::UserRole).toInt();

	switch (petActionIndex)
	{
		case dtAction_PetDoNothing:
		{
			pPetAction = new CBattleAction_PetDoNothing();
			break;
		}
		default:
		{
			if (!petActionStr.isEmpty())
			{
				pPetAction = new CBattleAction_PetSkillAttack(petActionStr);
			}
			else
			{
				QMessageBox::information(this, "错误:", "宠物技能为空!", "确定");
				return;
			}
			break;
		}
	}

	CBattleTarget *pPetTarget = NULL;
	if (pPetAction && bHasPetTarget)
	{
		int petTargetIndex = ui.comboBox_petTarget->currentIndex();
		petTargetIndex = ui.comboBox_petTarget->itemData(petTargetIndex, Qt::UserRole).toInt();

		switch (petTargetIndex)
		{
			case dtTarget_Enemy:
			{
				QString specialName;
				if (ui.checkBox_petSpecial->isChecked())
					specialName = ui.lineEdit_petSpecialName->text();
				int petTargetSelectIndex = ui.comboBox_petTargetSelect->currentIndex();
				petTargetSelectIndex = ui.comboBox_petTargetSelect->itemData(petTargetSelectIndex, Qt::UserRole).toInt();

				if (petTargetSelectIndex >= 0)
					pPetTarget = new CBattleTarget_Enemy(petTargetSelectIndex, specialName);
				else
				{
					QMessageBox::information(this, "错误:", "宠物指定目标错误!", "确定");
					return;
				}
				break;
			}
			case dtTarget_Teammate:
			{
				int petTargetSelectIndex = ui.comboBox_petTargetSelect->currentIndex();
				petTargetSelectIndex = ui.comboBox_petTargetSelect->itemData(petTargetSelectIndex, Qt::UserRole).toInt();

				if (petTargetSelectIndex >= 0)
					pPetTarget = new CBattleTarget_Teammate(petTargetSelectIndex);
				else
				{
					QMessageBox::information(this, "错误:", "宠物指定目标错误!", "确定");
					return;
				}
				break;
			}
			case dtTarget_Self:
			case dtTarget_Pet:
			case dtTarget_Condition:
			{
				pPetTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(petTargetIndex);
				break;
			}
		}
	}
	m_playerAction = pPlayerAction;
	m_playerTarget = pPlayerTarget;
	m_petAction = pPetAction;
	m_petTarget = pPetTarget;

	int ncount = ui.listWidget_copySet->count();
	for (int i = 0; i < ncount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget_copySet->item(i);
		if (pItem == m_pAllItem)
			continue;
		if (pItem->checkState() == Qt::Checked)
			m_nOtherEnemySet.append(pItem->data(Qt::UserRole).toInt());
	}
	accept();
}

void BattleSetDlg::on_pushButton_cancel_clicked()
{
	reject();
}

void BattleSetDlg::doItemClicked(QListWidgetItem *pItem)
{
	if (pItem == m_pAllItem)
	{
		SetAllListWidgetItemCheckState(pItem->checkState());
	}
}
