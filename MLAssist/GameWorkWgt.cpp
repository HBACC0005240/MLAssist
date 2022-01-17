#include "GameWorkWgt.h"
#include "stdafx.h"
//#include "gameservice.h"
GameWorkWgt::GameWorkWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl, SIGNAL(signal_activeGameFZ()), this, SLOT(Active()));
	connect(g_pGameCtrl, &GameCtrl::NotifyGameCompoundInfo, this, &GameWorkWgt::doUpdateCompoundList, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGameItemsInfo, this, &GameWorkWgt::doUpdateItemList, Qt::ConnectionType::QueuedConnection);

	connect(g_pGameFun, SIGNAL(signal_workStart()), this, SLOT(doStartProgress()));
	connect(g_pGameFun, SIGNAL(signal_workEnd()), this, SLOT(doFiniProgress()));
	connect(g_pGameFun, SIGNAL(signal_workThreadEnd(const QString &)), this, SLOT(doWorkThreadFini(const QString &)));
	//ui.pushButton_arrange->hide();
	m_szPickSkillList << "鉴定"
					  << "挖矿"
					  << "伐木"
					  << "狩猎"
					  << "修理武器"
					  << "修理防具"
					  << "挖矿体验"
					  << "伐木体验"
					  << "狩猎体验";
	ui.tableWidget->horizontalHeader()->setStyleSheet("font:bold;");
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
	//	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	//	ui.tableWidget->horizontalHeader()->setFixedHeight(30);
	//ui.tableWidget->setColumnWidth(1, 130);
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(15);
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); //打开右键功能
	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_customContextMenu(const QPoint &)));
	connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(doTableCellClicked(int, int)));
	connect(&m_progressTimer, SIGNAL(timeout()), this, SLOT(doUpdateProgress()));
	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(doShowMaterialInfo(QListWidgetItem *)));
	for (int i = 0; i < 4; ++i)
	{
		for (size_t n = 0; n < 5; n++)
		{
			QTableWidgetItem *pItem = new QTableWidgetItem();
			ui.tableWidget->setItem(i, n, pItem);
		}
	}

	m_workTypeForText.insert(TWork_Identify, "鉴定");
	m_workTypeForText.insert(TWork_Digging, "挖掘");
	m_workTypeForText.insert(TWork_Hunt, "狩猎");
	m_workTypeForText.insert(TWork_Lumber, "伐木");
	m_workTypeForText.insert(TWork_RepairWeapon, "修理武器");
	m_workTypeForText.insert(TWork_RepairArmor, "修理防具");

	m_workTypeForTime.insert(TWork_Identify, 6000);
	m_workTypeForTime.insert(TWork_Digging, 6000);
	m_workTypeForTime.insert(TWork_Hunt, 6000);
	m_workTypeForTime.insert(TWork_Lumber, 6000);
	m_workTypeForTime.insert(TWork_RepairWeapon, 6000);
	m_workTypeForTime.insert(TWork_RepairArmor, 6000);
	//QTimer *timer = new QTimer(this);
	//connect(timer, SIGNAL(timeout()), this, SLOT(OnQueueQueryProcess()));
	//timer->start(15000);//15秒取一次合成数据
}

GameWorkWgt::~GameWorkWgt()
{
}

void GameWorkWgt::initSkillComboBox()
{
	//ui.comboBox_skill->clear();
	////筛选 采集：挖矿、伐木、狩猎、挖矿体验、伐木体验、狩猎体验
	//QMap<int, QString> indexForName;
	//for (int i=0;i<15;++i)//先10个  15个技能栏
	//{
	//	DWORD pAddress = 0x00D84FEC;
	//	DWORD offset = i * 0x49FC;
	//	pAddress += offset;
	//	QString skillName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 100)));
	//	pAddress += 0x38;
	//	int nShowIndex = i;//YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pAddress);
	//	if (m_szPickSkillList.contains(skillName))
	//	{
	//		indexForName.insert(nShowIndex, skillName);
	//	}
	//}
	//for (auto it=indexForName.begin();it!=indexForName.end();++it )
	//{
	//	ui.comboBox_skill->addItem(it.value(),it.key());
	//}
}

