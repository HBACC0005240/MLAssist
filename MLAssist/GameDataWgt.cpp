#include "GameDataWgt.h"
#include "../include/ITPublic.h"
#include "GameCtrl.h"
#include "GameData.h"
#include "YunLai.h"
#include <QDebug>
#include <QLabel>
#include <QTableWidgetItem>
GameDataWgt::GameDataWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	m_lastUpdatePlayerTime.restart();
	m_lastUpdatePlayerSkillTime.restart();
	m_lastUpdatePlayerPetTime.restart();
	connect(g_pGameCtrl, &GameCtrl::NotifyGameCharacterInfo, this, &GameDataWgt::OnNotifyGameCharacterInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGameSkillsInfo, this, &GameDataWgt::OnNotifyGetSkillsInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapInfo, this, &GameDataWgt::doUpdateMapData, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_updateBattleHpMp, this, &GameDataWgt::doUpdateBattleHpMp, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGamePetsInfo, this, &GameDataWgt::OnNotifyGetPetsInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, SIGNAL(signal_clearUiInfo()), this, SLOT(doClearUiInfo()));
	connect(g_pGameCtrl, SIGNAL(signal_attachGame()), this, SLOT(doUpdateAttachInfo()));

	ui.tableWidget->setRowCount(16);
	for (int i = 0; i < 16; ++i)
	{
		for (size_t n = 0; n < 4; n++)
		{
			QTableWidgetItem *pItem = new QTableWidgetItem();
			ui.tableWidget->setItem(i, n, pItem);
		}
	}
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	ui.tableWidget->horizontalHeader()->setStyleSheet("font:bold;");
	//	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
	//	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	//ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->horizontalHeader()->setFixedHeight(20);
	//ui.tableWidget->setColumnWidth(1, 100);
	//ui.tableWidget->verticalHeader()->setDefaultSectionSize(15);
	//ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//ui.tableWidget->resizeColumnsToContents();//根据内容调整列宽 但每次都变 太麻烦 修改下
	auto pTabBar = ui.tabWidget->tabBar();
	m_pRealUpdateCheckBox = new QCheckBox("玩家实时刷新", this);
	connect(m_pRealUpdateCheckBox, SIGNAL(stateChanged(int)), g_pGameCtrl, SLOT(OnSetRealUpdatePlayerUi(int)));

	//m_pRealUpdateCheckBox->setMaximumWidth(40);
	QWidget *pWidget = new QWidget(this);
	QHBoxLayout *pHLayout = new QHBoxLayout(pWidget);
	pHLayout->addWidget(m_pRealUpdateCheckBox);
	pHLayout->setContentsMargins(0, 0, 0, 0);
	ui.tabWidget->setCornerWidget(pWidget);
}

GameDataWgt::~GameDataWgt()
{
}

