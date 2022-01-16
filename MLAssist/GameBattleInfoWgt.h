#pragma once

#include "GameCtrl.h"
#include "ui_GameBattleInfoWgt.h"
#include <QWidget>

class GameBattleInfoWgt : public QWidget
{
	Q_OBJECT

public:
	GameBattleInfoWgt(QWidget *parent = Q_NULLPTR);
	~GameBattleInfoWgt();

	void init();
	void setItemText(int row, int col, const QString &szText, const QColor &szColor, const QColor &backColor);
	void setTeamItemText(int row, int col, const QString &szText, const QColor &szColor, const QColor &backColor);
public slots:
	void DoNotifyTeamInfo(const QList<QSharedPointer<GameTeamPlayer> > &teamInfos);
	void OnNotifyGetBattleInfo(GameBattleUnitList units);

private:
	Ui::GameBattleInfoWgt ui;
	QHash<int, QPoint> m_battleUnitForCell; //站位信息和表格映射
};