void GameWorkWgt::initSkillComboBox2()
{
	//ui.comboBox_skill->clear();
	////筛选 采集：挖矿、伐木、狩猎、挖矿体验、伐木体验、狩猎体验
	//QMap<int, QString> indexForName;
	//auto pSkillList = g_pGameCtrl->getGameSkills();
	//for (auto pSkill:pSkillList)
	//{
	//	if (m_szPickSkillList.contains(pSkill->name))
	//	{
	//		ui.comboBox_skill->addItem(pSkill->name,(int)pSkill);
	//	}
	//}
}
void GameWorkWgt::on_pushButton_useSkill_clicked()
{
	/*QString skillName = ui.comboBox_skill->currentText();
	int nCurIndex = ui.comboBox_skill->itemData(ui.comboBox_skill->currentIndex()).toInt();*/
	//	SendMessageA(GameData::getInstance().getGameHwnd(), WM_CGA_START_WORK, nCurIndex, 0);
	//	GameData::getInstance().UI_OpenGatherDialog(nCurIndex, 0);
	//
	//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GameData::getInstance().getGamePID());
	//	int nTestArg[2];
	//	nTestArg[0] = nCurIndex;
	//	nTestArg[1] = 0;
	//	LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(nTestArg), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);
	//	DWORD byWrite;
	//	bool bRet = WriteProcessMemory(hProcess, callArg, nTestArg, sizeof(nTestArg), &byWrite);
	//	if (bRet == false)
	//	{
	//	//	std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
	//		VirtualFreeEx(hProcess, callArg, sizeof(nTestArg), MEM_DECOMMIT);
	//		CloseHandle(hProcess);
	//		return;
	//	}
	//	DWORD tId;
	//	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GameData::getInstance().UI_OpenGatherDialog, callArg, 0, &tId);
	//	if (hRemoteThread == nullptr)
	//	{
	//		printf("提示：远程调用代码失败\n");
	//	//	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
	//		CloseHandle(hRemoteThread);
	//		CloseHandle(hProcess);
	//		return;
	//	}
	//	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
	////	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
	//	CloseHandle(hRemoteThread);
	//	CloseHandle(hProcess);

	//	GameData::getInstance().Work(skillName);
}

void GameWorkWgt::Active()
{
	initSkillComboBox();
}

void GameWorkWgt::GetRowColFromItemPos(int itempos, int &row, int &col)
{
	row = itempos / 5;
	col = itempos % 5;
}

QString GameWorkWgt::GetItemShowText(GameItemPtr pItem)
{
	if (pItem == nullptr)
		return "";
	switch (m_nType)
	{
		case SHOW_Name: return QString("%1").arg(pItem->assessed ? pItem->name : pItem->maybeName); //pItem->name;
		case SHOW_NameCount: return QString("%1&%2").arg(pItem->name).arg(pItem->count);
		case SHOW_Count: return QString::number(pItem->count);
		case SHOW_Type: return QString("%1&%2").arg(pItem->assessed ? pItem->name : pItem->maybeName).arg(pItem->type); // QString::number(pItem->type);
		case SHOW_Code: return QString::number(pItem->id);
		default:
			break;
	}
	return "";
}

void GameWorkWgt::StartWorkThread(GameWorkWgt *pThis)
{
	qDebug() << "Work Thread" << pThis->m_nCurrentWorkSkillIndex << pThis->m_nCurrentWorkDelayTime;
	while (!pThis->m_bStopWork)
	{
		qDebug() << "Next Work";
		g_pGameFun->Work(pThis->m_nCurrentWorkSkillIndex, 0, pThis->m_nCurrentWorkDelayTime);
		Sleep(pThis->m_nCurrentWorkDelayTime);
	}
}
void GameWorkWgt::ResetWorkUI()
{
	ui.pushButton_digging->setText("高速挖掘");
	ui.pushButton_digging->setChecked(false);
	ui.pushButton_hunt->setText("高速狩猎");
	ui.pushButton_hunt->setChecked(false);

	ui.pushButton_lumber->setText("高速伐木");
	ui.pushButton_lumber->setChecked(false);

	ui.pushButton_compound->setText("全合成");
	ui.pushButton_compound->setChecked(false);

	ui.pushButton_Identify->setText("全鉴定");
	ui.pushButton_Identify->setChecked(false);

	ui.pushButton_RepairWeapon->setText("全武修");
	ui.pushButton_RepairWeapon->setChecked(false);

	ui.pushButton_RepairArmor->setText("全防修");
	ui.pushButton_RepairArmor->setChecked(false);
}

