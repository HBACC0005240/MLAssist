#include "GameBattleWgt.h"
#include "BattleSetDlg.h"
#include "GameCtrl.h"
#include "ITTabBarStyle.h"
#include "PetDoubleActionSetDlg.h"
#include "UserDefComboBoxDlg.h"
#include "stdafx.h"
#include <QDebug>
#include <QFileDialog>

GameBattleWgt::GameBattleWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	m_pSoundAlarm = new QSound(QApplication::applicationDirPath() + "//ALARM.wav");
	ui.tabWidget->setStyle(new ITTabBarStyle);

	//Setting主要是用来复用对话框，宠物部分有用，其他设置无实际意义
	CBattleCondition_EnemyType *pCondition = (CBattleCondition_EnemyType *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyType);
	CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
	CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
	//下面两个才有用
	m_pEscapePetAction = new CBattleAction_PetDoNothing();
	m_pEscapePetTarget = new CBattleTarget_Enemy(dtTargetCondition_Random);
	m_pEscapeSetting = CBattleSettingPtr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, m_pEscapePetAction, m_pEscapePetTarget));
	init();

	connect(g_pGameCtrl, &GameCtrl::signal_switchAutoBattleUI, this, [&](bool bFlag)
			{ ui.checkBox_auto->setChecked(bFlag); });
	connect(g_pGameCtrl, &GameCtrl::signal_switchNoPetDoubleActionUI, this, [&](bool bFlag)
			{ ui.groupBox_noPet->setChecked(bFlag); });
	connect(g_pGameCtrl, &GameCtrl::signal_switchNoPetActionUI, this, [&](int ntype, bool bFlag)
			{
				if (ntype == 0)
					ui.radioButton_defense->setChecked(bFlag);
				else if (ntype == 1)
					ui.radioButton_attack->setChecked(bFlag);
			});
	connect(g_pGameCtrl, &GameCtrl::signal_switchAutoHightSpeedBattleUI, this, [&](bool bFlag)
			{ ui.checkBox_HighSpeed->setChecked(bFlag); });
	connect(g_pGameCtrl, &GameCtrl::signal_setHightSpeedBattleDelayUI, this, [&](int delayTime)
			{ ui.comboBox_highSpeedDelay->setCurrentIndex(ui.comboBox_highSpeedDelay->findData(delayTime)); });
	connect(g_pGameCtrl, &GameCtrl::signal_switchAllEncounterEscapeUI, this, [&](bool bFlag)
			{ ui.checkBox_allEscape->setChecked(bFlag); });
	connect(g_pGameCtrl, &GameCtrl::signal_switchNoLvlEncounterEscapeUI, this, [&](bool bFlag)
			{ ui.checkBox_NoLv1Escape->setChecked(bFlag); });
	connect(g_pGameCtrl, &GameCtrl::signal_setMoveSpeedUI, this, [&](int speed)
			{ ui.horizontalSlider_moveSpeed->setValue(speed); });

	connect(ui.checkBox_auto, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetAutoBattle(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_HighSpeed, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetHighSpeed(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_firstNoDelay, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetFRND(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_bossProtect, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetBOSSProtect(int)), Qt::ConnectionType::QueuedConnection);

	connect(ui.checkBox_LockCountdown, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetLockCountdown(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_ShowHPMP, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetShowHPMPEnabled(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_PetDoubleAction, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetPetDoubleAction(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_PlayerForceAction, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetPlayerForceAction(int)), Qt::ConnectionType::QueuedConnection);

	//切图
	connect(ui.checkBox_noAnimation, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetNoSwitchAnim(int)), Qt::ConnectionType::QueuedConnection);

	connect(ui.checkBox_Lv1Protect, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetLv1Protect(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_allEscape, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetAllEscape(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_NoLv1Escape, SIGNAL(stateChanged(int)), g_pAutoBattleCtrl, SLOT(OnSetNoLv1Escape(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.listWidget_SpecialEnemy, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doSpecialEnemyItemChanged(QListWidgetItem *)));
	connect(g_pAutoBattleCtrl, SIGNAL(PlayAlarmWav()), this, SLOT(doPlayAlarmWav()));
	connect(g_pAutoBattleCtrl, SIGNAL(StopAlarmWav()), this, SLOT(doStopPalyAlarm()));
}

GameBattleWgt::~GameBattleWgt()
{
}

void GameBattleWgt::init()
{
	//延时
	ui.comboBox_highSpeedDelay->clear();
	ui.comboBox_highSpeedDelay->addItem("高速延时", 4);
	for (size_t i = 0; i <= 8; i++)
	{
		ui.comboBox_highSpeedDelay->addItem(QString::number(i), i);
	}
	for (int i = 1; i <= 10; i++)
	{
		QString szObjName = QString("pushButton_%1Enemy").arg(i);
		QPushButton *pObj = this->findChild<QPushButton *>(szObjName);
		if (pObj)
		{
			connect(pObj, SIGNAL(clicked()), this, SLOT(doSetBattleTactics()));
		}
	}
	for (int i = 1; i <= 10; i++)
	{
		QString szObjName = QString("pushButton_%1Round").arg(i);
		QPushButton *pObj = this->findChild<QPushButton *>(szObjName);
		if (pObj)
		{
			connect(pObj, SIGNAL(clicked()), this, SLOT(doSetBattleRound()));
		}
	}
	m_pBaohuCheckBoxList.append(ui.checkBox_selfHp1);
	m_pBaohuCheckBoxList.append(ui.checkBox_selfHp2);
	m_pBaohuCheckBoxList.append(ui.checkBox_selfMp1);
	m_pBaohuCheckBoxList.append(ui.checkBox_selfMp2);
	m_pBaohuCheckBoxList.append(ui.checkBox_teammateHp1);
	m_pBaohuCheckBoxList.append(ui.checkBox_petHp);
	m_pBaohuCheckBoxList.append(ui.checkBox_petMp);
	m_pBaohuCheckBoxList.append(ui.checkBox_troopHp);

	m_pBaohuLineEditList.append(ui.lineEdit_PlayerHp1);
	m_pBaohuLineEditList.append(ui.lineEdit_PlayerHp2);
	m_pBaohuLineEditList.append(ui.lineEdit_PlayerMp1);
	m_pBaohuLineEditList.append(ui.lineEdit_PlayerMp2);
	m_pBaohuLineEditList.append(ui.lineEdit_TeammateHp);
	m_pBaohuLineEditList.append(ui.lineEdit_PetHp);
	m_pBaohuLineEditList.append(ui.lineEdit_PetMp);
	m_pBaohuLineEditList.append(ui.lineEdit_TroopHp);

	m_pLv1CheckBoxList.append(ui.checkBox_Lv1FilterHp);
	m_pLv1CheckBoxList.append(ui.checkBox_Lv1FilterMp);
	m_pLv1CheckBoxList.append(ui.checkBox_Lv1Recall);
	m_pLv1CheckBoxList.append(ui.checkBox_Lv1RoundOne);
	m_pLv1CheckBoxList.append(ui.checkBox_Lv1Hp);
	m_pLv1CheckBoxList.append(ui.checkBox_Lv1CleanNo1);
	m_pLv1CheckBoxList.append(ui.checkBox_Lv1LastSet);

	m_pLv1LineEditList.append(ui.lineEdit_Lv1FilterHp);
	m_pLv1LineEditList.append(ui.lineEdit_Lv1FilterMp);
	m_pLv1LineEditList.append(nullptr);
	m_pLv1LineEditList.append(nullptr);
	m_pLv1LineEditList.append(ui.lineEdit_Lv1HpVal);
	m_pLv1LineEditList.append(nullptr);
	m_pLv1LineEditList.append(nullptr);
}

void GameBattleWgt::doLoadJsConfig(QJsonObject &obj)
{
	if (obj.contains("player"))
	{
		QJsonObject playerobj = obj.value("player").toObject();
		if (playerobj.contains("noswitchanim"))
			ui.checkBox_noAnimation->setChecked(playerobj.take("noswitchanim").toBool());

		if (playerobj.contains("gametextui"))
			ui.checkBox_ShowHPMP->setChecked(playerobj.take("gametextui").toBool());

		if (playerobj.contains("antiafkkick"))
			ui.checkBox_OverTimeT->setChecked(playerobj.take("antiafkkick").toBool());

		if (playerobj.contains("movespd"))
			ui.horizontalSlider_moveSpeed->setValue(playerobj.take("movespd").toInt());		
	}
	ui.customBattleWgt->ParseBattleSettings(obj.take("battle"));
}

void GameBattleWgt::doSaveJsConfig(QJsonObject &obj)
{
	QJsonObject player;
	player.insert("noswitchanim", ui.checkBox_noAnimation->isChecked());
	//player.insert("autosupply", ui->checkBox_autoSupply->isChecked());

	player.insert("gametextui", ui.checkBox_ShowHPMP->isChecked());
	player.insert("antiafkkick", ui.checkBox_OverTimeT->isChecked());

	player.insert("movespd", ui.horizontalSlider_moveSpeed->value());
	player.insert("workacc", 100);	  //ui->horizontalSlider_workacc->value());
	player.insert("workdelay", 6500); //ui->horizontalSlider_workdelay->value());
	obj.insert("player", player);

	QJsonObject battle;
	battle.insert("highspeed", ui.checkBox_HighSpeed->isChecked());
	battle.insert("autobattle", ui.checkBox_auto->isChecked());
	battle.insert("lockcd", ui.checkBox_LockCountdown->isChecked());
	battle.insert("lv1prot", ui.checkBox_Lv1Protect->isChecked());
	battle.insert("bossprot", ui.checkBox_bossProtect->isChecked());
	battle.insert("r1nodelay", ui.checkBox_firstNoDelay->isChecked());
	battle.insert("pet2action", ui.checkBox_PetDoubleAction->isChecked());
	battle.insert("beep", true);
	int delayVal = ui.comboBox_highSpeedDelay->currentData(Qt::UserRole).toInt();
	delayVal *= 1000;
	battle.insert("delayfrom", delayVal);
	battle.insert("delayto", delayVal);

	QJsonArray list;
	ui.customBattleWgt->SaveBattleSettings(battle);
	obj.insert("battle", battle);
}

//战术设置1-10个敌人
void GameBattleWgt::doSetBattleTactics()
{
	QObject *pObj = sender();
	QString szObjName = pObj->objectName();
	if (szObjName.endsWith("Enemy") == false)
		return;
	//取出针对的敌人数
	QString szEnemyNum = szObjName.remove("Enemy");
	szEnemyNum = szEnemyNum.remove("pushButton_");
	int enemyNum = szEnemyNum.toInt();
	//qDebug() << "敌人数-" << num;

	CBattleSettingPtr pExistSetting;
	CBattleSettingList pSettingList = g_pAutoBattleCtrl->GetSpecNumEnemySettings();
	for (size_t i = 0; i < pSettingList.size(); i++)
	{
		CBattleCondition *pCondition = pSettingList[i]->m_condition;
		if (pCondition->GetDevType() == dtCondition_EnemyCount) //敌人数
		{
			if (((CBattleCondition_EnemyCount *)pCondition)->GetConditionVal() == enemyNum)
			{
				pExistSetting = CBattleSettingPtr(pSettingList[i]);
				break;
			}
		}
	}

	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();

		//敌人数量 等于 指定数量
		int relation = dtCompare_Equal;
		QList<int> enemyNumCopySet = dlg.GetCopyToOtherSet();
		if (enemyNumCopySet.isEmpty())
		{
			CBattleCondition *pCondition = new CBattleCondition_EnemyCount(relation, enemyNum);
			CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
			CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
			g_pAutoBattleCtrl->AddSpecifNumEnemySetting(ptr, enemyNum);
		}
		else
		{
			if (enemyNumCopySet.contains(enemyNum) == false)
				enemyNumCopySet.append(enemyNum);
			for (size_t i = 0; i < enemyNumCopySet.size(); i++)
			{
				CBattleCondition *pCondition = new CBattleCondition_EnemyCount(relation, enemyNumCopySet[i]);
				CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
				CBattleAction *pCustomPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(pPlayerAction->GetDevType());
				CBattleAction *pCustomPetAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(pPetAction->GetDevType());
				CBattleTarget *pCustomPlayerTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(pPlayerTarget->GetDevType());
				CBattleTarget *pCustomPetTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(pPetTarget->GetDevType());
				memcpy(pCustomPlayerAction, pPlayerAction, g_battleModuleReg.GetClassSize(pPlayerAction->GetDevType()));
				memcpy(pCustomPetAction, pPetAction, g_battleModuleReg.GetClassSize(pPetAction->GetDevType()));
				memcpy(pCustomPlayerTarget, pPlayerTarget, g_battleModuleReg.GetClassSize(pPlayerTarget->GetDevType()));
				memcpy(pCustomPetTarget, pPetTarget, g_battleModuleReg.GetClassSize(pPetTarget->GetDevType()));
				CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pCustomPlayerAction, pCustomPlayerTarget, pCustomPetAction, pCustomPetTarget));
				g_pAutoBattleCtrl->AddSpecifNumEnemySetting(ptr, enemyNumCopySet[i]);
			}
		}
		ResetNumEnemyBtnColor();
	}
}

void GameBattleWgt::doSetBattleRound()
{
	QObject *pObj = sender();
	QString szObjName = pObj->objectName();
	if (szObjName.endsWith("Round") == false)
		return;
	//取出针对的回合数
	QString szEnemyNum = szObjName.remove("Round");
	szEnemyNum = szEnemyNum.remove("pushButton_");
	int enemyNum = szEnemyNum.toInt();
	//qDebug() << "回合数-" << num;

	CBattleSettingPtr pExistSetting;
	CBattleSettingList pSettingList = g_pAutoBattleCtrl->GetSpecNumRoundSettings();
	for (size_t i = 0; i < pSettingList.size(); i++)
	{
		CBattleCondition *pCondition = pSettingList[i]->m_condition;
		if (pCondition->GetDevType() == dtCondition_Round) //回合数
		{
			if (((CBattleCondition_Round *)pCondition)->GetConditionVal() == enemyNum)
			{
				pExistSetting = CBattleSettingPtr(pSettingList[i]);
				break;
			}
		}
	}

	BattleSetDlg dlg(this);
	dlg.SetCopyToMultiBattleGroupName("复用到多回合");
	dlg.SetCopyToMultiBattleListName("回合");
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();

		//回合数 等于 指定数量
		int relation = dtCompare_Equal;
		QList<int> enemyNumCopySet = dlg.GetCopyToOtherSet();
		if (enemyNumCopySet.isEmpty())
		{
			CBattleCondition *pCondition = new CBattleCondition_Round(relation, enemyNum);
			CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
			CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
			g_pAutoBattleCtrl->AddSpecifNumRoundSetting(ptr, enemyNum);
		}
		else
		{
			if (enemyNumCopySet.contains(enemyNum) == false)
				enemyNumCopySet.append(enemyNum);
			for (size_t i = 0; i < enemyNumCopySet.size(); i++)
			{
				CBattleCondition *pCondition = new CBattleCondition_Round(relation, enemyNumCopySet[i]);
				CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
				CBattleAction *pCustomPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(pPlayerAction->GetDevType());
				CBattleAction *pCustomPetAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(pPetAction->GetDevType());
				CBattleTarget *pCustomPlayerTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(pPlayerTarget->GetDevType());
				CBattleTarget *pCustomPetTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(pPetTarget->GetDevType());
				memcpy(pCustomPlayerAction, pPlayerAction, g_battleModuleReg.GetClassSize(pPlayerAction->GetDevType()));
				memcpy(pCustomPetAction, pPetAction, g_battleModuleReg.GetClassSize(pPetAction->GetDevType()));
				memcpy(pCustomPlayerTarget, pPlayerTarget, g_battleModuleReg.GetClassSize(pPlayerTarget->GetDevType()));
				memcpy(pCustomPetTarget, pPetTarget, g_battleModuleReg.GetClassSize(pPetTarget->GetDevType()));
				CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pCustomPlayerAction, pCustomPlayerTarget, pCustomPetAction, pCustomPetTarget));
				g_pAutoBattleCtrl->AddSpecifNumRoundSetting(ptr, enemyNumCopySet[i]);
			}
		}
		ResetNumRoundBtnColor();
	}
}
//清除指定数量敌人配置
void GameBattleWgt::on_pushButton_clearNum_clicked()
{
	UserDefComboBoxDlg dlg;
	dlg.setLabelText("指定数量:");
	dlg.setWindowTitle("清除指定数量配置");
	QStringList sItems;
	sItems.append("所有");
	for (size_t i = 1; i <= 10; i++)
	{
		sItems << QString::number(i);
	}

	dlg.setComboBoxItems(sItems);
	if (dlg.exec() == QDialog::Accepted)
	{
		QString sVal = dlg.getVal();
		if (sVal == "所有")
			g_pAutoBattleCtrl->ClearSpecifNumEnemySetting();
		else
			g_pAutoBattleCtrl->RemoveSpecifNumEnemySetting(sVal.toInt());
		ResetNumEnemyBtnColor();
	}
}
//清除指定回合配置
void GameBattleWgt::on_pushButton_clearRound_clicked()
{
	UserDefComboBoxDlg dlg;
	dlg.setLabelText("指定回合:");
	dlg.setWindowTitle("清除指定回合配置");
	QStringList sItems;
	sItems.append("所有");
	for (size_t i = 1; i <= 10; i++)
	{
		sItems << QString::number(i);
	}

	dlg.setComboBoxItems(sItems);
	if (dlg.exec() == QDialog::Accepted)
	{
		QString sVal = dlg.getVal();
		if (sVal == "所有")
			g_pAutoBattleCtrl->ClearSpecifNumRoundSetting();
		else
			g_pAutoBattleCtrl->RemoveSpecifNumRoundSetting(sVal.toInt());
		ResetNumRoundBtnColor();
	}
}

