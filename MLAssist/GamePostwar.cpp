#include "GamePostwar.h"
#include "GameCtrl.h"
#include <QMenu>
#include <QScrollBar>
GamePostwar::GamePostwar(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl, SIGNAL(signal_activeGameFZ()), this, SLOT(Active()));
	connect(g_pGameCtrl, SIGNAL(signal_addRenItem(GameItemPtr, bool)), this, SLOT(doAddRenItem(GameItemPtr, bool)));
	connect(g_pGameCtrl, SIGNAL(signal_addDieItem(GameItemPtr)), this, SLOT(doAddDieItem(GameItemPtr)));

	connect(g_pGameCtrl, SIGNAL(signal_addRenItemScript(QString, bool)), this, SLOT(doAddRenItemScript(QString, bool)));
	connect(g_pGameCtrl, SIGNAL(signal_addDieItemScript(QString, bool)), this, SLOT(doAddDieItemScript(QString, bool)));
	connect(g_pGameCtrl, SIGNAL(signal_switchAutoEatUi(int, bool)), this, SLOT(doSwitchAutoEatUi(int, bool)));
	connect(g_pGameCtrl, SIGNAL(signal_switchAutoCureUi(bool, bool, bool, int, int)), this, SLOT(doSwitchAutoCureUi(bool, bool, bool, int, int)));

	connect(g_pGameCtrl, SIGNAL(signal_switchAutoDrop(int)), this, SLOT(doSwitchAutoDrop(int)));
	connect(g_pGameCtrl, SIGNAL(signal_switchAutoPile(int)), this, SLOT(doSwitchAutoPile(int)));

	connect(ui.groupBox_die, SIGNAL(toggled(bool)), g_pGameCtrl, SLOT(OnSetAutoDieItems(bool)));
	connect(ui.groupBox_ren, SIGNAL(toggled(bool)), g_pGameCtrl, SLOT(OnSetAutoRenItems(bool)));
	connect(ui.groupBox_AutoFirstAid, SIGNAL(toggled(bool)), g_pGameCtrl, SLOT(OnSetAutoFirstAid(bool)));
	connect(ui.groupBox_AutoHeal, SIGNAL(toggled(bool)), g_pGameCtrl, SLOT(OnSetAutoHeal(bool)));

	connect(ui.listWidget_ren, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doRenItemChanged(QListWidgetItem *)));
	connect(ui.listWidget_die, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(doDieItemChanged(QListWidgetItem *)));
	ui.listWidget_ren->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.listWidget_die->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.listWidget_ren, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(OnRenContextMenu(const QPoint &)));
	connect(ui.listWidget_die, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(OnDieContextMenu(const QPoint &)));
}

GamePostwar::~GamePostwar()
{
}

void GamePostwar::initRenItems(GameItemList itemHash)
{
	ui.listWidget_ren->clear();
	foreach (auto it, itemHash)
	{
		QListWidgetItem *pItem = new QListWidgetItem(it->name);
		if (it->isDrop)
			pItem->setCheckState(Qt::Checked);
		else
			pItem->setCheckState(Qt::Unchecked);
		pItem->setToolTip(pItem->text());
		ui.listWidget_ren->addItem(pItem);
	}
}
void GamePostwar::initDieItems(GameItemList itemHash)
{
	ui.listWidget_die->clear();
	foreach (auto it, itemHash)
	{
		QString szName;
		if (it->maxCount != -1)
			szName = QString("%1&%2").arg(it->name).arg(it->maxCount);
		else
			szName = it->name;
		QListWidgetItem *pItem = new QListWidgetItem(szName);
		if (it->isPile)
			pItem->setCheckState(Qt::Checked);
		else
			pItem->setCheckState(Qt::Unchecked);
		pItem->setData(Qt::UserRole, it->name);
		pItem->setToolTip(pItem->text());
		ui.listWidget_die->addItem(pItem);
	}
}
void GamePostwar::on_pushButton_renAdd_clicked()
{
	QString renText = ui.lineEdit_renAdd->text();
	ui.lineEdit_renAdd->setText("");
	if (renText.isEmpty())
		return;
	QListWidgetItem *pItem = new QListWidgetItem(renText);
	pItem->setCheckState(Qt::Unchecked);
	ui.listWidget_ren->addItem(pItem);
	pItem->setToolTip(pItem->text());
	g_pGameCtrl->setRenItemIsChecked(renText, 0);
}

void GamePostwar::on_pushButton_dieAdd_clicked()
{
	QString dieText = ui.lineEdit_dieAdd->text();
	ui.lineEdit_dieAdd->setText("");
	if (dieText.isEmpty())
		return;
	QString dieName = dieText;
	int nDieCount = -1;
	if (dieText.contains("&"))
	{
		QStringList splitText = dieText.split("&");
		nDieCount = splitText.at(1).toInt();
		dieName = splitText.at(0);
	}
	QListWidgetItem *pItem = new QListWidgetItem(dieText);
	pItem->setCheckState(Qt::Unchecked);
	pItem->setData(Qt::UserRole, dieName);
	ui.listWidget_die->addItem(pItem);
	pItem->setToolTip(pItem->text());
	GameItemPtr pDieItem = g_pGameCtrl->setDieItemIsChecked(dieName, 0);
	if (pDieItem && nDieCount != -1)
		pDieItem->maxCount = nDieCount;
}

