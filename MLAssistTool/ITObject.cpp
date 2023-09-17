#include "ITObject.h"
//#include "stdafx.h"
#include "ITObjectDataMgr.h"
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

const int& ITObject::getObjectCode()const
{
	return m_gameCode;
}

const int& ITObject::getObjectType(void)const
{
	return m_nType;
}

const quint64 ITObject::getObjectID(void)const
{
	return m_ullID;
}

const QString ITObject::getObjectName(void)const
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

bool ITGameItem::operator==(const ITGameItem& o) const
{
	/*QString sName = getObjectName();
	int nCode = getObjectCode();
	QString sDesc = getObjectDesc();

	QString oName = o.getObjectName();
	int oCode = o.getObjectCode();
	QString oDesc = o.getObjectDesc();*/
	return std::tie(getObjectName(), getObjectCode(), getObjectDesc(), _itemCount, _itemType, _itemPrice, _itemPile, _itemLevel, _sellMinCount, _itemPos, _itemAttr, _bExist)
		== std::tie(o.getObjectName(), o.getObjectCode(), o.getObjectDesc(), o._itemCount, o._itemType, o._itemPrice, o._itemPile, o._itemLevel, o._sellMinCount, o._itemPos, o._itemAttr, o._bExist);
	/*return std::tie(sName, nCode, sDesc, _itemCount, _itemType, _itemPrice, _itemPile, _itemLevel, _sellMinCount, _itemPos, _itemAttr, _bExist)
		== std::tie(oName, oCode, oDesc, o._itemCount, o._itemType, o._itemPrice, o._itemPile, o._itemLevel, o._sellMinCount, o._itemPos, o._itemAttr, o._bExist);*/

}

bool ITGameItem::operator!=(const ITGameItem& o) const
{
	QString sName = getObjectName();
	int nCode = getObjectCode();
	QString sDesc = getObjectDesc();

	QString oName = o.getObjectName();
	int oCode = o.getObjectCode();
	QString oDesc = o.getObjectDesc();
	return std::tie(sName, nCode, sDesc, _itemCount, _itemType, _itemPrice, _itemPile, _itemLevel, _sellMinCount, _itemPos, _itemAttr, _bExist)
		!= std::tie(oName, oCode, oDesc, o._itemCount, o._itemType, o._itemPrice, o._itemPile, o._itemLevel, o._sellMinCount, o._itemPos, o._itemAttr, o._bExist);

}

