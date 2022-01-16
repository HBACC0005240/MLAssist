#pragma once

#include "AutoBattle.h"
#include "ui_GameBattleWgt.h"
#include <QSound>
#include <QWidget>

class GameBattleWgt : public QWidget
{
	Q_OBJECT

public:
	GameBattleWgt(QWidget *parent = Q_NULLPTR);
	~GameBattleWgt();

	enum EscapePetAction
	{
		EscapePetAction_Attack = 1,
		EscapePetAction_Guard = 2,
		EscapePetAction_Nothing = 3
	};
	void init();
	void CreateSetting(int tType, int nVal, int nObjType, CBattleAction *pPlayerAction, CBattleAction *pPetAction, CBattleTarget *pPlayerTarget, CBattleTarget *pPetTarget);
	void PopBattleSetDlg(int nType, int nVal, int nDevType);
	void CreatePetActionTarget(CBattleAction *&pPetAction, CBattleTarget *&pPetTarget);

protected:
	void UpdateBattleSetting(int tType, double nVal);
	void CreateNoPetDoubleAction();
	void ResetNumEnemyBtnColor();
	void ResetNumRoundBtnColor();

public slots:
	void doLoadJsConfig(QJsonObject &obj);
	void doSaveJsConfig(QJsonObject &obj);
	void doSetBattleTactics(); //数量设置
	void doSetBattleRound();   //回合设置
	void on_pushButton_clearNum_clicked();
	void on_pushButton_clearRound_clicked();
	void on_comboBox_highSpeedDelay_currentIndexChanged(int index);
	void on_pushButton_TroopHp_clicked();
	void on_pushButton_PetMp_clicked();
	void on_pushButton_PetHp_clicked();
	void on_pushButton_TeammateHp_clicked();
	void on_pushButton_PlayerMp2_clicked();
	void on_pushButton_PlayerMp1_clicked();
	void on_pushButton_PlayerHp2_clicked();
	void on_pushButton_PlayerHp1_clicked();
	void on_checkBox_selfHp1_stateChanged(int state);
	void on_checkBox_selfHp2_stateChanged(int state);
	void on_checkBox_selfMp1_stateChanged(int state);
	void on_checkBox_selfMp2_stateChanged(int state);
	void on_checkBox_teammateHp1_stateChanged(int state);
	void on_checkBox_petHp_stateChanged(int state);
	void on_checkBox_petMp_stateChanged(int state);
	void on_checkBox_troopHp_stateChanged(int state);
	void on_lineEdit_PlayerHp1_editingFinished();
	void on_lineEdit_PlayerHp2_editingFinished();
	void on_lineEdit_PlayerMp1_editingFinished();
	void on_lineEdit_PlayerMp2_editingFinished();
	void on_lineEdit_PetHp_editingFinished();
	void on_lineEdit_PetMp_editingFinished();
	void on_lineEdit_TeammateHp_editingFinished();
	void on_lineEdit_TroopHp_editingFinished();

	void on_checkBox_allEscape_stateChanged(int state);
	void on_checkBox_NoLv1Escape_stateChanged(int state);
	void on_checkBox_noBossEscape_stateChanged(int state);
	void on_pushButton_EscapePetSet_clicked(); //逃跑宠物技能设置
	//void on_radioButton_EscapeNothing_clicked();
	//void on_radioButton_EscapeGuard_clicked();
	//void on_radioButton_EscapeAttack_clicked();
	void on_groupBox_SpecialEnemyEscape_toggled(bool checked);
	void on_pushButton_AddSpecialEnemy_clicked();
	void doSpecialEnemyItemChanged(QListWidgetItem *pItem);
	void on_checkBox_EnemyAvgLv_stateChanged(int state);
	void on_checkBox_EnemyCount_stateChanged(int state);
	void on_checkBox_TeamCount_stateChanged(int state);
	void on_lineEdit_TeamCount_editingFinished();
	void on_lineEdit_EnemyCount_editingFinished();
	void on_lineEdit_EnemyAvgLv_editingFinished();
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void on_checkBox_OverTimeT_stateChanged(int state);
	void on_horizontalSlider_moveSpeed_valueChanged(int value);
	void doPlayAlarmWav();
	void doStopPalyAlarm();
	void on_checkBox_Lv1FilterHp_stateChanged(int state);
	void on_checkBox_Lv1FilterMp_stateChanged(int state);
	void on_lineEdit_Lv1FilterHp_editingFinished();
	void on_lineEdit_Lv1FilterMp_editingFinished();
	void on_lineEdit_Lv1HpVal_editingFinished();

	void on_groupBox_Have1LvEnemy_toggled(bool checked);
	void on_checkBox_Lv1RoundOne_stateChanged(int state); //1级怪第一回合设置
	void on_pushButton_Lv1RoundOneSet_clicked();		  //1级怪第一回合设置
	void on_checkBox_Lv1Hp_stateChanged(int state);
	void on_pushButton_Lv1HpSet_clicked();
	void on_checkBox_Lv1LastSet_stateChanged(int state);
	void on_pushButton_Lv1LastSet_clicked();
	void on_pushButton_Lv1CleanNo1_clicked();
	void on_checkBox_Lv1CleanNo1_stateChanged(int state);
	void on_checkBox_Lv1Recall_stateChanged(int state);
	void on_groupBox_noPet_toggled(bool checked);
	void on_checkBox_noPetFirstUse_stateChanged(int state);

	void on_radioButton_attack_clicked(bool checked);
	void on_radioButton_defense_clicked(bool checked);
	void on_radioButton_escape_clicked(bool checked);

	void on_pushButton_petDoubleAction_clicked();

private:
	Ui::GameBattleWgt ui;
	//逃跑时候宠物默认动作和目标
	//懒得写对话框了 这里加个配置，直接复用对话框
	CBattleSettingPtr m_pEscapeSetting;

	CBattleAction *m_pEscapePetAction = nullptr;
	CBattleTarget *m_pEscapePetTarget = nullptr;
	int m_nEscapePetAction = EscapePetAction_Guard;
	QList<QCheckBox *> m_pBaohuCheckBoxList;
	QList<QLineEdit *> m_pBaohuLineEditList;

	QList<QCheckBox *> m_pLv1CheckBoxList;
	QList<QLineEdit *> m_pLv1LineEditList;
	QSound *m_pSoundAlarm;
};