void GamePostwar::doSwitchAutoDrop(int state)
{
	ui.groupBox_ren->setChecked(state);
}

void GamePostwar::doSwitchAutoPile(int state)
{
	ui.groupBox_die->setChecked(state);
}

void GamePostwar::doLoadJsConfig(QJsonObject &obj)
{
	if (obj.contains("player"))
	{
		QJsonObject playerobj = obj.value("player").toObject();

		if (playerobj.contains("usefood"))
			ui.checkBox_playerEatMedicament->setChecked(playerobj.take("usefood").toBool());

		if (playerobj.contains("usemed"))
			ui.checkBox_playerEatMagic->setChecked(playerobj.take("usemed").toBool());

		if (playerobj.contains("petfood"))
			ui.checkBox_petEatMedicament->setChecked(playerobj.take("petfood").toBool());

		if (playerobj.contains("petmed"))
			ui.checkBox_petEatMagic->setChecked(playerobj.take("petmed").toBool());

		if (playerobj.contains("usefoodat"))
			ui.lineEdit_playerEatMedicament->setText(playerobj.take("usefoodat").toString());

		if (playerobj.contains("usemedat"))
			ui.lineEdit_playerEatMagic->setText(playerobj.take("usemedat").toString());

		if (playerobj.contains("petfoodat"))
			ui.lineEdit_petEatMedicament->setText(playerobj.take("petfoodat").toString());

		if (playerobj.contains("petmedat"))
			ui.lineEdit_petEatMagic->setText(playerobj.take("petmedat").toString());
	}
	if (obj.contains("itemdroplist"))
	{
		QJsonValue val = obj.take("itemdroplist");
		if (val.isArray())
		{
			ui.listWidget_ren->clear();
			const QJsonArray arr = val.toArray();
			QJsonArray::const_iterator itor = arr.constBegin();
			while (itor != arr.constEnd())
			{
				if ((*itor).isString())
				{
					QString line = (*itor).toString();
					if (!line.isEmpty())
					{
						QListWidgetItem *pItem = new QListWidgetItem(line.remove("#"));
						pItem->setCheckState(Qt::Checked);
						pItem->setToolTip(pItem->text());
						ui.listWidget_ren->addItem(pItem);
					}
				}
				++itor;
			}
		}
	}
	if (obj.contains("itemtweaklist"))
	{
		QJsonValue val = obj.take("itemtweaklist");
		if (val.isArray())
		{
			ui.listWidget_die->clear();
			const QJsonArray arr = val.toArray();
			QJsonArray::const_iterator itor = arr.constBegin();
			while (itor != arr.constEnd())
			{
				if ((*itor).isString())
				{
					QString line = (*itor).toString();
					if (!line.isEmpty())
					{
						auto strip = line.indexOf("|");
						if (strip <= 0)
							continue;
						bool bValue = false;
						int maxcount = line.mid(strip + 1).toInt(&bValue);
						if (!(bValue && maxcount > 0))
							continue;
						QString szName;
						if (line.at(0) == '#')
						{
							int value = line.mid(1, strip - 1).toInt(&bValue);
							if (bValue && value > 0)
							{
								szName = QString("%1&%2").arg(value).arg(maxcount);
							}
						}
						else
						{
							szName = QString("%1&%2").arg(line.mid(0, strip)).arg(maxcount);
						}
						QListWidgetItem *pItem = new QListWidgetItem(szName);
						pItem->setCheckState(Qt::Checked);
						pItem->setData(Qt::UserRole, szName);
						pItem->setToolTip(pItem->text());
						ui.listWidget_die->addItem(pItem);
					}
				}
				++itor;
			}
		}
	}
}

