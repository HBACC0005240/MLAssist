#include "GamePetWgt.h"
#include "CGFunction.h"
#include "GPCalc.h"
#include "GameCtrl.h"
#include "constDef.h"
#include "stdafx.h"
#include "ITObjectDataMgr.h"
GamePetWgt::GamePetWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);

	auto pTabBar = ui.tabWidget->tabBar();

	m_pCalcPetBtn = new QPushButton("重载算档", this);
	m_pCalcPetBtn->setToolTip("会重新从管理工具那里获取宠物算档数据！");
	connect(m_pCalcPetBtn, SIGNAL(clicked()), this, SLOT(on_pushButton_reloadCalc_clicked()));
	m_pCalcPetBtn->setMaximumWidth(80);
	QWidget *pWidget = new QWidget(this);
	QHBoxLayout *pHLayout = new QHBoxLayout(pWidget);
	pHLayout->addWidget(m_pCalcPetBtn);
	pHLayout->setContentsMargins(0, 0, 0, 0);
	ui.tabWidget->setCornerWidget(pWidget);

	for (int i = 0; i < 13; ++i)
	{
		for (size_t n = 0; n < 5; n++)
		{
			QTableWidgetItem *pItem = new QTableWidgetItem();
			ui.tableWidget->setItem(i, n, pItem);
		}
	}
	//档次加黑提醒
	//QFont gradeFont;
	////gradeFont.setItalic(true);
	//gradeFont.setBold(true);
	//for (int i=0;i<5;++i)
	//{
	//	QTableWidgetItem *pItem = ui.tableWidget->item(5, i);
	//	if (pItem)
	//	{
	//		pItem->setFont(gradeFont);
	//	}
	//}

	connect(g_pGameCtrl, &GameCtrl::NotifyGamePetsInfo, this, &GamePetWgt::OnNotifyGetPetsInfo, Qt::ConnectionType::QueuedConnection);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); //打开右键功能
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onTableCustomContextMenu(const QPoint &)));
	init();
}

