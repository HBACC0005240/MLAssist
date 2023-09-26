#include "GameOftenCoordinateWgt.h"
#include "CGFunction.h"
//#include "ITObjectDataMgr.h"
#include "UserDefDialog.h"
#include <QHeaderView>
#include <QMenu>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QtConcurrent>
GameOftenCoordinateWgt::GameOftenCoordinateWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(doUpdateTgtList(QListWidgetItem *)));
	connect(ui.listWidget_offtenMap, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(doUpdateTgtList(QListWidgetItem *)));
	ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.listWidget_offtenMap->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.listWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(OnAllMapContextMenu(const QPoint &)));
	connect(ui.listWidget_offtenMap, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(OnOftenMapContextMenu(const QPoint &)));
	connect(this, &GameOftenCoordinateWgt::updateMapName, this, &GameOftenCoordinateWgt::doUpdateMapName);
	//ui.splitter->setStretchFactor(0, 1);
	//ui.splitter->setStretchFactor(1, 1);
	//ui.splitter->setStretchFactor(2, 8);
	ui.splitter->setStretchFactor(0, 0);
	ui.splitter->setStretchFactor(1, 0);
	ui.splitter->setStretchFactor(2, 10);
	ui.splitter->setSizes(QList<int>() << 0 << 0 << 1000);
	init();
	connect(g_pGameFun, SIGNAL(signal_crossMapFini(const QString &)), this, SLOT(doUpdateCrossMapStatus(const QString &)));
	connect(&ITObjectDataMgr::getInstance(), SIGNAL(signal_loadDataFini()), this, SLOT(doInit()));
	connect(&ITObjectDataMgr::getInstance(), SIGNAL(signal_loadGateMapFini()), this, SLOT(doInit()));
	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapInfo, this, &GameOftenCoordinateWgt::doUpdateMapData, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_switchFloatOftenMap, this, &GameOftenCoordinateWgt::signal_float_window);
}

GameOftenCoordinateWgt::~GameOftenCoordinateWgt()
{
}
//加载传送地图数据
void GameOftenCoordinateWgt::init()
{
	
	QStringList saveHeadList;
	saveHeadList << "编码"
				 << "名称"
				 << "坐标"
				 << "类型"
				 << "对话"
				 << "备注"
				 << "寻路";
	ui.tableWidget->setColumnCount(saveHeadList.size());
	ui.tableWidget->setHorizontalHeaderLabels(saveHeadList);
	//	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	ui.tableWidget->verticalHeader()->setVisible(false);
	//ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->setRowCount(1);
	ui.tableWidget->horizontalHeader()->setFixedHeight(30);

	connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(doTableCellClicked(int, int)));
	connect(ui.tableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(doItemDoubleClicked(QTableWidgetItem *)));
	connect(ui.tableWidget->horizontalHeader(), SIGNAL(sectionPressed(int)), this, SLOT(doColPressed(int)));
	connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(doTableItemChanged(QTableWidgetItem *)));
	ui.tableWidget->setColumnWidth(0, 60);
	ui.tableWidget->setColumnWidth(1, 100);
	ui.tableWidget->setColumnWidth(2, 70);
	ui.tableWidget->setColumnWidth(3, 40);
	ui.tableWidget->setColumnWidth(4, 60);
	ui.tableWidget->setColumnWidth(5, 100);
	ui.tableWidget->setColumnWidth(6, 50);
}