void GameDataWgt::Active()
{
	QString szLoginUser = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "00E39EE4", 100);
	QString szGameUserName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "00E35878", 100)));
	QString szMapName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "009181C0", 100)));

	int nEast = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "0092BD28");
	int nSouth = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "0092BD30");
	setItemText(0, 0, szLoginUser);
	setItemText(1, 0, szGameUserName);
	setItemText(2, 0, szMapName);
	setItemText(3, 0, QString("东%1 南%2").arg(nEast).arg(nSouth));
	setItemText(4, 0, QString("连接有效"));
	int nHP = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A418");
	int nTotHP = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A41C");
	int nMP = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A420");
	int nTotMP = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A424");
	QString szHp = QString("HP:%1/%2").arg(nHP).arg(nTotHP);
	QString szMp = QString("MP:%1/%2").arg(nMP).arg(nTotMP);
	qDebug() << szHp << szMp;
	setItemText(0, 1, szHp);
	setItemText(1, 1, szMp);
	//	qDebug() << m_gameProcessID << m_gameHwnd << szEast;
	//获取升级经验
	int nExperience = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A438");
	int nNextExperience = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A43C");
	setItemText(2, 1, QString("升:%1").arg(nNextExperience - nExperience));
	int nAttack = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A460");
	setItemText(3, 1, QString("力:%1").arg(nAttack));
	int nDefence = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A464");
	setItemText(4, 1, QString("防:%1").arg(nDefence));
	int nAgility = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A468");
	setItemText(5, 1, QString("敏:%1").arg(nAgility));
	int nMind = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A46C");
	setItemText(6, 1, QString("精:%1").arg(nMind));
	int nCharm = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A474");
	setItemText(7, 1, QString("魅:%1").arg(nCharm));

	int nEarth = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4A4");
	int nWater = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4A8");
	int nFire = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4AC");
	int nWind = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4B0");
	QString szAttribute = "属:";
	if (nEarth > 0)
		szAttribute += QString(" 地%1").arg(nEarth);
	if (nWater > 0)
		szAttribute += QString(" 水%1").arg(nWater);
	if (nFire > 0)
		szAttribute += QString(" 火%1").arg(nFire);
	if (nWind > 0)
		szAttribute += QString(" 风%1").arg(nWind);
	setItemText(8, 1, szAttribute);
	int nGold = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4BC");
	setItemText(9, 1, QString("钱:%1").arg(nGold));

	QMap<int, QString> indexForName;
	for (int i = 0; i < 10; ++i) //10个技能栏
	{
		DWORD pAddress = 0x00D84FEC;
		DWORD offset = i * 0x49FC;
		pAddress += offset;
		QString skillName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 100)));
		pAddress += 0x38;
		int nShowIndex = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pAddress);
		indexForName.insert(nShowIndex, skillName);
	}
	for (auto it = indexForName.begin(); it != indexForName.end(); ++it)
	{
		setItemText(it.key() - 1, 2, it.value());
	}
}

void GameDataWgt::on_AutoAddPoint_name_editingFinished()
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		pCfg->playerName = ui.AutoAddPoint_name->text();
	}
}

void GameDataWgt::on_normalAddPointHp_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->normalCfg.bEnabled = true;
		pCfg->normalCfg.bVigor = bChcked;
	}
}

void GameDataWgt::on_normalAddPointAttack_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->normalCfg.bEnabled = true;
		pCfg->normalCfg.bStrength = bChcked;
	}
}

void GameDataWgt::on_normalAddPointDefence_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->normalCfg.bEnabled = true;
		pCfg->normalCfg.bEndurance = bChcked;
	}
}

void GameDataWgt::on_normalAddPointAgility_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->normalCfg.bEnabled = true;
		pCfg->normalCfg.bAgility = bChcked;
	}
}

void GameDataWgt::on_normalAddPointMp_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->normalCfg.bEnabled = true;
		pCfg->normalCfg.bMagical = bChcked;
	}
}

void GameDataWgt::on_specialAddPointHp_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->specialCfg.bEnabled = true;
		pCfg->specialCfg.bVigor = bChcked;
	}
}

void GameDataWgt::on_specialAddPointAttack_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->specialCfg.bEnabled = true;
		pCfg->specialCfg.bStrength = bChcked;
	}
}

void GameDataWgt::on_specialAddPointDefence_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->specialCfg.bEnabled = true;
		pCfg->specialCfg.bEndurance = bChcked;
	}
}

void GameDataWgt::on_specialAddPointAgility_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->specialCfg.bEnabled = true;
		pCfg->specialCfg.bAgility = bChcked;
	}
}

void GameDataWgt::on_specialAddPointMp_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		bool bChcked = (state == Qt::Checked ? true : false);
		if (bChcked)
			pCfg->specialCfg.bEnabled = true;

		pCfg->specialCfg.bMagical = bChcked;
	}
}

void GameDataWgt::on_lineEdit_specialHp_editingFinished()
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		pCfg->specialCfg.vigorVal = ui.lineEdit_specialHp->text().toInt();
	}
}

void GameDataWgt::on_lineEdit_Attack_editingFinished()
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		pCfg->specialCfg.strengthVal = ui.lineEdit_Attack->text().toInt();
	}
}

void GameDataWgt::on_lineEdit_defence_editingFinished()
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		pCfg->specialCfg.enduranceVal = ui.lineEdit_defence->text().toInt();
	}
}

void GameDataWgt::on_lineEdit_agility_editingFinished()
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		pCfg->specialCfg.agilityVal = ui.lineEdit_agility->text().toInt();
	}
}