void GameBattleWgt::on_comboBox_highSpeedDelay_currentIndexChanged(int index)
{
	int nDelayTime = ui.comboBox_highSpeedDelay->currentData().toInt();
	g_pAutoBattleCtrl->OnSetHightSpeedDelayVal(nDelayTime);
}

void GameBattleWgt::on_pushButton_TroopHp_clicked()
{
	int nVal = ui.lineEdit_TroopHp->text().toInt();
	PopBattleSetDlg(TProtectSet_TroopHp, nVal, dtCondition_TeammateAllHp);
}

void GameBattleWgt::CreateSetting(int tType, int nVal, int nObjType, CBattleAction *pPlayerAction, CBattleAction *pPetAction, CBattleTarget *pPlayerTarget, CBattleTarget *pPetTarget)
{
	//敌人数量 等于 指定数量
	int relation = dtCompare_LessThan;
	CBattleCondition_Compare *pCondition = (CBattleCondition_Compare *)g_battleModuleReg.CreateNewBattleObj(nObjType);
	pCondition->setConditionPercentage(true);	 //百分比
	pCondition->setConditionValue(nVal);		 //保护值
	pCondition->setRelation(dtCompare_LessThan); //小于
	CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
	CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
	g_pAutoBattleCtrl->AddInternalProtectSetting(tType, ptr);
}
void GameBattleWgt::PopBattleSetDlg(int nType, int nVal, int nDevType)
{
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->GetInternalProtectSetting(nType);
	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();
		CreateSetting(nType, nVal, nDevType, pPlayerAction, pPetAction, pPlayerTarget, pPetTarget);
	}
}

void GameBattleWgt::on_pushButton_PetMp_clicked()
{
	int nVal = ui.lineEdit_PetMp->text().toInt();
	PopBattleSetDlg(TProtectSet_PetMp, nVal, dtCondition_PetMp);
}

