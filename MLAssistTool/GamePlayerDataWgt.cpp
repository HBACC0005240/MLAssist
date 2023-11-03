#include "GamePlayerDataWgt.h"
#include "ITObjectDataMgr.h"
#include <QMenu>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include "AddAccountDlg.h"

GamePlayerDataWgt::GamePlayerDataWgt(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	init();

}

GamePlayerDataWgt::~GamePlayerDataWgt()
{
}

void GamePlayerDataWgt::init()
{
	m_pPlayerDataTreeModel = new PlayerDataTreeModel(this);
	auto pObjList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_ServerType, 0xffffff00);
	m_pPlayerDataTreeModel->SetupModelData(pObjList,m_pPlayerDataTreeModel->root());
	ui.treeView->setModel(NULL);
	ui.treeView->setModel(m_pPlayerDataTreeModel);
	ui.treeView->expandAll();
	ui.treeView->collapseAll();
	ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);      //TreeView启用右键菜单信号

	connect(ui.treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_treeView_customContextMenuRequested(const QPoint&)));
	connect(ui.treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(doTreeViewClicked(const QModelIndex&)));

	initTable(ui.tableWidget_item, 20);
	ui.tableWidget_item->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	initTable(ui.tableWidget_bankItem, 80);
	initTable(ui.tableWidget_pet, 60, 20);
	initTable(ui.tableWidget_bankPet, 60, 20);
	QStringList saveHeadList;
	saveHeadList << "宠物1" << "宠物2" << "宠物3" << "宠物4" << "宠物5";
	ui.tableWidget_pet->setColumnCount(saveHeadList.size());
	ui.tableWidget_pet->setHorizontalHeaderLabels(saveHeadList);
	ui.tableWidget_bankPet->setColumnCount(saveHeadList.size());
	ui.tableWidget_bankPet->setHorizontalHeaderLabels(saveHeadList);
	QStringList rowHeadList;
	rowHeadList << "名称" << "生命" << "魔力" << "等级" << "经验" << "档次"\
		<< "攻击" << "防御" << "敏捷" << "魔法" << "忠诚" << "状态";
	ui.tableWidget_pet->verticalHeader()->setVisible(true);
	ui.tableWidget_bankPet->verticalHeader()->setVisible(true);
	ui.tableWidget_pet->setVerticalHeaderLabels(rowHeadList);
	ui.tableWidget_bankPet->setVerticalHeaderLabels(rowHeadList);

	ui.tableWidget->setRowCount(16);
	for (int i = 0; i < 16; ++i)
	{
		for (size_t n = 0; n < 4; n++)
		{
			QTableWidgetItem* pItem = new QTableWidgetItem();
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
	m_petState.insert(TPET_STATE_READY, "待命");
	m_petState.insert(TPET_STATE_BATTLE, "战斗");
	m_petState.insert(TPET_STATE_REST, "休息");
	m_petState.insert(TPET_STATE_WALK, "散步");
}

void GamePlayerDataWgt::initTable(QTableWidget* pTable, int nCount/*=20*/, int height)
{
	pTable->horizontalHeader()->setStyleSheet("font:bold;");
	pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	pTable->verticalHeader()->setVisible(false);
	//	pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	pTable->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	pTable->horizontalHeader()->setStretchLastSection(true);
	//	pTable->horizontalHeader()->setFixedHeight(30);
	//pTable->setColumnWidth(1, 130);
	pTable->verticalHeader()->setDefaultSectionSize(15);
	//pTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	pTable->setContextMenuPolicy(Qt::CustomContextMenu); //打开右键功能
	int nRow = nCount / 5;
	for (int i = 0; i < nRow; ++i)
	{
		for (size_t n = 0; n < 5; n++)
		{
			QTableWidgetItem* pItem = new QTableWidgetItem();
			pTable->setItem(i, n, pItem);
		}
		pTable->setRowHeight(i, height);
	}
}

void GamePlayerDataWgt::resetModel(ITObjectList pObjList)
{
	if (m_pPlayerDataTreeModel)
	{
		delete m_pPlayerDataTreeModel;
		m_pPlayerDataTreeModel = nullptr;
	}
	m_pPlayerDataTreeModel = new PlayerDataTreeModel(this);
	m_pPlayerDataTreeModel->SetupModelData(pObjList,m_pPlayerDataTreeModel->root());
	ui.treeView->setModel(NULL);
	ui.treeView->setModel(m_pPlayerDataTreeModel);
	ui.treeView->expandAll();
	//ui.treeView->collapseAll();
	ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);      //TreeView启用右键菜单信号
}

