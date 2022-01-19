#pragma once

#include <QWidget>
#include "PlayerDataTreeModel.h"
#include "ui_GamePlayerDataWgt.h"

class GamePlayerDataWgt : public QWidget
{
	Q_OBJECT

public:
	GamePlayerDataWgt(QWidget* parent = Q_NULLPTR);
	~GamePlayerDataWgt();
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
	Ui::GamePlayerDataWgt ui;
	PlayerDataTreeModel* m_pPlayerDataTreeModel;
	ITObjectPtr m_curSelectObj;
};
