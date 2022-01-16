#include "GameDebugWgt.h"
#include "GameCtrl.h"
GameDebugWgt::GameDebugWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl, SIGNAL(signal_addOneDebugMsg(const QString&)),this,SLOT(doAddOneMsg(const QString&)),Qt::QueuedConnection);
}

GameDebugWgt::~GameDebugWgt()
{
}

void GameDebugWgt::doAddOneMsg(const QString &msg)
{
	if (m_bEnableDebug)
	{
		ui.textEdit->append(msg);
	}
}

void GameDebugWgt::on_checkBox_enable_stateChanged(int state)
{
	m_bEnableDebug = (state == Qt::Checked ? true : false);
}
