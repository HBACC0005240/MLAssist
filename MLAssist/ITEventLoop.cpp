#include "ITEventLoop.h"

ITEventLoop::ITEventLoop(QObject *parent)
	: QEventLoop(parent)
{
}

ITEventLoop::~ITEventLoop()
{
}

void ITEventLoop::resetQuitState()
{
	QMutexLocker locker(&m_mutex);
	m_bQuit = false;
}

void ITEventLoop::quit()
{
	QMutexLocker locker(&m_mutex);
	if (m_bQuit)
		return;
	m_bQuit = true;
	exit(0);
}