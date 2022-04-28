#include "ITTcpServer.h"
#include "ITTcpSocket.h"
#include "ITThread.h"
#include <QApplication>

ITTcpServer::ITTcpServer(QObject* parent)
	: QTcpServer(parent)
{
	m_bThreadRun = true;
	m_pContext = NULL;
	m_thread = new ITThread;
	m_thread->setThreadFun(threadRunCB, (void*)this);
	m_thread->startThread();
}

ITTcpServer::~ITTcpServer()
{
	QMap<int, ITTcpSocket*>::iterator iter;
	for (iter = m_newconnects.begin(); iter != m_newconnects.end(); iter++)
	{
		ITTcpSocket* tcpsocket = iter.value();
		delete tcpsocket;
	}
	m_newconnects.clear();

	m_bThreadRun = false;
	if (m_thread != NULL)
	{
		m_thread->stopThread();
		delete m_thread;
	}
}

void ITTcpServer::incomingConnection(qintptr handle)
{
	ITTcpSocket* tcpsocket = new ITTcpSocket(handle);
	QThread* thread = new QThread(tcpsocket);
	connect(this, SIGNAL(sendMsg(const QByteArray&, const int)), tcpsocket, SLOT(sendData(const QByteArray&, const int)));
	//connect(tcpsocket, SIGNAL(disconnected()), thread, SLOT(quit()));
	connect(tcpsocket, SIGNAL(sockDisConnect(int, QString, int)), this, SLOT(doSockDisconnect(int, QString, int)));
	connect(tcpsocket, SIGNAL(readAllData(int, QString, int, const QByteArray&)), this, SLOT(doReadAllData(int, QString, int, const QByteArray&)));
	//connect(thread, &QThread::finished, tcpsocket, &ITTcpSocket::deleteLater);
	//connect(thread, &QThread::finished, thread, &QObject::deleteLater);

	tcpsocket->moveToThread(thread);
	thread->start();
	m_newconnects.insert(handle, tcpsocket);
}

//************************************
// 函数名:  ITTcpServer::sendData
// 描述: 发送数据   
// 参数: const char * senddata   需要发送的数据
// 参数: const int datasize  发送数据的大小
// 返回值: void
//************************************
void ITTcpServer::sendData(const char* senddata, const int datasize)
{
	QByteArray bytearray(senddata, datasize);
	emit sendMsg(bytearray, datasize);
}

//************************************
// 函数名:  ITTcpServer::doSockDisconnect
// 描述:  槽函数 处理客户端连接断开 
// 参数: int socketdescrip  断开连接的SOCKET描述符
// 参数: QString ip  断开连接客户端的IP地址
// 参数: int port    断开连接客户端的端口号
// 返回值: void
//************************************
void ITTcpServer::doSockDisconnect(int socketdescrip, QString ip, int port)
{
	ITTcpSocket* tcpsocket = m_newconnects.value(socketdescrip);
	m_newconnects.remove(socketdescrip);
	QThread* thread = tcpsocket->thread();
	if (thread != NULL)
	{
		thread->wait();
		delete tcpsocket;
	}
}

//************************************
// 函数名:  ITTcpServer::doReadAllData
// 描述:  槽函数 处理接收端客户端发来的数据  
// 参数: int sockdescrip
// 参数: QString ipaddr
// 参数: int port
// 参数: const QByteArray & dataarray
// 返回值: void
//************************************
void ITTcpServer::doReadAllData(int sockdescrip, QString ipaddr, int port, const QByteArray& dataarray)
{
	QMutexLocker locker(&m_callPDataCBLock);
	//if (m_fnReadData != NULL)
	//{
	//	m_fnReadData((char*)ipaddr.toStdString().c_str(), port, (char*)dataarray.data(), dataarray.size(), m_pContext);
	//}
	//缓存过大 删除之前的
	while (m_recvDatas.size() > m_maxBufferCount)
	{
		auto tData = m_recvDatas.takeFirst();
		tData.second.clear();
	}
	m_recvDatas.append(qMakePair<int, QByteArray>(sockdescrip, dataarray));
}


//************************************
// 函数名:  ITTcpServer::threadRunCB
// 描述: 线程执行函数   
// 参数: void * param  线程用户数据
// 返回值: void
//************************************
void ITTcpServer::threadRunCB(void* param)
{
	ITTcpServer* pThis = (ITTcpServer*)param;
	while (pThis->m_bThreadRun)
	{
		{
			QMutexLocker locker(&pThis->m_msglistlock);
			if (pThis->m_sendmsglist.size() > 0)
			{
				QByteArray& bytearray = pThis->m_sendmsglist.front();
				pThis->sendData(bytearray.data(), bytearray.size());
				pThis->m_sendmsglist.pop_front();
			}
		}
		QThread::msleep(100);
	}
}

void ITTcpServer::sendDataToAllClients(const QByteArray& databuf, const int datasize)
{
	QMutexLocker locker(&m_msglistlock);
	m_sendmsglist.append(databuf);
}

void ITTcpServer::SendDataToDstClient(int handle, const QByteArray& data, const int datasize)
{
	if (m_newconnects.contains(handle))
	{
		auto tcpSocket = m_newconnects.value(handle);
		tcpSocket->sendData(data, datasize);
	}
}

QByteArray ITTcpServer::ReadNextRecvData(int& handle)
{
	QMutexLocker locker(&m_callPDataCBLock);

	if (m_recvDatas.size() > 0)
	{
		auto tData = m_recvDatas.takeFirst();
		handle = tData.first;
		return tData.second;
	}
	handle = -1;
	return QByteArray();
}

void ITTcpServer::shutdown()
{
	m_bThreadRun = false;
	if (m_thread != NULL)
		m_thread->stopThread();
	this->close();
	ITTcpSocket* pTcpSocket = NULL;
	for (auto it = m_newconnects.begin(); it != m_newconnects.end(); ++it)
	{
		pTcpSocket = it.value();
		if (pTcpSocket)
		{
			pTcpSocket->disconnectFromHost();
			pTcpSocket->close();
			auto linkThread = pTcpSocket->thread();
			if (linkThread)
			{
				linkThread->quit();
				linkThread->wait();
				linkThread->deleteLater();
			}
			pTcpSocket->deleteLater();

		}
	}
	m_newconnects.clear();
}
