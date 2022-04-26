#include "ITThread.h"

ITThread::ITThread(QObject* parent)
	: QThread(parent)
{
	m_threadfun = NULL;
	m_threaduserdata = NULL;
}

ITThread::~ITThread()
{

}

void ITThread::run()
{
	if (m_threadfun)
	{
		m_threadfun(m_threaduserdata);
	}
	return;
}

void ITThread::setThreadFun(threadFun tFun, void* userdata)
{
	m_threadfun = tFun;
	m_threaduserdata = userdata;
}

void ITThread::startThread()
{
	start();
}
void ITThread::stopThread()
{
	wait();
}