GamePetWgt::~GamePetWgt()
{
}
void GamePetWgt::doLoadUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("PetUi");
	ui.groupBox_DropPet->setChecked(iniFile.value("autoDrop").toBool());	 //是否打开自动扔宠
	ui.checkBox_DropLv->setChecked(iniFile.value("autoDropLevel").toBool()); //自动扔宠等级判断
	ui.checkBox_DropHp->setChecked(iniFile.value("autoDropHp").toBool());
	ui.checkBox_DropMp->setChecked(iniFile.value("autoDropMp").toBool());
	ui.checkBox_DropVigor->setChecked(iniFile.value("autoDropVigor").toBool());
	ui.checkBox_DropStrength->setChecked(iniFile.value("autoDropStrength").toBool());
	ui.checkBox_DropEndurance->setChecked(iniFile.value("autoDropEndurance").toBool());
	ui.checkBox_DropAgility->setChecked(iniFile.value("autoDropSpeed").toBool());
	ui.checkBox_DropMagical->setChecked(iniFile.value("autoDropMagical").toBool());
	ui.checkBox_attack->setChecked(iniFile.value("autoDropAttack").toBool());
	ui.checkBox_defensive->setChecked(iniFile.value("autoDropDefensive").toBool());
	ui.checkBox_agility->setChecked(iniFile.value("autoDropAgility").toBool());
	ui.checkBox_spirit->setChecked(iniFile.value("autoDropSpirit").toBool());
	ui.checkBox_recovery->setChecked(iniFile.value("autoDropRecovery").toBool());
	ui.checkBox_grade->setChecked(iniFile.value("autoDropGrade").toBool());
	ui.checkBox_realName->setChecked(iniFile.value("autoDropRealName").toBool());

	ui.lineEdit_DropLv->setText(iniFile.value("autoDropLevelVal").toString());
	ui.lineEdit_DropHp->setText(iniFile.value("autoDropHpVal").toString());
	ui.lineEdit_DropMp->setText(iniFile.value("autoDropMpVal").toString());
	ui.lineEdit_DropVigor->setText(iniFile.value("autoDropVigorVal").toString());
	ui.lineEdit_DropStrength->setText(iniFile.value("autoDropStrengthVal").toString());
	ui.lineEdit_DropDefense->setText(iniFile.value("autoDropEnduranceVal").toString());
	ui.lineEdit_DropAgility->setText(iniFile.value("autoDropSpeedVal").toString());
	ui.lineEdit_DropMagical->setText(iniFile.value("autoDropMagicalVal").toString());
	ui.lineEdit_attack->setText(iniFile.value("autoDropAttackVal").toString());
	ui.lineEdit_defensive->setText(iniFile.value("autoDropDefensiveVal").toString());
	ui.lineEdit_agility->setText(iniFile.value("autoDropAgilityVal").toString());
	ui.lineEdit_spirit->setText(iniFile.value("autoDropSpiritVal").toString());
	ui.lineEdit_recovery->setText(iniFile.value("autoDropRecoveryVal").toString());
	ui.lineEdit_grade->setText(iniFile.value("autoDropGradeVal").toString());
	ui.lineEdit_realName->setText(iniFile.value("autoDropRealNameVal").toString());
	//lineEdit需要手动调用下才能设置好值
	for (auto it = m_pDropLineEditMap.begin(); it != m_pDropLineEditMap.end(); ++it)
	{
		QLineEdit *pObj = (QLineEdit *)it.value();
		g_pGameCtrl->SetDropPetItemValue(it.key(), pObj->text());
	}

	ui.checkBox_RecallLoyalty->setChecked(iniFile.value("recallLoyalty").toBool());
	ui.checkBox_RecallMp->setChecked(iniFile.value("recallMp").toBool());
	ui.checkBox_SummonPet->setChecked(iniFile.value("autoSummon").toBool());
	ui.lineEdit_RecallLoyalty->setText(iniFile.value("recallLoyaltyVal").toString());
	ui.lineEdit_RecallMp->setText(iniFile.value("recallMpVal").toString());
	ui.comboBox_SummonPet->setCurrentIndex(ui.comboBox_SummonPet->findData(iniFile.value("autoSummonVal").toInt()));

	g_pGameCtrl->getGameBattlePetCfg()->nRecallLoyality = iniFile.value("recallLoyaltyVal").toInt();
	g_pGameCtrl->getGameBattlePetCfg()->nRecallMp = iniFile.value("recallMpVal").toInt();

	ui.checkBox_normalEnabled->setChecked(iniFile.value("autoAddPointNormalEnabled").toBool());
	ui.checkBox_burstEnabled->setChecked(iniFile.value("autoAddPointBurstEnabled").toBool());
	ui.AutoAddPoint_petName->setText(iniFile.value("autoAddPointName").toString());
	ui.AutoAddPoint_petRealName->setText(iniFile.value("autoAddPointRealName").toString());

	int normalBpType = iniFile.value("autoAddPointNormalType").toInt();
	int burstBpType = iniFile.value("autoAddPointBurstType").toInt();
	switch (normalBpType)
	{
		case TPET_POINT_TYPE_Vigor: ui.normalAddPointHp->setChecked(true); break;
		case TPET_POINT_TYPE_Strength: ui.normalAddPointAttack->setChecked(true); break;
		case TPET_POINT_TYPE_Endurance: ui.normalAddPointDefence->setChecked(true); break;
		case TPET_POINT_TYPE_Agility: ui.normalAddPointAgility->setChecked(true); break;
		case TPET_POINT_TYPE_Magical: ui.normalAddPointMp->setChecked(true); break;
		default: break;
	}
	switch (burstBpType)
	{
		case TPET_POINT_TYPE_Vigor: ui.burstAddPointHp->setChecked(true); break;
		case TPET_POINT_TYPE_Strength: ui.burstAddPointAttack->setChecked(true); break;
		case TPET_POINT_TYPE_Endurance: ui.burstAddPointDefence->setChecked(true); break;
		case TPET_POINT_TYPE_Agility: ui.burstAddPointAgility->setChecked(true); break;
		case TPET_POINT_TYPE_Magical: ui.burstAddPointMp->setChecked(true); break;
		default: break;
	}
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg)
	{
		petCfg->petName = iniFile.value("autoAddPointName").toString();
		petCfg->petRealName = iniFile.value("autoAddPointRealName").toString();
		petCfg->normalCfg.addBpType = normalBpType;
		petCfg->unnormalCfg.addBpType = burstBpType;
	}

	iniFile.endGroup();
}

void GamePetWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("PetUi");
	iniFile.setValue("autoDrop", ui.groupBox_DropPet->isChecked());				   //是否打开自动扔宠
	iniFile.setValue("autoDropLevel", ui.checkBox_DropLv->isChecked());			   //自动扔宠等级判断
	iniFile.setValue("autoDropHp", ui.checkBox_DropHp->isChecked());			   //自动扔宠等级判断
	iniFile.setValue("autoDropMp", ui.checkBox_DropMp->isChecked());			   //自动扔宠等级判断
	iniFile.setValue("autoDropVigor", ui.checkBox_DropVigor->isChecked());		   //自动扔宠Bp体力判断
	iniFile.setValue("autoDropStrength", ui.checkBox_DropStrength->isChecked());   //自动扔宠Bp力量判断
	iniFile.setValue("autoDropEndurance", ui.checkBox_DropEndurance->isChecked()); //自动扔宠Bp强度判断
	iniFile.setValue("autoDropSpeed", ui.checkBox_DropAgility->isChecked());	   //自动扔宠Bp速度判断
	iniFile.setValue("autoDropMagical", ui.checkBox_DropMagical->isChecked());	   //自动扔宠Bp魔力判断
	iniFile.setValue("autoDropAttack", ui.checkBox_attack->isChecked());		   //自动扔宠攻击判断
	iniFile.setValue("autoDropDefensive", ui.checkBox_defensive->isChecked());	   //自动扔宠防御判断
	iniFile.setValue("autoDropAgility", ui.checkBox_agility->isChecked());		   //自动扔宠敏捷判断
	iniFile.setValue("autoDropSpirit", ui.checkBox_spirit->isChecked());		   //自动扔宠精神判断
	iniFile.setValue("autoDropRecovery", ui.checkBox_recovery->isChecked());	   //自动扔宠回复判断
	iniFile.setValue("autoDropGrade", ui.checkBox_grade->isChecked());			   //自动扔宠档次判断
	iniFile.setValue("autoDropRealName", ui.checkBox_realName->isChecked());	   //自动扔宠档次判断
	iniFile.setValue("autoDropLevelVal", ui.lineEdit_DropLv->text());			   //自动扔宠等级判断
	iniFile.setValue("autoDropHpVal", ui.lineEdit_DropHp->text());				   //自动扔宠等级判断
	iniFile.setValue("autoDropMpVal", ui.lineEdit_DropMp->text());				   //自动扔宠等级判断
	iniFile.setValue("autoDropVigorVal", ui.lineEdit_DropVigor->text());		   //自动扔宠Bp体力判断
	iniFile.setValue("autoDropStrengthVal", ui.lineEdit_DropStrength->text());	   //自动扔宠Bp力量判断
	iniFile.setValue("autoDropEnduranceVal", ui.lineEdit_DropDefense->text());	   //自动扔宠Bp强度判断
	iniFile.setValue("autoDropSpeedVal", ui.lineEdit_DropAgility->text());		   //自动扔宠Bp速度判断
	iniFile.setValue("autoDropMagicalVal", ui.lineEdit_DropMagical->text());	   //自动扔宠Bp魔力判断
	iniFile.setValue("autoDropAttackVal", ui.lineEdit_attack->text());			   //自动扔宠攻击判断
	iniFile.setValue("autoDropDefensiveVal", ui.lineEdit_defensive->text());	   //自动扔宠防御判断
	iniFile.setValue("autoDropAgilityVal", ui.lineEdit_agility->text());		   //自动扔宠敏捷判断
	iniFile.setValue("autoDropSpiritVal", ui.lineEdit_spirit->text());			   //自动扔宠精神判断
	iniFile.setValue("autoDropRecoveryVal", ui.lineEdit_recovery->text());		   //自动扔宠回复判断
	iniFile.setValue("autoDropGradeVal", ui.lineEdit_grade->text());			   //自动扔宠档次判断
	iniFile.setValue("autoDropRealNameVal", ui.lineEdit_realName->text());		   //自动扔宠真实名称判断

	iniFile.setValue("recallLoyalty", ui.checkBox_RecallLoyalty->isChecked()); //是否打开召回指定忠诚
	iniFile.setValue("recallMp", ui.checkBox_RecallMp->isChecked());		   //是否打开召回指定魔
	iniFile.setValue("autoSummon", ui.checkBox_SummonPet->isChecked());		   //是否打开自动招宠
	iniFile.setValue("recallLoyaltyVal", ui.lineEdit_RecallLoyalty->text());   //是否打开召回指定忠诚
	iniFile.setValue("recallMpVal", ui.lineEdit_RecallMp->text());			   //是否打开召回指定魔
	iniFile.setValue("autoSummonVal", ui.comboBox_SummonPet->currentData());   //是否打开自动招宠

	iniFile.setValue("autoAddPointNormalEnabled", ui.checkBox_normalEnabled->isChecked()); //启用正常加点
	iniFile.setValue("autoAddPointBurstEnabled", ui.checkBox_burstEnabled->isChecked());   //启用爆点加点
	iniFile.setValue("autoAddPointName", ui.AutoAddPoint_petName->text());				   //自动加点名字判断
	iniFile.setValue("autoAddPointRealName", ui.AutoAddPoint_petRealName->text());		   //自动加点名字判断
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg)
	{
		iniFile.setValue("autoAddPointNormalType", petCfg->normalCfg.addBpType);  //正常加点
		iniFile.setValue("autoAddPointBurstType", petCfg->unnormalCfg.addBpType); //爆点加点
	}
	iniFile.endGroup();
}

