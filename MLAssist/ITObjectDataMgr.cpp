#include "ITObjectDataMgr.h"
#include "./Astar/AStarUtil.h"
#include "GPCalc.h"
#include "ITObjectID.h"
#include "RpcSocketClient.h"
#include "StdAfx.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QTextCodec>
#include <QtConcurrent>
#ifdef Q_OS_UNIX
#include "unistd.h"
#endif
#pragma comment(lib, "../ThirdPart/qmqtt/x32/release/lib/Qt5Mqtt.lib")
ObjectModuleRegisty::~ObjectModuleRegisty(void)
{
	for (auto it = moduleInfoList.begin(); it != moduleInfoList.end(); ++it)
	{
		if (it.value())
			delete it.value();
	}
	moduleInfoList.clear();
}
ObjectModuleRegisty &ObjectModuleRegisty::GetInstance()
{
	static ObjectModuleRegisty instance;
	return instance;
}

void ObjectModuleRegisty::RegisterModuleFactory(int nObjType, ObjectModuleFactory *pFactory, QString sIconPath)
{
	if (moduleInfoList.contains(nObjType))
		return;
	moduleInfoList.insert(nObjType, pFactory);
}
ITObjectPtr ObjectModuleRegisty::CreateNewObject(int objType)
{
	auto pModule = moduleInfoList.value(objType);
	if (pModule)
	{
		return pModule->Create();
	}
	return nullptr;
}

ITObjectDataMgr::ITObjectDataMgr(void)
{
	m_dbconn = NULL;
	m_bDBConnState = false;
	qRegisterMetaType<ITGameItemPtr>("ITGameItemPtr");
	qRegisterMetaType<ITGamePetPtr>("ITGamePetPtr");
	qRegisterMetaType<ITGameMapPtr>("ITGameMapPtr");
	qRegisterMetaType<ITGameGateMapPtr>("ITGameGateMapPtr");

	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_GateMap, NEW_MODULE_FACTORY(ITGameGateMap));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Pet, NEW_MODULE_FACTORY(ITGamePet));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Map, NEW_MODULE_FACTORY(ITGameMap));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_AccountAssemble, NEW_MODULE_FACTORY(ITAccountAssemble));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Account, NEW_MODULE_FACTORY(ITAccount));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_AccountGid, NEW_MODULE_FACTORY(ITAccountGid));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_AccountRole, NEW_MODULE_FACTORY(ITAccountRole));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_GidRoleLeft, NEW_MODULE_FACTORY(ITAccountRole));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_GidRoleRight, NEW_MODULE_FACTORY(ITAccountRole));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_RoleRunConfig, NEW_MODULE_FACTORY(ITAccountGidRoleRunConfig));

	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Item, NEW_MODULE_FACTORY(ITGameItem));
	connect(this, SIGNAL(signal_loadDataFini()), this, SLOT(doLoadDBInfoOver())); //这个是加载数据库完成
	connect(this, &ITObjectDataMgr::signal_publishMqttMsg, this, &ITObjectDataMgr::on_publishMqttMsg, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, SIGNAL(signal_attachGame()), this, SLOT(on_attachGame_sucess()));
	/*connect(g_pGameCtrl, &GameCtrl::NotifyGameItemsInfo, this, &ITObjectDataMgr::OnNotifyGetItemsInfo, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyGamePetsInfo, this, &ITObjectDataMgr::OnNotifyGetPetsInfo, Qt::ConnectionType::QueuedConnection);*/
}

ITObjectDataMgr::~ITObjectDataMgr(void)
{
}

bool ITObjectDataMgr::init()
{
	QString iniPath = QCoreApplication::applicationDirPath() + "/config.ini";
	QSettings iniFile(iniPath, QSettings::IniFormat);
	QString sServerIp = iniFile.value("server/ip", "www.wzqlive.com").toString();
	QString sServerPort = iniFile.value("server/port", "50051").toString();
	int startHide = iniFile.value("game/startHide", 0).toInt();
	int followPos = iniFile.value("game/followPos", 0).toInt();
	g_pGameCtrl->SetStartGameHide(startHide);
	g_pGameCtrl->SetFollowGamePos(followPos);

	m_client = new QMqttClient;
	m_client->setHostname("www.luguo666.com");
	m_client->setPort(1883);

	connect(m_client, &QMqttClient::stateChanged, this, &ITObjectDataMgr::updateLogStateChange);
	connect(m_client, &QMqttClient::disconnected, this, &ITObjectDataMgr::brokerDisconnected);
	connect(m_client, &QMqttClient::messageReceived, this, &ITObjectDataMgr::OnRecvMessage);
	connect(m_client, &QMqttClient::connected, this, &ITObjectDataMgr::OnMqttConnected);
	//connect(m_client, &QMqttClient::connected, [this]()
	//		{
	//			auto sub = m_client->subscribe(QLatin1String("qtmqtt/topic1"));
	//			m_client->connect(sub, &QMqttSubscription::stateChanged, [this](QMqttSubscription::SubscriptionState s)
	//					{
	//						qInfo() << "Subscription state:" << s;
	//						if (s == QMqttSubscription::Unsubscribed)
	//							m_client->disconnectFromHost();
	//					});
	//			/*m_client->connect(sub, &QMqttSubscription::messageReceived, [](const QMqttMessage &msg)
	//					{ qInfo() << "ID:" << msg.id()
	//							  << "Topic:" << msg.topic().name()
	//							  << "QoS:" << msg.qos()
	//							  << "Retain:" << msg.retain()
	//							  << "Duplicate:" << msg.duplicate()
	//							  << "Payload:" << msg.payload().left(50) << (msg.payload().size() > 50 ? "..." : ""); });*/

	//		});

	connect(m_client, &QMqttClient::pingResponseReceived, this, [this]()
			{
				const QString content = QDateTime::currentDateTime().toString() + QLatin1String(" PingResponse") + QLatin1Char('\n');
				qDebug() << content;
			});
	connect(&m_mqttTimer, SIGNAL(timeout()), this, SLOT(OnCheckConnectMqtt()));
	m_mqttTimer.start(10000);
	//m_client->publish("qtmqtt/topic2", "112323");
	//auto subscription = m_client->subscribe(QLatin1String ("qtmqtt/topic1"));
	//if (!subscription)
	//{
	//	qDebug() << QLatin1String("Could not subscribe. Is there a valid connection?");
	//}
	RpcSocketClient::getInstance().setServerIp(sServerIp);
	RpcSocketClient::getInstance().setServerPort(sServerPort);
	RpcSocketClient::getInstance().init();
	if (RpcSocketClient::getInstance().isConnected() && RpcSocketClient::getInstance().GetConnectState()) //貌似不能用这个判断 服务端如果没启动 这边还是返回true
	{
		//服务端数据
		//连接成功后
		//1、获取宠物数据用于算档
		//2、获取地图数据用户通知服务器对比更新
		//3、item实时通信获取但不获取所有，获取当前包裹的，并本地缓存，发现获取的服务器数据不是最新，则发通知服务器更新
		auto petData = RpcSocketClient::getInstance().GetPetGradeCalcData(); //这个是图鉴部分  和ITGamePet还不一样
		g_pGamePetCalc->setCaclPetData(petData);
		if (petData.size() > 0)
		{
			for (auto it=petData.begin();it!=petData.end();++it)
			{
				auto petBook = it.value();
				ITGamePetPtr pObj = newOneObject(TObject_Pet).dynamicCast<ITGamePet>();
				if (pObj)
				{
					pObj->_petNumber = petBook->number;
					pObj->_petRaceType = petBook->raceType;
					pObj->_petRace = petBook->raceTypeName;
					pObj->_petBaseBp = petBook->baseBP;
					pObj->_growVigor = petBook->bestBPGrade[0];
					pObj->_growStrength = petBook->bestBPGrade[1];
					pObj->_growDefense = petBook->bestBPGrade[2];
					pObj->_growAgility = petBook->bestBPGrade[3];
					pObj->_growMagic = petBook->bestBPGrade[4];
					pObj->_canCatch = petBook->can_catch;
					pObj->_cardType = petBook->card_type;
					pObj->_imageId = petBook->image_id;
					pObj->_skillCount = petBook->skill_slots;
					pObj->_elementEarth = petBook->element_earth;
					pObj->_elementWater = petBook->element_water;
					pObj->_elementFire = petBook->element_fire;
					pObj->_elementWind = petBook->element_wind;
					pObj->setObjectName(petBook->name);
					//pObj->setObjectDsec(sDesc);
					//pObj->setObjectCode(nCode);
				}
				m_numberForPet.insert(petBook->number, pObj);
			}
			
		}
		RpcSocketClient::getInstance().GetServerStoreMapData();
	}
	else
	{ //离线时候 算档数据同步过去
		bool isLoadOfflineDb = iniFile.value("db/loadOfflineDB", false).toBool();
		if (isLoadOfflineDb)
		{
			QString sDBPath = QApplication::applicationDirPath() + "//db//cg.db";
			bool bRet = false;
			m_dbconn = ITDataBaseConnPtr(new ITDataBaseConn("SQLITE"));
			if (connectToDB("SQLITE", "CG", sDBPath, "admin", "123456"))
			{
				qDebug() << "打开数据库成功！";
				//		QtConcurrent::run(loadDataBaseInfo, this);
				loadDataBaseInfo(this);
				auto petData = LoadPetBook();
				g_pGamePetCalc->setCaclPetData(petData);
				bRet = true;
			}
			else
			{
				if (m_dbconn)
					qDebug() << m_dbconn->getLastError();
				bRet = false;
				qDebug() << "打开数据库错误！";
			}
			QtConcurrent::run(SaveDataThread, this);
		}
	}
	return true;
}