QString GamePlayerDataWgt::DisplayInputDlg(const QString& title, const QString& labelName)
{

	QDialog dlg(this);
	dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	dlg.setWindowTitle(title);
	QVBoxLayout vLayout(&dlg);
	dlg.setMinimumWidth(100);
	dlg.setMinimumHeight(100);
	QHBoxLayout hLayout(&dlg);
	QLineEdit lineEdit(&dlg);
	QLabel label(&dlg);
	label.setText(labelName);
	//lineEdit.setText(strname);
	hLayout.addWidget(&label);
	hLayout.addWidget(&lineEdit);
	QPushButton btn1(&dlg), btn2(&dlg);
	btn1.setText("确定");
	btn2.setText("取消");
	QHBoxLayout hLayout2(&dlg);
	hLayout2.addStretch();
	hLayout2.addWidget(&btn1);
	hLayout2.addWidget(&btn2);
	hLayout2.addStretch();
	vLayout.addLayout(&hLayout);
	vLayout.addLayout(&hLayout2);
	connect(&btn1, SIGNAL(clicked()), &dlg, SLOT(accept()));
	connect(&btn2, SIGNAL(clicked()), &dlg, SLOT(reject()));
	if (dlg.exec() == QDialog::Accepted)
	{
		return lineEdit.text();
	}
	return "";

}

void GamePlayerDataWgt::on_treeView_customContextMenuRequested(const QPoint& pos)
{
	QModelIndex index = ui.treeView->indexAt(pos);                //当前所在行的index
	TreeItem* Item = static_cast<TreeItem*>(index.internalPointer());
	if (!Item)
	{
		QMenu menu(this);
		menu.setStyleSheet(QString("QMenu{background:rgb(0,95,91);border-radius:3px;color:rgb(255,255,255);}\
				QMenu::item{ background - color:transparent; }\
				QMenu::item:selected{ background - color:rgb(0,62,59); color:rgb(0,255,242); }"));
		menu.addAction(QString("全部展开"), [&]() {ui.treeView->expandAll(); });
		menu.addAction(QString("全部折叠"), [&]() {ui.treeView->collapseAll(); });
		menu.exec(QCursor::pos());
	}
	else
	{
		auto objid = Item->data(Qt::UserRole).toULongLong();
		m_curSelectObj = ITObjectDataMgr::getInstance().FindObject(objid);
		if (m_curSelectObj)
		{
			QMenu menu(this);
			menu.setStyleSheet(QString("QMenu{background:rgb(0,95,91);border-radius:3px;color:rgb(255,255,255);}\
				QMenu::item{ background - color:transparent; }\
				QMenu::item:selected{ background - color:rgb(0,62,59); color:rgb(0,255,242); }"));
			menu.addAction(QString("全部展开"), [&]() {ui.treeView->expandAll(); });
			menu.addAction(QString("全部折叠"), [&]() {ui.treeView->collapseAll(); });

			int devtype = m_curSelectObj->getObjectType();
			if (devtype == TObject_AccountIdentity)
			{				
				menu.addAction(QString("增加账号"), this, SLOT(doAddAccount()));
			}
			else if (devtype == TObject_Account)
			{				
				menu.addAction(QString("增加游戏ID"), this, SLOT(doAddAccountGid()));
			}			
			menu.exec(QCursor::pos());
		}
	}

}

