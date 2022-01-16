#include "GameOftenFunctionWgt.h"
#include "GameCtrl.h"
#include "stdafx.h"
GameOftenFunctionWgt::GameOftenFunctionWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.checkBox_AutoSupply, SIGNAL(stateChanged(int)), g_pGameCtrl, SLOT(OnSetAutoSupply(int)), Qt::ConnectionType::QueuedConnection);
	connect(ui.checkBox_autoSale, SIGNAL(stateChanged(int)), g_pGameCtrl, SLOT(OnSetAutoSale(int)), Qt::ConnectionType::QueuedConnection);

	connect(g_pGameCtrl, &GameCtrl::signal_switchAutoSupplyUI, this, [&](bool bFlag)
			{ ui.checkBox_AutoSupply->setChecked(bFlag); });
	connect(g_pGameCtrl, &GameCtrl::signal_switchAutoSaleUI, this, [&](bool bFlag)
			{ ui.checkBox_autoSale->setChecked(bFlag); });
	connect(g_pGameCtrl, &GameCtrl::signal_addSaleItemUI, this, [&](bool bFlag, QString sText)
			{
				if (sText.isEmpty())
					return;
				auto pFindItem = ui.listWidget_sale->findItems(sText, Qt::MatchExactly);
				if (pFindItem.size() > 0)
				{
					pFindItem.at(0)->setCheckState(bFlag ? Qt::Checked : Qt::Unchecked);
					return;
				}
				QListWidgetItem *pItem = new QListWidgetItem(sText);
				pItem->setCheckState(bFlag ? Qt::Checked : Qt::Unchecked);
				ui.listWidget_sale->addItem(pItem);
				pItem->setToolTip(pItem->text());
				g_pGameCtrl->setSaleItemIsChecked(sText, 0);
			});

	connect(ui.listWidget_searchFilter, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doSearchItemChanged(QListWidgetItem *)));
	connect(ui.listWidget_sale, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doSaleItemChanged(QListWidgetItem *)));
	connect(g_pGameCtrl, SIGNAL(signal_addSaleItem(GameItemPtr)), this, SLOT(doAddSaleItem(GameItemPtr)));
	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapCellInfo, this, &GameOftenFunctionWgt::OnNotifyGetMapCellInfo, Qt::ConnectionType::QueuedConnection);
	init();
}

GameOftenFunctionWgt::~GameOftenFunctionWgt()
{
}
void GameOftenFunctionWgt::init()
{
	QStringList searchList;
	searchList << "探测全部"
			   << "出入口"
			   << "无照护士米内鲁帕"
			   << "暗黑龙"
			   << "宝箱"
			   << "卡片"
			   << "魔石";
	for (auto searchText : searchList)
	{
		QListWidgetItem *pItem = new QListWidgetItem(searchText);
		pItem->setCheckState(Qt::Unchecked);
		ui.listWidget_searchFilter->addItem(pItem);
		pItem->setToolTip(pItem->text());
		g_pGameCtrl->setSearchItem(searchText, 0);
	}
	QStringList initSaleItems;
	initSaleItems << "魔石"
				  << "卡片？"
				  << "哥布林的卡片"
				  << "迷你蝙蝠的卡片"
				  << "锥形水晶"
				  << "岩虫的卡片"
				  << "锹型虫的卡片"
				  << "水晶怪的卡片";
	for (QString initSale : initSaleItems)
	{
		QListWidgetItem *pTableItem = new QListWidgetItem(initSale);
		pTableItem->setCheckState(Qt::Checked);
		ui.listWidget_sale->addItem(pTableItem);
		pTableItem->setToolTip(pTableItem->text());
		g_pGameCtrl->setSaleItemIsChecked(initSale, Qt::Checked);
	}
}
void GameOftenFunctionWgt::initSearchItems(GameSearchList itemList)
{
	ui.listWidget_searchFilter->clear();
	foreach (auto it, itemList)
	{
		QListWidgetItem *pItem = new QListWidgetItem(it->name);
		if (it->bSearch)
			pItem->setCheckState(Qt::Checked);
		else
			pItem->setCheckState(Qt::Unchecked);
		pItem->setToolTip(pItem->text());
		ui.listWidget_searchFilter->addItem(pItem);
	}
}
void GameOftenFunctionWgt::on_pushButton_addSearch_clicked()
{
	QString szText = ui.lineEdit_search->text();
	ui.lineEdit_search->setText("");
	if (szText.isEmpty())
		return;
	QListWidgetItem *pItem = new QListWidgetItem(szText);
	pItem->setCheckState(Qt::Unchecked);
	ui.listWidget_searchFilter->addItem(pItem);
	pItem->setToolTip(pItem->text());
	g_pGameCtrl->setSearchItem(szText, 0);
}

