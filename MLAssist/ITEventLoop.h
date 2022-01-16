#pragma once

#include <QEventLoop>
#include <QMutex>

class ITEventLoop : public QEventLoop
{
	Q_OBJECT

public:
	ITEventLoop(QObject *parent = nullptr);
	~ITEventLoop();

	void resetQuitState();
public slots:
	void quit();

private:
	QMutex m_mutex;
	bool m_bQuit = false;
};