void GameBattleWgt::on_pushButton_PetHp_clicked()
{
	int nVal = ui.lineEdit_PetHp->text().toInt();
	PopBattleSetDlg(TProtectSet_PetHp, nVal, dtCondition_PetHp);
}

void GameBattleWgt::on_pushButton_TeammateHp_clicked()
{
	int nVal = ui.lineEdit_TeammateHp->text().toInt();
	PopBattleSetDlg(TProtectSet_TeammateHp, nVal, dtCondition_TeammateHp);
}

void GameBattleWgt::on_pushButton_PlayerMp2_clicked()
{
	int nVal = ui.lineEdit_PlayerMp2->text().toInt();
	PopBattleSetDlg(TProtectSet_PlayerMp2, nVal, dtCondition_PlayerMp);
}

void GameBattleWgt::on_pushButton_PlayerMp1_clicked()
{
	int nVal = ui.lineEdit_PlayerMp1->text().toInt();
	PopBattleSetDlg(TProtectSet_PlayerMp1, nVal, dtCondition_PlayerMp);
}

void GameBattleWgt::on_pushButton_PlayerHp2_clicked()
{
	int nVal = ui.lineEdit_PlayerHp2->text().toInt();
	PopBattleSetDlg(TProtectSet_PlayerHp2, nVal, dtCondition_PlayerHp);
}

void GameBattleWgt::on_pushButton_PlayerHp1_clicked()
{
	int nVal = ui.lineEdit_PlayerHp1->text().toInt();
	PopBattleSetDlg(TProtectSet_PlayerHp1, nVal, dtCondition_PlayerHp);
}

void GameBattleWgt::on_checkBox_selfHp1_stateChanged(int state)
{
	g_pAutoBattleCtrl->SetInternalProtectSettingEnabled(TProtectSet_PlayerHp1, state == Qt::Checked);
}

void GameBattleWgt::on_checkBox_selfHp2_stateChanged(int state)
{
	g_pAutoBattleCtrl->SetInternalProtectSettingEnabled(TProtectSet_PlayerHp2, state == Qt::Checked);
}

void GameBattleWgt::on_checkBox_selfMp1_stateChanged(int state)
{
	g_pAutoBattleCtrl->SetInternalProtectSettingEnabled(TProtectSet_PlayerMp1, state == Qt::Checked);
}

void GameBattleWgt::on_checkBox_selfMp2_stateChanged(int state)
{
	g_pAutoBattleCtrl->SetInternalProtectSettingEnabled(TProtectSet_PlayerMp2, state == Qt::Checked);
}

void GameBattleWgt::on_checkBox_teammateHp1_stateChanged(int state)
{
	g_pAutoBattleCtrl->SetInternalProtectSettingEnabled(TProtectSet_TeammateHp, state == Qt::Checked);
}

void GameBattleWgt::on_checkBox_petHp_stateChanged(int state)
{
	g_pAutoBattleCtrl->SetInternalProtectSettingEnabled(TProtectSet_PetHp, state == Qt::Checked);
}

void GameBattleWgt::on_checkBox_petMp_stateChanged(int state)
{
	g_pAutoBattleCtrl->SetInternalProtectSettingEnabled(TProtectSet_PetMp, state == Qt::Checked);
}

void GameBattleWgt::on_checkBox_troopHp_stateChanged(int state)
{
	g_pAutoBattleCtrl->SetInternalProtectSettingEnabled(TProtectSet_TroopHp, state == Qt::Checked);
}

void GameBattleWgt::on_lineEdit_PlayerHp1_editingFinished()
{
	double dVal = ui.lineEdit_PlayerHp1->text().toDouble();
	UpdateBattleSetting(TProtectSet_PlayerHp1, dVal);
}

void GameBattleWgt::on_lineEdit_PlayerHp2_editingFinished()
{
	double dVal = ui.lineEdit_PlayerHp2->text().toDouble();
	UpdateBattleSetting(TProtectSet_PlayerHp2, dVal);
}

void GameBattleWgt::on_lineEdit_PlayerMp1_editingFinished()
{
	double dVal = ui.lineEdit_PlayerMp1->text().toDouble();
	UpdateBattleSetting(TProtectSet_PlayerMp1, dVal);
}

void GameBattleWgt::on_lineEdit_PlayerMp2_editingFinished()
{
	double dVal = ui.lineEdit_PlayerMp2->text().toDouble();
	UpdateBattleSetting(TProtectSet_PlayerMp2, dVal);
}

void GameBattleWgt::on_lineEdit_PetHp_editingFinished()
{
	double dVal = ui.lineEdit_PetHp->text().toDouble();
	UpdateBattleSetting(TProtectSet_PetHp, dVal);
}

void GameBattleWgt::on_lineEdit_PetMp_editingFinished()
{
	double dVal = ui.lineEdit_PetMp->text().toDouble();
	UpdateBattleSetting(TProtectSet_PetMp, dVal);
}

void GameBattleWgt::on_lineEdit_TeammateHp_editingFinished()
{
	double dVal = ui.lineEdit_TeammateHp->text().toDouble();
	UpdateBattleSetting(TProtectSet_TeammateHp, dVal);
}

void GameBattleWgt::on_lineEdit_TroopHp_editingFinished()
{
	double dVal = ui.lineEdit_TroopHp->text().toDouble();
	UpdateBattleSetting(TProtectSet_TroopHp, dVal);
}

///全跑
void GameBattleWgt::on_checkBox_allEscape_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		CBattleCondition_EnemyCount *pCondition = (CBattleCondition_EnemyCount *)g_battleModuleReg.CreateNewBattleObj(dtCondition_TeammateCount);
		pCondition->setConditionPercentage(false);
		pCondition->setConditionValue(1);				 //保护值 1
		pCondition->setRelation(dtCompare_GreaterEqual); //敌人数大于等于1
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		CBattleAction *pPetAction = nullptr;
		CBattleTarget *pPetTarget = nullptr;
		CreatePetActionTarget(pPetAction, pPetTarget);
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->AddEscapeSetting(TEscapeSet_All, ptr);
	}
	else if (state == Qt::Unchecked)
	{
		g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_All);
	}
}

//逃跑宠物设置
void GameBattleWgt::CreatePetActionTarget(CBattleAction *&pPetAction, CBattleTarget *&pPetTarget)
{

	CBattleAction *pCustomPetAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(m_pEscapePetAction->GetDevType());
	CBattleTarget *pCustomPetTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(m_pEscapePetTarget->GetDevType());
	memcpy(pCustomPetAction, m_pEscapePetAction, g_battleModuleReg.GetClassSize(m_pEscapePetAction->GetDevType()));
	memcpy(pCustomPetTarget, m_pEscapePetTarget, g_battleModuleReg.GetClassSize(m_pEscapePetTarget->GetDevType()));
	pPetAction = pCustomPetAction;
	pPetTarget = pCustomPetTarget; //这里没检测当前宠物技能，在战斗部分会检测

	//QString szPetSkillName;
	//if (m_nEscapePetAction == EscapePetAction_Guard)
	//{
	//	szPetSkillName = "防御";
	//}
	//else if (m_nEscapePetAction == EscapePetAction_Attack)
	//{
	//	szPetSkillName = "攻击";
	//}
	//bool bExistSkill = false;
	//GamePetList pPetList = g_pGameCtrl->getGamePets();
	//for (int i = 0; i < pPetList.size(); ++i)
	//{
	//	if (pPetList[i]->exist == false)
	//		continue;
	//	if (pPetList.at(i)->battle_flags & 2) //默认出战的宠物
	//	{
	//		GameSkillList pPetSkillList = pPetList.at(i)->skills;
	//		for (int j = 0; j < pPetSkillList.size(); ++j)
	//		{
	//			if (pPetSkillList.at(j)->name == szPetSkillName)
	//			{
	//				bExistSkill = true;
	//				break;
	//			}
	//		}
	//	}
	//	if (bExistSkill)
	//	{
	//		break;
	//	}
	//}
	//if (bExistSkill)
	//{
	//	pPetAction = new CBattleAction_PetSkillAttack(szPetSkillName);
	//	if (m_nEscapePetAction == EscapePetAction_Attack)
	//		pPetTarget = new CBattleTarget_Enemy(dtTargetCondition_Random);
	//}
	//else
	//{
	//	pPetAction = new CBattleAction_PetDoNothing();
	//}
}

void GameBattleWgt::UpdateBattleSetting(int tType, double dVal)
{
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->GetInternalProtectSetting(tType);
	if (pExistSetting)
	{
		CBattleCondition_Compare *pCondition = (CBattleCondition_Compare *)pExistSetting->m_condition;
		pCondition->setConditionValue(dVal);
	}
}

void GameBattleWgt::CreateNoPetDoubleAction()
{
	CBattleAction *pPlayerAction = nullptr;
	bool bAttack = ui.radioButton_attack->isChecked();
	if (ui.radioButton_attack->isChecked())
		pPlayerAction = new CBattleAction_PlayerAttack();
	else if (ui.radioButton_defense->isChecked())
		pPlayerAction = new CBattleAction_PlayerGuard();
	else if (ui.radioButton_escape->isChecked())
		pPlayerAction = new CBattleAction_PlayerEscape();
	CBattleAction *pPetAction = new CBattleAction_PetDoNothing();
	CBattleTarget *pPlayerTarget = new CBattleTarget_Enemy(dtTargetCondition_Random, "");
	CBattleTarget *pPetTarget = new CBattleTarget_Enemy(dtTargetCondition_Random, "");

	CBattleCondition_DoubleAction *pCondition = (CBattleCondition_DoubleAction *)g_battleModuleReg.CreateNewBattleObj(dtCondition_DoubleAction);
	CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
	CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
	g_pAutoBattleCtrl->SetNoPetDoubleAction(ptr);
}

