#include "ITTcpSocket.h"
#include <QHostAddress>

ITTcpSocket::ITTcpSocket(int socketDescriptor, QObject* parent)
	: QTcpSocket(parent)
{
	m_socketDescriptor = socketDescriptor;
	setSocketDescriptor(socketDescriptor);
	connect(this, SIGNAL(readyRead()), this, SLOT(onReadData()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisConnect()));
}

ITTcpSocket::ITTcpSocket(QObject* parent /* = 0 */)
	:QTcpSocket(parent)
{

}

ITTcpSocket::~ITTcpSocket()
{

}

//************************************
// 函数名:  ITTcpSocket::sendData
// 描述:  发送数据  
// 参数: const QByteArray & databuf  被发送数据
// 参数: const int datasize  被发送数据的大小
// 返回值: void
//************************************
void ITTcpSocket::sendData(const QByteArray& databuf, const int datasize)
{
	if (databuf.size() < datasize)
		return;
	writeData(databuf.data(), datasize);
}

//************************************
// 函数名:  ITTcpSocket::sendDataWithReturn
// 描述:  TCP连接发送数据,并返回实际发送的大小  
// 参数: const char * dataBuf  需要发送的数据
// 参数: const int dataSize  发送数据的大小
// 返回值: int
//************************************
int ITTcpSocket::sendDataWithReturn(const char* dataBuf, const int dataSize)
{
	if (dataBuf == NULL)
		return 0;
	if (dataSize <= 0)
		return 0;

	return writeData(dataBuf, dataSize);
}
//************************************
// 函数名:  ITTcpSocket::onDisConnect
// 描述:   槽函数 处理连接断开 
// 返回值: void
//************************************
void ITTcpSocket::onDisConnect()
{
	emit sockDisConnect(m_socketDescriptor, peerAddress().toString(), peerPort());
}

void ITTcpSocket::onReadData()
{
	QByteArray bytearray = readAll();
	emit readAllData(m_socketDescriptor, peerAddress().toString(), peerPort(), bytearray);
}