void GamePetWgt::init()
{
	ui.comboBox_SummonPet->clear();
	ui.comboBox_SummonPet->addItem("最高等级", TChangePet_HighestLv);
	ui.comboBox_SummonPet->addItem("血最多", TChangePet_HighestHp);
	ui.comboBox_SummonPet->addItem("魔最多", TChangePet_HighestMp);

	ui.comboBox_SummonPet->addItem("第1只", TChangePet_SpecialIndex0);
	ui.comboBox_SummonPet->addItem("第2只", TChangePet_SpecialIndex1);
	ui.comboBox_SummonPet->addItem("第3只", TChangePet_SpecialIndex2);
	ui.comboBox_SummonPet->addItem("第4只", TChangePet_SpecialIndex3);
	ui.comboBox_SummonPet->addItem("第5只", TChangePet_SpecialIndex4);

	m_pDropCheckBoxMap.insert(TDropPetType_PLevel, ui.checkBox_DropLv);
	m_pDropCheckBoxMap.insert(TDropPetType_PHp, ui.checkBox_DropHp);
	m_pDropCheckBoxMap.insert(TDropPetType_PMp, ui.checkBox_DropMp);
	m_pDropCheckBoxMap.insert(TDropPetType_PVigor, ui.checkBox_DropVigor);
	m_pDropCheckBoxMap.insert(TDropPetType_PStrength, ui.checkBox_DropStrength);
	m_pDropCheckBoxMap.insert(TDropPetType_PEndurance, ui.checkBox_DropEndurance);
	m_pDropCheckBoxMap.insert(TDropPetType_PAgility, ui.checkBox_DropAgility);
	m_pDropCheckBoxMap.insert(TDropPetType_PMagical, ui.checkBox_DropMagical);
	m_pDropCheckBoxMap.insert(TDropPetType_VAttack, ui.checkBox_attack);
	m_pDropCheckBoxMap.insert(TDropPetType_VDefensive, ui.checkBox_defensive);
	m_pDropCheckBoxMap.insert(TDropPetType_VAgility, ui.checkBox_agility);
	m_pDropCheckBoxMap.insert(TDropPetType_VSpirit, ui.checkBox_spirit);
	m_pDropCheckBoxMap.insert(TDropPetType_VRecovery, ui.checkBox_recovery);
	m_pDropCheckBoxMap.insert(TDropPetType_Grade, ui.checkBox_grade);
	m_pDropCheckBoxMap.insert(TDropPetType_RealName, ui.checkBox_realName);

	m_pDropLineEditMap.insert(TDropPetType_PLevel, ui.lineEdit_DropLv);
	m_pDropLineEditMap.insert(TDropPetType_PHp, ui.lineEdit_DropHp);
	m_pDropLineEditMap.insert(TDropPetType_PMp, ui.lineEdit_DropMp);
	m_pDropLineEditMap.insert(TDropPetType_PVigor, ui.lineEdit_DropVigor);
	m_pDropLineEditMap.insert(TDropPetType_PStrength, ui.lineEdit_DropStrength);
	m_pDropLineEditMap.insert(TDropPetType_PEndurance, ui.lineEdit_DropDefense);
	m_pDropLineEditMap.insert(TDropPetType_PAgility, ui.lineEdit_DropAgility);
	m_pDropLineEditMap.insert(TDropPetType_PMagical, ui.lineEdit_DropMagical);
	m_pDropLineEditMap.insert(TDropPetType_VAttack, ui.lineEdit_attack);
	m_pDropLineEditMap.insert(TDropPetType_VDefensive, ui.lineEdit_defensive);
	m_pDropLineEditMap.insert(TDropPetType_VAgility, ui.lineEdit_agility);
	m_pDropLineEditMap.insert(TDropPetType_VSpirit, ui.lineEdit_spirit);
	m_pDropLineEditMap.insert(TDropPetType_VRecovery, ui.lineEdit_recovery);
	m_pDropLineEditMap.insert(TDropPetType_Grade, ui.lineEdit_grade);
	m_pDropLineEditMap.insert(TDropPetType_RealName, ui.lineEdit_realName);
	m_petState.insert(TPET_STATE_READY, "待命");
	m_petState.insert(TPET_STATE_BATTLE, "战斗");
	m_petState.insert(TPET_STATE_REST, "休息");
	m_petState.insert(TPET_STATE_WALK, "散步");
	for (auto it = m_pDropCheckBoxMap.begin(); it != m_pDropCheckBoxMap.end(); ++it)
	{
		connect(it.value(), SIGNAL(stateChanged(int)), this, SLOT(on_dropPet_CheckBox_stateChanged(int)));
	}
	for (auto it = m_pDropLineEditMap.begin(); it != m_pDropLineEditMap.end(); ++it)
	{
		connect(it.value(), SIGNAL(editingFinished()), this, SLOT(on_dropPet_LineEdit_editingFinished()));
	}
	//读取默认值 触发
	on_checkBox_RecallLoyalty_stateChanged(ui.checkBox_RecallLoyalty->checkState());
	on_lineEdit_RecallLoyalty_editingFinished();
}

