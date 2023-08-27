#include "ITObject.h"
//#include "stdafx.h"

ITObject::ITObject()
{
	m_ullID = 0;
	m_nType = dtNone;
	m_parent = nullptr;
	m_nameMutex = new QMutex;
}

ITObject::ITObject(QString szName, int nObjType, quint64 ullID)
{
	m_ullID = ullID;
	m_szName = szName;
	m_nType = nObjType;
	m_parent = nullptr;
	m_nameMutex = new QMutex;
}

ITObject::~ITObject()
{
}

void ITObject::setObjectType(int nType)
{
	m_nType = nType;
}

void ITObject::setObjectID(quint64 nCode)
{
	m_ullID = nCode;
}

void ITObject::setObjectName(const QString& sName)
{
	QMutexLocker locker(m_nameMutex);
	m_szName = sName;
}

void ITObject::setObjectCode(int code)
{
	m_gameCode = code;
}

int ITObject::getObjectCode()
{
	return m_gameCode;
}

int ITObject::getObjectType(void)
{
	return m_nType;
}

quint64 ITObject::getObjectID(void)
{
	return m_ullID;
}

QString ITObject::getObjectName(void)
{
	QMutexLocker locker(m_nameMutex);
	return m_szName;
}

void ITObject::addChildObj(ITObjectPtr childObj)
{
	if (!m_childList.contains(childObj))
		m_childList.append(childObj);
}

void ITObject::removeChildObj(ITObjectPtr childObj)
{
	if (m_childList.contains(childObj))
		m_childList.removeOne(childObj);
}

void ITObject::clearAllChildObj()
{
	m_childList.clear();
}

ITGameGateMap::ITGameGateMap()
{
	setObjectType(TObject_GateMap);
}

ITGameGateMap::ITGameGateMap(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
}

ITGameGateMap::~ITGameGateMap()
{
}

ITGamePet::ITGamePet()
{
}

ITGamePet::ITGamePet(QString szName, int nType, quint64 ullID) :
	ITGameBaseData(szName, nType, ullID)

{
}

ITGamePet::~ITGamePet()
{
}

ITGameItem::ITGameItem()
{
}

ITGameItem::ITGameItem(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)

{
}

ITGameItem::~ITGameItem()
{
}

ITGameMap::ITGameMap(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
}

ITGameMap::~ITGameMap()
{
}

ITAccountGid::ITAccountGid()
{
}

ITAccountGid::ITAccountGid(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
}

ITAccountGid::~ITAccountGid()
{
}

ITAccount::ITAccount()
{
	//m_nType = TObject_Account;
}

ITAccount::ITAccount(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
}

ITAccount::~ITAccount()
{
}

ITAccountGidRoleRunConfig::ITAccountGidRoleRunConfig()
{
}

ITAccountGidRoleRunConfig::ITAccountGidRoleRunConfig(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
}

ITAccountGidRoleRunConfig::~ITAccountGidRoleRunConfig()
{
}

ITLoginGameType::ITLoginGameType()
{
}

ITLoginGameType::ITLoginGameType(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
}

ITLoginGameType::~ITLoginGameType()
{
}

ITGidRole::ITGidRole()
{
}

ITGidRole::ITGidRole(QString szName, int nType, quint64 ullID) :
	ITGameBaseData(szName, nType, ullID)
{
}

ITGidRole::~ITGidRole()
{
}

ITAccountIdentity::ITAccountIdentity()
{
}

ITAccountIdentity::ITAccountIdentity(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
}

ITAccountIdentity::~ITAccountIdentity()
{
}

ITGameBaseData::ITGameBaseData()
{

}

ITGameBaseData::ITGameBaseData(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{

}

ITGameBaseData::~ITGameBaseData()
{

}

ITGameSkill::ITGameSkill()
{

}

ITGameSkill::ITGameSkill(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{

}

ITGameSkill::~ITGameSkill()
{

}

ITCGPetPictorialBook::ITCGPetPictorialBook()
{
}

ITCGPetPictorialBook::ITCGPetPictorialBook(QString szName, int nType, quint64 ullID) :
		ITObject(szName, nType, ullID)
{
}

ITCGPetPictorialBook::~ITCGPetPictorialBook()
{
}
