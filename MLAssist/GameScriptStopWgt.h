#pragma once

#include <QWidget>
#include <QSettings>
#include "ui_GameScriptStopWgt.h"
#include <QListWidgetItem>

class GameScriptStopWgt : public QWidget
{
	Q_OBJECT

public:
	GameScriptStopWgt(QWidget *parent = Q_NULLPTR);
	~GameScriptStopWgt();


	void initListWidget();

public slots:
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void doStopScriptItemChanged(QListWidgetItem *pItem);
	void on_checkBox_stopPlayerHp_stateChanged(int state);
	void on_checkBox_stopPlayerMp_stateChanged(int state);
	void on_checkBox_stopPetHp_stateChanged(int state);
	void on_checkBox_stopPetMp_stateChanged(int state);
	void on_checkBox_stopTroopCount_stateChanged(int state);
	void on_lineEdit_petMp_editingFinished();
	void on_lineEdit_petHp_editingFinished();
	void on_lineEdit_playerMp_editingFinished();
	void on_lineEdit_playerHp_editingFinished();
	void on_lineEdit_troopCount_editingFinished();
	void on_checkBox_hpPercentage_stateChanged(int state);
	void on_checkBox_mpPercentage_stateChanged(int state);
	void on_checkBox_petHpPercentage_stateChanged(int state);
	void on_checkBox_petMpPercentage_stateChanged(int state);


private:
	Ui::GameScriptStopWgt ui;
};
