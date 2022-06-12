#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("UTF-8")
#endif
#include "GameMapWall.h"
#include "./AStar/AStar.h"
#include "graphicsView.h"
#include <QBrush>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <string>

using namespace std;
#define blockSize 2

GameMapWall::GameMapWall()
{
	mItemSize = QSizeF(100, 100);
	setVisible(true);
	setAcceptHoverEvents(true);
	setAcceptDrops(true);
	//setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable | ItemSendsGeometryChanges);
	setFocus();
	connect(this, &GameMapWall::RequestDownloadMap, g_pGameCtrl, &GameCtrl::OnDownloadMap, Qt::ConnectionType::QueuedConnection);
	qRegisterMetaType<QVector<quint32> >("QVector<quint32>");
	connect(g_pGameCtrl, SIGNAL(signal_load_navpath(QVector<quint32>)), this, SLOT(LoadNavPath(QVector<quint32>)), Qt::QueuedConnection);

	//	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapCellInfo, this, &GameMapWall::LoadMapCellInfo, Qt::ConnectionType::QueuedConnection);
	//	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapInfo, this, &GameMapWall::LoadMapInfo, Qt::ConnectionType::QueuedConnection);
	//	connect(g_pGameCtrl, &GameCtrl::NotifyRefreshMapRegion, this, &GameMapWall::RepaintCollisionPixels, Qt::ConnectionType::QueuedConnection);
}

GameMapWall::~GameMapWall()
{
}

QRect GameMapWall::rect() const
{
	return mRect;
}
void GameMapWall::setRect(const QRect &r)
{
	mRect = r;
	update();
}
QRectF GameMapWall::boundingRect() const
{
	return QRectF(QPointF(0, 0), QSizeF(mRect.width(), mRect.height()));
}

