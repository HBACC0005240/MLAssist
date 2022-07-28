#pragma once

#include <QWidget>
#include "ui_GameOnlineInfo.h"

class GameOnlineInfo : public QWidget
{
	Q_OBJECT

public:
	GameOnlineInfo(QWidget *parent = nullptr);
	~GameOnlineInfo();


public slots:
	void doUpdateTimer();

private:
	Ui::GameOnlineInfoClass ui;
};
