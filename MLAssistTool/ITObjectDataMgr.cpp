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
	qRegisterMetaType<ITGameSkillPtr>("ITGameGateMapPtr");
	qRegisterMetaType<ITAccountIdentityPtr>("ITAccountIdentityPtr");

	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_GateMap, NEW_MODULE_FACTORY(ITGameGateMap));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_PetBook, NEW_MODULE_FACTORY(ITCGPetPictorialBook));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Map, NEW_MODULE_FACTORY(ITGameMap));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_AccountIdentity, NEW_MODULE_FACTORY(ITAccountIdentity));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_Account, NEW_MODULE_FACTORY(ITAccount));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_AccountGid, NEW_MODULE_FACTORY(ITAccountGid));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_GidRole, NEW_MODULE_FACTORY(ITGidRole));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_GidRoleLeft, NEW_MODULE_FACTORY(ITGidRole));
	ObjectModuleRegisty::GetInstance().RegisterModuleFactory(TObject_GidRoleRight, NEW_MODULE_FACTORY(ITGidRole));
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
	connect(this, SIGNAL(signal_loadDataFini()), this, SLOT(doLoadDBInfoOver())); //??????????????????????????????
	init();
}

ITObjectDataMgr::~ITObjectDataMgr(void)
{
}
bool ITObjectDataMgr::Fini()
{
	return true;
}
//????????????????????? ???????????? ?????????
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
}

ITObjectDataMgr& ITObjectDataMgr::getInstance(void)
{
	static ITObjectDataMgr mitsObjicedata;
	return mitsObjicedata;
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

	//QString sDBPath = QApplication::applicationDirPath() + "//db//cg.db";
	bool bRet = false;
	//if (connectToDB("SQLITE", "CG", sDBPath, "admin", "123456"))
	if (connectToDB(strProvider,strHostName,strDBName, strUserName, strPassWord, nPort))
	{
		qDebug() << "????????????????????????";
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
	//??????????????????
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
	//????????????????????????tgt???????????????
	if (cRoute->_targetMapNumber == tgtIndex)
	{
		return true;
	}
	//?????????????????????????????????????????? ??????????????????
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

ITObjectList ITObjectDataMgr::GetDstObjTypeList(int objType)
{
	ITObjectList pSelectObjs;

	QHash<quint64, ITObjectPtr> tmpObjList;
	
	{
		QMutexLocker locker(&m_objMutex);
		tmpObjList = m_pObjectList;
	}
	for (auto it= tmpObjList.begin();it!= tmpObjList.end();++it)
	{
		if (it.value()->getObjectType() == objType)
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
		if (it.value()->getObjectType() == objType)
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
	//????????????
	pThis->LoadItems();
	pThis->LoadPets();
	pThis->LoadMaps();
	pThis->LoadGateMaps();
	//????????????
	pThis->LoadIdentification();
	pThis->LoadAccount();
	pThis->LoadAccountGid();
	pThis->LoadAccountRole();
	pThis->LoadGidItems();
	pThis->LoadGidPets();
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
	//! ????????????  ????????????
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
//! ?????????ITObjectDataMgr::getDBConnectState()
//! ???????????????????????????????????????
//! ????????????ture ?????????  false  ?????????
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
	//! ????????????  ????????????
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
	ITObjectPtr pParentObj = pObj->getObjectParent();
	if (pParentObj)
	{
		pParentObj->removeChildObj(pObj);
	}
	QMutexLocker locker(&m_objMutex);
	int nstatus = pObj->getStatus();
	if (nstatus == TStatus_Add)
	{
		pObj->setDelStatus();
		m_pAddObjectList.remove(pObj->getObjectID());
	}
	else
	{
		pObj->setDelStatus();
		m_pObjectList.remove(pObj->getObjectID());
		if (!m_pDelObjectList.contains(pObj->getObjectID()))
			m_pDelObjectList.insert(pObj->getObjectID(),pObj);
	}
	return true;
}

quint64 ITObjectDataMgr::getNewObjectID()
{
	//return ITObjectID::NewID();
	return ITObjectID::NewID32();
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
	}
	for (auto it= tmpObjList.begin();it!= tmpObjList.end();++it)
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
			strInfo += "????????????????????????\n";
			return false;
		}
	}
	if (!deleteDataFromDB())
	{
		strInfo += "?????????????????????\n";
		strInfo += m_dbconn->getLastError();
		//	return false;
	}
	if (!insertNewDataToDB())
	{
		strInfo += "?????????????????????\n";
		strInfo += m_dbconn->getLastError();
		//	return false;
	}
	if (!updateDataForDB())
	{
		strInfo += "?????????????????????\n";
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
			quint64 nID = (quint64)recordset->getIntValue("id");
			QString sName = recordset->getStrValue("name");
			QString identityCard = recordset->getStrValue("identityCard");
			int sex = recordset->getIntValue("sex");
			QString sDesc = recordset->getStrValue("desc");	//???????????? ???item??????
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
			quint64 nID = (quint64)recordset->getIntValue("id");
			QString user = recordset->getStrValue("user");
			QString passwd = recordset->getStrValue("passwd");
			QString identification = recordset->getStrValue("identification");
			QString sDesc = recordset->getStrValue("desc");	//???????????? ???item??????
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
			quint64 nID = (quint64)recordset->getIntValue("id");
			QString gid = recordset->getStrValue("gid");
			quint64 aid = (quint64)recordset->getIntValue("aid");
			QString sDesc = recordset->getStrValue("desc");	//???????????? ???item??????
			if (gid.isEmpty())
			{
				continue;
			}
			auto pAccountObj = FindObject(aid).dynamicCast<ITAccount>();
			ITAccountGidPtr pObj = newOneObject(TObject_AccountGid, nID).dynamicCast<ITAccountGid>();
			if (pObj)
			{
				pObj->_userGid = gid;
				pObj->setObjectName(gid);
				pObj->setObjectDsec(sDesc);
				pObj->setObjectID(nID);
				if (pAccountObj)
				{
					pAccountObj->addChildObj(pObj);
					pObj->setObjectParent(pAccountObj);
				}
				m_pObjectList.insert(nID, pObj);
				m_idForAccountGid.insert(gid, pObj);
			}
		}
		return true;
	}
	return false;
}