void GameOftenCoordinateWgt::initTableData(ITGameMapPtr pGameMap)
{
	QMutexLocker locked(&m_lockUpdateTable);
	clearTableData();
	//这个是自身可达的
	auto pReachableMapList = pGameMap->m_pGateMapList;
	for (size_t i = 0; i < pReachableMapList.size(); i++)
	{
		auto pGateMap = pReachableMapList.at(i);
		QString strItem1, strItem2, strItem3, strItem4, strItem5, strItem6;
		strItem1 = QString::number(pGateMap->_targetMapNumber);
		strItem2 = pGateMap->_targetMapName;
		strItem3 = QString("%1,%2").arg(pGateMap->_x).arg(pGateMap->_y);
		strItem4 = QString::number(pGateMap->_warpType);
		strItem5 = pGateMap->_npcSelect.join(";");
		strItem6 = pGateMap->getObjectDesc();

		int rowno = ui.tableWidget->rowCount();
		QTableWidgetItem *item1 = new QTableWidgetItem(strItem1);
		item1->setTextAlignment(Qt::AlignCenter);
		//item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.tableWidget->setItem(rowno - 1, 0, item1);

		QTableWidgetItem *item2 = new QTableWidgetItem(strItem2);
		item2->setTextAlignment(Qt::AlignCenter);
		//item2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.tableWidget->setItem(rowno - 1, 1, item2);

		QTableWidgetItem *item3 = new QTableWidgetItem(strItem3);
		item3->setTextAlignment(Qt::AlignCenter);
		//item3->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.tableWidget->setItem(rowno - 1, 2, item3);

		QTableWidgetItem *item4 = new QTableWidgetItem(strItem4);
		item4->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(rowno - 1, 3, item4);
		item4->setData(Qt::UserRole, QVariant::fromValue(pGateMap));

		QTableWidgetItem *item5 = new QTableWidgetItem(strItem5);
		item5->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(rowno - 1, 4, item5);
		item5->setData(Qt::UserRole, QVariant::fromValue(pGateMap));

		QTableWidgetItem *item6 = new QTableWidgetItem(strItem6);
		item6->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(rowno - 1, 5, item6);
		item6->setData(Qt::UserRole, QVariant::fromValue(pGateMap));

		QPushButton *pButton = new QPushButton("寻");
		pButton->setProperty("data", QVariant::fromValue(pGateMap));
		pButton->setMaximumWidth(30);
		QHBoxLayout *pHLayout = new QHBoxLayout();
		QWidget *pWidget = new QWidget();
		pHLayout->addWidget(pButton);
		pHLayout->setMargin(0);
		pHLayout->setAlignment(pButton, Qt::AlignCenter);
		pWidget->setLayout(pHLayout);
		ui.tableWidget->setCellWidget(rowno - 1, 6, pWidget);
		connect(pButton, SIGNAL(clicked()), this, SLOT(doButtonClicked()));
		if (rowno == pReachableMapList.size())
		{
			return;
		}
		rowno++;
		ui.tableWidget->setRowCount(rowno);
	}
	//ui.tableWidget->setUpdatesEnabled(true);
	//ui.tableWidget->resizeColumnsToContents(); //根据内容调整列宽 但每次都变 太麻烦 修改下
}

void GameOftenCoordinateWgt::clearTableData()
{
	int rowCounts = ui.tableWidget->rowCount();
	int colCounts = ui.tableWidget->columnCount();
	int nWidgetCol = 6;
	for (int i = 0; i < rowCounts - 1; i++)
	{
		for (int j = 0; j < colCounts; j++)
		{
			QTableWidgetItem *tableitem = ui.tableWidget->item(i, j);
			if (tableitem != NULL)
				delete tableitem;
		}

		ui.tableWidget->removeRow(0);
	}
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(1);
}
void GameOftenCoordinateWgt::runCrossMapNavigation(GameOftenCoordinateWgt *pThis, int tx, int ty, int tMapNumber)
{
	if (!pThis)
		return;
	g_pGameFun->RestFun();
	g_pGameFun->AutoMoveToTgtMap(tx, ty, tMapNumber);
}

void GameOftenCoordinateWgt::doInit()
{
	ui.listWidget->clear();
	ui.listWidget_offtenMap->clear();
	auto warpHash = ITObjectDataMgr::getInstance().GetWarpMapHash();
	for (auto it = warpHash.begin(); it != warpHash.end(); ++it)
	{
		auto pMapObj = it.value();
		QListWidgetItem *pItem = new QListWidgetItem(QString("%1 %2").arg(pMapObj->_mapNumber).arg(pMapObj->getObjectName()));
		pItem->setData(Qt::UserRole, QVariant::fromValue(pMapObj));
		ui.listWidget->addItem(pItem);
		m_allMapForItem.insert(pMapObj, pItem);
		if (pMapObj->_oftenMap)
		{
			QListWidgetItem *pOftenItem = new QListWidgetItem(QString("%1 %2").arg(pMapObj->_mapNumber).arg(pMapObj->getObjectName()));
			pOftenItem->setData(Qt::UserRole, QVariant::fromValue(pMapObj));
			ui.listWidget_offtenMap->addItem(pOftenItem);
			m_oftenMapForItem.insert(pMapObj, pOftenItem);
		}
	}
}

//显示可达目标项
void GameOftenCoordinateWgt::doUpdateTgtList(QListWidgetItem *pItem)
{
	if (!pItem)
		return;
	auto pMap = pItem->data(Qt::UserRole).value<ITGameMapPtr>();
	if (pMap.isNull())
		return;
	initTableData(pMap);
}

void GameOftenCoordinateWgt::OnAllMapContextMenu(const QPoint &pos)
{
	QListWidgetItem *curItem = ui.listWidget->itemAt(pos); //获取当前被点击的节点
	if (curItem == NULL)
		return;
	QMenu menu;
	menu.addAction("添加至常用", this, SLOT(doAddToOftenMap()));
	menu.addAction("修改地图名称", this, [&]()
			{ emit updateMapName(curItem); });
	menu.exec(QCursor::pos());
}