void GameOftenFunctionWgt::doLoadUserConfig(QSettings &iniFile)
{
	initSearchItems(g_pGameCtrl->GetSearchList());
	//initSaleItems(g_pGameCtrl->GetSaleItemList());
	iniFile.beginGroup("AutoNpcAction");
	ui.checkBox_autoSale->setChecked(iniFile.value("AutoSale").toBool());
	ui.checkBox_AutoSupply->setChecked(iniFile.value("AutoSupply").toBool());

	ui.listWidget_sale->clear();
	g_pGameCtrl->ClearSaleItems();
	int nCount = iniFile.value("saleCount").toInt();
	for (int i = 0; i < nCount; ++i)
	{
		QString itemKeyName = QString("item%1").arg(i + 1);
		QString itemKeyVal = QString("checked%1").arg(i + 1);
		QListWidgetItem *pItem = new QListWidgetItem(iniFile.value(itemKeyName).toString());
		pItem->setCheckState(iniFile.value(itemKeyVal).toBool() ? Qt::Checked : Qt::Unchecked);
		pItem->setToolTip(pItem->text());
		ui.listWidget_sale->addItem(pItem);
		g_pGameCtrl->setSaleItemIsChecked(pItem->text(), pItem->checkState() == Qt::Checked);
	}
	iniFile.endGroup();

	ui.listWidget_searchFilter->clear();
	iniFile.beginGroup("AutoSearch");
	ui.groupBox_search->setChecked(iniFile.value("AutoSearch").toBool());
	nCount = iniFile.value("itemCount").toInt();
	for (int i = 0; i < nCount; ++i)
	{
		QString itemKeyName = QString("item%1").arg(i + 1);
		QString itemKeyVal = QString("checked%1").arg(i + 1);
		QListWidgetItem *pItem = new QListWidgetItem(iniFile.value(itemKeyName).toString());
		pItem->setCheckState(iniFile.value(itemKeyVal).toBool() ? Qt::Checked : Qt::Unchecked);
		pItem->setToolTip(pItem->text());
		ui.listWidget_searchFilter->addItem(pItem);
		//	g_pGameCtrl->setSaleItemIsChecked(pItem->text(), pItem->checkState() == Qt::Checked);
	}
	iniFile.endGroup();
}

void GameOftenFunctionWgt::doSearchItemChanged(QListWidgetItem *pItem)
{
	if (pItem == nullptr)
		return;
	QString name = pItem->text();
	if (pItem->checkState() == Qt::Checked)
	{
		g_pGameCtrl->setSearchItem(name, 1);
		if (!m_pSearchNameList.contains(name))
		{
			m_pSearchNameList.append(name);
		}
	}
	else
	{
		g_pGameCtrl->setSearchItem(name, 0);
		m_pSearchNameList.removeAll(name);
	}
	if (name == "探测全部")
	{
		m_bIsSearchAll = (pItem->checkState() == Qt::Checked ? true : false);
	}
	if (name == "宝箱")
	{
		m_bSearchCrate = (pItem->checkState() == Qt::Checked ? true : false);
	}
	if (name == "出入口")
	{
		m_bSearchEntries = (pItem->checkState() == Qt::Checked ? true : false);
	}
}

void GameOftenFunctionWgt::doSaleItemChanged(QListWidgetItem *pItem)
{
	if (pItem == nullptr)
		return;
	QString name = pItem->text();
	if (pItem->checkState() == Qt::Checked)
		g_pGameCtrl->setSaleItemIsChecked(name, 1);
	else
		g_pGameCtrl->setSaleItemIsChecked(name, 0);
}

void GameOftenFunctionWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("AutoNpcAction");
	iniFile.setValue("AutoSale", ui.checkBox_autoSale->isChecked());
	iniFile.setValue("AutoSupply", ui.checkBox_AutoSupply->isChecked());
	int nCount = ui.listWidget_sale->count();
	iniFile.setValue("saleCount", nCount);
	for (int i = 0; i < nCount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget_sale->item(i);
		QString itemKeyName = QString("item%1").arg(i + 1);
		QString itemKeyVal = QString("checked%1").arg(i + 1);
		iniFile.setValue(itemKeyName, pItem->text());
		iniFile.setValue(itemKeyVal, pItem->checkState() == Qt::Checked);
	}
	iniFile.endGroup();

	iniFile.beginGroup("AutoSearch");
	iniFile.setValue("AutoSearch", ui.groupBox_search->isChecked());
	nCount = ui.listWidget_searchFilter->count();
	iniFile.setValue("itemCount", nCount);
	for (int i = 0; i < nCount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget_searchFilter->item(i);
		QString itemKeyName = QString("item%1").arg(i + 1);
		QString itemKeyVal = QString("checked%1").arg(i + 1);
		iniFile.setValue(itemKeyName, pItem->text());
		iniFile.setValue(itemKeyVal, pItem->checkState() == Qt::Checked);
	}
	iniFile.endGroup();
}

void GameOftenFunctionWgt::doLoadJsConfig(QJsonObject &obj)
{
	if (obj.contains("player"))
	{
		QJsonObject playerobj = obj.value("player").toObject();

		if (playerobj.contains("autosupply"))
			ui.checkBox_AutoSupply->setChecked(playerobj.take("autosupply").toBool());
	}
}

void GameOftenFunctionWgt::doSaveJsConfig(QJsonObject &obj)
{
	QJsonObject player = obj.value("player").toObject();
	player.insert("autosupply", ui.checkBox_AutoSupply->isChecked());
	obj.insert("player", player);
}

void GameOftenFunctionWgt::initSaleItems(GameItemList itemList)
{
	ui.listWidget_sale->clear();
	foreach (auto it, itemList)
	{
		QListWidgetItem *pItem = new QListWidgetItem(it->name);
		if (it->isSale)
			pItem->setCheckState(Qt::Checked);
		else
			pItem->setCheckState(Qt::Unchecked);
		pItem->setToolTip(pItem->text());
		ui.listWidget_sale->addItem(pItem);
	}
}

void GameOftenFunctionWgt::on_pushButton_addSale_clicked()
{
	QString szText = ui.lineEdit_sale->text();
	ui.lineEdit_sale->setText("");
	if (szText.isEmpty())
		return;
	QListWidgetItem *pItem = new QListWidgetItem(szText);
	pItem->setCheckState(Qt::Unchecked);
	ui.listWidget_sale->addItem(pItem);
	pItem->setToolTip(pItem->text());
	g_pGameCtrl->setSaleItemIsChecked(szText, 0);
}
void GameOftenFunctionWgt::SwitchMapThread(GameOftenFunctionWgt *pThis, int nDir, int nVal)
{
	g_pGameFun->ForceMoveToEx(nDir, nVal);
}

void GameOftenFunctionWgt::on_pushButton_LeftUp_clicked()
{
	int nVal = ui.spinBox_SwitchMap->value();
	QtConcurrent::run(SwitchMapThread, this, MOVE_DIRECTION_North, nVal);
}

void GameOftenFunctionWgt::on_pushButton_Left_clicked()
{
	int nVal = ui.spinBox_SwitchMap->value();
	QtConcurrent::run(SwitchMapThread, this, MOVE_DIRECTION_NorthWest, nVal);
}

void GameOftenFunctionWgt::on_pushButton_LeftDown_clicked()
{
	int nVal = ui.spinBox_SwitchMap->value();
	QtConcurrent::run(SwitchMapThread, this, MOVE_DIRECTION_West, nVal);
}

void GameOftenFunctionWgt::on_pushButton_Up_clicked()
{
	int nVal = ui.spinBox_SwitchMap->value();
	QtConcurrent::run(SwitchMapThread, this, MOVE_DIRECTION_NorthEast, nVal);
}

void GameOftenFunctionWgt::on_pushButton_RightUp_clicked()
{
	int nVal = ui.spinBox_SwitchMap->value();
	QtConcurrent::run(SwitchMapThread, this, MOVE_DIRECTION_East, nVal);
}

