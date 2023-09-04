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
	static bool SortObjectFun(const ITObjectPtr p1, const ITObjectPtr p2);

protected:
	void resizeEvent(QResizeEvent *ev);
public slots:
	void doUpdateTimer();

private:
	Ui::GameOnlineInfoClass ui;
	ITTableModel *m_pTableModel;
	int m_nTryCount=0;
};
