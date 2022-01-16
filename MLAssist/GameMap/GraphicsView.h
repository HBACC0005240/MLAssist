#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QPaintEvent>
#include <QPainter>
#include <QVariantAnimation>
#include <QLabel>
#include "GameMapWall.h"

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

class GraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit GraphicsView(QWidget *parent = 0);
	~GraphicsView();
	enum GRAPHICSTOOL
	{
		GRAP_TOOL_Pan = 0,
		GRAP_TOOL_ZoomIn,
		GRAP_TOOL_ZoomOut,
		GRAP_TOOL_Restore,
	};
	void init();
	void setGrapTool(int nTool) { m_nToolState = nTool; }
	int getGrapTool() { return m_nToolState; }
	void SetDisplayMap(bool bFlag);
	QGraphicsScene* GetScene() { return mScene; }
protected:
	void moveEvent(QMoveEvent *event);
	void resizeEvent(QResizeEvent *);
	void paintEvent(QPaintEvent *event);
	void timerEvent(QTimerEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

public slots:

	void zoomIn();
	void zoomOut();
	void zoomByFactor(double scaleFactor, const QPointF *center);
	void zoomReset();
	void translate(QPointF delta);
private:
	QPoint m_lastPointF;
	int m_nToolState;				//当前工具项
	bool m_bPan ;
	bool m_bZoom ;					//拖拽
	QGraphicsRectItem* m_pZoomRectItem;
	qreal m_scale ;
	QGraphicsScene* mScene;
	double mWheelZoomFactor ;
	qreal m_translateSpeed ;		//平移速度	
	int m_count = 0;				//缩放次数限制

};
#endif // GRAPHICSVIEW_H