void GamePetWgt::OnNotifyGetPetsInfo(GamePetList pets)
{
	if (!this->isVisible())
		return;
	ui.tableWidget->setUpdatesEnabled(false);
	GamePetList gamePets = pets;
	QMap<int, GamePetPtr> posMap;
	for (auto tmpPet:gamePets)
	{
		posMap.insert(tmpPet->index, tmpPet);
	}
	for (size_t i = 0; i < 5; i++)
	{
		GamePetPtr pPet = nullptr;
		int pos = i;		
		pPet = posMap.value(pos);		
		QString name;
		QString szHp;
		QString szMp;
		QString sLv;
		QString sXp;
		QString sDangCi = "--";
		QString sAttac, sDefensive, sAgility, sSpirit, sLoyality, sRecovery;
		QString state;
		QColor gradeColor("black");
		QColor backColor("white");
		if (pPet && pPet->exist)
		{
			name = pPet->name;
			if (name.isEmpty())
			{
				name = pPet->showname;
			}		
			pos = pPet->index;
			if (pos <0 ||pos >=5)
			{
				pos = i;
			}
			szHp = QString("%1/%2").arg(pPet->hp).arg(pPet->maxhp);
			szMp = QString("%1/%2").arg(pPet->mp).arg(pPet->maxmp);
			sLv = QString::number(pPet->level);
			sXp = QString("%1").arg(pPet->maxxp - pPet->xp);
			sAttac = QString("%1").arg(pPet->detail.value_attack);
			sDefensive = QString("%1").arg(pPet->detail.value_defensive);
			sAgility = QString("%1").arg(pPet->detail.value_agility);
			sSpirit = QString("%1").arg(pPet->detail.value_spirit);
			sRecovery = QString("%1").arg(pPet->detail.value_recovery);
			sLoyality = QString("%1").arg(pPet->loyality);
			if (pPet->level == 1 && pPet->bCalcGrade)
			{
				QStringList inputData = g_pGameFun->GetPetCalcBpData(pPet);
				//qDebug() << inputData;
				auto pCalcData = g_pGamePetCalc->ParseLine(inputData);
				if (pCalcData)
				{

					pPet->grade = pCalcData->lossMin;		 //最少掉档
					pPet->lossMinGrade = pCalcData->lossMin; //最少掉档
					pPet->lossMaxGrade = pCalcData->lossMax; //最多掉档
				}
				pPet->bCalcGrade = false; //不再次计算了
			}
			if (pPet->grade >= 0)
			{
				if (pPet->lossMinGrade != pPet->lossMaxGrade && pPet->lossMaxGrade != -1)
					sDangCi = QString("%1~%2").arg(pPet->lossMinGrade).arg(pPet->lossMaxGrade);
				else
					sDangCi = QString("%1").arg(pPet->grade);
				if (pPet->grade <= 3)
				{
					gradeColor = QColor(255, 0, 0); //QColor(255, 215, 0); //金色
				}
				else if (pPet->grade > 3 && pPet->grade <= 6)
				{
					gradeColor = QColor("#800080"); //紫色
				}
				else if (pPet->grade > 6 && pPet->grade <= 9)
				{
					gradeColor = QColor("blue"); //蓝色
				}
				else if (pPet->grade > 9 && pPet->grade <= 12)
				{
					gradeColor = QColor("green"); //绿色
				}
			}
			state = m_petState.value(pPet->battle_flags);
		}
		setItemText(0, pos, name, QColor("blue"));
		setItemText(1, pos, szHp, QColor("red"));
		setItemText(2, pos, szMp, QColor("blue"));
		setItemText(3, pos, sLv, QColor("blue"));
		setItemText(4, pos, sXp, QColor("green"));
		setItemText(5, pos, sDangCi, gradeColor,QColor(240,255,255));
		//setItemText(5, pos, sDangCi, gradeColor,QColor(255,255,240));
		setItemText(6, pos, sAttac);
		setItemText(7, pos, sDefensive);
		setItemText(8, pos, sAgility);
		setItemText(9, pos, sSpirit);
		setItemText(10, pos, sRecovery);
		setItemText(11, pos, sLoyality);
		setItemText(12, pos, state);
	}
	ui.tableWidget->setUpdatesEnabled(true);
	//ui.tableWidget->resizeColumnsToContents(); //根据内容调整列宽 但每次都变 太麻烦 修改下
}

