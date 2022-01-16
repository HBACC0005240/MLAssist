#pragma once

#include "ui_GameEquipWgt.h"
#include <QWidget>
#include <QTime>
#include "GameCtrl.h"
class GameEquipWgt : public QWidget
{
	Q_OBJECT

public:
	GameEquipWgt(QWidget *parent = Q_NULLPTR);
	~GameEquipWgt();

protected:
	void updatePlayerImage();
	void updateEquipImage(QLabel *pLabel, const int &imageid);

public slots:
	void doUpdateItemList(GameItemList pItemList);
	void on_pushButton_logOut_clicked();
	void on_pushButton_logBack_clicked();
	void on_checkBox_update_stateChanged(int state);
	void on_equip_ContextMenuRequested(const QPoint &pos);
	void on_unequip_item();

private:
	Ui::GameEquipWgt ui;
	QTime m_lastUpdateTime;
	bool m_bRealTimeUpdate = false;
	int m_equipPos = -1;
};
