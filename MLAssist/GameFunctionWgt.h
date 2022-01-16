#pragma once

#include "GlobalDefine.h"
#include "ui_GameFunctionWgt.h"
#include <QSettings>
#include <QWidget>
class GameFunctionWgt : public QWidget
{
	Q_OBJECT

public:
	GameFunctionWgt(QWidget *parent = Q_NULLPTR);
	~GameFunctionWgt();

	void init();

public slots:
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void doLoadJsConfig(QJsonObject &obj);
	void doSaveJsConfig(QJsonObject &obj);

private:
	Ui::GameFunctionWgt ui;
	QCheckBox *m_pRealUpdateCheckBox;
	QCheckBox *m_pEnableUpdateCheckBox;
};
