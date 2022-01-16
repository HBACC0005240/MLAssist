#include "GameFunctionWgt.h"
#include "GameCtrl.h"
#include "stdafx.h"
GameFunctionWgt::GameFunctionWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	init();
}

GameFunctionWgt::~GameFunctionWgt()
{
}
void GameFunctionWgt::init()
{
	auto pTabBar = ui.tabWidget->tabBar();
	m_pEnableUpdateCheckBox = new QCheckBox("启用功能", this);
	m_pRealUpdateCheckBox = new QCheckBox("NPC物品玩家实时刷新", this);
	connect(m_pEnableUpdateCheckBox, SIGNAL(stateChanged(int)), g_pGameCtrl, SLOT(OnEnableDataDisplayUi(int)));
	connect(m_pRealUpdateCheckBox, SIGNAL(stateChanged(int)), g_pGameCtrl, SLOT(OnSetRealUpdateUi(int)));

	//m_pRealUpdateCheckBox->setMaximumWidth(40);
	QWidget *pWidget = new QWidget(this);
	QHBoxLayout *pHLayout = new QHBoxLayout(pWidget);
	pHLayout->addWidget(m_pEnableUpdateCheckBox);
	pHLayout->addWidget(m_pRealUpdateCheckBox);
	pHLayout->setContentsMargins(0, 0, 0, 0);
	ui.tabWidget->setCornerWidget(pWidget);
}

void GameFunctionWgt::doLoadUserConfig(QSettings &iniFile)
{
	ui.gameOftenFunWgt->doLoadUserConfig(iniFile);
}

void GameFunctionWgt::doSaveUserConfig(QSettings &iniFile)
{
	ui.gameOftenFunWgt->doSaveUserConfig(iniFile);
}

void GameFunctionWgt::doLoadJsConfig(QJsonObject &obj)
{
	ui.gameOftenFunWgt->doLoadJsConfig(obj);
}

void GameFunctionWgt::doSaveJsConfig(QJsonObject &obj)
{
	ui.gameOftenFunWgt->doSaveJsConfig(obj);
}