void GameMapWall::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	if (m_pixelwidth <= 0 || m_pixelheight <= 0)
		return;
	//	double scaleFactor = 1.0 / painter->matrix().m11();
	//	qDebug() << scaleFactor;
	//	painter->scale(scaleFactor, scaleFactor);
	//	prepareGeometryChange();
	auto widgetSize = mRect;
	QImage image((uchar *)m_pixels.get(), m_pixelwidth, m_pixelheight, QImage::Format_ARGB32);
	QImage image2((uchar *)m_pixels2.get(), m_pixelwidth, m_pixelheight, QImage::Format_ARGB32);
	painter->setRenderHint(QPainter::Antialiasing);
	QRectF renderRect(0, 0, widgetSize.width(), widgetSize.height());
	painter->drawImage(renderRect, image);
	painter->drawImage(renderRect, image2);

	if (m_collision)
	{
		auto newBlockWidth = blockSize * widgetSize.width() / m_pixelwidth;
		auto newBlockHeight = blockSize * widgetSize.height() / m_pixelheight;
		if (m_bShowCrosshair)
		{
			painter->setPen(Qt::red);
			int x = m_playerx * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
			int y = m_playery * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
			painter->drawLine(0, y, widgetSize.width(), y);
			painter->drawLine(x, 0, x, widgetSize.height());
		}
		else
		{
			painter->setPen(Qt::red);
			int x = m_playerx * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
			int y = m_playery * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
			painter->setBrush(QBrush(Qt::red, Qt::SolidPattern));
			painter->drawRect(x - 2, y - 2, 4, 4);
		}

		if (m_select_unitid)
		{
			painter->setPen(Qt::red);
			int x = m_select_unitx * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
			int y = m_select_unity * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
			painter->setBrush(QBrush(Qt::NoBrush));
			painter->drawEllipse(x - 4, y - 4, 8, 8);

			painter->setPen(Qt::white);
			painter->drawText(x + 10, y + 4, m_select_unit_name);

			painter->setPen(Qt::red);
			painter->drawText(x + 9, y + 3, m_select_unit_name);
		}

		if (m_naverror.size())
		{
			int x = m_nav_x * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
			int y = m_nav_y * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;

			painter->setPen(Qt::white);
			painter->drawText(x + 10, y + 4, m_naverror);

			painter->setPen(Qt::red);
			painter->drawText(x + 9, y + 3, m_naverror);
		}

		if (m_navpath.size() >= 2)
		{
			if (m_navPathMapIndex == m_playermapindex)
			{
				painter->setPen(Qt::green);
				for (int j = 1; j < m_navpath.size(); ++j)
				{
					int prevx = (m_navpath.at(j - 1) & 0xFFFF) * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
					int prevy = ((m_navpath.at(j - 1) >> 16) & 0xFFFF) * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
					int x = (m_navpath.at(j) & 0xFFFF) * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
					int y = ((m_navpath.at(j) >> 16) & 0xFFFF) * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
					painter->drawLine(prevx, prevy, x, y);
				}

				int endx = m_navpath.at(m_navpath.size() - 1) & 0xFFFF;
				int endy = (m_navpath.at(m_navpath.size() - 1) >> 16) & 0xFFFF;
				int x = endx * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
				int y = endy * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;

				QString pathend = tr("(%1, %2)").arg(endx).arg(endy);

				painter->setPen(Qt::white);
				painter->drawText(x + 10, y + 4, pathend);

				painter->setPen(Qt::red);
				painter->drawText(x + 9, y + 3, pathend);
			}
			else
			{
				m_navpath.clear();
			}
		}
	}

	painter->setBrush(QBrush(Qt::NoBrush));
	painter->setPen(hasFocus() ? Qt::red : Qt::black);
	painter->drawRect(renderRect);
}
void GameMapWall::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (!m_collision)
		return;
	if (event->button() == Qt::LeftButton)
	{
		auto widgetSize = mRect;
		{
			int mapX = event->pos().x() * m_collision->xsize / widgetSize.width();
			int mapY = event->pos().y() * m_collision->ysize / widgetSize.height();

			float mindist = 9999;

			int select_unit = 0;
			int select_unitx = 0;
			int select_unity = 0;
			QString select_unit_name;

			for (int i = 0; i < m_units->size(); ++i)
			{
				const auto &unit = m_units->at(i);
				if (unit.valid && unit.type == 1 && unit.model_id != 0 && (unit.flags & 4096) != 0)
				{
					int x = unit.xpos;
					int y = unit.ypos;

					auto d = sqrt((x - mapX) * (x - mapX) + (y - mapY) * (y - mapY));
					//if (d < 3.0 && d < mindist)
					if (x == mapX && y == mapY)
					{
						mindist = d;
						select_unit = unit.unit_id;
						select_unitx = unit.xpos;
						select_unity = unit.ypos;
						select_unit_name = unit.unit_name + QObject::tr(" (%1, %2)").arg(unit.xpos).arg(unit.ypos);
					}
				}
				else if (unit.valid && unit.type == 2 && unit.model_id != 0 && (unit.flags & 1024) != 0)
				{
					int x = unit.xpos;
					int y = unit.ypos;

					auto d = sqrt((x - mapX) * (x - mapX) + (y - mapY) * (y - mapY));
					//if (d < 3.0 && d < mindist)
					if (x == mapX && y == mapY)
					{
						mindist = d;
						select_unit = unit.unit_id;
						select_unitx = unit.xpos;
						select_unity = unit.ypos;
						select_unit_name = unit.item_name + QObject::tr(" (%1, %2)").arg(unit.xpos).arg(unit.ypos);
					}
				}
			}

			if (select_unit)
			{
				m_select_unitid = select_unit;
				m_select_unitx = select_unitx;
				m_select_unity = select_unity;
				m_select_unit_name = select_unit_name;
				update();
			}
			//	else
			{
				updateMousePosition(mapX, mapY);
			}
		}
	}

	else if (event->button() == Qt::RightButton)
	{
		auto widgetSize = mRect;
		{
			int mapX = event->pos().x() * m_collision->xsize / widgetSize.width();
			int mapY = event->pos().y() * m_collision->ysize / widgetSize.height();

			if (mapX >= 0 && mapX < m_collision->xsize && mapY >= 0 && mapY < m_collision->ysize && m_collision->cells.at((size_t)(mapX + mapY * m_collision->xsize)) != 1)
			{
				int enter = 0;
				if (mapX < m_objects->xsize && mapY < m_objects->ysize)
				{
					enter = (m_objects->cells.at((size_t)(mapX + mapY * m_objects->xsize)) & 0xFF);
				}
				m_nav_x = mapX;
				m_nav_y = mapY;
				m_naverror = QString();
				updateMousePosition(mapX, mapY);
				QtConcurrent::run(RunNavigator, this, mapX, mapY, enter, &m_naverror);
			}
		}
	}
	QGraphicsItem::mousePressEvent(event);
	update();
}

void GameMapWall::ResetSelect()
{
	m_navpath.clear();
	m_select_unitid = 0;
}