ITGameItem& ITGameItem::operator=(const ITGameItem& o)
{
	if (this == &o)
		return *this;
	this->setObjectName(o.getObjectName());
	this->setObjectCode(o.getObjectCode());
	this->setObjectDsec(o.getObjectDesc());
	this->_itemCount = o._itemCount;
	this->_itemType = o._itemType;
	this->_itemPrice = o._itemPrice;
	this->_itemPile = o._itemPile;
	this->_itemLevel = o._itemLevel;
	this->_sellMinCount = o._sellMinCount;
	this->_itemPos = o._itemPos;
	this->_itemAttr = o._itemAttr;
	this->_bExist = o._bExist;
	return *this;
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

ITGameCharacter &ITGameCharacter::operator=(const ITGameCharacter &o)
{
	if (this == &o)
		return *this;
	this->setObjectName(o.getObjectName());
	this->setObjectCode(o.getObjectCode());
	this->setObjectDsec(o.getObjectDesc());
	//this->_info = o._info;
	//this->_id = o._id;
	//this->_cost = o._cost;
	//this->_flags = o._flags;
	//this->_index = o._index;
	//this->_level = o._level;
	//this->_maxLevel = o._maxLevel;
	//this->_available = o._available;
	//this->_xp = o._xp;
	//this->_maxxp = o._maxxp;
	//this->_bExist = o._bExist;
	return *this;
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

ITGameBaseData& ITGameBaseData::operator=(const ITGameBaseData &o) 
{
	if (this == &o)
		return *this;	
	char_id = o.char_id;
	_level = o._level;
	_xp = o._xp;
	_maxxp = o._maxxp;
	_hp = o._hp;
	_maxhp = o._maxhp;
	_mp = o._mp;
	_maxmp = o._maxmp;
	_health = o._health;
	_skillslots = o._skillslots;
	_imageid = o._imageid;			
	return *this;
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

bool ITGameSkill::operator==(const ITGameSkill& o) const
{
	return std::tie(getObjectName(), getObjectCode(), getObjectDesc(), _info, _id, _cost, _flags, _index, _level, _maxLevel, _available, _xp, _maxxp, _bExist)
		== std::tie(o.getObjectName(), o.getObjectCode(), o.getObjectDesc(), o._info, o._id, o._cost, o._flags, o._index, o._level, o._maxLevel, o._available, o._xp,o._maxxp,o._bExist);

}

bool ITGameSkill::operator!=(const ITGameSkill& o) const
{
	return std::tie(getObjectName(), getObjectCode(), getObjectDesc(), _info, _id, _cost, _flags, _index, _level, _maxLevel, _available, _xp, _maxxp, _bExist)
		!= std::tie(o.getObjectName(), o.getObjectCode(), o.getObjectDesc(), o._info, o._id, o._cost, o._flags, o._index, o._level, o._maxLevel, o._available, o._xp, o._maxxp,o._bExist);

}

ITGameSkill& ITGameSkill::operator=(const ITGameSkill& o)
{
	if (this == &o)
		return *this;
	this->setObjectName(o.getObjectName());
	this->setObjectCode(o.getObjectCode());
	this->setObjectDsec(o.getObjectDesc());
	this->_info = o._info;
	this->_id = o._id;
	this->_cost = o._cost;
	this->_flags = o._flags;
	this->_index = o._index;
	this->_level = o._level;
	this->_maxLevel = o._maxLevel;
	this->_available = o._available;
	this->_xp = o._xp;
	this->_maxxp = o._maxxp;
	this->_bExist = o._bExist;
	return *this;
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

ITGameAttributeData &ITGameAttributeData::operator=(const ITGameAttributeData &o)
{
	if (this == &o)
	{
		return *this;
	}
	this->_char_id = o._char_id;
	this->_points_remain = o._points_remain;
	this->_points_endurance = o._points_endurance;
	this->_points_strength = o._points_strength;
	this->_points_defense = o._points_defense;
	this->_points_agility = o._points_agility;
	this->_points_magical = o._points_magical;
	this->_value_attack = o._value_attack;
	this->_value_defensive = o._value_defensive;
	this->_value_agility = o._value_agility;
	this->_value_spirit = o._value_spirit;
	this->_value_recovery = o._value_recovery;
	this->_resist_poison = o._resist_poison;
	this->_resist_sleep = o._resist_sleep;
	this->_resist_medusa = o._resist_medusa;
	this->_resist_drunk = o._resist_drunk;
	this->_resist_chaos = o._resist_chaos;
	this->_resist_forget = o._resist_forget;
	this->_fix_critical = o._fix_critical;
	this->_fix_strikeback = o._fix_strikeback;
	this->_fix_accurancy = o._fix_accurancy;
	this->_fix_dodge = o._fix_dodge;
	this->_element_earth = o._element_earth;
	this->_element_water = o._element_water;
	this->_element_fire = o._element_fire;
	this->_element_wind = o._element_wind;
	this->_manu_endurance = o._manu_endurance;
	this->_manu_skillful = o._manu_skillful;
	this->_manu_intelligence = o._manu_intelligence;
	return *this;
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

ITObjectPtr ITGameServerType::getRoleObjFromRoleName(const QString &sCharName)
{
	if (_nameForObj.contains(sCharName))
	{
		return _nameForObj.value(sCharName);
	}
	return nullptr;
}

ITObjectPtr ITGameServerType::getGidObjFromGidName(const QString &sGid)
{
	if (_gidForObj.contains(sGid))
	{
		return _gidForObj.value(sGid);
	}
	return nullptr;
}

void ITGameServerType::appendObject(ITObjectPtr pObj)
{
	if (pObj == nullptr)
		return;
	if (GETDEVCLASS(pObj->getObjectType()) == TObject_Character)
	{
		//建立大区和游戏角色关系
		this->addChildObj(pObj);
		this->_nameForObj.insert(pObj->getObjectName(), pObj);
		auto tmpObj = pObj.dynamicCast<ITGameCharacter>();
		//建立
		auto pGidObj = this->getGidObjFromGidName(tmpObj->_gid).dynamicCast<ITAccountGid>();
		if (pGidObj == nullptr)
		{
			pGidObj = ITObjectDataMgr::getInstance().newOneObject(TObject_AccountGid).dynamicCast<ITAccountGid>();
			pGidObj->_userGid = tmpObj->_gid;
			pGidObj->_serverTypeID = this->getObjectID();
			pGidObj->setObjectParent(sharedFromThis());
			this->addChildObj(pGidObj);
			this->_gidForObj.insert(tmpObj->_gid, pGidObj);
		}
		pObj->setObjectParent(pGidObj);
		pGidObj->addChildObj(pObj);
	}
	else if (pObj->getObjectType() == TObject_AccountGid)
	{
		auto pGidObj = pObj.dynamicCast<ITAccountGid>();

		auto pExistGidObj = this->getGidObjFromGidName(pGidObj->_userGid);
		if (pExistGidObj == nullptr)
		{	
			pGidObj->setObjectParent(sharedFromThis());
			this->addChildObj(pGidObj);
			this->_gidForObj.insert(pGidObj->_userGid, pGidObj);
		}
		else if (pExistGidObj != pGidObj)//已存在 又来一个 删掉前面的 更新
		{
			ITObjectDataMgr::getInstance().deleteOneObject(pExistGidObj);
			pGidObj->setObjectParent(sharedFromThis());
			this->addChildObj(pGidObj);
			this->_gidForObj.insert(pGidObj->_userGid, pGidObj);
		}	
	}
}

void ITGameServerType::removeObject(ITObjectPtr pObj)
{
	if (pObj == nullptr)
		return;
	this->removeChildObj(pObj);
	if (GETDEVCLASS(pObj->getObjectType()) == TObject_Character)
	{
		auto pCharObj = pObj.dynamicCast<ITGameCharacter>();
		if (this->_nameForObj.contains(pCharObj->getObjectName()))
		{
			this->_nameForObj.remove(pCharObj->getObjectName());
		}
	}
	else if (pObj->getObjectType() == TObject_AccountGid)
	{
		auto pDelGidObj = pObj.dynamicCast<ITAccountGid>();
		auto pGidObj = this->getGidObjFromGidName(pDelGidObj->_userGid).dynamicCast<ITAccountGid>();
		if (pGidObj)
		{
			this->_gidForObj.remove(pGidObj->_userGid);
		}
	}
}
