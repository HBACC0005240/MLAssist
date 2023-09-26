#include "GameSysCfgWgt.h"
#include "CGFunction.h"
#include "CustomQuickKeyDlg.h"
#include "GameCtrl.h"
GameSysCfgWgt::GameSysCfgWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	init();
}

GameSysCfgWgt::~GameSysCfgWgt()
{
}

void GameSysCfgWgt::init()
{
	QMap<int, QLineEdit *> pKeyValForLineEdit;
	pKeyValForLineEdit.insert(TDef_Quick_Key_Logback, ui.lineEdit_logback);
	pKeyValForLineEdit.insert(TDef_Quick_Key_Logout, ui.lineEdit_logout);
	pKeyValForLineEdit.insert(TDef_Quick_Key_Cross, ui.lineEdit_cross);
	pKeyValForLineEdit.insert(TDef_Quick_Key_Trade, ui.lineEdit_trade);
	pKeyValForLineEdit.insert(TDef_Quick_Key_TradeNoAccept, ui.lineEdit_tradeNoAccept);
	pKeyValForLineEdit.insert(TDef_Quick_Key_SaveAll, ui.lineEdit_saveAll);
	pKeyValForLineEdit.insert(TDef_Quick_Key_FetchAll, ui.lineEdit_fetchAll);
	/*pKeyValForLineEdit.insert(TDef_Quick_Key_CallFz, ui.lineEdit_logback);
	pKeyValForLineEdit.insert(TDef_Quick_Key_Encounter, ui.lineEdit_logback);*/
	auto quickKey = g_pGameCtrl->GetQuickKeyMap();
	for (auto it = quickKey.begin(); it != quickKey.end(); ++it)
	{
		auto pEdit = pKeyValForLineEdit.value(it.key());
		if (pEdit)
		{
			pEdit->setText(QKeySequence(g_pGameCtrl->qNativeKeycode(it.value())).toString());
		}
	}
}
bool GameSysCfgWgt::GetInputKey(QString &inputKey)
{
	CustomQuickKeyDlg dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		inputKey = dlg.GetInputKey();
		qDebug() << inputKey;
		return true;
	}
	return false;
}

void GameSysCfgWgt::doLoadUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("SysConfig");
	ui.checkBox_autoSaveBank->setChecked(iniFile.value("uploadBankData").toBool());
	int quickCount = iniFile.value("keyCount").toInt();
	for (int i = 0; i < quickCount; ++i)
	{
		int type = iniFile.value(QString("keyType-%1").arg(i + 1)).toInt();
		QString key = iniFile.value(QString("keyVal-%1").arg(i + 1)).toString();
		g_pGameCtrl->SetQuickKey(type, key);
	}
	iniFile.endGroup();
}

void GameSysCfgWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("SysConfig");
	iniFile.setValue("uploadBankData", ui.checkBox_autoSaveBank->isChecked());

	auto quickKey = g_pGameCtrl->GetQuickKeyMap();
	int nCount = 0;
	iniFile.setValue("keyCount", quickKey.size());
	for (auto it = quickKey.begin(); it != quickKey.end(); ++it)
	{
		nCount++;
		int type = it.key();
		QString shortKey = QKeySequence(g_pGameCtrl->qNativeKeycode(it.value())).toString();
		iniFile.setValue(QString("keyType-%1").arg(nCount), type);
		iniFile.setValue(QString("keyVal-%1").arg(nCount), shortKey);
	}
	iniFile.endGroup();
}