bool ITObjectDataMgr::Fini()
{
	if (!m_client)
		return false;
	m_client->disconnectFromHost();
	return true;
}
//没有加类型判断 需要的话 加一个
QString ITObjectDataMgr::FindItemCodeName(int nCode)
{
	for (auto pObj : m_pObjectList)
	{
		if (pObj->getObjectCode() == nCode)
		{
			return pObj->getObjectName();
		}
	}
	for (auto pObj : m_pAddObjectList)
	{
		if (pObj->getObjectCode() == nCode)
		{
			return pObj->getObjectName();
		}
	}
}

ITObjectDataMgr &ITObjectDataMgr::getInstance(void)
{
	static ITObjectDataMgr mitsObjicedata;
	return mitsObjicedata;
}

QPoint ITObjectDataMgr::GetRandomSpaceOffLine(int mapIndex, int x, int y, int distance /*= 1*/)
{
	QImage mapImage = m_mapIndexForData.value(mapIndex);
	if (mapImage.width() <= 0 || mapImage.height() <= 0)
	{
		bool bLoad = LoadOffLineMapImageData(mapIndex, mapImage);
		if (!bLoad)
			return QPoint();
		m_mapIndexForData.insert(mapIndex, mapImage);
	}

	int nTempX = 0;
	int nTempY = 0;
	int w = mapImage.width();
	int h = mapImage.height();
	QColor qRgb(0, 0, 0);
	int xtop = h;
	int ytop = w;
	if (x > w || y > h)
	{
		return QPoint();
	}
	do
	{
		nTempX = x - distance;
		nTempY = y;
		auto FindFun = [&](int x, int y)
		{ return mapImage.pixelColor(x, y) != qRgb; };
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x + distance;
		nTempY = y;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x;
		nTempY = y - distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x;
		nTempY = y + distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x + distance;
		nTempY = y + distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x - distance;
		nTempY = y + distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x + distance;
		nTempY = y - distance;
		if (FindFun(nTempX, nTempY))
			break;
		nTempX = x - distance;
		nTempY = y - distance;
		if (FindFun(nTempX, nTempY))
			break;
	} while (0);

	qDebug() << nTempX << nTempY;
	return QPoint(nTempX, nTempY);
}

QMap<QString, QSharedPointer<CGPetPictorialBook> > ITObjectDataMgr::LoadPetBook()
{
	QMap<QString, QSharedPointer<CGPetPictorialBook> > petBookData;
	for (auto it = m_numberForPet.begin(); it != m_numberForPet.end(); ++it)
	{
		auto petData = it.value();
		QSharedPointer<CGPetPictorialBook> pPet(new CGPetPictorialBook);
		pPet->number = petData->_petNumber;
		pPet->name = petData->getObjectName();
		pPet->raceType = petData->_petRaceType;
		pPet->bestBPGrade[0] = petData->_growVigor;
		pPet->bestBPGrade[1] = petData->_growStrength;
		pPet->bestBPGrade[2] = petData->_growDefense;
		pPet->bestBPGrade[3] = petData->_growAgility;
		pPet->bestBPGrade[4] = petData->_growMagic;

		pPet->raceTypeName = petData->_petRace;
		pPet->can_catch = petData->_canCatch;
		pPet->card_type = petData->_cardType;
		pPet->image_id = petData->_imageId;
		/*pPet->rate_endurance = petData.rate_endurance();
		pPet->rate_strength = petData.rate_strength();
		pPet->rate_defense = petData.rate_defense();
		pPet->rate_agility = petData.rate_agility();
		pPet->rate_magical = petData.rate_magical();*/
		pPet->element_earth = petData->_elementEarth;
		pPet->element_water = petData->_elementWater;
		pPet->element_fire = petData->_elementFire;
		pPet->element_wind = petData->_elementWind;
		pPet->skill_slots = petData->_skillCount;
		petBookData.insert(pPet->name, pPet);
	}
	return petBookData;
}

void ITObjectDataMgr::AddNewSubscribe(const QStringList &subscribe)
{
	if (m_client && m_client->state() == QMqttClient::Connected)
	{
		//连接状态 进行订阅 不是连接 也就没有订阅
		for (auto oldSub : m_customSubscribeList)
		{
			if (!subscribe.contains(oldSub))
			{
				m_client->unsubscribe(oldSub);
			}
		}
	}
	m_customSubscribeList = subscribe;
	m_retrySubscribes = m_customSubscribeList;
}

void ITObjectDataMgr::PublishOneTopic(const QString &topic, const QString &msg)
{
	if (!m_client)
		return;
	m_client->publish(topic, msg.toUtf8());
}

void ITObjectDataMgr::DebugNavigationRoute(ITGameGateMapPtr pRoute, int nRoute, int &nTotalCost)
{
	int nCurRoute = 0;
	auto pNavigationList = pRoute->_navigationList;

	for (auto tmpGateMap : pNavigationList)
	{
		QString sRoute = QString("%1%2").arg(nRoute).arg(++nCurRoute);
		qDebug() << QString("路线%1 ").arg(sRoute)
				 << "源 " << tmpGateMap->_mapNumber << "目标 " << tmpGateMap->_targetMapNumber
				 << "传送点 " << tmpGateMap->_x << "," << tmpGateMap->_y
				 << "到达点 " << tmpGateMap->_tx << "," << tmpGateMap->_ty << "成本" << tmpGateMap->_nCost;
		int nowLineCost = nTotalCost + tmpGateMap->_nCost;
		if (tmpGateMap->_navigationList.size() < 1)
		{
			qDebug() << "此路线花费：" << nowLineCost;
		}
		else
			DebugNavigationRoute(tmpGateMap, sRoute.toInt(), nowLineCost);
	}
}

void ITObjectDataMgr::StatisticsRoute(ITRouteNodePtr pRoute)
{
	if (!pRoute->cur)
		return;

	auto pNavigationList = pRoute->cur->_navigationList;
	for (auto tmpGateMap : pNavigationList)
	{
		ITRouteNodePtr routeNode(new ITRouteNode());
		routeNode->parent = pRoute;
		routeNode->cur = tmpGateMap;

		if (tmpGateMap->_navigationList.size() < 1)
		{
			//一条路线
			m_reachableRouteList.append(BackMapTraceRouteNode(routeNode));
		}
		else
			StatisticsRoute(routeNode);
	}
}

