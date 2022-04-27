#include "StdAfx.h"
#include "ITNetAgent.h"
#include <QTextCodec>
extern QTextCodec* g_textCodec;
ITNetAgent::ITNetAgent(QObject* parent)
	: QThread(parent)
{
	m_bExit = false;
	m_bConnected = false;
	m_pDataCB = NULL;
	m_pContext = NULL;
	m_sServerName = "";
	m_lastSendHeartTime = time(NULL);
	m_nPeerPort = 0;
	m_strPeerIpAddr = "";
	m_stream = NULL;
	m_pThis = NULL;
}
ITNetAgent::~ITNetAgent()
{

}

//************************************
//函数名:  startThread(void* args)
//描述：启动线程函数
//参数：this指针  未用
//返回值：无
//时间：2015/6/3 WZQ
//************************************
void ITNetAgent::startThread(void* args)
{
	m_bExit = false;
	m_pThis = this;
	//启动线程
	start();
	return;
}

//************************************
//函数名:  stopThread
//描述：停止线程函数
//参数：无
//返回值：无
//时间：2015/6/3 WZQ
//************************************
void ITNetAgent::stopThread()
{
	m_bExit = true;
	//等待线程退出
	wait();
	if (m_stream)
	{
		m_stream->close();
		delete m_stream;
		m_stream = NULL;
	}
	qDebug() << QString("%1服务(端口号：%2)退出连接").arg(m_sServerName).arg(m_nPeerPort);
	m_bConnected = false;
}

//************************************
//函数名:  DoRecv
//描述：接收数据函数
//参数：无
//返回值： -1:对方连接中断；-2:回调函数要求退出，可能是收到退出的协议命令
//			0：没有收到数据；>0 接收到的数据 的长度
//时间：2015/6/3 WZQ
//************************************
int ITNetAgent::DoRecv()
{
	int nRet = 0;
	if (m_stream->waitForReadyRead(200))	///接收超时200毫秒
	{
		//QByteArray bytearray = m_stream->readAll();
		doRecvNewData();
	}
	if (m_stream->state() != QAbstractSocket::ConnectedState)
	{
		m_stream->close();
		m_bConnected = false;
		nRet = -1;
		qDebug() << QString("%1服务(端口号：%2)连接断开").arg(m_sServerName).arg(m_nPeerPort);
	}
	return nRet;
}
//************************************
//函数名:  run
//描述：线程函数
//参数：无
//返回值：无
//时间：2015/6/3 WZQ
//************************************
void ITNetAgent::run()
{
	int	nRetryCnt = 0;
	m_stream = new QTcpSocket;
	while (!m_bExit)
	{
		if (!m_bConnected)
		{
			m_stream->connectToHost(m_strPeerIpAddr, m_nPeerPort, QIODevice::ReadWrite);
			m_bConnected = m_stream->waitForConnected(3000);
			if (m_bConnected)
			{
				if (m_pDataCB)
				{
					//m_pDataCB(g_textCodec->fromUnicode(QString("#HBT:%1=%2").arg(m_sServerName).arg(ITNetAgent::online)), m_pContext, this);
				}
				nRetryCnt = 0;
				qDebug() << QString("%1服务(端口号：%2)连接成功").arg(m_sServerName).arg(m_nPeerPort);
			}
			else
			{
				++nRetryCnt;
				//断开后，retry20次后，大概100秒，更新状态 连接时 传递了超时3秒设置  计算下来差不多100秒
				if (nRetryCnt == 10)
				{
					if (m_pDataCB)
					{
						//m_pDataCB(g_textCodec->fromUnicode(QString("#HBT:%1=%2").arg(m_sServerName).arg(ITNetAgent::disconnect)), m_pContext, this);
					}
				}
			}
		}
		else
		{
			if (m_sendMsgList.size() > 0)
			{
				QMutexLocker locker(&m_sendMsgListLock);
				QByteArray& bytearray = m_sendMsgList.front();
				if (m_stream->write(bytearray.data(), bytearray.size()) == -1)
				{
					m_stream->close();
					m_bConnected = false;
					qDebug() << QString("%1服务(端口号：%2)连接断开").arg(m_sServerName).arg(m_nPeerPort);
					//		m_sendMsgList.pop_front();	///不弹栈 
				}
				m_sendMsgList.pop_front();
			}
			else		///没有数据  处理接收数据			
			{
				//发送缓冲区没有待发数据，则处理接收数据				
				int nRes = DoRecv();
				if (nRes == -2)
					break;
				if (nRes <= 0)//没有接收到数据时每30s发送一次心跳，用于检测连接是否断开
				{
					/*time_t curTime = time(NULL);
					if (difftime(curTime, m_lastSendHeartTime) > 30)
					{
						m_lastSendHeartTime = curTime;
						AddToSendQ("&&&\r\n");
					}*/
				}
			}
		}
	}
}

//************************************
//函数名:  SetDataCB(PDataCB DataCB,void* pContext)
//描述：设置回调信息
//参数：回调函数，回调处理指针
//返回值：无
//时间：2015/6/3 WZQ
//************************************
void ITNetAgent::SetDataCB(PDataCB DataCB, void* pContext)
{
	QMutexLocker locked(&m_callPDataCBLock);
	m_pDataCB = DataCB;
	m_pContext = pContext;

}

QByteArray ITNetAgent::ReadNextRecvData()
{
	QMutexLocker locker(&m_recvMsgListLock);

	if (m_recvMsgList.size() > 0)
	{
		auto tData = m_recvMsgList.takeFirst();
		return tData;
	}
	return QByteArray();
}

//************************************
//函数名:  SetServerAddr(const QString& szServerAddr,int nPort)
//描述：设置服务器地址信息
//参数：服务器地址，端口
//返回值：无
//时间：2015/6/3 WZQ
//************************************
void ITNetAgent::SetServerAddr(const QString& szServerAddr, int nPort)
{
	m_strPeerIpAddr = szServerAddr;
	m_nPeerPort = nPort;
}

//************************************
//函数名:  AddToSendQ(const QByteArray& szData)
//描述：把数据加入发送队列
//参数：待发数据
//返回值：无
//时间：2015/6/3 WZQ
//************************************
void ITNetAgent::AddToSendQ(const QByteArray& szData)
{
	QMutexLocker locker(&m_sendMsgListLock);
	m_sendMsgList.append(szData);

}
//************************************
//函数名:  doRecvNewData
//描述：接收新数据函数
//参数：无
//返回值：无
//时间：2015/6/3 WZQ
//************************************
void ITNetAgent::doRecvNewData()
{
	if (!m_stream)
		return;
	QMutexLocker locker(&m_recvMsgListLock);
	QByteArray bytearray = m_stream->readAll();
	while (m_recvMsgList.size() > m_maxBufferCount)
	{
		auto tData = m_recvMsgList.takeFirst();
		tData.clear();
	}
	m_recvMsgList.append(bytearray);
}
