#pragma once

#include "GameCtrl.h"
#include "ui_GameItemsPosWgt.h"
#include <QWidget>

class GameItemsPosWgt : public QWidget
{
	Q_OBJECT

public:
	GameItemsPosWgt(QWidget *parent = Q_NULLPTR);
	~GameItemsPosWgt();

	void initTable();
	void initTableData();

	void clearTableData();

protected:
	bool judgeNeedUpdate(const CGA_MapUnit_t &unit);
	bool removeUnitRow(const CGA_MapUnit_t &unit);
	bool AutoPickupItems(); //一格内自动捡物

public slots:
	void OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
	void on_customContextMenu(const QPoint &pos);
	void on_pushButton_del_clicked();
	void on_pushButton_add_clicked();
	void on_groupBox_toggled(bool checked);
	void doListItemChanged(QListWidgetItem *pItem);
	void on_checkBox_follow_stateChanged(int state);
	void on_checkBox_fixedPoint_stateChanged(int state);
	void on_lineEdit_east_editingFinished();
	void on_lineEdit_south_editingFinished();

private:
	Ui::GameItemsPosWgt ui;
	bool m_bFollowMap=false;
	bool m_bAutoPickup = true; //自动捡物？
	QStringList m_pickNameList;
	QMutex m_lockUpdateTable;
	CGA_MapUnits_t m_lastUnits;
	CGA_MapUnits_t m_units;
	QTime m_lastUpdate;
	QHash<quint64, QTableWidgetItem *> m_unitIDForItem;
};
