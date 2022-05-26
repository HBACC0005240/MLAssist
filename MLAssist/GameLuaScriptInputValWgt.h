#pragma once

#include "ui_GameLuaScriptInputValWgt.h"
#include <QGridLayout>
#include <QSettings>
#include <QWidget>

class GameLuaScriptInputValWgt : public QWidget
{
	Q_OBJECT

public:
	GameLuaScriptInputValWgt(QWidget *parent = Q_NULLPTR);
	~GameLuaScriptInputValWgt();

	void ClearAllInputWidget();

public slots:
	//增加一个用户自定义变量
	void AddOneInputVar(int type, const QVariant &sMsg, const QVariant &vData, const QVariant &vData2);
	void on_pushButton_save_clicked();
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);

protected:
	void increaseRowCol();

private:
	Ui::GameLuaScriptInputValWgt ui;
	int m_lastRow = 0;
	int m_lastCol = 0;
	QGridLayout *m_pGridLayout;
	QList<QWidget *> m_pAllInputWidget;
	QMap<QVariant, QPair<int, QWidget *> > m_pKeyForWidget;
};
