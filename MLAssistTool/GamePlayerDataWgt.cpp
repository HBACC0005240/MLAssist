#include "GamePlayerDataWgt.h"
#include "ITObjectDataMgr.h"
#include <QMenu>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "AddAccountDlg.h"

GamePlayerDataWgt::GamePlayerDataWgt(QWidget *parent)
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
