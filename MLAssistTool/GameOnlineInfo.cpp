#include "GameOnlineInfo.h"
#include "ITObjectDataMgr.h"
#include <QTimer>
#include "ITPublic.h"

GameOnlineInfo::GameOnlineInfo(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
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
			<< "状态";
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

void GameOnlineInfo::doUpdateTimer()
{
	//auto gameInfos = ITObjectDataMgr::getInstance().GetAlreadyConnectedData();
	//int onlineCount = 0;
	//int offlineCount = 0;
	//for (auto it=gameInfos.begin();it!=gameInfos.end();++it)
	//{
	//	if (it.value()->_connectState == 1)
	//		onlineCount += 1;
	//	else
	//		offlineCount += 1;
	//}
	//ui.label_fzCount->setText(QString::number(gameInfos.size()));
	//ui.label_fzOnline->setText(QString::number(onlineCount));
	//ui.label_fzOffline->setText(QString::number(offlineCount));
	int fzCount = ITObjectDataMgr::getInstance().GetGameRoleCount();
	if (fzCount != m_pTableModel->rowCount())
	{
		QStringList onlineRoles = ITObjectDataMgr::getInstance().GetOnlineRoleKeys();
		//排序 
		qSort(onlineRoles.begin(), onlineRoles.end(), SortStringFun);
		//获取所有列表 
		QStringList allRoles = ITObjectDataMgr::getInstance().GetAlreadyConnectedData().keys();
		//排序
		qSort(allRoles.begin(), allRoles.end(), SortStringFun);
		//从所有里面删除在线的
		for (auto tmpRole:onlineRoles)
		{
			allRoles.removeAll(tmpRole);
		}
		onlineRoles = onlineRoles + allRoles;

		m_pTableModel->setTableRowDataIds(onlineRoles);
		m_pTableModel->updateTable();
	}
	ui.label_fzCount->setText(QString::number(ITObjectDataMgr::getInstance().GetGameRoleCount()));
	ui.label_fzOnline->setText(QString::number(ITObjectDataMgr::getInstance().GetGameRoleOnlineCount()));
	ui.label_fzOffline->setText(QString::number(ITObjectDataMgr::getInstance().GetGameRoleOfflineCount()));
}
