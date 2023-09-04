#include "GameOnlineInfo.h"
#include "ITObjectDataMgr.h"
#include <QTimer>
#include "ITPublic.h"

GameOnlineInfo::GameOnlineInfo(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	//ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	QTimer *timer = new QTimer;
	connect(timer, SIGNAL(timeout()), this, SLOT(doUpdateTimer()));
	timer->start(5000);
	init();
}

GameOnlineInfo::~GameOnlineInfo()
{
}

void GameOnlineInfo::init()
{
	m_pTableModel = new ITTableModel;
	ui.tableView->setModel(m_pTableModel);
	QStringList headers;
	headers << "序号"
			<< "名称"
			<< "等级"
			<< "金币"
			<< "地图"
			<< "坐标"
			<< "状态"
			<< "线路"
			<<"大区"
		;
	m_pTableModel->setHorizontalHeader(headers);
}
bool GameOnlineInfo::SortStringFun(const QString &s1, const QString &s2)
{
	if (s1.compare(s2) == 0)
	{
		return true;
	}
	int nRet = customCompareString(s1, s2);
	if (nRet > 0)
		return false;
	else if (nRet < 0)
		return true;
	else
		return true;
}
bool GameOnlineInfo::SortObjectFun(const ITObjectPtr p1, const ITObjectPtr p2)
{
	ITGameCharacterPtr o1 = p1.dynamicCast<ITGameCharacter>();
	ITGameCharacterPtr o2 = p2.dynamicCast<ITGameCharacter>();
	if (o1->_connectState > o2->_connectState)
	{
		return true;
	}else if (o1->_connectState < o2->_connectState)
	{
		return false;
	}
	QString s1 = p1->getObjectName();
	QString s2 = p2->getObjectName();
	if (s1.compare(s2) == 0)
	{
		return true;
	}
	int nRet = customCompareString(s1, s2);
	if (nRet > 0)
		return false;
	else if (nRet < 0)
		return true;
	else
		return true;
}

void GameOnlineInfo::resizeEvent(QResizeEvent *ev)
{
	QWidget::resizeEvent(ev);
	int nWidth = ui.tableView->width();
	int nColCount =m_pTableModel->columnCount();

	int nDoubleStr = 50;
	int nThreeStr = 80;
	int nNameStr = 150;
	ui.tableView->setColumnWidth(0, nDoubleStr);
	int iWidth = (nWidth - 50) / nColCount;
	ui.tableView->setColumnWidth(1, nNameStr);
	ui.tableView->setColumnWidth(2, nDoubleStr);
	ui.tableView->setColumnWidth(3, nThreeStr);
	ui.tableView->setColumnWidth(4, nNameStr);
	ui.tableView->setColumnWidth(5, nThreeStr);
	ui.tableView->setColumnWidth(6, nDoubleStr);
	ui.tableView->setColumnWidth(7, nDoubleStr);
	ui.tableView->setColumnWidth(8, nThreeStr);
}

void GameOnlineInfo::doUpdateTimer()
{
	m_nTryCount++;
	int fzCount = ITObjectDataMgr::getInstance().GetGameRoleCount();
	if (fzCount != m_pTableModel->rowCount() || m_nTryCount%6==0)
	{
		ITObjectList pRoles = ITObjectDataMgr::getInstance().GetAllCharacterList();
		////排序 
		qSort(pRoles.begin(), pRoles.end(), SortObjectFun);
		m_pTableModel->setTableRowDatas(pRoles);
		m_pTableModel->updateTable();
	}
	ui.label_fzCount->setText(QString::number(ITObjectDataMgr::getInstance().GetGameRoleCount()));
	ui.label_fzOnline->setText(QString::number(ITObjectDataMgr::getInstance().GetGameRoleOnlineCount()));
	ui.label_fzOffline->setText(QString::number(ITObjectDataMgr::getInstance().GetGameRoleOfflineCount()));
}