void GameSysCfgWgt::on_pushButton_fetchItem_clicked()
{
	QString sItems = ui.lineEdit_fetchItem->text();
	QStringList sItemList;
	if (sItems.contains("|"))
	{
		sItemList = sItems.split("|");
	}
	else
		sItemList << sItems;
	//这里重新实现把，否则服务器刷新不过来，速度提不起来
	//for (auto sItem:sItemList)
	//{
	//	g_pGameFun->WithdrawItemAll(sItem);
	//	Sleep(1000);	//等数据包到达服务器，否则取东西位置会重复
	//}
	bool bSensitive = ui.checkBox_fetchItem_Sensitive->isChecked();
	QList<CGA::cga_item_info_t> filterBankInfos;
	CGA::cga_items_info_t bankInfos;
	g_CGAInterface->GetBankItemsInfo(bankInfos);
	if (bSensitive)
	{
		for (auto sItem : sItemList)
		{
			for (int i = 0; i < bankInfos.size(); i++)
			{
				CGA::cga_item_info_t itemInfo = bankInfos.at(i);
				if (itemInfo.name == sItem.toStdString() /*|| itemInfo.itemid == sItem.toInt()*/) //银行id 返回的是空这里不判断id
				{
					filterBankInfos.push_back(itemInfo);
				}
			}
		}
	}
	else
	{
		for (auto sItem : sItemList)
		{
			for (int i = 0; i < bankInfos.size(); i++)
			{
				CGA::cga_item_info_t itemInfo = bankInfos.at(i);
				if (QString::fromStdString(itemInfo.name).contains(sItem) /*|| itemInfo.itemid == sItem.toInt()*/) //银行id 返回的是空这里不判断id
				{
					filterBankInfos.push_back(itemInfo);
				}
			}
		}
	}

	//找到指定物品 取出
	if (filterBankInfos.size() > 0)
	{
		CGA::cga_items_info_t itemsinfo;
		g_CGAInterface->GetItemsInfo(itemsinfo);
		QList<int> existPosList;
		for (int i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
			existPosList.append(iteminfo.pos);
		}
		int nCount = 0;
		int bankIndex = 0;
		bool bRes = false;
		for (int i = 8; i < 28; ++i)
		{
			if (!existPosList.contains(i))
			{
				if (bankIndex < filterBankInfos.size())
				{
					g_CGAInterface->MoveItem(filterBankInfos.at(bankIndex).pos, i, -1, bRes);
					bankIndex++;
				}
			}
		}
	}
}

void GameSysCfgWgt::on_pushButton_saveItem_clicked()
{
	QString sItems = ui.lineEdit_saveItem->text();
	QStringList sItemList;
	if (sItems.contains("|"))
	{
		sItemList = sItems.split("|");
	}
	else
		sItemList << sItems;
	//for (auto sItem : sItemList)
	//{
	//	g_pGameFun->SaveToBankAll(sItem);
	//}
	bool bSensitive = ui.checkBox_saveItem_Sensitive->isChecked();

	CGA::cga_items_info_t myinfos;
	if (!g_CGAInterface->GetBankItemsInfo(myinfos))
		return;
	//银行格子pos从100开始
	QList<int> existUsePos;
	for (int i = 0; i < myinfos.size(); i++)
	{
		CGA::cga_item_info_t itemInfo = myinfos.at(i);
		existUsePos.append(itemInfo.pos - 100);
	}
	QList<int> emptyPos;
	for (int i = 0; i < 80; ++i)
	{
		if (!existUsePos.contains(i))
		{
			emptyPos.append(i + 100);
		}
	}
	if (emptyPos.size() < 1)
	{
		qDebug() << "银行没有空位, 无法存放到银行";
		return;
	}
	QList<int> bagItemPosList;
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		if (bSensitive)
		{
			for (auto sItem : sItemList) //要存的物品位置拿出来
			{
				for (size_t i = 0; i < itemsinfo.size(); ++i)
				{
					const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
					if (iteminfo.pos > 7 && ((iteminfo.assess_flags & 1) == 1 || iteminfo.assess_flags == 24) && (sItem.toStdString() == iteminfo.name || sItem.toInt() == iteminfo.itemid))
					{
						bagItemPosList.append(iteminfo.pos);
					}
				}
			}
		}
		else
		{

			for (auto sItem : sItemList) //要存的物品位置拿出来
			{
				for (size_t i = 0; i < itemsinfo.size(); ++i)
				{
					const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
					if (iteminfo.pos > 7 && ((iteminfo.assess_flags & 1) == 1 || iteminfo.assess_flags == 24) && (QString::fromStdString(iteminfo.name).contains(sItem) || sItem.toInt() == iteminfo.itemid))
					{
						bagItemPosList.append(iteminfo.pos);
					}
				}
			}
		}

		int bankIndex = 0;
		for (int i = 0; i < bagItemPosList.size(); ++i)
		{
			if (bankIndex > (emptyPos.size() - 1)) //没有空位
			{
				qDebug() << "银行没有空位, 无法存放到银行";
				return;
			}
			int emptyslot = emptyPos.at(bankIndex);
			bool bResult = false;
			g_CGAInterface->MoveItem(bagItemPosList.at(i), emptyslot, -1, bResult);
			bankIndex++;
		}
	}
}

void GameSysCfgWgt::on_pushButton_fetchPet_clicked()
{
	QString sPets = ui.lineEdit_fetchPet->text();
	QStringList sPetList;
	if (sPets.contains("|"))
	{
		sPetList = sPets.split("|");
	}
	else
		sPetList << sPets;
	bool bSensitive = ui.checkBox_fetchPet_Sensitive->isChecked();
	for (auto sPet : sPetList)
	{
		g_pGameFun->WithdrawPet(sPet, bSensitive);
	}
}

