#pragma once

#include "ui_GameSysCfgWgt.h"
#include <QKeySequence>
#include <QSettings>
#include <QWidget>
class GameSysCfgWgt : public QWidget
{
	Q_OBJECT

public:
	GameSysCfgWgt(QWidget *parent = Q_NULLPTR);
	~GameSysCfgWgt();

	void init();

protected:
	bool GetInputKey(QString &inputKey);

public slots:
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void on_pushButton_fetchItem_clicked();
	void on_pushButton_saveItem_clicked();
	void on_pushButton_fetchPet_clicked();
	void on_pushButton_savePet_clicked();
	void on_pushButton_fetchGold_clicked();
	void on_pushButton_saveGold_clicked();
	void on_pushButton_dropGold_clicked();

	void on_toolButton_logBack_clicked();
	void on_toolButton_logOut_clicked();
	void on_toolButton_cross_clicked();
	void on_toolButton_trade_clicked();
	void on_toolButton_tradeNoAccept_clicked();
	void on_toolButton_saveAll_clicked();
	void on_toolButton_fetchAll_clicked();
	void on_toolButton_often_map_clicked();
	void on_pushButton_sortBag_clicked();
	void on_pushButton_sortBank_clicked();
	void on_checkBox_autoSaveBank_stateChanged(int state);

private:
	Ui::GameSysCfgWgt ui;
};