void GameBattleWgt::ResetNumEnemyBtnColor()
{
	for (int i = 1; i <= 10; ++i)
	{
		int num = i;
		QString tmpObj = QString("pushButton_%1Enemy").arg(num);
		QPushButton *pBtn = this->findChild<QPushButton *>(tmpObj);
		if (pBtn)
		{
			pBtn->setStyleSheet("QPushButton{color: rgb(0, 0, 0);}");
		}
	}
	auto pSettings = g_pAutoBattleCtrl->GetSpecNumEnemySettings();
	for (auto pSetting : pSettings)
	{
		if (pSetting->m_condition)
		{
			int num = ((CBattleCondition_EnemyCount *)pSetting->m_condition)->GetConditionVal();
			QString tmpObj = QString("pushButton_%1Enemy").arg(num);
			QPushButton *pBtn = this->findChild<QPushButton *>(tmpObj);
			if (pBtn)
			{
				pBtn->setStyleSheet("QPushButton{color: rgb(200, 0, 0);}");
			}
		}
	}
}

void GameBattleWgt::ResetNumRoundBtnColor()
{
	for (int i = 1; i <= 10; ++i)
	{
		int num = i;
		QString tmpObj = QString("pushButton_%1Round").arg(num);
		QPushButton *pBtn = this->findChild<QPushButton *>(tmpObj);
		if (pBtn)
		{
			pBtn->setStyleSheet("QPushButton{color: rgb(0, 0, 0);}");
		}
	}
	auto pSettings = g_pAutoBattleCtrl->GetSpecNumRoundSettings();
	for (auto pSetting : pSettings)
	{
		if (pSetting->m_condition)
		{
			int num = ((CBattleCondition_Round *)pSetting->m_condition)->GetConditionVal();
			QString tmpObj = QString("pushButton_%1Round").arg(num);
			QPushButton *pBtn = this->findChild<QPushButton *>(tmpObj);
			if (pBtn)
			{
				pBtn->setStyleSheet("QPushButton{color: rgb(200, 0, 0);}");
			}
		}
	}
}

//无1级怪逃跑设置
void GameBattleWgt::on_checkBox_NoLv1Escape_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		CBattleCondition_EnemyLevel *pCondition = (CBattleCondition_EnemyLevel *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyLevel);
		pCondition->setConditionPercentage(false);
		pCondition->setConditionValue(1);
		pCondition->setRelation(dtCompare_NotContain); //敌人等级!=1
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		CBattleAction *pPetAction = nullptr;
		CBattleTarget *pPetTarget = nullptr;
		CreatePetActionTarget(pPetAction, pPetTarget);
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->AddEscapeSetting(TEscapeSet_No1Lv, ptr);
	}
	else if (state == Qt::Unchecked)
	{
		g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_No1Lv);
	}
}

void GameBattleWgt::on_checkBox_noBossEscape_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		CBattleCondition_EnemyType *pCondition = (CBattleCondition_EnemyType *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyType);
		pCondition->setConditionPercentage(false);
		pCondition->setConditionValue(1);
		pCondition->setRelation(dtCompare_NotEqual); //是否boss战
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		CBattleAction *pPetAction = nullptr;
		CBattleTarget *pPetTarget = nullptr;
		CreatePetActionTarget(pPetAction, pPetTarget);
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->AddEscapeSetting(TEscapeSet_NoBoss, ptr);
	}
	else if (state == Qt::Unchecked)
	{
		g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_NoBoss);
	}
}

void GameBattleWgt::on_pushButton_EscapePetSet_clicked()
{
	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(m_pEscapeSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();

		CBattleAction *pCustomPetAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(pPetAction->GetDevType());
		CBattleTarget *pCustomPetTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(pPetTarget->GetDevType());
		memcpy(pCustomPetAction, pPetAction, g_battleModuleReg.GetClassSize(pPetAction->GetDevType()));
		memcpy(pCustomPetTarget, pPetTarget, g_battleModuleReg.GetClassSize(pPetTarget->GetDevType()));
		m_pEscapePetAction = pCustomPetAction;
		m_pEscapePetTarget = pCustomPetTarget;

		//Setting主要是用来复用对话框，宠物部分有用，其他设置无实际意义
		CBattleCondition_EnemyType *pCondition = (CBattleCondition_EnemyType *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyType);
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		m_pEscapeSetting = CBattleSettingPtr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, m_pEscapePetAction, m_pEscapePetTarget));
	}
}

//void GameBattleWgt::on_radioButton_EscapeNothing_clicked()
//{
//	m_nEscapePetAction = EscapePetAction_Nothing;
//}
//
//void GameBattleWgt::on_radioButton_EscapeGuard_clicked()
//{
//	m_nEscapePetAction = EscapePetAction_Guard;
//}
//
//void GameBattleWgt::on_radioButton_EscapeAttack_clicked()
//{
//	m_nEscapePetAction = EscapePetAction_Attack;
//}
//指定怪逃跑
void GameBattleWgt::on_groupBox_SpecialEnemyEscape_toggled(bool checked)
{
	if (checked)
	{
		QStringList specialEnemyList;
		int ncount = ui.listWidget_SpecialEnemy->count();
		for (int i = 0; i < ncount; ++i)
		{
			QListWidgetItem *pItem = ui.listWidget_SpecialEnemy->item(i);
			if (pItem->checkState() == Qt::Checked)
				specialEnemyList.append(pItem->text());
		}
		for (int i = 0; i < specialEnemyList.size(); ++i)
		{
			CBattleCondition_EnemyUnit *pCondition = (CBattleCondition_EnemyUnit *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyUnit);
			pCondition->setConditionPercentage(false);
			pCondition->SetUnitName(specialEnemyList[i]); //保护值 1
			pCondition->setRelation(dtCompare_Contain);	  //敌人名称包含
			CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
			CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
			CBattleAction *pPetAction = nullptr;
			CBattleTarget *pPetTarget = nullptr;
			CreatePetActionTarget(pPetAction, pPetTarget);
			CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, pPetAction, pPetTarget));
			g_pAutoBattleCtrl->AddEscapeSetting(TEscapeSet_SpecialEnemy, ptr);
		}
	}
	else
	{
		g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_SpecialEnemy);
	}
}
//增加遇指定敌人逃跑
void GameBattleWgt::on_pushButton_AddSpecialEnemy_clicked()
{
	QString szText = ui.lineEdit_SpecialEnemy->text();
	if (szText.isEmpty())
		return;
	ui.lineEdit_SpecialEnemy->setText("");
	QListWidgetItem *pItem = new QListWidgetItem(szText);
	pItem->setCheckState(Qt::Unchecked);
	ui.listWidget_SpecialEnemy->addItem(pItem);
}
//指定怪逃跑项设置
void GameBattleWgt::doSpecialEnemyItemChanged(QListWidgetItem *pItem)
{
	if (pItem->checkState() == Qt::Checked)
	{
		CBattleCondition_EnemyUnit *pCondition = (CBattleCondition_EnemyUnit *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyUnit);
		pCondition->setConditionPercentage(false);
		pCondition->SetUnitName(pItem->text());		//保护值
		pCondition->setRelation(dtCompare_Contain); //敌人名称包含
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		CBattleAction *pPetAction = nullptr;
		CBattleTarget *pPetTarget = nullptr;
		CreatePetActionTarget(pPetAction, pPetTarget);
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->AddEscapeSetting(TEscapeSet_SpecialEnemy, ptr);
	}
	else if (pItem->checkState() == Qt::Unchecked)
	{
		g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_SpecialEnemy, pItem->text());
	}
}
//平均等级小于指定值逃跑
void GameBattleWgt::on_checkBox_EnemyAvgLv_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		int nCount = ui.lineEdit_EnemyAvgLv->text().toInt();
		CBattleCondition_EnemyAvgLevel *pCondition = (CBattleCondition_EnemyAvgLevel *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyAvgLevel);
		pCondition->setConditionPercentage(false);
		pCondition->setConditionValue(nCount);		 //平均等级
		pCondition->setRelation(dtCompare_LessThan); //小于
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		CBattleAction *pPetAction = nullptr;
		CBattleTarget *pPetTarget = nullptr;
		CreatePetActionTarget(pPetAction, pPetTarget);
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->AddEscapeSetting(TEscapeSet_EnemyAvgLv, ptr);
	}
	else if (state == Qt::Unchecked)
	{
		g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_EnemyAvgLv);
	}
}
//敌人个数大于指定值逃跑
void GameBattleWgt::on_checkBox_EnemyCount_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		int nCount = ui.lineEdit_EnemyCount->text().toInt();
		CBattleCondition_EnemyCount *pCondition = (CBattleCondition_EnemyCount *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyCount);
		pCondition->setConditionPercentage(false);
		pCondition->setConditionValue(nCount);			//数量
		pCondition->setRelation(dtCompare_GreaterThan); //大于
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		CBattleAction *pPetAction = nullptr;
		CBattleTarget *pPetTarget = nullptr;
		CreatePetActionTarget(pPetAction, pPetTarget);
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->AddEscapeSetting(TEscapeSet_EnemyCount, ptr);
	}
	else if (state == Qt::Unchecked)
	{
		g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_EnemyCount);
	}
}

void GameBattleWgt::on_checkBox_TeamCount_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		int nCount = ui.lineEdit_TeamCount->text().toInt();
		CBattleCondition_TeammateCount *pCondition = (CBattleCondition_TeammateCount *)g_battleModuleReg.CreateNewBattleObj(dtCondition_TeammateCount);
		pCondition->setConditionPercentage(false);
		pCondition->setConditionValue(nCount);		 //数量
		pCondition->setRelation(dtCompare_LessThan); //小于
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		CBattleAction *pPetAction = nullptr;
		CBattleTarget *pPetTarget = nullptr;
		CreatePetActionTarget(pPetAction, pPetTarget);
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->AddEscapeSetting(TEscapeSet_TeammateCount, ptr);
	}
	else if (state == Qt::Unchecked)
	{
		g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_TeammateCount);
	}
}