void GameOftenFunctionWgt::on_pushButton_Right_clicked()
{
	int nVal = ui.spinBox_SwitchMap->value();
	QtConcurrent::run(SwitchMapThread, this, MOVE_DIRECTION_SouthEast, nVal);
}

void GameOftenFunctionWgt::on_pushButton_RightDown_clicked()
{
	int nVal = ui.spinBox_SwitchMap->value();
	QtConcurrent::run(SwitchMapThread, this, MOVE_DIRECTION_South, nVal);
}

void GameOftenFunctionWgt::on_pushButton_Down_clicked()
{
	int nVal = ui.spinBox_SwitchMap->value();
	QtConcurrent::run(SwitchMapThread, this, MOVE_DIRECTION_SouthWest, nVal);
}

void GameOftenFunctionWgt::on_pushButton_SwitchMap_clicked()
{
	int x = ui.lineEdit_SwicthMapX->text().toInt();
	int y = ui.lineEdit_SwicthMapY->text().toInt();
	int v = ui.lineEdit_SwicthMapVal->text().toInt();
	g_pGameFun->ForceMoveTo(x, y, true);
}

void GameOftenFunctionWgt::doAddSaleItem(GameItemPtr pItem)
{
	if (!pItem || pItem->name.isEmpty())
		return;
	QListWidgetItem *pTableItem = new QListWidgetItem(pItem->name);
	pTableItem->setCheckState(Qt::Unchecked);
	ui.listWidget_sale->addItem(pTableItem);
	pTableItem->setToolTip(pTableItem->text());
}

