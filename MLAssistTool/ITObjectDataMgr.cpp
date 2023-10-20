#include "ITObjectDataMgr.h"
#include "./Astar/AStarUtil.h"
#include "ITObjectID.h"
//#include "StdAfx.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QTextCodec>
#include <QtConcurrent>
#include "RpcSocket.h"
#include <QApplication>
#ifdef Q_OS_UNIX
#include "unistd.h"
#endif
ObjectModuleRegisty::~ObjectModuleRegisty(void)
{
	for (auto it = moduleInfoList.begin(); it != moduleInfoList.end(); ++it)
	{
		if (it.value())
			delete it.value();
	}
	moduleInfoList.clear();
}
ObjectModuleRegisty& ObjectModuleRegisty::GetInstance()
{
	static ObjectModuleRegisty instance;
	return instance;
}

void ObjectModuleRegisty::RegisterModuleFactory(int nObjType, ObjectModuleFactory* pFactory, QString sIconPath)
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
	qRegisterMetaType<ITGameSkillPtr>("ITGameSkillPtr");
	qRegisterMetaType<ITAccountIdentityPtr>("ITAccountIdentityPtr");
	qRegisterMetaType<ITCharcterServerPtr>("ITCharcterServerPtr");
	qRegisterMetaType<ITGameCharacterPtr>("ITGameCharacterPtr");

	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_GateMap, NEW_MODULE_FACTORY(ITGameGateMap));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_PetBook, NEW_MODULE_FACTORY(ITCGPetPictorialBook));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Map, NEW_MODULE_FACTORY(ITGameMap));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_AccountIdentity, NEW_MODULE_FACTORY(ITAccountIdentity));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Account, NEW_MODULE_FACTORY(ITAccount));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_AccountGid, NEW_MODULE_FACTORY(ITAccountGid));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Character, NEW_MODULE_FACTORY(ITGameCharacter));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CharacterLeft, NEW_MODULE_FACTORY(ITGameCharacter));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CharacterRight, NEW_MODULE_FACTORY(ITGameCharacter));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_BaseData, NEW_MODULE_FACTORY(ITGameBaseData));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_AttributeData, NEW_MODULE_FACTORY(ITGameAttributeData));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_RoleRunConfig, NEW_MODULE_FACTORY(ITAccountGidRoleRunConfig));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Pet, NEW_MODULE_FACTORY(ITGamePet));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CGPet, NEW_MODULE_FACTORY(ITGamePet));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_BankPet, NEW_MODULE_FACTORY(ITGamePet));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CharPet, NEW_MODULE_FACTORY(ITGamePet));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CGItem, NEW_MODULE_FACTORY(ITGameItem));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Item, NEW_MODULE_FACTORY(ITGameItem));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CharItem, NEW_MODULE_FACTORY(ITGameItem));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CharBankItem, NEW_MODULE_FACTORY(ITGameItem));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CGSkill, NEW_MODULE_FACTORY(ITGameSkill));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CharSkill, NEW_MODULE_FACTORY(ITGameSkill));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CharPetSkill, NEW_MODULE_FACTORY(ITGameSkill));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_CharServer, NEW_MODULE_FACTORY(ITCharcterServer));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_ServerType, NEW_MODULE_FACTORY(ITGameServerType));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_ServerType_ItemSales, NEW_MODULE_FACTORY(ITGameServerType));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_ServerType_ItemSales_Telecom, NEW_MODULE_FACTORY(ITGameServerType));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_ServerType_ItemSales_Netcom, NEW_MODULE_FACTORY(ITGameServerType));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_ServerType_PUK2, NEW_MODULE_FACTORY(ITGameServerType));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_ServerType_CGOld, NEW_MODULE_FACTORY(ITGameServerType));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_ServerType_CGOld_AriGem, NEW_MODULE_FACTORY(ITGameServerType));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_ServerType_CGOld_Taurus, NEW_MODULE_FACTORY(ITGameServerType));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Host, NEW_MODULE_FACTORY(ITHost));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Host_PC, NEW_MODULE_FACTORY(ITHost));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Host_Notebook, NEW_MODULE_FACTORY(ITHost));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Host_Server, NEW_MODULE_FACTORY(ITHost));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Host_Pad, NEW_MODULE_FACTORY(ITHost));
	//m_serverTypeForObjType.insert(13, TObject_ServerType);
	//m_serverTypeForObjType.insert(13, TObject_ServerType_ItemSales);
	m_serverTypeForObjType.insert(13, TObject_ServerType_ItemSales_Telecom);
	m_serverTypeForObjType.insert(14, TObject_ServerType_ItemSales_Netcom);
	m_serverTypeForObjType.insert(1, TObject_ServerType_PUK2);
	//m_serverTypeForObjType.insert(23, TObject_ServerType_CGOld);
	m_serverTypeForObjType.insert(23, TObject_ServerType_CGOld_AriGem);
	m_serverTypeForObjType.insert(24, TObject_ServerType_CGOld_Taurus);	
	
	//m_serverTypeForObjName.insert(13, TObject_ServerType);
	//m_serverTypeForObjName.insert(13, TObject_ServerType_ItemSales);
	m_serverTypeForObjName.insert(13, "道具电信");
	m_serverTypeForObjName.insert(14, "道具网通");
	m_serverTypeForObjName.insert(1, "时长");
	//m_serverTypeForObjName.insert(23, TObject_ServerType_CGOld);
	m_serverTypeForObjName.insert(23, "怀旧牧羊双子");
	m_serverTypeForObjName.insert(24, "怀旧金牛");

	m_sPrestigeMap.insert("恶人", -3);
	m_sPrestigeMap.insert("受忌讳的人", -2);
	m_sPrestigeMap.insert("受挫折的人", -1);
	m_sPrestigeMap.insert("无名的旅人", 0);
	m_sPrestigeMap.insert("路旁的落叶", 1);
	m_sPrestigeMap.insert("水面上的小草", 2);
	m_sPrestigeMap.insert("呢喃的歌声", 3);
	m_sPrestigeMap.insert("地上的月影", 4);
	m_sPrestigeMap.insert("奔跑的春风", 5);
	m_sPrestigeMap.insert("苍之风云", 6);
	m_sPrestigeMap.insert("摇曳的金星", 7);
	m_sPrestigeMap.insert("欢喜的慈雨", 8);
	m_sPrestigeMap.insert("蕴含的太阳", 9);
	m_sPrestigeMap.insert("敬畏的寂静", 10);
	m_sPrestigeMap.insert("无尽星空", 11);
	m_sPrestigeMap.insert("迈步前进者", 1);
	m_sPrestigeMap.insert("追求技巧的人", 2);
	m_sPrestigeMap.insert("刻于新月之铭", 3);
	m_sPrestigeMap.insert("掌上的明珠", 4);
	m_sPrestigeMap.insert("敬虔的技巧", 5);
	m_sPrestigeMap.insert("踏入神的领域", 6);
	m_sPrestigeMap.insert("贤者", 7);
	m_sPrestigeMap.insert("神匠", 8);
	m_sPrestigeMap.insert("摘星的技巧", 9);
	m_sPrestigeMap.insert("万物创造者", 10);
	m_sPrestigeMap.insert("持石之贤者", 11);
	connect(this, SIGNAL(signal_loadDataFini()), this, SLOT(doLoadDBInfoOver())); //这个是加载数据库完成
	init();
}

ITObjectDataMgr::~ITObjectDataMgr(void)
{
}
bool ITObjectDataMgr::Fini()
{
	return true;
}
//没有加类型判断 需要的话 加一个
QString ITObjectDataMgr::FindItemCodeName(int nCode)
{
	for (auto it=m_pObjectList.begin();it!=m_pObjectList.end();++it)
	{
		if (it.value()->getObjectCode() == nCode)
		{
			return it.value()->getObjectName();
		}
	}
	for (auto it = m_pAddObjectList.begin(); it != m_pAddObjectList.end(); ++it)
	{
		if (it.value()->getObjectCode() == nCode)
		{
			return it.value()->getObjectName();
		}
	}
	return "";
}

ITObjectDataMgr& ITObjectDataMgr::getInstance(void)
{
	static ITObjectDataMgr mitsObjicedata;
	return mitsObjicedata;
}

void ITObjectDataMgr::checkOnlineThread(ITObjectDataMgr *pThis)
{
	if (!pThis)
		return;
	while (!pThis->m_bExit)
	{
		auto tmpObjs = pThis->GetAllCharacterList();
		pThis->m_gameRoleCount = tmpObjs.size();
		int onlineCount = 0;
		int offlineCount = 0;
		for (auto it : tmpObjs)
		{
			ITGameCharacterPtr pCharacter = it.dynamicCast<ITGameCharacter>();
			if (pCharacter->_connectState == 1 && pCharacter->_lastUploadTime.elapsed() > 30000) //30秒 状态更新为离线
			{
				//QMutexLocker locker(&it.value()->_mutex);
				pCharacter->_connectState = 0;
				offlineCount += 1;
				QString sOneID = pCharacter->getObjectName() + QString::number(pCharacter->_big_line);
				if (pThis->m_onlineAccountRoles.contains(sOneID))
				{
					pThis->m_onlineAccountRoles.removeOne(sOneID);
				}
			}
			else if (pCharacter->_connectState == 1)
			{
				onlineCount += 1;
			}
		}
		pThis->m_offlineCount = offlineCount;
		pThis->m_onlineCount = onlineCount;
		QThread::msleep(10000);		//10秒检测一次
	}
}

ITObjectList ITObjectDataMgr::GetAllCharacterList()
{
	ITObjectList pAllList;
	for (auto it = m_serverTypeForObj.begin(); it != m_serverTypeForObj.end();++it)
	{
		pAllList.append(it.value()->_nameForObj.values());
	}
	return pAllList;
}

bool ITObjectDataMgr::init()
{
	QString iniPath = QCoreApplication::applicationDirPath() + "/config.ini";
	QSettings ini(iniPath, QSettings::IniFormat);
	QString  strProvider = ini.value("dbconn/ConnType", "SQLSERVER").toString();
	QString	strHostName = ini.value("dbconn/DataSource", "127.0.0.1").toString();
	QString strDBName = ini.value("dbconn/Catalog", "cg").toString();
	QString strUserName = ini.value("dbconn/UserID", "sa").toString();
	QString strPassWord = ini.value("dbconn/password", "123").toString();
	int nPort = ini.value("dbconn/Port", "3306").toInt();
	m_bForceUpdate = ini.value("dbconn/forceUpdateItem", false).toBool();

	//QString sDBPath = QApplication::applicationDirPath() + "//db//cg.db";
	bool bRet = false;
	//if (connectToDB("SQLITE", "CG", sDBPath, "admin", "123456"))
	if (connectToDB(strProvider,strHostName,strDBName, strUserName, strPassWord, nPort))
	{
		qDebug() << "打开数据库成功！";
		//		QtConcurrent::run(loadDataBaseInfo, this);
		loadDataBaseInfo(this);
		bRet = true;
	}
	else
	{
		if (m_dbconn)
			qDebug() << m_dbconn->getLastError();
		bRet = false;
	}
	QtConcurrent::run(SaveDataThread, this);
	QtConcurrent::run(checkOnlineThread, this);
	return bRet;
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

ITObjectList ITObjectDataMgr::GetDstObjTypeList(int objType, quint64 nVal)
{
	ITObjectList pSelectObjs;

	QHash<quint64, ITObjectPtr> tmpObjList;
	
	{
		QMutexLocker locker(&m_objMutex);
		tmpObjList = m_pObjectList;
	}	
	for (auto it= tmpObjList.begin();it!= tmpObjList.end();++it)
	{
		if ((it.value()->getObjectType() & nVal) == objType)
		{
			pSelectObjs.append(it.value());
		}
	}
	{
		QMutexLocker locker(&m_objMutex);
		tmpObjList = m_pAddObjectList;
	}
	for (auto it = tmpObjList.begin(); it != tmpObjList.end(); ++it)
	{
		if ((it.value()->getObjectType() & nVal) == objType)
		{
			pSelectObjs.append(it.value());
		}
	}
	//
	return pSelectObjs;
}

ITObjectPtr ITObjectDataMgr::FindObject(quint64 objid)
{
	if (m_pObjectList.contains(objid))
	{
		return m_pObjectList.value(objid);
	}
	if (m_pAddObjectList.contains(objid))
	{
		return m_pAddObjectList.value(objid);
	}
	return nullptr;
}


void ITObjectDataMgr::loadDataBaseInfo(ITObjectDataMgr* pThis)
{
	if (!pThis)
		return;
	//QMutexLocker locker(&pThis->m_objMutex);
	//基础数据
	pThis->LoadItems();
	pThis->LoadPets();
	pThis->LoadMaps();
	pThis->LoadGateMaps();
	//角色数据
	pThis->LoadIdentification();
	pThis->LoadAccount();
	pThis->LoadGameServerType();
	pThis->LoadAccountGid();
	pThis->LoadGameCharacter();
	pThis->LoadGidPets();
	pThis->LoadBaseData();
	pThis->LoadAttributeData();
	pThis->LoadGidItems();
	pThis->LoadGidSkills();
	emit pThis->signal_loadDataFini();
}

void ITObjectDataMgr::SaveDataThread(ITObjectDataMgr* pThis)
{
	if (!pThis)
		return;
	while (!pThis->m_bExit)
	{
		if (pThis->isNeedSaveData())
			pThis->saveAllData();
		QThread::msleep(10000);
	}
}

bool ITObjectDataMgr::connectToDB(const QString& strDBType, const QString& strHostName, const QString& strDBName, \
	const QString& strUser, const QString& strPwd, int nport)
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
	if (m_dbconn->openDataBase(strDBName, strHostName, strUser, strPwd, nport))
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
	//qDebug() << uNewID;

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
		QMutexLocker locker(&m_objMutex);
		m_pAddObjectList.insert(uNewID,pNewObj);
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

	int nObjType = pObj->getObjectType();
	ITObjectPtr pOwnObj = pObj->getObjectParent();
	if (GETDEVCLASS(nObjType) == TObject_Character)
	{
		if (pOwnObj)
		{
			int pOwnType = pOwnObj->getObjectType();
			if (pOwnType == TObject_AccountGid)
			{
				pOwnObj->removeChildObj(pObj);	
				ITGameServerTypePtr pServerTypeObj = pOwnObj->getObjectParent().dynamicCast<ITGameServerType>();
				if (pServerTypeObj && GETDEVCLASS(pServerTypeObj->getObjectType()) == TObject_ServerType)
				{
					pServerTypeObj->removeObject(pObj);	
				}
			}
		}
	}
	else if (nObjType == TObject_AccountGid)
	{
		ITGameServerTypePtr pServerTypeObj = pOwnObj.dynamicCast<ITGameServerType>();
		if (pServerTypeObj && GETDEVCLASS(pServerTypeObj->getObjectType()) == TObject_ServerType)
		{
			pServerTypeObj->removeObject(pObj);
		}
	}
	else if (GETDEVCLASS(nObjType) == TObject_ServerType)
	{
		ITGameServerTypePtr pServerTypeObj = pObj.dynamicCast<ITGameServerType>();
		m_serverTypeForObj.remove(pServerTypeObj->_server_type);
	}
	else
	{
		if (pOwnObj)
		{
			pOwnObj->removeChildObj(pObj);
		}
	}	
	QMutexLocker locker(&m_objMutex);
	m_pObjectList.remove(pObj->getObjectID());//不管状态，在这里调一下，有就删，没有不影响
	int nstatus = pObj->getStatus();
	if (nstatus == TStatus_Add)
	{
		pObj->setDelStatus();			// 还没写入数据库 移除释放即可
		m_pAddObjectList.remove(pObj->getObjectID());	//智能指针，其他地方没存的话，会自动释放
	}else
	{
		pObj->setDelStatus();		
		if (!m_pDelObjectList.contains(pObj->getObjectID()))
			m_pDelObjectList.insert(pObj->getObjectID(), pObj);
	}
	return true;
}