void GameBattleWgt::on_lineEdit_TeamCount_editingFinished()
{
	int nCount = ui.lineEdit_TeamCount->text().toInt();
	auto ptr = g_pAutoBattleCtrl->GetEscapeSetting(TEscapeSet_TeammateCount);
	if (ptr)
	{
		CBattleCondition_TeammateCount *pCondition = (CBattleCondition_TeammateCount *)ptr->m_condition;
		pCondition->setConditionValue(nCount);
	}
}

void GameBattleWgt::on_lineEdit_EnemyCount_editingFinished()
{
	auto pSetting = g_pAutoBattleCtrl->GetEscapeSetting(TEscapeSet_EnemyCount);
	if (pSetting)
	{
		CBattleCondition_EnemyCount *pCondition = (CBattleCondition_EnemyCount *)pSetting->m_condition;
		if (pCondition)
		{
			int nCount = ui.lineEdit_EnemyCount->text().toInt();
			pCondition->setConditionValue(nCount);
		}
	}
}

void GameBattleWgt::on_lineEdit_EnemyAvgLv_editingFinished()
{
	auto pSetting = g_pAutoBattleCtrl->GetEscapeSetting(TEscapeSet_EnemyAvgLv);
	if (pSetting)
	{
		CBattleCondition_EnemyAvgLevel *pCondition = (CBattleCondition_EnemyAvgLevel *)pSetting->m_condition;
		if (pCondition)
		{
			int nCount = ui.lineEdit_EnemyAvgLv->text().toInt();
			pCondition->setConditionValue(nCount);
		}
	}
}