void GamePostwar::doSaveJsConfig(QJsonObject &obj)
{
	QJsonObject player = obj.value("player").toObject();
	//	player.insert("autosupply", ui->checkBox_autoSupply->isChecked());

	player.insert("usefood", ui.checkBox_playerEatMedicament->isChecked());
	player.insert("usemed", ui.checkBox_playerEatMagic->isChecked());
	player.insert("petfood", ui.checkBox_petEatMedicament->isChecked());
	player.insert("petmed", ui.checkBox_petEatMagic->isChecked());
	if (ui.lineEdit_playerEatMedicament->text().toInt() < 100)
		player.insert("usefoodat", ui.lineEdit_playerEatMedicament->text() + "%");
	else
		player.insert("usefoodat", ui.lineEdit_playerEatMedicament->text());
	if (ui.lineEdit_playerEatMagic->text().toInt() < 100)
		player.insert("usemedat", ui.lineEdit_playerEatMagic->text() + "%");
	else
		player.insert("usemedat", ui.lineEdit_playerEatMagic->text());
	if (ui.lineEdit_petEatMedicament->text().toInt() < 100)
		player.insert("petfoodat", ui.lineEdit_petEatMedicament->text() + "%");
	else
		player.insert("petfoodat", ui.lineEdit_petEatMedicament->text());
	if (ui.lineEdit_petEatMagic->text().toInt() < 100)
		player.insert("petmedat", ui.lineEdit_petEatMagic->text() + "%");
	else
		player.insert("petmedat", ui.lineEdit_petEatMagic->text());
	obj.insert("player", player);

	QJsonArray drop;
	//物品叠加与丢弃
	int nCount = ui.listWidget_ren->count();
	bool bTrans = false;
	for (int i = 0; i < nCount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget_ren->item(i);
		QString sName = pItem->text();
		sName.toInt(&bTrans);
		if (bTrans)
			sName = QString("#%1").arg(sName.toInt());
		drop.insert(drop.end(), sName);
	}
	obj.insert("itemdroplist", drop);

	QJsonArray tweak;
	nCount = ui.listWidget_die->count();
	for (int i = 0; i < nCount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget_die->item(i);
		QString sName = pItem->text();
		QStringList sNameList = sName.split("&");
		if (sNameList.size() < 2)
			continue;
		sNameList[0].toInt(&bTrans);
		if (bTrans)
			sName = QString("#%1").arg(sNameList[0].toInt());
		sName += "|";
		sName += sNameList[1].toInt();
		tweak.insert(tweak.end(), sName);
	}
	obj.insert("itemtweaklist", tweak);
}

void GamePostwar::doLoadUserConfig(QSettings &iniFile)
{
	//initRenItems(g_pGameCtrl->GetRenItemHash());
	//initDieItems(g_pGameCtrl->GetDieItemHash());

	iniFile.beginGroup("AutoHeal");
	ui.groupBox_AutoHeal->setChecked(iniFile.value("AutoHealGroup").toBool());
	ui.groupBox_AutoHeal->setChecked(iniFile.value("AutoHealGroup").toBool());
	ui.comboBox_HurtLv->setCurrentIndex(iniFile.value("HurtLv").toInt());
	ui.comboBox_HealLv->setCurrentIndex(iniFile.value("SkillLv").toInt());
	ui.checkBox_HealPet->setChecked(iniFile.value("Pet").toBool());
	ui.checkBox_HealTeammate->setChecked(iniFile.value("Teammate").toBool());
	iniFile.endGroup();
	iniFile.beginGroup("AutoFirstAid");
	ui.groupBox_AutoFirstAid->setChecked(iniFile.value("AutoFirstAidGroup").toBool());
	ui.checkBox_playerHp->setChecked(iniFile.value("AutoFirstAid").toBool());
	ui.lineEdit_playerHp->setText(iniFile.value("hp").toString());
	ui.comboBox_playerSkillLv->setCurrentIndex(iniFile.value("SkillLv").toInt());
	ui.checkBox_firstAidPet->setChecked(iniFile.value("Pet").toBool());
	ui.checkBox_firstAidTeammate->setChecked(iniFile.value("Teammate").toBool());
	iniFile.endGroup();

	ui.listWidget_ren->clear();
	g_pGameCtrl->ClearRenItems();
	int renCount = iniFile.value("ren/count", 0).toInt();
	for (int i = 1; i <= renCount; ++i)
	{
		QString itemKeyName = QString("item%1").arg(i);
		QString itemKeyVal = QString("checked%1").arg(i);
		QString itemName = iniFile.value(QString("ren/%1").arg(itemKeyName), "").toString();
		bool itemVal = iniFile.value(QString("ren/%1").arg(itemKeyVal), "").toBool();

		QListWidgetItem *pItem = new QListWidgetItem(itemName);
		pItem->setCheckState(itemVal ? Qt::Checked : Qt::Unchecked);
		ui.listWidget_ren->addItem(pItem);
		pItem->setToolTip(itemName);
		g_pGameCtrl->setRenItemIsChecked(itemName, itemVal);
	}

	ui.listWidget_die->clear();
	g_pGameCtrl->ClearDieItems();
	int dieCount = iniFile.value("die/count", 0).toInt();
	for (int i = 1; i <= dieCount; ++i)
	{
		QString itemKeyName = QString("item%1").arg(i);
		QString itemKeyVal = QString("checked%1").arg(i);
		QString itemName = iniFile.value(QString("die/%1").arg(itemKeyName), "").toString();
		bool itemVal = iniFile.value(QString("die/%1").arg(itemKeyVal), "").toBool();

		QString dieName = itemName;
		int nDieCount = -1;
		if (itemName.contains("&"))
		{
			QStringList splitText = itemName.split("&");
			nDieCount = splitText.at(1).toInt();
			dieName = splitText.at(0);
		}
		QListWidgetItem *pItem = new QListWidgetItem(itemName);
		pItem->setData(Qt::UserRole, dieName);
		ui.listWidget_die->addItem(pItem);
		pItem->setToolTip(pItem->text());
		GameItemPtr pDieItem = g_pGameCtrl->setDieItemIsChecked(dieName, 0);
		if (pDieItem && nDieCount != -1)
			pDieItem->maxCount = nDieCount;
		pItem->setCheckState(itemVal ? Qt::Checked : Qt::Unchecked);
	}
	iniFile.beginGroup("AutoEat");
	ui.lineEdit_playerEatMedicament->setText(iniFile.value("AutoHpVal").toString());
	ui.lineEdit_playerEatMagic->setText(iniFile.value("AutoMpVal").toString());
	ui.lineEdit_petEatMedicament->setText(iniFile.value("AutoPetHpVal").toString());
	ui.lineEdit_petEatMagic->setText(iniFile.value("AutoPetMpVal").toString());	

	ui.checkBox_playerEatMedicament->setChecked(iniFile.value("AutoHp").toBool());
	ui.checkBox_playerEatMagic->setChecked(iniFile.value("AutoMp").toBool());
	ui.checkBox_petEatMedicament->setChecked(iniFile.value("AutoPetHp").toBool());
	ui.checkBox_petEatMagic->setChecked(iniFile.value("AutoPetMp").toBool());

	iniFile.endGroup();
	iniFile.beginGroup("EquipProtect");
	ui.groupBox_equipProtect->setChecked(iniFile.value("Enable").toBool());
	ui.lineEdit_equipDurable->setText(iniFile.value("EquipVal").toString());
	ui.radioButton_sameEquip->setChecked(iniFile.value("SameEquip").toBool());
	ui.radioButton_offLine->setChecked(iniFile.value("OffLine").toBool());
	ui.radioButton_sameType->setChecked(iniFile.value("SameEquipType").toBool());
	ui.checkBox_noEquipOffline->setChecked(iniFile.value("NoEquipOffLine").toBool());
	ui.checkBox_renEquip->setChecked(iniFile.value("RenEquip").toBool());
	updateEquipProtect();
	iniFile.endGroup();
	syncUiData();
}

