#include "GameBattleSetWgt.h"
#include "GameCtrl.h"
#include "GameData.h"
#include "stdafx.h"

GameBattleSetWgt::GameBattleSetWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl, SIGNAL(signal_activeGameFZ()), this, SLOT(Active()));
	connect(g_pGameCtrl, SIGNAL(signal_switchEncounterSpeedUI(int)), this, SLOT(updateEncounterSpeed(int)));
	connect(g_pGameCtrl, &GameCtrl::signal_switchAutoEncounterUI, this, [&](bool bChecked)
			{ 
				ui.checkBox_AutoEncounter->setChecked(bChecked); 
				if (bChecked)
				{
					g_pGameFun->begin_auto_action();
				}
				else
					g_pGameFun->end_auto_action();
		});
	connect(g_pGameFun, SIGNAL(signal_stopAutoEncounterEnemy()), this, SLOT(doStopAutoEncounterEnemy()));
	connect(g_pGameFun, SIGNAL(signal_startAutoEncounterEnemySucess()), this, SLOT(doSyncEncounterUi()));
	initListWidget();
	ui.comboBox_encounterInterval->clear();
	ui.comboBox_encounterInterval->addItem("速度", 20);
	int nVal = 50;
	for (int i = 0; i < 13; ++i)
	{
		nVal = (50) + i * 50;
		ui.comboBox_encounterInterval->addItem(QString::number(nVal), nVal);
	}
	ui.comboBox_EncounterType->clear();
	ui.comboBox_EncounterType->addItem("走动方式", TEncounter_Origin_Random);
	ui.comboBox_EncounterType->addItem("↖", TEncounter_Origin_North);
	ui.comboBox_EncounterType->addItem("↑", TEncounter_Origin_NorthEast);
	ui.comboBox_EncounterType->addItem("↗", TEncounter_Origin_East);
	ui.comboBox_EncounterType->addItem("→", TEncounter_Origin_SouthEast);
	ui.comboBox_EncounterType->addItem("↘", TEncounter_Origin_South);
	ui.comboBox_EncounterType->addItem("↓", TEncounter_Origin_SouthWest);
	ui.comboBox_EncounterType->addItem("↙", TEncounter_Origin_West);
	ui.comboBox_EncounterType->addItem("←", TEncounter_Origin_NorthWest);
	ui.comboBox_EncounterType->addItem("原地随机", TEncounter_Origin_Random);
}

GameBattleSetWgt::~GameBattleSetWgt()
{
}

void GameBattleSetWgt::initListWidget()
{
	ui.listWidget->clear();
	QMap<int, QString> stopList;
	stopList.insert(TCondition_PlayerInjuredWhite, "人物白伤");
	stopList.insert(TCondition_PlayerInjuredYellow, "人物黄伤");
	stopList.insert(TCondition_PlayerInjuredPurple, "人物紫伤");
	stopList.insert(TCondition_PlayerInjuredRed, "人物红伤");
	stopList.insert(TCondition_PetCountFull, "宠物满");
	stopList.insert(TCondition_ItemListFull, "物品满");
	stopList.insert(TCondition_DontHaveMagicFood, "无料理");
	stopList.insert(TCondition_DontHaveMedicament, "无血瓶");
	stopList.insert(TCondition_BattlePetLoyaltyTooLow, "出战宠物忠诚低于60");
	stopList.insert(TCondition_PlayeLossSoul, "掉魂");

	for (auto it = stopList.begin(); it != stopList.end(); ++it)
	{
		QListWidgetItem *pItem = new QListWidgetItem(it.value());
		pItem->setData(Qt::UserRole, it.key());
		pItem->setCheckState(Qt::Unchecked);
		pItem->setToolTip(pItem->text());
		ui.listWidget->addItem(pItem);
	}
	connect(ui.listWidget, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doStopEncountrItemChanged(QListWidgetItem *)));
}