void GameMapWall::RunNavigator(GameMapWall *pThis, int x, int y, int enter, QString *result)
{
	if (pThis == nullptr)
		return;
	pThis->m_bAutoMaze = true;
	g_pGameFun->RestFun();
	QVector<quint32> navpath;
	QPoint curPos = g_pGameFun->GetMapCoordinate();
	auto findPath = g_pGameFun->CalculatePath(curPos.x(), curPos.y(), x, y);
	for (auto it : findPath)
	{
		navpath.push_back((it.first & 0xFFFF) | ((it.second & 0xFFFF) << 16));
	}
	pThis->LoadNavPath(navpath);
	g_pGameFun->AutoMoveTo(x, y);
	pThis->m_bAutoMaze = false;
	emit pThis->updateUIMsg(QString("到达目的地，寻路结束！"));
}

void GameMapWall::AutoWallMaze()
{
	if (m_bAutoMaze)
	{
		QMessageBox::information(nullptr, "提示：", "寻路中！", QMessageBox::Ok);
		return;
	}
	QtConcurrent::run(AutoWallMazeThread, this);
}

void GameMapWall::AutoWallMazeThread(GameMapWall *pThis)
{
	pThis->m_bAutoMaze = true;
	g_pGameFun->RestFun();
	int tryNum = 0;
	int beginFloor = g_pGameFun->GetMapFloorNumberFromName(); //开始楼层
	int lastFloor = beginFloor;
	while (pThis->m_bAutoMaze)
	{
		if (g_pGameCtrl->GetExitGame() || g_pGameFun->IsStop())
			break;
		//if (!g_pGameFun->IsMapDownload())
		//{
		//	g_pGameFun->DownloadMap();
		//	Sleep(2000); //等待2秒
		//}
		auto entranceList = g_pGameFun->GetMazeEntranceList();
		if (entranceList.size() < 2)
		{
			g_pGameFun->MakeMapOpenContainNextEntrance();
			qDebug() << "传送点小于1，重新下载地图";
			if (tryNum >= 3)
			{
				qDebug() << "未找到下一层坐标点，退出";
				break;
			}
			tryNum++;
			continue;
		}
		//迷宫多个出入点 返回空
		if (entranceList.size() > 2)
		{
			qDebug() << "迷宫多个出入点，退出";
			break;
		}
		QPoint curPos = g_pGameFun->GetMapCoordinate();
		qSort(entranceList.begin(), entranceList.end(), [&](auto a, auto b)
				{
					double aDistance = AStarUtil::octile(abs(a.x() - curPos.x()), abs(a.y() - curPos.y()));
					double bDistance = AStarUtil::octile(abs(b.x() - curPos.x()), abs(b.y() - curPos.y()));
					//qDebug() << aDistance << bDistance;
					return aDistance > bDistance;
				});
		QPoint targetEntrance = entranceList[0];
		QVector<quint32> navpath;
		auto findPath = g_pGameFun->CalculatePath(curPos.x(), curPos.y(), targetEntrance.x(), targetEntrance.y());
		for (auto it : findPath)
		{
			navpath.push_back((it.first & 0xFFFF) | ((it.second & 0xFFFF) << 16));
		}
		pThis->LoadNavPath(navpath);
		int moveRet = g_pGameFun->AutoMoveTo(targetEntrance.x(), targetEntrance.y());

		if (moveRet == 0)
		{
			break;
		}
		QString curMapName = g_pGameFun->GetMapName();
		if (curMapName.contains(pThis->m_targetMazeName))
		{
			emit pThis->updateUIMsg(QString("到达目标楼层，寻路结束！"));
			break;
		}
		else
		{															//判断楼层有没有反
			int curFloor = g_pGameFun->GetMapFloorNumberFromName(); //当前楼层
			if (curFloor == 0)										//获取错误 不判断楼层
				continue;
			else
			{
				if (curFloor < lastFloor) //一般都是大于 小于的还没遇到
				{						  //大于正常走 小于 这里处理
					qDebug() << "当前楼层小于上次楼层，返回上一层重新走迷宫";
					curPos = g_pGameFun->GetMapCoordinate();
					auto tmpPos = g_pGameFun->GetRandomSpace(curPos.x(), curPos.y(), 1);
					g_pGameFun->AutoMoveTo(tmpPos.x(), tmpPos.y());
					g_pGameFun->AutoMoveTo(curPos.x(), curPos.y());
				}
				curFloor = g_pGameFun->GetMapFloorNumberFromName();
				if (curFloor != 0)
					lastFloor = curFloor;
			}
		}
	}
	pThis->m_bAutoMaze = false;
}

