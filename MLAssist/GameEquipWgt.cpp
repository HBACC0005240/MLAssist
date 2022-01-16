#include "GameEquipWgt.h"
#include "CGFunction.h"
#include <QMenu>
#include <QMovie>

GameEquipWgt::GameEquipWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	//图片动画太占cpu 不适合多开，还是以文字展示
	m_lastUpdateTime.start();
	//for (int i = 0; i < 8; ++i)
	//{
	//	QString sObjName = QString("equip_%1").arg(i);
	//	QLabel *pLabel = this->findChild<QLabel *>(sObjName);
	//	if (pLabel)
	//	{
	//		QMovie *move = new QMovie();
	//		pLabel->setMovie(move);
	//		pLabel->setScaledContents(true);
	//		move->start(); //没有这一行，就不会显示任何内容
	//	}
	//}
	//QMovie *move = new QMovie();
	//ui.equip_player->setMovie(move);
	//ui.equip_player->setScaledContents(true);
	//move->start(); //没有这一行，就不会显示任何内容

	connect(g_pGameCtrl, &GameCtrl::NotifyGameItemsInfo, this, &GameEquipWgt::doUpdateItemList, Qt::ConnectionType::QueuedConnection);
	for (int i = 0; i <= 7; ++i)
	{
		QString sObjName = QString("equip_%1").arg(i);
		QLabel *plabel = this->findChild<QLabel *>(sObjName);
		if (plabel)
		{
			connect(plabel, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_equip_ContextMenuRequested(const QPoint &)));
		}
	}
}

GameEquipWgt::~GameEquipWgt()
{
}

void GameEquipWgt::updatePlayerImage()
{
	auto pGamer = g_pGameFun->GetGameCharacter();
	auto pMovie = ui.equip_player->movie();
	QString sPath = QCoreApplication::applicationDirPath() + "/db/人物/" + QString("%1.gif").arg(pGamer->image_id);
	if (pMovie->fileName() != sPath)
	{
		delete pMovie;
		QMovie *move = new QMovie(sPath);
		ui.equip_player->setMovie(move);
		ui.equip_player->setScaledContents(false);
		move->start(); //没有这一行，就不会显示任何内容
	}
}

void GameEquipWgt::updateEquipImage(QLabel *pLabel, const int &imageid)
{
	QString sPath = QCoreApplication::applicationDirPath() + "/db/道具/" + QString("%1.gif").arg(imageid);
	auto pMovie = pLabel->movie();
	if (pMovie)
	{
		if (pMovie->fileName() != sPath)
		{
			delete pMovie;
		}
	}
	QMovie *move = new QMovie(sPath);
	pLabel->setMovie(move);
	pLabel->setScaledContents(false);
	move->start(); //没有这一行，就不会显示任何内容
}

void GameEquipWgt::on_pushButton_logOut_clicked()
{
	g_CGAInterface->LogOut();
}

void GameEquipWgt::on_pushButton_logBack_clicked()
{
	g_CGAInterface->LogBack();
}

void GameEquipWgt::on_checkBox_update_stateChanged(int state)
{
	m_bRealTimeUpdate = (state == Qt::Checked ? true : false);
}

void GameEquipWgt::on_equip_ContextMenuRequested(const QPoint &pos)
{
	auto obj = sender();
	if (obj == ui.equip_0)
		m_equipPos = 0;
	else if (obj == ui.equip_1)
		m_equipPos = 1;
	else if (obj == ui.equip_2)
		m_equipPos = 2;
	else if (obj == ui.equip_3)
		m_equipPos = 3;
	else if (obj == ui.equip_4)
		m_equipPos = 4;
	else if (obj == ui.equip_5)
		m_equipPos = 5;
	else if (obj == ui.equip_6)
		m_equipPos = 6;
	else if (obj == ui.equip_7)
		m_equipPos = 7;
	else
		m_equipPos = -1;
	QMenu menu(this);
	menu.addAction("取下装备", this, SLOT(on_unequip_item()));
	menu.exec(QCursor::pos());
}

void GameEquipWgt::on_unequip_item()
{
	if (m_equipPos != -1)
	{
		g_pGameFun->UnEquipPos(m_equipPos);
		m_equipPos = -1;
	}
}

void GameEquipWgt::doUpdateItemList(GameItemList pItemList)
{
	if (!m_bRealTimeUpdate)
	{
		if (m_lastUpdateTime.elapsed() < 10000)
			return;
		m_lastUpdateTime.restart();
	}
	for (size_t i = 0; i < pItemList.size(); i++)
	{
		GameItemPtr pItem = pItemList[i];
		if (pItem->pos > 7)
			continue;
		QString sObjName = QString("equip_%1").arg(pItem->pos);
		QLabel *pLabel = this->findChild<QLabel *>(sObjName);
		if (pLabel)
		{
			if (pItem->exist == false)
			{
				pLabel->setToolTip("");
				pLabel->setText("");
				continue;
			}
			pLabel->setText(pItem->name);
			QString szToolTip = QString("%1 \n类型:%3 代码:%4 \n%5\n%6").arg(pItem->name).arg(pItem->type).arg(pItem->id).arg(pItem->attr).arg(pItem->info);
			szToolTip = szToolTip.remove("$0");
			szToolTip = szToolTip.remove("$1");
			szToolTip = szToolTip.remove("$2");
			szToolTip = szToolTip.remove("$3");
			szToolTip = szToolTip.remove("$4");
			szToolTip = szToolTip.remove("$5");
			szToolTip = szToolTip.remove("$6");
			szToolTip = szToolTip.remove("$7");
			szToolTip = szToolTip.remove("$8");
			//qDebug() << pItem->pos << pItem->id << szToolTip;
			pLabel->setToolTip(szToolTip);
		}
	}
}
//void GameEquipWgt::doUpdateItemListGif()
//{
//	return;
//	if (!m_bRealTimeUpdate)
//	{
//		if (m_lastUpdateTime.elapsed() < 10000)
//			return;
//		m_lastUpdateTime.restart();
//
//	}
//	updatePlayerImage();
//	int row = 0, col = 0;
//	GameItemList pItemList = g_pGameCtrl->getGameItems();
//	for (size_t i = 0; i < pItemList.size(); i++)
//	{
//		GameItem *pItem = pItemList[i];
//		if (pItem->pos > 7)
//			continue;
//		QString sObjName = QString("equip_%1").arg(pItem->pos);
//		QLabel *pLabel = this->findChild<QLabel *>(sObjName);
//		if (pLabel)
//		{
//			if (pItem->exist ==false)
//			{
//				pLabel->setToolTip("");
//				auto pMovie = pLabel->movie();
//				if (!pMovie)
//					continue;;
//				delete pMovie;
//				pLabel->update();
//				continue;
//			}
//			QString szToolTip = QString("%1 \n类型:%3 代码:%4 \n%5\n%6").arg(pItem->name).arg(pItem->type).arg(pItem->id).arg(pItem->attr).arg(pItem->info);
//			szToolTip = szToolTip.remove("$0");
//			szToolTip = szToolTip.remove("$1");
//			szToolTip = szToolTip.remove("$2");
//			szToolTip = szToolTip.remove("$3");
//			szToolTip = szToolTip.remove("$4");
//			szToolTip = szToolTip.remove("$5");
//			szToolTip = szToolTip.remove("$6");
//			szToolTip = szToolTip.remove("$7");
//			szToolTip = szToolTip.remove("$8");
//			//qDebug() << pItem->pos << pItem->id << szToolTip;
//			pLabel->setToolTip(szToolTip);
//			updateEquipImage(pLabel, pItem->image_id);
//		}
//	}
//}