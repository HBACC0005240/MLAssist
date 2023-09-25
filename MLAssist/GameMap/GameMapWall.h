#pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("UTF-8")
#endif
#include "GameCtrl.h"
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QVariantAnimation>
#include <QWidget>

class GameMapWall : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
public:
	explicit GameMapWall();
	~GameMapWall();

	void MoveToTarget(int x, int y);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
	QRectF boundingRect() const;
	//! returns canvas item rectangle in map units
	QRect rect() const;
	void setRect(const QRect &r);

	void dealTimerEvent();
	bool IsDisplayMap();
	void SetDisplayMap(bool bFlag) { m_bDisplayMap = bFlag; }
	void ResetSelect();
	static void RunNavigator(GameMapWall *pThis, int x, int y, int enter, QString *result);
	void AutoWallMaze();
	static void AutoWallMazeThread(GameMapWall *pThis);
	void SetTargetMazeName(const QString &sTgtName) { m_targetMazeName = sTgtName; }
	void StopAutoMaze();
	void SetSearchData(const QString &sName, int ntype);
	void RunSearch();
	void DownLoadMap();
	void QuestMap();
	void SaveMapData();

	static void SearchThread(GameMapWall *pThis);
	static void AutoMazeOpen(GameMapWall *pThis);
	void ResetMapInfo();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
signals:
	void updateMousePosition(int x, int y);
	void RequestDownloadMap(int x, int y);
	void updateUIMsg(const QString &sMsg);

public slots:
	void LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
	void LoadMapInfo(QString name, int x, int y, int mapindex);
	void LoadNavPath(QVector<quint32> navpath);
	void RepaintCollisionPixels(int xbase, int ybase, int xtop, int ytop);

private:
	QRect mRect;
	bool m_bDisplayMap = false;
	QSizeF mItemSize;
	QSharedPointer<QRgb> m_pixels;
	QSharedPointer<QRgb> m_pixels2;
	size_t m_pixelwidth;
	size_t m_pixelheight;
	QString m_naverror;
	int m_nav_x;
	int m_nav_y;
	int m_movecamera_x;
	int m_movecamera_y;
	int m_mapindex;
	int m_playerx, m_playery, m_playermapindex;
	int m_select_unitid;
	int m_select_unitx;
	int m_select_unity;
	QString m_select_unit_name;
	QSharedPointer<CGA_MapCellData_t> m_collision, m_objects;
	QSharedPointer<CGA_MapUnits_t> m_units;
	QVector<quint32> m_navpath;
	int m_navPathMapIndex = 0; //同步存放寻路线地图index
	QString m_targetMazeName;
	bool m_bAutoMaze = false; //自动寻路
	QString m_searchNpcName;
	int m_searchType = 1;
	QFuture<void> m_runNavigatorFuture;

public:
	bool m_bShowCrosshair = false;
};