void GameBattleWgt::doLoadUserConfig(QSettings &iniFile)
{
	ui.customBattleWgt->doLoadUserConfig(iniFile);
	//战斗设置读取
	iniFile.beginGroup("BattleSet");
	ui.checkBox_auto->setChecked(iniFile.value("AutoBattle").toBool());
	ui.checkBox_HighSpeed->setChecked(iniFile.value("HighSpeed").toBool());
	int nDelayTime = iniFile.value("HightSpeedDelay").toInt();
	ui.comboBox_highSpeedDelay->setCurrentIndex(ui.comboBox_highSpeedDelay->findData(nDelayTime));
	ui.checkBox_firstNoDelay->setChecked(iniFile.value("FirstRoundNoDelay").toBool());
	ui.checkBox_bossProtect->setChecked(iniFile.value("BossProtect").toBool());
	ui.checkBox_Lv1Protect->setChecked(iniFile.value("Lv1Protect").toBool());
	ui.checkBox_LockCountdown->setChecked(iniFile.value("LockCountdown").toBool());
	ui.checkBox_ShowHPMP->setChecked(iniFile.value("ShowHPMP").toBool());
	g_pAutoBattleCtrl->OnSetShowHPMPEnabled(ui.checkBox_ShowHPMP->checkState());
	ui.checkBox_PetDoubleAction->setChecked(iniFile.value("PetDoubleAction").toBool());
	ui.checkBox_PlayerForceAction->setChecked(iniFile.value("PlayerForceAction").toBool());
	ui.checkBox_noAnimation->setChecked(iniFile.value("NoSwitchAnim").toBool());
	ui.checkBox_allEscape->setChecked(iniFile.value("AllEscape").toBool());
	ui.checkBox_NoLv1Escape->setChecked(iniFile.value("NoLv1Escape").toBool());
	//	ui.checkBox_noBossEscape->setChecked(iniFile.value("IsNoBoss", ui.checkBox_noBossEscape->isChecked()); //无Boss怪逃跑
	ui.groupBox_noPet->setChecked(iniFile.value("NoPetDoubleAction").toBool());		//不带宠二动
	ui.radioButton_attack->setChecked(iniFile.value("NoPetActionAttack").toBool()); //不带宠二动攻击
	ui.radioButton_defense->setChecked(iniFile.value("NoPetActionGuard").toBool()); //不带宠二动防御

	ui.groupBox_SpecialEnemyEscape->setChecked(iniFile.value("SpecialEscape").toBool());
	ui.horizontalSlider_moveSpeed->setValue(iniFile.value("MoveSpeed").toInt());
	ui.checkBox_OverTimeT->setChecked(iniFile.value("OverTimeT").toBool());
	iniFile.endGroup();

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

		//	nDevType = iniFile.value(QString("ActionType%1").arg(i)).toInt();
		sDevType = iniFile.value(QString("ActionType%1").arg(i)).toString();
		nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		CBattleAction *pAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(nDevType);
		int nActionTextValCount = iniFile.value(QString("ActionTextValCount%1").arg(i)).toInt();
		QStringList textAction;
		for (int n = 0; n < nActionTextValCount; ++n)
		{
			textAction.append(iniFile.value(QString("ActionTextVal%1-%2").arg(i).arg(n)).toString());
		}
		pAction->SetActionTextData(textAction);

		int nActionValCount = iniFile.value(QString("ActionValCount%1").arg(i)).toInt();
		QList<int> actionData;
		for (int n = 0; n < nActionValCount; ++n)
		{
			QVariant tmpVal = iniFile.value(QString("ActionVal%1-%2").arg(i).arg(n));
			actionData.append(tmpVal.toInt());
		}
		pAction->SetActionData(actionData);

		//		nDevType = iniFile.value(QString("PetActionType%1").arg(i)).toInt();
		sDevType = iniFile.value(QString("PetActionType%1").arg(i)).toString();
		nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		CBattleAction *pPetAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(nDevType);
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

		//	nDevType = iniFile.value(QString("TargetType%1").arg(i)).toInt();
		sDevType = iniFile.value(QString("TargetType%1").arg(i)).toString();
		nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		CBattleTarget *pTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(nDevType);

		int tgtVal = 0;
		QString sTgtVal = iniFile.value(QString("TargetVal%1").arg(i)).toString();
		tgtVal = sTgtVal.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			tgtVal = g_pAutoBattleCtrl->GetBattleTypeFromText(sTgtVal);
		pTarget->SetTargetVal(tgtVal);
		pTarget->SetTargetSpecialName(iniFile.value(QString("TargetSpcName%1").arg(i)).toString());

		//nDevType = iniFile.value(QString("PetTargetType%1").arg(i)).toInt();
		sDevType = iniFile.value(QString("PetTargetType%1").arg(i)).toString();
		nDevType = sDevType.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
		CBattleTarget *pPetTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(nDevType);

		sTgtVal = iniFile.value(QString("PetTargetVal%1").arg(i)).toString();
		tgtVal = sTgtVal.toInt(&bTrans);
		if (!bTrans) //字符串 转为int
			tgtVal = g_pAutoBattleCtrl->GetBattleTypeFromText(sTgtVal);
		pPetTarget->SetTargetVal(tgtVal);
		pPetTarget->SetTargetSpecialName(iniFile.value(QString("PetTargetSpcName%1").arg(i)).toString());

		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCond, pCondition2, pAction, pTarget, pPetAction, pPetTarget));
		return ptr;
	};
	//战斗设置
	iniFile.beginGroup("AutoAction");
	g_pAutoBattleCtrl->ClearSpecifNumEnemySetting();
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
			g_pAutoBattleCtrl->AddSpecifNumEnemySetting(ptr, szVal.toInt());
		}
	}
	iniFile.endGroup();

	//回合设置
	iniFile.beginGroup("AutoRoundAction");
	g_pAutoBattleCtrl->ClearSpecifNumRoundSetting();
	//1-10敌人设置
	int nRoundSettingCount = iniFile.value("count").toInt();
	for (int i = 0; i < nRoundSettingCount; ++i)
	{
		auto ptr = LoadSettingCfg(i);
		if (ptr)
		{
			auto pCond = ptr->m_condition;
			QString szVal;
			pCond->GetConditionValue(szVal);
			g_pAutoBattleCtrl->AddSpecifNumRoundSetting(ptr, szVal.toInt());
		}
	}
	iniFile.endGroup();
	//读取逃跑设置 自动移除
	iniFile.beginGroup("EscapeSet");
	//逃跑宠物配置读取 放最上面，下面逃跑创建时候，可以读取
	auto sDevType = iniFile.value(QString("EscapePetActionType")).toString();
	bool bTrans = false;
	auto nDevType = sDevType.toInt(&bTrans);
	if (!bTrans) //字符串 转为int
		nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
	CBattleAction *pPetAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(nDevType);
	auto nActionTextValCount = iniFile.value(QString("EscapePetActionTextValCount")).toInt();
	QStringList textAction;
	for (int n = 0; n < nActionTextValCount; ++n)
	{
		textAction.append(iniFile.value(QString("EscapePetActionTextVal-%1").arg(n)).toString());
	}
	if (pPetAction)
		pPetAction->SetActionTextData(textAction);

	auto nActionValCount = iniFile.value(QString("EscapePetActionValCount")).toInt();
	QList<int> actionData;
	for (int n = 0; n < nActionValCount; ++n)
	{
		QVariant tmpVal = iniFile.value(QString("EscapePetActionVal-%1").arg(n));
		if (n == 0)
		{
			if (g_pAutoBattleCtrl->GetBattleTypeFromText(tmpVal.toString()) == 0)
				actionData.append(tmpVal.toInt());
			else
				actionData.append(g_pAutoBattleCtrl->GetBattleTypeFromText(tmpVal.toString()));
		}
		else
			actionData.append(tmpVal.toInt());
	}
	if (pPetAction)
		pPetAction->SetActionData(actionData);

	sDevType = iniFile.value(QString("EscapePetTargetType")).toString();
	nDevType = sDevType.toInt(&bTrans);
	if (!bTrans) //字符串 转为int
		nDevType = g_pAutoBattleCtrl->GetBattleTypeFromText(sDevType);
	CBattleTarget *pPetTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(nDevType);

	auto sTgtVal = iniFile.value(QString("EscapePetTargetVal")).toString();
	auto tgtVal = sTgtVal.toInt(&bTrans);
	if (!bTrans) //字符串 转为int
		tgtVal = g_pAutoBattleCtrl->GetBattleTypeFromText(sTgtVal);
	if (pPetTarget)
	{
		pPetTarget->SetTargetVal(tgtVal);
		pPetTarget->SetTargetSpecialName(iniFile.value(QString("EscapePetTargetSpcName")).toString());
	}
	if (pPetAction && pPetTarget)
	{
		m_pEscapePetAction = pPetAction;
		m_pEscapePetTarget = pPetTarget;
		CBattleCondition_EnemyType *pCondition = (CBattleCondition_EnemyType *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyType);
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleAction *pPlayerAction = (CBattleAction *)g_battleModuleReg.CreateNewBattleObj(dtAction_PlayerEscape);
		//自动释放之前内存
		m_pEscapeSetting = CBattleSettingPtr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, nullptr, m_pEscapePetAction, m_pEscapePetTarget));
	}

	ui.checkBox_allEscape->setChecked(iniFile.value("IsAllEscape").toBool());
	ui.checkBox_NoLv1Escape->setChecked(iniFile.value("IsNoLv1").toBool());
	ui.checkBox_noBossEscape->setChecked(iniFile.value("IsNoBoss").toBool()); //无Boss怪逃跑

	ui.groupBox_SpecialEnemyEscape->setChecked(iniFile.value("IsSpecialEnemy").toBool());
	/*ui.radioButton_EscapeAttack->setChecked(iniFile.value("PetAttack").toBool());
	ui.radioButton_EscapeGuard->setChecked(iniFile.value("PetGuard").toBool());
	ui.radioButton_EscapeNothing->setChecked(iniFile.value("PetNothing").toBool());*/
	ui.checkBox_EnemyAvgLv->setChecked(iniFile.value("IsEnemyAvgLv").toBool());
	ui.checkBox_EnemyCount->setChecked(iniFile.value("IsEnemyCount").toBool());
	ui.lineEdit_EnemyAvgLv->setText(iniFile.value("EnemyAvgLv").toString());
	ui.lineEdit_EnemyCount->setText(iniFile.value("EnemyCount").toString());

	int ncount = iniFile.value("SpecialEnemyCount").toInt();
	bool bChecked = false;
	ui.listWidget_SpecialEnemy->clear();
	//移除逃跑设置
	g_pAutoBattleCtrl->RemoveEscapeSetting(TEscapeSet_SpecialEnemy);
	//勾选自动新增
	for (int i = 0; i < ncount; ++i)
	{
		QListWidgetItem *pItem = new QListWidgetItem(iniFile.value(QString("SpecialEnemy%1").arg(i)).toString());
		bChecked = iniFile.value(QString("SpecialEnemy%1Checked").arg(i)).toBool();
		pItem->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);
		ui.listWidget_SpecialEnemy->addItem(pItem);
	}
	iniFile.endGroup();

	//读取保护设置

	//内置保护部分保存
	iniFile.beginGroup("BaoHu");
	g_pAutoBattleCtrl->ClearInternalProtectSetting();
	//界面新增以及删除
	for (int i = 0; i < m_pBaohuCheckBoxList.size(); ++i)
	{
		m_pBaohuCheckBoxList[i]->setChecked(iniFile.value(QString("checked%1").arg(i)).toBool());
		m_pBaohuLineEditList[i]->setText(iniFile.value(QString("item%1").arg(i)).toString());
	}
	//读取配置
	nSettingCount = iniFile.value("count").toInt();
	for (int i = 0; i < nSettingCount; ++i)
	{
		auto ptr = LoadSettingCfg(i);
		if (ptr)
		{
			auto pCond = ptr->m_condition;
			int nBaohuType = iniFile.value(QString("BaoHuType%1").arg(i)).toInt();
			g_pAutoBattleCtrl->AddInternalProtectSetting(nBaohuType, ptr);
		}
	}
	iniFile.endGroup();

	//内置Lv1抓宠设置
	iniFile.beginGroup("Level1");
	g_pAutoBattleCtrl->ClearHaveLv1Setting();

	ui.groupBox_Have1LvEnemy->setChecked(iniFile.value("enabled").toBool());
	for (int i = 0; i < m_pLv1CheckBoxList.size(); ++i)
	{
		m_pLv1CheckBoxList[i]->setChecked(iniFile.value(QString("checked%1").arg(i)).toBool());
		if (m_pLv1LineEditList[i])
		{
			m_pLv1LineEditList[i]->setText(iniFile.value(QString("item%1").arg(i)).toString());
		}
	}
	nSettingCount = iniFile.value("count").toInt();
	for (int i = 0; i < nSettingCount; ++i)
	{
		auto ptr = LoadSettingCfg(i);
		if (ptr)
		{
			auto pCond = ptr->m_condition;
			int nLv1Type = iniFile.value(QString("type%1").arg(i)).toInt();
			g_pAutoBattleCtrl->addHaveLv1Setting(nLv1Type, ptr);
		}
	}
	iniFile.endGroup();
	//同步数据到游戏
	g_pAutoBattleCtrl->OnSetAutoBattle(ui.checkBox_auto->checkState());
	g_pAutoBattleCtrl->OnSetHighSpeed(ui.checkBox_HighSpeed->checkState());
	g_pAutoBattleCtrl->OnSetFRND(ui.checkBox_firstNoDelay->checkState());
	g_pAutoBattleCtrl->OnSetBOSSProtect(ui.checkBox_bossProtect->checkState());
	g_pAutoBattleCtrl->OnSetLockCountdown(ui.checkBox_LockCountdown->checkState());
	g_pAutoBattleCtrl->OnSetShowHPMPEnabled(ui.checkBox_ShowHPMP->checkState());
	g_pAutoBattleCtrl->OnSetPetDoubleAction(ui.checkBox_PetDoubleAction->checkState());
	g_pAutoBattleCtrl->OnSetPlayerForceAction(ui.checkBox_PlayerForceAction->checkState());
	g_pAutoBattleCtrl->OnSetNoSwitchAnim(ui.checkBox_noAnimation->checkState());
	g_pAutoBattleCtrl->OnSetLv1Protect(ui.checkBox_Lv1Protect->checkState());
	g_pAutoBattleCtrl->OnSetAllEscape(ui.checkBox_allEscape->checkState());
	g_pAutoBattleCtrl->OnSetNoLv1Escape(ui.checkBox_NoLv1Escape->checkState());
	//高速延时
	on_comboBox_highSpeedDelay_currentIndexChanged(ui.comboBox_highSpeedDelay->currentIndex());

	on_checkBox_selfHp1_stateChanged(ui.checkBox_selfHp1->checkState());
	on_checkBox_selfHp2_stateChanged(ui.checkBox_selfHp2->checkState());
	on_checkBox_selfMp1_stateChanged(ui.checkBox_selfMp1->checkState());
	on_checkBox_selfMp2_stateChanged(ui.checkBox_selfMp2->checkState());
	on_checkBox_teammateHp1_stateChanged(ui.checkBox_teammateHp1->checkState());
	on_checkBox_petHp_stateChanged(ui.checkBox_petHp->checkState());
	on_checkBox_petMp_stateChanged(ui.checkBox_petMp->checkState());
	on_checkBox_troopHp_stateChanged(ui.checkBox_troopHp->checkState());

	on_lineEdit_PlayerHp1_editingFinished();
	on_lineEdit_PlayerHp2_editingFinished();
	on_lineEdit_PlayerMp1_editingFinished();
	on_lineEdit_PlayerMp2_editingFinished();
	on_lineEdit_PetHp_editingFinished();
	on_lineEdit_PetMp_editingFinished();
	on_lineEdit_TeammateHp_editingFinished();
	on_lineEdit_TroopHp_editingFinished();
	on_checkBox_allEscape_stateChanged(ui.checkBox_allEscape->checkState());
	on_checkBox_NoLv1Escape_stateChanged(ui.checkBox_NoLv1Escape->checkState());
	on_checkBox_noBossEscape_stateChanged(ui.checkBox_noBossEscape->checkState());
	on_groupBox_SpecialEnemyEscape_toggled(ui.groupBox_SpecialEnemyEscape->isChecked());
	on_checkBox_EnemyAvgLv_stateChanged(ui.checkBox_EnemyAvgLv->checkState());
	on_checkBox_EnemyCount_stateChanged(ui.checkBox_EnemyCount->checkState());
	on_checkBox_TeamCount_stateChanged(ui.checkBox_TeamCount->checkState());
	on_lineEdit_TeamCount_editingFinished();
	on_lineEdit_EnemyCount_editingFinished();
	on_lineEdit_EnemyAvgLv_editingFinished();
	on_checkBox_OverTimeT_stateChanged(ui.checkBox_OverTimeT->checkState());
	on_horizontalSlider_moveSpeed_valueChanged(ui.horizontalSlider_moveSpeed->value());
	on_checkBox_Lv1FilterHp_stateChanged(ui.checkBox_Lv1FilterHp->checkState());
	on_checkBox_Lv1FilterMp_stateChanged(ui.checkBox_Lv1FilterMp->checkState());
	on_lineEdit_Lv1FilterHp_editingFinished();
	on_lineEdit_Lv1FilterMp_editingFinished();
	on_lineEdit_Lv1HpVal_editingFinished();
	on_groupBox_Have1LvEnemy_toggled(ui.groupBox_Have1LvEnemy->isChecked());
	on_checkBox_Lv1RoundOne_stateChanged(ui.checkBox_Lv1RoundOne->checkState()); //1级怪第一回合设置
	on_checkBox_Lv1Hp_stateChanged(ui.checkBox_Lv1Hp->checkState());
	on_checkBox_Lv1LastSet_stateChanged(ui.checkBox_Lv1LastSet->checkState());
	on_checkBox_Lv1CleanNo1_stateChanged(ui.checkBox_Lv1CleanNo1->checkState());
	on_checkBox_Lv1Recall_stateChanged(ui.checkBox_Lv1Recall->checkState());
	//激活无宠二动
	on_groupBox_noPet_toggled(ui.groupBox_noPet->isChecked());
	CreateNoPetDoubleAction();
	ResetNumEnemyBtnColor();
	ResetNumRoundBtnColor();
}