void GameMapWall::StopAutoMaze()
{
	m_bAutoMaze = false;
}

void GameMapWall::SetSearchData(const QString &sName, int ntype)
{
	m_searchNpcName = sName;
	m_searchType = ntype;
}

void GameMapWall::RunSearch()
{
	if (m_bAutoMaze)
	{
		QMessageBox::information(nullptr, "提示：", "寻路中！", QMessageBox::Ok);
		return;
	}
	QtConcurrent::run(SearchThread, this);
}

void GameMapWall::DownLoadMap()
{
	int index1 = 0, index2 = 0, mapIndex = 0;
	std::string filemap;
	if (g_CGAInterface->GetMapIndex(index1, index2, mapIndex, filemap))
	{
		if (index1 == 0) //0下面地图 才进行缓存 迷宫不缓存
		{
			qDebug() << "只有迷宫才能开图！";
			return;
		}
	}
	QtConcurrent::run(AutoMazeOpen, this);

	//if (m_collision)
	//{
	//	emit RequestDownloadMap(m_collision->xsize, m_collision->ysize);
	//}
}

void GameMapWall::SaveMapData()
{
	QString sPath = QCoreApplication::applicationDirPath() + "//tmpMap//" + QString("%1.jpg").arg(g_pGameFun->GetMapIndex());
	//QImage image((uchar *)m_pixels.get(), m_pixelwidth/2, m_pixelheight/2, QImage::Format_ARGB32);
	QImage image = g_pGameFun->CreateMapImage();
	image.save(sPath);
}

void GameMapWall::SearchThread(GameMapWall *pThis)
{
	pThis->m_bAutoMaze = true;
	g_pGameFun->RestFun();
	/*while (pThis->m_bAutoMaze)
	{
		if (!g_pGameFun->IsMapDownload())
		{
			g_pGameFun->DownloadMap();
		}
		QPoint findPos, nextPos;
		bool bFind = g_pGameFun->SearchMap(pThis->m_searchNpcName, findPos, nextPos, pThis->m_searchType);
		if (bFind)
		{
			g_pGameFun->MoveToNpcNear(findPos.x(), findPos.y());
			emit pThis->updateUIMsg(QString("找到【%1】：%2,%3").arg(pThis->m_searchNpcName).arg(findPos.x()).arg(findPos.y()));
			break;
		}
	}*/
	while (pThis->m_bAutoMaze)
	{
		QPoint findPos, nextPos;
		bool bFind = g_pGameFun->SearchMapEx(pThis->m_searchNpcName, findPos, nextPos, pThis->m_searchType);
		if (bFind)
		{
			g_pGameFun->MoveToNpcNear(findPos.x(), findPos.y());
			emit pThis->updateUIMsg(QString("找到【%1】：%2,%3").arg(pThis->m_searchNpcName).arg(findPos.x()).arg(findPos.y()));
			break;
		}
	}
	pThis->m_bAutoMaze = false;
}
void GameMapWall::AutoMazeOpen(GameMapWall *pThis)
{
	g_pGameFun->MakeMapOpenContainNextEntrance(1);
}

void GameMapWall::ResetMapInfo()
{
	m_pixels = nullptr;
	m_pixels2 = nullptr;
	m_collision = nullptr;
	m_objects = nullptr;
	m_units = nullptr;
	m_pixelwidth = 0;
	m_pixelheight = 0;
	m_mapindex = 0;
}

bool GameMapWall::IsDisplayMap()
{
	return m_bDisplayMap;
}

