#include "GameItemsPosWgt.h"
#include "UserDefDialog.h"
#include <QMenu>
GameItemsPosWgt::GameItemsPosWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	m_lastUpdate.start();
	connect(g_pGameCtrl, &GameCtrl::signal_switchAutoPickItemUI, this, [&](bool bFlag)
			{ ui.groupBox->setChecked(bFlag); });
	connect(g_pGameCtrl, &GameCtrl::signal_addPickItemUI, this, [&](bool bFlag, QString sText)
			{
				if (sText.isEmpty())
					return;
				auto pFindItem = ui.listWidget->findItems(sText, Qt::MatchExactly);
				if (pFindItem.size() > 0)
				{
					pFindItem.at(0)->setCheckState(bFlag ? Qt::Checked : Qt::Unchecked);
					return;
				}
				QListWidgetItem *pItem = new QListWidgetItem(sText);
				pItem->setCheckState(bFlag ? Qt::Checked : Qt::Unchecked);
				ui.listWidget->addItem(pItem);
				pItem->setToolTip(pItem->text());
				g_pGameCtrl->SetPickItemIsChecked(sText, bFlag);
			});
	connect(g_pGameCtrl, &GameCtrl::signal_switchFoolowPickItemUI, this, [&](bool bFlag)
			{ ui.groupBox->setChecked(bFlag); });
	
	initTable();
	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapCellInfo, this, &GameItemsPosWgt::OnNotifyGetMapCellInfo, Qt::ConnectionType::QueuedConnection);
	connect(ui.listWidget, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doListItemChanged(QListWidgetItem *)));
}

GameItemsPosWgt::~GameItemsPosWgt()
{
}

void GameItemsPosWgt::initTable()
{
	QStringList saveHeadList;
	saveHeadList << "物品名字"
				 << "物品ID"
				 << "物品位置"
				 << "外观ID";
	ui.tableWidget->setColumnCount(saveHeadList.size());
	ui.tableWidget->setHorizontalHeaderLabels(saveHeadList);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	ui.tableWidget->verticalHeader()->setVisible(false);
	//ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->horizontalHeader()->setFixedHeight(30);
	//	ui.tableWidget->setRowCount(1);

	connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(doTableCellClicked(int, int)));
	connect(ui.tableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(doItemDoubleClicked(QTableWidgetItem *)));
	connect(ui.tableWidget->horizontalHeader(), SIGNAL(sectionPressed(int)), this, SLOT(doColPressed(int)));
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); //打开右键功能
	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_customContextMenu(const QPoint &)));
}

void GameItemsPosWgt::OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
	//不启用 不刷新
	if (!g_pGameCtrl->IsEnabledDisplayDataUi())
		return;
	if (!g_pGameCtrl->IsRealUpdateUi())
	{
		if (m_lastUpdate.elapsed() < 5000) //5秒更新
			return;
	}	
	if (units)
	{
		QMutexLocker locked(&m_lockUpdateTable);
		m_units.clear();
		QList<quint64> unitIDList;
		for (size_t i = 0; i < units->size(); i++)
		{
			auto unit = units->at(i);
			if (unit.valid && unit.model_id != 0 && !(unit.flags & 4096) && !(unit.flags & 256)) //物品
			{
				m_units.append(unit);
				unitIDList.append(unit.unit_id);
				/*		qDebug() << unit.valid << unit.type << unit.level << unit.flags << unit.injury << unit.item_count << unit.model_id
						 << unit.unit_id << unit.unit_name << unit.nick_name << unit.title_name
						 << unit.item_name << unit.xpos << unit.ypos;*/
			}
		}
		//对比差异
		CGA_MapUnits_t needRemoveUnits;
		for (size_t i = 0; i < m_units.size(); i++)
		{
			auto unit = m_units.at(i);
			judgeNeedUpdate(unit); //新增 和修改
		}
		for (size_t i = 0; i < m_lastUnits.size(); i++)
		{
			auto unit = m_lastUnits.at(i);
			if (unitIDList.contains(unit.unit_id))
			{
				continue;
			}
			else
			{
				removeUnitRow(unit);
			}
		}
		m_lastUnits = m_units;
		//AutoPickupItems();

		//	initTableData();
	}
	m_lastUpdate.restart();
}