void GameOftenCoordinateWgt::OnOftenMapContextMenu(const QPoint &pos)
{
	QListWidgetItem *curItem = ui.listWidget_offtenMap->itemAt(pos); //获取当前被点击的节点
	if (curItem == NULL)
		return;
	QMenu menu;
	menu.addAction("取消常用", this, SLOT(doRemoveOftenMap()));
	menu.addAction("修改地图名称", this, [&]()
			{ emit updateMapName(curItem); });
	menu.exec(QCursor::pos());
}
//添加至常用
void GameOftenCoordinateWgt::doAddToOftenMap()
{
	QListWidgetItem *curItem = ui.listWidget->currentItem();
	if (curItem == NULL)
		return;
	auto pMap = curItem->data(Qt::UserRole).value<ITGameMapPtr>();
	if (pMap.isNull())
		return;
	if (pMap->_oftenMap)
		return;
	pMap->_oftenMap = 1;
	pMap->setEditStatus();
	QListWidgetItem *pOftenItem = new QListWidgetItem(QString("%1 %2").arg(pMap->_mapNumber).arg(pMap->getObjectName()));
	pOftenItem->setData(Qt::UserRole, QVariant::fromValue(pMap));
	ui.listWidget_offtenMap->addItem(pOftenItem);
	m_oftenMapForItem.insert(pMap, pOftenItem);
}
//修改地图名称 同时更新全部和常用
void GameOftenCoordinateWgt::doUpdateMapName(QListWidgetItem *curItem)
{
	//QListWidgetItem *curItem = ui.listWidget->currentItem();
	if (curItem == NULL)
		return;
	auto pMap = curItem->data(Qt::UserRole).value<ITGameMapPtr>();
	if (pMap.isNull())
		return;
	UserDefDialog dlg(this);
	dlg.setLabelText("地图名称：");
	dlg.setDefaultVal(pMap->getObjectName());
	if (dlg.exec() == QDialog::Accepted)
	{
		QString sName = dlg.getVal();
		pMap->setObjectName(sName);
		pMap->setEditStatus();
		//不实时更新GateMap的名称了 下次加载数据库更新
		auto pAllMapItem = m_allMapForItem.value(pMap);
		auto pOftenMapItem = m_oftenMapForItem.value(pMap);
		QString itemName = QString("%1 %2").arg(pMap->_mapNumber).arg(pMap->getObjectName());
		if (pAllMapItem)
			pAllMapItem->setText(itemName);
		if (pOftenMapItem)
			pOftenMapItem->setText(itemName);
	}
}
//删除常用
void GameOftenCoordinateWgt::doRemoveOftenMap()
{
	QListWidgetItem *curItem = ui.listWidget_offtenMap->currentItem();
	if (curItem == NULL)
		return;
	auto pMap = curItem->data(Qt::UserRole).value<ITGameMapPtr>();
	if (pMap.isNull())
		return;
	pMap->_oftenMap = 0;
	pMap->setEditStatus();
	ui.listWidget_offtenMap->takeItem(ui.listWidget_offtenMap->currentRow());
	m_oftenMapForItem.remove(pMap);
}

void GameOftenCoordinateWgt::doButtonClicked()
{
	QPushButton *pBtn = (QPushButton *)sender();
	if (!pBtn)
		return;
	auto pGateMap = pBtn->property("data").value<ITGameGateMapPtr>();
	if (!pGateMap)
		return;
	pGateMap->_mapNumber;
	//这个是跨地图坐标寻路
	int curMapIndex = g_pGameFun->GetMapIndex();
	if (curMapIndex == pGateMap->_mapNumber) //当前地图寻路
	{
		ui.label_status->setText("寻路中...");
		//如果需要对话的话 目标坐标算附近空格
		auto tmpTgtPos = QPoint(pGateMap->_x, pGateMap->_y);
		if (pGateMap->_warpType != 0)
			tmpTgtPos = g_pGameFun->GetRandomSpace(pGateMap->_x, pGateMap->_y, 1);

		emit g_pGameCtrl->signal_moveToTarget(tmpTgtPos.x(), tmpTgtPos.y());
		//		g_pGameFun->AutoMoveTo(pGateMap->_x, pGateMap->_y);
	}
	else
	{ //跨地图寻路
		ui.label_status->setText("寻路中...");
		QtConcurrent::run(runCrossMapNavigation, this, pGateMap->_x, pGateMap->_y, pGateMap->_mapNumber);
		//g_pGameFun->RestFun();
		//g_pGameFun->AutoMoveToTgtMap(pGateMap->_x, pGateMap->_y,pGateMap->_mapNumber);
		//		ITObjectDataMgr::getInstance().FindTargetNavigation(pGateMap->_mapNumber, QPoint(pGateMap->_x, pGateMap->_y)); //100, QPoint(475, 196));
	}
}