void GamePostwar::doSaveUserConfig(QSettings &iniFile)
{
	//物品叠加与丢弃
	int nCount = ui.listWidget_ren->count();
	iniFile.setValue("ren/count", nCount);
	iniFile.beginGroup("ren");
	for (int i = 0; i < nCount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget_ren->item(i);
		QString itemKeyName = QString("item%1").arg(i + 1);
		QString itemKeyVal = QString("checked%1").arg(i + 1);
		iniFile.setValue(itemKeyName, pItem->text());
		iniFile.setValue(itemKeyVal, pItem->checkState() == Qt::Checked);
	}
	iniFile.endGroup();
	nCount = ui.listWidget_die->count();
	iniFile.setValue("die/count", nCount);
	iniFile.beginGroup("die");
	for (int i = 0; i < nCount; ++i)
	{
		QListWidgetItem *pItem = ui.listWidget_die->item(i);
		QString itemKeyName = QString("item%1").arg(i + 1);
		QString itemKeyVal = QString("checked%1").arg(i + 1);
		iniFile.setValue(itemKeyName, pItem->text());
		iniFile.setValue(itemKeyVal, pItem->checkState() == Qt::Checked);
	}
	iniFile.endGroup();

	//GameItemList m_pRenItemList = g_pGameCtrl->GetRenItemHash();
	//iniFile.setValue("ren/count", m_pRenItemList.size());
	//iniFile.beginGroup("ren");
	//for (int i = 0; i < m_pRenItemList.size(); ++i)
	//{
	//	GameItem *pItem = m_pRenItemList.at(i);
	//	QString itemKeyName = QString("item%1").arg(i + 1);
	//	QString itemKeyVal = QString("checked%1").arg(i + 1);
	//	iniFile.setValue(itemKeyName, pItem->name);
	//	iniFile.setValue(itemKeyVal, pItem->isDrop);
	//}
	//iniFile.endGroup();

	//int nCount = ui.listWidget_die->count();
	//iniFile.setValue("die/count", nCount);
	//iniFile.beginGroup("die");
	//for (int i = 0; i < nCount; ++i)
	//{
	//	QListWidgetItem *pItem = ui.listWidget_die->item(i);
	//	QString itemKeyName = QString("item%1").arg(i + 1);
	//	QString itemKeyVal = QString("checked%1").arg(i + 1);
	//	iniFile.setValue(itemKeyName, pItem->text());
	//	iniFile.setValue(itemKeyVal, pItem->checkState() == Qt::Checked);
	//}
	//iniFile.endGroup();
	iniFile.beginGroup("AutoHeal");
	iniFile.setValue("AutoHealGroup", ui.groupBox_AutoHeal->isChecked());
	iniFile.setValue("HurtLv", ui.comboBox_HurtLv->currentIndex());
	iniFile.setValue("SkillLv", ui.comboBox_HealLv->currentIndex());
	iniFile.setValue("Pet", ui.checkBox_HealPet->isChecked());
	iniFile.setValue("Teammate", ui.checkBox_HealTeammate->isChecked());
	iniFile.endGroup();
	iniFile.beginGroup("AutoFirstAid");
	iniFile.setValue("AutoFirstAidGroup", ui.groupBox_AutoFirstAid->isChecked());
	iniFile.setValue("AutoFirstAid", ui.checkBox_playerHp->isChecked());
	iniFile.setValue("hp", ui.lineEdit_playerHp->text());
	iniFile.setValue("SkillLv", ui.comboBox_playerSkillLv->currentIndex());
	iniFile.setValue("Pet", ui.checkBox_firstAidPet->isChecked());
	iniFile.setValue("Teammate", ui.checkBox_firstAidTeammate->isChecked());
	iniFile.endGroup();
	iniFile.beginGroup("AutoEat");
	iniFile.setValue("AutoHp", ui.checkBox_playerEatMedicament->isChecked());
	iniFile.setValue("AutoHpVal", ui.lineEdit_playerEatMedicament->text());
	iniFile.setValue("AutoMp", ui.checkBox_playerEatMagic->isChecked());
	iniFile.setValue("AutoMpVal", ui.lineEdit_playerEatMagic->text());
	iniFile.setValue("AutoPetHp", ui.checkBox_petEatMedicament->isChecked());
	iniFile.setValue("AutoPetHpVal", ui.lineEdit_petEatMedicament->text());
	iniFile.setValue("AutoPetMp", ui.checkBox_petEatMagic->isChecked());
	iniFile.setValue("AutoPetMpVal", ui.lineEdit_petEatMagic->text());
	iniFile.endGroup();
	iniFile.beginGroup("EquipProtect");
	iniFile.setValue("Enable", ui.groupBox_equipProtect->isChecked());
	iniFile.setValue("EquipVal", ui.lineEdit_equipDurable->text());
	on_lineEdit_equipDurable_editingFinished();
	iniFile.setValue("SameEquip", ui.radioButton_sameEquip->isChecked());
	iniFile.setValue("OffLine", ui.radioButton_offLine->isChecked());
	iniFile.setValue("SameEquipType", ui.radioButton_sameType->isChecked());
	iniFile.setValue("NoEquipOffLine", ui.checkBox_noEquipOffline->isChecked());
	iniFile.setValue("RenEquip", ui.checkBox_renEquip->isChecked());
	iniFile.endGroup();
}

