#include "GameScriptStopWgt.h"
#include "GameCtrl.h"
GameScriptStopWgt::GameScriptStopWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initListWidget();
}

GameScriptStopWgt::~GameScriptStopWgt()
{
}

void GameScriptStopWgt::initListWidget()
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
	connect(ui.listWidget, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doStopScriptItemChanged(QListWidgetItem *)));
}

void GameScriptStopWgt::doLoadUserConfig(QSettings &iniFile)
{
	//iniFile.beginGroup("ScriptUi");
	//ui.comboBox_leaderName->setCurrentText(iniFile.value("TeamLeaderName").toString());
	//ui.lineEdit_playerHpUI->setText(iniFile.value("PlayerHp").toString());
	//ui.lineEdit_playerMpUI->setText(iniFile.value("PlayerMp").toString());
	//ui.lineEdit_petHpUI->setText(iniFile.value("PetHp").toString());
	//ui.lineEdit_petMpUI->setText(iniFile.value("PetMp").toString());
	//ui.lineEdit_troopCountUI->setText(iniFile.value("TroopCount").toString());
	//ui.lineEdit_userCustomUI1->setText(iniFile.value("UserCustom1").toString());
	//ui.lineEdit_userCustomUI2->setText(iniFile.value("UserCustom2").toString());
	//ui.lineEdit_userCustomUI3->setText(iniFile.value("UserCustom3").toString());
	//iniFile.endGroup();
	//on_pushButton_fetchTeamData_clicked();
	//on_lineEdit_playerHpUI_editingFinished();
	//on_lineEdit_playerMpUI_editingFinished();
	//on_lineEdit_petMpUI_editingFinished();
	//on_lineEdit_petHpUI_editingFinished();
	//on_lineEdit_troopCountUI_editingFinished();
	//on_lineEdit_userCustomUI1_editingFinished();
	//on_lineEdit_userCustomUI2_editingFinished();
	//on_lineEdit_userCustomUI3_editingFinished();
}

void GameScriptStopWgt::doSaveUserConfig(QSettings &iniFile)
{
	/*iniFile.beginGroup("ScriptUi");
	iniFile.setValue("TeamLeaderName", ui.comboBox_leaderName->currentText());
	iniFile.setValue("PlayerHp", ui.lineEdit_playerHpUI->text());
	iniFile.setValue("PlayerMp", ui.lineEdit_playerMpUI->text());
	iniFile.setValue("PetHp", ui.lineEdit_petHpUI->text());
	iniFile.setValue("PetMp", ui.lineEdit_petMpUI->text());
	iniFile.setValue("TroopCount", ui.lineEdit_troopCountUI->text());
	iniFile.setValue("UserCustom1", ui.lineEdit_userCustomUI1->text());
	iniFile.setValue("UserCustom2", ui.lineEdit_userCustomUI2->text());
	iniFile.setValue("UserCustom3", ui.lineEdit_userCustomUI3->text());
	iniFile.endGroup();*/
}

void GameScriptStopWgt::doStopScriptItemChanged(QListWidgetItem *pItem)
{
	if (!pItem)
		return;
	int ntype = pItem->data(Qt::UserRole).toInt();
	auto pCfg = g_pGameFun->GetStopScriptCfg(ntype);
	if (pCfg)
	{
		pCfg->bChecked = (pItem->checkState() == Qt::Checked ? true : false);
		pCfg->bPercentage = false; //是否百分比判断
	}
}

void GameScriptStopWgt::on_checkBox_stopPlayerHp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPlayerHp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
	}
}

void GameScriptStopWgt::on_checkBox_stopPlayerMp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPlayerMp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
	}
}

void GameScriptStopWgt::on_checkBox_stopPetHp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPetHp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
	}
}

void GameScriptStopWgt::on_checkBox_stopPetMp_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPetMp);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
	}
}

void GameScriptStopWgt::on_checkBox_stopTroopCount_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterTroopCount);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
	}
}
void GameScriptStopWgt::on_lineEdit_troopCount_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterTroopCount);
	if (pCfg)
	{
		if (ui.lineEdit_troopCount->text().isEmpty())
			pCfg->bChecked = false;
		else
		{
			pCfg->dVal = ui.lineEdit_troopCount->text().toDouble();
			pCfg->bChecked = true;
		}
	}
}

void GameScriptStopWgt::on_checkBox_hpPercentage_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPlayerHp);
	if (pCfg)
		pCfg->bPercentage = (state == Qt::Checked ? true : false);
}

void GameScriptStopWgt::on_checkBox_mpPercentage_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPlayerMp);
	if (pCfg)
		pCfg->bPercentage = (state == Qt::Checked ? true : false);
}

void GameScriptStopWgt::on_checkBox_petHpPercentage_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPetHp);
	if (pCfg)
		pCfg->bPercentage = (state == Qt::Checked ? true : false);
}

void GameScriptStopWgt::on_checkBox_petMpPercentage_stateChanged(int state)
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPetMp);
	if (pCfg)
		pCfg->bPercentage = (state == Qt::Checked ? true : false);
}
void GameScriptStopWgt::on_lineEdit_playerHp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPlayerHp);
	if (pCfg)
	{
		if (ui.lineEdit_playerHp->text().isEmpty())
			pCfg->bChecked = false;
		else
		{
			pCfg->dVal = ui.lineEdit_playerHp->text().toDouble();
			pCfg->bChecked = true;
			pCfg->bPercentage = ui.checkBox_hpPercentage->isChecked();
		}
	}
}

void GameScriptStopWgt::on_lineEdit_playerMp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPlayerMp);
	if (pCfg)
	{
		if (ui.lineEdit_playerMp->text().isEmpty())
			pCfg->bChecked = false;
		else
		{
			pCfg->dVal = ui.lineEdit_playerMp->text().toDouble();
			pCfg->bChecked = true;
			pCfg->bPercentage = ui.checkBox_mpPercentage->isChecked();
		}
	}
}

void GameScriptStopWgt::on_lineEdit_petHp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPetHp);
	if (pCfg)
	{
		if (ui.lineEdit_petHp->text().isEmpty())
			pCfg->bChecked = false;
		else
		{
			pCfg->dVal = ui.lineEdit_petHp->text().toDouble();
			pCfg->bChecked = true;
			pCfg->bPercentage = ui.checkBox_petHpPercentage->isChecked();
		}
	}
}

void GameScriptStopWgt::on_lineEdit_petMp_editingFinished()
{
	auto pCfg = g_pGameFun->GetStopScriptCfg(TCondition_StopEncounterPetMp);
	if (pCfg)
	{

		if (ui.lineEdit_petMp->text().isEmpty())
			pCfg->bChecked = false;
		else
		{
			pCfg->dVal = ui.lineEdit_petMp->text().toDouble();
			pCfg->bChecked = true;
			pCfg->bPercentage = ui.checkBox_petMpPercentage->isChecked();
		}
	}
}