void GameDataWgt::on_lineEdit_specialMp_editingFinished()
{
	auto pCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (pCfg)
	{
		pCfg->specialCfg.magicalVal = ui.lineEdit_specialMp->text().toInt();
	}
}

void GameDataWgt::doLoadUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("PlayerUi");
	ui.AutoAddPoint_name->setText(iniFile.value("autoAddPointName").toString());

	ui.normalAddPointHp->setChecked(iniFile.value("autoAddPointNormalVigor").toBool());
	ui.normalAddPointAttack->setChecked(iniFile.value("autoAddPointNormalStrength").toBool());
	ui.normalAddPointDefence->setChecked(iniFile.value("autoAddPointNormalEndurance").toBool());
	ui.normalAddPointAgility->setChecked(iniFile.value("autoAddPointNormalAgility").toBool());
	ui.normalAddPointMp->setChecked(iniFile.value("autoAddPointNormalMagical").toBool());

	ui.specialAddPointHp->setChecked(iniFile.value("autoAddPointSpecialVigor").toBool());
	ui.specialAddPointAttack->setChecked(iniFile.value("autoAddPointSpecialStrength").toBool());
	ui.specialAddPointDefence->setChecked(iniFile.value("autoAddPointSpecialEndurance").toBool());
	ui.specialAddPointAgility->setChecked(iniFile.value("autoAddPointSpecialAgility").toBool());
	ui.specialAddPointMp->setChecked(iniFile.value("autoAddPointSpecialMagical").toBool());

	ui.lineEdit_specialHp->setText(iniFile.value("autoAddPointSpecialVigorVal").toString());
	ui.lineEdit_Attack->setText(iniFile.value("autoAddPointSpecialStrengthVal").toString());
	ui.lineEdit_defence->setText(iniFile.value("autoAddPointSpecialEnduranceVal").toString());
	ui.lineEdit_agility->setText(iniFile.value("autoAddPointSpecialAgilityVal").toString());
	ui.lineEdit_specialMp->setText(iniFile.value("autoAddPointSpecialMagicalVal").toString());

	auto playerCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (playerCfg)
	{
		playerCfg->playerName = iniFile.value("autoAddPointName").toString();
		playerCfg->specialCfg.vigorVal = iniFile.value("autoAddPointSpecialVigorVal").toInt();
		playerCfg->specialCfg.strengthVal = iniFile.value("autoAddPointSpecialStrengthVal").toInt();
		playerCfg->specialCfg.enduranceVal = iniFile.value("autoAddPointSpecialEnduranceVal").toInt();
		playerCfg->specialCfg.agilityVal = iniFile.value("autoAddPointSpecialAgilityVal").toInt();
		playerCfg->specialCfg.magicalVal = iniFile.value("autoAddPointSpecialMagicalVal").toInt();
	}

	iniFile.endGroup();
}

void GameDataWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("PlayerUi");

	auto playerCfg = g_pGameCtrl->GetUpgradePlayerCfg();
	if (playerCfg)
	{
		iniFile.setValue("autoAddPointNormalEnabled", playerCfg->normalCfg.bEnabled);	//启用正常加点
		iniFile.setValue("autoAddPointSpecialEnabled", playerCfg->specialCfg.bEnabled); //启用指定加点
		iniFile.setValue("autoAddPointName", ui.AutoAddPoint_name->text());				//自动加点名字判断

		iniFile.setValue("autoAddPointNormalVigor", playerCfg->normalCfg.bVigor);
		iniFile.setValue("autoAddPointNormalStrength", playerCfg->normalCfg.bStrength);
		iniFile.setValue("autoAddPointNormalEndurance", playerCfg->normalCfg.bEndurance);
		iniFile.setValue("autoAddPointNormalAgility", playerCfg->normalCfg.bAgility);
		iniFile.setValue("autoAddPointNormalMagical", playerCfg->normalCfg.bMagical);

		iniFile.setValue("autoAddPointSpecialVigor", playerCfg->specialCfg.bVigor);
		iniFile.setValue("autoAddPointSpecialStrength", playerCfg->specialCfg.bStrength);
		iniFile.setValue("autoAddPointSpecialEndurance", playerCfg->specialCfg.bEndurance);
		iniFile.setValue("autoAddPointSpecialAgility", playerCfg->specialCfg.bAgility);
		iniFile.setValue("autoAddPointSpecialMagical", playerCfg->specialCfg.bMagical);

		iniFile.setValue("autoAddPointSpecialVigorVal", playerCfg->specialCfg.vigorVal);
		iniFile.setValue("autoAddPointSpecialStrengthVal", playerCfg->specialCfg.strengthVal);
		iniFile.setValue("autoAddPointSpecialEnduranceVal", playerCfg->specialCfg.enduranceVal);
		iniFile.setValue("autoAddPointSpecialAgilityVal", playerCfg->specialCfg.agilityVal);
		iniFile.setValue("autoAddPointSpecialMagicalVal", playerCfg->specialCfg.magicalVal);
	}
	iniFile.endGroup();
}