quint64 ITObjectDataMgr::getNewObjectID()
{
	//return ITObjectID::NewID();
	return ITObjectID::NewID32();
}

void ITObjectDataMgr::gameCharacterAddToServerType(ITGameCharacterPtr pCharacter, ITGameServerTypePtr pServerType)
{
	//建立大区和游戏角色关系
	pServerType->addChildObj(pCharacter);
	pServerType->_nameForObj.insert(pCharacter->getObjectName(), pCharacter);
	//建立
	auto pGidObj = pServerType->getGidObjFromGidName(pCharacter->_gid).dynamicCast<ITAccountGid>();
	if (pGidObj == nullptr)
	{
		//不需要再次查找 没有就创建
		pGidObj = newOneObject(TObject_AccountGid).dynamicCast<ITAccountGid>();
		pGidObj->_userGid = pCharacter->_gid;
		pGidObj->setObjectName(pCharacter->_gid);
		pGidObj->_serverTypeID = pServerType->getObjectID();
		pGidObj->setObjectParent(pServerType);
		pServerType->addChildObj(pGidObj);
		pServerType->_gidForObj.insert(pCharacter->_gid, pGidObj);
	}
	pCharacter->setObjectParent(pGidObj);
	pGidObj->addChildObj(pCharacter);
}

bool ITObjectDataMgr::isNeedSaveData()
{
	QHash<quint64, ITObjectPtr> tmpObjList;	
	{
		QMutexLocker locker(&m_objMutex);
		if (m_pDelObjectList.size() > 0)
			return true;
		if (m_pAddObjectList.size() > 0)
			return true;
		
	}
	{
		QMutexLocker locker(&m_objMutex);
		tmpObjList = m_pObjectList;
	/*	for (auto it = m_pObjectList.begin(); it != m_pObjectList.end(); ++it)
		{
			if (it.value()->getStatus() != TStatus_Normal)
			{
				return true;
			}
		}*/
	}
	for (auto it = tmpObjList.begin(); it != tmpObjList.end(); ++it)
	{
		if (it.value()->getStatus() != TStatus_Normal)
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
		//	return false;
	}
	if (!insertNewDataToDB())
	{
		strInfo += "新增数据错误！\n";
		strInfo += m_dbconn->getLastError();
		//	return false;
	}
	if (!updateDataForDB())
	{
		strInfo += "更新数据错误！\n";
		strInfo += m_dbconn->getLastError();
		//	return false;
	}
	//	updateTotalVersion();
	return true;
}

