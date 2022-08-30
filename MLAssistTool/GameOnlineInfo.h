#pragma once

#include "ITTableModel.h"
#include "ui_GameOnlineInfo.h"
#include <QWidget>

class GameOnlineInfo : public QWidget
{
	Q_OBJECT

public:
	GameOnlineInfo(QWidget *parent = nullptr);
	~GameOnlineInfo();

	void init();
	static bool SortStringFun(const QString &s1, const QString &s2);

public slots:
	void doUpdateTimer();

private:
	Ui::GameOnlineInfoClass ui;
	ITTableModel *m_pTableModel;
};
