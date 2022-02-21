#include "ITSignalProxy.h"
#include <QMetaType>

ITSignalProxy *g_pSignalProxy = new ITSignalProxy(NULL);
ITSignalProxy::ITSignalProxy(QObject *parent)
	: QObject(parent)
{
	qRegisterMetaType<QKeySequence>("QKeySequence");
}

ITSignalProxy::~ITSignalProxy()
{

}
