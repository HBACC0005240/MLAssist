#pragma once

#include "ui_GameScriptSetWgt.h"
#include <QJsonArray>
#include <QSettings>
#include <QWidget>
class GameScriptSetWgt : public QWidget
{
	Q_OBJECT

public:
	GameScriptSetWgt(QWidget *parent = Q_NULLPTR);
	~GameScriptSetWgt();

	void initListWidget();
	void createListWidgetItem(QVariantList userData);

protected:
	bool readCustomJson();

public slots:
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void doCustomScript_editingFinished();
	void doCustomScriptItemChanged(QListWidgetItem *pItem);

	void on_pushButton_fetchTeamData_clicked();
	void on_pushButton_fetchTeammateName_clicked();
	void on_comboBox_leaderName_currentTextChanged(const QString &text);
	void on_lineEdit_teammateName_editingFinished();
	void on_lineEdit_playerHpUI_editingFinished();
	void on_lineEdit_playerMpUI_editingFinished();
	void on_lineEdit_petMpUI_editingFinished();
	void on_lineEdit_petHpUI_editingFinished();
	void on_lineEdit_troopCountUI_editingFinished();

	void on_checkBox_hpPercentageUI_stateChanged(int state);
	void on_checkBox_mpPercentageUI_stateChanged(int state);
	void on_checkBox_petMpPercentageUI_stateChanged(int state);
	void on_checkBox_petHpPercentageUI_stateChanged(int state);
	QVariant GetUserDefineData(const QString &name, QString sVal);

private:
	Ui::GameScriptSetWgt ui;
	QMap<QString, QVariantList> m_userCustomScriptData;
	QMap<QString, QLineEdit *> m_nameForEdit;
};