bool ITObjectDataMgr::LoadAccountRole()
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
			quint64 nID = (quint64)recordset->getIntValue("id");
			QString sGid = recordset->getStrValue("gid");
			QString sName = recordset->getStrValue("name");
			int objType = recordset->getIntValue("role_type");
			if (sGid.isEmpty())
			{
				continue;
			}
			if (objType == 0 )
			{
				objType = TObject_GidRole;
			}
			ITAccountGidPtr pGid = m_idForAccountGid.value(sGid);
			if (pGid == nullptr)
			{
				QMutexLocker locker(&m_rpcGidMutex);

				pGid = newOneObject(TObject_AccountGid).dynamicCast<ITAccountGid>();
				pGid->setObjectName(sGid);
				pGid->_userGid = sGid;
				m_idForAccountGid.insert(sGid, pGid);
			}
			ITGidRolePtr pCharacter = newOneObject(objType, nID).dynamicCast<ITGidRole>();
			if (pCharacter)
			{
				if (pGid)
				{
					pGid->addChildObj(pCharacter);
					pCharacter->setObjectParent(pGid);
				}
				pCharacter->_gid = sGid;
				pCharacter->_level = recordset->getIntValue("level");
				pCharacter->_type = recordset->getIntValue("type");
				pCharacter->_imageid = recordset->getIntValue("imageid");
				pCharacter->_sex = recordset->getIntValue("sex");
				pCharacter->_gold = recordset->getIntValue("gold");
				pCharacter->_bankgold = recordset->getIntValue("bankgold");
				pCharacter->_xp = recordset->getIntValue("xp");
				pCharacter->_maxxp = recordset->getIntValue("maxxp");
				pCharacter->_hp = recordset->getIntValue("hp");
				pCharacter->_maxhp = recordset->getIntValue("maxhp");
				pCharacter->_mp = recordset->getIntValue("mp");
				pCharacter->_maxmp = recordset->getIntValue("maxmp");
				pCharacter->_score = recordset->getIntValue("score");
				pCharacter->_job = recordset->getStrValue("job");
				pCharacter->_useTitle = recordset->getIntValue("useTitle");
				pCharacter->_titles = recordset->getStrValue("titles").split("|");
				pCharacter->_skillslots = recordset->getIntValue("skillslots");
				pCharacter->_manu_endurance = recordset->getIntValue("manu_endurance");
				pCharacter->_manu_skillful = recordset->getIntValue("manu_skillful");
				pCharacter->_manu_intelligence = recordset->getIntValue("manu_intelligence");
				pCharacter->_value_charisma = recordset->getIntValue("value_charisma");
				pCharacter->_points_endurance = recordset->getIntValue("points_endurance");
				pCharacter->_points_strength = recordset->getIntValue("points_strength");
				pCharacter->_points_defense = recordset->getIntValue("points_defense");
				pCharacter->_points_agility = recordset->getIntValue("points_agility");
				pCharacter->_points_magical = recordset->getIntValue("points_magical");
				pCharacter->_value_attack = recordset->getIntValue("value_attack");
				pCharacter->_value_defensive = recordset->getIntValue("value_defensive");
				pCharacter->_value_agility = recordset->getIntValue("value_agility");
				pCharacter->_value_spirit = recordset->getIntValue("value_spirit");
				pCharacter->_value_recovery = recordset->getIntValue("value_recovery");
				pCharacter->_resist_poison = recordset->getIntValue("resist_poison");
				pCharacter->_resist_sleep = recordset->getIntValue("resist_sleep");
				pCharacter->_resist_medusa = recordset->getIntValue("resist_medusa");
				pCharacter->_resist_drunk = recordset->getIntValue("resist_drunk");
				pCharacter->_resist_chaos = recordset->getIntValue("resist_chaos");
				pCharacter->_resist_forget = recordset->getIntValue("resist_forget");
				pCharacter->_fix_critical = recordset->getIntValue("fix_critical");
				pCharacter->_fix_strikeback = recordset->getIntValue("fix_strikeback");
				pCharacter->_fix_accurancy = recordset->getIntValue("fix_accurancy");
				pCharacter->_fix_dodge = recordset->getIntValue("fix_dodge");
				pCharacter->_element_earth = recordset->getIntValue("element_earth");
				pCharacter->_element_water = recordset->getIntValue("element_water");
				pCharacter->_element_fire = recordset->getIntValue("element_fire");
				pCharacter->_element_wind = recordset->getIntValue("element_wind");
				pCharacter->_points_remain = recordset->getIntValue("points_remain");
				pCharacter->setObjectName(sName);
				m_pObjectList.insert(nID, pCharacter);
				m_idForAccountRole.insert(sGid + sName, pCharacter);
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
			quint64 nID = (quint64)recordset->getIntValue("id");
			quint64 nChara_dbid = (quint64)recordset->getIntValue("character_id");//??????id
			QString sName = recordset->getStrValue("name");
			int count = recordset->getIntValue("count");
			int isbank = recordset->getIntValue("isbank");
			int item_id = recordset->getIntValue("item_id");	//???????????? ???item??????
			int item_pos = recordset->getIntValue("item_pos");	//???????????? ???item??????
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

				ITGidRolePtr pGidRole = FindObject(nChara_dbid).dynamicCast<ITGidRole>();
				if (!pGidRole)
				{
					if (!m_pDelObjectList.contains(nID))
						m_pDelObjectList.insert(nID, pItem);
				}
				else
				{
					pItem->setObjectParent(pGidRole);
					pGidRole->addChildObj(pItem);
					m_pObjectList.insert(nID, pItem);
					//?????? ???????????? ????????????
					auto oldItem = pGidRole->_itemPosForPtr.value(item_pos);
					if (oldItem != nullptr)
					{							
						if (!m_pDelObjectList.contains(oldItem->getObjectID()))
						{
							m_pDelObjectList.insert(oldItem->getObjectID(), oldItem);
							m_pObjectList.remove(oldItem->getObjectID());
						}						
					}		
					pGidRole->_itemPosForPtr.insert(item_pos, pItem);//????????????
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
			quint64 nID = (quint64)recordset->getIntValue("id");
			quint64 character_id = (quint64)recordset->getIntValue("character_id");
			QString sName = recordset->getStrValue("name");
			QString realname = recordset->getStrValue("realname");
			int objtype = recordset->getIntValue("type");

			ITGamePetPtr pObj = newOneObject(objtype, nID).dynamicCast<ITGamePet>();
			if (pObj)
			{

				pObj->_character_id = character_id;
				pObj->_realName = realname;
				pObj->_level = recordset->getIntValue("level");
				pObj->_state = recordset->getIntValue("state");
				//pObj->_imageid = recordset->getIntValue("imageid");			
				pObj->_xp = recordset->getIntValue("xp");
				pObj->_maxxp = recordset->getIntValue("maxxp");
				pObj->_hp = recordset->getIntValue("hp");
				pObj->_maxhp = recordset->getIntValue("maxhp");
				pObj->_mp = recordset->getIntValue("mp");
				pObj->_maxmp = recordset->getIntValue("maxmp");
				pObj->_skillslots = recordset->getIntValue("skillslots");
				pObj->_points_endurance = recordset->getIntValue("points_endurance");
				pObj->_points_strength = recordset->getIntValue("points_strength");
				pObj->_points_defense = recordset->getIntValue("points_defense");
				pObj->_points_agility = recordset->getIntValue("points_agility");
				pObj->_points_magical = recordset->getIntValue("points_magical");
				pObj->_value_attack = recordset->getIntValue("value_attack");
				pObj->_value_defensive = recordset->getIntValue("value_defensive");
				pObj->_value_agility = recordset->getIntValue("value_agility");
				pObj->_value_spirit = recordset->getIntValue("value_spirit");
				pObj->_value_recovery = recordset->getIntValue("value_recovery");
				pObj->_resist_poison = recordset->getIntValue("resist_poison");
				pObj->_resist_sleep = recordset->getIntValue("resist_sleep");
				pObj->_resist_medusa = recordset->getIntValue("resist_medusa");
				pObj->_resist_drunk = recordset->getIntValue("resist_drunk");
				pObj->_resist_chaos = recordset->getIntValue("resist_chaos");
				pObj->_resist_forget = recordset->getIntValue("resist_forget");
				pObj->_fix_critical = recordset->getIntValue("fix_critical");
				pObj->_fix_strikeback = recordset->getIntValue("fix_strikeback");
				pObj->_fix_accurancy = recordset->getIntValue("fix_accurancy");
				pObj->_fix_dodge = recordset->getIntValue("fix_dodge");
				pObj->_element_earth = recordset->getIntValue("element_earth");
				pObj->_element_water = recordset->getIntValue("element_water");
				pObj->_element_fire = recordset->getIntValue("element_fire");
				pObj->_element_wind = recordset->getIntValue("element_wind");
				pObj->_points_remain = recordset->getIntValue("points_remain");
				pObj->_pos = recordset->getIntValue("pos");

				pObj->setObjectName(sName);
				//	pObj->setObjectDsec(sDesc);
				//	pObj->setObjectCode(nCode);
				pObj->setObjectID(nID);
				ITGidRolePtr pGidRole = FindObject(character_id).dynamicCast<ITGidRole>();
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
					//?????? ???????????? ????????????
					auto pOldObj = pGidRole->_petPosForPet.value(pObj->_pos);
					if (pOldObj != nullptr)
					{
						if (!m_pDelObjectList.contains(pOldObj->getObjectID()))
						{
							m_pDelObjectList.insert(pOldObj->getObjectID(), pOldObj);
							m_pObjectList.remove(pOldObj->getObjectID());
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
			quint64 nID = (quint64)recordset->getIntValue("id");
			quint64 nChara_dbid = (quint64)recordset->getIntValue("linkid");//??????id
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

				ITGidRolePtr pGidRole = FindObject(nChara_dbid).dynamicCast<ITGidRole>();
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
					//?????? ???????????? ????????????
					auto pOldObj = pGidRole->_skillPosForSkill.value(index);
					if (pOldObj != nullptr)
					{
						if (!m_pDelObjectList.contains(pOldObj->getObjectID()))
						{
							m_pDelObjectList.insert(pOldObj->getObjectID(), pOldObj);
							m_pObjectList.remove(pOldObj->getObjectID());
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

	if (str.contains("????????????????????????") || str.contains("????????????") || str.contains("???????????????"))
		return false;
	else if (str.contains("??????=") || str.contains("bytes="))
		return true;
	else
		return true;
}

bool ITObjectDataMgr::reconnectDB() //???????????????
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
	//! ????????????  ????????????
	if (m_dbconn)
	{
		m_dbconn.clear();
		m_dbconn = NULL;
	}
	return false;
}

bool ITObjectDataMgr::insertNewDataToDB()
{
	ITObjectList tempSuccessList; ///?????????????????????????????????????????????????????????????????????
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
	if (!bSucc)
	{ ///?????????????????????????????????
		foreach(auto pDev, tempSuccessList)
		{
			m_pObjectList.insert(pDev->getObjectID(), pDev);
			if (m_pAddObjectList.contains(pDev->getObjectID()))
			{
				m_pAddObjectList.remove(pDev->getObjectID());
			}
		}
		return false;
	}
	m_pAddObjectList.clear();
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
		strSql = QString("DELETE FROM item WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_PetBook)
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
	else if (GETDEVCLASS(objType) == TObject_GidRole)
	{
		strSql = QString("DELETE FROM character WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_Account)
	{
		strSql = QString("DELETE FROM account WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AccountGid)
	{
		strSql = QString("DELETE FROM gid WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (objType == TObject_AccountIdentity)
	{
		strSql = QString("DELETE FROM identification WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CGSkill)
	{
		strSql = QString("DELETE FROM char_skill WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CharItem)
	{
		strSql = QString("DELETE FROM char_item WHERE id=%1").arg((int)pObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CharPet)
	{
		strSql = QString("DELETE FROM char_pet WHERE id=%1").arg((int)pObj->getObjectID());
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
			.arg((int)tmpObj->getObjectID()).arg(tmpObj->getObjectName())
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
			.arg((int)tmpObj->getObjectID()).arg(tmpObj->_userName)
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
		strSql = QString("INSERT INTO gid(id,gid,aid,desc) VALUES(%1,'%2','%3','%4')")
			.arg((int)tmpObj->getObjectID()).arg(tmpObj->_userGid)
			.arg(aid)
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
			.arg((int)tmpObj->getObjectID()).arg((int)char_id)
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
			.arg((int)tmpObj->getObjectID())
			.arg((int)char_id)
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
		strSql = QString("INSERT INTO char_pet(character_id,name,realname,level,hp,maxhp,mp,maxmp,xp,maxxp,loyality,"
			"health,state,skillslots,race,grade,lossMinGrade,lossMaxGrade,points_remain,"
			"points_endurance, points_strength, points_defense, points_agility,"
			"points_magical,value_attack,value_defensive,value_agility,value_spirit,value_recovery,resist_poison,"
			"resist_sleep,resist_medusa,resist_drunk,resist_chaos,resist_forget,fix_critical,fix_strikeback,fix_accurancy,"
			"fix_dodge,element_earth,element_water,element_fire,element_wind,type,pos,id)"
			" VALUES(%1,'%2','%3',%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,"
			"%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,%41,%42,%43,%44,%45,%46)")
			.arg((int)char_id).arg(tmpObj->getObjectName())
			.arg(tmpObj->_realName).arg(tmpObj->_level).arg(tmpObj->_hp).arg(tmpObj->_maxhp).arg(tmpObj->_mp)\
			.arg(tmpObj->_maxmp).arg(tmpObj->_xp).arg(tmpObj->_maxxp).arg(tmpObj->_loyality).arg(tmpObj->_health).arg(tmpObj->_state)
			.arg(tmpObj->_skillslots).arg(tmpObj->_race).arg(tmpObj->_grade).arg(tmpObj->_lossMinGrade).arg(tmpObj->_lossMaxGrade)
			.arg(tmpObj->_points_remain).arg(tmpObj->_points_endurance)\
			.arg(tmpObj->_points_strength).arg(tmpObj->_points_defense).arg(tmpObj->_points_agility).arg(tmpObj->_points_magical).arg(tmpObj->_value_attack)\
			.arg(tmpObj->_value_defensive).arg(tmpObj->_value_agility).arg(tmpObj->_value_spirit).arg(tmpObj->_value_recovery).arg(tmpObj->_resist_poison)\
			.arg(tmpObj->_resist_sleep).arg(tmpObj->_resist_medusa).arg(tmpObj->_resist_drunk).arg(tmpObj->_resist_chaos).arg(tmpObj->_resist_forget)\
			.arg(tmpObj->_fix_critical).arg(tmpObj->_fix_strikeback).arg(tmpObj->_fix_accurancy).arg(tmpObj->_fix_dodge)
			.arg(tmpObj->_element_earth).arg(tmpObj->_element_water).arg(tmpObj->_element_fire).arg(tmpObj->_element_wind)
			.arg(tmpObj->getObjectType()).arg(tmpObj->_pos)
			.arg((int)tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_GidRole)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGidRole>();
		QMutexLocker locker(&tmpObj->_mutex);
		strSql = QString("INSERT INTO character(gid,type,name,level,imageid,sex,gold,bankgold,xp,"
			"maxxp,hp,maxhp,mp,maxmp,score,job,useTitle,titles,skillslots,manu_endurance,manu_skillful,"
			"manu_intelligence,value_charisma,points_endurance,points_strength,points_defense,points_agility,"
			"points_magical,value_attack,value_defensive,value_agility,value_spirit,value_recovery,resist_poison,"
			"resist_sleep,resist_medusa,resist_drunk,resist_chaos,resist_forget,fix_critical,fix_strikeback,fix_accurancy,"
			"fix_dodge,element_earth,element_water,element_fire,element_wind,points_remain,id,role_type)"
			" VALUES('%1',%2,'%3',%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,'%16',%17,'%18',"
			"%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,%41,%42,%43,%44,%45,%46,%47,%48,%49,%50)")
			.arg(tmpObj->_gid).arg(tmpObj->_type).arg(tmpObj->getObjectName()).arg(tmpObj->_level).arg(tmpObj->_imageid).arg(tmpObj->_sex)
			.arg(tmpObj->_gold).arg(tmpObj->_bankgold).arg(tmpObj->_xp).arg(tmpObj->_maxxp).arg(tmpObj->_hp).arg(tmpObj->_maxhp).arg(tmpObj->_mp)\
			.arg(tmpObj->_maxmp).arg(tmpObj->_score).arg(tmpObj->_job).arg(tmpObj->_useTitle).arg(tmpObj->_titles.join("|")).arg(tmpObj->_skillslots)\
			.arg(tmpObj->_manu_endurance).arg(tmpObj->_manu_skillful).arg(tmpObj->_manu_intelligence).arg(tmpObj->_value_charisma).arg(tmpObj->_points_endurance)\
			.arg(tmpObj->_points_strength).arg(tmpObj->_points_defense).arg(tmpObj->_points_agility).arg(tmpObj->_points_magical).arg(tmpObj->_value_attack)\
			.arg(tmpObj->_value_defensive).arg(tmpObj->_value_agility).arg(tmpObj->_value_spirit).arg(tmpObj->_value_recovery).arg(tmpObj->_resist_poison)\
			.arg(tmpObj->_resist_sleep).arg(tmpObj->_resist_medusa).arg(tmpObj->_resist_drunk).arg(tmpObj->_resist_chaos).arg(tmpObj->_resist_forget)\
			.arg(tmpObj->_fix_critical).arg(tmpObj->_fix_strikeback).arg(tmpObj->_fix_accurancy).arg(tmpObj->_fix_dodge)
			.arg(tmpObj->_element_earth).arg(tmpObj->_element_water).arg(tmpObj->_element_fire).arg(tmpObj->_element_wind).arg(tmpObj->_points_remain)
			.arg((int)tmpObj->getObjectID()).arg(tmpObj->getObjectType());
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
	else if (objType == TObject_AccountIdentity)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITAccountIdentity>();
		strSql = QString("UPDATE identification set name='%1',identityCard='%2',sex=%3,desc='%4' WHERE id=%5")
			.arg(tmpObj->getObjectName())
			.arg(tmpObj->_identity)
			.arg(tmpObj->_sex)
			.arg(tmpObj->getObjectDesc())
			.arg((int)tmpObj->getObjectID());
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
			.arg((int)tmpObj->getObjectID());
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
		strSql = QString("UPDATE gid gid='%1',aid=%2,desc='%3' WHERE id=%4")
			.arg(tmpObj->_userGid)
			.arg((int)aid)
			.arg(tmpObj->getObjectDesc())
			.arg((int)tmpObj->getObjectID());
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
			.arg((int)char_id)
			.arg(tmpObj->getObjectName())
			.arg(tmpObj->_itemCount)
			.arg(tmpObj->getObjectType() == TObject_CharItem ? 0 : 1)
			.arg(tmpObj->getObjectCode())
			.arg(tmpObj->_itemPos)
			.arg((int)tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVSUBCLASS(objType) == TObject_CharPet)
	{
		QString szOwnCode;
		quint64 char_id = 0;
		auto tmpObj = pObj.dynamicCast<ITGamePet>();
		auto pOwn = tmpObj->getObjectParent();
		if (pOwn)
			char_id = pOwn->getObjectID();
		strSql = QString("UPDATE char_pet set character_id=%1,name='%2',realname='%3',level=%4,"
			"hp=%5,maxhp=%6,mp=%7,maxmp=%8,xp=%9,maxxp=%10,loyality=%11,health=%12,state=%13,"
			"skillslots=%14,race=%15,grade=%16,lossMinGrade=%17,lossMaxGrade=%18,"
			"points_remain=%19,points_endurance=%20,points_strength=%21,"
			"points_defense=%22,points_agility=%23,points_magical=%24,value_attack=%25,value_defensive=%26,value_agility=%27,value_spirit=%28,"
			"value_recovery=%29,resist_poison=%30,resist_sleep=%31,resist_medusa=%32,resist_drunk=%33,resist_chaos=%34,resist_forget=%35,"
			"fix_critical=%36,fix_strikeback=%37,fix_accurancy=%38,fix_dodge=%39,element_earth=%40,element_water=%41,element_fire=%42,"
			"element_wind=%43,type=%44,pos=%45   WHERE id=%46")
			.arg((int)char_id).arg(tmpObj->getObjectName())
			.arg(tmpObj->_realName).arg(tmpObj->_level).arg(tmpObj->_hp).arg(tmpObj->_maxhp).arg(tmpObj->_mp)\
			.arg(tmpObj->_maxmp).arg(tmpObj->_xp).arg(tmpObj->_maxxp).arg(tmpObj->_loyality).arg(tmpObj->_health).arg(tmpObj->_state)
			.arg(tmpObj->_skillslots).arg(tmpObj->_race).arg(tmpObj->_grade).arg(tmpObj->_lossMinGrade).arg(tmpObj->_lossMaxGrade)
			.arg(tmpObj->_points_remain).arg(tmpObj->_points_endurance)\
			.arg(tmpObj->_points_strength).arg(tmpObj->_points_defense).arg(tmpObj->_points_agility).arg(tmpObj->_points_magical).arg(tmpObj->_value_attack)\
			.arg(tmpObj->_value_defensive).arg(tmpObj->_value_agility).arg(tmpObj->_value_spirit).arg(tmpObj->_value_recovery).arg(tmpObj->_resist_poison)\
			.arg(tmpObj->_resist_sleep).arg(tmpObj->_resist_medusa).arg(tmpObj->_resist_drunk).arg(tmpObj->_resist_chaos).arg(tmpObj->_resist_forget)\
			.arg(tmpObj->_fix_critical).arg(tmpObj->_fix_strikeback).arg(tmpObj->_fix_accurancy).arg(tmpObj->_fix_dodge)
			.arg(tmpObj->_element_earth).arg(tmpObj->_element_water).arg(tmpObj->_element_fire).arg(tmpObj->_element_wind)
			.arg(tmpObj->getObjectType()).arg(tmpObj->_pos)
			.arg((int)tmpObj->getObjectID());
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
			.arg((int)char_id)
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
			.arg((int)tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	else if (GETDEVCLASS(objType) == TObject_GidRole)
	{
		QString szOwnCode;
		auto tmpObj = pObj.dynamicCast<ITGidRole>();
		QMutexLocker locker(&tmpObj->_mutex);

		strSql = QString("UPDATE character set gid='%1',type=%2,name='%3',level=%4,imageid=%5,sex=%6,gold=%7,bankgold=%8,xp=%9,\
				maxxp=%10,hp=%11,maxhp=%12,mp=%13,maxmp=%14,score=%15,job='%16',useTitle=%17,titles='%18',skillslots=%19,\
				manu_endurance=%20,manu_skillful=%21,manu_intelligence=%22,value_charisma=%23,points_endurance=%24,points_strength=%25,\
				points_defense=%26,points_agility=%27,points_magical=%28,value_attack=%29,value_defensive=%30,value_agility=%31,value_spirit=%32,\
				value_recovery=%33,resist_poison=%34,resist_sleep=%35,resist_medusa=%36,resist_drunk=%37,resist_chaos=%38,resist_forget=%39,\
				fix_critical=%40,fix_strikeback=%41,fix_accurancy=%42,fix_dodge=%43,element_earth=%44,element_water=%45,element_fire=%46,\
				element_wind=%47,points_remain=%48,role_type=%49   WHERE id=%50")
			.arg(tmpObj->_gid).arg(tmpObj->_type).arg(tmpObj->getObjectName()).arg(tmpObj->_level).arg(tmpObj->_imageid).arg(tmpObj->_sex)
			.arg(tmpObj->_gold).arg(tmpObj->_bankgold).arg(tmpObj->_xp).arg(tmpObj->_maxxp).arg(tmpObj->_hp).arg(tmpObj->_maxhp).arg(tmpObj->_mp)\
			.arg(tmpObj->_maxmp).arg(tmpObj->_score).arg(tmpObj->_job).arg(tmpObj->_useTitle).arg(tmpObj->_titles.join("|")).arg(tmpObj->_skillslots)\
			.arg(tmpObj->_manu_endurance).arg(tmpObj->_manu_skillful).arg(tmpObj->_manu_intelligence).arg(tmpObj->_value_charisma).arg(tmpObj->_points_endurance)\
			.arg(tmpObj->_points_strength).arg(tmpObj->_points_defense).arg(tmpObj->_points_agility).arg(tmpObj->_points_magical).arg(tmpObj->_value_attack)\
			.arg(tmpObj->_value_defensive).arg(tmpObj->_value_agility).arg(tmpObj->_value_spirit).arg(tmpObj->_value_recovery).arg(tmpObj->_resist_poison)\
			.arg(tmpObj->_resist_sleep).arg(tmpObj->_resist_medusa).arg(tmpObj->_resist_drunk).arg(tmpObj->_resist_chaos).arg(tmpObj->_resist_forget)\
			.arg(tmpObj->_fix_critical).arg(tmpObj->_fix_strikeback).arg(tmpObj->_fix_accurancy).arg(tmpObj->_fix_dodge)
			.arg(tmpObj->_element_earth).arg(tmpObj->_element_water).arg(tmpObj->_element_fire).arg(tmpObj->_element_wind).arg(tmpObj->_points_remain)
			.arg(tmpObj->getObjectType())
			.arg((int)tmpObj->getObjectID());
		bret = m_dbconn->execSql(strSql);
	}
	qDebug() << strSql;
	return bret;
}

//????????????????????????????????? ????????????????????????????????????
QString ITObjectDataMgr::StoreServerItemData(const ::CGData::CGStoreItemRequest* pItem)
{
	if (!pItem)
		return "";

	bool bForceUpdate = false;
	//??????code ?????????code???????????????????????? ??????????????????
	auto localItem = m_codeForGameItem.value(pItem->item_id());
	if (localItem)
	{
		//????????? ??????????????? ???????????????
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
//????????????????????????????????? ????????????????????????????????????
void ITObjectDataMgr::StoreServerMapData(const QString& mapName, int mapNumber)
{
	bool bForceUpdate = false;
	//??????code ?????????code???????????????????????? ??????????????????
	auto localObj = m_numberForGameMap.value(mapNumber);
	if (localObj)
	{
		//??????????????? ???????????????
		if (!localObj->_bUpdate && bForceUpdate || (localObj->getObjectName() != mapName))
		{
			localObj->_mapNumber = mapNumber;
			localObj->setObjectName(mapName);
			localObj->_bUpdate = true;
			localObj->setEditStatus();
			qDebug() << "??????????????????????????????" << localObj->getObjectName() << " " << mapNumber;
		}
	}
	else
	{
		localObj = newOneObject(TObject_Map).dynamicCast<ITGameMap>();
		localObj->_mapNumber = mapNumber;
		localObj->setObjectName(mapName);
		localObj->_bUpdate = true;
		m_numberForGameMap.insert(mapNumber, localObj);
		qDebug() << "??????????????????????????????" << localObj->getObjectName() << " " << mapNumber;
	}
}

QString ITObjectDataMgr::GetCharacterSexText(int image_id)
{
	//qDebug() << "????????????id:" << image_id;
	if (image_id >= 100000 && image_id < 100250)
	{
		return "???";
	}
	else if (image_id >= 100250 && image_id <= 100423)
	{
		return "???";
	}
	else if (image_id >= 100525 && image_id <= 100630)
	{
		return "???";
	}
	else if (image_id >= 100650 && image_id < 100800)
	{
		return "???";
	}
	else if (image_id >= 100800 && image_id < 105250)
	{
		return "???";
	}
	else if (image_id >= 105250 && image_id < 106000)
	{
		return "???";
	}
	else if (image_id >= 106000 && image_id < 106250)
	{
		return "???";
	}
	else if (image_id >= 106250 && image_id < 106400)
	{
		return "???";
	}
	else if (image_id >= 106400 && image_id < 106600)
	{
		return "???";
	}
	else if (image_id >= 106600 && image_id < 106730)
	{
		return "???";
	}
	return "";

}

int ITObjectDataMgr::GetCharacterSex(int image_id)
{
	int boy = 0;
	int girl = 1;
	//qDebug() << "????????????id:" << image_id;
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
	if (sGid.isEmpty() || sCharacterName.isEmpty())
		return;
	//??????????????????gid
	ITAccountGidPtr pGid = m_idForAccountGid.value(sGid);
	if (pGid == nullptr)
	{
		QMutexLocker locker(&m_rpcGidMutex);
		if (m_idForAccountGid.value(sGid) == nullptr)
		{
			pGid = newOneObject(TObject_AccountGid).dynamicCast<ITAccountGid>();
			if (pGid)
			{
				m_idForAccountGid.insert(sGid, pGid);
			}
		}
	}
	QString sID = sGid + sCharacterName;
	int roleType = request->role_type();
	int roleObjectType = TObject_GidRole;
	if (roleType == 0)
	{
		roleObjectType = TObject_GidRoleLeft;
	}else if (roleType == 1)
	{
		roleObjectType = TObject_GidRoleRight;
	}
	auto pCharacter = m_idForAccountRole.value(sID);
	if (!pCharacter)
	{
		pCharacter = newOneObject(roleObjectType, pGid).dynamicCast<ITGidRole>();;
		m_idForAccountRole.insert(sID, pCharacter);
	}
	else
		pCharacter->setEditStatus();
	
	if (pCharacter->getObjectType() != roleObjectType)
	{
		pCharacter->setObjectType(roleObjectType);
		pCharacter->setEditStatus();
	}
	pCharacter->setObjectName(sCharacterName);
	pCharacter->_gid = QString::fromStdString(request->gid());
	pCharacter->_level = request->character_data().base_data().level();
	pCharacter->_hp = request->character_data().base_data().hp();
	pCharacter->_mp = request->character_data().base_data().mp();
	pCharacter->_maxhp = request->character_data().base_data().maxhp();
	pCharacter->_maxmp = request->character_data().base_data().maxmp();
	pCharacter->_xp = request->character_data().base_data().xp();
	pCharacter->_maxxp = request->character_data().base_data().maxxp();
	pCharacter->_health = request->character_data().base_data().health();
	pCharacter->_souls = request->character_data().souls();
	pCharacter->_gold = request->character_data().gold();
	pCharacter->_imageid = request->character_data().image_id();		//??????????????????id
	pCharacter->_sex = GetCharacterSex(pCharacter->_imageid);

	pCharacter->_score = request->character_data().score();				//?????????
	pCharacter->_skillslots = request->character_data().skillslots();			//?????????
	pCharacter->_useTitle = request->character_data().use_title();		//?????????????????????
	pCharacter->_avatar_id = request->character_data().avatar_id();
	pCharacter->_unitid = request->character_data().unitid();
	pCharacter->_petid = request->character_data().petid();				  //??????????????????id
	pCharacter->_petriding = request->character_data().petriding();			  //????????????
	pCharacter->_direction = request->character_data().direction();				  //???????????????
	pCharacter->_punchclock = request->character_data().punchclock();			  //??????
	pCharacter->_usingpunchclock = request->character_data().usingpunchclock();	  //???????????????
	pCharacter->_job = QString::fromStdString(request->character_data().job());				  //????????????
	{
		QMutexLocker locker(&pCharacter->_mutex);
		pCharacter->_titles.clear();

		for (int i = 0; i < request->character_data().titles_size(); ++i)
		{
			pCharacter->_titles.append(QString::fromStdString(request->character_data().titles(i)));
		}
	}

	pCharacter->_manu_endurance = request->character_data().manu_endurance();		  //??????
	pCharacter->_manu_skillful = request->character_data().manu_skillful();		  //??????
	pCharacter->_manu_intelligence = request->character_data().manu_intelligence();		  //??????
	pCharacter->_value_charisma = request->character_data().value_charisma();	  //??????
	pCharacter->_x = request->character_data().x();						  //????????????
	pCharacter->_y = request->character_data().y();
	pCharacter->_map_name = QString::fromStdString(request->character_data().map_name());				  //????????????
	pCharacter->_map_number = request->character_data().map_number();			  //????????????
	pCharacter->_server_line = request->character_data().server_line();		  //?????????????????????
	qDebug() << QString::fromStdString(request->gid()) << QString::fromStdString(request->character_name()) << request->character_data().level();
	pCharacter->_points_remain = request->character_data().detail().points_remain();
	pCharacter->_points_endurance = request->character_data().detail().points_endurance();
	pCharacter->_points_strength = request->character_data().detail().points_strength();
	pCharacter->_points_defense = request->character_data().detail().points_defense();
	pCharacter->_points_agility = request->character_data().detail().points_agility();
	pCharacter->_points_magical = request->character_data().detail().points_magical();
	pCharacter->_value_attack = request->character_data().detail().value_attack();
	pCharacter->_value_defensive = request->character_data().detail().value_defensive();
	pCharacter->_value_agility = request->character_data().detail().value_agility();
	pCharacter->_value_spirit = request->character_data().detail().value_spirit();
	pCharacter->_value_recovery = request->character_data().detail().value_recovery();
	pCharacter->_resist_poison = request->character_data().detail().resist_poison();
	pCharacter->_resist_sleep = request->character_data().detail().resist_sleep();
	pCharacter->_resist_medusa = request->character_data().detail().resist_medusa();
	pCharacter->_resist_drunk = request->character_data().detail().resist_drunk();
	pCharacter->_resist_chaos = request->character_data().detail().resist_chaos();
	pCharacter->_resist_forget = request->character_data().detail().resist_forget();
	pCharacter->_fix_critical = request->character_data().detail().fix_critical();
	pCharacter->_fix_strikeback = request->character_data().detail().fix_strikeback();
	pCharacter->_fix_accurancy = request->character_data().detail().fix_accurancy();
	pCharacter->_fix_dodge = request->character_data().detail().fix_dodge();
	pCharacter->_element_earth = request->character_data().detail().element_earth();
	pCharacter->_element_wind = request->character_data().detail().element_wind();
	pCharacter->_element_water = request->character_data().detail().element_water();
	pCharacter->_element_fire = request->character_data().detail().element_fire();
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
			skillPtr->_bExist = true;

			posExist.append(reqSkill.index());
		}		
	}
	//????????????
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
	//?????????????????????  ???????????? ????????????  ????????????
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
			else
				pItemPtr->setEditStatus();
			pItemPtr->_itemAttr = QString::fromStdString(reqItem.attr());
			pItemPtr->setObjectName(QString::fromStdString(reqItem.name()));
			pItemPtr->_itemCount = reqItem.count();
			pItemPtr->_itemPos = reqItem.pos();
			pItemPtr->_itemType = reqItem.type();	//???????????????????????????????????????Item??????
			pItemPtr->_bExist = true;
			pItemPtr->setObjectCode(reqItem.item_id());
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
				pCharacter->_petPosForPet.insert(reqPet.index(), petPtr);
			}
			else
				petPtr->setEditStatus();

			petPtr->setObjectName(QString::fromStdString(reqPet.base_data().name()));
			petPtr->_realName = QString::fromStdString(reqPet.real_name());

			//	petPtr->setObjectCode(reqPet.petnumber);
			petPtr->_level = reqPet.base_data().level();
			petPtr->_hp = reqPet.base_data().hp();
			petPtr->_mp = reqPet.base_data().mp();
			petPtr->_maxhp = reqPet.base_data().maxhp();
			petPtr->_maxmp = reqPet.base_data().maxmp();
			petPtr->_xp = reqPet.base_data().xp();
			petPtr->_maxxp = reqPet.base_data().maxxp();
			petPtr->_health = reqPet.base_data().health();
			//petPtr->_imageid = reqPet.image_id();		//??????????????????id
			petPtr->_skillslots = reqPet.skillslots();			//?????????
			petPtr->_loyality = reqPet.loyality();
			petPtr->_race = reqPet.race();
			petPtr->_state = reqPet.state();
			petPtr->_grade = reqPet.grade();
			petPtr->_lossMinGrade = reqPet.lossmingrade();
			petPtr->_lossMaxGrade = reqPet.lossmaxgrade();
			petPtr->_pos = reqPet.index();


			petPtr->_points_remain = reqPet.detail().points_remain();
			petPtr->_points_endurance = reqPet.detail().points_endurance();
			petPtr->_points_strength = reqPet.detail().points_strength();
			petPtr->_points_defense = reqPet.detail().points_defense();
			petPtr->_points_agility = reqPet.detail().points_agility();
			petPtr->_points_magical = reqPet.detail().points_magical();
			petPtr->_value_attack = reqPet.detail().value_attack();
			petPtr->_value_defensive = reqPet.detail().value_defensive();
			petPtr->_value_agility = reqPet.detail().value_agility();
			petPtr->_value_spirit = reqPet.detail().value_spirit();
			petPtr->_value_recovery = reqPet.detail().value_recovery();
			petPtr->_resist_poison = reqPet.detail().resist_poison();
			petPtr->_resist_sleep = reqPet.detail().resist_sleep();
			petPtr->_resist_medusa = reqPet.detail().resist_medusa();
			petPtr->_resist_drunk = reqPet.detail().resist_drunk();
			petPtr->_resist_chaos = reqPet.detail().resist_chaos();
			petPtr->_resist_forget = reqPet.detail().resist_forget();
			petPtr->_fix_critical = reqPet.detail().fix_critical();
			petPtr->_fix_strikeback = reqPet.detail().fix_strikeback();
			petPtr->_fix_accurancy = reqPet.detail().fix_accurancy();
			petPtr->_fix_dodge = reqPet.detail().fix_dodge();
			petPtr->_element_earth = reqPet.detail().element_earth();
			petPtr->_element_wind = reqPet.detail().element_wind();
			petPtr->_element_water = reqPet.detail().element_water();
			petPtr->_element_fire = reqPet.detail().element_fire();
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
					skillPtr->_bExist = true;
					petSkillPosExist.append(reqSkill.index());
				}
				//????????????
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
	//????????????
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
	if (sGid.isEmpty() || sCharacterName.isEmpty())
		return;
	int roleType = request->role_type();
	int roleObjectType = TObject_GidRole;
	if (roleType == 0)
	{
		roleObjectType = TObject_GidRoleLeft;
	}
	else if (roleType == 1)
	{
		roleObjectType = TObject_GidRoleRight;
	}
	QString sID = sGid + sCharacterName;
	auto pCharacter = m_idForAccountRole.value(sID);
	if (!pCharacter)
	{
		pCharacter = newOneObject(roleObjectType).dynamicCast<ITGidRole>();;
		m_idForAccountRole.insert(sID, pCharacter);
	}
	else
		pCharacter->setEditStatus();

	pCharacter->setObjectName(sCharacterName);
	pCharacter->_gid = QString::fromStdString(request->gid());
	pCharacter->_bankgold = request->gold();

	//?????????????????????  ???????????? ????????????  ????????????
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
			else
				pItemPtr->setEditStatus();
			pItemPtr->_itemAttr = QString::fromStdString(reqItem.attr());
			pItemPtr->setObjectName(QString::fromStdString(reqItem.name()));
			pItemPtr->_itemCount = reqItem.count();
			pItemPtr->_itemPos = reqItem.pos();
			pItemPtr->_itemType = reqItem.type();
			pItemPtr->_bExist = true;
			pItemPtr->setObjectCode(reqItem.item_id());
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
				pCharacter->_petPosForPet.insert(reqPet.index(), petPtr);
			}
			else
				petPtr->setEditStatus();

			petPtr->setObjectName(QString::fromStdString(reqPet.base_data().name()));
			petPtr->_realName = QString::fromStdString(reqPet.real_name());
			//	petPtr->setObjectCode(reqPet.petnumber);
			petPtr->_level = reqPet.base_data().level();
			petPtr->_hp = reqPet.base_data().hp();
			petPtr->_mp = reqPet.base_data().mp();
			petPtr->_maxhp = reqPet.base_data().maxhp();
			petPtr->_maxmp = reqPet.base_data().maxmp();
			petPtr->_xp = reqPet.base_data().xp();
			petPtr->_maxxp = reqPet.base_data().maxxp();
			petPtr->_health = reqPet.base_data().health();
			//petPtr->_imageid = reqPet.image_id();		//??????????????????id
			petPtr->_skillslots = reqPet.skillslots();			//?????????
			petPtr->_loyality = reqPet.loyality();
			petPtr->_race = reqPet.race();
			petPtr->_state = reqPet.state();
			petPtr->_grade = reqPet.grade();
			petPtr->_lossMinGrade = reqPet.lossmingrade();
			petPtr->_lossMaxGrade = reqPet.lossmaxgrade();
			petPtr->_pos = reqPet.index();


			petPtr->_points_remain = reqPet.detail().points_remain();
			petPtr->_points_endurance = reqPet.detail().points_endurance();
			petPtr->_points_strength = reqPet.detail().points_strength();
			petPtr->_points_defense = reqPet.detail().points_defense();
			petPtr->_points_agility = reqPet.detail().points_agility();
			petPtr->_points_magical = reqPet.detail().points_magical();
			petPtr->_value_attack = reqPet.detail().value_attack();
			petPtr->_value_defensive = reqPet.detail().value_defensive();
			petPtr->_value_agility = reqPet.detail().value_agility();
			petPtr->_value_spirit = reqPet.detail().value_spirit();
			petPtr->_value_recovery = reqPet.detail().value_recovery();
			petPtr->_resist_poison = reqPet.detail().resist_poison();
			petPtr->_resist_sleep = reqPet.detail().resist_sleep();
			petPtr->_resist_medusa = reqPet.detail().resist_medusa();
			petPtr->_resist_drunk = reqPet.detail().resist_drunk();
			petPtr->_resist_chaos = reqPet.detail().resist_chaos();
			petPtr->_resist_forget = reqPet.detail().resist_forget();
			petPtr->_fix_critical = reqPet.detail().fix_critical();
			petPtr->_fix_strikeback = reqPet.detail().fix_strikeback();
			petPtr->_fix_accurancy = reqPet.detail().fix_accurancy();
			petPtr->_fix_dodge = reqPet.detail().fix_dodge();
			petPtr->_element_earth = reqPet.detail().element_earth();
			petPtr->_element_wind = reqPet.detail().element_wind();
			petPtr->_element_water = reqPet.detail().element_water();
			petPtr->_element_fire = reqPet.detail().element_fire();
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

Status ITObjectDataMgr::SelectGidData(const ::CGData::SelectGidDataRequest* request, ::CGData::SelectGidDataResponse* response)
{
	QString sGid = QString::fromStdString(request->gid());
	int nRoleType = request->role_type();
	if (sGid.isEmpty())
	{
		return Status::OK;
	}
	int roleObjType = TObject_GidRole;
	if (nRoleType == 1)
	{
		roleObjType = TObject_GidRoleRight;
	}
	else
		roleObjType = TObject_GidRoleLeft;
	ITAccountGidPtr pGid = m_idForAccountGid.value(sGid);
	if (pGid)
	{
		response->set_gid(pGid->_userGid.toStdString());
		ITGidRolePtr pRole = nullptr;
		for (auto pTmpRole : pGid->GetAllChildObj())
		{
			if (pTmpRole->getObjectType() == roleObjType)
			{
				pRole =qSharedPointerDynamicCast<ITGidRole>(pTmpRole);
				break;
			}
		}
		if (pRole == nullptr)
		{
			roleObjType = TObject_GidRole;
			for (auto pTmpRole : pGid->GetAllChildObj())
			{
				if (pTmpRole->getObjectType() == roleObjType)
				{
					pRole = qSharedPointerDynamicCast<ITGidRole>(pTmpRole);
					break;
				}
			}
		}
		if (pRole)
		{
			auto pChar = response->character_data();
			response->set_character_name(pRole->getObjectName().toStdString());
			pChar.set_souls(pRole->_souls);
			pChar.set_level(pRole->_level);
			pChar.set_gold(pRole->_gold);
			pChar.set_map_name(pRole->_map_name.toStdString());
			pChar.set_map_number(pRole->_map_number);
			pChar.set_job(pRole->_job.toStdString());
			pChar.set_nick(pRole->_nickName.toStdString());
			pChar.set_bank_gold(pRole->_bankgold);
			pChar.set_image_id(pRole->_imageid);
			pChar.set_score(pRole->_score);
			pChar.set_skillslots(pRole->_skillslots);
			pChar.set_use_title(pRole->_useTitle);
			pChar.set_avatar_id(pRole->_avatar_id);
			pChar.set_unitid(pRole->_unitid);
			pChar.set_petid(pRole->_petid);
			pChar.set_petriding(pRole->_petriding);
			pChar.set_direction(pRole->_direction);
			pChar.set_punchclock(pRole->_punchclock);
			pChar.set_usingpunchclock(pRole->_usingpunchclock);
			for (auto tTitle : pRole->_titles)
			{
				pChar.add_titles(tTitle.toStdString());
			}
			pChar.set_manu_endurance(pRole->_manu_endurance);
			pChar.set_manu_skillful(pRole->_manu_skillful);
			pChar.set_manu_intelligence(pRole->_manu_intelligence);
			pChar.set_value_charisma(pRole->_value_charisma);
			pChar.set_x(pRole->_x);
			pChar.set_y(pRole->_y);
			pChar.set_server_line(pRole->_server_line);
			//pChar.set_battle_position(pRole->_battle_position);
			//pChar.set_battle_position(pRole->_battle_position);
			auto detailData = pChar.detail();
			detailData.set_points_remain(pRole->_points_remain);
			detailData.set_points_endurance(pRole->_points_endurance);
			detailData.set_points_strength(pRole->_points_strength);
			detailData.set_points_defense(pRole->_points_defense);
			detailData.set_points_agility(pRole->_points_agility);
			detailData.set_points_magical(pRole->_points_magical);
			detailData.set_value_attack(pRole->_value_attack);
			detailData.set_value_defensive(pRole->_value_defensive);
			detailData.set_value_agility(pRole->_value_agility);
			detailData.set_value_spirit(pRole->_value_spirit);
			detailData.set_value_recovery(pRole->_value_recovery);
			detailData.set_resist_poison(pRole->_resist_poison);
			detailData.set_resist_sleep(pRole->_resist_sleep);
			detailData.set_resist_medusa(pRole->_resist_medusa);
			detailData.set_resist_drunk(pRole->_resist_drunk);
			detailData.set_resist_chaos(pRole->_resist_chaos);
			detailData.set_resist_forget(pRole->_resist_forget);
			detailData.set_fix_critical(pRole->_fix_critical);
			detailData.set_fix_strikeback(pRole->_fix_strikeback);
			detailData.set_fix_accurancy(pRole->_fix_accurancy);
			detailData.set_fix_dodge(pRole->_fix_dodge);
			detailData.set_element_earth(pRole->_element_earth);
			detailData.set_element_water(pRole->_element_water);
			detailData.set_element_fire(pRole->_element_fire);
			detailData.set_element_wind(pRole->_element_wind);

			{
				auto pBaseData = pChar.base_data();
				pBaseData.set_hp(pRole->_hp);
				pBaseData.set_mp(pRole->_mp);
				pBaseData.set_name(pRole->getObjectName().toStdString());
				pBaseData.set_maxhp(pRole->_maxhp);
				pBaseData.set_maxmp(pRole->_maxmp);
				pBaseData.set_level(pRole->_level);
				pBaseData.set_xp(pRole->_xp);
				pBaseData.set_maxxp(pRole->_maxxp);
				pBaseData.set_health(pRole->_health);
			}
			auto persDescData = pChar.pers_desc();
			//persDescData.set_buyicon(pRole->bu)

			for (auto it=pRole->_petPosForPet.begin();it!=pRole->_petPosForPet.end();++it)
			{
				if (!it.value()->_bExist)
				{
					continue;
				}
				auto petData = response->add_pet_data();
				auto pBaseData = petData->base_data();
				pBaseData.set_hp(it.value()->_hp);
				pBaseData.set_mp(it.value()->_mp);
				pBaseData.set_name(it.value()->getObjectName().toStdString());
				pBaseData.set_maxhp(it.value()->_maxhp);
				pBaseData.set_maxmp(it.value()->_maxmp);
				pBaseData.set_level(it.value()->_level);
				pBaseData.set_xp(it.value()->_xp);
				pBaseData.set_maxxp(it.value()->_maxxp);
				pBaseData.set_health(it.value()->_health);
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
				petData->set_skillslots(it.value()->_skillslots);
			}
			for (auto it = pRole->_itemPosForPtr.begin(); it != pRole->_itemPosForPtr.end(); ++it)
			{
				if (!it.value()->_bExist)
				{
					continue;
				}
				auto itemData = response->add_items();
				//?????????????????????item?????????????????? ???????????????????????????
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
					//?????????????????? 
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
					//?????????????????? 
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
			{	//?????????????????? 
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
