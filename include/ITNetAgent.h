#pragma  once

#include "StdAfx.h"
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include "time.h"

const int BUFLEN = 4096;
class ITNetAgent : public QThread
{
	Q_OBJECT
public:
	typedef int (*PDataCB)(const char* pszBuffer, void* pContext, void* pAgent);
	ITNetAgent(QObject* parent = NULL);
	virtual~ITNetAgent();

	enum NetState
	{
		online = 0,			///正常
		disconnect = 1,		///断开
		noresponse = 2,		///未响应
	};
	///初始化
	void			startThread(void* args = 0);
	///关闭
	void			stopThread();
	///发送数据
	void			AddToSendQ(const QByteArray& szData);

	///设置数据回调函数，当收到一行完整数据时，回调此函数
	void			SetDataCB(PDataCB DataCB, void* pContext);

	///设置要连接的服务器地址和信息
	void			SetServerAddr(const QString& szServerAddr, int nPort);
	QString			getServerIpAddr() { return m_stream->peerAddress().toString(); }
	int				getServerPort() { return m_stream->peerPort(); }
	void			SetServerName(QString& name) { m_sServerName = name; }
	QString			GetServerName() { return m_sServerName; }
	bool			isConnecte() { return m_bConnected; }	//连接状态判断
	int				DoRecv();								//处理接收数据的函数	
public slots:
	void doRecvNewData();					///处理新数据函数
protected:
	void run();								///线程函数
private:
	ITNetAgent* m_pThis;
	QTcpSocket* m_stream;					//通讯的socket
	bool			m_bConnected;			//是否已连接成功的标志
	bool			m_bExit;				//退出的标志
	QString			m_strPeerIpAddr;		//对端ip地址
	int				m_nPeerPort;			//对端端口号
	PDataCB			m_pDataCB;				//处理收到的数据的回调函数
	void* m_pContext;						//调用回调函数的上下文参数
	QString			m_sServerName;			///服务名称
	time_t			m_lastSendHeartTime;	///最后一次发送数据时间 暂未用
	QList<QByteArray> m_sendMsgList;		///发送数据队列		
	QMutex			m_sendMsgListLock;		///发送数据队列锁
	QList<QByteArray> m_recvMsgList;		///接收数据队列		
	QMutex			m_recvMsgListLock;		///接收数据队列锁
	QMutex			m_callPDataCBLock;		///回调函数锁		
};