void GameItemsPosWgt::on_customContextMenu(const QPoint &pos)
{
	QTableWidgetItem *tempitem = ui.tableWidget->itemAt(pos);
	if (tempitem != NULL)
	{
		int nrow = tempitem->row();
		QMenu menu;
		menu.addAction(QString("添加至自动捡取"), [&]()
				{
					QTableWidgetItem *pNameItem = ui.tableWidget->item(nrow, 0);
					if (pNameItem)
					{
						auto pFindItems = ui.listWidget->findItems(pNameItem->text(), Qt::MatchCaseSensitive);
						if (pFindItems.size() > 0)
							return;
						QListWidgetItem *pTableItem = new QListWidgetItem(pNameItem->text());
						pTableItem->setCheckState(Qt::Checked);
						ui.listWidget->addItem(pTableItem);
						pTableItem->setToolTip(pTableItem->text());
						g_pGameCtrl->SetPickItemIsChecked(pNameItem->text(), true);
						//m_pickNameList.append(pNameItem->text());
					}
				});
		menu.addAction(QString("移动到物品附近"), [&]()
				{
					QTableWidgetItem *pPosItem = ui.tableWidget->item(nrow, 2);
					if (pPosItem)
					{
						QStringList splitPos = pPosItem->text().split(",");
						if (splitPos.size() > 1)
						{
							int x = splitPos.at(0).toInt();
							int y = splitPos.at(1).toInt();
							auto tgtPos = g_pGameFun->GetRandomSpace(x, y);
							emit g_pGameCtrl->signal_moveToTarget(tgtPos.x(), tgtPos.y());
						}
					}
				});
		menu.exec(QCursor::pos());
	}
}

void GameItemsPosWgt::on_pushButton_del_clicked()
{
	QListWidgetItem *pItem = ui.listWidget->currentItem();
	if (pItem)
	{
		m_pickNameList.removeAll(pItem->text());
		g_pGameCtrl->SetPickItemIsChecked(pItem->text(), false);
		ui.listWidget->takeItem(ui.listWidget->currentRow());
	}
}

void GameItemsPosWgt::on_pushButton_add_clicked()
{
	UserDefDialog dlg(this);
	dlg.setWindowTitle("要捡的物品名称");
	dlg.setLabelText("名称：");
	if (dlg.exec() == QDialog::Accepted)
	{
		auto pFindItems = ui.listWidget->findItems(dlg.getVal(), Qt::MatchCaseSensitive);
		if (pFindItems.size() > 0)
			return;
		//m_pickNameList.append(dlg.getVal());
		QListWidgetItem *pTableItem = new QListWidgetItem(dlg.getVal());
		pTableItem->setCheckState(Qt::Unchecked);
		ui.listWidget->addItem(pTableItem);
		pTableItem->setToolTip(pTableItem->text());
	}
}

void GameItemsPosWgt::on_groupBox_toggled(bool checked)
{
	m_bAutoPickup = checked;
	g_pGameCtrl->OnSetAutoPickItems(checked);
	g_pGameFun->RestFun();
}

void GameItemsPosWgt::doListItemChanged(QListWidgetItem *pItem)
{
	if (pItem == nullptr)
		return;
	QString name = pItem->text();
	if (pItem->checkState() == Qt::Checked)
	{
		m_pickNameList.push_back(name);
		g_pGameCtrl->SetPickItemIsChecked(name, true);
	}
	else
	{
		g_pGameCtrl->SetPickItemIsChecked(name, false);
		m_pickNameList.removeAll(name);
	}
}

void GameItemsPosWgt::on_checkBox_follow_stateChanged(int state)
{
	m_bFollowMap = (state == Qt::Checked ? true : false);
	g_pGameCtrl->OnSetFollowPickItem(state);
}

void GameItemsPosWgt::on_checkBox_fixedPoint_stateChanged(int state)
{
	int x = ui.lineEdit_east->text().toInt();
	int y = ui.lineEdit_south->text().toInt();
	auto pickCfg = g_pGameCtrl->GetGamePickItemCfg();
	pickCfg->bFixedPoint = (state == Qt::Checked ? true : false);
	if (x ==0 && y ==0)
	{
		auto curPos = g_pGameFun->GetMapCoordinate();
		ui.lineEdit_east->setText(QString::number(curPos.x()));
		ui.lineEdit_south->setText(QString::number(curPos.y()));
		x = curPos.x();
		y = curPos.y();
	}
	pickCfg->east = x;
	pickCfg->south = y;
}

void GameItemsPosWgt::on_lineEdit_east_editingFinished()
{
	int x = ui.lineEdit_east->text().toInt();
	auto pickCfg = g_pGameCtrl->GetGamePickItemCfg();
	pickCfg->east = x;
}

void GameItemsPosWgt::on_lineEdit_south_editingFinished()
{
	int y = ui.lineEdit_south->text().toInt();
	auto pickCfg = g_pGameCtrl->GetGamePickItemCfg();
	pickCfg->south = y;
}