void GameBattleSetWgt::doLoadUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("Encounter");
	ui.comboBox_encounterInterval->setCurrentIndex(ui.comboBox_encounterInterval->findText(iniFile.value("speed").toString()));
	ui.comboBox_EncounterType->setCurrentIndex(ui.comboBox_EncounterType->findData(iniFile.value("type").toInt()));
	iniFile.endGroup();

	iniFile.beginGroup("StopEncounter");
	ui.checkBox_troop->setChecked(iniFile.value("teamCountChecked").toBool());
	ui.lineEdit_troopCount->setText(iniFile.value("teamCountVal").toString());
	on_checkBox_troop_stateChanged(ui.checkBox_troop->checkState());
	on_lineEdit_troopCount_editingFinished();

	ui.checkBox_playerHp->setChecked(iniFile.value("playerHpChecked").toBool());
	ui.lineEdit_playerHp->setText(iniFile.value("playerHpVal").toString());
	on_checkBox_playerHp_stateChanged(ui.checkBox_playerHp->checkState());
	on_lineEdit_playerHp_editingFinished();

	ui.checkBox_playerMp->setChecked(iniFile.value("playerMpChecked").toBool());
	ui.lineEdit_playerMp->setText(iniFile.value("playerMpVal").toString());
	on_checkBox_playerMp_stateChanged(ui.checkBox_playerMp->checkState());
	on_lineEdit_playerMp_editingFinished();

	ui.checkBox_petHp->setChecked(iniFile.value("petHpChecked").toBool());
	ui.lineEdit_petHp->setText(iniFile.value("petHpVal").toString());
	on_checkBox_petHp_stateChanged(ui.checkBox_petHp->checkState());
	on_lineEdit_petHp_editingFinished();

	ui.checkBox_petMp->setChecked(iniFile.value("petMpChecked").toBool());
	ui.lineEdit_petMp->setText(iniFile.value("petMpVal").toString());
	on_checkBox_petMp_stateChanged(ui.checkBox_petMp->checkState());
	on_lineEdit_petMp_editingFinished();

	ui.checkBox_teammateHp->setChecked(iniFile.value("teamHpChecked").toBool());
	ui.lineEdit_teammateHp->setText(iniFile.value("teamHpVal").toString());
	on_checkBox_teammateHp_stateChanged(ui.checkBox_teammateHp->checkState());
	on_lineEdit_teammateHp_editingFinished();

	int nCount = iniFile.value("stopCount").toInt();
	for (int i = 0; i < nCount; ++i)
	{
		QString itemKeyName = QString("item%1").arg(i + 1);
		QString itemKeyVal = QString("checked%1").arg(i + 1);

		QString itemName = iniFile.value(itemKeyName, "").toString();
		bool itemVal = iniFile.value(itemKeyVal, "").toBool();

		auto pItemList = ui.listWidget->findItems(itemName, Qt::MatchExactly);
		if (pItemList.size() > 0)
		{
			pItemList.at(0)->setCheckState(itemVal ? Qt::Checked : Qt::Unchecked);
			doStopEncountrItemChanged(pItemList.at(0)); //同步后台
		}
	}
	iniFile.endGroup();
}

void GameBattleSetWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("Encounter");
	iniFile.setValue("speed", ui.comboBox_encounterInterval->currentText());
	iniFile.setValue("type", ui.comboBox_EncounterType->currentData());
	iniFile.endGroup();

	iniFile.beginGroup("StopEncounter");
	iniFile.setValue("teamCountChecked", ui.checkBox_troop->isChecked());
	iniFile.setValue("teamCountVal", ui.lineEdit_troopCount->text());

	iniFile.setValue("playerHpChecked", ui.checkBox_playerHp->isChecked());
	iniFile.setValue("playerHpVal", ui.lineEdit_playerHp->text());

	iniFile.setValue("playerMpChecked", ui.checkBox_playerMp->isChecked());
	iniFile.setValue("playerMpVal", ui.lineEdit_playerMp->text());

	iniFile.setValue("petHpChecked", ui.checkBox_petHp->isChecked());
	iniFile.setValue("petHpVal", ui.lineEdit_petHp->text());

	iniFile.setValue("petMpChecked", ui.checkBox_petMp->isChecked());
	iniFile.setValue("petMpVal", ui.lineEdit_petMp->text());

	iniFile.setValue("teamHpChecked", ui.checkBox_teammateHp->isChecked());
	iniFile.setValue("teamHpVal", ui.lineEdit_teammateHp->text());

	int nCount = ui.listWidget->count();
	iniFile.setValue("stopCount", nCount);

	for (int i = 0; i < nCount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget->item(i);
		QString itemKeyName = QString("item%1").arg(i + 1);
		QString itemKeyVal = QString("checked%1").arg(i + 1);
		iniFile.setValue(itemKeyName, pItem->text());
		iniFile.setValue(itemKeyVal, pItem->checkState() == Qt::Checked);
	}
	iniFile.endGroup();
}

void GameBattleSetWgt::on_checkBox_AutoEncounter_clicked(bool bChecked)
{
	if (bChecked)
	{
		int nInterval = ui.comboBox_encounterInterval->currentData(Qt::UserRole).toInt();
		int nDir = ui.comboBox_EncounterType->currentData(Qt::UserRole).toInt();
		g_pGameFun->setAutoEncounterDir(nDir);
		g_pGameFun->setAutoEncounterInterval(nInterval);
		g_pGameFun->begin_auto_action();
	}
	else
	{
		g_pGameFun->end_auto_action();
	}
}
//遇敌时间可改 位置只能关了重开
void GameBattleSetWgt::on_comboBox_encounterInterval_currentIndexChanged(int index)
{
	int nInterval = ui.comboBox_encounterInterval->currentData(Qt::UserRole).toInt();
	g_pGameFun->setAutoEncounterInterval(nInterval);
}

void GameBattleSetWgt::doSyncEncounterUi()
{
	if (!ui.checkBox_AutoEncounter->isChecked())
	{
		ui.checkBox_AutoEncounter->setChecked(true);
	}
}

