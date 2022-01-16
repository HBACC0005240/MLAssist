#pragma once

#include "GameCtrl.h"
#include "ui_GameNpcSymbolWgt.h"
#include <QWidget>

class GameNpcSymbolWgt : public QWidget
{
	Q_OBJECT

public:
	GameNpcSymbolWgt(QWidget *parent = Q_NULLPTR);
	~GameNpcSymbolWgt();

	void initTable();

protected:
	bool judgeNeedUpdate(const CGA_MapUnit_t &unit);
	bool removeUnitRow(const CGA_MapUnit_t &unit);

public slots:
	void OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
	void on_customContextMenu(const QPoint & pos);

private:
	Ui::GameNpcSymbolWgt ui;
	bool m_bFilterNoName=false;
	QMutex m_lockUpdateTable;
	CGA_MapUnits_t m_lastUnits;
	CGA_MapUnits_t m_units;
	QTime m_lastUpdate;
	QHash<quint64, QTableWidgetItem *> m_unitIDForItem;
};