void GamePlayerDataWgt::doAddAccountAssemble()
{
	QDialog dlg(this);
	dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	dlg.setWindowTitle("增加证件");
	QVBoxLayout vLayout(&dlg);
	dlg.setMinimumWidth(100);
	dlg.setMinimumHeight(100);
	QHBoxLayout hLayout(&dlg);
	QLineEdit lineEdit(&dlg);
	QLabel label(&dlg);
	label.setText("证件名称：");
	//lineEdit.setText(strname);
	hLayout.addWidget(&label);
	hLayout.addWidget(&lineEdit);
	QPushButton btn1(&dlg), btn2(&dlg);
	btn1.setText("确定");
	btn2.setText("取消");
	QHBoxLayout hLayout2(&dlg);
	hLayout2.addStretch();
	hLayout2.addWidget(&btn1);
	hLayout2.addWidget(&btn2);
	hLayout2.addStretch();
	vLayout.addLayout(&hLayout);
	vLayout.addLayout(&hLayout2);
	connect(&btn1, SIGNAL(clicked()), &dlg, SLOT(accept()));
	connect(&btn2, SIGNAL(clicked()), &dlg, SLOT(reject()));
	if (dlg.exec() == QDialog::Accepted)
	{
		QString strCurName = lineEdit.text();
		auto pAcctountAsse = ITObjectDataMgr::getInstance().newOneObject(TObject_AccountIdentity);
		if (pAcctountAsse)
		{
			pAcctountAsse->setObjectName(strCurName);
			auto pObjList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_AccountGid);
			resetModel(pObjList);
		}
	}
}

void GamePlayerDataWgt::doAddAccount()
{
	AddAccountDlg dlg;
	dlg.SetAccountAssemblePtr(qSharedPointerCast<ITAccountIdentity>(m_curSelectObj));
	if (dlg.exec() == QDialog::Accepted)
	{
		auto pObjList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_AccountGid);
		resetModel(pObjList);
	}
}

void GamePlayerDataWgt::doAddAccountGid()
{
	QString sText = DisplayInputDlg("增加游戏ID", "游戏ID");
	if (sText.isEmpty())
		return;
	auto pOwn = m_curSelectObj;
	if (m_curSelectObj && m_curSelectObj->getObjectType() == TObject_AccountGid)
	{
		pOwn = m_curSelectObj->getObjectParent();
	}
	auto pGid = ITObjectDataMgr::getInstance().newOneObject(TObject_AccountGid, pOwn);
	if (pGid)
	{
		pGid->setObjectName(sText);
		auto pObjList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_AccountGid);
		resetModel(pObjList);
	}
}
void GamePlayerDataWgt::setItemText(int row, int col, const QString& szText, const QColor& szColor)
{
	QTableWidgetItem* pItem = ui.tableWidget->item(row, col);
	if (pItem && pItem->text() != szText)
	{
		pItem->setText(szText);
		pItem->setTextColor(szColor);
	}
}