void GameBattleSetWgt::doStopAutoEncounterEnemy()
{
	if (ui.checkBox_AutoEncounter->isChecked())
	{
		ui.checkBox_AutoEncounter->setChecked(false); //勾选去掉
	}
}

void GameBattleSetWgt::on_checkBox_IsShow_stateChanged(int state)
{
	g_pGameFun->setAutoEncounterShowMove(state == Qt::Checked ? true : false);
}

void GameBattleSetWgt::on_checkBox_transformation_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->getGameTransformationCfg();
	pCfg->bChecked = (state == Qt::Checked ? true : false);
}

void GameBattleSetWgt::on_checkBox_Cosplay_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->getGameCosplayCfg();
	pCfg->bChecked = (state == Qt::Checked ? true : false);
}

void GameBattleSetWgt::on_checkBox_AutoPetRiding_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->getGamePetRidingCfg();
	pCfg->bChecked = (state == Qt::Checked ? true : false);
	//pCfg->selectSubName = ui.lineEdit_transPetName->text();
}

void GameBattleSetWgt::on_lineEdit_transPetName_editingFinished()
{
	auto pCfg = g_pGameCtrl->getGameTransformationCfg();
	pCfg->selectSubName = ui.lineEdit_transPetName->text();
}

void GameBattleSetWgt::on_lineEdit_CosplayName_editingFinished()
{
	auto pCfg = g_pGameCtrl->getGameCosplayCfg();
	pCfg->selectName = ui.lineEdit_CosplayName->text();
}

void GameBattleSetWgt::updateEncounterSpeed(int speed)
{
	int index = ui.comboBox_encounterInterval->findText(QString::number(speed));
	if (index < 0)
	{
		ui.comboBox_encounterInterval->addItem(QString::number(speed), speed);
		index = ui.comboBox_encounterInterval->findText(QString::number(speed));
	}
	ui.comboBox_encounterInterval->setCurrentIndex(index);
}

void GameBattleSetWgt::on_checkBox_troop_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterTroopCount);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		pCfg->bPercentage = false;
		pCfg->dVal = ui.lineEdit_troopCount->text().toDouble();
	}
}

void GameBattleSetWgt::on_checkBox_playerHp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterPlayerHp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		pCfg->bPercentage = true;
		pCfg->dVal = ui.lineEdit_playerHp->text().toDouble();
	}
}

void GameBattleSetWgt::on_checkBox_playerMp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterPlayerMp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		pCfg->bPercentage = true;
		pCfg->dVal = ui.lineEdit_playerMp->text().toDouble();
	}
}

void GameBattleSetWgt::on_checkBox_petHp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterPetHp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		pCfg->bPercentage = true;
		pCfg->dVal = ui.lineEdit_petHp->text().toDouble();
	}
}

void GameBattleSetWgt::on_checkBox_petMp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterPetMp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		pCfg->bPercentage = true;
		pCfg->dVal = ui.lineEdit_petMp->text().toDouble();
	}
}

void GameBattleSetWgt::on_checkBox_teammateHp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterTeammateHp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		pCfg->bPercentage = true;
		pCfg->dVal = ui.lineEdit_teammateHp->text().toDouble();
	}
}

void GameBattleSetWgt::on_lineEdit_troopCount_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterTroopCount);
	if (pCfg)
	{
		pCfg->dVal = ui.lineEdit_troopCount->text().toDouble();
	}
}

void GameBattleSetWgt::on_lineEdit_playerHp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterPlayerHp);
	if (pCfg)
	{
		pCfg->dVal = ui.lineEdit_playerHp->text().toDouble();
	}
}

void GameBattleSetWgt::on_lineEdit_playerMp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterPlayerMp);
	if (pCfg)
	{
		pCfg->dVal = ui.lineEdit_playerMp->text().toDouble();
	}
}

void GameBattleSetWgt::on_lineEdit_petHp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterPetHp);
	if (pCfg)
	{
		pCfg->dVal = ui.lineEdit_petHp->text().toDouble();
	}
}

void GameBattleSetWgt::on_lineEdit_petMp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterPetMp);
	if (pCfg)
	{
		pCfg->dVal = ui.lineEdit_petMp->text().toDouble();
	}
}

void GameBattleSetWgt::on_lineEdit_teammateHp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopEncounter(TCondition_StopEncounterTeammateHp);
	if (pCfg)
	{
		pCfg->dVal = ui.lineEdit_teammateHp->text().toDouble();
	}
}

void GameBattleSetWgt::doStopEncountrItemChanged(QListWidgetItem *pItem)
{
	if (!pItem)
		return;
	int ntype = pItem->data(Qt::UserRole).toInt();
	auto pCfg = g_pGameFun->GetStopEncounter(ntype);
	if (pCfg)
	{
		pCfg->bChecked = (pItem->checkState() == Qt::Checked ? true : false);
		pCfg->bPercentage = false; //是否百分比判断
	}
}
