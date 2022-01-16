#pragma once

#include "AutoBattle.h"
#include "ui_BattleSetDlg.h"
#include <QDialog>

class BattleSetDlg : public QDialog
{
	Q_OBJECT

public:
	BattleSetDlg(QWidget *parent = Q_NULLPTR);
	~BattleSetDlg();

	void init();
	void setBattleSetting(CBattleSettingPtr &pSetting);
	CBattleAction *GetPlayerAction() { return m_playerAction; }
	CBattleAction *GetPetAction() { return m_petAction; }
	CBattleTarget *GetPlayerTarget() { return m_playerTarget; }
	CBattleTarget *GetPetTarget() { return m_petTarget; }

	void SetAllListWidgetItemCheckState(Qt::CheckState nCheckState);
	QList<int> GetCopyToOtherSet() { return m_nOtherEnemySet; }

	void SetCopyToMultiBattleGroupName(const QString &name);
	void SetCopyToMultiBattleListName(const QString &name);

public slots:
	void on_comboBox_playerAction_currentIndexChanged(int index);
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();
	void doItemClicked(QListWidgetItem *pItem);

private:
	Ui::BattleSetDlg ui;
	QString m_sCopyToMultiName;
	CBattleSettingPtr m_pSetting;

	CBattleAction *m_playerAction = nullptr;
	CBattleTarget *m_playerTarget = nullptr;
	CBattleAction *m_petAction = nullptr;
	//CBattleAction *m_petAction2;
	CBattleTarget *m_petTarget = nullptr;
	//CBattleTarget *m_petTarget2;
	CBattleTarget *m_defaultTarget = nullptr;

	QListWidgetItem *m_pAllItem;
	QList<int> m_nOtherEnemySet;
};