void GameWorkWgt::doUpdateItemList(GameItemList pItemList)
{
	int row = 0, col = 0;
	QVector<int> itemPosList;
	for (size_t i = 0; i < pItemList.size(); i++)
	{
		auto pItem = pItemList[i];
		if (pItem->pos < 8)
			continue;
		itemPosList.append(pItem->pos);
		GetRowColFromItemPos(pItem->pos - 8, row, col);
		QTableWidgetItem *pTableItem = ui.tableWidget->item(row, col);
		if (pTableItem == nullptr)
			continue;
		auto userData = pTableItem->data(Qt::UserRole).value<GameItemPtr>();
		if (pItem->exist)
		{
			QString szText = GetItemShowText(pItem);
			if (szText != pTableItem->text() /*|| userData != pItem*/)
			{
				//				pTableItem->setToolTip(QString("%1 \n%2\n等级 %3\n%4\n种类 %5").arg(pItem->name).arg(pItem->attr).arg(pItem->level).arg(pItem->type).arg(pItem->info));
				QString szToolTip = QString("%1 \n%2\n%5").arg(pItem->assessed ? pItem->name : pItem->maybeName).arg(pItem->attr).arg(pItem->info);
				szToolTip = szToolTip.remove("$0");
				szToolTip = szToolTip.remove("$1");
				szToolTip = szToolTip.remove("$2");
				szToolTip = szToolTip.remove("$3");
				szToolTip = szToolTip.remove("$4");
				pTableItem->setToolTip(szToolTip);

				pTableItem->setText(szText);
				if (pItem->assessed)
					pTableItem->setTextColor(QColor(0, 0, 0));
				else
					pTableItem->setTextColor(QColor(0, 0, 255));
				pTableItem->setData(Qt::UserRole, QVariant::fromValue(pItem));
				//	qDebug() << "Exist Update" << pItem->name << pTableItem->text() << i << pItem->pos << pItem->attr << pItem->id;
			}
		}
		else
		{
			if ("" != pTableItem->text())
			{
				//	pTableItem->setToolTip();
				pTableItem->setText("");
				pTableItem->setData(Qt::UserRole, 0);
				//	qDebug() << "NoExist Update" << pItem->name << pTableItem->text() << i << pItem->pos;
			}
		}
	}
	for (int i = 8; i < 28; ++i)
	{
		if (!itemPosList.contains(i))
		{
			GetRowColFromItemPos(i - 8, row, col);
			QTableWidgetItem *pTableItem = ui.tableWidget->item(row, col);
			if (pTableItem == nullptr)
				continue;
			if ("" != pTableItem->text())
			{
				pTableItem->setText("");
				pTableItem->setData(Qt::UserRole, 0);
			}
		}
	}
}

void GameWorkWgt::on_radioButton_name_clicked()
{
	if (ui.radioButton_name->isChecked())
		m_nType = SHOW_Name;
}

//void GameWorkWgt::on_radioButton_count_clicked()
//{
//	if (ui.radioButton_count->isChecked())
//		m_nType = SHOW_Count;
//}

void GameWorkWgt::on_radioButton_code_clicked()
{
	if (ui.radioButton_code->isChecked())
		m_nType = SHOW_Code;
}

void GameWorkWgt::on_radioButton_nameCount_clicked()
{
	if (ui.radioButton_nameCount->isChecked())
		m_nType = SHOW_NameCount;
}

void GameWorkWgt::on_radioButton_type_clicked()
{
	if (ui.radioButton_type->isChecked())
		m_nType = SHOW_Type;
}

void GameWorkWgt::on_customContextMenu(const QPoint &pos)
{
	QTableWidgetItem *tempitem = ui.tableWidget->itemAt(pos);
	if (tempitem != NULL)
	{
		QMenu menu;
		menu.addAction(QString("使用物品"), this, SLOT(doUseItem()));
		menu.addAction(QString("添加到售卖列表"), this, SLOT(doAddSaleList()));
		menu.addAction(QString("添加到堆叠列表"), this, SLOT(doAddDieList()));
		menu.addAction(QString("添加到丢弃列表"), this, SLOT(doAddRenList()));
		menu.addAction(QString("添加到代码丢弃"), this, SLOT(doAddRenCodeList()));

		menu.addAction(QString("丢弃"), this, SLOT(doDropItem()));
		menu.exec(QCursor::pos());
	}
}
void GameWorkWgt::doAddRenList()
{
	QTableWidgetItem *pSelectItem = ui.tableWidget->currentItem();
	if (pSelectItem)
	{
		GameItemPtr pItem = (GameItemPtr)pSelectItem->data(Qt::UserRole).value<GameItemPtr>();
		if (pItem)
		{
			g_pGameCtrl->signal_addRenItem(pItem, false);
		}
	}
}