void GameBattleWgt::doSaveUserConfig(QSettings &iniFile)
{
	ui.customBattleWgt->doSaveUserConfig(iniFile);

	//战斗部分  应该用界面的  懒得改 后面有时间改
	iniFile.beginGroup("BattleSet");
	iniFile.setValue("AutoBattle", g_pAutoBattleCtrl->m_bAutoBattle);									//是否自动战斗
	iniFile.setValue("HighSpeed", g_pAutoBattleCtrl->m_bHighSpeed);										//是否高速战斗
																										//	iniFile.setValue("HightSpeedDelay", g_pAutoBattleCtrl->m_nHightSpeedDelay);		//延时
	iniFile.setValue("HightSpeedDelay", ui.comboBox_highSpeedDelay->currentData(Qt::UserRole).toInt()); //延时
	iniFile.setValue("FirstRoundNoDelay", g_pAutoBattleCtrl->m_bFirstRoundNoDelay);						//第一回合加速
	iniFile.setValue("BossProtect", g_pAutoBattleCtrl->m_bBOSSProtect);									//Boss战停止
	iniFile.setValue("Lv1Protect", g_pAutoBattleCtrl->m_bLevelOneProtect);								//1级怪停止
	iniFile.setValue("LockCountdown", g_pAutoBattleCtrl->m_bLockCountdown);								//锁定倒计时
	iniFile.setValue("ShowHPMP", g_pAutoBattleCtrl->m_bShowHPMP);										//显血
	iniFile.setValue("PetDoubleAction", g_pAutoBattleCtrl->m_bPetDoubleAction);							//宠物2动
	iniFile.setValue("PlayerForceAction", g_pAutoBattleCtrl->m_bPlayerForceAction);						//人物强制行动
	iniFile.setValue("NoSwitchAnim", g_pAutoBattleCtrl->m_bNoSwitchAnim);								//切图
	iniFile.setValue("AllEscape", g_pAutoBattleCtrl->m_bAllEscape);										//全跑
	iniFile.setValue("NoLv1Escape", g_pAutoBattleCtrl->m_bNoLv1Escape);									//无1级怪逃跑
	iniFile.setValue("NoBossEscape", ui.checkBox_noBossEscape->isChecked());							//无Boss怪逃跑
	iniFile.setValue("NoPetDoubleAction", ui.groupBox_noPet->isChecked());								//不带宠二动
	iniFile.setValue("NoPetActionAttack", ui.radioButton_attack->isChecked());							//不带宠二动攻击
	iniFile.setValue("NoPetActionGuard", ui.radioButton_defense->isChecked());							//不带宠二动防御

	iniFile.setValue("SpecialEscape", g_pAutoBattleCtrl->m_bSpecialEnemyEscape); //指定怪逃跑
	iniFile.setValue("MoveSpeed", ui.horizontalSlider_moveSpeed->value());		 //高速移动速度
	iniFile.setValue("OverTimeT", ui.checkBox_OverTimeT->isChecked());			 //超时被T
	iniFile.endGroup();
	//战斗设置

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

		CBattleAction *pAction = pSetting->m_playerAction;
		iniFile.setValue(QString("ActionType%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pAction->GetDevType()));
		QStringList textAction = pAction->GetActionTextData();
		for (int i = 0; i < textAction.size(); ++i)
		{
			iniFile.setValue(QString("ActionTextVal%1-%2").arg(index).arg(i), textAction.at(i));
		}
		iniFile.setValue(QString("ActionTextValCount%1").arg(index), textAction.size());

		QList<int> actionData = pAction->GetActionData();
		for (int i = 0; i < actionData.size(); ++i)
		{
			iniFile.setValue(QString("ActionVal%1-%2").arg(index).arg(i), actionData.at(i));
		}
		iniFile.setValue(QString("ActionValCount%1").arg(index), actionData.size());

		CBattleAction *pPetAction = pSetting->m_petAction;
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

		CBattleTarget *pTarget = pSetting->m_playerTarget;
		int targetID = pTarget->GetTargetSelectId();
		iniFile.setValue(QString("TargetVal%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(targetID));
		iniFile.setValue(QString("TargetType%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pTarget->GetDevType()));
		iniFile.setValue(QString("TargetSpcName%1").arg(index), pTarget->GetTargetSpecialName());

		CBattleTarget *pPetTarget = pSetting->m_petTarget;
		targetID = pPetTarget->GetTargetSelectId();
		iniFile.setValue(QString("PetTargetVal%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(targetID));
		iniFile.setValue(QString("PetTargetType%1").arg(index), g_pAutoBattleCtrl->GetBattleTypeText(pPetTarget->GetDevType()));
		iniFile.setValue(QString("PetTargetSpcName%1").arg(index), pPetTarget->GetTargetSpecialName());
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
	iniFile.beginGroup("AutoAction");
	//1-10敌人设置
	CBattleSettingList pSettingList = g_pAutoBattleCtrl->GetSpecNumEnemySettings();
	int nSettingCount = SaveSettingFun(pSettingList);
	iniFile.setValue("count", nSettingCount);
	iniFile.endGroup();

	//回合设置
	iniFile.beginGroup("AutoRoundAction");
	//1-10回合设置
	CBattleSettingList pRoundSettingList = g_pAutoBattleCtrl->GetSpecNumRoundSettings();
	nSettingCount = SaveSettingFun(pRoundSettingList);
	iniFile.setValue("count", nSettingCount);
	iniFile.endGroup();

	//逃跑部分保存
	iniFile.beginGroup("EscapeSet");
	iniFile.setValue("IsAllEscape", ui.checkBox_allEscape->isChecked());
	iniFile.setValue("IsNoLv1", ui.checkBox_NoLv1Escape->isChecked());
	iniFile.setValue("IsNoBoss", ui.checkBox_noBossEscape->isChecked()); //无Boss怪逃跑
	iniFile.setValue("IsSpecialEnemy", ui.groupBox_SpecialEnemyEscape->isChecked());
	//iniFile.setValue("PetAttack", ui.radioButton_EscapeAttack->isChecked());
	//iniFile.setValue("PetGuard", ui.radioButton_EscapeGuard->isChecked());
	//iniFile.setValue("PetNothing", ui.radioButton_EscapeNothing->isChecked());
	iniFile.setValue("IsEnemyAvgLv", ui.checkBox_EnemyAvgLv->isChecked());
	iniFile.setValue("IsEnemyCount", ui.checkBox_EnemyCount->isChecked());
	iniFile.setValue("EnemyAvgLv", ui.lineEdit_EnemyAvgLv->text());
	iniFile.setValue("EnemyCount", ui.lineEdit_EnemyCount->text());
	if (ui.groupBox_SpecialEnemyEscape->isChecked())
	{
		int ncount = ui.listWidget_SpecialEnemy->count();
		iniFile.setValue("SpecialEnemyCount", ncount);
		for (int i = 0; i < ncount; ++i)
		{
			QListWidgetItem *pItem = ui.listWidget_SpecialEnemy->item(i);
			iniFile.setValue(QString("SpecialEnemy%1").arg(i), pItem->text());
			iniFile.setValue(QString("SpecialEnemy%1Checked").arg(i), pItem->checkState() == Qt::Checked);
		}
	}
	//逃跑宠物配置保存
	iniFile.setValue(QString("EscapePetActionType"), g_pAutoBattleCtrl->GetBattleTypeText(m_pEscapePetAction->GetDevType()));
	auto textAction = m_pEscapePetAction->GetActionTextData();
	for (int i = 0; i < textAction.size(); ++i)
	{
		iniFile.setValue(QString("EscapePetActionTextVal-%1").arg(i), textAction.at(i));
	}
	iniFile.setValue(QString("EscapePetActionTextValCount"), textAction.size());

	auto actionData = m_pEscapePetAction->GetActionData();
	for (int i = 0; i < actionData.size(); ++i)
	{
		if (i == 0)
			iniFile.setValue(QString("EscapePetActionVal-%1").arg(i), g_pAutoBattleCtrl->GetBattleTypeText(actionData.at(i)));
		else
			iniFile.setValue(QString("EscapePetActionVal-%1").arg(i), actionData.at(i));
	}
	iniFile.setValue(QString("EscapePetActionValCount"), actionData.size());

	auto targetID = m_pEscapePetTarget->GetTargetSelectId();
	iniFile.setValue(QString("EscapePetTargetVal"), g_pAutoBattleCtrl->GetBattleTypeText(targetID));
	iniFile.setValue(QString("EscapePetTargetType"), g_pAutoBattleCtrl->GetBattleTypeText(m_pEscapePetTarget->GetDevType()));
	iniFile.setValue(QString("EscapePetTargetSpcName"), m_pEscapePetTarget->GetTargetSpecialName());

	iniFile.endGroup();
	//内置保护部分保存
	iniFile.beginGroup("BaoHu");
	for (int i = 0; i < m_pBaohuCheckBoxList.size(); ++i)
	{
		iniFile.setValue(QString("checked%1").arg(i), m_pBaohuCheckBoxList[i]->isChecked());
		iniFile.setValue(QString("item%1").arg(i), m_pBaohuLineEditList[i]->text());
	}

	nSettingCount = 0;
	auto pBaohuSettingHash = g_pAutoBattleCtrl->GetInternalProtectSettings();
	for (auto it = pBaohuSettingHash.begin(); it != pBaohuSettingHash.end(); ++it)
	{
		iniFile.setValue(QString("BaoHuType%1").arg(nSettingCount), it.key());
		auto pSetting = it.value();
		SaveOneSettingFun(pSetting, nSettingCount);
		nSettingCount++;
	}
	iniFile.setValue("count", nSettingCount);
	iniFile.endGroup();

	//内置Lv1抓宠设置
	iniFile.beginGroup("Level1");
	iniFile.setValue("enabled", ui.groupBox_Have1LvEnemy->isChecked());

	for (int i = 0; i < m_pLv1CheckBoxList.size(); ++i)
	{
		iniFile.setValue(QString("checked%1").arg(i), m_pLv1CheckBoxList[i]->isChecked());
		if (m_pLv1LineEditList[i])
			iniFile.setValue(QString("item%1").arg(i), m_pLv1LineEditList[i]->text());
		else
			iniFile.setValue(QString("item%1").arg(i), "");
	}
	nSettingCount = 0;
	auto pLv1SettingHash = g_pAutoBattleCtrl->GetInternalLv1Settings();
	for (auto it = pLv1SettingHash.begin(); it != pLv1SettingHash.end(); ++it)
	{
		iniFile.setValue(QString("type%1").arg(nSettingCount), it.key());
		auto pSetting = it.value();
		SaveOneSettingFun(pSetting, nSettingCount);
		nSettingCount++;
	}
	iniFile.setValue("count", nSettingCount);
	iniFile.endGroup();
}

void GameBattleWgt::on_checkBox_OverTimeT_stateChanged(int state)
{
	g_pGameCtrl->OnSetOverTimeT(state);
}

void GameBattleWgt::on_horizontalSlider_moveSpeed_valueChanged(int value)
{
	QString sText = QString("速度%1").arg(value);
	ui.label_moveSpeed->setText(sText);
	g_CGAInterface->SetMoveSpeed(value);
}

void GameBattleWgt::doPlayAlarmWav()
{
	m_pSoundAlarm->play();
}

void GameBattleWgt::doStopPalyAlarm()
{
	m_pSoundAlarm->stop();
}

void GameBattleWgt::on_checkBox_Lv1FilterHp_stateChanged(int state)
{
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_FilterHP, state == Qt::Checked ? true : false);
}

void GameBattleWgt::on_checkBox_Lv1FilterMp_stateChanged(int state)
{
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_FilterMP, state == Qt::Checked ? true : false);
}

void GameBattleWgt::on_lineEdit_Lv1FilterHp_editingFinished()
{
	int nVal = ui.lineEdit_Lv1FilterHp->text().toInt();
	g_pAutoBattleCtrl->SetLv1FilterMaxHpVal(nVal);
}

void GameBattleWgt::on_lineEdit_Lv1FilterMp_editingFinished()
{
	int nVal = ui.lineEdit_Lv1FilterMp->text().toInt();
	g_pAutoBattleCtrl->SetLv1FilterMaxMpVal(nVal);
}

void GameBattleWgt::on_groupBox_Have1LvEnemy_toggled(bool checked)
{
	g_pAutoBattleCtrl->OnSetHave1LvEnemy(checked);
}

void GameBattleWgt::on_checkBox_Lv1RoundOne_stateChanged(int state)
{
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_RoundOne, state == Qt::Checked ? true : false);
}

void GameBattleWgt::on_pushButton_Lv1RoundOneSet_clicked()
{
	//有1级怪 第一回合
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->GetHaveLv1Setting(TLv1EnemySet_RoundOne);
	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();

		CBattleCondition_Compare *pCondition = (CBattleCondition_Compare *)g_battleModuleReg.CreateNewBattleObj(dtCondition_Round);
		pCondition->setConditionValue(1);		  //第一回合
		pCondition->setRelation(dtCompare_Equal); //等于第一回合
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->addHaveLv1Setting(TLv1EnemySet_RoundOne, ptr);
	}
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_RoundOne, ui.checkBox_Lv1RoundOne->isChecked());
}

void GameBattleWgt::on_checkBox_Lv1Hp_stateChanged(int state)
{
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_Lv1HpAction, state == Qt::Checked ? true : false);
}

void GameBattleWgt::on_lineEdit_Lv1HpVal_editingFinished()
{
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->GetHaveLv1Setting(TLv1EnemySet_Lv1HpAction);
	if (pExistSetting)
	{
		CBattleCondition_Compare *pCondition = (CBattleCondition_Compare *)pExistSetting->m_condition;
		int nVal = ui.lineEdit_Lv1HpVal->text().toInt();
		pCondition->setConditionValue(nVal);
		g_pAutoBattleCtrl->addHaveLv1Setting(TLv1EnemySet_Lv1HpAction, pExistSetting);
	}
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_Lv1HpAction, ui.checkBox_Lv1Hp->isChecked());
}

void GameBattleWgt::on_pushButton_Lv1HpSet_clicked()
{
	//1级怪 血量判断
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->GetHaveLv1Setting(TLv1EnemySet_Lv1HpAction);
	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();
		int nVal = ui.lineEdit_Lv1HpVal->text().toInt();
		CBattleCondition_Compare *pCondition = (CBattleCondition_Compare *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyLv1Hp);
		pCondition->setConditionValue(nVal);
		pCondition->setRelation(dtCompare_GreaterThan);
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->addHaveLv1Setting(TLv1EnemySet_Lv1HpAction, ptr);
	}
}