//加载地图信息
void GameMapWall::LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
	/*if (!IsDisplayMap())
		return;*/
	m_collision = coll;
	m_objects = obj;
	m_units = units;

	if (m_collision)
	{
		if (m_pixelwidth != m_collision->xsize * blockSize || m_pixelheight != m_collision->ysize * blockSize)
		{
			m_pixelwidth = m_collision->xsize * blockSize;
			m_pixelheight = m_collision->ysize * blockSize;
			m_pixels = QSharedPointer<QRgb>(new QRgb[m_pixelwidth * m_pixelheight]);  //墙和空白能行走的地方
			m_pixels2 = QSharedPointer<QRgb>(new QRgb[m_pixelwidth * m_pixelheight]); //物品 对象 等有颜色部分 可以不关心这个
		}

		if (m_mapindex != m_collision->mapindex)
		{
			m_select_unitid = 0;
			RepaintCollisionPixels(0, 0, m_collision->xsize, m_collision->ysize);
		}

		m_mapindex = m_collision->mapindex;
	}
	for (uint i = 0; i < m_pixelheight * m_pixelwidth; ++i)
		m_pixels2.get()[i] = qRgba(255, 255, 255, 0); //默认刷成白色

	if (m_collision && m_objects && m_collision->xsize && m_collision->ysize)
	{
		for (int y = 0; y < m_collision->ysize; ++y)
		{
			for (int x = 0; x < m_collision->xsize; ++x)
			{
				if (x < m_objects->xsize && y < m_objects->ysize)
				{
					auto cellObject = m_objects->cells.at((size_t)(x + y * m_objects->xsize));
					if (cellObject & 0xff)
					{
						if ((cellObject & 0xff) == 3)
						{
							for (int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
								for (int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
									m_pixels2.get()[xx + yy * m_pixelwidth] = qRgb(0, 50, 200); //蓝色
						}
						else
						{
							for (int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
								for (int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
									m_pixels2.get()[xx + yy * m_pixelwidth] = qRgb(0, 200, 0); //绿色 切换坐标点 传送门
						}
					}
				}
			}
		}

		for (int i = 0; i < m_units->size(); ++i)
		{
			const auto &unit = m_units->at(i);
			if (unit.valid && unit.type == 1 && unit.model_id != 0 && (unit.flags & 4096) != 0)
			{
				int x = unit.xpos;
				int y = unit.ypos;

				if (x < m_collision->xsize && y < m_collision->ysize)
				{
					for (int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
						for (int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
							m_pixels2.get()[xx + yy * m_pixelwidth] = qRgb(255, 150, 50); //黄色 丢弃的物品
				}
			}
			else if (unit.valid && unit.type == 2 && unit.model_id != 0 && (unit.flags & 1024) != 0)
			{
				int x = unit.xpos;
				int y = unit.ypos;

				if (x < m_collision->xsize && y < m_collision->ysize)
				{
					for (int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
						for (int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
							m_pixels2.get()[xx + yy * m_pixelwidth] = qRgb(255, 100, 255); //紫色 NPC
				}
			}
		}
	}
	update();
}

void GameMapWall::RepaintCollisionPixels(int xbase, int ybase, int xtop, int ytop)
{
	/*if (!IsDisplayMap())
		return;*/
	if (xbase >= 0 && ybase >= 0 && xtop <= m_collision->xsize && ytop <= m_collision->ysize)
	{
		if (m_collision && m_collision->xsize * blockSize == m_pixelwidth && m_collision->ysize * blockSize == m_pixelheight)
		{
			for (int y = ybase; y < ytop; ++y)
			{
				for (int x = xbase; x < m_collision->xsize; ++x)
				{
					auto cellWall = m_collision->cells.at((size_t)(x + y * m_collision->xsize)); //地图单位数据 行列
					if (cellWall == 1)															 //不可通行 1
					{
						for (int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy) //找到图片对应点 置灰
							for (int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
								m_pixels.get()[xx + yy * m_pixelwidth] = qRgb(100, 100, 100); //灰色
					}
					else
					{
						for (int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
							for (int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
								m_pixels.get()[xx + yy * m_pixelwidth] = qRgb(255, 255, 255); //白色
					}
				}
			}

			update();
		}
	}
}
//探测距离 11码 x y最远+11能探测到 超过探测不到
void GameMapWall::LoadMapInfo(QString name, int x, int y, int mapindex)
{
	/*if (!IsDisplayMap())
		return;*/
	m_playerx = x;
	m_playery = y;
	m_playermapindex = mapindex;

	update();
}
void GameMapWall::LoadNavPath(QVector<quint32> navpath)
{
	m_navPathMapIndex = g_pGameFun->GetMapIndex();
	m_navpath = navpath;
	update();
}
void GameMapWall::MoveToTarget(int x, int y)
{
	if (m_bAutoMaze)
	{
		QMessageBox::information(nullptr, "提示：", "寻路中！", QMessageBox::Ok);
		return;
	}
	QtConcurrent::run(RunNavigator, this, x, y, false, &m_naverror);
}