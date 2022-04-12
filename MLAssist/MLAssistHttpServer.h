#pragma once

#include "qhttpserver.hpp"
#include "qhttpserverrequest.hpp"
#include "qhttpserverresponse.hpp"
#include <QObject>
#pragma comment(lib, "../qhttp/xbin/qhttp.lib")
using namespace qhttp::server;
class MLAssistHttpServer : public QObject
{
	Q_OBJECT

public:
	MLAssistHttpServer();
	~MLAssistHttpServer();

	static MLAssistHttpServer *getInstace();
	void init();

public slots:
	void deal_new_request(QHttpRequest *request, QHttpResponse *response);

protected:
	void deal_get_request(QHttpRequest *request, QHttpResponse *response);
	void deal_post_request(QHttpRequest *request, QHttpResponse *response);

private:
	QHttpServer _httpServer;
};
#define g_pHttpServer MLAssistHttpServer::getInstace()