bool ITObjectDataMgr::IsReachableTargetNavigationRoute(ITGameGateMapPtr pRoute, int sx, int sy, int tx, int ty, int tgtIndex)
{
	int nCost = 0;
	if (pRoute->_targetMapNumber == tgtIndex)
	{
		if (IsReachableTarget(tgtIndex, pRoute->_tx, pRoute->_ty, tx, ty, nCost))
		{
			pRoute->_nCost += nCost; //最后一个在计算上次坐标到达自己传送点的cost基础上 加上 到达目标图传送点到指定坐标的cost
			return true;
		}
		return false;
	}
	bool bReachable = false;
	auto pNavigationList = pRoute->_navigationList;
	for (auto tmpGateMap : pNavigationList)
	{
		nCost = 0;
		tmpGateMap->_nCost = 0;
		//人物当前坐标 是否能到达传送点
		//如果需要对话的话 目标坐标算附近空格
		auto tmpTgtPos = QPoint(tmpGateMap->_x, tmpGateMap->_y);
		if (tmpGateMap->_warpType != 0)
			tmpTgtPos = GetRandomSpaceOffLine(tmpGateMap->_mapNumber, tmpGateMap->_x, tmpGateMap->_y, 1);
		if (IsReachableTarget(tmpGateMap->_mapNumber, sx, sy, tmpTgtPos.x(), tmpTgtPos.y(), nCost))
		{
			tmpGateMap->_nCost = nCost; //上次传送落在当前图的位置 和本次传送坐标位置的cost
			if (IsReachableTargetNavigationRoute(tmpGateMap, tmpGateMap->_tx, tmpGateMap->_ty, tx, ty, tgtIndex))
			{
				bReachable = true;
			}
			else
			{
				pRoute->_navigationList.removeOne(tmpGateMap);
			}
		}
		else
		{
			pRoute->_navigationList.removeOne(tmpGateMap);
		}
	}
	return bReachable;
}

QString ITObjectDataMgr::FindItemCodeNameFromDB(int nCode)
{
	if (m_dbconn == nullptr)
		return "";
	QString sql = QString("select * from item where code = %1").arg(nCode);
	auto pRecord = m_dbconn->execQuerySql(sql);
	if (pRecord)
	{
		while (pRecord->next())
		{
			return pRecord->getStrValue("name");
		}
	}
	//宠物卡片编号
	sql = QString("select * from Pet where code = %1").arg(nCode);
	pRecord = m_dbconn->execQuerySql(sql);
	if (pRecord)
	{
		while (pRecord->next())
		{
			return pRecord->getStrValue("name");
		}
	}
	return "";
}

ITGameItemPtr ITObjectDataMgr::FindItemFromCode(int nCode)
{
	QMutexLocker locker(&m_storeItemMutex);
	return m_codeForGameItem.value(nCode);
}

ITGameGateMapPtr ITObjectDataMgr::BackMapTrace(ITGameGateMapPtr node)
{
	int nTotalCost = 0;
	ITGameGateMapPtr bestRoute;
	while (node->_parent)
	{
		nTotalCost += node->_nCost;
		bestRoute = node->_parent;
		bestRoute->_subGateMap = node;
		node = node->_parent;
	}
	nTotalCost += bestRoute->_nCost;
	bestRoute->_nTotalCost = nTotalCost;
	return bestRoute;
}

ITRouteNodePtr ITObjectDataMgr::BackMapTraceRouteNode(ITRouteNodePtr pRoute)
{
	if (!pRoute)
		return nullptr;

	int nTotalCost = pRoute->cur->_nCost;
	ITRouteNodePtr curNode(new ITRouteNode());
	curNode->parent = nullptr;
	curNode->cur = pRoute->cur;

	ITRouteNodePtr rootNode = curNode;
	while (pRoute->parent)
	{
		ITRouteNodePtr parentNode(new ITRouteNode());
		parentNode->parent = nullptr;
		parentNode->cur = pRoute->parent->cur;

		parentNode->next = curNode;
		curNode = parentNode;
		rootNode = parentNode;

		nTotalCost += pRoute->parent->cur->_nCost;

		pRoute = pRoute->parent;
	}
	rootNode->nTotalCost = nTotalCost;
	return rootNode;
	//int nTotalCost = 0;
	//ITRouteNodePtr bestRoute;
	//while (pRoute->parent)
	//{
	//	pRoute->parent->next = pRoute;
	//	bestRoute = pRoute->parent;
	//	pRoute = pRoute->parent;
	//	nTotalCost += pRoute->cur->_nCost;
	//}
	//pRoute->nTotalCost = nTotalCost;
	//return pRoute;
}