void GameWorkWgt::doAddRenCodeList()
{
	QTableWidgetItem *pSelectItem = ui.tableWidget->currentItem();
	if (pSelectItem)
	{
		GameItemPtr pItem = (GameItemPtr)pSelectItem->data(Qt::UserRole).value<GameItemPtr>();
		if (pItem)
		{
			g_pGameCtrl->signal_addRenItem(pItem, true);
		}
	}
}

void GameWorkWgt::doAddSaleList()
{
	QTableWidgetItem *pSelectItem = ui.tableWidget->currentItem();
	if (pSelectItem)
	{
		GameItemPtr pItem = (GameItemPtr)pSelectItem->data(Qt::UserRole).value<GameItemPtr>();
		if (pItem)
		{
			g_pGameCtrl->signal_addSaleItem(pItem);
		}
	}
}

void GameWorkWgt::doAddDieList()
{
	QTableWidgetItem *pSelectItem = ui.tableWidget->currentItem();
	if (pSelectItem)
	{
		GameItemPtr pItem = pSelectItem->data(Qt::UserRole).value<GameItemPtr>();
		if (pItem)
		{
			g_pGameCtrl->signal_addDieItem(pItem);
		}
	}
}
void GameWorkWgt::doDropItem()
{
	QTableWidgetItem *pSelectItem = ui.tableWidget->currentItem();
	if (pSelectItem)
	{
		GameItemPtr pItem = pSelectItem->data(Qt::UserRole).value<GameItemPtr>();
		if (pItem)
		{
			g_pGameCtrl->RenItem(pItem);
		}
	}
}

void GameWorkWgt::doUseItem()
{
	QTableWidgetItem *pSelectItem = ui.tableWidget->currentItem();
	if (pSelectItem)
	{
		GameItemPtr pItem = (GameItemPtr)pSelectItem->data(Qt::UserRole).value<GameItemPtr>();
		if (pItem)
		{
			g_pGameFun->UseItemPos(pItem->pos);
		}
	}
}

void GameWorkWgt::doTableCellClicked(int nRow, int nCol)
{
	QTableWidgetItem *pSelectItem = ui.tableWidget->item(nRow, nCol);
	if (pSelectItem)
	{
		GameItemPtr pItem = (GameItemPtr)pSelectItem->data(Qt::UserRole).value<GameItemPtr>();
		if (pItem)
		{
			if (pItem->assessed)
				ui.label_itemData->setText(QString("%1 代码:%2 数量:%3 类型:%4").arg(pItem->name).arg(pItem->id).arg(pItem->count).arg(pItem->type));
			else
				ui.label_itemData->setText(QString("%1 代码:%2 数量:%3 类型:%4").arg(pItem->maybeName).arg(pItem->id).arg(pItem->count).arg(pItem->type));
		}
	}
}

void GameWorkWgt::doLoadUserConfig(QSettings &iniFile)
{
}

void GameWorkWgt::doSaveUserConfig(QSettings &iniFile)
{
}

void GameWorkWgt::on_pushButton_compound_clicked(bool checked)
{
	if (checked)
	{
		ui.pushButton_compound->setText("停止合成");
		m_nCurrentWorkType = TWork_Compound;
		StartWork();
	}
	else
	{
		ResetWorkText();
	}
}

void GameWorkWgt::on_pushButton_Identify_clicked(bool checked)
{
	//g_pGameFun->AssessAllItem();
	if (checked)
	{
		ui.pushButton_Identify->setText("停止鉴定");
		m_nCurrentWorkType = TWork_Identify;
		StartWork();
	}
	else
	{
		ResetWorkText();
	}
}

void GameWorkWgt::on_pushButton_hunt_clicked(bool checked)
{
	if (checked)
	{
		ui.pushButton_hunt->setText("停止打猎");
		m_nCurrentWorkType = TWork_Hunt;
		StartWork();
	}
	else
	{
		ResetWorkText();
	}
}

void GameWorkWgt::on_pushButton_digging_clicked(bool checked)
{
	if (checked)
	{
		ui.pushButton_digging->setText("停止挖掘");
		m_nCurrentWorkType = TWork_Digging;
		StartWork();
	}
	else
	{
		ResetWorkText();
	}
}