void GameDataWgt::doUpdateAttachInfo()
{
	setItemText(12, 0, QString("进程:%1  端口:%2").arg(g_pGameCtrl->getGamePID()).arg(g_pGameCtrl->GetGamePort()));
}

//这个不彻底清除，只显示连接断开
void GameDataWgt::doClearUiInfo()
{
	setItemText(4, 0, QString("连接断开"), QColor("red"));

	//for (int i = 0; i < 16; ++i)
	//{
	//	for (size_t n = 0; n < 4; n++)
	//	{
	//		QTableWidgetItem *pItem = new QTableWidgetItem();
	//		ui.tableWidget->setItem(i, n, pItem);
	//	}
	//}
}

//qfmoli.exe+A2A414  人物等级地址  可以此为基址 初始化结构体获取人物信息 人物信息后面跟着是人物物品信息类
void GameDataWgt::OnNotifyGameCharacterInfo(CharacterPtr char_info)
{
	if (!this->isVisible())
		return;
	if (!m_pRealUpdateCheckBox->isChecked())
	{
		if (m_lastUpdatePlayerTime.elapsed() < 5000)
			return;
		m_lastUpdatePlayerTime.restart();
	}

	ui.tableWidget->setUpdatesEnabled(false);
	auto gamePlayer = char_info;
	//0xE8FECA  E21C4A这两个应该是环境变量，周围有其他信息，不是太准
	//5CEE6C BDB488 这两个应该比较准 不知道哪个 先用1个BDB488附近有服务器id地址，并且和游戏名字地址接近 用这个
	//if (g_pGameCtrl->getGamePID() != 0 && g_pGameCtrl->getGameBaseAddr() != 0)
	//{
	//	QString szLoginUser = YunLai::ReadMemoryStrFromProcessID(g_pGameCtrl->getGamePID(), (ULONG_PTR)g_pGameCtrl->getGameBaseAddr() + 0xBDB488, 100); //
	//	g_pGameCtrl->SetGameGid(szLoginUser);
	//	setItemText(0, 0, szLoginUser, QColor("red"));
	//}
	setItemText(0, 0, gamePlayer->sGid, QColor("red"));
	setItemText(1, 0, gamePlayer->name, QColor("blue"));
	//地图更新 还是由另外一个通知 比这个快点
	//setItemText(2, 0, gamePlayer->mapName);
	//setItemText(3, 0, QString("东%1 南%2").arg(gamePlayer->x).arg(gamePlayer->y));
	if (g_pGameCtrl->getGameConnectState())
	{
		setItemText(4, 0, QString("连接有效"), QColor("#00ff00"));
	}
	else
	{
		setItemText(4, 0, QString("连接断开"), QColor("red"));
	}
	//setItemText(5, 0, QString("地图:%1").arg(gamePlayer->mapIndex));
	//setItemText(6, 0, g_pGameCtrl->getGameState());	//另外一个信号通知
	setItemText(7, 0, g_pGameFun->getSystemTime());
	setItemText(8, 0, QString("卡时:%1").arg(FormaClockIn(gamePlayer->punchclock)));
	QColor healthColor("green");
	if (gamePlayer->health > 25 && gamePlayer->health <= 50)
		healthColor = QColor("yellow");
	else if (gamePlayer->health > 50 && gamePlayer->health <= 75)
		healthColor = QColor("purple");
	else if (gamePlayer->health > 75 && gamePlayer->health <= 100)
		healthColor = QColor("red");
	if (gamePlayer->souls > 0)
		healthColor = QColor("red");
	setItemText(9, 0, QString("健康:%1 掉魂:%2").arg(gamePlayer->health).arg(gamePlayer->souls), healthColor);			//健康 图标后面补上
	setItemText(10, 0, QString("Lv:%1 %2").arg(gamePlayer->level).arg(gamePlayer->job));								//称号
	setItemText(11, 0, QString("Game:%1 World:%2").arg(g_pGameFun->GetGameStatus()).arg(g_pGameFun->GetWorldStatus())); //称号

	if (!g_pGameCtrl->IsInBattle())
	{
		QString szHp = QString("HP:%1/%2").arg(gamePlayer->hp).arg(gamePlayer->maxhp);
		QString szMp = QString("MP:%1/%2").arg(gamePlayer->mp).arg(gamePlayer->maxmp);

		setItemText(0, 1, szHp, QColor("red"));
		setItemText(1, 1, szMp, QColor("blue"));
	}

	//获取升级经验
	setItemText(2, 1, QString("升:%1").arg(gamePlayer->maxxp - gamePlayer->xp), QColor("green"));
	setItemText(3, 1, QString("力:%1").arg(gamePlayer->detail.value_attack), QColor(255, 128, 64));
	setItemText(4, 1, QString("防:%1").arg(gamePlayer->detail.value_defensive), QColor(0, 128, 128));
	setItemText(5, 1, QString("敏:%1").arg(gamePlayer->detail.value_agility), QColor(128, 0, 64));
	setItemText(6, 1, QString("精:%1").arg(gamePlayer->detail.value_spirit), QColor(0, 0, 128));
	setItemText(7, 1, QString("回:%1").arg(gamePlayer->detail.value_recovery), QColor(255, 0, 128));
	setItemText(8, 1, QString("魅:%1").arg(gamePlayer->value_charisma), QColor(255, 0, 128));

	int nEarth = gamePlayer->detail.element_earth;
	int nWater = gamePlayer->detail.element_water;
	int nFire = gamePlayer->detail.element_fire;
	int nWind = gamePlayer->detail.element_wind;
	QString szAttribute = "属:";
	if (nEarth > 0)
		szAttribute += QString(" 地%1").arg(nEarth);
	if (nWater > 0)
		szAttribute += QString(" 水%1").arg(nWater);
	if (nFire > 0)
		szAttribute += QString(" 火%1").arg(nFire);
	if (nWind > 0)
		szAttribute += QString(" 风%1").arg(nWind);
	setItemText(9, 1, szAttribute);
	setItemText(10, 1, QString("钱:%1").arg(gamePlayer->gold), QColor("#cd7f32"));
	setItemText(11, 1, gamePlayer->prestige, QColor(0, 0, 255)); //称号
	setItemText(12, 1, QString("银行:%1").arg(g_pGameFun->GetCharacterData("银行金币").toInt()), QColor("#cd7f32"));

	/*int default_petid = gamePlayer->default_petid;
	if (default_petid != -1)
	{	
		if (default_petid >= 0 )
		{
			GamePetPtr battlePet = g_pGameFun->GetBattlePet();				
			if (battlePet)
			{
				setItemText(0, 3, QString("HP:%1/%2").arg(battlePet->hp).arg(battlePet->maxhp), QColor("red"));
				setItemText(1, 3, QString("MP:%1/%2").arg(battlePet->mp).arg(battlePet->maxmp), QColor("blue"));
				setItemText(2, 3, QString("升:%1").arg(battlePet->maxxp - battlePet->xp), QColor("green"));
				setItemText(3, 3, QString("力:%1").arg(battlePet->detail.value_attack), QColor(255, 128, 64));
				setItemText(4, 3, QString("防:%1").arg(battlePet->detail.value_defensive), QColor(0, 128, 128));
				setItemText(5, 3, QString("敏:%1").arg(battlePet->detail.value_agility), QColor(128, 0, 64));
				setItemText(6, 3, QString("精:%1").arg(battlePet->detail.value_spirit), QColor(0, 0, 128));
				setItemText(7, 3, QString("忠:%1").arg(battlePet->loyality), QColor(255, 0, 128));
				int nEarth = battlePet->detail.element_earth;
				int nWater = battlePet->detail.element_water;
				int nFire = battlePet->detail.element_fire;
				int nWind = battlePet->detail.element_wind;
				QString szAttribute = "属:";
				if (nEarth > 0)
					szAttribute += QString(" 地%1").arg(nEarth);
				if (nWater > 0)
					szAttribute += QString(" 水%1").arg(nWater);
				if (nFire > 0)
					szAttribute += QString(" 火%1").arg(nFire);
				if (nWind > 0)
					szAttribute += QString(" 风%1").arg(nWind);

				setItemText(8, 3, szAttribute);
				QColor petHealthColor("green");
				if (battlePet->health > 25 && battlePet->health <= 50)
					petHealthColor = QColor("yellow");
				else if (battlePet->health > 50 && battlePet->health <= 75)
					petHealthColor = QColor("purple");
				else if (battlePet->health > 75 && battlePet->health <= 100)
					petHealthColor = QColor("red");

				if (battlePet->battle_flags)
					setItemText(9, 3, QString("战斗 健康:%1").arg(battlePet->health), petHealthColor);
				else
					setItemText(9, 3, QString("健康:%1").arg(battlePet->health), petHealthColor);
				setItemText(10, 3, QString("Lv:%1 %2").arg(battlePet->level).arg(battlePet->name.isEmpty() ? battlePet->realname : battlePet->name));
			}
		}
	}
	else
	{
		for (int i = 0; i < 11; ++i)
		{
			setItemText(i, 3, "");
		}
	}*/

	ui.tableWidget->setUpdatesEnabled(true);
	//ui.tableWidget->resizeColumnsToContents(); //根据内容调整列宽 但每次都变 太麻烦 修改下
}

