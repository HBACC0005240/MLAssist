#pragma once

#include <QTcpServer>
#include <QThread>
#include <QMutex>

class ITThread;
class ITTcpSocket;
class ITTcpServer : public QTcpServer
{
	Q_OBJECT

public:
	ITTcpServer(QObject* parent = 0);
	~ITTcpServer();

	///发送数据给所有连接的SOCKET客户端
	void sendDataToAllClients(const QByteArray& databuf, const int datasize);

	//发送数据给指定客户端
	void SendDataToDstClient(int handle, const QByteArray& data, const int datasize);

	//读取缓存队列里的客户端数据
	QByteArray ReadNextRecvData(int& handle);

	///线程执行函数
	static void threadRunCB(void* param);
	void shutdown();
signals:
	void sendMsg(const QByteArray& data, const int datasize);

public slots:
	///处理客户端连接断开
	void doSockDisconnect(int socketdescrip, QString ip, int port);
	///处理接收端客户端发来的数据
	void doReadAllData(int, QString, int, const QByteArray&);

protected:
	void incomingConnection(qintptr handle);

	///发送数据
	void sendData(const char* senddata, const int datasize);
private:
	///存储所有的套接字连接
	QMap<int, ITTcpSocket*> m_newconnects;

	///发送给所有客户端的数据队列
	QList<QByteArray> m_sendmsglist;

	///发送数据队列锁
	QMutex  m_msglistlock;

	///异步执行线程
	ITThread* m_thread;

	///线程运行标志
	bool m_bThreadRun;
	void* m_pContext;

	///回调函数锁
	QMutex m_callPDataCBLock;

	//收到的客户端数据
	QList<QPair<int, QByteArray>> m_recvDatas;
	int m_maxBufferCount = 100;	//最大缓存数 
};