void GameWorkWgt::on_pushButton_lumber_clicked(bool checked)
{
	if (checked)
	{
		ui.pushButton_lumber->setText("停止伐木");
		m_nCurrentWorkType = TWork_Lumber;
		StartWork();
	}
	else
	{
		ResetWorkText();
	}
}

void GameWorkWgt::on_pushButton_RepairArmor_clicked(bool checked)
{
	if (checked)
	{
		ui.pushButton_RepairArmor->setText("停止修理");
		m_nCurrentWorkType = TWork_RepairArmor;
		StartWork();
	}
	else
	{
		ResetWorkText();
	}
}

void GameWorkWgt::on_pushButton_RepairWeapon_clicked(bool checked)
{
	if (checked)
	{
		ui.pushButton_RepairWeapon->setText("停止修理");
		m_nCurrentWorkType = TWork_RepairWeapon;
		StartWork();
	}
	else
	{
		ResetWorkText();
	}
}

void GameWorkWgt::ResetWorkText()
{
	m_bStopWork = true;
	g_pGameFun->StopWork();
	ResetWorkUI();
	m_nCurrentWorkType = TWork_None;
}

void GameWorkWgt::StartWork()
{
	m_bStopWork = false;
	if (m_nCurrentWorkType != TWork_Compound)
	{
		QString skillName = m_workTypeForText.value(m_nCurrentWorkType);
		m_nCurrentWorkSkillIndex = g_pGameFun->FindPlayerSkill(skillName);
		if (m_nCurrentWorkSkillIndex < 0)
		{
			ResetWorkText();
			QMessageBox::information(this, "提示：", QString("没有%1技能").arg(skillName), "确定");
			return;
		}
	}
	
	int ntime = m_workTypeForTime.value(m_nCurrentWorkType);
	//	ntime += 4500;	//内部会-4500  这里加上
	m_nCurrentWorkDelayTime = ntime;
	//g_pGameCtrl->OnSetWorkDelayTime(ntime);	//采集时间设置 同步到系统
	ui.progressBar_collection->setRange(0, m_nCurrentWorkDelayTime / 1000);
	ui.progressBar_collection->setValue(0);

	g_pGameFun->StartWork(m_nCurrentWorkType, m_nCurrentWorkDelayTime, m_sCurrentCompoundName);
	//	QtConcurrent::run(StartWorkThread, this);
}

void GameWorkWgt::on_lineEdit_compound_editingFinished()
{
	m_workTypeForTime.insert(TWork_Compound, ui.lineEdit_compound->text().toInt());
}

void GameWorkWgt::on_lineEdit_digging_editingFinished()
{
	m_workTypeForTime.insert(TWork_Digging, ui.lineEdit_digging->text().toInt());
}

void GameWorkWgt::on_lineEdit_hunt_editingFinished()
{
	qDebug() << "狩猎延时" << ui.lineEdit_hunt->text().toInt();
	m_workTypeForTime.insert(TWork_Hunt, ui.lineEdit_hunt->text().toInt());
	qDebug() << "狩猎延时" << m_workTypeForTime.value(TWork_Hunt);
}

void GameWorkWgt::on_lineEdit_lumber_editingFinished()
{
	m_workTypeForTime.insert(TWork_Lumber, ui.lineEdit_lumber->text().toInt());
}

void GameWorkWgt::doStartProgress()
{
	m_progressTimer.start(1000);
	ui.progressBar_collection->setValue(0);
}

void GameWorkWgt::doFiniProgress()
{
	m_progressTimer.stop();
	ui.progressBar_collection->setValue(ui.progressBar_collection->maximum());
}

void GameWorkWgt::doUpdateProgress()
{
	int nVal = ui.progressBar_collection->value();
	nVal += 1;
	ui.progressBar_collection->setValue(nVal);
}
//手动获取吧
void GameWorkWgt::doUpdateCompoundList(GameCompoundList compound)
{
	if (ui.listWidget->count() > 0)
		return;
	ui.listWidget->clear();
	GameCompoundList pCompoundList = compound;
	for (auto pCoupound : pCompoundList)
	{
		auto pItemList = ui.listWidget->findItems(pCoupound->name, Qt::MatchCaseSensitive);
		if (pCoupound->available && pItemList.size() < 1) //没有就新增
		{
			QListWidgetItem *pItem = new QListWidgetItem(pCoupound->name, ui.listWidget);
			pItem->setData(Qt::UserRole, QVariant::fromValue<GameCompoundPtr>(pCoupound));
		}
	}
}