void GameOftenCoordinateWgt::doTableItemChanged(QTableWidgetItem *pItem)
{
	if (!pItem)
		return;
	auto pGateMap = pItem->data(Qt::UserRole).value<ITGameGateMapPtr>();
	if (pGateMap.isNull())
		return;
	int nCol = pItem->column();
	QString sText = pItem->text();
	switch (nCol)
	{
		case 3: //类型
		{
			if (sText.toInt() != pGateMap->_warpType)
			{
				pGateMap->_warpType = sText.toInt();
				pGateMap->setEditStatus();
			}
			break;
		}
		case 4: //对话
		{
			if (sText != pGateMap->_npcSelect.join(";"))
			{
				QStringList npcOperation = sText.split(";");
				pGateMap->_npcSelect = npcOperation;
				pGateMap->setEditStatus();
			}
			break;
		}
		case 5:
		{
			if (sText != pGateMap->getObjectDesc())
			{
				pGateMap->setObjectDsec(pItem->text());
				pGateMap->setEditStatus();
			}
			break;
		}
		default:
			break;
	}
}

void GameOftenCoordinateWgt::on_pushButton_stop_clicked()
{
	ui.label_status->setText("状态：");
	g_pGameFun->StopFun();
}

void GameOftenCoordinateWgt::on_lineEdit_all_returnPressed()
{
	QString sText = ui.lineEdit_all->text();
	searchItem(sText, ui.listWidget);
}

void GameOftenCoordinateWgt::on_pushButton_all_clicked()
{
	on_lineEdit_all_returnPressed();
}

void GameOftenCoordinateWgt::on_lineEdit_often_returnPressed()
{
	QString sText = ui.lineEdit_often->text();
	searchItem(sText, ui.listWidget_offtenMap);
}

void GameOftenCoordinateWgt::on_pushButton_often_clicked()
{
	on_lineEdit_often_returnPressed();
}

void GameOftenCoordinateWgt::on_lineEdit_target_returnPressed()
{
	QString sText = ui.lineEdit_target->text();

	auto findItems = ui.tableWidget->findItems(sText, Qt::MatchContains);
	if (findItems.size() > 0)
	{
		auto curItem = ui.tableWidget->currentItem();
		if (findItems.contains(curItem))
		{
			int index = findItems.indexOf(curItem);
			index += 1;
			if (index >= findItems.size())
			{
				index = 0;
			}
			auto newCurItem = findItems.at(index);
			ui.tableWidget->setCurrentItem(newCurItem);
		}
		else
		{
			ui.tableWidget->setCurrentItem(findItems.at(0));
		}
	}
}

void GameOftenCoordinateWgt::on_pushButton_target_clicked()
{
	on_lineEdit_target_returnPressed();
}

void GameOftenCoordinateWgt::searchItem(const QString &sText, QListWidget *pListWidget)
{
	if (sText.isEmpty() || !pListWidget)
		return;

	auto findItems = pListWidget->findItems(sText, Qt::MatchContains);
	if (findItems.size() > 0)
	{
		auto curItem = pListWidget->currentItem();
		if (findItems.contains(curItem))
		{
			int index = findItems.indexOf(curItem);
			index += 1;
			if (index >= findItems.size())
			{
				index = 0;
			}
			auto newCurItem = findItems.at(index);
			pListWidget->setCurrentItem(newCurItem);
		}
		else
		{
			pListWidget->setCurrentItem(findItems.at(0));
		}
	}
}

void GameOftenCoordinateWgt::doUpdateCrossMapStatus(const QString &msg)
{
	//	qDebug() << "upda" << msg;
	if (msg.isEmpty())
		ui.label_status->setText("状态：");
	else
		ui.label_status->setText("状态：" + msg);
	//g_pGameFun->StopFun();
}

void GameOftenCoordinateWgt::on_pushButton_loadMap_clicked()
{
	if (!ITObjectDataMgr::getInstance().getDBConnectState())
	{
		QString sDBPath = QApplication::applicationDirPath() + "//db//cg.db";
		bool bRet = false;
		ITObjectDataMgr::getInstance().connectToDB("SQLITE", "CG", sDBPath, "admin", "123456");
	}
	ITObjectDataMgr::getInstance().loadGateMapData();
}

void GameOftenCoordinateWgt::on_pushButton_float_clicked()
{
	emit signal_float_window();
}

void GameOftenCoordinateWgt::doUpdateMapData(QString name, int index1, int index2, int index3, int x, int y)
{
	if (m_lastMapNumber == index3)
		return;	
	if (m_allMapForItem.size() <1)
	{
		return;
	}
	m_lastMapNumber = index3;
	QListWidgetItem *pItem = nullptr;
	for (auto it = m_allMapForItem.begin(); it != m_allMapForItem.end(); ++it)
	{
		if (it.key()->_mapNumber == index3)
		{

			pItem = it.value();
			break;
		}
	}
	doUpdateTgtList(pItem);
}