ITGameGateMapList ITObjectDataMgr::FindTargetNavigation(int tgtIndex, QPoint tgtPos)
{
	QString curMapName = g_pGameFun->GetMapName();
	int curMapIndex = g_pGameFun->GetMapIndex();	  //1500;
	QPoint curPoint = g_pGameFun->GetMapCoordinate(); //QPoint(27, 82); //
	auto pGateMapList = GetDstObjTypeList(TObject_GateMap);
	//1、找当前图到达的所有路径
	ITGameGateMapList pSelectGateMapList;
	for (auto tmpGateMap : pGateMapList)
	{
		auto pGateMap = tmpGateMap.dynamicCast<ITGameGateMap>();
		if (pGateMap->_mapNumber == curMapIndex)
		{
			pSelectGateMapList.append(pGateMap);
		}
		pGateMap->_navigationList.clear();
	}
	//遍历，看哪个路线，可以到达目标地图
	//要递归
	/*ITRouteNodeList pRouteNodeList;
	ITRouteNodePtr oneRoute(new ITRouteNode);
	oneRoute->cur = tmpGateMap;*/
	ITGameGateMapList pNavigationList;
	for (auto tmpGateMap : pSelectGateMapList)
	{
		QList<int> traceRouteList;
		traceRouteList.append(tmpGateMap->_mapNumber);
		auto pGateMap = tmpGateMap.dynamicCast<ITGameGateMap>();
		bool bRet = FindTargetNavigationEx(pGateMap, tgtIndex, traceRouteList);
		if (bRet)
		{
			pNavigationList.append(pGateMap);
		}
	}
	//打印查找的路线
	int nRoute = 0;
	int nCost = 0;
	auto pRealRouteList = pNavigationList;
	for (auto tmpGateMap : pNavigationList)
	{
		//qDebug() << QString("路线%1 ").arg(++nRoute)
		//		 << "源 " << tmpGateMap->_mapNumber << "目标 " << tmpGateMap->_targetMapNumber
		//		 << "传送点 " << tmpGateMap->_x << "," << tmpGateMap->_y
		//		 << "到达点 " << tmpGateMap->_tx << "," << tmpGateMap->_ty;
		//DebugNavigationRoute(tmpGateMap, nRoute);
		//人物当前坐标 是否能到达传送点
		nCost = 0;
		tmpGateMap->_nCost = 0; ///先重置

		//如果需要对话的话 目标坐标算附近空格
		auto tmpTgtPos = QPoint(tmpGateMap->_x, tmpGateMap->_y);
		if (tmpGateMap->_warpType != 0)
			tmpTgtPos = GetRandomSpaceOffLine(tmpGateMap->_mapNumber, tmpGateMap->_x, tmpGateMap->_y, 1);

		if (IsReachableTarget(tmpGateMap->_mapNumber, curPoint.x(), curPoint.y(), tmpTgtPos.x(), tmpTgtPos.y(), nCost))
		{
			tmpGateMap->_nCost = nCost; //到达当前传送点成本
			//当前可达  继续判断子项是否可达
			if (!IsReachableTargetNavigationRoute(tmpGateMap, tmpGateMap->_tx, tmpGateMap->_ty, tgtPos.x(), tgtPos.y(), tgtIndex))
				pRealRouteList.removeOne(tmpGateMap);
		}
		else
			pRealRouteList.removeOne(tmpGateMap);
	}
	m_reachableRouteList.clear();
	for (auto tmpGateMap : pRealRouteList)
	{
		qDebug() << QString("路线%1 ").arg(++nRoute)
				 << "源 " << tmpGateMap->_mapNumber << "目标 " << tmpGateMap->_targetMapNumber
				 << "传送点 " << tmpGateMap->_x << "," << tmpGateMap->_y
				 << "到达点 " << tmpGateMap->_tx << "," << tmpGateMap->_ty << "成本" << tmpGateMap->_nCost;
		int nTotalCost = tmpGateMap->_nCost;

		ITRouteNodePtr routeNode(new ITRouteNode());
		routeNode->parent = nullptr;
		routeNode->cur = tmpGateMap;

		if (tmpGateMap->_navigationList.size() < 1)
		{
			qDebug() << "总花费:" << nTotalCost;
			//一条路线
			routeNode->nTotalCost = tmpGateMap->_nCost;
			m_reachableRouteList.append(BackMapTraceRouteNode(routeNode));
		}
		else
		{

			StatisticsRoute(routeNode);
			DebugNavigationRoute(tmpGateMap, nRoute, nTotalCost);
		}
	}
	ITGameGateMapList bestRouteList;
	ITRouteNodePtr bestRouteNode = nullptr;
	for (auto route : m_reachableRouteList)
	{
		if (!bestRouteNode)
			bestRouteNode = route;
		if (bestRouteNode->nTotalCost > route->nTotalCost)
			bestRouteNode = route;
	}
	if (bestRouteNode)
	{
		bestRouteList.append(bestRouteNode->cur);
		while (bestRouteNode->next)
		{
			bestRouteList.append(bestRouteNode->next->cur);
			bestRouteNode = bestRouteNode->next;
		}
	}
	//nRoute = 0;
	//for (auto route : m_reachableRouteList)
	//{
	//	auto routeNode = route;
	//	//要取子级  next
	//	while (routeNode->next)
	//	{
	//		auto tmpGateMap = routeNode->cur;

	//		qDebug() << QString("统计后的路线%1 ").arg(++nRoute)
	//				 << "源 " << tmpGateMap->_mapNumber << "目标 " << tmpGateMap->_targetMapNumber
	//				 << "传送点 " << tmpGateMap->_x << "," << tmpGateMap->_y
	//				 << "到达点 " << tmpGateMap->_tx << "," << tmpGateMap->_ty << "成本" << tmpGateMap->_nCost;
	//		routeNode = routeNode->next;
	//	}
	//	auto tmpGateMap = routeNode->cur;
	//	qDebug() << QString("统计后的路线%1 ").arg(++nRoute)
	//			 << "源 " << tmpGateMap->_mapNumber << "目标 " << tmpGateMap->_targetMapNumber
	//			 << "传送点 " << tmpGateMap->_x << "," << tmpGateMap->_y
	//			 << "到达点 " << tmpGateMap->_tx << "," << tmpGateMap->_ty << "成本" << tmpGateMap->_nCost;
	//	qDebug() << "总成本：" << route->nTotalCost;
	//
	//}
	//返回最短路径
	//筛选真的能到的线路 以及最近的线路
	return bestRouteList;
}
bool ITObjectDataMgr::FindTargetNavigationEx(ITGameGateMapPtr cRoute, int tgtIndex, QList<int> traceRouteList)
{
	if (cRoute == nullptr)
		return false;
	cRoute->_navigationList.clear();
	//如果当前目标就是tgt，加入返回
	if (cRoute->_targetMapNumber == tgtIndex)
	{
		return true;
	}
	//当前目标到不了，看它的目的地 地图是否能到
	bool bAllow = false;
	auto pMapObj = m_numberForGameMap.value(cRoute->_targetMapNumber);
	if (!pMapObj)
		return false;
	traceRouteList.append(cRoute->_targetMapNumber);

	auto pTgtGateMapList = pMapObj->m_pGateMapList;
	for (auto tmpGateMap : pTgtGateMapList)
	{
		if (tmpGateMap->_mapNumber == cRoute->_targetMapNumber && tmpGateMap->_targetMapNumber == cRoute->_mapNumber)
			continue;
		if (traceRouteList.contains(tmpGateMap->_targetMapNumber))
			continue;
		bool bRet = FindTargetNavigationEx(tmpGateMap, tgtIndex, traceRouteList);
		if (bRet)
		{
			if (cRoute->_navigationList.contains(tmpGateMap))
			{
				cRoute->_navigationList.removeOne(tmpGateMap);
				return false;
			}
			cRoute->_navigationList.append(tmpGateMap);
			bAllow = true;
		}
	}
	return bAllow;
}
//加载离线地图 判断坐标是否可达
bool ITObjectDataMgr::IsReachableTarget(int mapIndex, int sx, int sy, int tx, int ty, int &ncost)
{
	if (sx == tx && sy == ty)
		return true;

	QImage mapImage = m_mapIndexForData.value(mapIndex);
	if (mapImage.width() <= 0 || mapImage.height() <= 0)
	{
		bool bLoad = LoadOffLineMapImageData(mapIndex, mapImage);
		if (!bLoad)
			return false;
		m_mapIndexForData.insert(mapIndex, mapImage);
	}

	double F = sqrt(2) - 1;
	auto findPath = g_pGameFun->CalculatePathEx(mapImage, sx, sy, tx, ty);
	if (findPath.size() > 0) //找到路径 计算长度  最优解
	{
		ncost = AStarUtil::pathLength(findPath);
		/*	int cost = 0;
		int nPathSize = findPath.size();
		for (int i = 0; i < (nPathSize - 1); ++i)
		{
			auto dx = abs(findPath.at(i).first - findPath.at(i + 1).first);
			auto dy = abs(findPath.at(i).second - findPath.at(i + 1).second);
			cost += (dx < dy) ? F * dx + dy : F * dy + dx;
		}
		ncost = cost;*/
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadOffLineMapData(int index)
{
	QString sOffLineMapPath = QApplication::applicationDirPath() + QString("//map//%1.dat").arg(index);
	// 读取地图信息
	FILE *pFile = nullptr;
	if (0 == fopen_s(&pFile, sOffLineMapPath.toLocal8Bit().data(), "rb"))
	{
		memset(&_mapHead, 0, sizeof(_mapHead));
		int len = sizeof(tMapHead);
		if (len == fread_s(&_mapHead, len, 1, len, pFile))
		{
			int dataLen = _mapHead.w * _mapHead.h;

			fseek(pFile, 0, SEEK_END);
			int fileLen = ftell(pFile);
			if (fileLen != dataLen * 6 + len)
			{
				return false;
			}

			fseek(pFile, len, SEEK_SET);

			unsigned short n = 0;
			int i = 0;
			while (2 == fread_s(&n, 2, 1, 2, pFile))
			{
				if (i < dataLen)
				{
					// 获取地表信息
					_vTileData.push_back((int)n);

					if (n > 0)
						_mapTileMap[n] = 0;
				}
				else if (i < dataLen * 2)
				{
					// 物件信息
					_vObjectData.push_back((int)n);

					if (n > 0)
						_mapObjectMap[n] = 0;
				}
				else
				{
					// 未知信息
					break;
				}
				++i;
			}
		}
	}
	if (pFile)
		fclose(pFile);

	return true;
}

bool ITObjectDataMgr::LoadOffLineMapImageData(int index, QImage &mapImage)
{
	QString sOffLineMapPath = QApplication::applicationDirPath() + QString("//map//%1.jpg").arg(index);
	if (QFile::exists(sOffLineMapPath) == false)
	{
		qDebug() << "跨图寻路：未找到指定地图数据！";
		return false;
	}
	memset(&_mapHead, 0, sizeof(_mapHead));
	mapImage.load(sOffLineMapPath);
	_mapHead.w = mapImage.width();
	_mapHead.h = mapImage.height();
	if (_mapHead.w <= 0 || _mapHead.h <= 0)
	{
		qDebug() << "跨图寻路：地图数据错误！";
		return false;
	}
	return true;
}
//判断当前图 到目标图 终点一样的传送
//后期如果有起点误差很大 但终点一样的 可以加起点判断
bool ITObjectDataMgr::CheckExistAlikeWrap(int mapIndex, int tgtMapIndex, int sx, int sy, int tx, int ty)
{
	auto pMapObj = m_numberForGameMap.value(mapIndex);
	if (pMapObj)
	{
		auto pGateMapList = pMapObj->m_pGateMapList;
		for (auto pGateMap : pGateMapList)
		{
			if (pGateMap->_targetMapNumber == tgtMapIndex && pGateMap->_tx == tx && pGateMap->_ty == ty)
			{
				return true;
			}
		}
	}
	return false;
}

ITObjectPtr ITObjectDataMgr::FindObject(quint64 objid)
{
	for (int i = 0; i < m_pObjectList.size(); ++i)
	{
		if (m_pObjectList[i]->getObjectID() == objid)
		{
			return m_pObjectList[i];
		}
	}
	for (int i = 0; i < m_pAddObjectList.size(); ++i)
	{
		if (m_pAddObjectList[i]->getObjectID() == objid)
		{
			return m_pAddObjectList[i];
		}
	}
	return nullptr;
}
ITObjectList ITObjectDataMgr::GetDstObjTypeList(int objType)
{
	ITObjectList pSelectObjs;
	for (auto pObj : m_pObjectList)
	{
		if (pObj->getObjectType() == objType)
		{
			pSelectObjs.append(pObj);
		}
	}
	for (auto pObj : m_pAddObjectList)
	{
		if (pObj->getObjectType() == objType)
		{
			pSelectObjs.append(pObj);
		}
	}
	//
	return pSelectObjs;
}
//通过当前实时获取的数据 更新数据库存储的物品信息
void ITObjectDataMgr::StoreServerItemData(GameItemPtr pItem)
{
	if (isLoading())
		return;

	if (!pItem || !pItem->exist)
		return;

	bool bForceUpdate = false;
	//通过code 先通过code判断吧，后续再说 或者名称判断
	ITGameItemPtr localItem = nullptr;
	{
		QMutexLocker locker(&m_storeItemMutex);
		localItem = m_codeForGameItem.value(pItem->id);
	}

	if (localItem)
	{
		//已鉴定 名称不一样 则更新本地
		if (!localItem->_bUpdate && bForceUpdate || (pItem->assessed && localItem->getObjectName() != pItem->name))
		{
			localItem->_itemType = pItem->type;
			//localItem->_itemPrice = nPrice;
			//localItem->_itemPile = nMaxPile;
			localItem->_itemLevel = pItem->level;
			//localItem->_sellMinCount = nSellLimitCount;
			localItem->setObjectName(pItem->name);
			QString sDesc = pItem->info;
			if (sDesc.startsWith("$4"))
			{
				sDesc = sDesc.mid(2);
			}
			localItem->setObjectDsec(sDesc);
			localItem->setObjectCode(pItem->id);
			localItem->_itemAttr = pItem->attr;
			localItem->_bUpdate = true;
			localItem->setEditStatus();

			qDebug() << localItem->getObjectName() << " " << pItem->type << " " << pItem->level << " " << pItem->name << " " << pItem->info << " " << pItem->id;
		}
		if (pItem->assessed == false && pItem->maybeName != localItem->getObjectName())
		{
			pItem->maybeName = localItem->getObjectName();
		}
	}
	else
	{
		//取服务器数据 没有再新建
		localItem = RpcSocketClient::getInstance().GetCGItemData(pItem->id);
		if (!localItem)
		{
			localItem = newOneObject(TObject_Item).dynamicCast<ITGameItem>();
			localItem->_itemType = pItem->type;
			//localItem->_itemPrice = nPrice;
			//localItem->_itemPile = nMaxPile;
			localItem->_itemLevel = pItem->level;
			//localItem->_sellMinCount = nSellLimitCount;
			localItem->setObjectName(pItem->name);
			QString sDesc = pItem->info;
			if (sDesc.startsWith("$4"))
			{
				sDesc = sDesc.mid(2);
			}
			localItem->_itemAttr = pItem->attr;
			localItem->setObjectDsec(sDesc);
			localItem->setObjectCode(pItem->id);
			//服务器有 也可能没有 让服务器判断，这里只本地缓存，下次不会进来了
			RpcSocketClient::getInstance().StoreCGItemData(pItem);
		}
		{

			QMutexLocker locker(&m_storeItemMutex);
			m_codeForGameItem.insert(pItem->id, localItem);
		}
		qDebug() << localItem->getObjectName() << " " << pItem->type << " " << pItem->level << " " << pItem->name << " " << pItem->info << " " << pItem->id;
	}
}
//通过当前实时获取的数据 更新数据库存储的地图信息
void ITObjectDataMgr::StoreServerMapData(const QString &mapName, int mapNumber)
{
	if (isLoading())
		return;
	bool bForceUpdate = false;
	//通过code 先通过code判断吧，后续再说 或者名称判断
	ITGameMapPtr localObj = nullptr;
	{
		QMutexLocker locker(&m_storeMapMutex);
		localObj = m_numberForGameMap.value(mapNumber);
	}

	if (localObj)
	{
		//已鉴定 名称不一样 则更新本地
		if (!localObj->_bUpdate && bForceUpdate || (localObj->getObjectName() != mapName))
		{
			localObj->_mapNumber = mapNumber;
			localObj->setObjectName(mapName);
			localObj->_bUpdate = true;
			localObj->setEditStatus();
			qDebug() << "更新数据库地图信息：" << localObj->getObjectName() << " " << mapNumber;
			RpcSocketClient::getInstance().StoreCGMapData(localObj);
		}
	}
	else
	{
		localObj = newOneObject(TObject_Map).dynamicCast<ITGameMap>();
		localObj->_mapNumber = mapNumber;
		localObj->setObjectName(mapName);
		localObj->_bUpdate = true;
		{
			QMutexLocker locker(&m_storeMapMutex);
			m_numberForGameMap.insert(mapNumber, localObj);
		}
		qDebug() << "新增数据库地图信息：" << localObj->getObjectName() << " " << mapNumber;
		RpcSocketClient::getInstance().StoreCGMapData(localObj);
	}
}

void ITObjectDataMgr::StoreServerPetData(CGA::cga_picbook_info_t &picBook)
{
	if (isLoading())
		return;
	auto petBook = m_numberForPet.value(picBook.index);
	if (!petBook)
		return;
	bool bEdit = false;
	if (petBook->_canCatch != picBook.can_catch)
	{
		petBook->_canCatch = picBook.can_catch;
		bEdit = true;
	}
	if (petBook->_imageId != picBook.image_id)
	{
		petBook->_imageId = picBook.image_id;
		bEdit = true;
	}
	if (petBook->_skillCount != picBook.skill_slots)
	{
		petBook->_skillCount = picBook.skill_slots;
		bEdit = true;
	}
	if (petBook->_elementEarth != picBook.element_earth)
	{
		petBook->_elementEarth = picBook.element_earth;
		bEdit = true;
	}
	if (petBook->_elementWater != picBook.element_water)
	{
		petBook->_elementWater = picBook.element_water;
		bEdit = true;
	}
	if (petBook->_elementFire != picBook.element_fire)
	{
		petBook->_elementFire = picBook.element_fire;
		bEdit = true;
	}
	if (petBook->_elementWind != picBook.element_wind)
	{
		petBook->_elementWind = picBook.element_wind;
		bEdit = true;
	}
	petBook->_cardType = picBook.card_type;
	if (bEdit)
		petBook->setEditStatus();
	//执行一次即可
}

void ITObjectDataMgr::loadDataBaseInfo(ITObjectDataMgr *pThis)
{
	if (!pThis)
		return;
	pThis->m_bIsLoading = true;
	pThis->LoadItems();
	pThis->LoadPets();
	pThis->LoadMaps();
	//pThis->LoadGateMaps();
	pThis->m_bIsLoading = false;
	emit pThis->signal_loadDataFini();
}

void ITObjectDataMgr::SaveDataThread(ITObjectDataMgr *pThis)
{
	if (!pThis)
		return;
	while (!g_pGameCtrl->GetExitGame())
	{
		if (pThis->isNeedSaveData())
			pThis->saveAllData();
		QThread::msleep(10000);
	}
}

bool ITObjectDataMgr::connectToDB(const QString &strDBType, const QString &strHostName, const QString &strDBName, const QString &strUser, const QString &strPwd)
{
	if (m_dbconn)
	{
		m_dbconn.clear();
		m_dbconn = NULL;
		m_strDBName.clear();
		m_strDBIp.clear();
		m_strDBType.clear();
		m_strDBUser.clear();
		m_strDBPwd.clear();
	}
	m_dbconn = ITDataBaseConnPtr(new ITDataBaseConn(strDBType));
	if (m_dbconn->openDataBase(strDBName, strHostName, strUser, strPwd))
	{
		m_strDBName = strDBName;
		m_strDBIp = strHostName;
		m_strDBType = strDBType;
		m_strDBUser = strUser;
		m_strDBPwd = strPwd;

		return true;
	}
	//! 连接失败  释放指针
	if (m_dbconn)
	{
		qDebug() << m_dbconn->getLastError();
		m_dbconn.clear();
		m_dbconn = NULL;
		m_strDBName.clear();
		m_strDBIp.clear();
		m_strDBType.clear();
		m_strDBUser.clear();
		m_strDBPwd.clear();
	}
	return false;
}
//! 函数名ITObjectDataMgr::getDBConnectState()
//! 作用：判断数据库是否已连接
//! 返回值：ture 已连接  false  未连接
bool ITObjectDataMgr::getDBConnectState()
{
	if (m_dbconn)
		m_bDBConnState = true;
	else
		m_bDBConnState = false;
	return m_bDBConnState;
}

bool ITObjectDataMgr::refreshConnectToDB()
{
	if (m_dbconn)
	{
		m_dbconn.clear();
		m_dbconn = NULL;
	}
	if (m_strDBType.isEmpty() || m_strDBUser.isEmpty() || m_strDBPwd.isEmpty() || m_strDBIp.isEmpty() || m_strDBName.isEmpty())
		return false;
	m_dbconn = ITDataBaseConnPtr(new ITDataBaseConn(m_strDBType));
	if (m_dbconn->openDataBase(m_strDBName, m_strDBIp, m_strDBUser, m_strDBPwd))
	{
		QtConcurrent::run(loadDataBaseInfo, this);
		return true;
	}
	//! 连接失败  释放指针
	if (m_dbconn)
	{
		m_dbconn.clear();
		m_dbconn = NULL;
	}
	return false;
}

ITObjectPtr ITObjectDataMgr::newOneObject(int nObjType, ITObjectPtr pOwn /*= NULL*/)
{
	ITObjectPtr pNewObj = nullptr;
	quint64 uNewID = getNewObjectID();
	qDebug() << uNewID;

	pNewObj = g_objectModuleReg.CreateNewObject(nObjType);
	if (!pNewObj)
		return nullptr;
	pNewObj->setObjectType(nObjType);
	pNewObj->setObjectID(uNewID);
	if (pOwn)
	{
		pNewObj->setObjectParent(pOwn);
		pOwn->addChildObj(pNewObj);
	}
	if (pNewObj)
	{
		pNewObj->setAddStatus();
		m_pAddObjectList.append(pNewObj);
	}
	return pNewObj;
}

ITObjectPtr ITObjectDataMgr::newOneObject(int nObjType, quint64 devID)
{
	ITObjectPtr pNewObj = nullptr;
	pNewObj = g_objectModuleReg.CreateNewObject(nObjType);
	if (!pNewObj)
		return nullptr;
	pNewObj->setObjectType(nObjType);
	pNewObj->setObjectID(devID);
	return pNewObj;
}

bool ITObjectDataMgr::deleteOneObject(ITObjectPtr pObj)
{
	if (pObj == NULL)
		return false;
	ITObjectPtr pParentObj = pObj->getObjectParent();
	if (pParentObj)
	{
		pParentObj->removeChildObj(pObj);
	}
	int nstatus = pObj->getStatus();
	if (nstatus == TStatus_Add)
	{
		pObj->setDelStatus();
		m_pAddObjectList.removeOne(pObj);
	}
	else
	{
		pObj->setDelStatus();
		m_pObjectList.removeOne(pObj);
		if (!m_pDelObjectList.contains(pObj))
			m_pDelObjectList.append(pObj);
	}
	return true;
}

quint64 ITObjectDataMgr::getNewObjectID()
{
	return ITObjectID::NewID();
}

bool ITObjectDataMgr::isNeedSaveData()
{
	if (m_pDelObjectList.size() > 0)
		return true;

	if (m_pAddObjectList.size() > 0)
		return true;
	for (int i = 0; i < m_pObjectList.size(); ++i)
	{
		if (m_pObjectList[i]->getStatus() != TStatus_Normal)
		{
			return true;
		}
	}
	return false;
}

bool ITObjectDataMgr::saveAllData(QString strInfo)
{
	if (!m_dbconn)
	{
		if (!reconnectDB())
		{
			strInfo += "重连数据库失败！\n";
			return false;
		}
	}
	if (!deleteDataFromDB())
	{
		strInfo += "删除数据错误！\n";
		strInfo += m_dbconn->getLastError();
		return false;
	}
	if (!insertNewDataToDB())
	{
		strInfo += "新增数据错误！\n";
		strInfo += m_dbconn->getLastError();
		return false;
	}
	if (!updateDataForDB())
	{
		strInfo += "更新数据错误！\n";
		strInfo += m_dbconn->getLastError();
		return false;
	}
	//	updateTotalVersion();
	return true;
}

bool ITObjectDataMgr::LoadItems()
{
	if (m_dbconn == NULL)
		return false;
	//auto pTableHash = GetTableObjectHash("item");
	m_codeForGameItem.clear();
	QString strsql = QString("SELECT * FROM item");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = (quint64)recordset->getIntValue("id");
			QString sName = recordset->getStrValue("name");
			int nCode = recordset->getIntValue("code");
			int nType = recordset->getIntValue("type");

			QString sDesc = recordset->getStrValue("desc");
			int nPrice = recordset->getIntValue("price");
			int nMaxPile = recordset->getIntValue("maxPile");
			int nLevel = recordset->getIntValue("level");
			int nSellLimitCount = recordset->getIntValue("sellLimitCount");
			ITGameItemPtr pItem = newOneObject(TObject_Item, nID).dynamicCast<ITGameItem>();
			if (pItem)
			{
				pItem->_itemType = nType;
				pItem->_itemPrice = nPrice;
				pItem->_itemPile = nMaxPile;
				pItem->_itemLevel = nLevel;
				pItem->_sellMinCount = nSellLimitCount;
				pItem->setObjectName(sName);
				pItem->setObjectDsec(sDesc);
				pItem->setObjectCode(nCode);
				m_pObjectList.append(pItem);
				m_codeForGameItem.insert(nCode, pItem);
				//	pTableHash.insert(nCode,pItem);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadPets()
{
	if (m_dbconn == NULL)
		return false;

	QString strsql = QString("SELECT * FROM pet");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = (quint64)recordset->getIntValue("id");
			QString sName = recordset->getStrValue("name");
			int nCode = recordset->getIntValue("code");
			int raceType = recordset->getIntValue("raceType");

			QString sDesc = recordset->getStrValue("desc");
			int petNumber = recordset->getIntValue("number");
			QString raceText = recordset->getStrValue("race");
			int abilityRate = recordset->getIntValue("abilityRate");
			int vigor = recordset->getIntValue("growVigor");
			int strength = recordset->getIntValue("growStrength");
			int defense = recordset->getIntValue("growDefense");
			int agility = recordset->getIntValue("growAgility");
			int magic = recordset->getIntValue("growMagic");
			int canCatch = recordset->getIntValue("canCatch");
			int cardType = recordset->getIntValue("cardType");
			int imageId = recordset->getIntValue("imageId");
			int skillCount = recordset->getIntValue("skillCount");
			int elementEarth = recordset->getIntValue("elementEarth");
			int elementWater = recordset->getIntValue("elementWater");
			int elementFire = recordset->getIntValue("elementFire");
			int elementWind = recordset->getIntValue("elementWind");
			ITGamePetPtr pObj = newOneObject(TObject_Pet, nID).dynamicCast<ITGamePet>();
			if (pObj)
			{
				pObj->_petNumber = petNumber;
				pObj->_petRaceType = raceType;
				pObj->_petRace = raceText;
				pObj->_petBaseBp = abilityRate;
				pObj->_growVigor = vigor;
				pObj->_growStrength = strength;
				pObj->_growDefense = defense;
				pObj->_growAgility = agility;
				pObj->_growMagic = magic;
				pObj->_canCatch = canCatch;
				pObj->_cardType = cardType;
				pObj->_imageId = imageId;
				pObj->_skillCount = skillCount;
				pObj->_elementEarth = elementEarth;
				pObj->_elementWater = elementWater;
				pObj->_elementFire = elementFire;
				pObj->_elementWind = elementWind;
				pObj->setObjectName(sName);
				pObj->setObjectDsec(sDesc);
				pObj->setObjectCode(nCode);
				m_pObjectList.append(pObj);
				m_numberForPet.insert(petNumber, pObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadGateMaps()
{
	if (m_dbconn == NULL)
		return false;

	QString strsql = QString("SELECT * FROM gateMap");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = (quint64)recordset->getIntValue("id");
			QString sName = recordset->getStrValue("name");
			/*int nCode = recordset->getIntValue("code");
			int nType = recordset->getIntValue("type");*/

			QString sDesc = recordset->getStrValue("desc");
			QString sMapName = recordset->getStrValue("name");
			QString stgtMapName = recordset->getStrValue("tgtMapName");
			int mapNumber = recordset->getIntValue("mapNumber");
			int tgtMapNumber = recordset->getIntValue("tgtMapNumber");
			int warpType = recordset->getIntValue("type");
			QString npcSelect = recordset->getStrValue("npc");

			int nX = recordset->getIntValue("x");
			int nY = recordset->getIntValue("y");
			int tx = recordset->getIntValue("tx");
			int ty = recordset->getIntValue("ty");
			if (CheckExistAlikeWrap(mapNumber, tgtMapNumber, nX, nY, tx, ty))
			{ //类似坐标 不重复加入了，比如里堡南门出去 3个坐标点都可以出去，这边取其中一个
				continue;
			}
			auto tgtMapList = m_warpHash.value(mapNumber);
			tgtMapList.append(tgtMapNumber);
			m_warpHash.insert(mapNumber, tgtMapList);

			ITGameGateMapPtr pObj = newOneObject(TObject_GateMap, nID).dynamicCast<ITGameGateMap>();
			if (pObj)
			{
				pObj->_x = nX;
				pObj->_y = nY;
				pObj->_tx = tx;
				pObj->_ty = ty;
				pObj->_mapName = sMapName;
				pObj->_targetMapName = stgtMapName;
				pObj->_mapNumber = mapNumber;
				pObj->_targetMapNumber = tgtMapNumber;
				pObj->_warpType = warpType;
				pObj->_npcSelect = npcSelect.split(";");
				pObj->setObjectName(sName);
				pObj->setObjectDsec(sDesc);
				//pItem->setObjectCode(nCode);
				m_pObjectList.append(pObj);
				auto pMapObj = m_numberForGameMap.value(mapNumber);
				if (pMapObj)
				{
					pMapObj->m_pGateMapList.append(pObj);
					if (sMapName.isEmpty() || sMapName != pMapObj->getObjectName())
					{
						pObj->_mapName = pMapObj->getObjectName();
						pObj->setEditStatus();
					}
				}

				auto pTgtMapObj = m_numberForGameMap.value(tgtMapNumber);
				if (pTgtMapObj)
				{
					if (stgtMapName.isEmpty() || stgtMapName != pTgtMapObj->getObjectName())
					{
						pObj->_targetMapName = pTgtMapObj->getObjectName();
						pObj->setEditStatus();
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadMaps()
{
	if (m_dbconn == NULL)
		return false;
	m_numberForGameMap.clear();
	QString strsql = QString("SELECT * FROM map");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = (quint64)recordset->getIntValue("id");
			QString sName = recordset->getStrValue("name");
			QString sDesc = recordset->getStrValue("desc");
			int number = recordset->getIntValue("number");
			int isOften = recordset->getIntValue("often");
			ITGameMapPtr pObj = newOneObject(TObject_Map, nID).dynamicCast<ITGameMap>();
			if (pObj)
			{
				pObj->_mapNumber = number;
				pObj->_oftenMap = isOften;
				pObj->setObjectName(sName);
				pObj->setObjectDsec(sDesc);
				m_pObjectList.append(pObj);
				m_numberForGameMap.insert(number, pObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::isLoading()
{
	return m_bIsLoading;
}

void ITObjectDataMgr::doLoadDBInfoOver()
{
}

void ITObjectDataMgr::on_attachGame_sucess()
{
	CGA::cga_picbooks_info_t picbooks;
	if (g_CGAInterface->GetPicBooksInfo(picbooks))
	{
		for (auto tPicBook : picbooks)
		{
			StoreServerPetData(tPicBook);
		}
	}
}

void ITObjectDataMgr::OnNotifyGetItemsInfo(GameItemList items)
{
}

void ITObjectDataMgr::OnNotifyGetPetsInfo(GamePetList pets)
{
}

void ITObjectDataMgr::updateLogStateChange()
{
	if (!m_client)
		return;
	const QString content = QDateTime::currentDateTime().toString() + QLatin1String(": State Change") + QString::number(m_client->state()) + QLatin1Char('\n');
	qDebug() << content;
}

void ITObjectDataMgr::brokerDisconnected()
{
	qDebug() << "brokerDisconnected";
}

void ITObjectDataMgr::OnRecvMessage(const QByteArray &message, const QMqttTopicName &topic)
{
	const QString content = QDateTime::currentDateTime().toString() + QLatin1String(" Received Topic: ") + topic.name() + QLatin1String(" Message: ") + message + QLatin1Char('\n');

	//qDebug() << content;
	emit signal_mqttMsg(topic.name(), message);
}

void ITObjectDataMgr::OnCheckConnectMqtt()
{
	if (!m_client)
		return;
	if (m_client->state() == QMqttClient::Disconnected)
	{
		m_client->connectToHost();
	}
	else if (m_client->state() == QMqttClient::Connected)
	{
		for (auto subTopic : m_customSubscribeList)
		{
			auto sub = m_client->subscribe(subTopic);
			m_client->connect(sub, &QMqttSubscription::stateChanged, this, &ITObjectDataMgr::OnSubscribeState);
			/*m_client->connect(sub, &QMqttSubscription::stateChanged, [this](QMqttSubscription::SubscriptionState s)
					{
						qInfo() << "Subscription state:" << s;
						if (s == QMqttSubscription::Unsubscribed)
							m_client->disconnectFromHost();
					});*/
		}
	//	m_retrySubscribes.clear();
	}
}
//连接成功信号 订阅
void ITObjectDataMgr::OnMqttConnected()
{
	if (!m_client)
		return;
	for (auto subTopic : m_customSubscribeList)
	{
		auto sub = m_client->subscribe(subTopic);
		m_client->connect(sub, &QMqttSubscription::stateChanged, this, &ITObjectDataMgr::OnSubscribeState);
	}
	/*m_retrySubscribes.clear();
	auto sub = m_client->subscribe(QString("qtmqtt/topic1"));
	m_client -> connect(sub, &QMqttSubscription::stateChanged, this, &ITObjectDataMgr::OnSubscribeState);*/
	//m_client->connect(
	//		sub, &QMqttSubscription::stateChanged, [this](QMqttSubscription::SubscriptionState s) 
	//		{
	//			qInfo() << "Subscription state:" << s;
	//			if (s == QMqttSubscription::Unsubscribed)
	//				m_client->disconnectFromHost();
	//		});
}

void ITObjectDataMgr::OnSubscribeState(QMqttSubscription::SubscriptionState s)
{
	qInfo() << "Subscription state:" << s;
	if (s == QMqttSubscription::Unsubscribed)
		m_client->disconnectFromHost();	
}

void ITObjectDataMgr::on_publishMqttMsg(const QString &topic, const QString &msg)
{
	PublishOneTopic(topic,msg);
}

bool ITObjectDataMgr::pingToDestination(const QString &strIp)
{
	int exitCode;
	QString strPing = "ping " + strIp + " -n 1";
	QProcess p(0);
	p.start(strPing);
	p.waitForStarted();
	p.waitForFinished();
	QString str = QString(p.readAllStandardOutput());

	if (str.contains("无法访问目标主机") || str.contains("请求超时") || str.contains("传输中过期"))
		return false;
	else if (str.contains("字节=") || str.contains("bytes="))
		return true;
	else
		return true;
}

bool ITObjectDataMgr::reconnectDB() //重连数据库
{
	if (m_dbconn)
	{
		m_dbconn.clear();
		m_dbconn = NULL;
	}
	if (m_strDBType.isEmpty() || m_strDBUser.isEmpty() || m_strDBPwd.isEmpty() || m_strDBIp.isEmpty() || m_strDBName.isEmpty())
		return false;
	m_dbconn = ITDataBaseConnPtr(new ITDataBaseConn(m_strDBType));
	if (m_dbconn->openDataBase(m_strDBName, m_strDBIp, m_strDBUser, m_strDBPwd))
	{
		return true;
	}
	//! 连接失败  释放指针
	if (m_dbconn)
	{
		m_dbconn.clear();
		m_dbconn = NULL;
	}
	return false;
}

bool ITObjectDataMgr::insertNewDataToDB()
{
	ITObjectList tempSuccessList; ///把成功的删除，不重复增加，修改和删除不做处理了
	bool bSucc = true;
	for (int i = 0; i < m_pAddObjectList.size(); i++)
	{
		//recordLog(m_addlist[i]);
		if (!insertOneDeviceToDB(m_pAddObjectList[i]))
		{
			bSucc = false;
			break;
		}
		tempSuccessList.append(m_pAddObjectList[i]);

		m_pAddObjectList[i]->setNomalStatus();
		m_pObjectList.append(m_pAddObjectList[i]);
	}
	if (!bSucc)
	{ ///删除成功写入数据库的项
		foreach (auto pDev, tempSuccessList)
		{
			if (m_pAddObjectList.contains(pDev))
			{
				m_pAddObjectList.removeOne(pDev);
			}
		}
		return false;
	}
	m_pAddObjectList.clear();
	return true;
}

bool ITObjectDataMgr::updateDataForDB()
{
	for (int i = 0; i < m_pObjectList.size(); ++i)
	{
		if (m_pObjectList[i]->getStatus() != TStatus_Normal)
		{
			if (!updateOneDeviceToDB(m_pObjectList[i]))
				return false;
			m_pObjectList[i]->setNomalStatus();
		}
	}
	return true;
}
bool ITObjectDataMgr::deleteDataFromDB()
{
	bool bRet = true;
	for (int i = 0; i < m_pDelObjectList.size(); i++)
	{
		if (!deleteOneDeviceFromDB(m_pDelObjectList[i]))
		{
			bRet = false;
			return bRet;
		}
	}
	m_pDelObjectList.clear();
	return bRet;
}

bool ITObjectDataMgr::deleteOneDeviceFromDB(ITObjectPtr pObj)
{
	bool bret = false;
	if (pObj == NULL)
		return false;
	int objType = pObj->getObjectType();
	QString strSql;
	if (objType == TObject_Item)
	{
		strSql = QString("DELETE FROM item WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Pet)
	{
		strSql = QString("DELETE FROM pet WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Map)
	{
		strSql = QString("DELETE FROM map WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_GateMap)
	{
		strSql = QString("DELETE FROM gateMap WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	qDebug() << strSql;
	return bret;
}

bool ITObjectDataMgr::insertOneDeviceToDB(ITObjectPtr pObj)
{
	if (pObj == NULL)
		return false;
	bool bret = false;
	int objType = pObj->getObjectType();
	QString strSql;
	if (objType == TObject_Item)
	{
		auto tmpObj = pObj.dynamicCast<ITGameItem>();
		strSql = QString("INSERT INTO item(name,code,type,desc,price,maxPile,level,sellLimitCount,attribute) VALUES('%2',%3,%4,'%5',%6,%7,%8,%9,'%10')")
						 .arg(tmpObj->getObjectName())
						 .arg(tmpObj->getObjectCode())
						 .arg(tmpObj->_itemType)
						 .arg(tmpObj->getObjectDesc())
						 .arg(tmpObj->_itemPrice)
						 .arg(tmpObj->_itemPile)
						 .arg(tmpObj->_itemLevel)
						 .arg(tmpObj->_sellMinCount)
						 .arg(tmpObj->_itemAttr);
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Pet)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGamePet>();
		strSql = QString("INSERT INTO pet(number,name,race,raceType,abilityRate,growVigor,growStrength,growDefense,growAgility,growMagic,code,canCatch,cardType,imageId,skillCount,elementEarth,elementWater,elementFire,elementWind) VALUES(%2,'%3','%4',%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20)")
						 .arg(tmpObj->_petNumber)
						 .arg(tmpObj->getObjectName())
						 .arg(tmpObj->_petRace)
						 .arg(tmpObj->_petRaceType)
						 .arg(tmpObj->_petBaseBp)
						 .arg(tmpObj->_growVigor)
						 .arg(tmpObj->_growStrength)
						 .arg(tmpObj->_growDefense)
						 .arg(tmpObj->_growAgility)
						 .arg(tmpObj->_growMagic)
						 .arg(tmpObj->getObjectCode())
						 .arg(tmpObj->_canCatch)
						 .arg(tmpObj->_cardType)
						 .arg(tmpObj->_imageId)
						 .arg(tmpObj->_skillCount)
						 .arg(tmpObj->_elementEarth)
						 .arg(tmpObj->_elementWater)
						 .arg(tmpObj->_elementFire)
						 .arg(tmpObj->_elementWind);
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Map)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameMap>();
		strSql = QString("INSERT INTO map(name,number,desc,often) VALUES('%1',%2,'%3',%4)")
						 .arg(tmpObj->getObjectName())
						 .arg(tmpObj->_mapNumber)
						 .arg(tmpObj->getObjectDesc())
						 .arg(tmpObj->_oftenMap);
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_GateMap)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameGateMap>();
		strSql = QString("INSERT INTO gateMap(name,mapNumber,x,y,tgtMapName,tgtMapNumber,desc,tx,ty,type,npc) VALUES('%2',%3,%4,%5,'%6',%7,'%8',%9,%10,%11,'%12')")
						 .arg(tmpObj->_mapName)
						 .arg(tmpObj->_mapNumber)
						 .arg(tmpObj->_x)
						 .arg(tmpObj->_y)
						 .arg(tmpObj->_targetMapName)
						 .arg(tmpObj->_targetMapNumber)
						 .arg(tmpObj->getObjectDesc())
						 .arg(tmpObj->_tx)
						 .arg(tmpObj->_ty)
						 .arg(tmpObj->_warpType)
						 .arg(tmpObj->_npcSelect.join(";"));
		bret = m_dbconn->execSql(strSql);
	}
	qDebug() << strSql;

	return bret;
}

bool ITObjectDataMgr::updateOneDeviceToDB(ITObjectPtr pObj)
{
	if (pObj == NULL)
		return false;
	bool bret = false;
	int objType = pObj->getObjectType();
	QString strSql;
	if (objType == TObject_Item)
	{
		auto tmpObj = pObj.dynamicCast<ITGameItem>();
		strSql = QString("UPDATE item SET name='%1',code=%2,type=%3,desc='%4',price=%5,maxPile=%6,level=%7,sellLimitCount=%8,attribute='%9' WHERE id=%10")
						 .arg(tmpObj->getObjectName())
						 .arg(tmpObj->getObjectCode())
						 .arg(tmpObj->_itemType)
						 .arg(tmpObj->getObjectDesc())
						 .arg(tmpObj->_itemPrice)
						 .arg(tmpObj->_itemPile)
						 .arg(tmpObj->_itemLevel)
						 .arg(tmpObj->_sellMinCount)
						 .arg(tmpObj->_itemAttr)
						 .arg((int)tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Pet)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGamePet>();
		strSql = QString("UPDATE pet SET name='%1',number=%2,raceType=%3,race='%4',abilityRate=%5,growVigor=%6,growStrength=%7,growDefense=%8,growAgility=%9,growMagic=%10,code=%11,canCatch=%12,cardType=%13,imageId=%14,skillCount=%15,elementEarth=%16,elementWater=%17,elementFire=%18,elementWind=%19 WHERE id=%20")
						 .arg(tmpObj->getObjectName())
						 .arg(tmpObj->_petNumber)
						 .arg(tmpObj->_petRaceType)
						 .arg(tmpObj->_petRace)
						 .arg(tmpObj->_petBaseBp)
						 .arg(tmpObj->_growVigor)
						 .arg(tmpObj->_growStrength)
						 .arg(tmpObj->_growDefense)
						 .arg(tmpObj->_growAgility)
						 .arg(tmpObj->_growMagic)
						 .arg(tmpObj->getObjectCode())
						 .arg(tmpObj->_canCatch)
						 .arg(tmpObj->_cardType)
						 .arg(tmpObj->_imageId)
						 .arg(tmpObj->_skillCount)
						 .arg(tmpObj->_elementEarth)
						 .arg(tmpObj->_elementWater)
						 .arg(tmpObj->_elementFire)
						 .arg(tmpObj->_elementWind)
						 .arg((int)tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Map)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameMap>();
		strSql = QString("UPDATE map set name='%1',number=%2,desc='%3',often=%4 WHERE id=%5")
						 .arg(tmpObj->getObjectName())
						 .arg(tmpObj->_mapNumber)
						 .arg(tmpObj->getObjectDesc())
						 .arg(tmpObj->_oftenMap)
						 .arg((int)tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_GateMap)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameGateMap>();
		strSql = QString("UPDATE gateMap set name='%1',mapNumber=%2,x=%3,y=%4,tgtMapName='%5',tgtMapNumber=%6,desc='%7',tx=%9,ty=%10,type=%11,npc='%12' WHERE id=%13")
						 .arg(tmpObj->_mapName)
						 .arg(tmpObj->_mapNumber)
						 .arg(tmpObj->_x)
						 .arg(tmpObj->_y)
						 .arg(tmpObj->_targetMapName)
						 .arg(tmpObj->_targetMapNumber)
						 .arg(tmpObj->getObjectDesc())
						 .arg(tmpObj->_tx)
						 .arg(tmpObj->_ty)
						 .arg(tmpObj->_warpType)
						 .arg(tmpObj->_npcSelect.join(";"))
						 .arg((int)tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	qDebug() << strSql;
	return bret;
}
