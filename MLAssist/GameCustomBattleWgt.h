#pragma once

#include "BattleSetting.h"
#include "GameCtrl.h"
#include "ui_GameCustomBattleWgt.h"
#include <QWidget>

class GameCustomBattleWgt : public QWidget
{
	Q_OBJECT

public:
	GameCustomBattleWgt(QWidget *parent = Q_NULLPTR);
	~GameCustomBattleWgt();

	void init();

protected:
	int calcTextRow(QFont &font, QString sText, int totalWidth);
	void showEvent(QShowEvent *event);

signals:
	void signal_float_window();

private slots:
	void OnCloseWindow();
	void on_comboBox_condition_type_currentIndexChanged(int index);
	void on_comboBox_condition2_type_currentIndexChanged(int index);
	void on_comboBox_playerAction_currentIndexChanged(int index);
	void on_comboBox_playerTarget_currentIndexChanged(int index);
	void on_comboBox_petAction_currentIndexChanged(int index);
	void on_comboBox_petTarget_currentIndexChanged(int index);
	void on_pushButton_add_clicked();
	void on_pushButton_del_clicked();
	void on_pushButton_moveUp_clicked();
	void on_pushButton_moveDown_clicked();
	void on_table_customContextMenu(const QPoint &pos);

public slots:
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	bool ParseBattleSettings(const QJsonValue &val);
	void SaveBattleSettings(QJsonObject &obj);
	void showToolTip(const QModelIndex &);
	void OnNotifyGetSkillsInfo(GameSkillList skills);
	void OnNotifyGetItemsInfo(GameItemList items);
	void OnNotifyGetPetsInfo(GamePetList pets);
	void on_pushButton_float_clicked();

private:
	Ui::GameCustomBattleWgt ui;

	CBattleSettingModel *m_model;
	QList<int> m_targetSelect;
	QList<int> m_targetTeammateSelect;
	QList<int> m_conditionCompare;
	QList<int> m_conditionCompareString;
	QMap<int, QString> hBattlePlayerActionString; //人物动作
	QMap<int, QString> hBattleTargetString;		  //目标
	QMap<int, QString> hPetActionString;		  //宠物动作
	QMap<int, QString> hTargetCondString;		  //优先规则
	GamePetList m_pets;
	GameItemList m_items;
};