void GameSysCfgWgt::on_pushButton_savePet_clicked()
{
	QString sPets = ui.lineEdit_savePet->text();
	QStringList sPetList;
	if (sPets.contains("|"))
	{
		sPetList = sPets.split("|");
	}
	else
		sPetList << sPets;
	bool bSensitive = ui.checkBox_savePet_Sensitive->isChecked();
	for (auto sPet : sPetList)
	{
		g_pGameFun->DepositPet(sPet, bSensitive);
	}
}

void GameSysCfgWgt::on_pushButton_fetchGold_clicked()
{
	QString sGold = ui.lineEdit_fetchGold->text();
	int nGold = sGold.toInt();
	g_pGameFun->WithdrawGold(nGold);
}

void GameSysCfgWgt::on_pushButton_saveGold_clicked()
{
	QString sGold = ui.lineEdit_saveGold->text();
	int nGold = sGold.toInt();
	g_pGameFun->DepositGold(nGold);
}

void GameSysCfgWgt::on_pushButton_dropGold_clicked()
{
	QString sGold = ui.lineEdit_dropGold->text();
	int nGold = sGold.toInt();
	g_pGameFun->DropGold(nGold);
}

void GameSysCfgWgt::on_toolButton_logBack_clicked()
{
	QString sKey;
	bool bInput = GetInputKey(sKey);
	if (bInput)
	{
		g_pGameCtrl->SetQuickKey(TDef_Quick_Key_Logback, sKey);
		ui.lineEdit_logback->setText(sKey);
	}
}

void GameSysCfgWgt::on_toolButton_logOut_clicked()
{
	QString sKey;
	bool bInput = GetInputKey(sKey);
	if (bInput)
	{
		g_pGameCtrl->SetQuickKey(TDef_Quick_Key_Logout, sKey);
		ui.lineEdit_logout->setText(sKey);
	}
}

void GameSysCfgWgt::on_toolButton_cross_clicked()
{
	QString sKey;
	bool bInput = GetInputKey(sKey);
	if (bInput)
	{
		g_pGameCtrl->SetQuickKey(TDef_Quick_Key_Cross, sKey);
		ui.lineEdit_cross->setText(sKey);
	}
}

void GameSysCfgWgt::on_toolButton_trade_clicked()
{
	QString sKey;
	bool bInput = GetInputKey(sKey);
	if (bInput)
	{
		g_pGameCtrl->SetQuickKey(TDef_Quick_Key_Trade, sKey);
		ui.lineEdit_trade->setText(sKey);
	}
}

void GameSysCfgWgt::on_toolButton_tradeNoAccept_clicked()
{
	QString sKey;
	bool bInput = GetInputKey(sKey);
	if (bInput)
	{
		g_pGameCtrl->SetQuickKey(TDef_Quick_Key_TradeNoAccept, sKey);
		ui.lineEdit_tradeNoAccept->setText(sKey);
	}
}

void GameSysCfgWgt::on_toolButton_saveAll_clicked()
{
	QString sKey;
	bool bInput = GetInputKey(sKey);
	if (bInput)
	{
		g_pGameCtrl->SetQuickKey(TDef_Quick_Key_SaveAll, sKey);
		ui.lineEdit_saveAll->setText(sKey);
	}
}

void GameSysCfgWgt::on_toolButton_fetchAll_clicked()
{
	QString sKey;
	bool bInput = GetInputKey(sKey);
	if (bInput)
	{
		g_pGameCtrl->SetQuickKey(TDef_Quick_Key_FetchAll, sKey);
		ui.lineEdit_fetchAll->setText(sKey);
	}
}

void GameSysCfgWgt::on_toolButton_often_map_clicked()
{
	QString sKey;
	bool bInput = GetInputKey(sKey);
	if (bInput)
	{
		g_pGameCtrl->SetQuickKey(TDef_Quick_Key_OftenMap, sKey);
		ui.lineEdit_often_map->setText(sKey);
	}
}

void GameSysCfgWgt::on_pushButton_sortBag_clicked()
{
	g_pGameFun->SortBagItems();
}

void GameSysCfgWgt::on_pushButton_sortBank_clicked()
{
	g_pGameFun->SortBankItems(true);
}

void GameSysCfgWgt::on_checkBox_autoSaveBank_stateChanged(int state)
{
	g_pGameCtrl->OnSetAutoUploadBankData(state);
}