void GameOftenFunctionWgt::doAddSeachItem(GameItemPtr pItem)
{
}
//勾选后，实时刷新周围信息
void GameOftenFunctionWgt::OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
	//1、进行筛选，如果勾选，显示，否则不显示
	//2、如果勾选了，看列表是否已经显示，如果已显示，更新位置信息和颜色，无效时候，置灰
	if (!m_bEnableSearch)
		return;
	if ((GetTickCount() - m_lastTime) < 2000) //2秒更新一次
		return;
	m_lastTime = GetTickCount();
	if (coll && coll->xsize && coll->ysize)
	{
		QPoint curPos = g_pGameFun->GetMapCoordinate();
		QString szItemInfo;
		QString szDir;
		QString curMapName = g_pGameFun->GetMapName();
		if (m_bIsSearchAll)
		{
			for (int i = 0; i < units->size(); ++i)
			{
				szItemInfo.clear();
				szDir = g_pGameFun->GetDirectionText(g_pGameFun->GetDirection(curPos.x(), curPos.y(), units->at(i).xpos, units->at(i).ypos));
				szDir = QString("%1 %2").arg(curMapName).arg(szDir);
				if (units->at(i).valid && units->at(i).type == 2 && units->at(i).model_id != 0 && (units->at(i).flags & 1024) != 0) //物品
				{
					szItemInfo = QString("%1 %2（%3,%4）").arg(szDir).arg(units->at(i).item_name).arg(units->at(i).xpos).arg(units->at(i).ypos);
				}
				else if (judgeIsEntries(units->at(i)))
				{
					szItemInfo = QString("%1 %2（%3,%4）").arg(szDir).arg("出入口").arg(units->at(i).xpos).arg(units->at(i).ypos);
				}
				else
				{
					szItemInfo = QString("%1 %2（%3,%4）").arg(szDir).arg(units->at(i).unit_name).arg(units->at(i).xpos).arg(units->at(i).ypos);
				}
				//		qDebug() << units->at(i).valid << units->at(i).type << units->at(i).level << units->at(i).flags << units->at(i).model_id << units->at(i).unit_id << units->at(i).unit_name << units->at(i).nick_name << units->at(i).title_name << units->at(i).item_name << units->at(i).xpos << units->at(i).ypos;
				ui.textEdit->append(szItemInfo);
				ui.textEdit->moveCursor(QTextCursor::End);
			}
		}
		else
		{
			for (int i = 0; i < units->size(); ++i)
			{
				//			qDebug() << units->at(i).valid << units->at(i).type << units->at(i).level << units->at(i).flags << units->at(i).model_id << units->at(i).unit_id << units->at(i).unit_name << units->at(i).nick_name << units->at(i).title_name << units->at(i).item_name << units->at(i).xpos << units->at(i).ypos;
				if (m_bSearchCrate)
				{
					if (judgeIsCrate(units->at(i)))
					{
						szItemInfo.clear();
						szDir = g_pGameFun->GetDirectionText(g_pGameFun->GetDirection(curPos.x(), curPos.y(), units->at(i).xpos, units->at(i).ypos));
						szDir = QString("%1 %2").arg(curMapName).arg(szDir);

						szItemInfo = QString("%1 %2（%3,%4）").arg(szDir).arg(units->at(i).item_name).arg(units->at(i).xpos).arg(units->at(i).ypos);
						ui.textEdit->append(szItemInfo);
						ui.textEdit->moveCursor(QTextCursor::End);
						continue;
					}
				}
				if (m_bSearchEntries)
				{
					if (judgeIsEntries(units->at(i)))
					{
						szItemInfo.clear();
						szDir = g_pGameFun->GetDirectionText(g_pGameFun->GetDirection(curPos.x(), curPos.y(), units->at(i).xpos, units->at(i).ypos));
						szDir = QString("%1 %2").arg(curMapName).arg(szDir);

						szItemInfo = QString("%1 %2（%3,%4）").arg(szDir).arg("出入口").arg(units->at(i).xpos).arg(units->at(i).ypos);
						ui.textEdit->append(szItemInfo);
						ui.textEdit->moveCursor(QTextCursor::End);
						continue;
					}
				}
				for (auto searchItem : m_pSearchNameList)
				{
					if (units->at(i).unit_name.contains(searchItem) || units->at(i).item_name.contains(searchItem))
					{

						szItemInfo.clear();
						szDir = g_pGameFun->GetDirectionText(g_pGameFun->GetDirection(curPos.x(), curPos.y(), units->at(i).xpos, units->at(i).ypos));
						szDir = QString("%1 %2").arg(curMapName).arg(szDir);

						if (units->at(i).valid && units->at(i).type == 2 && units->at(i).model_id != 0 && (units->at(i).flags & 1024) != 0) //物品
						{
							szItemInfo = QString("%1 %2（%3,%4）").arg(szDir).arg(units->at(i).item_name).arg(units->at(i).xpos).arg(units->at(i).ypos);
						}
						else
						{
							szItemInfo = QString("%1 %2（%3,%4）").arg(szDir).arg(units->at(i).unit_name).arg(units->at(i).xpos).arg(units->at(i).ypos);
						}
						ui.textEdit->append(szItemInfo);
						ui.textEdit->moveCursor(QTextCursor::End);
						break;
					}
				}
				//if(!m_pSearchNameList.contains(units->at(i).unit_name) && !m_pSearchNameList.contains(units->at(i).item_name))//NPC
				//	continue;
			}
		}
	}
}

bool GameOftenFunctionWgt::judgeIsCrate(CGA_MapUnit_t u)
{
	if (u.valid == 2 && u.type == 1 && u.model_id != 0 && (u.flags & 4096) != 0 && u.item_name.contains("宝箱"))
		return true;
	return false;
}
bool GameOftenFunctionWgt::judgeIsEntries(CGA_MapUnit_t u)
{
	if (u.valid == 2 && u.type == 1 /* && u.model_id == 0*/ && (u.flags == 4096) && (u.unit_name == " " || u.unit_name.isEmpty()))
		return true;
	return false;
}

void GameOftenFunctionWgt::on_groupBox_search_clicked()
{
	m_bEnableSearch = ui.groupBox_search->isChecked();
}

void GameOftenFunctionWgt::on_groupBox_autoTalk_toggled(bool checked)
{
	g_pGameCtrl->OnSetAutoTalkNpc(checked);
}

void GameOftenFunctionWgt::on_radioButton_talkNo_clicked(bool checked)
{
	g_pGameCtrl->OnSetAutoTalkNpcYesOrNo(!checked);
}

void GameOftenFunctionWgt::on_radioButton_talkYes_clicked(bool checked)
{
	g_pGameCtrl->OnSetAutoTalkNpcYesOrNo(checked);
}
