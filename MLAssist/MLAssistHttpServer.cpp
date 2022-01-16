#include "MLAssistHttpServer.h"
#include "GameCtrl.h"

MLAssistHttpServer::MLAssistHttpServer()
{
	connect(&_httpServer, SIGNAL(newRequest(QHttpRequest *, QHttpResponse *)), this, SLOT(deal_new_request(QHttpRequest *, QHttpResponse *)));
}

MLAssistHttpServer::~MLAssistHttpServer()
{
}

MLAssistHttpServer *MLAssistHttpServer::getInstace()
{
	static MLAssistHttpServer pHttpServer;
	return &pHttpServer;
}

void MLAssistHttpServer::init()
{

	for (unsigned int qport = 14396; qport < 14396 + 1000; ++qport)
	{
		if (_httpServer.listen(QHostAddress::LocalHost, qport))
		{
			QByteArray qportString = QString("%1").arg(qport).toLocal8Bit();
			qputenv("CGA_GUI_PORT", qportString);
			break;
		}
	}
}

void MLAssistHttpServer::deal_new_request(QHttpRequest *req, QHttpResponse *res)
{
	req->collectData();
	req->end();
	res->addHeader("connection", "close");
	if (req->method() == qhttp::THttpMethod::EHTTP_GET)
	{
		auto path = req->url().path();
		if (path.indexOf("/cga/") == 0)
		{
			auto subreq = path.mid(sizeof("/cga/") - 1);
			if (0 == subreq.compare("GetGameProcInfo"))
			{
				QJsonDocument doc;
				g_pGameCtrl->HttpGetGameProcInfo(&doc);
				res->setStatusCode(qhttp::ESTATUS_OK);
				res->end(doc.toJson());
				return;
			}
			else if (0 == subreq.compare("GetSettings"))
			{
				QJsonDocument doc;
				g_pGameCtrl->HttpGetSettings(doc);
				res->setStatusCode(qhttp::ESTATUS_OK);
				res->end(doc.toJson());
				return;
			}
		}
	}
	else if (req->method() == qhttp::THttpMethod::EHTTP_POST)
	{
		auto path = req->url().path();
		if (path.indexOf("/cga/") == 0)
		{
			auto subreq = path.mid(sizeof("/cga/") - 1);
			if (0 == subreq.compare("LoadSettings"))
			{
				auto reqData = req->collectedData();
				QJsonDocument doc;
				g_pGameCtrl->HttpLoadSettings(req->url().query(), reqData, &doc);
				res->setStatusCode(qhttp::ESTATUS_OK);
				res->end(doc.toJson());
				return;
			}
			else if (0 == subreq.compare("LoadScript"))
			{
				auto reqData = req->collectedData();
				QJsonDocument doc;
				g_pGameCtrl->HttpLoadScript(req->url().query(), reqData, &doc);
				res->setStatusCode(qhttp::ESTATUS_OK);
				res->end(doc.toJson());
				return;
			}
			else if (0 == subreq.compare("LoadAccount"))
			{
				auto reqData = req->collectedData();

				QJsonDocument doc;
				g_pGameCtrl->HttpLoadAccount(req->url().query(), reqData, &doc);
				res->setStatusCode(qhttp::ESTATUS_OK);
				res->end(doc.toJson());
				return;
			}
		}
	}

	res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
	res->end(QByteArray("invalid request"));
}