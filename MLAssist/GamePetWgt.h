#pragma once

#include "ui_GamePetWgt.h"
#include <QSettings>
#include <QWidget>
#include "GameCtrl.h"

class GamePetWgt : public QWidget
{
	Q_OBJECT

public:
	GamePetWgt(QWidget *parent = Q_NULLPTR);
	~GamePetWgt();

	void init();
	void setItemText(int row, int col, const QString &szText, const QColor &szColor = QColor("black"), const QColor &backColor = QColor("white"));
public slots:
	void OnNotifyGetPetsInfo(GamePetList pets);
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void on_checkBox_SummonPet_stateChanged(int state);
	void on_checkBox_RecallLoyalty_stateChanged(int state);
	void on_checkBox_RecallMp_stateChanged(int state);
	void on_lineEdit_RecallLoyalty_editingFinished();
	void on_lineEdit_RecallMp_editingFinished();
	void on_comboBox_SummonPet_currentIndexChanged(int index);
	void on_groupBox_DropPet_toggled(bool checked);
	void on_dropPet_CheckBox_stateChanged(int state);
	void on_dropPet_LineEdit_editingFinished();
	void on_pushButton_saveBook_clicked();
	void onTableCustomContextMenu(const QPoint &);
	void doChangePetName();
	void doChangePetState();

	void on_groupBox_autoAddPoint_toggled(bool checked);
	void on_AutoAddPoint_petName_editingFinished();
	void on_AutoAddPoint_petRealName_editingFinished();
	//void on_AutoAddPoint_nomal_toggled(bool checked);
	//void on_AutoAddPoint_burst_toggled(bool checked);
	void on_checkBox_normalEnabled_stateChanged(int state);
	void on_checkBox_burstEnabled_stateChanged(int state);
	void on_normalAddPointHp_clicked(bool checked);
	void on_normalAddPointAttack_clicked(bool checked);
	void on_normalAddPointDefence_clicked(bool checked);
	void on_normalAddPointAgility_clicked(bool checked);
	void on_normalAddPointMp_clicked(bool checked);
	void on_burstAddPointHp_clicked(bool checked);
	void on_burstAddPointAttack_clicked(bool checked);
	void on_burstAddPointDefence_clicked(bool checked);
	void on_burstAddPointAgility_clicked(bool checked);
	void on_burstAddPointMp_clicked(bool checked);
	void on_pushButton_reloadCalc_clicked();

private:
	Ui::GamePetWgt ui;
	QPushButton *m_pCalcPetBtn;
	QMap<int, QObject *> m_pDropCheckBoxMap;
	QMap<int, QObject *> m_pDropLineEditMap;
	QMap<int, QString> m_petState;

	QTableWidgetItem *m_pCurItem = nullptr;
};
