#pragma once

#include "ui_GameBattleSetWgt.h"
#include <QSettings>
#include <QWidget>

class GameBattleSetWgt : public QWidget
{
	Q_OBJECT

public:
	GameBattleSetWgt(QWidget *parent = Q_NULLPTR);
	~GameBattleSetWgt();

	void initListWidget();
public slots:
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void on_checkBox_AutoEncounter_clicked(bool bChecked);
	void on_comboBox_encounterInterval_currentIndexChanged(int index);
	void doSyncEncounterUi();
	void doStopAutoEncounterEnemy();
	void on_checkBox_IsShow_stateChanged(int state);
	void on_checkBox_transformation_stateChanged(int state);
	void on_checkBox_Cosplay_stateChanged(int state);
	void on_checkBox_AutoPetRiding_stateChanged(int state);

	void on_lineEdit_transPetName_editingFinished();
	void on_lineEdit_CosplayName_editingFinished();
	void updateEncounterSpeed(int speed);

	void on_checkBox_troop_stateChanged(int state);
	void on_checkBox_playerHp_stateChanged(int state);
	void on_checkBox_playerMp_stateChanged(int state);
	void on_checkBox_petHp_stateChanged(int state);
	void on_checkBox_petMp_stateChanged(int state);
	void on_checkBox_teammateHp_stateChanged(int state);

	void on_lineEdit_troopCount_editingFinished();
	void on_lineEdit_playerHp_editingFinished();
	void on_lineEdit_playerMp_editingFinished();
	void on_lineEdit_petHp_editingFinished();
	void on_lineEdit_petMp_editingFinished();
	void on_lineEdit_teammateHp_editingFinished();
	void doStopEncountrItemChanged(QListWidgetItem *);

private:
	Ui::GameBattleSetWgt ui;
};