bool ITObjectDataMgr::LoadIdentification()
{
	if (m_dbconn == NULL)
		return false;
	QString strsql = QString("SELECT * FROM identification");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			QString sName = recordset->getStrValue("name");
			QString identityCard = recordset->getStrValue("identityCard");
			int sex = recordset->getIntValue("sex");
			QString sDesc = recordset->getStrValue("desc");	//详细信息 从item表拿
			ITAccountIdentityPtr pObj = newOneObject(TObject_AccountIdentity, nID).dynamicCast<ITAccountIdentity>();
			if (pObj)
			{
				pObj->_identity = identityCard;
				pObj->_sex = sex;
				pObj->setObjectName(sName);
				pObj->setObjectDsec(sDesc);

				pObj->setObjectID(nID);
				m_pObjectList.insert(nID, pObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadAccount()
{
	if (m_dbconn == NULL)
		return false;
	QString strsql = QString("SELECT * FROM account");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			QString user = recordset->getStrValue("user");
			QString passwd = recordset->getStrValue("passwd");
			QString identification = recordset->getStrValue("identification");
			QString sDesc = recordset->getStrValue("desc");	//详细信息 从item表拿
			ITAccountPtr pObj = newOneObject(TObject_Account, nID).dynamicCast<ITAccount>();
			if (pObj)
			{
				pObj->_identity = identification;
				pObj->_userName = user;
				pObj->_passwd = passwd;
				pObj->setObjectName(user);
				pObj->setObjectDsec(sDesc);
				pObj->setObjectID(nID);
				m_pObjectList.insert(nID, pObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadAccountGid()
{
	if (m_dbconn == NULL)
		return false;
	QString strsql = QString("SELECT * FROM gid");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			QString gid = recordset->getStrValue("gid");
			quint64 aid = recordset->getUInt64Value("aid");
			quint64 serverTypeID = recordset->getUInt64Value("server_type_id");
			QString sDesc = recordset->getStrValue("desc");	//详细信息 从item表拿
			if (gid.isEmpty())
			{
					//加入删除列表，后面自动删除数据库 这里未加入objectList和addObjectList 所以调deleteOneObject无意义
				if (!m_pDelObjectList.contains(nID))
				{
					ITAccountGidPtr pObj = newOneObject(TObject_AccountGid, nID).dynamicCast<ITAccountGid>();
					m_pDelObjectList.insert(pObj->getObjectID(), pObj);
				}				
				continue;
				
			}			
			auto pAccountObj = FindObject(aid).dynamicCast<ITAccount>();
			auto pServerTypeObj = FindObject(serverTypeID).dynamicCast<ITGameServerType>();
			ITAccountGidPtr pObj = newOneObject(TObject_AccountGid, nID).dynamicCast<ITAccountGid>();
			if (pObj)
			{
				pObj->_userGid = gid;
				pObj->_serverTypeID = serverTypeID;
				pObj->setObjectName(gid);
				pObj->setObjectDsec(sDesc);
				pObj->setObjectID(nID);
				if (pServerTypeObj == nullptr)
				{
					//加入删除列表，后面自动删除数据库 这里未加入objectList和addObjectList 所以调deleteOneObject无意义
					if (!m_pDelObjectList.contains(pObj->getObjectID()))
						m_pDelObjectList.insert(pObj->getObjectID(), pObj);
					continue;
				}
				if (pAccountObj)
				{
					pAccountObj->addChildObj(pObj);
					pObj->setObjectParent(pAccountObj);
				}


				if (pServerTypeObj->_gidForObj.contains(gid))
				{
					deleteOneObject(pServerTypeObj->_gidForObj.value(gid));//释放老的
				}
				pObj->setObjectParent(pServerTypeObj);
				pServerTypeObj->addChildObj(pObj);
				pServerTypeObj->_gidForObj.insert(gid, pObj);

				m_pObjectList.insert(nID, pObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadHost()
{
	if (m_dbconn == NULL)
		return false;
	QString strsql = QString("SELECT * FROM host_data");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			QString pc_name = recordset->getStrValue("pc_name");
			QString pc_user_name = recordset->getStrValue("pc_user_name");
			QString pc_mac_addr = recordset->getStrValue("pc_mac_addr");
			QString pc_local_ip = recordset->getStrValue("pc_local_ip");
			int order = recordset->getIntValue("order_num");		
			int type = recordset->getIntValue("type");		
			QString sDesc = recordset->getStrValue("remark");	//详细信息 从item表拿
			if (pc_mac_addr.isEmpty())
			{
				//加入删除列表，后面自动删除数据库 这里未加入objectList和addObjectList 所以调deleteOneObject无意义
				if (!m_pDelObjectList.contains(nID))
				{
					ITHostPtr pObj = newOneObject(type, nID).dynamicCast<ITHost>();
					m_pDelObjectList.insert(pObj->getObjectID(), pObj);
				}
				continue;

			}
			ITHostPtr pObj = newOneObject(type, nID).dynamicCast<ITHost>();
			if (pObj)
			{
				pObj->_sPcName = pc_name;
				pObj->_sPcUserName = pc_user_name;
				pObj->_sPcMacAddr = pc_mac_addr;
				pObj->_sPcLocalIp = pc_local_ip;
				pObj->setObjectName(pc_name);
				pObj->setObjectDsec(sDesc);
				pObj->setObjectID(nID);				
				pObj->setObjectOrder(order);
				m_pObjectList.insert(nID, pObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadGameServerType()
{
	if (m_dbconn == NULL)
		return false;
	QString strsql = QString("SELECT * FROM game_server_type");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			int bigLine = recordset->getIntValue("big_line");
			QString name = recordset->getStrValue("name");
			if (bigLine <= 0)
			{
				if (!m_pDelObjectList.contains(nID))
				{
					ITGameServerTypePtr pObj = newOneObject(TObject_ServerType_ItemSales, nID).dynamicCast<ITGameServerType>();
					m_pDelObjectList.insert(pObj->getObjectID(), pObj);
				}
				continue;
			}
			int nServerType;
			if (m_serverTypeForObjType.contains(bigLine))
			{
				nServerType = m_serverTypeForObjType.value(bigLine);
			}
			else
			{
				nServerType = TObject_ServerType_ItemSales;
			}
		
			ITGameServerTypePtr pObj = newOneObject(nServerType, nID).dynamicCast<ITGameServerType>();
			if (pObj)
			{
				pObj->_server_type = bigLine;
				pObj->setObjectName(name);			
				pObj->setObjectID(nID);
				if (m_serverTypeForObj.contains(bigLine))
				{
					auto oldServerTypeObj = m_serverTypeForObj.value(bigLine);
					if (oldServerTypeObj != nullptr)
					{						
						deleteOneObject(oldServerTypeObj);						
					}
				}
				m_pObjectList.insert(nID, pObj);
				m_serverTypeForObj.insert(bigLine, pObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadGameCharacter()
{
	if (m_dbconn == NULL)
		return false;
	m_idForAccountRole.clear();
	QString strsql = QString("SELECT * FROM character");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			QString sGid = recordset->getStrValue("gid");
			QString sName = recordset->getStrValue("name");
			int objType = recordset->getIntValue("role_type");
			int bigLine = recordset->getIntValue("big_line");
			if (sGid.isEmpty() || sName.isEmpty())
			{
				if (!m_pDelObjectList.contains(nID))
				{
					ITGameCharacterPtr pObj = newOneObject(TObject_Character, nID).dynamicCast<ITGameCharacter>();
					m_pDelObjectList.insert(pObj->getObjectID(), pObj);
				}
			
				continue;
			}
			if (objType == 0 )
			{
				objType = TObject_Character;
			}
			if (bigLine==0)
			{
				bigLine = 13;	//默认道具电信
			}
			ITGameServerTypePtr pServerType = m_serverTypeForObj.value(bigLine);
			if (pServerType == nullptr)
			{
				QMutexLocker locker(&m_rpcGidMutex);
				int nServerType;
				if (m_serverTypeForObjType.contains(bigLine))
				{
					nServerType = m_serverTypeForObjType.value(bigLine);
				}else
				{					
					nServerType = TObject_ServerType_ItemSales_Telecom;
					bigLine = 13;
				}
				pServerType = newOneObject(nServerType).dynamicCast<ITGameServerType>();
				pServerType->setObjectName(m_serverTypeForObjName.value(bigLine));
				pServerType->_server_type = bigLine;
				m_serverTypeForObj.insert(bigLine, pServerType);
			}
			ITGameCharacterPtr pCharacter = newOneObject(objType, nID).dynamicCast<ITGameCharacter>();
			if (pCharacter)
			{			
				pCharacter->setObjectName(sName);						
				pCharacter->_gid = sGid;			
				pCharacter->_type = recordset->getIntValue("type");	
				pCharacter->_sex = recordset->getIntValue("sex");
				pCharacter->_gold = recordset->getIntValue("gold");
				pCharacter->_bankgold = recordset->getIntValue("bankgold");			
				pCharacter->_souls = recordset->getIntValue("souls");
				pCharacter->_score = recordset->getIntValue("score");
				pCharacter->_job = recordset->getStrValue("job");
				pCharacter->_useTitle = recordset->getIntValue("useTitle");
				pCharacter->_titles = recordset->getStrValue("titles").split("|");				
				pCharacter->_value_charisma = recordset->getIntValue("value_charisma");				
				pCharacter->_map_name = recordset->getStrValue("map_name");		
				pCharacter->_map_number = recordset->getIntValue("map_number");		
				pCharacter->_nickName = recordset->getStrValue("nick_name");		
				pCharacter->_big_line = bigLine;
				pCharacter->_avatar_id = recordset->getIntValue("avatar_id");
				pCharacter->_unitid = recordset->getIntValue("unitid");
				pCharacter->_petid = recordset->getIntValue("petid");
				pCharacter->_petriding = recordset->getIntValue("petriding");
				pCharacter->_direction = recordset->getIntValue("direction");
				pCharacter->_punchclock = recordset->getIntValue("punchclock");
				pCharacter->_usingpunchclock = recordset->getIntValue("usingpunchclock");
				pCharacter->_x = recordset->getIntValue("x");
				pCharacter->_y = recordset->getIntValue("y");
				pCharacter->_battle_position = recordset->getIntValue("battle_position");
				pCharacter->_server_line = recordset->getIntValue("line");
				pCharacter->_lastUpdateDateTime = QDateTime::fromSecsSinceEpoch(recordset->getInt64Value("last_time"));
				pCharacter->_connectState = recordset->getIntValue("conn_state");		
				pCharacter->_lastUploadTime.restart();

				//重复 删除老的 插入新的  有bug先用名称顶一下
				auto oldChara = pServerType->getRoleObjFromRoleName(sName);
				if (oldChara != nullptr)
				{
					deleteOneObject(oldChara);							
				}
				m_pObjectList.insert(nID, pCharacter);
				gameCharacterAddToServerType(pCharacter, pServerType);				

			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadBaseData()
{
	if (m_dbconn == NULL)
		return false;
	m_idForAccountRole.clear();
	QString strsql = QString("SELECT * FROM base_data");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			quint64 char_id = recordset->getUInt64Value("char_id");	
			ITObjectPtr pChar = FindObject(char_id);
			if (pChar == nullptr)
			{				
				if (!m_pDelObjectList.contains(nID))
				{
					ITGameBaseDataPtr pBaseData = newOneObject(TObject_BaseData, nID).dynamicCast<ITGameBaseData>();
					//加入删除列表，后面自动删除数据库 这里未加入objectList和addObjectList 所以调deleteOneObject无意义
					m_pDelObjectList.insert(pBaseData->getObjectID(), pBaseData);
				}
				continue;
			}
			ITGameBaseDataPtr pBaseData = newOneObject(TObject_BaseData, nID).dynamicCast<ITGameBaseData>();
			if (pBaseData)
			{
				if (GETDEVCLASS(pChar->getObjectType()) == TObject_Character)
				{
					qSharedPointerDynamicCast<ITGameCharacter>(pChar)->_baseData = pBaseData;
					pBaseData->setObjectParent(pChar);
				}
				else if (GETDEVCLASS(pChar->getObjectType()) == TObject_CGPet)
				{
					qSharedPointerDynamicCast<ITGamePet>(pChar)->_baseData = pBaseData;
					pBaseData->setObjectParent(pChar);
				}else
				{					
					if (!m_pDelObjectList.contains(pBaseData->getObjectID()))
						m_pDelObjectList.insert(pBaseData->getObjectID(), pBaseData);
					continue;
				}
				pBaseData->_level = recordset->getIntValue("level");
				pBaseData->_imageid = recordset->getIntValue("imageid");
				pBaseData->_xp = recordset->getIntValue("xp");
				pBaseData->_maxxp = recordset->getIntValue("maxxp");
				pBaseData->_hp = recordset->getIntValue("hp");
				pBaseData->_maxhp = recordset->getIntValue("maxhp");
				pBaseData->_mp = recordset->getIntValue("mp");
				pBaseData->_maxmp = recordset->getIntValue("maxmp");				
				pBaseData->_skillslots = recordset->getIntValue("skillslots");						
				m_pObjectList.insert(nID, pBaseData);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadAttributeData()
{
	if (m_dbconn == NULL)
		return false;
	m_idForAccountRole.clear();
	QString strsql = QString("SELECT * FROM attribute_data");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			quint64 char_id = recordset->getUInt64Value("char_id");
			ITObjectPtr pChar = FindObject(char_id);
			if (pChar == nullptr)
			{
				if (!m_pDelObjectList.contains(nID))
				{
					ITGameAttributeDataPtr pBaseData = newOneObject(TObject_AttributeData, nID).dynamicCast<ITGameAttributeData>();
					if (!m_pDelObjectList.contains(pBaseData->getObjectID()))
						m_pDelObjectList.insert(pBaseData->getObjectID(), pBaseData);
				}
				continue;
			}
			ITGameAttributeDataPtr pDBObj = newOneObject(TObject_AttributeData, nID).dynamicCast<ITGameAttributeData>();
			if (pDBObj)
			{
				if (GETDEVCLASS(pChar->getObjectType()) == TObject_Character)
				{
					qSharedPointerDynamicCast<ITGameCharacter>(pChar)->_attrData = pDBObj;
					pDBObj->setObjectParent(pChar);
				}
				else if (GETDEVCLASS(pChar->getObjectType()) == TObject_CGPet)
				{
					qSharedPointerDynamicCast<ITGamePet>(pChar)->_attrData = pDBObj;
					pDBObj->setObjectParent(pChar);
				}else
				{
					if (!m_pDelObjectList.contains(pDBObj->getObjectID()))
						m_pDelObjectList.insert(pDBObj->getObjectID(), pDBObj);
				}
				pDBObj->_manu_endurance = recordset->getIntValue("manu_endurance");
				pDBObj->_manu_skillful = recordset->getIntValue("manu_skillful");
				pDBObj->_manu_intelligence = recordset->getIntValue("manu_intelligence");
				pDBObj->_points_endurance = recordset->getIntValue("points_endurance");
				pDBObj->_points_strength = recordset->getIntValue("points_strength");
				pDBObj->_points_defense = recordset->getIntValue("points_defense");
				pDBObj->_points_agility = recordset->getIntValue("points_agility");
				pDBObj->_points_magical = recordset->getIntValue("points_magical");
				pDBObj->_value_attack = recordset->getIntValue("value_attack");
				pDBObj->_value_defensive = recordset->getIntValue("value_defensive");
				pDBObj->_value_agility = recordset->getIntValue("value_agility");
				pDBObj->_value_spirit = recordset->getIntValue("value_spirit");
				pDBObj->_value_recovery = recordset->getIntValue("value_recovery");
				pDBObj->_resist_poison = recordset->getIntValue("resist_poison");
				pDBObj->_resist_sleep = recordset->getIntValue("resist_sleep");
				pDBObj->_resist_medusa = recordset->getIntValue("resist_medusa");
				pDBObj->_resist_drunk = recordset->getIntValue("resist_drunk");
				pDBObj->_resist_chaos = recordset->getIntValue("resist_chaos");
				pDBObj->_resist_forget = recordset->getIntValue("resist_forget");
				pDBObj->_fix_critical = recordset->getIntValue("fix_critical");
				pDBObj->_fix_strikeback = recordset->getIntValue("fix_strikeback");
				pDBObj->_fix_accurancy = recordset->getIntValue("fix_accurancy");
				pDBObj->_fix_dodge = recordset->getIntValue("fix_dodge");
				pDBObj->_element_earth = recordset->getIntValue("element_earth");
				pDBObj->_element_water = recordset->getIntValue("element_water");
				pDBObj->_element_fire = recordset->getIntValue("element_fire");
				pDBObj->_element_wind = recordset->getIntValue("element_wind");
				pDBObj->_points_remain = recordset->getIntValue("points_remain");	
				m_pObjectList.insert(nID, pDBObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadGidItems()
{
	if (m_dbconn == NULL)
		return false;
	QString strsql = QString("SELECT * FROM char_item");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			quint64 nChara_dbid = recordset->getUInt64Value("character_id");//角色id
			QString sName = recordset->getStrValue("name");
			int count = recordset->getIntValue("count");
			int isbank = recordset->getIntValue("isbank");
			int item_id = recordset->getIntValue("item_id");	//详细信息 从item表拿
			int item_pos = recordset->getIntValue("item_pos");	//详细信息 从item表拿
			int devType = isbank ? TObject_CharItem : TObject_CharBankItem;
			ITGameItemPtr pItem = newOneObject(devType, nID).dynamicCast<ITGameItem>();
			if (pItem)
			{
				pItem->_itemCount = count;
				pItem->_itemPos = item_pos;
				pItem->setObjectName(sName);
				//pItem->setObjectDsec(sDesc);
				pItem->setObjectCode(item_id);
				pItem->setObjectID(nID);
				pItem->_bExist = true;
				ITGameCharacterPtr pGidRole = FindObject(nChara_dbid).dynamicCast<ITGameCharacter>();
				if (!pGidRole)//查找道具相关的角色 没找到 则删除当前道具对象
				{
					if (!m_pDelObjectList.contains(nID))
						m_pDelObjectList.insert(nID, pItem);
				}
				else
				{
					pItem->setObjectParent(pGidRole);
					pGidRole->addChildObj(pItem);
					m_pObjectList.insert(nID, pItem);
					//重复 删除老的映射 插入新的
					auto oldItem = pGidRole->_itemPosForPtr.value(item_pos);
					if (oldItem != nullptr)
					{							
						if (!m_pDelObjectList.contains(oldItem->getObjectID()))
						{
							deleteOneObject(oldItem);							
						}						
					}		
					pGidRole->_itemPosForPtr.insert(item_pos, pItem);//替换新的
				}	
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadGidPets()
{
	if (m_dbconn == NULL)
		return false;

	QString strsql = QString("SELECT * FROM char_pet");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			quint64 character_id = recordset->getUInt64Value("character_id");
			QString sName = recordset->getStrValue("name");
			QString realname = recordset->getStrValue("realname");
			int objtype = recordset->getIntValue("type");

			ITGamePetPtr pObj = newOneObject(objtype, nID).dynamicCast<ITGamePet>();
			if (pObj)
			{

				pObj->_character_id = character_id;
				pObj->_realName = realname;
				pObj->_state = recordset->getIntValue("state");
				pObj->_loyality = recordset->getIntValue("loyality");			
				pObj->_race = recordset->getIntValue("race");			
				pObj->_grade = recordset->getIntValue("grade");			
				pObj->_lossMinGrade = recordset->getIntValue("lossMinGrade");			
				pObj->_lossMaxGrade = recordset->getIntValue("lossMaxGrade");				
				pObj->_pos = recordset->getIntValue("pos");
				pObj->_bExist = true;
				pObj->setObjectName(sName);
				//	pObj->setObjectDsec(sDesc);
				//	pObj->setObjectCode(nCode);
				pObj->setObjectID(nID);
				ITGameCharacterPtr pGidRole = FindObject(character_id).dynamicCast<ITGameCharacter>();
				if (!pGidRole)
				{
					if (!m_pDelObjectList.contains(nID))
						m_pDelObjectList.insert(nID, pObj);
				}
				else
				{						
					pObj->setObjectParent(pGidRole);
					pGidRole->addChildObj(pObj);
					m_pObjectList.insert(nID, pObj);
					//重复 删除老的 插入新的
					auto pOldObj = pGidRole->_petPosForPet.value(pObj->_pos);
					if (pOldObj != nullptr)
					{
						if (!m_pDelObjectList.contains(pOldObj->getObjectID()))
						{
							deleteOneObject(pOldObj);			
						}						
					}
					pGidRole->_petPosForPet.insert(pObj->_pos, pObj);					
				}
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadGidSkills()
{
	if (m_dbconn == NULL)
		return false;
	QString strsql = QString("SELECT * FROM char_skill");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			quint64 nChara_dbid = recordset->getUInt64Value("linkid");//角色id
			QString sName = recordset->getStrValue("name");
			QString sInfo = recordset->getStrValue("info");
			int skillid = recordset->getIntValue("skillid");
			int objtype = recordset->getIntValue("type");
			int index = recordset->getIntValue("s_index");
			int level = recordset->getIntValue("level");
			int maxlevel = recordset->getIntValue("maxlevel");
			int xp = recordset->getIntValue("xp");
			int maxxp = recordset->getIntValue("maxxp");
			int cost = recordset->getIntValue("cost");
			int available = recordset->getIntValue("available");
			int flags = recordset->getIntValue("flags");
			ITGameSkillPtr pObj = newOneObject(objtype, nID).dynamicCast<ITGameSkill>();
			if (pObj)
			{				
				pObj->_info = sInfo;
				pObj->_index = index;
				pObj->_level = level;
				pObj->_maxLevel = maxlevel;
				pObj->_xp = xp;
				pObj->_maxxp = maxxp;
				pObj->_cost = cost;
				pObj->_available = available;
				pObj->_flags = flags;
				pObj->_id = skillid;
				pObj->setObjectName(sName);
				//pItem->setObjectDsec(sDesc);
				pObj->setObjectCode(skillid);
				pObj->setObjectID(nID);
				pObj->_bExist = true;
				ITGameCharacterPtr pGidRole = FindObject(nChara_dbid).dynamicCast<ITGameCharacter>();
				if (!pGidRole)
				{
					if (!m_pDelObjectList.contains(nID))
						m_pDelObjectList.insert(nID, pObj);
				}
				else
				{
					pObj->setObjectParent(pGidRole);
					pGidRole->addChildObj(pObj);
					m_pObjectList.insert(nID, pObj);
					//重复 删除老的 插入新的
					auto pOldObj = pGidRole->_skillPosForSkill.value(index);
					if (pOldObj != nullptr)
					{
						if (!m_pDelObjectList.contains(pOldObj->getObjectID()))
						{
							deleteOneObject(pOldObj);
							/*m_pDelObjectList.insert(pOldObj->getObjectID(), pOldObj);
							m_pObjectList.remove(pOldObj->getObjectID());*/
						}
					}
					pGidRole->_skillPosForSkill.insert(index, pObj);
				}				
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadItems()
{
	if (m_dbconn == NULL)
		return false;
	m_codeForGameItem.clear();
	QString strsql = QString("SELECT * FROM item");
	auto recordset = m_dbconn->execQuerySql(strsql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			quint64 nID = recordset->getUInt64Value("id");
			QString sName = recordset->getStrValue("name");
			int nCode = recordset->getIntValue("code");
			int nType = recordset->getIntValue("type");

			QString sDesc = recordset->getStrValue("desc");
			QString attr = recordset->getStrValue("attribute");
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
				pItem->_itemAttr = attr;
				m_pObjectList.insert(nID, pItem);
				m_codeForGameItem.insert(nCode, pItem);
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
			quint64 nID = recordset->getUInt64Value("id");
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
			ITCGPetPictorialBookPtr pObj = newOneObject(TObject_PetBook, nID).dynamicCast<ITCGPetPictorialBook>();
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
				m_pObjectList.insert(nID, pObj);
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
			quint64 nID = recordset->getUInt64Value("id");
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
				pObj->setObjectCode(mapNumber);
				m_pObjectList.insert(nID, pObj);
				m_numberForGateMap.insert(mapNumber, pObj);
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
			quint64 nID = recordset->getUInt64Value("id");
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
				m_pObjectList.insert(nID, pObj);
				m_numberForGameMap.insert(number, pObj);
			}
		}
		return true;
	}
	return false;
}

void ITObjectDataMgr::doLoadDBInfoOver()
{
}


bool ITObjectDataMgr::pingToDestination(const QString& strIp)
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
	QHash<quint64, ITObjectPtr> tmpObjList;
	{
		QMutexLocker locker(&m_objMutex);
		tmpObjList = m_pAddObjectList;
	}
	for (auto it = tmpObjList.begin();it!= tmpObjList.end();++it)
	{
		if (!insertOneDeviceToDB(it.value()))
		{
			bSucc = false;
			break;
		}
		tempSuccessList.append(it.value());
		it.value()->setNomalStatus();		
	}
	QMutexLocker locker(&m_objMutex);
	///删除成功写入数据库的项
	foreach(auto pDev, tempSuccessList)
	{
		m_pObjectList.insert(pDev->getObjectID(), pDev);
		if (m_pAddObjectList.contains(pDev->getObjectID()))
		{
			m_pAddObjectList.remove(pDev->getObjectID());
		}
	}
	if (!bSucc)
	{		
		return false;
	}	
	return true;
}

bool ITObjectDataMgr::updateDataForDB()
{
	QHash<quint64, ITObjectPtr> tmpObjList;
	{
		QMutexLocker locker(&m_objMutex);
		tmpObjList = m_pObjectList;
	}
	for (auto it = tmpObjList.begin(); it != tmpObjList.end(); ++it)
	{
		if (it.value()->getStatus() != TStatus_Normal)
		{
			if (!updateOneDeviceToDB(it.value()))
				return false;
			it.value()->setNomalStatus();
		}
	}
	return true;
}
bool ITObjectDataMgr::deleteDataFromDB()
{
	bool bRet = true;
	QHash<quint64, ITObjectPtr> tmpObjList;
	{
		QMutexLocker locker(&m_objMutex);
		tmpObjList = m_pDelObjectList;
	}
	for (auto it = tmpObjList.begin(); it != tmpObjList.end(); ++it)
	{
		if (!deleteOneDeviceFromDB(it.value()))
		{
			bRet = false;
			//return bRet;
		}
	}
	QMutexLocker locker(&m_objMutex);
	for (auto it = tmpObjList.begin(); it != tmpObjList.end(); ++it)
	{
		m_pDelObjectList.remove(it.key());
	}	
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
		strSql = QString("DELETE FROM item WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_PetBook)
	{
		strSql = QString("DELETE FROM pet WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Map)
	{
		strSql = QString("DELETE FROM map WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_GateMap)
	{
		strSql = QString("DELETE FROM gateMap WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_Character)
	{
		strSql = QString("DELETE FROM character WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Account)
	{
		strSql = QString("DELETE FROM account WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AccountGid)
	{
		strSql = QString("DELETE FROM gid WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AccountIdentity)
	{
		strSql = QString("DELETE FROM identification WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CGSkill)
	{
		strSql = QString("DELETE FROM char_skill WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CharItem)
	{
		strSql = QString("DELETE FROM char_item WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CGPet)
	{
		strSql = QString("DELETE FROM char_pet WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_ServerType)
	{
		strSql = QString("DELETE FROM game_server_type WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_BaseData)
	{
		strSql = QString("DELETE FROM base_data WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AttributeData)
	{
		strSql = QString("DELETE FROM attribute_data WHERE id=%1").arg(pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_Host)
	{
		strSql = QString("DELETE FROM  host_data WHERE id=%1").arg(pObj->getObjectID());			
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
	else if (objType == TObject_PetBook)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITCGPetPictorialBook>();
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
			.arg(tmpObj->_elementWind)
			;
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
	else if (objType == TObject_AccountIdentity)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITAccountIdentity>();
		strSql = QString("INSERT INTO identification(id,name,identityCard,sex,desc) VALUES(%1,'%2','%3',%4,'%5')")
			.arg(tmpObj->getObjectID()).arg(tmpObj->getObjectName())
			.arg(tmpObj->_identity)
			.arg(tmpObj->_sex)
			.arg(tmpObj->getObjectDesc());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Account)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITAccount>();
		strSql = QString("INSERT INTO account(id,user,passwd,identification,desc) VALUES(%1,'%2','%3','%4','%5')")
			.arg(tmpObj->getObjectID()).arg(tmpObj->_userName)
			.arg(tmpObj->_passwd)
			.arg(tmpObj->_identity)
			.arg(tmpObj->getObjectDesc());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AccountGid)
	{
		QString szOwnCode;
		quint64 aid = 0;
		auto tmpObj = pObj.dynamicCast<ITAccountGid>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			aid = pOwn->getObjectID();
		strSql = QString("INSERT INTO gid(id,gid,aid,server_type_id,desc) VALUES(%1,'%2',%3,%4,'%5')")
			.arg(tmpObj->getObjectID()).arg(tmpObj->_userGid)
			.arg(aid).arg(tmpObj->_serverTypeID)
			.arg(tmpObj->getObjectDesc());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CharItem)
	{
		QString szOwnCode;
		quint64 char_id = 0;
		auto tmpObj = pObj.dynamicCast<ITGameItem>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			char_id = pOwn->getObjectID();
		strSql = QString("INSERT INTO char_item(id,character_id,name,count,isbank,item_id,item_pos) VALUES(%1,%2,'%3',%4,%5,%6,%7)")
			.arg(tmpObj->getObjectID()).arg(char_id)
			.arg(tmpObj->getObjectName())
			.arg(tmpObj->_itemCount)
			.arg(tmpObj->getObjectType() == TObject_CharItem ? 0 : 1)
			.arg(tmpObj->getObjectCode())
			.arg(tmpObj->_itemPos);
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CGSkill)
	{
		QString szOwnCode;
		quint64 char_id = 0;
		auto tmpObj = pObj.dynamicCast<ITGameSkill>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			char_id = pOwn->getObjectID();
		strSql = QString("INSERT INTO char_skill(id,linkid,type,name,info,s_index,level,maxlevel,xp,maxxp,cost,available,flags)"
			" VALUES(%1,%2,%3,'%4','%5',%6,%7,%8,%9,%11,%12,%13,%14)")
			.arg(tmpObj->getObjectID())
			.arg(char_id)
			.arg(tmpObj->getObjectType())
			.arg(tmpObj->getObjectName())
			.arg(tmpObj->_info)
			.arg(tmpObj->_index)
			.arg(tmpObj->_level)
			.arg(tmpObj->_maxLevel)
			.arg(tmpObj->_xp)
			.arg(tmpObj->_maxxp)
			.arg(tmpObj->_cost)
			.arg(tmpObj->_available)
			.arg(tmpObj->_flags)
			;
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CGPet)
	{
		QString szOwnCode;
		quint64 char_id = 0;
		auto tmpObj = pObj.dynamicCast<ITGamePet>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			char_id = pOwn->getObjectID();
		strSql = QString("INSERT INTO char_pet(character_id,name,realname,loyality,"
			"state,race,grade,lossMinGrade,lossMaxGrade,type,pos,id)"
			" VALUES(%1,'%2','%3',%4,%5,%6,%7,%8,%9,%10,%11,%12)")
			.arg(char_id).arg(tmpObj->getObjectName())
			.arg(tmpObj->_realName).arg(tmpObj->_loyality).arg(tmpObj->_state)
			.arg(tmpObj->_race).arg(tmpObj->_grade).arg(tmpObj->_lossMinGrade).arg(tmpObj->_lossMaxGrade)
			.arg(tmpObj->getObjectType()).arg(tmpObj->_pos)
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_Character)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameCharacter>();
		QMutexLocker locker(&tmpObj->_mutex);
		strSql = QString("INSERT INTO character(type,sex,gold,bankgold,score,job,useTitle,"
			"avatar_id,unitid,petid,petriding,direction,punchclock,usingpunchclock,value_charisma,"
			"x,y,battle_position,map_number,line,big_line,conn_state,last_time,id,role_type,souls,gid,name,titles,map_name,nick_name,host_id)"
			" VALUES(%1,%2,%3,%4,%5,'%6',%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,"
			"%19,%20,%21,%22,%23,%24,%25,%26,'%27','%28','%29','%30','%31',%32)")
			.arg(tmpObj->_type).arg(tmpObj->_sex).arg(tmpObj->_gold).arg(tmpObj->_bankgold).arg(tmpObj->_score).arg(tmpObj->_job).arg(tmpObj->_useTitle)
			.arg(tmpObj->_avatar_id).arg(tmpObj->_unitid).arg(tmpObj->_petid).arg(tmpObj->_petriding).arg(tmpObj->_direction).arg(tmpObj->_punchclock)\
			.arg(tmpObj->_usingpunchclock).arg(tmpObj->_value_charisma).arg(tmpObj->_x).arg(tmpObj->_y)\
			.arg(tmpObj->_battle_position).arg(tmpObj->_map_number).arg(tmpObj->_server_line).arg(tmpObj->_big_line).arg(tmpObj->_connectState)\
			.arg(tmpObj->_lastUpdateDateTime.toSecsSinceEpoch()).arg(tmpObj->getObjectID()) .arg(tmpObj->getObjectType())\
			.arg(tmpObj->_souls).arg(tmpObj->_gid).arg(tmpObj->getObjectName()).arg(tmpObj->_titles.join("|")).arg(tmpObj->_map_name).arg(tmpObj->_nickName)
			.arg(tmpObj->_localHostId);
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_BaseData)
	{
		quint64 char_id = 0;
		if (pObj->getObjectParent())
		{
			char_id = pObj->getObjectParent()->getObjectID();
		}
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameBaseData>();
		strSql = QString("INSERT INTO base_data(char_id,level,hp,maxhp,mp,maxmp,xp,"
						 "maxxp,health,skillslots,imageid,id)"
						 " VALUES(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12)")
						 .arg(char_id)
						 .arg(tmpObj->_level)
						 .arg(tmpObj->_hp)
						 .arg(tmpObj->_maxhp)
						 .arg(tmpObj->_mp)
						 .arg(tmpObj->_maxmp)		
						 .arg(tmpObj->_xp)
						 .arg(tmpObj->_maxxp)
						 .arg(tmpObj->_health)
						 .arg(tmpObj->_skillslots)				
						 .arg(tmpObj->_imageid)	
						 .arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AttributeData)
	{
		quint64 char_id = 0;
		if (pObj->getObjectParent())
		{
			char_id = pObj->getObjectParent()->getObjectID();
		}
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameAttributeData>();
		strSql = QString("INSERT INTO attribute_data(char_id,manu_endurance,manu_skillful,"
						 "manu_intelligence,points_endurance,points_strength,points_defense,points_agility,"
						 "points_magical,value_attack,value_defensive,value_agility,value_spirit,value_recovery,resist_poison,"
						 "resist_sleep,resist_medusa,resist_drunk,resist_chaos,resist_forget,fix_critical,fix_strikeback,fix_accurancy,"
						 "fix_dodge,element_earth,element_water,element_fire,element_wind,points_remain,id)"
						 " VALUES(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,"
						 "%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30)")
						 .arg(char_id)						
						 .arg(tmpObj->_manu_endurance)
						 .arg(tmpObj->_manu_skillful)
						 .arg(tmpObj->_manu_intelligence)
						 .arg(tmpObj->_points_endurance)
						 .arg(tmpObj->_points_strength)
						 .arg(tmpObj->_points_defense)
						 .arg(tmpObj->_points_agility)
						 .arg(tmpObj->_points_magical)
						 .arg(tmpObj->_value_attack)
						 .arg(tmpObj->_value_defensive)
						 .arg(tmpObj->_value_agility)
						 .arg(tmpObj->_value_spirit)
						 .arg(tmpObj->_value_recovery)
						 .arg(tmpObj->_resist_poison)
						 .arg(tmpObj->_resist_sleep)
						 .arg(tmpObj->_resist_medusa)
						 .arg(tmpObj->_resist_drunk)
						 .arg(tmpObj->_resist_chaos)
						 .arg(tmpObj->_resist_forget)
						 .arg(tmpObj->_fix_critical)
						 .arg(tmpObj->_fix_strikeback)
						 .arg(tmpObj->_fix_accurancy)
						 .arg(tmpObj->_fix_dodge)
						 .arg(tmpObj->_element_earth)
						 .arg(tmpObj->_element_water)
						 .arg(tmpObj->_element_fire)
						 .arg(tmpObj->_element_wind)
						 .arg(tmpObj->_points_remain)
						 .arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_ServerType)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameServerType>();
		strSql = QString("INSERT INTO game_server_type(id,big_line,name) VALUES(%1,%2,'%3')")
						 .arg(tmpObj->getObjectID())						 
						 .arg(tmpObj->_server_type)
						 .arg(tmpObj->getObjectName());
		bret = m_dbconn->execSql(strSql);
	}else if (GETDEVCLASS(objType) == TObject_Host)
	{
		auto tmpObj = pObj.dynamicCast<ITHost>();
		strSql = QString("INSERT INTO host_data(id,type,pc_name,pc_user_name,pc_mac_addr,pc_local_ip,order_num,remark) VALUES(%1,%2,'%3','%4','%5','%6',%7,'%8')")
			.arg(tmpObj->getObjectID())
			.arg(tmpObj->getObjectType())
			.arg(tmpObj->_sPcName)			
			.arg(tmpObj->_sPcUserName)
			.arg(tmpObj->_sPcMacAddr)
			.arg(tmpObj->_sPcLocalIp)
			.arg(tmpObj->getObjectOrder())
			.arg(tmpObj->getObjectDesc());
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
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_PetBook)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITCGPetPictorialBook>();
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
			.arg(tmpObj->getObjectID());
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
			.arg(tmpObj->getObjectID());
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
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AccountIdentity)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITAccountIdentity>();
		strSql = QString("UPDATE identification set name='%1',identityCard='%2',sex=%3,desc='%4' WHERE id=%5")
			.arg(tmpObj->getObjectName())
			.arg(tmpObj->_identity)
			.arg(tmpObj->_sex)
			.arg(tmpObj->getObjectDesc())
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Account)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITAccount>();
		strSql = QString("UPDATE account user='%1',passwd='%2',identifacation='%3',desc='%4' WHERE id=%5")
			.arg(tmpObj->_userName)
			.arg(tmpObj->_passwd)
			.arg(tmpObj->_identity)
			.arg(tmpObj->getObjectDesc())
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AccountGid)
	{
		QString szOwnCode;
		quint64 aid = 0;
		auto tmpObj = pObj.dynamicCast<ITAccountGid>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			aid = pOwn->getObjectID();
		strSql = QString("UPDATE gid gid='%1',aid=%2,server_type_id=%3,desc='%4' WHERE id=%5")
			.arg(tmpObj->_userGid).arg(aid) .arg(tmpObj->_serverTypeID) .arg(tmpObj->getObjectDesc())
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CharItem)
	{
		QString szOwnCode;
		quint64 char_id = 0;
		auto tmpObj = pObj.dynamicCast<ITGameItem>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			char_id = pOwn->getObjectID();
		strSql = QString("UPDATE char_item set character_id=%1,name='%2',count=%3,isbank=%4,item_id=%5,item_pos=%6 WHERE id=%7")
			.arg(char_id)
			.arg(tmpObj->getObjectName())
			.arg(tmpObj->_itemCount)
			.arg(tmpObj->getObjectType() == TObject_CharItem ? 0 : 1)
			.arg(tmpObj->getObjectCode())
			.arg(tmpObj->_itemPos)
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CGPet)
	{
		QString szOwnCode;
		quint64 char_id = 0;
		auto tmpObj = pObj.dynamicCast<ITGamePet>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			char_id = pOwn->getObjectID();
		strSql = QString("UPDATE char_pet set character_id=%1,name='%2',realname='%3',loyality=%4,"
			"state=%5,race=%6,grade=%7,lossMinGrade=%8,lossMaxGrade=%9,type=%10,pos=%11 WHERE id=%12")
			.arg(char_id).arg(tmpObj->getObjectName())
			.arg(tmpObj->_realName).arg(tmpObj->_loyality).arg(tmpObj->_state)
			.arg(tmpObj->_race).arg(tmpObj->_grade).arg(tmpObj->_lossMinGrade).arg(tmpObj->_lossMaxGrade)			
			.arg(tmpObj->getObjectType()).arg(tmpObj->_pos)
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CGSkill)
	{
		QString szOwnCode;
		quint64 char_id = 0;
		auto tmpObj = pObj.dynamicCast<ITGameSkill>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			char_id = pOwn->getObjectID();
		strSql = QString("UPDATE char_skill set linkid=%1,type=%2,name='%3',info='%4',s_index=%5,level=%6,maxlevel=%7,xp=%8,maxxp=%9,cost=%11,available=%12,flags=%13 WHERE id=%14")
			.arg(char_id)
			.arg(tmpObj->getObjectType())
			.arg(tmpObj->getObjectName())
			.arg(tmpObj->_info)
			.arg(tmpObj->_index)
			.arg(tmpObj->_level)
			.arg(tmpObj->_maxLevel)
			.arg(tmpObj->_xp)
			.arg(tmpObj->_maxxp)
			.arg(tmpObj->_cost)
			.arg(tmpObj->_available)
			.arg(tmpObj->_flags)
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_Character)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameCharacter>();
		QMutexLocker locker(&tmpObj->_mutex);

		strSql = QString("UPDATE character set gid='%1',type=%2,name='%3',sex=%4,gold=%5,bankgold=%6,"
				"score=%7,job='%8',useTitle=%9,titles='%10',"
				"value_charisma=%11,souls=%12,nick_name='%13',avatar_id=%14,unitid=%15,petid=%16,petriding=%17,"
				"direction=%18,punchclock=%19,usingpunchclock=%20,x=%21,y=%22,battle_position=%23,map_name='%24',"
				"map_number=%25,line=%26,big_line=%27,last_time=%28,conn_state=%29,role_type=%30,host_id=%31 WHERE id=%32")
			.arg(tmpObj->_gid).arg(tmpObj->_type).arg(tmpObj->getObjectName()).arg(tmpObj->_sex)
			.arg(tmpObj->_gold).arg(tmpObj->_bankgold).arg(tmpObj->_score).arg(tmpObj->_job).arg(tmpObj->_useTitle).arg(tmpObj->_titles.join("|"))\
			.arg(tmpObj->_value_charisma)
			.arg(tmpObj->_souls)
			.arg(tmpObj->_nickName)
			.arg(tmpObj->_avatar_id)
			.arg(tmpObj->_unitid)
			.arg(tmpObj->_petid)
			.arg(tmpObj->_petriding)
			.arg(tmpObj->_direction)
			.arg(tmpObj->_punchclock)
			.arg(tmpObj->_usingpunchclock)
			.arg(tmpObj->_x)
			.arg(tmpObj->_y)
			.arg(tmpObj->_battle_position)
			.arg(tmpObj->_map_name)
			.arg(tmpObj->_map_number)
			.arg(tmpObj->_server_line)
			.arg(tmpObj->_big_line)
			.arg(tmpObj->_lastUpdateDateTime.toSecsSinceEpoch())
			.arg(tmpObj->_connectState)
			.arg(tmpObj->getObjectType())
			.arg(tmpObj->_localHostId)
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_BaseData)
	{
		quint64 char_id = 0;
		if (pObj->getObjectParent())
		{
			char_id = pObj->getObjectParent()->getObjectID();
		}
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameBaseData>();
		strSql = QString("UPDATE base_data set char_id=%1,level=%2,hp=%3,maxhp=%4,mp=%5,maxmp=%6,xp=%7,"
			"maxxp=%8,health=%9,skillslots=%10,imageid=%11 WHERE id=%12")
						 .arg(char_id)
			.arg(tmpObj->_level)
			.arg(tmpObj->_hp)
			.arg(tmpObj->_maxhp)
			.arg(tmpObj->_mp)
			.arg(tmpObj->_maxmp)
			.arg(tmpObj->_xp)
			.arg(tmpObj->_maxxp)
			.arg(tmpObj->_health)
			.arg(tmpObj->_skillslots)
			.arg(tmpObj->_imageid)
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AttributeData)
	{
		quint64 char_id = 0;
		if (pObj->getObjectParent())
		{
			char_id = pObj->getObjectParent()->getObjectID();
		}
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameAttributeData>();
		strSql = QString("UPDATE attribute_data SET char_id=%1,manu_endurance=%2,manu_skillful=%3,"
			"manu_intelligence=%4,points_endurance=%5,points_strength=%6,points_defense=%7,points_agility=%8,"
			"points_magical=%9,value_attack=%10,value_defensive=%11,value_agility=%12,value_spirit=%13,value_recovery=%14,resist_poison=%15,"
			"resist_sleep=%16,resist_medusa=%17,resist_drunk=%18,resist_chaos=%19,resist_forget=%20,fix_critical=%21,fix_strikeback=%22,fix_accurancy=%23,"
			"fix_dodge=%24,element_earth=%25,element_water=%26,element_fire=%27,element_wind=%28,points_remain=%29 WHERE id=%30")
			 .arg(char_id)
			.arg(tmpObj->_manu_endurance)
			.arg(tmpObj->_manu_skillful)
			.arg(tmpObj->_manu_intelligence)
			.arg(tmpObj->_points_endurance)
			.arg(tmpObj->_points_strength)
			.arg(tmpObj->_points_defense)
			.arg(tmpObj->_points_agility)
			.arg(tmpObj->_points_magical)
			.arg(tmpObj->_value_attack)
			.arg(tmpObj->_value_defensive)
			.arg(tmpObj->_value_agility)
			.arg(tmpObj->_value_spirit)
			.arg(tmpObj->_value_recovery)
			.arg(tmpObj->_resist_poison)
			.arg(tmpObj->_resist_sleep)
			.arg(tmpObj->_resist_medusa)
			.arg(tmpObj->_resist_drunk)
			.arg(tmpObj->_resist_chaos)
			.arg(tmpObj->_resist_forget)
			.arg(tmpObj->_fix_critical)
			.arg(tmpObj->_fix_strikeback)
			.arg(tmpObj->_fix_accurancy)
			.arg(tmpObj->_fix_dodge)
			.arg(tmpObj->_element_earth)
			.arg(tmpObj->_element_water)
			.arg(tmpObj->_element_fire)
			.arg(tmpObj->_element_wind)
			.arg(tmpObj->_points_remain)
			.arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_ServerType)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGameServerType>();
		strSql = QString("UPDATE game_server_type SET big_line=%1,name='%2' WHERE id=%3")
						 .arg(tmpObj->_server_type)
						 .arg(tmpObj->getObjectName())
						 .arg(tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_Host)
	{
		auto tmpObj = pObj.dynamicCast<ITHost>();
		strSql = QString("UPDATE host_data SET type=%1,pc_name='%2',pc_user_name='%3',pc_mac_addr='%4',pc_local_ip='%5',order_num=%6,remark='%7' WHERE id=%8")
		.arg(tmpObj->getObjectType())
		.arg(tmpObj->_sPcName)
		.arg(tmpObj->_sPcUserName)
		.arg(tmpObj->_sPcMacAddr)
		.arg(tmpObj->_sPcLocalIp)
		.arg(tmpObj->getObjectOrder())
		.arg(tmpObj->getObjectDesc())
		.arg(tmpObj->getObjectID())
		;
		bret = m_dbconn->execSql(strSql);
	}
	qDebug() << strSql;
	return bret;
}

//通过当前实时获取的数据 更新数据库存储的物品信息
QString ITObjectDataMgr::StoreServerItemData(const ::CGData::CGStoreItemRequest* pItem)
{
	if (!pItem)
		return "";

	bool bForceUpdate = m_bForceUpdate;
	//通过code 先通过code判断吧，后续再说 或者名称判断
	auto localItem = m_codeForGameItem.value(pItem->item_id());
	if (localItem)
	{
		//已鉴定 名称不一样 则更新本地
		if (!localItem->_bUpdate && bForceUpdate || (pItem->assessed() && localItem->getObjectName().toStdString() != pItem->name()))
		{
			localItem->_itemType = pItem->type();
			//localItem->_itemPrice = nPrice;
			//localItem->_itemPile = nMaxPile;
			localItem->_itemLevel = pItem->level();
			//localItem->_sellMinCount = nSellLimitCount;
			localItem->setObjectName(QString::fromStdString(pItem->name()));
			QString sDesc = QString::fromStdString(pItem->desc());
			if (sDesc.startsWith("$4"))
			{
				sDesc = sDesc.mid(2);
			}
			localItem->setObjectDsec(sDesc);
			localItem->setObjectCode(pItem->item_id());
			localItem->_itemAttr = QString::fromStdString(pItem->attribute());
			localItem->_bUpdate = true;
			localItem->setEditStatus();

			/*qDebug() << localItem->getObjectName() << " " << localItem->_itemType << " " << localItem->_itemLevel << " "\
				<< localItem->getObjectName() << " " << localItem->getObjectDesc() << " " << localItem->getObjectCode();*/
		}
		//qDebug() << localItem->getObjectName() << " " << localItem->getObjectCode() << QString::fromStdString(pItem->name()) << " " << pItem->item_id();
		return localItem->getObjectName();
	}
	else
	{
		localItem = newOneObject(TObject_Item).dynamicCast<ITGameItem>();
		localItem->_itemType = pItem->type();
		//localItem->_itemPrice = nPrice;
		//localItem->_itemPile = nMaxPile;
		localItem->_itemLevel = pItem->level();
		//localItem->_sellMinCount = nSellLimitCount;
		localItem->setObjectName(QString::fromStdString(pItem->name()));
		QString sDesc = QString::fromStdString(pItem->desc());
		if (sDesc.startsWith("$4"))
		{
			sDesc = sDesc.mid(2);
		}
		localItem->setObjectDsec(sDesc);
		localItem->setObjectCode(pItem->item_id());
		localItem->_itemAttr = QString::fromStdString(pItem->attribute());
		m_codeForGameItem.insert(pItem->item_id(), localItem);
		/*qDebug() << localItem->getObjectName() << " " << localItem->_itemType << " " << localItem->_itemLevel << " "\
			<< localItem->getObjectName() << " " << localItem->getObjectDesc() << " " << localItem->getObjectCode();*/
	}
	return "";
}
//通过当前实时获取的数据 更新数据库存储的地图信息
void ITObjectDataMgr::StoreServerMapData(const QString& mapName, int mapNumber)
{
	bool bForceUpdate = false;
	//通过code 先通过code判断吧，后续再说 或者名称判断
	auto localObj = m_numberForGameMap.value(mapNumber);
	if (localObj)
	{
		//名称不一样 则更新本地
		if (!localObj->_bUpdate && bForceUpdate || (localObj->getObjectName() != mapName))
		{
			localObj->_mapNumber = mapNumber;
			localObj->setObjectName(mapName);
			localObj->_bUpdate = true;
			localObj->setEditStatus();
			qDebug() << "更新数据库地图信息：" << localObj->getObjectName() << " " << mapNumber;
		}
	}
	else
	{
		localObj = newOneObject(TObject_Map).dynamicCast<ITGameMap>();
		localObj->_mapNumber = mapNumber;
		localObj->setObjectName(mapName);
		localObj->_bUpdate = true;
		m_numberForGameMap.insert(mapNumber, localObj);
		qDebug() << "新增数据库地图信息：" << localObj->getObjectName() << " " << mapNumber;
	}
}

QString ITObjectDataMgr::GetCharacterSexText(int image_id)
{
	//qDebug() << "人物图片id:" << image_id;
	if (image_id >= 100000 && image_id < 100250)
	{
		return "男";
	}
	else if (image_id >= 100250 && image_id <= 100423)
	{
		return "女";
	}
	else if (image_id >= 100525 && image_id <= 100630)
	{
		return "男";
	}
	else if (image_id >= 100650 && image_id < 100800)
	{
		return "女";
	}
	else if (image_id >= 100800 && image_id < 105250)
	{
		return "男";
	}
	else if (image_id >= 105250 && image_id < 106000)
	{
		return "女";
	}
	else if (image_id >= 106000 && image_id < 106250)
	{
		return "男";
	}
	else if (image_id >= 106250 && image_id < 106400)
	{
		return "女";
	}
	else if (image_id >= 106400 && image_id < 106600)
	{
		return "男";
	}
	else if (image_id >= 106600 && image_id < 106730)
	{
		return "女";
	}
	return "";

}

int ITObjectDataMgr::GetCharacterSex(int image_id)
{
	int boy = 0;
	int girl = 1;
	//qDebug() << "人物图片id:" << image_id;
	if (image_id >= 100000 && image_id < 100250)
	{
		return boy;
	}
	else if (image_id >= 100250 && image_id <= 100423)
	{
		return girl;
	}
	else if (image_id >= 100525 && image_id <= 100630)
	{
		return boy;
	}
	else if (image_id >= 100650 && image_id < 100800)
	{
		return girl;
	}
	else if (image_id >= 100800 && image_id < 105250)
	{
		return boy;
	}
	else if (image_id >= 105250 && image_id < 106000)
	{
		return girl;
	}
	else if (image_id >= 106000 && image_id < 106250)
	{
		return boy;
	}
	else if (image_id >= 106250 && image_id < 106400)
	{
		return girl;
	}
	else if (image_id >= 106400 && image_id < 106600)
	{
		return boy;
	}
	else if (image_id >= 106600 && image_id < 106730)
	{
		return girl;
	}
	return boy;

}

void ITObjectDataMgr::StoreUploadGidData(const ::CGData::UploadGidDataRequest* request)
{
	if (!request)
		return;
	QString sGid = QString::fromStdString(request->gid());
	QString sCharacterName = QString::fromStdString(request->character_name());
	int nBigLine = request->big_line();
	if (sGid.isEmpty() || sCharacterName.isEmpty())
		return;
	//附带生成一个gid
	int nServerType;
	if (m_serverTypeForObjType.contains(nBigLine))
	{
		nServerType = m_serverTypeForObjType.value(nBigLine);
	}
	else
	{
		nServerType = TObject_ServerType_ItemSales_Telecom;
		nBigLine = 13;
	}
	auto pServerType = m_serverTypeForObj.value(nBigLine);
	if (pServerType == nullptr)
	{
		QMutexLocker locker(&m_rpcGidMutex);		
		pServerType = newOneObject(nServerType).dynamicCast<ITGameServerType>();
		if (pServerType)
		{
			pServerType->setObjectName(m_serverTypeForObjName.value(nBigLine));
			m_serverTypeForObj.insert(nBigLine, pServerType);
		}		
	}
	QString sBigLine = QString::number(nBigLine);
	QString sID = sCharacterName + sBigLine;
	int roleType = request->role_type();
	int roleObjectType = TObject_Character;
	if (roleType == 0)
	{
		roleObjectType = TObject_CharacterLeft;
	}else if (roleType == 1)
	{
		roleObjectType = TObject_CharacterRight;
	}
	ITGameCharacterPtr pCharacter = pServerType->getRoleObjFromRoleName(sCharacterName).dynamicCast<ITGameCharacter>();		
	if (!pCharacter)
	{
		pCharacter = newOneObject(roleObjectType, pServerType).dynamicCast<ITGameCharacter>();		
		pCharacter->_baseData = newOneObject(TObject_BaseData, pCharacter).dynamicCast<ITGameBaseData>();		
		pCharacter->_attrData = newOneObject(TObject_AttributeData, pCharacter).dynamicCast<ITGameAttributeData>();		
		pCharacter->setObjectName(sCharacterName);
		pCharacter->_gid = QString::fromStdString(request->gid());

		gameCharacterAddToServerType(pCharacter, pServerType);
	}
	else
	{
		pCharacter->setEditStatus();
		if (pCharacter->_baseData == nullptr)
		{
			pCharacter->_baseData = newOneObject(TObject_BaseData, pCharacter).dynamicCast<ITGameBaseData>();		
		}
		if (pCharacter->_attrData == nullptr)
		{
			pCharacter->_attrData = newOneObject(TObject_AttributeData, pCharacter).dynamicCast<ITGameAttributeData>();
		}
	}
	pCharacter->_connectState = 1;
	pCharacter->_lastUploadTime.restart();
	pCharacter->_lastUpdateDateTime = QDateTime::currentDateTime();
	if (!m_onlineAccountRoles.contains(sID))
	{
		m_onlineAccountRoles.append(sID);
	}	
	if (pCharacter->getObjectType() != roleObjectType)
	{
		pCharacter->setObjectType(roleObjectType);
		pCharacter->setEditStatus();
	}
	pCharacter->_gid = QString::fromStdString(request->gid());
	pCharacter->setObjectName(sCharacterName);
	ITGameBaseData tmpBaseData;
	tmpBaseData._level = request->character_data().base_data().level();
	tmpBaseData._hp = request->character_data().base_data().hp();
	tmpBaseData._mp = request->character_data().base_data().mp();
	tmpBaseData._maxhp = request->character_data().base_data().maxhp();
	tmpBaseData._maxmp = request->character_data().base_data().maxmp();
	tmpBaseData._xp = request->character_data().base_data().xp();
	tmpBaseData._maxxp = request->character_data().base_data().maxxp();
	tmpBaseData._health = request->character_data().base_data().health();
	tmpBaseData._imageid = request->character_data().image_id(); //人物模型图片id
	tmpBaseData._skillslots = request->character_data().skillslots(); //技能格
	if (*pCharacter->_baseData != tmpBaseData)
	{
		*pCharacter->_baseData = tmpBaseData;
		pCharacter->_baseData->setEditStatus();
	}
	pCharacter->_souls = request->character_data().souls();
	pCharacter->_gold = request->character_data().gold();
	pCharacter->_sex = GetCharacterSex(pCharacter->_baseData->_imageid);
	pCharacter->_score = request->character_data().score();				//战绩？
	pCharacter->_useTitle = request->character_data().use_title();		//当前使用的称号
	pCharacter->_avatar_id = request->character_data().avatar_id();
	pCharacter->_unitid = request->character_data().unitid();
	pCharacter->_petid = request->character_data().petid();				  //当前出战宠物id
	pCharacter->_petriding = request->character_data().petriding();			  //骑乘中？
	pCharacter->_direction = request->character_data().direction();				  //面对的方向
	pCharacter->_punchclock = request->character_data().punchclock();			  //卡时
	pCharacter->_usingpunchclock = request->character_data().usingpunchclock();	  //是否打卡中
	pCharacter->_job = QString::fromStdString(request->character_data().job());				  //职业名称
	pCharacter->_nickName = QString::fromStdString(request->character_data().nick());
	pCharacter->_battle_position = request->character_data().battle_position();
	{
		QMutexLocker locker(&pCharacter->_mutex);
		pCharacter->_titles.clear();

		for (int i = 0; i < request->character_data().titles_size(); ++i)
		{
			pCharacter->_titles.append(QString::fromStdString(request->character_data().titles(i)));
		}
	}
	pCharacter->_x = request->character_data().x(); //当前坐标
	pCharacter->_y = request->character_data().y();
	pCharacter->_map_name = QString::fromStdString(request->character_data().map_name());						 //地图名称
	pCharacter->_map_number = request->character_data().map_number();											 //地图编号
	pCharacter->_server_line = request->character_data().server_line();											 //当前服务器线路
	pCharacter->_big_line = nBigLine; //当前服务器线路
	pCharacter->_value_charisma = request->character_data().value_charisma();									 //魅力
	if (request->character_data().has_game_status())
		pCharacter->_game_status = request->character_data().game_status();	  
	if (request->character_data().has_world_status())
		pCharacter->_world_status = request->character_data().world_status();		
	if (request->character_data().has_game_pid())
		pCharacter->_game_pid = request->character_data().game_pid();		  
	if (request->character_data().has_game_port())
		pCharacter->_game_port = request->character_data().game_port();				
	if (request->character_data().has_game_time())
	{
		pCharacter->_gameSysTime.years = request->character_data().game_time().years();	  
		pCharacter->_gameSysTime.month = request->character_data().game_time().month();	  
		pCharacter->_gameSysTime.days = request->character_data().game_time().days();	  
		pCharacter->_gameSysTime.hours = request->character_data().game_time().hours();	  
		pCharacter->_gameSysTime.mins = request->character_data().game_time().mins();	  
		pCharacter->_gameSysTime.secs = request->character_data().game_time().secs();	  
		pCharacter->_gameSysTime.local_time = request->character_data().game_time().local_time();	  
		pCharacter->_gameSysTime.server_time = request->character_data().game_time().server_time();	  
	}
	qDebug() << QString::fromStdString(request->gid()) << QString::fromStdString(request->character_name()) << request->character_data().level();

	ITGameAttributeData tmpAttrData;
	tmpAttrData._manu_endurance = request->character_data().manu_endurance();		  //耐力
	tmpAttrData._manu_skillful = request->character_data().manu_skillful();		  //灵巧
	tmpAttrData._manu_intelligence = request->character_data().manu_intelligence();		  //智力
	tmpAttrData._points_remain = request->character_data().detail().points_remain();
	tmpAttrData._points_endurance = request->character_data().detail().points_endurance();
	tmpAttrData._points_strength = request->character_data().detail().points_strength();
	tmpAttrData._points_defense = request->character_data().detail().points_defense();
	tmpAttrData._points_agility = request->character_data().detail().points_agility();
	tmpAttrData._points_magical = request->character_data().detail().points_magical();
	tmpAttrData._value_attack = request->character_data().detail().value_attack();
	tmpAttrData._value_defensive = request->character_data().detail().value_defensive();
	tmpAttrData._value_agility = request->character_data().detail().value_agility();
	tmpAttrData._value_spirit = request->character_data().detail().value_spirit();
	tmpAttrData._value_recovery = request->character_data().detail().value_recovery();
	tmpAttrData._resist_poison = request->character_data().detail().resist_poison();
	tmpAttrData._resist_sleep = request->character_data().detail().resist_sleep();
	tmpAttrData._resist_medusa = request->character_data().detail().resist_medusa();
	tmpAttrData._resist_drunk = request->character_data().detail().resist_drunk();
	tmpAttrData._resist_chaos = request->character_data().detail().resist_chaos();
	tmpAttrData._resist_forget = request->character_data().detail().resist_forget();
	tmpAttrData._fix_critical = request->character_data().detail().fix_critical();
	tmpAttrData._fix_strikeback = request->character_data().detail().fix_strikeback();
	tmpAttrData._fix_accurancy = request->character_data().detail().fix_accurancy();
	tmpAttrData._fix_dodge = request->character_data().detail().fix_dodge();
	tmpAttrData._element_earth = request->character_data().detail().element_earth();
	tmpAttrData._element_wind = request->character_data().detail().element_wind();
	tmpAttrData._element_water = request->character_data().detail().element_water();
	tmpAttrData._element_fire = request->character_data().detail().element_fire();
	if (*pCharacter->_attrData != tmpAttrData)
	{
		*pCharacter->_attrData = tmpAttrData;
		pCharacter->_attrData->setEditStatus();
	}
	QVector<int> posExist;
	if (request->character_data().skill_size() > 0)
	{
		for (int i = 0; i < request->character_data().skill_size(); ++i)
		{
			auto reqSkill = request->character_data().skill(i);
			auto skillPtr = pCharacter->_skillPosForSkill.value(reqSkill.index());
			if (!skillPtr)
			{
				skillPtr = qSharedPointerDynamicCast<ITGameSkill>(newOneObject(TObject_CharSkill, pCharacter));
				pCharacter->_skillPosForSkill.insert(reqSkill.index(), skillPtr);
			}
			ITGameSkill tmpSkill;
			tmpSkill.setObjectName(QString::fromStdString(reqSkill.name()));
			tmpSkill._info = QString::fromStdString(reqSkill.info());
			tmpSkill.setObjectCode(reqSkill.id());
			tmpSkill._cost = reqSkill.cost();
			tmpSkill._flags = reqSkill.flags();
			tmpSkill._index = reqSkill.index();
			tmpSkill._level = reqSkill.level();
			tmpSkill._maxLevel = reqSkill.maxlevel();
			tmpSkill._available = reqSkill.available();
			tmpSkill._xp = reqSkill.xp();
			tmpSkill._maxxp = reqSkill.maxxp();
			tmpSkill._bExist = true;
			if (*skillPtr != tmpSkill )
			{
				*skillPtr  = tmpSkill;
				skillPtr->setEditStatus();
			}
			posExist.append(reqSkill.index());
		}		
	}
	//技能删除
	for (int i = 0; i < 15; ++i)
	{
		if (posExist.contains(i))
			continue;
		auto pObjPtr = pCharacter->_skillPosForSkill.value(i);
		if (pObjPtr)
		{
			pObjPtr->_bExist = false;
			deleteOneObject(pObjPtr);
		}
	}
	posExist.clear();
	//物品位置判断吧  每次更新 不存在的  数据置空
	if (request->items_size() > 0)
	{
		for (int i = 0; i < request->items_size(); ++i)
		{
			auto reqItem = request->items(i);
			auto pItemPtr = pCharacter->_itemPosForPtr.value(reqItem.pos());
			if (!pItemPtr)
			{
				pItemPtr = qSharedPointerDynamicCast<ITGameItem>(newOneObject(TObject_CharItem, pCharacter));
				pCharacter->_itemPosForPtr.insert(reqItem.pos(), pItemPtr);
			}
			ITGameItem tmpItem;			
			tmpItem._itemAttr = QString::fromStdString(reqItem.attr());
			tmpItem.setObjectName(QString::fromStdString(reqItem.name()));
			tmpItem._itemCount = reqItem.count();
			tmpItem._itemPos = reqItem.pos();
			tmpItem._itemType = reqItem.type();	//不需要这么多信息的，从原始Item表拿
			tmpItem._bExist = true;
			tmpItem.setObjectCode(reqItem.item_id());
			if (*pItemPtr != tmpItem)
			{
				*pItemPtr = tmpItem;
				pItemPtr->setEditStatus();
			}
			posExist.append(reqItem.pos());
		}	
	}
	for (int i = 0; i < 28; ++i)
	{
		if (posExist.contains(i))
			continue;
		auto pItemPtr = pCharacter->_itemPosForPtr.value(i);
		if (pItemPtr)
		{
			pItemPtr->_bExist = false;
			deleteOneObject(pItemPtr);
			pCharacter->_itemPosForPtr.remove(i);
		}
	}
	posExist.clear();
	if (request->pet_data_size() > 0)
	{
		for (int i = 0; i < request->pet_data_size(); ++i)
		{
			auto reqPet = request->pet_data(i);
			auto petPtr = pCharacter->_petPosForPet.value(reqPet.index());
			if (!petPtr)
			{
				petPtr = qSharedPointerDynamicCast<ITGamePet>(newOneObject(TObject_CharPet, pCharacter));
				petPtr->_baseData = newOneObject(TObject_BaseData, petPtr).dynamicCast<ITGameBaseData>();
				petPtr->_attrData = newOneObject(TObject_AttributeData, petPtr).dynamicCast<ITGameAttributeData>();		
				pCharacter->_petPosForPet.insert(reqPet.index(), petPtr);
			}
			else
			{
				petPtr->setEditStatus();
				if (petPtr->_baseData == nullptr)
				{
					petPtr->_baseData = newOneObject(TObject_BaseData, petPtr).dynamicCast<ITGameBaseData>();
				}
				if (petPtr->_attrData == nullptr)
				{
					petPtr->_attrData = newOneObject(TObject_AttributeData, petPtr).dynamicCast<ITGameAttributeData>();
				}
			}

			petPtr->setObjectName(QString::fromStdString(reqPet.base_data().name()));
			petPtr->_realName = QString::fromStdString(reqPet.real_name());

			ITGameBaseData tmpBaseData;
			tmpBaseData._level = reqPet.base_data().level();
			tmpBaseData._hp = reqPet.base_data().hp();
			tmpBaseData._mp = reqPet.base_data().mp();
			tmpBaseData._maxhp = reqPet.base_data().maxhp();
			tmpBaseData._maxmp = reqPet.base_data().maxmp();
			tmpBaseData._xp = reqPet.base_data().xp();
			tmpBaseData._maxxp = reqPet.base_data().maxxp();
			tmpBaseData._health = reqPet.base_data().health();
			tmpBaseData._skillslots = reqPet.skillslots(); //技能格
			if (*petPtr->_baseData != tmpBaseData)
			{
				*petPtr->_baseData = tmpBaseData;
				petPtr->_baseData->setEditStatus();
			}

			//petPtr->_imageid = reqPet.image_id();		//人物模型图片id
			petPtr->_loyality = reqPet.loyality();
			petPtr->_race = reqPet.race();
			petPtr->_state = reqPet.state();
			petPtr->_grade = reqPet.grade();
			petPtr->_lossMinGrade = reqPet.lossmingrade();
			petPtr->_lossMaxGrade = reqPet.lossmaxgrade();
			petPtr->_pos = reqPet.index();

			ITGameAttributeData tmpAttrData;			
			tmpAttrData._points_remain = reqPet.detail().points_remain();
			tmpAttrData._points_endurance = reqPet.detail().points_endurance();
			tmpAttrData._points_strength = reqPet.detail().points_strength();
			tmpAttrData._points_defense = reqPet.detail().points_defense();
			tmpAttrData._points_agility = reqPet.detail().points_agility();
			tmpAttrData._points_magical = reqPet.detail().points_magical();
			tmpAttrData._value_attack = reqPet.detail().value_attack();
			tmpAttrData._value_defensive = reqPet.detail().value_defensive();
			tmpAttrData._value_agility = reqPet.detail().value_agility();
			tmpAttrData._value_spirit = reqPet.detail().value_spirit();
			tmpAttrData._value_recovery = reqPet.detail().value_recovery();
			tmpAttrData._resist_poison = reqPet.detail().resist_poison();
			tmpAttrData._resist_sleep = reqPet.detail().resist_sleep();
			tmpAttrData._resist_medusa = reqPet.detail().resist_medusa();
			tmpAttrData._resist_drunk = reqPet.detail().resist_drunk();
			tmpAttrData._resist_chaos = reqPet.detail().resist_chaos();
			tmpAttrData._resist_forget = reqPet.detail().resist_forget();
			tmpAttrData._fix_critical = reqPet.detail().fix_critical();
			tmpAttrData._fix_strikeback = reqPet.detail().fix_strikeback();
			tmpAttrData._fix_accurancy = reqPet.detail().fix_accurancy();
			tmpAttrData._fix_dodge = reqPet.detail().fix_dodge();
			tmpAttrData._element_earth = reqPet.detail().element_earth();
			tmpAttrData._element_wind = reqPet.detail().element_wind();
			tmpAttrData._element_water = reqPet.detail().element_water();
			tmpAttrData._element_fire = reqPet.detail().element_fire();
			if (*petPtr->_attrData != tmpAttrData)
			{
				*petPtr->_attrData = tmpAttrData;
				petPtr->_attrData->setEditStatus();
			}

			petPtr->_bExist = true;

			posExist.append(reqPet.index());

			if (reqPet.skill_size() > 0)
			{
				QVector<int> petSkillPosExist;
				for (int i = 0; i < reqPet.skill_size(); ++i)
				{
					auto reqSkill = reqPet.skill(i);
					auto skillPtr = petPtr->_skillPosForSkill.value(reqSkill.index());
					if (!skillPtr)
					{
						skillPtr = qSharedPointerDynamicCast<ITGameSkill>(newOneObject(TObject_CharPetSkill, petPtr));
						petPtr->_skillPosForSkill.insert(reqSkill.index(), skillPtr);
					}
					ITGameSkill tmpSkill;
					tmpSkill.setObjectName(QString::fromStdString(reqSkill.name()));
					tmpSkill._info = QString::fromStdString(reqSkill.info());
					tmpSkill.setObjectCode(reqSkill.id());
					tmpSkill._cost = reqSkill.cost();
					tmpSkill._flags = reqSkill.flags();
					tmpSkill._index = reqSkill.index();
					tmpSkill._level = reqSkill.level();
					tmpSkill._maxLevel = reqSkill.maxlevel();
					tmpSkill._available = reqSkill.available();
					tmpSkill._xp = reqSkill.xp();
					tmpSkill._maxxp = reqSkill.maxxp();
					tmpSkill._bExist = true;
					if (*skillPtr != tmpSkill)
					{
						*skillPtr = tmpSkill;
						skillPtr->setEditStatus();
					}	
					petSkillPosExist.append(reqSkill.index());
				}
				//技能删除
				for (int i = 0; i < 10; ++i)
				{
					if (petSkillPosExist.contains(i))
						continue;
					auto pObjPtr = petPtr->_skillPosForSkill.value(i);
					if (pObjPtr)
					{
						pObjPtr->_bExist = false;
						deleteOneObject(pObjPtr);
					}
				}
			}
		}	
	}
	//宠物删除
	for (int i = 0; i < 5; ++i)
	{
		if (posExist.contains(i))
			continue;
		auto petPtr = pCharacter->_petPosForPet.value(i);
		if (petPtr)
		{
			petPtr->_bExist = false;
			deleteOneObject(petPtr);
		}
	}
}

void ITObjectDataMgr::StoreUploadGidBankData(const ::CGData::UploadGidBankDataRequest* request)
{
	if (!request)
		return;
	QString sGid = QString::fromStdString(request->gid());
	QString sCharacterName = QString::fromStdString(request->character_name());
	int nBidLine = request->big_line();

	if (sGid.isEmpty() || sCharacterName.isEmpty())
		return;
	int nServerType;
	if (m_serverTypeForObjType.contains(nBidLine))
	{
		nServerType = m_serverTypeForObjType.value(nBidLine);
	}
	else
	{
		nServerType = TObject_ServerType_ItemSales_Telecom;
		nBidLine = 13;
	}
	auto pServerType = m_serverTypeForObj.value(nBidLine);
	if (pServerType == nullptr)
	{
		QMutexLocker locker(&m_rpcGidMutex);	
		pServerType = newOneObject(nServerType).dynamicCast<ITGameServerType>();
		if (pServerType)
		{
			pServerType->setObjectName(m_serverTypeForObjName.value(nBidLine));
			m_serverTypeForObj.insert(nBidLine, pServerType);
		}
	}
	QString sBigLine = QString::number(nBidLine);
	QString sID = sCharacterName + sBigLine;
	if (!m_onlineAccountRoles.contains(sID))
	{
		m_onlineAccountRoles.append(sID);
	}	
	int roleType = request->role_type();
	int roleObjectType = TObject_Character;
	if (roleType == 0)
	{
		roleObjectType = TObject_CharacterLeft;
	}
	else if (roleType == 1)
	{
		roleObjectType = TObject_CharacterRight;
	}
	ITGameCharacterPtr pCharacter = pServerType->getRoleObjFromRoleName(sCharacterName).dynamicCast<ITGameCharacter>();
	if (!pCharacter)
	{
		pCharacter = newOneObject(roleObjectType, pServerType).dynamicCast<ITGameCharacter>();
		pCharacter->_baseData = newOneObject(TObject_BaseData, pCharacter).dynamicCast<ITGameBaseData>();
		pCharacter->_attrData = newOneObject(TObject_AttributeData, pCharacter).dynamicCast<ITGameAttributeData>();
		pCharacter->setObjectName(sCharacterName);
		pCharacter->_gid = QString::fromStdString(request->gid());

		gameCharacterAddToServerType(pCharacter, pServerType);
	}
	else
	{
		pCharacter->setEditStatus();
		if (pCharacter->_baseData == nullptr)
		{
			pCharacter->_baseData = newOneObject(TObject_BaseData, pCharacter).dynamicCast<ITGameBaseData>();
		}
		if (pCharacter->_attrData == nullptr)
		{
			pCharacter->_attrData = newOneObject(TObject_AttributeData, pCharacter).dynamicCast<ITGameAttributeData>();
		}
	}
	pCharacter->_lastUploadTime.restart();
	pCharacter->_lastUpdateDateTime = QDateTime::currentDateTime();
	pCharacter->setObjectName(sCharacterName);
	pCharacter->_gid = QString::fromStdString(request->gid());
	pCharacter->_bankgold = request->gold();

	//物品位置判断吧  每次更新 不存在的  数据置空
	QVector<int> posExist;
	if (request->items_size() > 0)
	{
		for (int i = 0; i < request->items_size(); ++i)
		{
			auto reqItem = request->items(i);
			auto pItemPtr = pCharacter->_itemPosForPtr.value(reqItem.pos());
			if (!pItemPtr)
			{
				pItemPtr = qSharedPointerDynamicCast<ITGameItem>(newOneObject(TObject_CharBankItem, pCharacter));
				pCharacter->_itemPosForPtr.insert(reqItem.pos(), pItemPtr);
			}	

			ITGameItem tmpItem;
			tmpItem._itemAttr = QString::fromStdString(reqItem.attr());
			tmpItem.setObjectName(QString::fromStdString(reqItem.name()));
			tmpItem._itemCount = reqItem.count();
			tmpItem._itemPos = reqItem.pos();
			tmpItem._itemType = reqItem.type();	//不需要这么多信息的，从原始Item表拿
			tmpItem._bExist = true;
			tmpItem.setObjectCode(reqItem.item_id());
			if (*pItemPtr != tmpItem)
			{
				*pItemPtr = tmpItem;
				pItemPtr->setEditStatus();
			}	
			posExist.append(reqItem.pos());
		}		
	}
	for (int i = 100; i < 180; ++i)
	{
		if (posExist.contains(i))
			continue;
		auto pItemPtr = pCharacter->_itemPosForPtr.value(i);
		if (pItemPtr)
		{
			pItemPtr->_bExist = false;
			deleteOneObject(pItemPtr);
		}
	}
	posExist.clear();
	if (request->pet_data_size() > 0)
	{
		for (int i = 0; i < request->pet_data_size(); ++i)
		{
			auto reqPet = request->pet_data(i);
			auto petPtr = pCharacter->_petPosForPet.value(reqPet.index());
			if (!petPtr)
			{
				petPtr = qSharedPointerDynamicCast<ITGamePet>(newOneObject(TObject_BankPet, pCharacter));
				petPtr->_baseData = newOneObject(TObject_BaseData, petPtr).dynamicCast<ITGameBaseData>();
				petPtr->_attrData = newOneObject(TObject_AttributeData, petPtr).dynamicCast<ITGameAttributeData>();		
				pCharacter->_petPosForPet.insert(reqPet.index(), petPtr);
			}
			else
			{
				petPtr->setEditStatus();
				if (petPtr->_baseData == nullptr)
				{
					petPtr->_baseData = newOneObject(TObject_BaseData, petPtr).dynamicCast<ITGameBaseData>();
				}
				if (petPtr->_attrData == nullptr)
				{
					petPtr->_attrData = newOneObject(TObject_AttributeData, petPtr).dynamicCast<ITGameAttributeData>();
				}
			}

			petPtr->setObjectName(QString::fromStdString(reqPet.base_data().name()));
			petPtr->_realName = QString::fromStdString(reqPet.real_name());
			//	petPtr->setObjectCode(reqPet.petnumber);
			ITGameBaseData tmpBaseData;
			tmpBaseData._level = reqPet.base_data().level();
			tmpBaseData._hp = reqPet.base_data().hp();
			tmpBaseData._mp = reqPet.base_data().mp();
			tmpBaseData._maxhp = reqPet.base_data().maxhp();
			tmpBaseData._maxmp = reqPet.base_data().maxmp();
			tmpBaseData._xp = reqPet.base_data().xp();
			tmpBaseData._maxxp = reqPet.base_data().maxxp();
			tmpBaseData._health = reqPet.base_data().health();
			//tmpBaseData._imageid = reqPet.image_id();		//人物模型图片id
			tmpBaseData._skillslots = reqPet.skillslots(); //技能格
			if (*petPtr->_baseData != tmpBaseData)
			{
				*petPtr->_baseData = tmpBaseData;
				petPtr->_baseData->setEditStatus();
			}
			petPtr->_loyality = reqPet.loyality();
			petPtr->_race = reqPet.race();
			petPtr->_state = reqPet.state();
			petPtr->_grade = reqPet.grade();
			petPtr->_lossMinGrade = reqPet.lossmingrade();
			petPtr->_lossMaxGrade = reqPet.lossmaxgrade();
			petPtr->_pos = reqPet.index();


			ITGameAttributeData tmpAttrData;
			tmpAttrData._points_remain = reqPet.detail().points_remain();
			tmpAttrData._points_endurance = reqPet.detail().points_endurance();
			tmpAttrData._points_strength = reqPet.detail().points_strength();
			tmpAttrData._points_defense = reqPet.detail().points_defense();
			tmpAttrData._points_agility = reqPet.detail().points_agility();
			tmpAttrData._points_magical = reqPet.detail().points_magical();
			tmpAttrData._value_attack = reqPet.detail().value_attack();
			tmpAttrData._value_defensive = reqPet.detail().value_defensive();
			tmpAttrData._value_agility = reqPet.detail().value_agility();
			tmpAttrData._value_spirit = reqPet.detail().value_spirit();
			tmpAttrData._value_recovery = reqPet.detail().value_recovery();
			tmpAttrData._resist_poison = reqPet.detail().resist_poison();
			tmpAttrData._resist_sleep = reqPet.detail().resist_sleep();
			tmpAttrData._resist_medusa = reqPet.detail().resist_medusa();
			tmpAttrData._resist_drunk = reqPet.detail().resist_drunk();
			tmpAttrData._resist_chaos = reqPet.detail().resist_chaos();
			tmpAttrData._resist_forget = reqPet.detail().resist_forget();
			tmpAttrData._fix_critical = reqPet.detail().fix_critical();
			tmpAttrData._fix_strikeback = reqPet.detail().fix_strikeback();
			tmpAttrData._fix_accurancy = reqPet.detail().fix_accurancy();
			tmpAttrData._fix_dodge = reqPet.detail().fix_dodge();
			tmpAttrData._element_earth = reqPet.detail().element_earth();
			tmpAttrData._element_wind = reqPet.detail().element_wind();
			tmpAttrData._element_water = reqPet.detail().element_water();
			tmpAttrData._element_fire = reqPet.detail().element_fire();
			if (*petPtr->_attrData != tmpAttrData)
			{
				*petPtr->_attrData = tmpAttrData;
				petPtr->_attrData->setEditStatus();
			}
			petPtr->_bExist = true;
			posExist.append(reqPet.index());

			/*if (reqPet.skill_size() > 0)
			{
				QVector<int> posExist;
				for (int i = 0; i < reqPet.skill_size(); ++i)
				{
					auto reqSkill = request->character_data().skill(i);
					auto skillPtr = petPtr->_skillPosForSkill.value(reqSkill.index());
					if (!skillPtr)
					{
						skillPtr = qSharedPointerDynamicCast<ITGameSkill>(newOneObject(TObject_CharPetSkill, skillPtr));
						petPtr->_skillPosForSkill.insert(reqSkill.index(), skillPtr);
					}
					else
						skillPtr->setEditStatus();
					skillPtr->setObjectName(QString::fromStdString(reqSkill.name()));
					skillPtr->_info = QString::fromStdString(reqSkill.info());
					skillPtr->setObjectCode(reqSkill.id());
					skillPtr->_cost = reqSkill.cost();
					skillPtr->_flags = reqSkill.flags();
					skillPtr->_index = reqSkill.index();
					skillPtr->_level = reqSkill.level();
					skillPtr->_maxLevel = reqSkill.maxlevel();
					skillPtr->_available = reqSkill.available();
					skillPtr->_xp = reqSkill.xp();
					skillPtr->_maxxp = reqSkill.maxxp();
					posExist.append(reqSkill.index());
				}
				for (int i = 0; i < 10; ++i)
				{
					if (posExist.contains(i))
						continue;
					auto pItemPtr = petPtr->_skillPosForSkill.value(i);
					if (pItemPtr)
					{
						deleteOneObject(pItemPtr);
					}
				}
			}*/

		}	
	}
	for (int i = 100; i < 105; ++i)
	{
		if (posExist.contains(i))
			continue;
		auto petPtr = pCharacter->_petPosForPet.value(i);
		if (petPtr)
		{
			petPtr->_bExist = false;
			deleteOneObject(petPtr);
		}
	}
}

void ITObjectDataMgr::UploadCharcterServer(const ::CGData::UploadCharcterServerRequest *request)
{
	if (!request)
		return;
	QString sCharacterName = QString::fromStdString(request->char_name());
	int big_line = request->big_line();
	QString sIP = QString::fromStdString(request->ip());
	int nPort = request->port();
	int nOnline = request->online();
	if ( sCharacterName.isEmpty())
		return;
	auto pCharForObjHash = m_charNameForObj.value(big_line);
	auto pCharacter = pCharForObjHash.value(sCharacterName);
	if (!pCharacter)
	{
		pCharacter = newOneObject(TObject_CharServer).dynamicCast<ITCharcterServer>();	
	}
	else
		pCharacter->setEditStatus();
	pCharacter->setObjectName(sCharacterName);
	pCharacter->_ip = sIP;
	pCharacter->_port = nPort;
	pCharacter->_big_line = big_line;
	pCharacter->online = nOnline;
	pCharacter->is_open = request->is_open();
	pCharacter->is_multicast = request->is_multicast();
	pCharacter->multicast_ip = QString::fromStdString(request->multicast_ip());
	pCharacter->multicast_port = request->multicast_port();
	pCharForObjHash.insert(sCharacterName, pCharacter);
	m_charNameForObj.insert(big_line, pCharForObjHash);
	
}

void ITObjectDataMgr::UploadLocalPCData(const ::CGData::UploadLocalPCInfoRequest* request, ::CGData::UploadLocalPCInfoResponse* response)
{
	QString sMac = QString::fromStdString(request->pc_mac_addr());
	if (sMac.isEmpty())
	{
		return;
	}
	QString sGid = QString::fromStdString(request->gid());
	QString sCharacterName = QString::fromStdString(request->character_name());
	int nBidLine = request->big_line();

	if (sGid.isEmpty() || sCharacterName.isEmpty())
		return;
	int nServerType;
	if (m_serverTypeForObjType.contains(nBidLine))
	{
		nServerType = m_serverTypeForObjType.value(nBidLine);
	}
	else
	{
		nServerType = TObject_ServerType_ItemSales_Telecom;
		nBidLine = 13;
	}
	auto pServerType = m_serverTypeForObj.value(nBidLine);
	if (pServerType == nullptr)
	{
		return;
	}
	ITGameCharacterPtr pCharacter = pServerType->getRoleObjFromRoleName(sCharacterName).dynamicCast<ITGameCharacter>();
	if (!pCharacter)
		request;
	
	ITHostPtr pHost = nullptr;
	auto pObjList = GetDstObjTypeList(TObject_Host);	
	for (auto& pObj : pObjList)
	{
		auto pTmpHost = pObj.dynamicCast<ITHost>();
		if (pTmpHost && pTmpHost->_sPcMacAddr == sMac)
		{
			pHost = pTmpHost;
			break;
		}
	}	
	if (!pHost)
		pHost = newOneObject(TObject_Host).dynamicCast<ITHost>();
	if (!pHost)
	{
		qDebug() << "UploadLocalPCInfo newOneObject Failed";
		return;
	}
	pHost->_sPcMacAddr = sMac;
	pHost->_sPcLocalIp = QString::fromStdString(request->pc_local_ip());
	pHost->_sPcName = QString::fromStdString(request->pc_name());
	pHost->_sPcUserName = QString::fromStdString(request->pc_user_name());
	pHost->setEditStatus();
	pCharacter->_localHostId = pHost->getObjectID();
	pCharacter->setEditStatus();
}

Status ITObjectDataMgr::SelectGidData(const ::CGData::SelectGidDataRequest* request, ::CGData::SelectGidDataResponse* response)
{
	QString sGid = QString::fromStdString(request->gid());
	int nRoleType = request->role_type();
	QString sBigLine = QString::number(request->big_line());
	int nBigLine = request->big_line();
	if (sGid.isEmpty())
	{
		return Status::OK;
	}
	if (nBigLine == 0)
	{
		nBigLine = 13;
	}
	ITGameServerTypePtr pServerType = m_serverTypeForObj.value(nBigLine);
	if (pServerType == nullptr)
	{
		return Status::OK;
	}
	auto pGidObj = pServerType->getGidObjFromGidName(sGid);
	if (!pGidObj)
	{
		return Status::OK;
	}

	int roleObjType = TObject_Character;
	if (nRoleType == 1)
	{
		roleObjType = TObject_CharacterRight;
	}
	else
		roleObjType = TObject_CharacterLeft;
	ITAccountGidPtr pGid = pGidObj.dynamicCast<ITAccountGid>();
	if (pGid)
	{
		response->set_gid(pGid->_userGid.toStdString());
		response->set_online(0);	
		ITGameCharacterPtr pRole = nullptr;
		for (auto pTmpRole : pGid->GetAllChildObj())
		{
			if (pTmpRole->getObjectType() == roleObjType)
			{
				pRole =qSharedPointerDynamicCast<ITGameCharacter>(pTmpRole);
				break;
			}
		}
		if (pRole == nullptr)
		{
			roleObjType = TObject_Character;
			for (auto pTmpRole : pGid->GetAllChildObj())
			{
				if (pTmpRole->getObjectType() == roleObjType)
				{
					pRole = qSharedPointerDynamicCast<ITGameCharacter>(pTmpRole);
					break;
				}
			}
		}
		if (pRole)
		{
			response->set_online(pRole->_connectState);			
			auto pChar = response->mutable_character_data();
			response->set_character_name(pRole->getObjectName().toStdString());
			pChar->set_souls(pRole->_souls);
			pChar->set_level(pRole->_baseData->_level);
			pChar->set_gold(pRole->_gold);
			pChar->set_map_name(pRole->_map_name.toStdString());
			pChar->set_map_number(pRole->_map_number);
			pChar->set_job(pRole->_job.toStdString());
			pChar->set_nick(pRole->_nickName.toStdString());
			pChar->set_bank_gold(pRole->_bankgold);
			pChar->set_image_id(pRole->_baseData->_imageid);
			pChar->set_score(pRole->_score);
			pChar->set_skillslots(pRole->_baseData->_skillslots);
			pChar->set_use_title(pRole->_useTitle);
			pChar->set_avatar_id(pRole->_avatar_id);
			pChar->set_unitid(pRole->_unitid);
			pChar->set_petid(pRole->_petid);
			pChar->set_petriding(pRole->_petriding);
			pChar->set_direction(pRole->_direction);
			pChar->set_punchclock(pRole->_punchclock);
			pChar->set_usingpunchclock(pRole->_usingpunchclock);
			{
				QMutexLocker locker(&pRole->_mutex);
				for (auto tTitle : pRole->_titles)
				{
					pChar->add_titles(tTitle.toStdString());
				}
			}
			pChar->set_manu_endurance(pRole->_attrData->_manu_endurance);
			pChar->set_manu_skillful(pRole->_attrData->_manu_skillful);
			pChar->set_manu_intelligence(pRole->_attrData->_manu_intelligence);
			pChar->set_value_charisma(pRole->_value_charisma);
			pChar->set_x(pRole->_x);
			pChar->set_y(pRole->_y);
			pChar->set_server_line(pRole->_server_line);			
			pChar->set_big_line(pRole->_big_line);
			pChar->set_battle_position(pRole->_battle_position);
			//pChar->set_battle_position(pRole->_battle_position);
			//pChar->set_battle_position(pRole->_battle_position);
			auto detailData = pChar->mutable_detail();
			detailData->set_points_remain(pRole->_attrData->_points_remain);
			detailData->set_points_endurance(pRole->_attrData->_points_endurance);
			detailData->set_points_strength(pRole->_attrData->_points_strength);
			detailData->set_points_defense(pRole->_attrData->_points_defense);
			detailData->set_points_agility(pRole->_attrData->_points_agility);
			detailData->set_points_magical(pRole->_attrData->_points_magical);
			detailData->set_value_attack(pRole->_attrData->_value_attack);
			detailData->set_value_defensive(pRole->_attrData->_value_defensive);
			detailData->set_value_agility(pRole->_attrData->_value_agility);
			detailData->set_value_spirit(pRole->_attrData->_value_spirit);
			detailData->set_value_recovery(pRole->_attrData->_value_recovery);
			detailData->set_resist_poison(pRole->_attrData->_resist_poison);
			detailData->set_resist_sleep(pRole->_attrData->_resist_sleep);
			detailData->set_resist_medusa(pRole->_attrData->_resist_medusa);
			detailData->set_resist_drunk(pRole->_attrData->_resist_drunk);
			detailData->set_resist_chaos(pRole->_attrData->_resist_chaos);
			detailData->set_resist_forget(pRole->_attrData->_resist_forget);
			detailData->set_fix_critical(pRole->_attrData->_fix_critical);
			detailData->set_fix_strikeback(pRole->_attrData->_fix_strikeback);
			detailData->set_fix_accurancy(pRole->_attrData->_fix_accurancy);
			detailData->set_fix_dodge(pRole->_attrData->_fix_dodge);
			detailData->set_element_earth(pRole->_attrData->_element_earth);
			detailData->set_element_water(pRole->_attrData->_element_water);
			detailData->set_element_fire(pRole->_attrData->_element_fire);
			detailData->set_element_wind(pRole->_attrData->_element_wind);

			{
				auto pBaseData = pChar->mutable_base_data();
				pBaseData->set_hp(pRole->_baseData->_hp);
				pBaseData->set_mp(pRole->_baseData->_mp);
				pBaseData->set_name(pRole->getObjectName().toStdString());
				pBaseData->set_maxhp(pRole->_baseData->_maxhp);
				pBaseData->set_maxmp(pRole->_baseData->_maxmp);
				pBaseData->set_level(pRole->_baseData->_level);
				pBaseData->set_xp(pRole->_baseData->_xp);
				pBaseData->set_maxxp(pRole->_baseData->_maxxp);
				pBaseData->set_health(pRole->_baseData->_health);
			}
			auto persDescData = pChar->mutable_pers_desc();
			//persDescData->set_buyicon(pRole->bu)

			for (auto it = pRole->_petPosForPet.begin(); it != pRole->_petPosForPet.end(); ++it)
			{
				if (!it.value()->_bExist)
				{
					continue;
				}
				auto petData = response->add_pet_data();
				auto pBaseData = petData->base_data();
				pBaseData.set_hp(it.value()->_baseData->_hp);
				pBaseData.set_mp(it.value()->_baseData->_mp);
				pBaseData.set_name(it.value()->getObjectName().toStdString());
				pBaseData.set_maxhp(it.value()->_baseData->_maxhp);
				pBaseData.set_maxmp(it.value()->_baseData->_maxmp);
				pBaseData.set_level(it.value()->_baseData->_level);
				pBaseData.set_xp(it.value()->_baseData->_xp);
				pBaseData.set_maxxp(it.value()->_baseData->_maxxp);
				pBaseData.set_health(it.value()->_baseData->_health);
				petData->set_index(it.key());
				//petData->set_flags(it.value()->_fl);
				petData->set_grade(it.value()->_grade);
				petData->set_loyality(it.value()->_loyality);
				//petData->set_default_battle(it.value()->_loyality);
				petData->set_state(it.value()->_state);
				petData->set_lossmingrade(it.value()->_lossMinGrade);
				petData->set_lossmaxgrade(it.value()->_lossMaxGrade);
				petData->set_real_name(it.value()->_realName.toStdString());
				petData->set_race(it.value()->_race);
				petData->set_skillslots(it.value()->_baseData->_skillslots);
			}
			for (auto it = pRole->_itemPosForPtr.begin(); it != pRole->_itemPosForPtr.end(); ++it)
			{
				if (!it.value()->_bExist)
				{
					continue;
				}
				auto itemData = response->add_items();
				//一些数据需要从item表查数据返回 这里不需要关注这些
				//itemData->set_image_id(it.value()->_itemAttr);
				itemData->set_count(it.value()->_itemCount);
				itemData->set_type(it.value()->_itemType);
				/*itemData->set_image_id(it.value()->_itemPrice);
				itemData->set_image_id(it.value()->_itemPile);
				itemData->set_image_id(it.value()->_itemLevel);
				itemData->set_image_id(it.value()->_sellMinCount);*/
				itemData->set_pos(it.value()->_itemPos);
				// itemData->set_level(it.value()->_itemPos);
				itemData->set_attr(it.value()->_itemAttr.toStdString());
				//itemData->set_info(it.value()->_itemAttr.toStdString());
				itemData->set_item_id(it.value()->getObjectCode());
				itemData->set_name(it.value()->getObjectName().toStdString());
			}
		}	
	}
	return Status::OK;

}

Status ITObjectDataMgr::SelectCharacterServer(const ::CGData::SelectCharacterServerRequest *request, ::CGData::SelectCharacterServerResponse *response)
{
	if (!request)
		return Status::OK;
	QString sCharacterName = QString::fromStdString(request->char_name());
	int big_line = request->big_line();	
	if (sCharacterName.isEmpty())
		return Status::OK;
	auto pCharForObjHash = m_charNameForObj.value(big_line);
	auto pCharacter = pCharForObjHash.value(sCharacterName);
	if (!pCharacter)
	{
		return Status::OK;
	}	
	response->set_ip(pCharacter->_ip.toStdString());
	response->set_port(pCharacter->_port);
	response->set_online(pCharacter->online);
	response->set_big_line(pCharacter->_big_line);

	response->set_is_open(pCharacter->is_open);
	response->set_is_multicast(pCharacter->is_multicast);
	response->set_multicast_ip( pCharacter->multicast_ip.toStdString());
	response->set_multicast_port( pCharacter->multicast_port);
	return Status::OK;
}

Status ITObjectDataMgr::SelectCharacterData(const ::CGData::SelectCharacterDataRequest *request, ::CGData::SelectCharacterDataResponse *response)
{
	if (!request)
		return Status::OK;
	QString sCharacterName = QString::fromStdString(request->char_name());
	QString sBigLine = QString::number(request->big_line());
	int nBigLine = request->big_line();
	if (sCharacterName.isEmpty())
		return Status::OK;
	if (nBigLine == 0)
	{
		nBigLine = 13;
	}
	ITGameServerTypePtr pServerType = m_serverTypeForObj.value(nBigLine);
	if (pServerType == nullptr)
	{
		return Status::OK;
	}
	
	auto pCharacter = pServerType->getRoleObjFromRoleName(sCharacterName);
	if (!pCharacter)
	{
		return Status::OK;
	}
	ITGameCharacterPtr pRole = pCharacter.dynamicCast<ITGameCharacter>();

	//查到了的话 复制一份
	response->set_character_name(request->char_name());
	response->set_big_line(request->big_line());
	response->set_online(0);	
	if (pRole)
	{
		/*ITGameCharacter tmpCharacter;
		tmpCharacter = *(pRole.get());*/

		response->set_online(pRole->_connectState);
		auto pChar = response->mutable_character_data();
		response->set_character_name(pRole->getObjectName().toStdString());
		pChar->set_souls(pRole->_souls);
		pChar->set_gold(pRole->_gold);
		pChar->set_map_name(pRole->_map_name.toStdString());
		pChar->set_map_number(pRole->_map_number);
		pChar->set_job(pRole->_job.toStdString());
		pChar->set_nick(pRole->_nickName.toStdString());
		pChar->set_bank_gold(pRole->_bankgold);
		pChar->set_score(pRole->_score);
		pChar->set_use_title(pRole->_useTitle);
		pChar->set_avatar_id(pRole->_avatar_id);
		pChar->set_unitid(pRole->_unitid);
		pChar->set_petid(pRole->_petid);
		pChar->set_petriding(pRole->_petriding);
		pChar->set_direction(pRole->_direction);
		pChar->set_punchclock(pRole->_punchclock);
		pChar->set_usingpunchclock(pRole->_usingpunchclock);

		{
			QMutexLocker locker(&pRole->_mutex);
			for (auto tTitle : pRole->_titles)
			{
				pChar->add_titles(tTitle.toStdString());
			}
		}
	
		pChar->set_x(pRole->_x);
		pChar->set_y(pRole->_y);
		pChar->set_server_line(pRole->_server_line);
		pChar->set_big_line(pRole->_big_line);
		pChar->set_battle_position(pRole->_battle_position);
		pChar->set_value_charisma(pRole->_value_charisma);

		if (pRole->_attrData)
		{
			pChar->set_manu_endurance(pRole->_attrData->_manu_endurance);
			pChar->set_manu_skillful(pRole->_attrData->_manu_skillful);
			pChar->set_manu_intelligence(pRole->_attrData->_manu_intelligence);
			auto detailData = pChar->mutable_detail();
			detailData->set_points_remain(pRole->_attrData->_points_remain);
			detailData->set_points_endurance(pRole->_attrData->_points_endurance);
			detailData->set_points_strength(pRole->_attrData->_points_strength);
			detailData->set_points_defense(pRole->_attrData->_points_defense);
			detailData->set_points_agility(pRole->_attrData->_points_agility);
			detailData->set_points_magical(pRole->_attrData->_points_magical);
			detailData->set_value_attack(pRole->_attrData->_value_attack);
			detailData->set_value_defensive(pRole->_attrData->_value_defensive);
			detailData->set_value_agility(pRole->_attrData->_value_agility);
			detailData->set_value_spirit(pRole->_attrData->_value_spirit);
			detailData->set_value_recovery(pRole->_attrData->_value_recovery);
			detailData->set_resist_poison(pRole->_attrData->_resist_poison);
			detailData->set_resist_sleep(pRole->_attrData->_resist_sleep);
			detailData->set_resist_medusa(pRole->_attrData->_resist_medusa);
			detailData->set_resist_drunk(pRole->_attrData->_resist_drunk);
			detailData->set_resist_chaos(pRole->_attrData->_resist_chaos);
			detailData->set_resist_forget(pRole->_attrData->_resist_forget);
			detailData->set_fix_critical(pRole->_attrData->_fix_critical);
			detailData->set_fix_strikeback(pRole->_attrData->_fix_strikeback);
			detailData->set_fix_accurancy(pRole->_attrData->_fix_accurancy);
			detailData->set_fix_dodge(pRole->_attrData->_fix_dodge);
			detailData->set_element_earth(pRole->_attrData->_element_earth);
			detailData->set_element_water(pRole->_attrData->_element_water);
			detailData->set_element_fire(pRole->_attrData->_element_fire);
			detailData->set_element_wind(pRole->_attrData->_element_wind);		
		}
	
	
		//pChar->set_battle_position(pRole->_battle_position);
		//pChar->set_battle_position(pRole->_battle_position);
		
		auto pBaseData = pChar->mutable_base_data();
		pBaseData->set_name(pRole->getObjectName().toStdString());
		if (pRole->_baseData)
		{
			pChar->set_level(pRole->_baseData->_level);
			pChar->set_image_id(pRole->_baseData->_imageid);
			pChar->set_skillslots(pRole->_baseData->_skillslots);
			pBaseData->set_hp(pRole->_baseData->_hp);
			pBaseData->set_mp(pRole->_baseData->_mp);
			pBaseData->set_maxhp(pRole->_baseData->_maxhp);
			pBaseData->set_maxmp(pRole->_baseData->_maxmp);
			pBaseData->set_level(pRole->_baseData->_level);
			pBaseData->set_xp(pRole->_baseData->_xp);
			pBaseData->set_maxxp(pRole->_baseData->_maxxp);
			pBaseData->set_health(pRole->_baseData->_health);
		}		
		
		auto persDescData = pChar->mutable_pers_desc();
		for (auto it = pRole->_petPosForPet.begin(); it != pRole->_petPosForPet.end(); ++it)
		{
			if (!it.value()->_bExist)
			{
				continue;
			}
			auto petData = response->add_pet_data();
			auto pBaseData = petData->base_data();
			pBaseData.set_name(it.value()->getObjectName().toStdString());

			if (it.value()->_baseData)
			{
				pBaseData.set_hp(it.value()->_baseData->_hp);
				pBaseData.set_mp(it.value()->_baseData->_mp);
				pBaseData.set_maxhp(it.value()->_baseData->_maxhp);
				pBaseData.set_maxmp(it.value()->_baseData->_maxmp);
				pBaseData.set_level(it.value()->_baseData->_level);
				pBaseData.set_xp(it.value()->_baseData->_xp);
				pBaseData.set_maxxp(it.value()->_baseData->_maxxp);
				pBaseData.set_health(it.value()->_baseData->_health);
				petData->set_skillslots(it.value()->_baseData->_skillslots);
			}			
			petData->set_index(it.key());
			//petData->set_flags(it.value()->_fl);
			petData->set_grade(it.value()->_grade);
			petData->set_loyality(it.value()->_loyality);
			//petData->set_default_battle(it.value()->_loyality);
			petData->set_state(it.value()->_state);
			petData->set_lossmingrade(it.value()->_lossMinGrade);
			petData->set_lossmaxgrade(it.value()->_lossMaxGrade);
			petData->set_real_name(it.value()->_realName.toStdString());
			petData->set_race(it.value()->_race);
		}
		for (auto it = pRole->_itemPosForPtr.begin(); it != pRole->_itemPosForPtr.end(); ++it)
		{
			if (!it.value()->_bExist)
			{
				continue;
			}
			auto itemData = response->add_items();
			//一些数据需要从item表查数据返回 这里不需要关注这些
			//itemData->set_image_id(it.value()->_itemAttr);
			itemData->set_count(it.value()->_itemCount);
			itemData->set_type(it.value()->_itemType);
			/*itemData->set_image_id(it.value()->_itemPrice);
				itemData->set_image_id(it.value()->_itemPile);
				itemData->set_image_id(it.value()->_itemLevel);
				itemData->set_image_id(it.value()->_sellMinCount);*/
			itemData->set_pos(it.value()->_itemPos);
			// itemData->set_level(it.value()->_itemPos);
			itemData->set_attr(it.value()->_itemAttr.toStdString());
			//itemData->set_info(it.value()->_itemAttr.toStdString());
			itemData->set_item_id(it.value()->getObjectCode());
			itemData->set_name(it.value()->getObjectName().toStdString());
		}
	}	
	return Status::OK;
}

ITObjectList ITObjectDataMgr::FindData(int nType, const QString& sName)
{
	ITObjectList pGidList;
	switch (nType)
	{
	case TObject_CGItem:
	case TObject_Item:
	case TObject_CharItem:
	case TObject_CharBankItem:
	{
		for (auto it = m_idForAccountRole.begin(); it != m_idForAccountRole.end(); ++it)
		{
			auto pItems = it.value()->_itemPosForPtr;
			for (auto tmpItem=pItems.begin();tmpItem!=pItems.end();++tmpItem)
			{
				if (tmpItem.value()->getObjectName().contains(sName))
				{
					//没有类型判断 
					pGidList.append(it.value()->getObjectParent());
					break;
				}
			}
		}
		break;
	}
	case TObject_CGPet:
	case TObject_Pet:
	case TObject_BankPet:
	case TObject_CharPet:
	{
		for (auto it = m_idForAccountRole.begin(); it != m_idForAccountRole.end(); ++it)
		{
			auto pPets = it.value()->_petPosForPet;
			for (auto tmpPet = pPets.begin(); tmpPet != pPets.end(); ++tmpPet)
			{
				if (tmpPet.value()->getObjectName().contains(sName) || tmpPet.value()->_realName.contains(sName))
				{
					//没有类型判断 
					pGidList.append(it.value()->getObjectParent().dynamicCast<ITAccountGid>());
					break;
				}
			}
		}
		break;
	}
	case TObject_CharGold:
	case TObject_CharBankGold:
	{
		for (auto it = m_idForAccountRole.begin(); it != m_idForAccountRole.end(); ++it)
		{
			if (it.value()->_gold == sName.toInt()|| it.value()->_bankgold == sName.toInt())
			{	//没有类型判断 
				pGidList.append(it.value()->getObjectParent().dynamicCast<ITAccountGid>());
				break;
			}		
		}
		break;
	}
	default:
		break;
	}
	return pGidList;
}