void GameDataWgt::OnNotifyGetSkillsInfo(GameSkillList skills)
{
	if (!this->isVisible())
		return;
	if (!m_pRealUpdateCheckBox->isChecked())
	{
		if (m_lastUpdatePlayerSkillTime.elapsed() < 5000)
			return;
		m_lastUpdatePlayerSkillTime.restart();
	}
	GameSkillList gameSkills = skills;
	for (int i = 0; i < gameSkills.size(); ++i)
	{
		if (i > 15)
			break;
		GameSkillPtr pSkill = gameSkills[i];
		if (pSkill && pSkill->exist && !pSkill->name.isEmpty())
		{
			if (pSkill->maxxp == 99999999 || pSkill->xp >= 322200)
				setItemText(i, 2, QString("%1 Lv:%2 %3").arg(pSkill->name).arg(pSkill->level).arg("已满"));
			else
				setItemText(i, 2, QString("%1 Lv:%2 %3").arg(pSkill->name).arg(pSkill->level).arg(pSkill->maxxp - pSkill->xp));
		}
		else
			setItemText(i, 2, "");
	}
}

void GameDataWgt::OnNotifyGetPetsInfo(GamePetList pets)
{	
	if (!this->isVisible())
		return;
	if (!m_pRealUpdateCheckBox->isChecked())
	{
		if (m_lastUpdatePlayerPetTime.elapsed() < 5000)
			return;
		m_lastUpdatePlayerPetTime.restart();
	}
	GamePetPtr battlePet = nullptr;
	for (auto pet : pets)
	{
		if (pet && pet->exist && pet->battle_flags == TPET_STATE_BATTLE) //默认出战宠物
		{
			battlePet = pet;
			break;
		}
	}
	if (battlePet)
	{
		setItemText(0, 3, QString("HP:%1/%2").arg(battlePet->hp).arg(battlePet->maxhp), QColor("red"));
		setItemText(1, 3, QString("MP:%1/%2").arg(battlePet->mp).arg(battlePet->maxmp), QColor("blue"));
		setItemText(2, 3, QString("升:%1").arg(battlePet->maxxp - battlePet->xp), QColor("green"));
		setItemText(3, 3, QString("力:%1").arg(battlePet->detail.value_attack), QColor(255, 128, 64));
		setItemText(4, 3, QString("防:%1").arg(battlePet->detail.value_defensive), QColor(0, 128, 128));
		setItemText(5, 3, QString("敏:%1").arg(battlePet->detail.value_agility), QColor(128, 0, 64));
		setItemText(6, 3, QString("精:%1").arg(battlePet->detail.value_spirit), QColor(0, 0, 128));
		setItemText(7, 3, QString("忠:%1").arg(battlePet->loyality), QColor(255, 0, 128));
		int nEarth = battlePet->detail.element_earth;
		int nWater = battlePet->detail.element_water;
		int nFire = battlePet->detail.element_fire;
		int nWind = battlePet->detail.element_wind;
		QString szAttribute = "属:";
		if (nEarth > 0)
			szAttribute += QString(" 地%1").arg(nEarth);
		if (nWater > 0)
			szAttribute += QString(" 水%1").arg(nWater);
		if (nFire > 0)
			szAttribute += QString(" 火%1").arg(nFire);
		if (nWind > 0)
			szAttribute += QString(" 风%1").arg(nWind);

		setItemText(8, 3, szAttribute);
		QColor petHealthColor("green");
		if (battlePet->health > 25 && battlePet->health <= 50)
			petHealthColor = QColor("yellow");
		else if (battlePet->health > 50 && battlePet->health <= 75)
			petHealthColor = QColor("purple");
		else if (battlePet->health > 75 && battlePet->health <= 100)
			petHealthColor = QColor("red");

		if (battlePet->battle_flags)
			setItemText(9, 3, QString("战斗 健康:%1").arg(battlePet->health), petHealthColor);
		else
			setItemText(9, 3, QString("健康:%1").arg(battlePet->health), petHealthColor);
		setItemText(10, 3, QString("Lv:%1 %2").arg(battlePet->level).arg(battlePet->name.isEmpty() ? battlePet->realname : battlePet->name));
	}
	else
	{
		for (int i = 0; i < 11; ++i)
		{
			setItemText(i, 3, "");
		}
	}
}