void GamePostwar::doRenItemChanged(QListWidgetItem *item)
{
	if (item == nullptr)
		return;
	QString name = item->text();
	if (item->checkState() == Qt::Checked)
		g_pGameCtrl->setRenItemIsChecked(name, 1);
	else
		g_pGameCtrl->setRenItemIsChecked(name, 0);
}

void GamePostwar::doDieItemChanged(QListWidgetItem *item)
{
	if (item == nullptr)
		return;
	QString name = item->data(Qt::UserRole).toString();
	if (item->checkState() == Qt::Checked)
		g_pGameCtrl->setDieItemIsChecked(name, 1);
	else
		g_pGameCtrl->setDieItemIsChecked(name, 0);
}

void GamePostwar::doAddRenItemScript(QString name, bool bChecked)
{
	if (name.isEmpty())
		return;

	auto addRenItemFun = [&](const QString &sName, bool bChecked)
	{
		QString sToolTip = sName;
		auto pFindItem = ui.listWidget_ren->findItems(sName, Qt::MatchExactly);
		if (pFindItem.size() > 0)
		{
			pFindItem.at(0)->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);
			return;
		}
		QListWidgetItem *pTableItem = new QListWidgetItem(sName);
		pTableItem->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);
		ui.listWidget_ren->addItem(pTableItem);
		pTableItem->setToolTip(sToolTip);
		g_pGameCtrl->setRenItemIsChecked(sName, bChecked);
	};
	if (name.contains("|"))
	{
		QStringList sRenItems = name.split("|");
		for (auto tItem : sRenItems)
		{
			addRenItemFun(tItem, bChecked);
		}
	}
	else
		addRenItemFun(name, bChecked);
}

void GamePostwar::doAddDieItemScript(QString name, bool bChecked)
{
	if (name.isEmpty())
		return;

	auto addDieItemFun = [&](const QString &sName, bool bChecked)
	{
		QString sToolTip = sName;
		auto pFindItem = ui.listWidget_die->findItems(sName, Qt::MatchExactly);
		if (pFindItem.size() > 0)
		{
			pFindItem.at(0)->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);
			return;
		}
		QString dieName = sName;
		int nDieCount = -1;
		if (sName.contains("&"))
		{
			QStringList splitText = sName.split("&");
			nDieCount = splitText.at(1).toInt();
			dieName = splitText.at(0);
		}
		QListWidgetItem *pItem = new QListWidgetItem(sName);
		pItem->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);
		pItem->setData(Qt::UserRole, dieName);
		ui.listWidget_die->addItem(pItem);
		pItem->setToolTip(pItem->text());
		GameItemPtr pDieItem = g_pGameCtrl->setDieItemIsChecked(dieName, bChecked);
		if (pDieItem && nDieCount != -1)
			pDieItem->maxCount = nDieCount;
	};
	if (name.contains("|"))
	{
		QStringList sRenItems = name.split("|");
		for (auto tItem : sRenItems)
		{
			addDieItemFun(tItem, bChecked);
		}
	}
	else
		addDieItemFun(name, bChecked);
}

