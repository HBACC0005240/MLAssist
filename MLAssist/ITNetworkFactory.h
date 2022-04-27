#pragma once

#include "ITNetAgent.h"
#include "ITTcpServer.h"

class ITNetworkFactory : public QObject
{
	Q_OBJECT
public:
	static ITNetworkFactory *getInstace();

	std::tuple<ITTcpServer *, bool, QString> CreateNewTcpServer(int nPort);
	std::tuple<ITNetAgent *, bool, QString> CreateNewTcpClient(const QString &sIP, int nPort, const QString &sName);

	void CloseAllTcpServer();
	void CloseAllTcpClient();

signals:
	void signal_createTcpServer(int nPort);
	void signal_createTcpClient(const QString &sIP, int nPort, const QString &sName);
	void signal_returnCreateTcpServer(int ptr, bool state, const QString &msg);
	void signal_returnCreateTcpClient(int ptr, bool state, const QString &msg);

public slots:
	void slot_createTcpServer(int nPort);
	void slot_createTcpClient(const QString &sIP, int nPort, const QString &sName);

private:
	ITNetworkFactory();
	QSet<ITTcpServer *> m_pTcpServers;
	QSet<ITNetAgent *> m_pTcpAgents;
};

#define g_pNetworkFactory ITNetworkFactory::getInstace()