//半秒更新一次坐标
void GameDataWgt::doUpdateMapData(QString name, int index1, int index2, int index3, int x, int y)
{
	if (!this->isVisible())
		return;
	setItemText(2, 0, name);
	setItemText(3, 0, QString("东%1 南%2").arg(x).arg(y));
	setItemText(5, 0, QString("%1").arg(index3));
	QString state = g_pGameCtrl->getGameState();
	setItemText(6, 0, state);
}

void GameDataWgt::doUpdateBattleHpMp(int hp, int maxhp, int mp, int maxmp)
{
	if (!this->isVisible())
		return;
	QString szHp = QString("HP:%1/%2").arg(hp).arg(maxhp);
	QString szMp = QString("MP:%1/%2").arg(mp).arg(maxmp);
	setItemText(0, 1, szHp, QColor("red"));
	setItemText(1, 1, szMp, QColor("blue"));
}

QString GameDataWgt::FormaClockIn(int val)
{
	val /= 1000;
	int hours = val / 3600;
	int mins = (val - hours * 3600) / 60;
	return QString("%1:%2").arg(hours).arg(mins, 2, 10, QLatin1Char('0'));
}
void GameDataWgt::setItemText(int row, int col, const QString &szText, const QColor &szColor)
{
	QTableWidgetItem *pItem = ui.tableWidget->item(row, col);
	if (pItem && pItem->text() != szText)
	{
		pItem->setText(szText);
		pItem->setTextColor(szColor);
	}
}

void GameDataWgt::refreshBattleUI()
{
	for (int i = 0; i < 20; i++)
	{
		Character info = m_Infos[i];
		QString szLabelName = QString("label_%1").arg(i);
		QLabel *pLabel = this->findChild<QLabel *>(szLabelName);
		if (pLabel)
		{
			WCHAR buf[MAXBUFLEN] = { 0 };
			if (info.name.isEmpty() == false) //
			{
				QString szShowText = QString("%1\n%2/%3\n%4").arg(info.name).arg(info.hp).arg(info.maxhp).arg(info.level);
				//			qDebug() << szShowText << info.name << info.hp << info.totalhp << info.level;
				pLabel->setText(szShowText);
			}
			else
			{
				pLabel->setText("");
			}
		}
	}
}
