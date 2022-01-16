#include "GameBattleInfoWgt.h"
#include "GameCtrl.h"
GameBattleInfoWgt::GameBattleInfoWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	ui.tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget_2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	init();
	connect(g_pGameCtrl, &GameCtrl::NotifyGameBattlesInfo, this, &GameBattleInfoWgt::OnNotifyGetBattleInfo, Qt::ConnectionType::QueuedConnection);

	connect(g_pGameCtrl, &GameCtrl::NotifyTeamInfo, this, &GameBattleInfoWgt::DoNotifyTeamInfo, Qt::QueuedConnection);
}

GameBattleInfoWgt::~GameBattleInfoWgt()
{
}

void GameBattleInfoWgt::init()
{
	int col[5] = { 2, 3, 1, 4, 0 };

	int index = 0;
	for (int i = 3; i >= 2; i--)
	{
		for (int n = 0; n < 5; ++n)
		{
			m_battleUnitForCell.insert(index, QPoint(i, col[n]));
			index++;
		}
	}
	for (int i = 0; i <= 1; i++)
	{
		for (int n = 0; n < 5; ++n)
		{
			m_battleUnitForCell.insert(index, QPoint(i, col[n]));
			index++;
		}
	}
	QColor defaultColor("white");
	for (int i = 0; i < 4; ++i)
	{
		for (size_t n = 0; n < 5; n++)
		{
			QTableWidgetItem *pItem = new QTableWidgetItem();
			ui.tableWidget->setItem(i, n, pItem);
			pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			pItem->setTextColor(defaultColor);
			pItem->setBackgroundColor(defaultColor);
		}
	}
	for (int i = 0; i < 5; ++i)
	{
		QTableWidgetItem *pItem = new QTableWidgetItem();
		ui.tableWidget_2->setItem(i, 0, pItem);
		pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		pItem->setTextColor(defaultColor);
		pItem->setBackgroundColor(defaultColor);
	}
}



void GameBattleInfoWgt::DoNotifyTeamInfo(const QList<QSharedPointer<GameTeamPlayer> > &teamInfos)
{
	int teamCount = teamInfos.size();
	for (int i = 0; i < 5; ++i)
	{
		if (i > (teamCount - 1))
		{
			setTeamItemText(i, 0, "", QColor("white"), QColor("white"));
			continue;
		}
		if (teamInfos.at(i)->maxhp == 0 || teamInfos.at(i)->level == 0)
			continue;
		QString szShowText = QString("%1 %2\n%3/%4").arg(teamInfos.at(i)->level).arg(teamInfos.at(i)->name).arg(teamInfos.at(i)->hp).arg(teamInfos.at(i)->maxhp);
		setTeamItemText(i, 0, szShowText, QColor("white"), QColor("green"));
	}
}

void GameBattleInfoWgt::OnNotifyGetBattleInfo(GameBattleUnitList pBattleUs)
{
	for (int i = 0; i < pBattleUs.size(); i++)
	{
		auto info = pBattleUs[i];
		QPoint cellVal = m_battleUnitForCell.value(i);

		if (info->exist) //
		{
			QString szShowText = QString("%1\n%2/%3\n%4").arg(info->name).arg(info->hp).arg(info->maxhp).arg(info->level);

			if (i >= 0 && i < 10) //队友
			{
				setItemText(cellVal.x(), cellVal.y(), szShowText, QColor("white"), QColor("green"));
			} //敌方
			else
			{
				setItemText(cellVal.x(), cellVal.y(), szShowText, QColor("white"), QColor("red"));
			}
		}
		else
		{
			setItemText(cellVal.x(), cellVal.y(), "", QColor("white"), QColor("white"));
		}
	}
}

void GameBattleInfoWgt::setItemText(int row, int col, const QString &szText, const QColor &szColor, const QColor &backColor)
{
	QTableWidgetItem *pItem = ui.tableWidget->item(row, col);
	if (pItem && pItem->text() != szText)
	{
		pItem->setText(szText);
		pItem->setTextColor(szColor);
		pItem->setBackgroundColor(backColor);
	}
}
void GameBattleInfoWgt::setTeamItemText(int row, int col, const QString &szText, const QColor &szColor, const QColor &backColor)
{
	QTableWidgetItem *pItem = ui.tableWidget_2->item(row, col);
	if (pItem && pItem->text() != szText)
	{
		pItem->setText(szText);
		pItem->setTextColor(szColor);
		pItem->setBackgroundColor(backColor);
	}
}