void GamePetWgt::on_checkBox_SummonPet_stateChanged(int state)
{
	g_pGameCtrl->getGameBattlePetCfg()->bSummonPet = (state == Qt::Checked ? true : false);
}

void GamePetWgt::on_checkBox_RecallLoyalty_stateChanged(int state)
{
	g_pGameCtrl->getGameBattlePetCfg()->bRecallLoyality = (state == Qt::Checked ? true : false);
}

void GamePetWgt::on_checkBox_RecallMp_stateChanged(int state)
{
	g_pGameCtrl->getGameBattlePetCfg()->bRecallMp = (state == Qt::Checked ? true : false);
}

void GamePetWgt::on_lineEdit_RecallLoyalty_editingFinished()
{
	int nVal = ui.lineEdit_RecallLoyalty->text().toInt();
	g_pGameCtrl->getGameBattlePetCfg()->nRecallLoyality = nVal;
}

void GamePetWgt::on_lineEdit_RecallMp_editingFinished()
{
	int nVal = ui.lineEdit_RecallMp->text().toInt();
	g_pGameCtrl->getGameBattlePetCfg()->nRecallMp = nVal;
}

void GamePetWgt::on_comboBox_SummonPet_currentIndexChanged(int index)
{
	int nType = ui.comboBox_SummonPet->currentData().toInt();
	g_pGameCtrl->getGameBattlePetCfg()->nSummonPetType = nType;
}