QString GamePlayerDataWgt::getGameState(ITGameCharacterPtr pRole)
{
	int gameStatus = pRole->_game_status;
	int worldStatus = pRole->_world_status;
	//if (g_CGAInterface->GetGameStatus(gameStatus) && g_CGAInterface->GetWorldStatus(worldStatus))
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
QString GamePlayerDataWgt::getSystemTime(ITGameCharacterPtr pRole)
{
	QStringList stages = { "黎明", "白天", "黄昏", "夜晚" };
	ITGameSysTime sysTime=pRole->_gameSysTime;
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
	//还是用真实时间
	return sDateTime + " " + timeStage;
}
QString GamePlayerDataWgt::FormaClockIn(int val)
{
	val /= 1000;
	int hours = val / 3600;
	int mins = (val - hours * 3600) / 60;
	return QString("%1:%2").arg(hours).arg(mins, 2, 10, QLatin1Char('0'));
}
void GamePlayerDataWgt::doUpdateCharacterTable(ITGameCharacterPtr pRole)
{
	ui.tableWidget->setUpdatesEnabled(false);
	auto gamePlayer = pRole;
	setItemText(0, 0, gamePlayer->_gid, QColor("red"));
	setItemText(1, 0, gamePlayer->getObjectName(), QColor("blue"));
	//地图更新 还是由另外一个通知 比这个快点
	setItemText(2, 0, gamePlayer->_map_name);
	setItemText(3, 0, QString("东%1 南%2").arg(gamePlayer->_x).arg(gamePlayer->_y));
	if (gamePlayer->_connectState)
	{
		setItemText(4, 0, QString("连接有效"), QColor("#00ff00"));
	}
	else
	{
		setItemText(4, 0, QString("连接断开"), QColor("red"));
	}
	setItemText(5, 0, QString("地图:%1").arg(gamePlayer->_map_number));
	setItemText(6, 0, getGameState(pRole)); //另外一个信号通知
	setItemText(7, 0, getSystemTime(pRole));
	setItemText(8, 0, QString("卡时:%1").arg(FormaClockIn(gamePlayer->_punchclock)));
	QColor healthColor("green");
	if (gamePlayer->_baseData->_health > 25 && gamePlayer->_baseData->_health <= 50)
		healthColor = QColor("yellow");
	else if (gamePlayer->_baseData->_health > 50 && gamePlayer->_baseData->_health <= 75)
		healthColor = QColor("purple");
	else if (gamePlayer->_baseData->_health > 75 && gamePlayer->_baseData->_health <= 100)
		healthColor = QColor("red");
	if (gamePlayer->_souls > 0)
		healthColor = QColor("red");
	setItemText(9, 0, QString("健康:%1 掉魂:%2").arg(gamePlayer->_baseData->_health).arg(gamePlayer->_souls), healthColor);			//健康 图标后面补上
	setItemText(10, 0, QString("Lv:%1 %2").arg(gamePlayer->_baseData->_level).arg(gamePlayer->_job));								//称号
	setItemText(11, 0, QString("Game:%1 World:%2").arg(pRole->_game_status).arg(pRole->_world_status)); //称号
	setItemText(12, 0, QString("进程:%1  端口:%2").arg(pRole->_game_pid).arg(pRole->_game_port));

	
	QString szHp = QString("HP:%1/%2").arg(gamePlayer->_baseData->_hp).arg(gamePlayer->_baseData->_maxhp);
	QString szMp = QString("MP:%1/%2").arg(gamePlayer->_baseData->_mp).arg(gamePlayer->_baseData->_maxmp);

	setItemText(0, 1, szHp, QColor("red"));
	setItemText(1, 1, szMp, QColor("blue"));
	

	//获取升级经验
	setItemText(2, 1, QString("升:%1").arg(gamePlayer->_baseData->_maxxp - gamePlayer->_baseData->_xp), QColor("green"));
	setItemText(3, 1, QString("力:%1").arg(gamePlayer->_attrData->_value_attack), QColor(255, 128, 64));
	setItemText(4, 1, QString("防:%1").arg(gamePlayer->_attrData->_value_defensive), QColor(0, 128, 128));
	setItemText(5, 1, QString("敏:%1").arg(gamePlayer->_attrData->_value_agility), QColor(128, 0, 64));
	setItemText(6, 1, QString("精:%1").arg(gamePlayer->_attrData->_value_spirit), QColor(0, 0, 128));
	setItemText(7, 1, QString("回:%1").arg(gamePlayer->_attrData->_value_recovery), QColor(255, 0, 128));
	setItemText(8, 1, QString("魅:%1").arg(gamePlayer->_value_charisma), QColor(255, 0, 128));

	int nEarth = gamePlayer->_attrData->_element_earth;
	int nWater = gamePlayer->_attrData->_element_water;
	int nFire = gamePlayer->_attrData->_element_fire;
	int nWind = gamePlayer->_attrData->_element_wind;
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
	setItemText(10, 1, QString("钱:%1").arg(gamePlayer->_gold), QColor("#cd7f32"));
	QString sPrestige;
	auto prestigeMap = ITObjectDataMgr::getInstance().GetPrestigeMap();	
	for (auto sTitle:pRole->_titles)
	{
		if (prestigeMap.contains(sTitle))
		{
			sPrestige = sTitle;
			break;
		}
	}
	setItemText(11, 1, sPrestige, QColor(0, 0, 255)); //称号
	setItemText(12, 1, QString("银行:%1").arg( gamePlayer->_bankgold), QColor("#cd7f32"));
		
	for (int i=0;i<15;++i)
	{
		setItemText(i, 2, "");
	}
	for (auto it = pRole->_skillPosForSkill.begin(); it!=pRole->_skillPosForSkill.end(); ++it)
	{
		int i = it.key();
		if (i > 15)
			break;
		ITGameSkillPtr pSkill = it.value();
		if (pSkill && pSkill->_bExist && !pSkill->getObjectName().isEmpty())
		{
			if (pSkill->_maxxp == 99999999 || pSkill->_xp >= 322200)
				setItemText(i, 2, QString("%1 Lv:%2 %3").arg(pSkill->getObjectName()).arg(pSkill->_level).arg("已满"));
			else
				setItemText(i, 2, QString("%1 Lv:%2 %3").arg(pSkill->getObjectName()).arg(pSkill->_level).arg(pSkill->_maxxp - pSkill->_xp));
		}
		else
			setItemText(i, 2, "");
	}

	ITGamePetPtr battlePet = pRole->_petPosForPet.value(pRole->_petid);
	if (battlePet)
	{
		setItemText(0, 3, QString("HP:%1/%2").arg(battlePet->_baseData->_hp).arg(battlePet->_baseData->_maxhp), QColor("red"));
		setItemText(1, 3, QString("MP:%1/%2").arg(battlePet->_baseData->_mp).arg(battlePet->_baseData->_maxmp), QColor("blue"));
		setItemText(2, 3, QString("升:%1").arg(battlePet->_baseData->_maxxp - battlePet->_baseData->_xp), QColor("green"));
		setItemText(3, 3, QString("力:%1").arg(battlePet->_attrData->_value_attack), QColor(255, 128, 64));
		setItemText(4, 3, QString("防:%1").arg(battlePet->_attrData->_value_defensive), QColor(0, 128, 128));
		setItemText(5, 3, QString("敏:%1").arg(battlePet->_attrData->_value_agility), QColor(128, 0, 64));
		setItemText(6, 3, QString("精:%1").arg(battlePet->_attrData->_value_spirit), QColor(0, 0, 128));
		setItemText(7, 3, QString("忠:%1").arg(battlePet->_loyality), QColor(255, 0, 128));
		int nEarth = battlePet->_attrData->_element_earth;
		int nWater = battlePet->_attrData->_element_water;
		int nFire = battlePet->_attrData->_element_fire;
		int nWind = battlePet->_attrData->_element_wind;
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
		if (battlePet->_baseData->_health > 25 && battlePet->_baseData->_health <= 50)
			petHealthColor = QColor("yellow");
		else if (battlePet->_baseData->_health > 50 && battlePet->_baseData->_health <= 75)
			petHealthColor = QColor("purple");
		else if (battlePet->_baseData->_health > 75 && battlePet->_baseData->_health <= 100)
			petHealthColor = QColor("red");

		/*if (battlePet->_battle_flags)
			setItemText(9, 3, QString("战斗 健康:%1").arg(battlePet->_baseData->_health), petHealthColor);
		else*/
			setItemText(9, 3, QString("健康:%1").arg(battlePet->_baseData->_health), petHealthColor);
		setItemText(10, 3, QString("Lv:%1 %2").arg(battlePet->_baseData->_level).arg(battlePet->getObjectName().isEmpty() ? battlePet->_realName : battlePet->getObjectName()));
	}
	else
	{
		for (int i = 0; i < 11; ++i)
		{
			setItemText(i, 3, "");
		}
	}
	ui.tableWidget->setUpdatesEnabled(true);
}

void GamePlayerDataWgt::doTreeViewClicked(const QModelIndex& index)
{
	if (!index.isValid())
		return;
	TreeItem* Item = static_cast<TreeItem*>(index.internalPointer());
	if (Item)
	{
		auto objid = Item->data(Qt::UserRole).toULongLong();
		m_curSelectObj = ITObjectDataMgr::getInstance().FindObject(objid);
		if (m_curSelectObj == nullptr)
			return;
		qDebug() << m_curSelectObj;

		if (GETDEVCLASS( m_curSelectObj->getObjectType()) != TObject_Character)
			return;
		ITGameCharacterPtr pRole = qSharedPointerCast<ITGameCharacter>(m_curSelectObj);
		ui.lineEdit_gold->setText(QString::number(pRole->_gold));
		ui.lineEdit_bankGold->setText(QString::number(pRole->_bankgold));
		doUpdateCharacterTable(pRole);
		doUpdateBagItemTableWidget(pRole);
		doUpdateBankItemTableWidget(pRole);
		doUpdatePetTableWidget(ui.tableWidget_pet, pRole);
		doUpdatePetTableWidget(ui.tableWidget_bankPet, pRole);
		//tableWidget_item
	}
}
void GamePlayerDataWgt::GetRowColFromItemPos(int itempos, int& row, int& col)
{
	row = itempos / 5;
	col = itempos % 5;
}

void GamePlayerDataWgt::doUpdateBagItemTableWidget(ITGameCharacterPtr pRole)
{
	if (!pRole)
		return;
	int row = 0, col = 0;
	qDebug() << "总数:" << pRole->_itemPosForPtr.size();
	QVector<int> itemPosList;
	for (auto it = pRole->_itemPosForPtr.begin(); it != pRole->_itemPosForPtr.end(); ++it)
	{
		if (it.key() < 8)
			continue;

		auto pItem = it.value();
		if (!pItem->_bExist)
			continue;
		itemPosList.append(pItem->_itemPos);

		qDebug() << it.key() << pItem->getObjectName() << pItem->_itemPos << pItem->_itemAttr;

		GetRowColFromItemPos(pItem->_itemPos - 8, row, col);
		QTableWidgetItem* pTableItem = ui.tableWidget_item->item(row, col);
		if (pTableItem == nullptr)
			continue;

		QString szText;
		if (pItem->_itemCount > 0)
			szText = QString("%1 x %2\n#%3 @%4\n%5").arg(pItem->getObjectName()).arg(pItem->_itemCount).arg(pItem->getObjectCode()).arg(pItem->_itemType).arg(pItem->_itemAttr);
		else
			szText = QString("%1\n#%2 @%3\n%5").arg(pItem->getObjectName()).arg(pItem->getObjectCode()).arg(pItem->_itemType).arg(pItem->_itemAttr);

		QString szToolTip = szText;
		szToolTip = szToolTip.remove("$0");
		szToolTip = szToolTip.remove("$1");
		szToolTip = szToolTip.remove("$2");
		szToolTip = szToolTip.remove("$3");
		szToolTip = szToolTip.remove("$4");
		if (szToolTip != pTableItem->text() /*|| userData != pItem*/)
		{
			//				pTableItem->setToolTip(QString("%1 \n%2\n等级 %3\n%4\n种类 %5").arg(pItem->name).arg(pItem->attr).arg(pItem->level).arg(pItem->type).arg(pItem->info));

			pTableItem->setToolTip(szToolTip);
			pTableItem->setText(szToolTip);
			pTableItem->setTextColor(QColor(0, 0, 0));
			//pTableItem->setData(Qt::UserRole, QVariant::fromValue(pItem));
			//	qDebug() << "Exist Update" << pItem->name << pTableItem->text() << i << pItem->pos << pItem->attr << pItem->id;
		}

	}
	for (int i = 8; i < 28; ++i)
	{
		if (!itemPosList.contains(i))
		{
			GetRowColFromItemPos(i - 8, row, col);
			QTableWidgetItem* pTableItem = ui.tableWidget_item->item(row, col);
			if (pTableItem == nullptr)
				continue;
			if ("" != pTableItem->text())
			{
				pTableItem->setText("");
				pTableItem->setData(Qt::UserRole, 0);
			}
		}
	}
	//ui.tableWidget_item->resizeRowsToContents();

}

void GamePlayerDataWgt::doUpdateBankItemTableWidget(ITGameCharacterPtr pRole)
{
	if (!pRole)
		return;
	int row = 0, col = 0;
	qDebug() << "总数:" << pRole->_itemPosForPtr.size();
	QVector<int> itemPosList;
	for (auto it = pRole->_itemPosForPtr.begin(); it != pRole->_itemPosForPtr.end(); ++it)
	{
		if (it.key() < 28)
			continue;
		auto pItem = it.value();
		if (!pItem->_bExist)
			continue;
		itemPosList.append(pItem->_itemPos);

		qDebug() << it.key() << pItem->getObjectName() << pItem->_itemPos << pItem->_itemAttr;
		GetRowColFromItemPos(pItem->_itemPos - 100, row, col);
		QTableWidgetItem* pTableItem = ui.tableWidget_bankItem->item(row, col);
		if (pTableItem == nullptr)
			continue;
		QString szText;
		if (pItem->_itemCount > 0)
			szText = QString("%1 x %2\n#%3 @%4\n%5").arg(pItem->getObjectName()).arg(pItem->_itemCount).arg(pItem->getObjectCode()).arg(pItem->_itemType).arg(pItem->_itemAttr);
		else
			szText = QString("%1\n#%2 @%3\n%5").arg(pItem->getObjectName()).arg(pItem->getObjectCode()).arg(pItem->_itemType).arg(pItem->_itemAttr);

		//QString szText = QString("%1 %2 %4").arg(pItem->getObjectName())
		//	.arg(pItem->_itemCount < 1 ? "" : pItem->_itemCount)
		//	.arg(pItem->_itemAttr);
		QString szToolTip = szText;
		szToolTip = szToolTip.remove("$0");
		szToolTip = szToolTip.remove("$1");
		szToolTip = szToolTip.remove("$2");
		szToolTip = szToolTip.remove("$3");
		szToolTip = szToolTip.remove("$4");
		if (szToolTip != pTableItem->text() /*|| userData != pItem*/)
		{
			//				pTableItem->setToolTip(QString("%1 \n%2\n等级 %3\n%4\n种类 %5").arg(pItem->name).arg(pItem->attr).arg(pItem->level).arg(pItem->type).arg(pItem->info));

			pTableItem->setToolTip(szToolTip);
			pTableItem->setText(szToolTip);
			pTableItem->setTextColor(QColor(0, 0, 0));
			//pTableItem->setData(Qt::UserRole, QVariant::fromValue(pItem));
			//	qDebug() << "Exist Update" << pItem->name << pTableItem->text() << i << pItem->pos << pItem->attr << pItem->id;
		}

	}
	for (int i = 100; i < 180; ++i)
	{
		if (!itemPosList.contains(i))
		{
			GetRowColFromItemPos(i - 100, row, col);
			QTableWidgetItem* pTableItem = ui.tableWidget_bankItem->item(row, col);
			if (pTableItem == nullptr)
				continue;
			if ("" != pTableItem->text())
			{
				pTableItem->setText("");
				pTableItem->setData(Qt::UserRole, 0);
			}
		}
	}
	//ui.tableWidget_bankItem->resizeRowsToContents();
}

void GamePlayerDataWgt::doUpdatePetTableWidget(QTableWidget *pTable, ITGameCharacterPtr pRole)
{
	if (!pRole)
		return;
	pTable->setUpdatesEnabled(false);

	int baseNum = 0;
	if (pTable == ui.tableWidget_bankPet)
	{
		baseNum = 100;
	}
	qDebug() << "PetSize:"<<pRole->_petPosForPet.size();
	for (auto it=pRole->_petPosForPet.begin();it!=pRole->_petPosForPet.end();++it)
	{
		qDebug() << it.key();
	}
	for (size_t i = baseNum; i < 5 + baseNum; i++)
	{
		auto pPet = pRole->_petPosForPet.value(i);
		int pos = i;
		QString name;
		QString szHp;
		QString szMp;
		QString sLv;
		QString sXp;
		QString sDangCi;
		QString sAttac, sDefensive, sAgility, sSpirit, sLoyality;
		QString state;
		QColor gradeColor("black");
		QColor backColor("white");
		if (pPet && pPet->_bExist)
		{
			name = pPet->getObjectName();
			if (name.isEmpty())
			{
				name = pPet->_realName;
			}
			pos = pPet->_pos;
			if (pos < 0 || pos >= 5)
			{
				pos = i;
			}
			szHp = QString("%1/%2").arg(pPet->_baseData->_hp).arg(pPet->_baseData->_maxhp);
			szMp = QString("%1/%2").arg(pPet->_baseData->_mp).arg(pPet->_baseData->_maxmp);
			sLv = QString::number(pPet->_baseData->_level);
			sXp = QString("%1").arg(pPet->_baseData->_maxxp - pPet->_baseData->_xp);
			sAttac = QString("%1").arg(pPet->_attrData->_value_attack);
			sDefensive = QString("%1").arg(pPet->_attrData->_value_defensive);
			sAgility = QString("%1").arg(pPet->_attrData->_value_agility);
			sSpirit = QString("%1").arg(pPet->_attrData->_value_spirit);
			sLoyality = QString("%1").arg(pPet->_loyality);

			if (pPet->_grade >= 0)
			{
				if (pPet->_lossMinGrade != pPet->_lossMaxGrade && pPet->_lossMaxGrade != -1)
					sDangCi = QString("%1~%2").arg(pPet->_lossMinGrade).arg(pPet->_lossMaxGrade);
				else
					sDangCi = QString("%1").arg(pPet->_grade);
				if (pPet->_grade <= 3)
				{
					gradeColor = QColor(255, 0, 0); //QColor(255, 215, 0); //金色
				}
				else if (pPet->_grade > 3 && pPet->_grade <= 6)
				{
					gradeColor = QColor("#800080"); //紫色
				}
				else if (pPet->_grade > 6 && pPet->_grade <= 9)
				{
					gradeColor = QColor("blue"); //蓝色
				}
				else if (pPet->_grade > 9 && pPet->_grade <= 12)
				{
					gradeColor = QColor("green"); //绿色
				}
			}
			state = m_petState.value(pPet->_state);
		}
		pos = pos - baseNum;
		setItemText(pTable, 0, pos, name, QColor("blue"));
		setItemText(pTable, 1, pos, szHp, QColor("red"));
		setItemText(pTable, 2, pos, szMp, QColor("blue"));
		setItemText(pTable, 3, pos, sLv, QColor("blue"));
		setItemText(pTable, 4, pos, sXp, QColor("green"));
		setItemText(pTable, 5, pos, sDangCi, gradeColor);
		setItemText(pTable, 6, pos, sAttac);
		setItemText(pTable, 7, pos, sDefensive);
		setItemText(pTable, 8, pos, sAgility);
		setItemText(pTable, 9, pos, sSpirit);
		setItemText(pTable, 10, pos, sLoyality);
		setItemText(pTable, 11, pos, state);
	}
	pTable->setUpdatesEnabled(true);
}

void GamePlayerDataWgt::setItemText(QTableWidget* pTable, int row, int col, const QString& szText, const QColor& szColor, const QColor& backColor)
{
	QTableWidgetItem* pItem = pTable->item(row, col);
	if (pItem && pItem->text() != szText)
	{
		pItem->setText(szText);
		pItem->setTextColor(szColor);
		pItem->setBackgroundColor(backColor);
	}
}
void GamePlayerDataWgt::on_pushButton_refreshModel_clicked()
{
	auto pObjList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_ServerType,0xffffff00);
	resetModel(pObjList);
}