void GameWorkWgt::doShowMaterialInfo(QListWidgetItem *pItem)
{
	if (!pItem)
		return;
	GameCompoundPtr pCompound = (GameCompoundPtr)pItem->data(Qt::UserRole).value<GameCompoundPtr>();
	if (!pCompound)
		return;
	ui.textEdit->clear();
	QString sText = QString("<font color=\"#0000FF\">%1</font><br>").arg("所需材料");
	ui.textEdit->insertHtml(sText);
	m_sCurrentCompoundName = pCompound->name;
	qDebug() << m_sCurrentCompoundName;
	for (int i = 0; i < 5; ++i)
	{
		if (pCompound->materials[i].name.isEmpty())
		{
			continue;
		}
		QString materialText = QString("<font color=\"#0000FF\">%1: %2</font><br>").arg(pCompound->materials[i].name).arg(pCompound->materials[i].count);
		//sText.append(materialText);
		ui.textEdit->insertHtml(materialText);
	}
	//ui.textEdit->insertHtml();
	//ui.textEdit->setPlainText(sText);
}

void GameWorkWgt::doWorkThreadFini(const QString &sMsg)
{
	m_bStopWork = true;
	ResetWorkUI();
	m_nCurrentWorkType = TWork_None;
	if (!sMsg.isEmpty())
	{
		ui.label_itemData->setText(sMsg);
	}
}
bool GameWorkWgt::SortItem(QSharedPointer<GameItem> p1, QSharedPointer<GameItem> p2)
{
	//if (p1 && p2)
	//{
	//	return p1->id > p2->id ? true : false;
	//}
	//return false;
	bool bRet = false;
	if (p1 && p2)
	{
		if (p1->id > p2->id)
		{
			g_CGAInterface->MoveItem(p1->pos, p2->pos, -1, bRet);
			qDebug() << p1->id << p2->id << p1->pos << p2->pos << bRet;
		}
		return p1->id > p2->id ? true : false;
	}
	return false;
}
//一键整理
void GameWorkWgt::on_pushButton_arrange_clicked()
{
	auto pItemList = g_pGameFun->GetGameItems();
	QList<QSharedPointer<GameItem> > sortItemList;
	QList<int> sortIdList;
	QMap<int, QList<QSharedPointer<GameItem> > > idMapItem;
	for (auto pItem : pItemList)
	{
		if (pItem->exist && pItem->pos >= 8)
		{
			QSharedPointer<GameItem> newItem(new GameItem);
			newItem->id = pItem->id;
			newItem->pos = pItem->pos;
			sortItemList.append(newItem);
			if (!sortIdList.contains(pItem->id))
			{
				sortIdList.append(pItem->id);
				QList<QSharedPointer<GameItem> > tmpSortItemList;
				tmpSortItemList.append(newItem);
				idMapItem.insert(pItem->id, tmpSortItemList);
			}
			else
			{
				auto tmpSortItemList = idMapItem.value(pItem->id);
				tmpSortItemList.append(newItem);
				idMapItem.insert(pItem->id, tmpSortItemList);
			}
		}
	}
	qSort(sortIdList.begin(), sortIdList.end(), [&](const int &a, const int &b)
			{ return a > b; });
	//qSort(sortIdList.begin(), sortIdList.end());
	QList<int> itemIndexMgr; //空位列表
	qDebug() << sortIdList;
	qDebug() << idMapItem;
	//貌似map本来就顺序 先不管了
	bool bRet = false;
	for (int i = 0; i < sortIdList.size(); ++i)
	{
		auto tmpSortItemList = idMapItem.value(sortIdList[i]);
		for (auto tmpSortItem : tmpSortItemList)
		{
			//for (int n = 8; n < 28; ++n)
			for (int n = 27; n >= 8; --n)
			{
				if (!itemIndexMgr.contains(n))
				{ //分配位置
					g_CGAInterface->MoveItem(tmpSortItem->pos, n, -1, bRet);
					itemIndexMgr.append(n);
					//原有位置给赋值
					for (auto pTmpSortItem : sortItemList)
					{
						if (pTmpSortItem->pos == n)
						{
							pTmpSortItem->pos = tmpSortItem->pos;
							break;
						}
					}
					tmpSortItem->pos = n;
					break;
				}
			}
		}
	}
	qDebug() << itemIndexMgr;
	for (auto pItem : sortItemList)
	{
		qDebug() << pItem->id << pItem->pos;
	}
}

void GameWorkWgt::on_pushButton_pullCompound_clicked()
{
	ui.listWidget->clear();
}