void GamePetWgt::on_groupBox_DropPet_toggled(bool checked)
{
	g_pGameCtrl->OnSetAutoDropPet(checked);
}

void GamePetWgt::on_dropPet_CheckBox_stateChanged(int state)
{
	QObject *obj = sender();
	g_pGameCtrl->SetDropPetItemChecked(m_pDropCheckBoxMap.key(obj), state);
}

void GamePetWgt::on_dropPet_LineEdit_editingFinished()
{
	QObject *obj = sender();
	QLineEdit *pObj = (QLineEdit *)obj;
	g_pGameCtrl->SetDropPetItemValue(m_pDropLineEditMap.key(obj), pObj->text());
}

void GamePetWgt::on_pushButton_saveBook_clicked()
{
	QString path = QString("%1\\图鉴\\%2.html").arg(QApplication::applicationDirPath()).arg(g_pGameFun->GetGameCharacter()->name);

	g_pGameFun->SavePetPictorialBookToHtml(path);
	//是否打开对话框
	if (QMessageBox::information(this, "提示", "保存完成,程序所在图鉴文件夹下,是否打开图鉴信息！", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(path));
	}
}

void GamePetWgt::onTableCustomContextMenu(const QPoint &pos)
{
	m_pCurItem = ui.tableWidget->itemAt(pos);
	if (m_pCurItem != NULL)
	{
		int nrow = m_pCurItem->row();
		int ncol = m_pCurItem->column();
		if (nrow != 0 && nrow != 12) //名字和状态可以改
			return;

		QMenu menu;
		if (nrow == 0)
		{
			menu.addAction("修改名称", this, SLOT(doChangePetName()));
			menu.addAction("修改为名称档次", this, SLOT(doChangePetNameGrade()));
		}
		else if (nrow == 12)
			menu.addAction("修改状态", this, SLOT(doChangePetState()));
		menu.exec(QCursor::pos());
	}
}

void GamePetWgt::doChangePetName()
{
	if (m_pCurItem == nullptr)
		return;
	int nCol = m_pCurItem->column();
	UserDefDialog dlg(this);
	dlg.setWindowTitle("修改宠物名称");
	dlg.setLabelText("名称：");
	if (dlg.exec() == QDialog::Accepted)
	{
		QString sName = dlg.getVal();
		bool bRet = false;
		g_CGAInterface->ChangePetName(nCol, sName.toStdString(), bRet);
	}
}

void GamePetWgt::doChangePetNameGrade()
{
	if (m_pCurItem == nullptr)
		return;
	int nCol = m_pCurItem->column();
	CGA::cga_pet_info_t info;
	g_CGAInterface->GetPetInfo(nCol, info);
	if (info.level != 1)
	{
		qDebug() << "暂时只支持1级宠!";
		return;
	}
	QString realName = QString::fromStdString(info.realname);
	if (realName.size() > 4)
	{
		realName = realName.left(4);
	}
	auto pTableItem = ui.tableWidget->item(5, nCol);
	if (pTableItem)
	{
		realName += pTableItem->text();
	}
	bool bRet = false;
	g_CGAInterface->ChangePetName(nCol, realName.toStdString(), bRet);
}

void GamePetWgt::doChangePetState()
{
	if (m_pCurItem == nullptr)
		return;
	int nCol = m_pCurItem->column();

	QMap<QString, int> mapText;
	mapText.insert("无", TPET_STATE_NONE);
	mapText.insert("战斗", TPET_STATE_BATTLE);
	mapText.insert("待命", TPET_STATE_READY);
	mapText.insert("休息", TPET_STATE_REST);
	mapText.insert("散步", TPET_STATE_WALK);
	UserDefComboBoxDlg dlg(this);
	dlg.setWindowTitle("修改宠物名称");
	dlg.setLabelText("名称：");
	dlg.setComboBoxItems(mapText.keys());
	if (dlg.exec() == QDialog::Accepted)
	{
		QString sState = dlg.getVal();
		int tState = mapText.value(sState);
		g_pGameFun->RestPetState(nCol, tState);
	}
}

