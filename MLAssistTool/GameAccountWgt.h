#pragma once

#include <QWidget>
#include "AccountTreeModel.h"
#include "ui_GameAccountWgt.h"

class GameAccountWgt : public QWidget
{
	Q_OBJECT

public:
	GameAccountWgt(QWidget* parent = Q_NULLPTR);
	~GameAccountWgt();

	void init();
protected:
	void resetModel();
	QString DisplayInputDlg(const QString& title, const QString& labelName);

public slots:
	void on_treeView_customContextMenuRequested(const QPoint&);
	void doAddAccountAssemble();
	void doAddAccount();
	void doAddAccountGid();

private:
	Ui::GameAccountWgt ui;
	AccountTreeModel* m_pAccountTreeModel;
	ITObjectPtr m_curSelectObj;
};