void GamePostwar::doAddRenItem(GameItemPtr pItem, bool bCode)
{
	if (!pItem || pItem->name.isEmpty())
		return;
	QString sName = pItem->name;
	QString sToolTip = QString("%1|%2").arg(pItem->name).arg(pItem->id);
	if (bCode)
		sName = QString::number(pItem->id);
	QListWidgetItem *pTableItem = new QListWidgetItem(sName);
	pTableItem->setCheckState(Qt::Checked);
	ui.listWidget_ren->addItem(pTableItem);
	pTableItem->setToolTip(sToolTip);
}

void GamePostwar::doAddDieItem(GameItemPtr pItem)
{
	if (!pItem || pItem->name.isEmpty())
		return;
	//后续增加个数量数据库，自动获取数量上限
	ui.lineEdit_dieAdd->setText(QString("%1&").arg(pItem->name));
	return;
	QListWidgetItem *pTableItem = new QListWidgetItem(pItem->name);
	pTableItem->setCheckState(Qt::Checked);
	ui.listWidget_die->addItem(pTableItem);
	pTableItem->setToolTip(pTableItem->text());
}

void GamePostwar::on_checkBox_playerHp_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		g_pGameCtrl->getGameFirstAidCfg()->bSelf = true;
	}
	else if (state == Qt::Unchecked)
	{
		g_pGameCtrl->getGameFirstAidCfg()->bSelf = false;
	}
}

void GamePostwar::on_checkBox_firstAidPet_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		g_pGameCtrl->getGameFirstAidCfg()->bPet = true;
	}
	else if (state == Qt::Unchecked)
	{
		g_pGameCtrl->getGameFirstAidCfg()->bPet = false;
	}
}

void GamePostwar::on_checkBox_firstAidTeammate_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		g_pGameCtrl->getGameFirstAidCfg()->bTeammate = true;
	}
	else if (state == Qt::Unchecked)
	{
		g_pGameCtrl->getGameFirstAidCfg()->bTeammate = false;
	}
}

void GamePostwar::on_lineEdit_playerHp_editingFinished()
{
	QString text = ui.lineEdit_playerHp->text();
	g_pGameCtrl->getGameFirstAidCfg()->dFirstAidHp = text.toDouble();
}

void GamePostwar::on_comboBox_playerSkillLv_currentIndexChanged(int index)
{
	g_pGameCtrl->getGameFirstAidCfg()->nLv = index - 1;
}

void GamePostwar::on_comboBox_HurtLv_currentIndexChanged(int index)
{
	if (index == 0)
	{
		g_pGameCtrl->getGameHealCfg()->nHurtVal = 100;
	}
	else
	{
		g_pGameCtrl->getGameHealCfg()->nHurtVal = index * 25;
	}
}

void GamePostwar::on_comboBox_HealLv_currentIndexChanged(int index)
{
	g_pGameCtrl->getGameHealCfg()->nLv = index - 1;
}

void GamePostwar::on_checkBox_HealPet_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		g_pGameCtrl->getGameHealCfg()->bPet = true;
	}
	else if (state == Qt::Unchecked)
	{
		g_pGameCtrl->getGameHealCfg()->bPet = false;
	}
}

void GamePostwar::on_checkBox_HealTeammate_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		g_pGameCtrl->getGameHealCfg()->bTeammate = true;
	}
	else if (state == Qt::Unchecked)
	{
		g_pGameCtrl->getGameHealCfg()->bTeammate = false;
	}
}

void GamePostwar::on_checkBox_playerEatMedicament_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetPlayerEatCfg(TCondition_PlayerEatMedicament);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		if (ui.lineEdit_playerEatMedicament->text().contains("%"))
		{
			pCfg->bPercentage = true;
			pCfg->dVal = ui.lineEdit_playerEatMedicament->text().remove("%").toDouble();
		}
		else
		{
			pCfg->bPercentage = false;
			pCfg->dVal = ui.lineEdit_playerEatMedicament->text().toDouble();
		}
	}
}

void GamePostwar::on_checkBox_playerEatMagic_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetPlayerEatCfg(TCondition_PlayerEatMagic);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);

		if (ui.lineEdit_playerEatMagic->text().contains("%"))
		{
			pCfg->bPercentage = true;
			pCfg->dVal = ui.lineEdit_playerEatMagic->text().remove("%").toDouble();
		}
		else
		{
			pCfg->bPercentage = false;
			pCfg->dVal = ui.lineEdit_playerEatMagic->text().toDouble();
		}
	}
}

