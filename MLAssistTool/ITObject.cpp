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
	ITObject(szName, nType, ullID)

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

ITGameCharacter::ITGameCharacter()
{
}

ITGameCharacter::ITGameCharacter(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
}

ITGameCharacter::~ITGameCharacter()
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
	setObjectType(TObject_BaseData);
}

ITGameBaseData::ITGameBaseData(QString szName, int nType, quint64 ullID) :
	ITObject(szName, nType, ullID)
{
	setObjectType(TObject_BaseData);

}

ITGameBaseData::~ITGameBaseData()
{

}

bool ITGameBaseData::operator==(const ITGameBaseData& o) const
{
	return std::tie(char_id, _level, _xp, _maxxp, _hp, _maxhp, _mp, _maxmp, _health, _skillslots, _imageid) 
		== std::tie(o.char_id, o._level, o._xp, o._maxxp, o._hp, o._maxhp, o._mp, o._maxmp, o._health, o._skillslots, o._imageid);
}

bool ITGameBaseData::operator!=(const ITGameBaseData& o) const
{
	return std::tie(char_id, _level, _xp, _maxxp, _hp, _maxhp, _mp, _maxmp, _health, _skillslots, _imageid)
		!= std::tie(o.char_id, o._level, o._xp, o._maxxp, o._hp, o._maxhp, o._mp, o._maxmp, o._health, o._skillslots, o._imageid);
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

ITGameAttributeData::ITGameAttributeData()
{
	setObjectType(TObject_AttributeData);
}

ITGameAttributeData::ITGameAttributeData(QString szName, int nType, quint64 ullID)
	:ITObject(szName, nType, ullID)
{
	setObjectType(TObject_AttributeData);
}

ITGameAttributeData::~ITGameAttributeData()
{

}

bool ITGameAttributeData::operator==(const ITGameAttributeData& o) const
{
	return std::tie(_char_id,_points_remain, _points_endurance, _points_strength, _points_defense, \
		_points_agility, _points_magical, _value_attack, _value_defensive, _value_agility, _value_spirit,\
		_value_recovery, _resist_poison, _resist_sleep, _resist_medusa, _resist_drunk, _resist_chaos, \
		_resist_forget, _fix_critical, _fix_strikeback, _fix_accurancy, _fix_dodge, _element_earth, \
		_element_water, _element_fire, _element_wind, _manu_endurance, _manu_skillful, _manu_intelligence) 
		== std::tie(o._char_id,o._points_remain, o._points_endurance, o._points_strength, o._points_defense,\
			o._points_agility, o._points_magical, o._value_attack, o._value_defensive, o._value_agility,\
			o._value_spirit, o._value_recovery, o._resist_poison, o._resist_sleep, o._resist_medusa,\
			o._resist_drunk, o._resist_chaos, o._resist_forget, o._fix_critical, o._fix_strikeback, \
			o._fix_accurancy, o._fix_dodge, o._element_earth, o._element_water, o._element_fire,\
			o._element_wind, o._manu_endurance, o._manu_skillful, o._manu_intelligence);
}

bool ITGameAttributeData::operator!=(const ITGameAttributeData& o) const
{
	return std::tie(_char_id, _points_remain, _points_endurance, _points_strength, _points_defense, \
		_points_agility, _points_magical, _value_attack, _value_defensive, _value_agility, _value_spirit, \
		_value_recovery, _resist_poison, _resist_sleep, _resist_medusa, _resist_drunk, _resist_chaos, \
		_resist_forget, _fix_critical, _fix_strikeback, _fix_accurancy, _fix_dodge, _element_earth, \
		_element_water, _element_fire, _element_wind, _manu_endurance, _manu_skillful, _manu_intelligence)
		!= std::tie(o._char_id, o._points_remain, o._points_endurance, o._points_strength, o._points_defense, \
			o._points_agility, o._points_magical, o._value_attack, o._value_defensive, o._value_agility, \
			o._value_spirit, o._value_recovery, o._resist_poison, o._resist_sleep, o._resist_medusa, \
			o._resist_drunk, o._resist_chaos, o._resist_forget, o._fix_critical, o._fix_strikeback, \
			o._fix_accurancy, o._fix_dodge, o._element_earth, o._element_water, o._element_fire, \
			o._element_wind, o._manu_endurance, o._manu_skillful, o._manu_intelligence);
}
