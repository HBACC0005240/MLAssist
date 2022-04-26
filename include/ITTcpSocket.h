#pragma once

#include <QTcpSocket>

class ITTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	ITTcpSocket(QObject* parent = 0);
	ITTcpSocket(int socketDescriptor, QObject* parent = 0);
	~ITTcpSocket();
	///TCP连接发送数据,并返回实际发送的大小  
	int sendDataWithReturn(const char* dataBuf, const int dataSize);
signals:
	void sockDisConnect(int, QString, int);
	void readAllData(int, QString, int, const QByteArray&);
public slots:
	///发送数据
	void sendData(const QByteArray& databuf, const int datasize);
	///处理连接断开
	void onDisConnect();
	///处理读取接收数据
	void onReadData();
private:
	int m_socketDescriptor;
};
