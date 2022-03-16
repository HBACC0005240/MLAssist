#ifndef MYPAINTMAP_H
#define MYPAINTMAP_H

#include "GameCtrl.h"
#include <QMenu>
#include <QPaintEvent>
#include <QWidget>

class MyPaintMap : public QWidget
{
	Q_OBJECT

public:
	MyPaintMap(QWidget *parent);

	void LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
	void LoadMapInfo(QString name, int x, int y, int mapindex);
	void LoadNavPath(QVector<quint32> navpath);
	void RepaintCollisionPixels(int xbase, int ybase, int xtop, int ytop);
	void ShowNavError(QString err);
	virtual void keyPressEvent(QKeyEvent *event);
	static void RunNavigator(MyPaintMap *pThis, int x, int y, int enter, QString *result);
	void ResetMapInfo();

signals:
	void updateMousePosition(int x, int y);
	void runNavigatorScript(int x, int y, int enter, QString *result);
	void stopNavigatorScript();
public slots:
	void zoomIn();			// 放大
	void zoomOut();			// 缩小
	void zoom(float scale); // 缩放 - scaleFactor：缩放的比例因子
	void MoveToTarget(int x, int y);
	void on_load_navpath(QVector<quint32> navpath);

	//void translate(QPointF delta);  // 平移
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
	//void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	//void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	//void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	QSharedPointer<CGA_MapCellData_t> m_collision, m_objects;
	QSharedPointer<CGA_MapUnits_t> m_units;
	QVector<quint32> m_navpath;
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
	QPointF m_zoomPos;

public:
	bool m_bShowCrosshair;
	bool m_bMoveCamera;

private:
	QMutex m_mapMutex;
	QSharedPointer<QRgb> m_pixels;
	QSharedPointer<QRgb> m_pixels2;
	size_t m_pixelwidth;
	size_t m_pixelheight;

	qreal horizontalOffset = 0;
	qreal verticalOffset = 0;
	qreal m_scaleFactor = 1; //缩放系数
	qreal currentStepScaleFactor = 1;
	Qt::MouseButton m_translateButton = Qt::LeftButton; // 平移按钮
	bool m_bMouseTranslate = false;
	qreal m_zoomDelta = 0.1; // 缩放的增量
	QPoint m_lastMousePos;	 // 鼠标最后按下的位置
};

#endif // MYPAINTMAP_H
