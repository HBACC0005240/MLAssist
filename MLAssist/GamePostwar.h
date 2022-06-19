#pragma once

#include "GameCtrl.h"
#include "ui_GamePostwar.h"
#include <QWidget>

class GamePostwar : public QWidget
{
	Q_OBJECT

public:
	GamePostwar(QWidget *parent = Q_NULLPTR);
	~GamePostwar();

	void initRenItems(GameItemList itemHash);
	void initDieItems(GameItemList itemHash);
public slots:
	void doLoadJsConfig(QJsonObject &obj);
	void doSaveJsConfig(QJsonObject &obj);
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void on_pushButton_renAdd_clicked();
	void on_pushButton_dieAdd_clicked();
	void doSwitchAutoDrop(int state);
	void doSwitchAutoPile(int state);
	void doRenItemChanged(QListWidgetItem *item);
	void doDieItemChanged(QListWidgetItem *item);
	void doAddRenItemScript(QString, bool);
	void doAddDieItemScript(QString, bool);
	void doAddRenItem(GameItemPtr pItem, bool bCode = false);
	void doAddDieItem(GameItemPtr pItem);
	void on_checkBox_playerHp_stateChanged(int state);
	void on_checkBox_firstAidPet_stateChanged(int state);
	void on_checkBox_firstAidTeammate_stateChanged(int state);

	void on_lineEdit_playerHp_editingFinished();
	void on_comboBox_playerSkillLv_currentIndexChanged(int index);

	void on_comboBox_HurtLv_currentIndexChanged(int index);
	void on_comboBox_HealLv_currentIndexChanged(int index);
	void on_checkBox_HealPet_stateChanged(int state);
	void on_checkBox_HealTeammate_stateChanged(int state);

	void on_checkBox_playerEatMedicament_stateChanged(int state);
	void on_checkBox_playerEatMagic_stateChanged(int state);
	void on_checkBox_petEatMedicament_stateChanged(int state);
	void on_checkBox_petEatMagic_stateChanged(int state);
	void on_lineEdit_playerEatMedicament_editingFinished();
	void on_lineEdit_playerEatMagic_editingFinished();
	void on_lineEdit_petEatMedicament_editingFinished();
	void on_lineEdit_petEatMagic_editingFinished();
	void OnRenContextMenu(const QPoint &);
	void OnDieContextMenu(const QPoint &);
	void doRemoveRenInfo();
	void doRemoveDieInfo();
	void on_groupBox_equipProtect_toggled(bool);
	void on_lineEdit_equipDurable_editingFinished();
	void on_radioButton_sameEquip_clicked(bool bChecked);
	void on_radioButton_offLine_clicked(bool bChecked);
	void on_radioButton_sameType_clicked(bool bChecked);
	void on_radioButton_sameIDEquip_clicked(bool bChecked);
	void on_checkBox_noEquipOffline_stateChanged(int state);
	void on_checkBox_renEquip_stateChanged(int state);
	void on_checkBox_eatTime_stateChanged(int state);
	void on_checkBox_eatShenLan_stateChanged(int state);
	void on_checkBox_eatGouLiang_stateChanged(int state);
	void updateEquipProtect();
	void syncUiData();
	void doSwitchAutoEatUi(int, bool);
	void doSwitchAutoCureUi(bool, bool, bool, int, int);

private:
	Ui::GamePostwar ui;
};