void GamePostwar::on_checkBox_petEatMedicament_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetPlayerEatCfg(TCondition_PetEatMedicament);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		if (ui.lineEdit_petEatMedicament->text().contains("%"))
		{
			pCfg->bPercentage = true;
			pCfg->dVal = ui.lineEdit_petEatMedicament->text().remove("%").toDouble();
		}
		else
		{
			pCfg->bPercentage = false;
			pCfg->dVal = ui.lineEdit_petEatMedicament->text().toDouble();
		}
	}
}

void GamePostwar::on_checkBox_petEatMagic_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetPlayerEatCfg(TCondition_PetEatMagic);
	if (pCfg)
	{
		pCfg->bChecked = (state == Qt::Checked ? true : false);
		if (ui.lineEdit_petEatMagic->text().contains("%"))
		{
			pCfg->bPercentage = true;
			pCfg->dVal = ui.lineEdit_petEatMagic->text().remove("%").toDouble();
		}
		else
		{
			pCfg->bPercentage = false;
			pCfg->dVal = ui.lineEdit_petEatMagic->text().toDouble();
		}
	}
}

void GamePostwar::on_lineEdit_playerEatMedicament_editingFinished()
{
	on_checkBox_playerEatMedicament_stateChanged(ui.checkBox_playerEatMedicament->checkState());
}

void GamePostwar::on_lineEdit_playerEatMagic_editingFinished()
{
	on_checkBox_playerEatMagic_stateChanged(ui.checkBox_playerEatMagic->checkState());
}

void GamePostwar::on_lineEdit_petEatMedicament_editingFinished()
{
	on_checkBox_petEatMedicament_stateChanged(ui.checkBox_petEatMedicament->checkState());
}

void GamePostwar::on_lineEdit_petEatMagic_editingFinished()
{
	on_checkBox_petEatMagic_stateChanged(ui.checkBox_petEatMagic->checkState());
}

void GamePostwar::OnRenContextMenu(const QPoint &pos)
{
	QListWidgetItem *curItem = ui.listWidget_ren->itemAt(pos); //获取当前被点击的节点
	if (curItem != NULL)
	{
		QMenu menu;
		menu.addAction("删除", this, SLOT(doRemoveRenInfo()));
		menu.exec(QCursor::pos());
	}
}

void GamePostwar::OnDieContextMenu(const QPoint &pos)
{
	QListWidgetItem *curItem = ui.listWidget_die->itemAt(pos); //获取当前被点击的节点
	if (curItem != NULL)
	{
		QMenu menu;
		menu.addAction("删除", this, SLOT(doRemoveDieInfo()));
		menu.exec(QCursor::pos());
	}
}

void GamePostwar::doRemoveRenInfo()
{
	QListWidgetItem *curItem = ui.listWidget_ren->currentItem();
	if (!curItem)
		return;
	QString name = curItem->text();
	g_pGameCtrl->setRenItemIsChecked(name, 0);
	ui.listWidget_ren->takeItem(ui.listWidget_ren->currentRow());
}

void GamePostwar::doRemoveDieInfo()
{
	QListWidgetItem *curItem = ui.listWidget_die->currentItem();
	if (!curItem)
		return;
	QString name = curItem->text();
	g_pGameCtrl->setDieItemIsChecked(name, 0);
	ui.listWidget_die->takeItem(ui.listWidget_die->currentRow());
}

void GamePostwar::on_groupBox_equipProtect_toggled(bool bChecked)
{
	auto pCfg = g_pGameCtrl->GetPlayerEquipProtectCfg();
	if (pCfg)
	{
		pCfg->bChecked = bChecked;
	}
}

void GamePostwar::on_lineEdit_equipDurable_editingFinished()
{
	double dVal = ui.lineEdit_equipDurable->text().toDouble();
	auto pCfg = g_pGameCtrl->GetPlayerEquipProtectCfg();
	if (pCfg && pCfg->dVal != dVal)
	{
		pCfg->dVal = dVal;
		qDebug() << pCfg->dVal;
	}
}

void GamePostwar::on_radioButton_sameEquip_clicked(bool bChecked)
{
	auto pCfg = g_pGameCtrl->GetPlayerEquipProtectCfg();
	if (pCfg && bChecked)
	{
		pCfg->nSwapEquip = 0;
		//	qDebug() << pCfg->nSwapEquip;
	}
}

void GamePostwar::on_radioButton_offLine_clicked(bool bChecked)
{
	auto pCfg = g_pGameCtrl->GetPlayerEquipProtectCfg();
	if (pCfg && bChecked)
	{
		pCfg->nSwapEquip = 2;
		//qDebug() << pCfg->nSwapEquip;
	}
}

void GamePostwar::on_radioButton_sameType_clicked(bool bChecked)
{
	auto pCfg = g_pGameCtrl->GetPlayerEquipProtectCfg();
	if (pCfg && bChecked)
	{
		pCfg->nSwapEquip = 1;
		//qDebug() << pCfg->nSwapEquip;
	}
}

