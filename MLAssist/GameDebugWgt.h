#pragma once

#include <QWidget>
#include "ui_GameDebugWgt.h"

class GameDebugWgt : public QWidget
{
	Q_OBJECT

public:
	GameDebugWgt(QWidget *parent = Q_NULLPTR);
	~GameDebugWgt();


public slots:
	void doAddOneMsg(const QString& msg);
	void on_checkBox_enable_stateChanged(int state);

private:
	Ui::GameDebugWgt ui;
	bool m_bEnableDebug=false;
};
