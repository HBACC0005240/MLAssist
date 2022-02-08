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
	ui.treeView->setModel(NULL);
	ui.treeView->setModel(m_pPlayerDataTreeModel);
	ui.treeView->expandAll();
	ui.treeView->collapseAll();
	ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);      //TreeView启用右键菜单信号

	//connect(ui.treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_treeView_customContextMenuRequested(const QPoint&)));
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

void GamePlayerDataWgt::resetModel()
{
	if (m_pPlayerDataTreeModel)
	{
		delete m_pPlayerDataTreeModel;
		m_pPlayerDataTreeModel = nullptr;
	}
	m_pPlayerDataTreeModel = new PlayerDataTreeModel(this);
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
		menu.addAction(QString("增加证件"), this, SLOT(doAddAccountAssemble()));
		menu.exec(QCursor::pos());
	}
	else
	{
		auto objid = Item->data(Qt::UserRole).toULongLong();
		m_curSelectObj = ITObjectDataMgr::getInstance().FindObject(objid);
		if (m_curSelectObj)
		{
			int devtype = m_curSelectObj->getObjectType();
			if (devtype == TObject_AccountIdentity)
			{
				QMenu menu(this);
				menu.setStyleSheet(QString("QMenu{background:rgb(0,95,91);border-radius:3px;color:rgb(255,255,255);}\
				QMenu::item{ background - color:transparent; }\
				QMenu::item:selected{ background - color:rgb(0,62,59); color:rgb(0,255,242); }"));
				menu.addAction(QString("增加账号"), this, SLOT(doAddAccount()));
				menu.exec(QCursor::pos());
			}
			else if (devtype == TObject_Account)
			{
				QMenu menu(this);
				menu.setStyleSheet(QString("QMenu{background:rgb(0,95,91);border-radius:3px;color:rgb(255,255,255);}\
				QMenu::item{ background - color:transparent; }\
				QMenu::item:selected{ background - color:rgb(0,62,59); color:rgb(0,255,242); }"));
				menu.addAction(QString("增加游戏ID"), this, SLOT(doAddAccountGid()));
				menu.exec(QCursor::pos());

			}
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
			resetModel();
		}
	}
}

void GamePlayerDataWgt::doAddAccount()
{
	AddAccountDlg dlg;
	dlg.SetAccountAssemblePtr(qSharedPointerCast<ITAccountIdentity>(m_curSelectObj));
	if (dlg.exec() == QDialog::Accepted)
	{
		resetModel();
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
		resetModel();
	}
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

		if (m_curSelectObj->getObjectType() != TObject_GidRole)
			return;
		ITGidRolePtr pRole = qSharedPointerCast<ITGidRole>(m_curSelectObj);
		ui.lineEdit_gold->setText(QString::number(pRole->_gold));
		ui.lineEdit_bankGold->setText(QString::number(pRole->_bankgold));
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

void GamePlayerDataWgt::doUpdateBagItemTableWidget(ITGidRolePtr pRole)
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

void GamePlayerDataWgt::doUpdateBankItemTableWidget(ITGidRolePtr pRole)
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

void GamePlayerDataWgt::doUpdatePetTableWidget(QTableWidget* pTable, ITGidRolePtr pRole)
{
	if (!pRole)
		return;
	pTable->setUpdatesEnabled(false);

	int baseNum = 0;
	if (pTable == ui.tableWidget_bankPet)
	{
		baseNum = 100;
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
			szHp = QString("%1/%2").arg(pPet->_hp).arg(pPet->_maxhp);
			szMp = QString("%1/%2").arg(pPet->_mp).arg(pPet->_maxmp);
			sLv = QString::number(pPet->_level);
			sXp = QString("%1").arg(pPet->_maxxp - pPet->_xp);
			sAttac = QString("%1").arg(pPet->_value_attack);
			sDefensive = QString("%1").arg(pPet->_value_defensive);
			sAgility = QString("%1").arg(pPet->_value_agility);
			sSpirit = QString("%1").arg(pPet->_value_spirit);
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
			state = "";// m_petState.value(pPet->battle_flags);
		}
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
	resetModel();
}
