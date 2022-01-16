#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("UTF-8")
#endif
#include "graphicsView.h"
#include <QDebug>
#include <QTimerEvent>
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>

GraphicsView::GraphicsView(QWidget *parent)
    :QGraphicsView(parent)
{
	m_nToolState = GRAP_TOOL_Pan;	//当前工具项
	m_bPan = false;
    m_bZoom = false;//拖拽
	m_pZoomRectItem = nullptr;
	m_scale = 1;
	mWheelZoomFactor = 2.0;
	m_translateSpeed = 1.0;  // 平移速度

	mScene = new QGraphicsScene();
	this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setRenderHint(QPainter::Antialiasing);
	setScene(mScene);
	setMouseTracking(true);
	QSize s = viewport()->size();
	mScene->setSceneRect(0, 0, this->width(), this->height());
	//m_pGameMapWallItem = new GameMapWall();
	//m_pGameMapWallItem->setRect(mScene->sceneRect().toRect());
	//m_pGameMapWallItem->setVisible(true);
	//mScene->addRect(mScene->sceneRect(), QPen(Qt::red));
	//m_pGameMapWallItem->setPos(0,0);
	//m_pGameMapWallItem->update();
	//mScene->addItem(m_pGameMapWallItem);
	//mScene->update();

	//setAttribute(Qt::WA_TranslucentBackground);
	init();
}

GraphicsView::~GraphicsView()
{
	
}
void GraphicsView::init()
{
	QRect tRect = this->viewport()->rect();
	mScene->setSceneRect(tRect);
	auto sceneItems= mScene->items();
	for (auto pItem:sceneItems)
	{
		qDebug() << "Resize";
		GameMapWall* pWall = dynamic_cast<GameMapWall*>(pItem);
		if (pWall)
		{
			pWall->setRect(tRect);
			pWall->setPos(0, 0);
			qDebug() << "GameMapWall Resize"<< tRect;
		}
	}
	//m_pGameMapWallItem->setRect(tRect);
	//m_pGameMapWallItem->setPos(0,0);	
}

void GraphicsView::SetDisplayMap(bool bFlag)
{
	/*if (m_pGameMapWallItem)
	{
		m_pGameMapWallItem->SetDisplayMap(bFlag);
	}*/
}

void GraphicsView::zoomIn()
{
	this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
	if (m_count < 10)
	{
		scale(5.0 / 4.0, 5.0 / 4.0);
		m_count++;
	}
}

void GraphicsView::zoomOut()
{
	this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
	if (m_count > 0)
	{
		scale(0.8, 0.8);
		m_count--;
	}
}
void GraphicsView::zoomReset()
{
	this->resetMatrix();
	qreal sceneWidth = mScene->width();
	qreal scnenHeight = mScene->height();
	int width = this->width();
	int height = this->height();
	qreal scaleX = width / sceneWidth;
	qreal scaleY = height / scnenHeight;

	///按照最小的缩放比例进行缩放
	qreal tempScale = scaleX < scaleY ? scaleX : scaleY;
	this->scale(tempScale, tempScale);
	m_count = 0;
}
void GraphicsView::zoomByFactor(double scaleFactor, const QPointF *center)
{
	qreal scale = qPow(qreal(2), scaleFactor);

	QMatrix matrix;
	matrix.scale(scale, scale);
	matrix.rotate(0);

	this->setMatrix(matrix);
}

void GraphicsView::moveEvent(QMoveEvent *event)
{
	Q_UNUSED(event);
}
void GraphicsView::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	init();

}
void GraphicsView::paintEvent(QPaintEvent *event) 
{
	Q_UNUSED(event);
	QGraphicsView::paintEvent(event);
}
void GraphicsView::timerEvent(QTimerEvent *event)
{
	/*{
		m_pRadarItem->dealTimerEvent();
		update();
	}*/
}


void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	QPoint p = event->pos();	
	update();
	switch (m_nToolState)
	{
	case GRAP_TOOL_Pan:
	{
		if (m_bPan)
		{
			QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastPointF);
			translate(mouseDelta);
			m_lastPointF = event->pos();
		}
		break;
	}
	case GRAP_TOOL_ZoomIn:
	case  GRAP_TOOL_ZoomOut:
	{
		if (m_bZoom)
		{
			QPointF disPointF = event->pos() - m_lastPointF;
			m_pZoomRectItem->setRect(0, 0, disPointF.x(), disPointF.y());
		}
		break;
	}
	default:
		break;
	}	
	QGraphicsView::mouseMoveEvent(event);	
}
// 平移
void GraphicsView::translate(QPointF delta)
{
	// 根据当前 zoom 缩放平移数
	delta *= m_scale;
	delta *= m_translateSpeed;

	// view 根据鼠标下的点作为锚点来定位 scene
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
	centerOn(mapToScene(newCenter));
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{	
	if (event->button() == Qt::LeftButton) 
	{
		m_lastPointF = event->pos();
		switch (m_nToolState)
		{
			case GRAP_TOOL_Pan:
			{
				// 当光标底下没有 item 时，才能移动
				QPointF point = mapToScene(event->pos());
				//if (scene()->itemAt(point, transform()) == NULL) 
				{
					m_bPan = true;

				}
				break;
			}
			case GRAP_TOOL_ZoomIn:
			case GRAP_TOOL_ZoomOut:
			{
				m_bZoom = true;
				if (!m_pZoomRectItem)
					m_pZoomRectItem = new QGraphicsRectItem();
				m_pZoomRectItem->setPos(m_lastPointF);
				m_pZoomRectItem->setPen(QPen(Qt::red));
				m_pZoomRectItem->setRect(0, 0, 0, 0);
				m_pZoomRectItem->setVisible(true);
				mScene->addItem(m_pZoomRectItem);
				break;
			}
			default:break;
		}		
	}
	QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		switch (m_nToolState)
		{
		case GRAP_TOOL_Pan:
		{
			m_bPan = false;
			break;
		}
		case GRAP_TOOL_ZoomIn: {zoomReset(); m_bZoom = false; break; }
		case GRAP_TOOL_ZoomOut:
		{
			m_bZoom = false;
			scale(10, 10);
			if (m_pZoomRectItem)
				m_pZoomRectItem->setVisible(false);
		}
		default:break;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
	//	zoomReset();
	}
	
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{	
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	if (numSteps < 0)	//向上滚  缩小
	{
		numSteps = abs(numSteps);
		while (numSteps--)
		{
			zoomOut();
		}
	}
	else if (numSteps > 0)	//向下滚  放大
	{
		numSteps = abs(numSteps);
		while (numSteps--)
		{
			zoomIn();
		}
	}
	event->accept();
}

