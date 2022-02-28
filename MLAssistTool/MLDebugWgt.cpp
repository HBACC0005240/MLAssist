#include "MLDebugWgt.h"
#include "ITSignalProxy.h"
MLDebugWgt::MLDebugWgt(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.plainTextEdit->setMaximumBlockCount(1000);
	connect(g_pSignalProxy, SIGNAL(signal_addOneDebugMsg(const QString&)), this, SLOT(doAddOneDebugMsg(const QString&)));
}

MLDebugWgt::~MLDebugWgt()
{
}

void MLDebugWgt::doAddOneDebugMsg(const QString& msg)
{
	if (ui.checkBox_enable->isChecked())
		ui.plainTextEdit->appendPlainText(msg);
}
