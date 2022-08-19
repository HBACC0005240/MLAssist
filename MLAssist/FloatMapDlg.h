#pragma once

#include "GameCtrl.h"
#include "GameMapWall.h"
#include "ui_FloatMapDlg.h"
#include <QDialog>
class FloatMapDlg : public QDialog
{
	Q_OBJECT

public:
	FloatMapDlg(QWidget *parent = Q_NULLPTR);
	~FloatMapDlg();

	void LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
	void LoadMapInfo(QString name, int x, int y, int mapindex);
	void RepaintCollisionPixels(int xbase, int ybase, int xtop, int ytop);
	void SetCrosshair(bool bFlag);

signals:
	void signal_checkCross(bool);

protected:
	void hideEvent(QHideEvent *event);

public slots:
	void updateeUiMsg(const QString &sMsg);
	void UpdateMousePosition(int x, int y);
	void on_pushButton_go_clicked();
	void on_checkBox_cross_stateChanged(int state);
	void on_pushButton_AutoMaze_clicked();
	void on_pushButton_StopNav_clicked();
	void on_toolButton_clear_clicked();
	void on_toolButton_zoomIn_clicked();
	void on_toolButton_zoomOut_clicked();
	void on_toolButton_zoomFit_clicked();
	void on_lineEdit_TargetMazeName_editingFinished();
	void on_lineEdit_searchName_editingFinished();
	void on_pushButton_search_clicked();
	void on_pushButton_downMap_clicked();
	void on_pushButton_searchMazePos_clicked();
	void on_toolButton_downMap_clicked();
	void on_pushButton_questMap_clicked();

private:
	Ui::FloatMapDlg ui;
	GameMapWall *m_pGameMapWallItem; //��ͼǽ
	QString m_stopFloorName;		 //停止走迷宫名称 包含
};
