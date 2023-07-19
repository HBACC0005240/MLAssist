#include "FloatMapDlg.h"

FloatMapDlg::FloatMapDlg(QWidget *parent) :
		QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	auto mScene = ui.mapWidget->GetScene();

	m_pGameMapWallItem = new GameMapWall();
	m_pGameMapWallItem->setRect(mScene->sceneRect().toRect());
	m_pGameMapWallItem->setVisible(true);
	mScene->addRect(mScene->sceneRect(), QPen(Qt::red));
	m_pGameMapWallItem->setPos(0, 0);
	m_pGameMapWallItem->update();
	mScene->addItem(m_pGameMapWallItem);
	mScene->update();
	ui.comboBox_searchType->addItem("物品", 0);
	ui.comboBox_searchType->addItem("Npc", 1);
	ui.comboBox_searchType->addItem("玩家", 2);
	connect(m_pGameMapWallItem, &GameMapWall::updateMousePosition, this, &FloatMapDlg::UpdateMousePosition);
	connect(m_pGameMapWallItem, &GameMapWall::updateUIMsg, this, &FloatMapDlg::updateeUiMsg);
	//connect(ui.mapWidget, &MyPaintMap::runNavigatorScript, this, &FloatMapDlg::RunNavigatorScript);
	//connect(ui.mapWidget, &MyPaintMap::stopNavigatorScript, this, &FloatMapDlg::StopNavigatorScript);
}

FloatMapDlg::~FloatMapDlg()
{
}

void FloatMapDlg::LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
	m_pGameMapWallItem->LoadMapCellInfo(coll, obj, units);
	ui.mapWidget->GetScene()->update();
}

void FloatMapDlg::LoadMapInfo(QString name, int x, int y, int mapindex)
{
	m_pGameMapWallItem->LoadMapInfo(name, x, y, mapindex);
	ui.label_mapName->setText(name);
	ui.label_mapNum->setText(" 编号:" + QString::number(mapindex));
	ui.label_xy->setText(QString("坐标:%1,%2").arg(x).arg(y));
}

void FloatMapDlg::RepaintCollisionPixels(int xbase, int ybase, int xtop, int ytop)
{
	m_pGameMapWallItem->RepaintCollisionPixels(xbase, ybase, xtop, ytop);
	ui.mapWidget->GetScene()->update();
}

void FloatMapDlg::SetCrosshair(bool bFlag)
{
	m_pGameMapWallItem->m_bShowCrosshair = bFlag;
}

void FloatMapDlg::hideEvent(QHideEvent *event)
{
	m_pGameMapWallItem->ResetMapInfo();
}

void FloatMapDlg::updateeUiMsg(const QString &sMsg)
{
	ui.textEdit->clear();
	ui.textEdit->append(sMsg);
}

void FloatMapDlg::UpdateMousePosition(int x, int y)
{
	ui.lineEdit_x->setText(QString::number(x));
	ui.lineEdit_y->setText(QString::number(y));
}

void FloatMapDlg::on_pushButton_go_clicked()
{
	int x = ui.lineEdit_x->text().toInt();
	int y = ui.lineEdit_y->text().toInt();
	m_pGameMapWallItem->MoveToTarget(x, y);
}

void FloatMapDlg::on_checkBox_cross_stateChanged(int state)
{
	emit signal_checkCross(state == Qt::Checked ? true : false); //外部更新
	m_pGameMapWallItem->m_bShowCrosshair = (state == Qt::Checked ? true : false);
}
//自动走迷宫
void FloatMapDlg::on_pushButton_AutoMaze_clicked()
{
	//QPoint targetEntrance =  g_pGameFun->GetMazeEntrance();
	//qDebug() << targetEntrance;
	//m_pGameMapWallItem->MoveToTarget(targetEntrance.x(), targetEntrance.y());
	m_pGameMapWallItem->AutoWallMaze();
}

void FloatMapDlg::on_pushButton_StopNav_clicked()
{
	g_pGameFun->StopFun();
	m_pGameMapWallItem->StopAutoMaze();
}

void FloatMapDlg::on_toolButton_clear_clicked()
{
	m_pGameMapWallItem->ResetSelect();
}

void FloatMapDlg::on_toolButton_zoomIn_clicked()
{
	ui.mapWidget->zoomIn();
}

void FloatMapDlg::on_toolButton_zoomOut_clicked()
{
	ui.mapWidget->zoomOut();
}

void FloatMapDlg::on_toolButton_zoomFit_clicked()
{
	ui.mapWidget->zoomReset();
}

void FloatMapDlg::on_lineEdit_TargetMazeName_editingFinished()
{
	m_stopFloorName = ui.lineEdit_TargetMazeName->text();
	m_pGameMapWallItem->SetTargetMazeName(m_stopFloorName);
}

void FloatMapDlg::on_lineEdit_searchName_editingFinished()
{
	//m_pGameMapWallItem->SetTargetMazeName(m_stopFloorName);
}

void FloatMapDlg::on_pushButton_search_clicked()
{
	QString searchName = ui.lineEdit_searchName->text();
	int searchType = ui.comboBox_searchType->currentData(Qt::UserRole).toInt();
	m_pGameMapWallItem->SetSearchData(searchName, searchType);
	m_pGameMapWallItem->RunSearch();
}

void FloatMapDlg::on_pushButton_downMap_clicked()
{
	m_pGameMapWallItem->DownLoadMap();
}
void FloatMapDlg::on_pushButton_questMap_clicked()
{
	m_pGameMapWallItem->QuestMap();
}
//搜索迷宫坐标
void FloatMapDlg::on_pushButton_searchMazePos_clicked()
{
	int x = ui.lineEdit_x_2->text().toInt();
	int y = ui.lineEdit_y_2->text().toInt();
	int w = ui.lineEdit_width->text().toInt();
	int h = ui.lineEdit_height->text().toInt();
	QString sFilter = ui.lineEdit_filter->text();
	auto warpList = g_pGameFun->FindRandomEntryEx(x, y, w, h, sFilter);
	ui.textEdit->clear();
	ui.textEdit->append("范围内传送列表：");
	if (warpList.size() < 1)
	{
		ui.textEdit->append("未找到");
	}
	for (auto warpPos : warpList)
	{
		ui.textEdit->append(QString("%1,%2").arg(warpPos.x()).arg(warpPos.y()));
	}
}

void FloatMapDlg::on_toolButton_downMap_clicked()
{
	m_pGameMapWallItem->SaveMapData();
}
