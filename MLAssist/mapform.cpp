#include "mapform.h"
#include "stdafx.h"
#include "ui_mapform.h"

#include "FloatMapDlg.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

MapForm::MapForm(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	m_pFloatMapDlg = new FloatMapDlg;
	connect(m_pFloatMapDlg, SIGNAL(signal_checkCross(bool)), this, SLOT(on_maxMapDlg_checkCross(bool)));
	connect(ui.widget_paintmap, &MyPaintMap::updateMousePosition, this, &MapForm::UpdateMousePosition);
	connect(ui.widget_paintmap, &MyPaintMap::runNavigatorScript, this, &MapForm::RunNavigatorScript);
	connect(ui.widget_paintmap, &MyPaintMap::stopNavigatorScript, this, &MapForm::StopNavigatorScript);

	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapCellInfo, this, &MapForm::OnNotifyGetMapCellInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGetMapInfo, this, &MapForm::OnNotifyGetMapInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyRefreshMapRegion, this, &MapForm::OnNotifyRefreshMapRegion, Qt::ConnectionType::QueuedConnection);
	connect(this, &MapForm::RequestDownloadMap, g_pGameCtrl, &GameCtrl::OnDownloadMap, Qt::ConnectionType::QueuedConnection);
}

MapForm::~MapForm()
{
	SafeDelete(m_pFloatMapDlg);
}

void MapForm::keyPressEvent(QKeyEvent *event)
{
	if (!m_collision)
		return;

	ui.widget_paintmap->keyPressEvent(event);
}

void MapForm::showEvent(QShowEvent *event)
{
	//	qDebug() << "showEvent";
	g_pGameCtrl->SetMapIsVisible(true);
}

void MapForm::hideEvent(QHideEvent *event)
{
	//	qDebug() << "hideEvent";
	if (m_pFloatMapDlg->isHidden()) //当前界面隐藏，并且浮动地图窗口也是隐藏，则通知地图下载部分不要下载了
		g_pGameCtrl->SetMapIsVisible(false);
	m_collision = nullptr;
	ui.widget_paintmap->ResetMapInfo();
}

void MapForm::AutoMazeThread(MapForm *pThis)
{
	g_pGameFun->RestFun();
	g_pGameFun->AutoWalkRandomMaze();
}

void MapForm::AutoMazeOpen(MapForm *pThis)
{
	//g_pGameFun->MakeMapOpenContainNextEntrance(1);
	g_pGameFun->MakeMapOpenEx();
}

void MapForm::UpdateMousePosition(int x, int y)
{
	ui.label_xy_2->setText(tr("鼠标点击坐标(%1, %2)").arg(x).arg(y));
}

void MapForm::on_pushButton_stopNav_clicked()
{
	g_pGameFun->StopFun();
}

void MapForm::on_pushButton_maxMap_clicked()
{
	m_pFloatMapDlg->show();
}

void MapForm::on_pushButton_AutoMaze_clicked()
{
	/*if (!g_pGameFun->IsMapDownload())
    {
        g_pGameFun->DownloadMap();
    }
	QPoint targetEntrance = g_pGameFun->GetMazeEntrance();
	qDebug() << targetEntrance;
	ui.widget_paintmap->MoveToTarget(targetEntrance.x(), targetEntrance.y());*/
	QtConcurrent::run(AutoMazeThread, this);
}

void MapForm::RunNavigatorScript(int x, int y, int enter, QString *result)
{
	runNavigatorScript(x, y, enter, result);
}

void MapForm::StopNavigatorScript()
{
	stopNavigatorScript();
}

void MapForm::OnReportNavigatorFinish(int exitCode)
{
	if (exitCode != 0)
		ui.widget_paintmap->ShowNavError(QObject::tr("导航错误"));
	ui.widget_paintmap->LoadNavPath(QVector<quint32>());
}