void GamePetWgt::on_groupBox_autoAddPoint_toggled(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg)
	{
		petCfg->bEnabled = checked;
	}
}

void GamePetWgt::on_AutoAddPoint_petName_editingFinished()
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg)
	{
		petCfg->petName = ui.AutoAddPoint_petName->text();
	}
}

void GamePetWgt::on_AutoAddPoint_petRealName_editingFinished()
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg)
	{
		petCfg->petRealName = ui.AutoAddPoint_petRealName->text();
	}
}

void GamePetWgt::on_checkBox_normalEnabled_stateChanged(int state)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg)
	{
		petCfg->normalCfg.bEnabled = (state == Qt::Checked ? true : false);
	}
}

void GamePetWgt::on_checkBox_burstEnabled_stateChanged(int state)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg)
	{
		petCfg->unnormalCfg.bEnabled = (state == Qt::Checked ? true : false);
	}
}
//
//void GamePetWgt::on_AutoAddPoint_nomal_toggled(bool checked)
//{
//	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
//	if (petCfg)
//	{
//		petCfg->normalCfg.bEnabled = checked;
//	}
//}
//
//void GamePetWgt::on_AutoAddPoint_burst_toggled(bool checked)
//{
//	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
//	if (petCfg)
//	{
//		petCfg->unnormalCfg.bEnabled = checked;
//	}
//}

void GamePetWgt::on_normalAddPointHp_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->normalCfg.addBpType = TPET_POINT_TYPE_Vigor;
	}
}

void GamePetWgt::on_normalAddPointAttack_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->normalCfg.addBpType = TPET_POINT_TYPE_Strength;
	}
}

void GamePetWgt::on_normalAddPointDefence_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->normalCfg.addBpType = TPET_POINT_TYPE_Endurance;
	}
}

void GamePetWgt::on_normalAddPointAgility_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->normalCfg.addBpType = TPET_POINT_TYPE_Agility;
	}
}

void GamePetWgt::on_normalAddPointMp_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->normalCfg.addBpType = TPET_POINT_TYPE_Magical;
	}
}

void GamePetWgt::on_burstAddPointHp_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->unnormalCfg.addBpType = TPET_POINT_TYPE_Vigor;
	}
}

void GamePetWgt::on_burstAddPointAttack_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->unnormalCfg.addBpType = TPET_POINT_TYPE_Strength;
	}
}

void GamePetWgt::on_burstAddPointDefence_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->unnormalCfg.addBpType = TPET_POINT_TYPE_Endurance;
	}
}

void GamePetWgt::on_burstAddPointAgility_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->unnormalCfg.addBpType = TPET_POINT_TYPE_Agility;
	}
}

void GamePetWgt::on_burstAddPointMp_clicked(bool checked)
{
	auto petCfg = g_pGameCtrl->GetUpgradePetCfg();
	if (petCfg && checked)
	{
		petCfg->unnormalCfg.addBpType = TPET_POINT_TYPE_Magical;
	}
}

void GamePetWgt::on_pushButton_reloadCalc_clicked()
{
	ITObjectDataMgr::getInstance().ReloadCalcPetData();
}

void GamePetWgt::setItemText(int row, int col, const QString &szText, const QColor &szColor, const QColor &backColor)
{
	/*QAbstractItemModel* tmpModel =  ui.tableWidget->model();
	if (tmpModel)
	{
		QModelIndex index = tmpModel->index(row, col);
		if (index.isValid())
		{
			tmpModel->setData(index, szText, Qt::DisplayRole);
			tmpModel->setData(index, szColor, Qt::ForegroundRole);
		}
	}*/
	QTableWidgetItem *pItem = ui.tableWidget->item(row, col);
	if (pItem && pItem->text() != szText)
	{
		pItem->setText(szText);
		pItem->setTextColor(szColor);
		pItem->setBackgroundColor(backColor);
	}
}