#include "GameNpcSymbolWgt.h"
#include <QMenu>
GameNpcSymbolWgt::GameNpcSymbolWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	m_lastUpdate.start();
	initTable();
	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapCellInfo, this, &GameNpcSymbolWgt::OnNotifyGetMapCellInfo, Qt::ConnectionType::QueuedConnection);
}

GameNpcSymbolWgt::~GameNpcSymbolWgt()
{
}

void GameNpcSymbolWgt::initTable()
{
	QStringList saveHeadList;
	saveHeadList  << "名字"
			<< "NpcID"
			<< "Npc位置"
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

void GameNpcSymbolWgt::OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
	//不启用 不刷新
	if (!g_pGameCtrl->IsEnabledDisplayDataUi())
		return;
	
	if ( !g_pGameCtrl->IsRealUpdateUi())
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
			if (unit.valid && unit.type == 1 && unit.model_id != 0 && (unit.flags & 4096) != 0)
			{
				if (m_bFilterNoName && unit.unit_name.isEmpty())
				{
					continue;
				}
				/*qDebug() << unit.valid << unit.type << unit.level << unit.flags << unit.model_id
						 << unit.unit_id << unit.unit_name << unit.nick_name << unit.title_name
						 << unit.item_name << unit.xpos << unit.ypos;*/
				m_units.append(unit);
				unitIDList.append(unit.unit_id);
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

		//	initTableData();
	}
	m_lastUpdate.restart();
	return;
}


void GameNpcSymbolWgt::on_customContextMenu(const QPoint &pos)
{
	QMenu menu;
	int nrow = -1;
	QTableWidgetItem *tempitem = ui.tableWidget->itemAt(pos);
	if (tempitem != NULL)
	{
		nrow = tempitem->row();
	}
	auto pAction = menu.addAction(QString("屏蔽没名字的"), [&] ()
		{
		m_bFilterNoName = !m_bFilterNoName;
		});
	menu.addAction(QString("移动到物品附近"), [&]() {
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
	pAction->setCheckable(true);
	pAction->setChecked(m_bFilterNoName);
	menu.exec(QCursor::pos());
	
}

//更新坐标 以及新增行
bool GameNpcSymbolWgt::judgeNeedUpdate(const CGA_MapUnit_t &unit)
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
		strItem1 = unit.unit_name;
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

bool GameNpcSymbolWgt::removeUnitRow(const CGA_MapUnit_t &unit)
{
	auto pTblItem = m_unitIDForItem.value(unit.unit_id);
	if (pTblItem)
	{
		ui.tableWidget->removeRow(pTblItem->row());
	}
	m_unitIDForItem.remove(unit.unit_id);
	return true;
}
