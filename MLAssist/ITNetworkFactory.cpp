#include "ITNetworkFactory.h"

ITNetworkFactory *ITNetworkFactory::getInstace()
{
	static ITNetworkFactory instance;
	return &instance;
}
std::tuple<ITTcpServer *, bool, QString> ITNetworkFactory::CreateNewTcpServer(int nPort)
{
	int ptrVal = 0;
	bool retState = false;
	QString retMsg = "";
	QEventLoop loop;
	qDebug() << "CreateNewTcpServer";
	QTimer::singleShot(600000, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pNetworkFactory, &ITNetworkFactory::signal_returnCreateTcpServer, [&](int nVal, bool state, const QString &msg)
			{
				ptrVal = nVal;
				retState = state;
				retMsg = msg;
				if (loop.isRunning())
					loop.quit(); //放到后面，否则 loop.exec()执行完成，会直接返回了 再调用崩溃
			});
	emit g_pNetworkFactory->signal_createTcpServer(nPort);
	loop.exec();
	QObject::disconnect(connection); //利用Connection 断开lambda的连接
	qDebug() << "CreateNewTcpServer Succe" << ptrVal;
	//std::tuple<ITTcpServer *, bool, QString> retVal;
	return std::make_tuple((ITTcpServer *)ptrVal, retState, retMsg);
	//return (ITTcpServer *)ptrVal;
}

std::tuple<ITNetAgent *, bool, QString> ITNetworkFactory::CreateNewTcpClient(const QString &sIP, int nPort, const QString &sName)
{
	int ptrVal = 0;
	bool retState = false;
	QString retMsg = "";
	QEventLoop loop;
	qDebug() << "CreateNewTcpClient";
	QTimer::singleShot(600000, &loop, &QEventLoop::quit);
	connect(g_pGameFun, &CGFunction::signal_stopScript, &loop, &QEventLoop::quit);
	auto connection = connect(g_pNetworkFactory, &ITNetworkFactory::signal_returnCreateTcpClient, [&](int nVal, bool state, const QString &msg)
			{
				ptrVal = nVal;
				retState = state;
				retMsg = msg;
				if (loop.isRunning())
					loop.quit(); //放到后面，否则 loop.exec()执行完成，会直接返回了 再调用崩溃
			});
	emit g_pNetworkFactory->signal_createTcpClient(sIP, nPort, sName);
	loop.exec();
	QObject::disconnect(connection); //利用Connection 断开lambda的连接
	qDebug() << "CreateNewTcpClient Succe" << ptrVal;
	return std::make_tuple((ITNetAgent *)ptrVal, retState, retMsg);
}

void ITNetworkFactory::slot_createTcpServer(int nPort)
{
	ITTcpServer *pTcpServer = new ITTcpServer;
	bool bRet = pTcpServer->listen(QHostAddress::Any, nPort);
	if (!bRet)
	{
		SafeDelete(pTcpServer);
	}
	else
		m_pTcpServers.insert(pTcpServer);

	emit signal_returnCreateTcpServer(int(pTcpServer), bRet, "");
}

void ITNetworkFactory::slot_createTcpClient(const QString &sIP, int nPort, const QString &sName)
{
	ITNetAgent *pNetAgent = new ITNetAgent;
	pNetAgent->SetServerName(sName);
	pNetAgent->SetServerAddr(sIP, nPort);
	pNetAgent->startThread();
	m_pTcpAgents.insert(pNetAgent);

	emit signal_returnCreateTcpClient(int(pNetAgent), true, "");
}

ITNetworkFactory::ITNetworkFactory()
{
	connect(this, SIGNAL(signal_createTcpServer(int)), this, SLOT(slot_createTcpServer(int)));
	connect(this, SIGNAL(signal_createTcpClient(const QString &, int, const QString &)), this, SLOT(slot_createTcpClient(const QString &, int, const QString &)));
}

void ITNetworkFactory::CloseAllTcpServer()
{
	for (auto tmpServer : m_pTcpServers)
	{
		tmpServer->shutdown();
		SafeDelete(tmpServer);
	}
	m_pTcpServers.clear();
}

void ITNetworkFactory::CloseAllTcpClient()
{
	for (auto tmpClient : m_pTcpAgents)
	{
		tmpClient->stopThread();
		SafeDelete(tmpClient);
	}
	m_pTcpAgents.clear();
}

void ITNetworkFactory::CloseClient(ITNetAgent *pAgent)
{
	if (!pAgent)
		return;

	if (m_pTcpAgents.contains(pAgent))
	{
		m_pTcpAgents.remove(pAgent);
		pAgent->stopThread();
		SafeDelete(pAgent);
	}
}

void ITNetworkFactory::CloseServer(ITTcpServer *pServer)
{
	if (!pServer)
		return;

	if (m_pTcpServers.contains(pServer))
	{
		m_pTcpServers.remove(pServer);
		pServer->shutdown();
		SafeDelete(pServer);
	}
}