void MapForm::OnReportNavigatorPath(QString json)
{
	QJsonParseError err;
	auto doc = QJsonDocument::fromJson(json.toLocal8Bit(), &err);

	if (err.error != QJsonParseError::NoError)
		return;

	if (!doc.isArray())
		return;

	auto arr = doc.array();

	QVector<quint32> navpath;
	for (auto itor = arr.begin(); itor != arr.end(); ++itor)
	{
		if (itor->isArray())
		{
			auto sub = itor->toArray();
			if (sub.count() == 2)
			{
				int x = sub.at(0).toInt();
				int y = sub.at(1).toInt();
				navpath.push_back((x & 0xFFFF) | ((y & 0xFFFF) << 16));
			}
		}
	}

	if (navpath.empty())
		ui.widget_paintmap->ShowNavError(QObject::tr("导航错误"));

	ui.widget_paintmap->LoadNavPath(navpath);
}

void MapForm::OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
	//qDebug() << "isActiveWindow" << this->isActiveWindow() << this->hasFocus() << this->isVisible() << this->isVisibleTo(this);
	if (!m_pFloatMapDlg->isHidden())
	{
		if (coll && coll->xsize && coll->ysize)
			m_pFloatMapDlg->LoadMapCellInfo(coll, obj, units);
	}
	if (!this->isVisible())
	{
		return;
	}
	ui.widget_paintmap->m_bShowCrosshair = ui.checkBox_showcrosshair->isChecked();
	m_pFloatMapDlg->SetCrosshair(ui.checkBox_showcrosshair->isChecked());
	if (coll && coll->xsize && coll->ysize)
	{
		m_collision = coll;
		ui.widget_paintmap->LoadMapCellInfo(coll, obj, units);
	}
}

void MapForm::OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y)
{
	if (!m_pFloatMapDlg->isHidden())
	{
		m_pFloatMapDlg->LoadMapInfo(name, x, y, index3);
	}
	if (!this->isVisible())
	{
		return;
	}
	m_pFloatMapDlg->SetCrosshair(ui.checkBox_showcrosshair->isChecked());
	ui.widget_paintmap->m_bShowCrosshair = ui.checkBox_showcrosshair->isChecked();
	ui.widget_paintmap->LoadMapInfo(name, x, y, index3);
	ui.label_mapname->setText(name);
	ui.label_xy->setText(tr("坐标：(%1, %2)").arg(x).arg(y));
	ui.label_index->setText(tr("地图编号 : %1").arg(index3));
}

void MapForm::OnNotifyRefreshMapRegion(int xbase, int ybase, int xtop, int ytop, int index3)
{
	//浮动窗体单独处理
	if (!m_pFloatMapDlg->isHidden())
	{
		if (m_collision && index3 == m_collision->mapindex)
			m_pFloatMapDlg->RepaintCollisionPixels(xbase, ybase, xtop, ytop);
	}
	if (!this->isVisible())
		return;
	if (m_collision && index3 == m_collision->mapindex)
	{
		ui.widget_paintmap->RepaintCollisionPixels(xbase, ybase, xtop, ytop);
	}
}

void MapForm::OnCloseWindow()
{
}

void MapForm::on_pushButton_loadmap_clicked()
{
	int index1 = 0, index2 = 0, mapIndex = 0;
	std::string filemap;
	if (g_CGAInterface->GetMapIndex(index1, index2, mapIndex, filemap))
	{
		if (QString::fromStdString(filemap).contains("map\\0")) //0下面地图 才进行缓存 迷宫不缓存
		{
			qDebug() << "只有迷宫才能开图！";
			return;
		}
	}
	QtConcurrent::run(AutoMazeOpen, this);
	return;
	if (m_collision)
	{
		RequestDownloadMap(m_collision->xsize, m_collision->ysize);
	}
}

void MapForm::on_maxMapDlg_checkCross(bool bFlag)
{
	ui.checkBox_showcrosshair->setChecked(bFlag);
}
void MapForm::on_pushButton_go_clicked()
{
	int x = ui.lineEdit_x->text().toInt();
	int y = ui.lineEdit_y->text().toInt();
	ui.widget_paintmap->MoveToTarget(x, y);
}