void GameBattleWgt::on_checkBox_Lv1LastSet_stateChanged(int state)
{
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_LastSet, state == Qt::Checked ? true : false);
}

void GameBattleWgt::on_pushButton_Lv1LastSet_clicked()
{
	//1级怪 血量判断
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->GetHaveLv1Setting(TLv1EnemySet_LastSet);
	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();
		CBattleCondition_Compare *pCondition = (CBattleCondition_Compare *)g_battleModuleReg.CreateNewBattleObj(dtCondition_Round);
		pCondition->setConditionValue(1); //回合数大于1  就触发
		pCondition->setRelation(dtCompare_GreaterThan);
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->addHaveLv1Setting(TLv1EnemySet_LastSet, ptr);
	}
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_LastSet, ui.checkBox_Lv1LastSet->isChecked());
}

void GameBattleWgt::on_pushButton_Lv1CleanNo1_clicked()
{
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->GetHaveLv1Setting(TLv1EnemySet_CleanNoLv1);
	BattleSetDlg dlg(this);
	dlg.init();
	dlg.setBattleSetting(pExistSetting);
	if (dlg.exec() == QDialog::Accepted)
	{
		//获取数据 生成战斗方案
		CBattleAction *pPlayerAction = dlg.GetPlayerAction();
		CBattleAction *pPetAction = dlg.GetPetAction();
		CBattleTarget *pPlayerTarget = dlg.GetPlayerTarget();
		CBattleTarget *pPetTarget = dlg.GetPetTarget();
		CBattleCondition_Compare *pCondition = (CBattleCondition_Compare *)g_battleModuleReg.CreateNewBattleObj(dtCondition_EnemyLevel);
		pCondition->setConditionValue(1); //敌人等级不等于1  就触发
		pCondition->setRelation(dtCompare_NotEqual);
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->addHaveLv1Setting(TLv1EnemySet_CleanNoLv1, ptr);
	}
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_CleanNoLv1, ui.checkBox_Lv1CleanNo1->isChecked());
}

void GameBattleWgt::on_checkBox_Lv1CleanNo1_stateChanged(int state)
{
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_CleanNoLv1, state == Qt::Checked ? true : false);
}

void GameBattleWgt::on_checkBox_Lv1Recall_stateChanged(int state)
{
	CBattleSettingPtr pExistSetting = g_pAutoBattleCtrl->GetHaveLv1Setting(TLv1EnemySet_RecallPet);
	if (!pExistSetting)
	{
		CBattleAction *pPlayerAction = new CBattleAction_PlayerChangePet(TChangePet_Recall);
		CBattleAction *pPetAction = new CBattleAction_PetDoNothing(); //怕宠没学防御 什么也不做
		CBattleTarget *pPlayerTarget = (CBattleTarget *)g_battleModuleReg.CreateNewBattleObj(dtTarget_Pet);
		CBattleTarget *pPetTarget = (CBattleTarget *)new CBattleTarget_Enemy(dtTargetCondition_Random);
		//通过宠血判断 >=1 认为带宠 不带宠默认返回false
		CBattleCondition_Compare *pCondition = (CBattleCondition_Compare *)g_battleModuleReg.CreateNewBattleObj(dtCondition_PetHp);
		pCondition->setConditionValue(1); //敌人等级不等于1  就触发
		pCondition->setRelation(dtCompare_GreaterEqual);
		CBattleCondition *pCondition2 = new CBattleCondition_Ignore();
		CBattleSettingPtr ptr(new CBattleSetting(pCondition, pCondition2, pPlayerAction, pPlayerTarget, pPetAction, pPetTarget));
		g_pAutoBattleCtrl->addHaveLv1Setting(TLv1EnemySet_RecallPet, ptr);
	}
	g_pAutoBattleCtrl->OnSetHave1LvAction(TLv1EnemySet_RecallPet, state == Qt::Checked ? true : false);
}
//不带宠二动
void GameBattleWgt::on_groupBox_noPet_toggled(bool checked)
{
	auto pSetting = g_pAutoBattleCtrl->GetNoPetDoubleAction();
	if (pSetting == nullptr)
	{
		CreateNoPetDoubleAction();
	}
	else
		pSetting->bEnabled = checked;
}

void GameBattleWgt::on_checkBox_noPetFirstUse_stateChanged(int state)
{
	g_pAutoBattleCtrl->OnSetNoPetFirstAction(state);
}

void GameBattleWgt::on_radioButton_attack_clicked(bool checked)
{
	CreateNoPetDoubleAction();
}

void GameBattleWgt::on_radioButton_defense_clicked(bool checked)
{
	CreateNoPetDoubleAction();
}

void GameBattleWgt::on_radioButton_escape_clicked(bool checked)
{
	CreateNoPetDoubleAction();
}

void GameBattleWgt::on_pushButton_petDoubleAction_clicked()
{
	PetDoubleActionSetDlg dlg;
	dlg.exec();
}
