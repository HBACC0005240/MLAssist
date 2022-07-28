#include "GameOnlineInfo.h"
#include <QTimer>
#include "ITObjectDataMgr.h"

GameOnlineInfo::GameOnlineInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QTimer *timer = new QTimer;
	connect(timer, SIGNAL(timeOut()), this, SLOT(doUpdateTimer()));
	timer->start(5000);
}

GameOnlineInfo::~GameOnlineInfo()
{}

void GameOnlineInfo::doUpdateTimer()
{
	auto gameInfos = ITObjectDataMgr::getInstance().GetAlreadyConnectedData();
	int onlineCount = 0;
	int offlineCount = 0;
	for (auto it=gameInfos.begin();it!=gameInfos.end();++it)
	{
		if (it.value()->_connectState == 1)
			onlineCount += 1;
		else
			offlineCount += 1;		
	}
	ui.label_fzCount->setText(QString::number(gameInfos.size()));
	ui.label_fzOnline->setText(QString::number(onlineCount));
	ui.label_fzOffline->setText(QString::number(offlineCount));
}
