#ifndef MAPFORM_H
#define MAPFORM_H

#include "FloatMapDlg.h"
#include "GameCtrl.h"
#include "ui_mapform.h"
#include <QWidget>

class MapForm : public QWidget
{
	Q_OBJECT

public:
	explicit MapForm(QWidget *parent = nullptr);
	~MapForm();

protected:
	virtual void keyPressEvent(QKeyEvent *event);
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);

	static void AutoMazeThread(MapForm *pThis);
	static void AutoMazeOpen(MapForm *pThis);

public slots:
	void OnNotifyRefreshMapRegion(int xbase, int ybase, int xtop, int ytop, int index3);
	void OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
	void OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y);
	void OnReportNavigatorPath(QString json);
	void OnReportNavigatorFinish(int exitCode);
	void OnCloseWindow();
	void RunNavigatorScript(int x, int y, int enter, QString *result);
	void StopNavigatorScript();
	void UpdateMousePosition(int x, int y);
	void on_pushButton_stopNav_clicked();
	void on_pushButton_maxMap_clicked();
	void on_pushButton_AutoMaze_clicked();
private slots:
	void on_pushButton_loadmap_clicked();
	void on_maxMapDlg_checkCross(bool bFlag);
	void on_pushButton_go_clicked();
signals:
	void RequestDownloadMap(int xsize, int ysize);
	void runNavigatorScript(int x, int y, int enter, QString *result);
	void stopNavigatorScript();

private:
	Ui::MapForm ui;
	QSharedPointer<CGA_MapCellData_t> m_collision;
	FloatMapDlg *m_pFloatMapDlg;
};

#endif // MAPFORM_H