void GamePostwar::on_checkBox_noEquipOffline_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetPlayerEquipProtectCfg();
	if (pCfg)
	{
		pCfg->bNoEquipOffLine = state == Qt::Checked ? true : false;
	}
}

void GamePostwar::on_checkBox_renEquip_stateChanged(int state)
{
	auto pCfg = g_pGameCtrl->GetPlayerEquipProtectCfg();
	if (pCfg)
	{
		pCfg->bRenEquip = state == Qt::Checked ? true : false;
	}
}

void GamePostwar::on_checkBox_eatTime_stateChanged(int state)
{
	g_pGameCtrl->OnSetAutoEatTimeCrystal(state);
}

void GamePostwar::on_checkBox_eatShenLan_stateChanged(int state)
{
	g_pGameCtrl->OnSetAutoEatDeepBlue(state);
}

void GamePostwar::on_checkBox_eatGouLiang_stateChanged(int state)
{
	g_pGameCtrl->OnSetAutoEatDogFood(state);
}

void GamePostwar::updateEquipProtect()
{
	on_groupBox_equipProtect_toggled(ui.groupBox_equipProtect->isChecked());
	on_lineEdit_equipDurable_editingFinished();
	on_radioButton_sameEquip_clicked(ui.radioButton_sameEquip->isChecked());
	on_radioButton_offLine_clicked(ui.radioButton_offLine->isChecked());
	on_radioButton_sameType_clicked(ui.radioButton_sameType->isChecked());
	on_checkBox_noEquipOffline_stateChanged(ui.checkBox_noEquipOffline->checkState());
	on_checkBox_renEquip_stateChanged(ui.checkBox_renEquip->checkState());
}

void GamePostwar::syncUiData()
{
	g_pGameCtrl->OnSetAutoDieItems(ui.groupBox_die->isChecked());
	g_pGameCtrl->OnSetAutoRenItems(ui.groupBox_ren->isChecked());
	g_pGameCtrl->OnSetAutoFirstAid(ui.groupBox_AutoFirstAid->isChecked());
	g_pGameCtrl->OnSetAutoHeal(ui.groupBox_AutoHeal->isChecked());

	on_checkBox_playerHp_stateChanged(ui.checkBox_playerHp->checkState());
	on_checkBox_firstAidPet_stateChanged(ui.checkBox_firstAidPet->checkState());
	on_checkBox_firstAidTeammate_stateChanged(ui.checkBox_firstAidTeammate->checkState());

	on_lineEdit_playerHp_editingFinished();
	on_comboBox_playerSkillLv_currentIndexChanged(ui.comboBox_playerSkillLv->currentIndex());

	on_comboBox_HurtLv_currentIndexChanged(ui.comboBox_HurtLv->currentIndex());
	on_comboBox_HealLv_currentIndexChanged(ui.comboBox_HealLv->currentIndex());
	on_checkBox_HealPet_stateChanged(ui.checkBox_HealPet->checkState());
	on_checkBox_HealTeammate_stateChanged(ui.checkBox_HealTeammate->checkState());

	on_checkBox_playerEatMedicament_stateChanged(ui.checkBox_playerEatMedicament->checkState());
	on_checkBox_playerEatMagic_stateChanged(ui.checkBox_playerEatMagic->checkState());
	on_checkBox_petEatMedicament_stateChanged(ui.checkBox_petEatMedicament->checkState());
	on_checkBox_petEatMagic_stateChanged(ui.checkBox_petEatMagic->checkState());
	on_lineEdit_playerEatMedicament_editingFinished();
	on_lineEdit_playerEatMagic_editingFinished();
	on_lineEdit_petEatMedicament_editingFinished();
	on_lineEdit_petEatMagic_editingFinished();
}

void GamePostwar::doSwitchAutoEatUi(int type, bool bChecked)
{
	switch (type)
	{
		case TSysConfigSet_AutoEatDeepBlue:
		{
			ui.checkBox_eatShenLan->setChecked(bChecked);
			on_checkBox_eatShenLan_stateChanged(ui.checkBox_eatShenLan->checkState());
			break;
		}
		case TSysConfigSet_AutoEatDogFood:
		{
			ui.checkBox_eatGouLiang->setChecked(bChecked);
			on_checkBox_eatGouLiang_stateChanged(ui.checkBox_eatGouLiang->checkState());
			break;
		}
		case TSysConfigSet_AutoEatTimeCrystal:
		{
			ui.checkBox_eatTime->setChecked(bChecked);
			on_checkBox_eatTime_stateChanged(ui.checkBox_eatTime->checkState());
			break;
		}
		default:
			break;
	}
}

void GamePostwar::doSwitchAutoCureUi(bool v1, bool v2, bool v3, int v4, int v5)
{
	ui.groupBox_AutoHeal->setChecked(v1);
	ui.checkBox_HealPet->setChecked(v2);
	ui.checkBox_HealTeammate->setChecked(v3);
	ui.comboBox_HurtLv->setCurrentIndex(v4);
	ui.comboBox_HealLv->setCurrentIndex(v5);
}
