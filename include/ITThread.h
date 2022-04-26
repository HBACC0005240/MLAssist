#pragma once

#include <QThread>

typedef void (*threadFun)(void* param);
class ITThread : public QThread
{
	Q_OBJECT

public:
	ITThread(QObject* parent = 0);
	~ITThread();

	void setThreadFun(threadFun tFun, void* userdata);

	void startThread();
	void stopThread();

protected:
	void run();
private:
	threadFun   m_threadfun;
	void* m_threaduserdata;
};

