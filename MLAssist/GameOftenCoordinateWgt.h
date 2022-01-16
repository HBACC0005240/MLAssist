#pragma once

#include "ui_GameOftenCoordinateWgt.h"
#include <QListWidgetItem>
#include <QWidget>
#include <QMutex>

class GameOftenCoordinateWgt : public QWidget
{
	Q_OBJECT

public:
	GameOftenCoordinateWgt(QWidget *parent = Q_NULLPTR);
	~GameOftenCoordinateWgt();

	//void init();
	//void initTableData(ITGameMapPtr pGameMap);
	void clearTableData();

	static void runCrossMapNavigation(GameOftenCoordinateWgt* pThis,int tx,int ty,int tMapNumber);
signals:
	void updateMapName(QListWidgetItem *item);

public slots:
	//void doUpdateTgtList(QListWidgetItem *pItem);
	void OnAllMapContextMenu(const QPoint &);
	void OnOftenMapContextMenu(const QPoint &);
	/*void doAddToOftenMap();
	void doUpdateMapName(QListWidgetItem *curItem);
	void doRemoveOftenMap();
	void doButtonClicked();
	void doTableItemChanged(QTableWidgetItem *pItem);*/
	void on_pushButton_stop_clicked();
	void on_lineEdit_all_returnPressed();
	void on_pushButton_all_clicked();
	void on_lineEdit_often_returnPressed();
	void on_pushButton_often_clicked();
	void on_lineEdit_target_returnPressed();
	void on_pushButton_target_clicked();
	void searchItem(const QString &sText, QListWidget *pListWidget);
	void doUpdateCrossMapStatus(const QString& msg);

private:
	Ui::GameOftenCoordinateWgt ui;
	QMutex m_lockUpdateTable;
	//QHash<ITGameMapPtr, QListWidgetItem *> m_allMapForItem;
	//QHash<ITGameMapPtr, QListWidgetItem *> m_oftenMapForItem;
};