void GameItemsPosWgt::initTableData()
{
	if (m_units.size() < 1)
		return;
	//unit_id唯一 得改成每次新增n行 已经删除指定行
	//clearTableData();
	for (size_t i = 0; i < m_units.size(); i++)
	{ //model_id是外观编号  unit_id应该是一个当前唯一编号
		auto unit = m_units.at(i);
		qDebug() << unit.valid << unit.type << unit.level << unit.flags << unit.model_id
				 << unit.unit_id << unit.unit_name << unit.nick_name << unit.title_name
				 << unit.item_name << unit.xpos << unit.ypos;
		QString strItem1, strItem2, strItem3, strItem4, strItem5, strItem6;
		strItem1 = unit.item_name;
		strItem2 = QString::number(unit.unit_id);
		strItem3 = QString("%1,%2").arg(unit.xpos).arg(unit.ypos);
		strItem4 = QString::number(unit.model_id);

		int rowno = ui.tableWidget->rowCount();
		QTableWidgetItem *item1 = new QTableWidgetItem(strItem1);
		item1->setTextAlignment(Qt::AlignCenter);
		//item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		item1->setData(Qt::UserRole, unit.unit_id);
		ui.tableWidget->setItem(rowno - 1, 0, item1);

		QTableWidgetItem *item2 = new QTableWidgetItem(strItem2);
		item2->setTextAlignment(Qt::AlignCenter);
		//item2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.tableWidget->setItem(rowno - 1, 1, item2);

		QTableWidgetItem *item3 = new QTableWidgetItem(strItem3);
		item3->setTextAlignment(Qt::AlignCenter);
		//item3->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.tableWidget->setItem(rowno - 1, 2, item3);
		item3->setData(Qt::UserRole, unit.unit_id);

		QTableWidgetItem *item4 = new QTableWidgetItem(strItem4);
		item4->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(rowno - 1, 3, item4);
		m_unitIDForItem.insert(unit.unit_id, item3);
		if (rowno == m_units.size())
		{
			return;
		}
		rowno++;
		ui.tableWidget->setRowCount(rowno);
	}
	//ui.tableWidget->setUpdatesEnabled(true);
	//ui.tableWidget->resizeColumnsToContents(); //根据内容调整列宽 但每次都变 太麻烦 修改下
}

void GameItemsPosWgt::clearTableData()
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
	m_unitIDForItem.clear();
}
//更新坐标 以及新增行
bool GameItemsPosWgt::judgeNeedUpdate(const CGA_MapUnit_t &unit)
{
	auto pTblItem = m_unitIDForItem.value(unit.unit_id);
	if (pTblItem)
	{
		auto sPos = QString("%1,%2").arg(unit.xpos).arg(unit.ypos);
		if (pTblItem->text() == sPos)
		{
			return false;
		}
		else
		{
			pTblItem->setText(sPos);
		}
	}
	else
	{
		QString strItem1, strItem2, strItem3, strItem4, strItem5, strItem6;
		strItem1 = unit.item_name;
		strItem2 = QString::number(unit.unit_id);
		strItem3 = QString("%1,%2").arg(unit.xpos).arg(unit.ypos);
		strItem4 = QString::number(unit.model_id);

		int rowno = ui.tableWidget->rowCount();
		ui.tableWidget->setRowCount(rowno + 1);

		QTableWidgetItem *item1 = new QTableWidgetItem(strItem1);
		item1->setTextAlignment(Qt::AlignCenter);
		//item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		item1->setData(Qt::UserRole, unit.unit_id);
		ui.tableWidget->setItem(rowno, 0, item1);

		QTableWidgetItem *item2 = new QTableWidgetItem(strItem2);
		item2->setTextAlignment(Qt::AlignCenter);
		//item2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.tableWidget->setItem(rowno, 1, item2);

		QTableWidgetItem *item3 = new QTableWidgetItem(strItem3);
		item3->setTextAlignment(Qt::AlignCenter);
		//item3->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.tableWidget->setItem(rowno, 2, item3);
		item3->setData(Qt::UserRole, unit.unit_id);

		QTableWidgetItem *item4 = new QTableWidgetItem(strItem4);
		item4->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(rowno, 3, item4);
		m_unitIDForItem.insert(unit.unit_id, item3);
	}
	return true;
}

bool GameItemsPosWgt::removeUnitRow(const CGA_MapUnit_t &unit)
{
	auto pTblItem = m_unitIDForItem.value(unit.unit_id);
	if (pTblItem)
	{
		ui.tableWidget->removeRow(pTblItem->row());
	}
	m_unitIDForItem.remove(unit.unit_id);
	return true;
}

bool GameItemsPosWgt::AutoPickupItems()
{
	if (!m_bAutoPickup)
		return false;
	if (m_pickNameList.size() < 1)
		return false;
	if (g_pGameFun->GetInventoryEmptySlotCount() < 1)
	{
		qDebug() << "包裹满了，不自动捡咯";
		return false;
	}
	QPoint curPos = g_pGameFun->GetMapCoordinate();
	for (size_t i = 0; i < m_lastUnits.size(); i++)
	{
		auto unit = m_lastUnits.at(i);
		if (m_pickNameList.contains(unit.item_name))
		{
			if (m_bFollowMap)
			{
				g_pGameFun->MoveToNpcNear(unit.xpos, unit.ypos);
			}
			curPos = g_pGameFun->GetMapCoordinate();
			if (g_pGameFun->IsNearTarget(curPos.x(), curPos.y(), unit.xpos, unit.ypos))
			{
				g_pGameFun->TurnAboutEx(unit.xpos, unit.ypos);
			}
		}
	}
	return true;
}