void GamePlayerDataWgt::on_pushButton_search_clicked()
{
	QString sName = ui.lineEdit_search->text();
	if (sName.isEmpty())
		return;
	QString sType = ui.comboBox_type->currentText();
	ITObjectList pObjList;
	if (sType == "物品")
	{
		pObjList = ITObjectDataMgr::getInstance().FindData(TObject_CharItem, sName);
	}else if (sType == "宠物")
	{
		pObjList = ITObjectDataMgr::getInstance().FindData(TObject_CharPet, sName);
	}
	else if (sType == "金币")
	{
		pObjList = ITObjectDataMgr::getInstance().FindData(TObject_CharGold, sName);
	}
	qDebug() << "查询数量：" << pObjList.size();
	if (pObjList.size()< 1)
	{
		pObjList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_AccountGid);
	}
	resetModel(pObjList);

}

void GamePlayerDataWgt::on_lineEdit_search_returnPressed()
{
	on_pushButton_search_clicked();
}

void GamePlayerDataWgt::on_pushButton_filterGid_clicked()
{
	filterGid();
}

void GamePlayerDataWgt::on_lineEdit_filter_returnPressed()
{
	filterGid();
}

void GamePlayerDataWgt::filterGid()
{
	QString sGid = ui.lineEdit_filter->text();
	if (m_pPlayerDataTreeModel)
		m_pPlayerDataTreeModel->SetFilterData(sGid);
}
