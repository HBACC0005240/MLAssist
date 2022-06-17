#pragma once

#include "GameData.h"
#include "ui_GameDataWgt.h"
#include <QWidget>
class GameDataWgt : public QWidget
{
	Q_OBJECT

public:
	GameDataWgt(QWidget *parent = Q_NULLPTR);
	~GameDataWgt();

	void setItemText(int row, int col, const QString &szText, const QColor &szColor = QColor("black"));
	void refreshBattleUI();
	QString FormaClockIn(int val);
public slots:
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);

	void doUpdateAttachInfo();
	void doClearUiInfo();
	void OnNotifyGameCharacterInfo(CharacterPtr char_info);
	void OnNotifyGetSkillsInfo(GameSkillList skills);
	void OnNotifyGetPetsInfo(GamePetList pets);

	void doUpdateMapData(QString name, int index1, int index2, int index3, int x, int y);
	void doUpdateBattleHpMp(int hp, int maxhp, int mp, int maxmp);
	void Active();

	void on_AutoAddPoint_name_editingFinished();
	void on_normalAddPointHp_stateChanged(int state);
	void on_normalAddPointAttack_stateChanged(int state);
	void on_normalAddPointDefence_stateChanged(int state);
	void on_normalAddPointAgility_stateChanged(int state);
	void on_normalAddPointMp_stateChanged(int state);
	void on_specialAddPointHp_stateChanged(int state);
	void on_specialAddPointAttack_stateChanged(int state);
	void on_specialAddPointDefence_stateChanged(int state);
	void on_specialAddPointAgility_stateChanged(int state);
	void on_specialAddPointMp_stateChanged(int state);

	void on_lineEdit_specialHp_editingFinished();
	void on_lineEdit_Attack_editingFinished();
	void on_lineEdit_defence_editingFinished();
	void on_lineEdit_agility_editingFinished();
	void on_lineEdit_specialMp_editingFinished();

private:
	Ui::GameDataWgt ui;
	Character m_Infos[20];
	QCheckBox *m_pRealUpdateCheckBox;
	QTime m_lastUpdatePlayerTime;
	QTime m_lastUpdatePlayerSkillTime;
	QTime m_lastUpdatePlayerPetTime;
};
