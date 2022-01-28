#pragma once

#include "constDef.h"
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>

class ITObject;
class ITGameItem;
class ITGamePet;
class ITGameMap;
class ITGameGateMap;
class ITAccountIdentity;
class ITAccount;
class ITAccountGid;
class ITGidRole;
class ITCGPetPictorialBook;
class ITGameSkill;
typedef QSharedPointer<ITObject> ITObjectPtr;
typedef QList<ITObjectPtr> ITObjectList;

typedef QSharedPointer<ITGameItem> ITGameItemPtr;
typedef QList<ITGameItemPtr> ITGameItemList;

typedef QSharedPointer<ITGamePet> ITGamePetPtr;
typedef QList<ITGamePetPtr> ITGamePetList;

typedef QSharedPointer<ITCGPetPictorialBook> ITCGPetPictorialBookPtr;
typedef QList<ITCGPetPictorialBookPtr> ITCGPetPictorialBookList;

typedef QSharedPointer<ITGameMap> ITGameMapPtr;
typedef QList<ITGameMapPtr> ITGameMapList;

typedef QSharedPointer<ITGameGateMap> ITGameGateMapPtr;
typedef QList<ITGameGateMapPtr> ITGameGateMapList;

typedef QSharedPointer<ITAccountIdentity> ITAccountIdentityPtr;
typedef QList<ITAccountIdentityPtr> ITAccountIdentityList;

typedef QSharedPointer<ITAccount> ITAccountPtr;
typedef QList<ITAccountPtr> ITAccountList;

typedef QSharedPointer<ITAccountGid> ITAccountGidPtr;
typedef QList<ITAccountGidPtr> ITAccountGidList;

typedef QSharedPointer<ITGidRole> ITGidRolePtr;
typedef QList<ITGidRolePtr> ITGidRoleList;

typedef QSharedPointer<ITGameSkill> ITGameSkillPtr;
typedef QList<ITGameSkillPtr> ITGameSkillList;

Q_DECLARE_METATYPE(ITGameItemPtr)
Q_DECLARE_METATYPE(ITGamePetPtr)
Q_DECLARE_METATYPE(ITGameMapPtr)
Q_DECLARE_METATYPE(ITGameGateMapPtr)
Q_DECLARE_METATYPE(ITAccountPtr)
Q_DECLARE_METATYPE(ITAccountGidPtr)
Q_DECLARE_METATYPE(ITGidRolePtr)
Q_DECLARE_METATYPE(ITCGPetPictorialBook)
Q_DECLARE_METATYPE(ITGameSkillPtr)
enum TObjStatus //设备状态
{
	TStatus_Normal = 0, //正常
	TStatus_Add = 1,	//增加
	TStatus_Edit = 2,	//修改
	TStatus_Delete = 3, //删除
};

/*!
* @brief 工程模块工厂
*/
class ObjectModuleFactory
{
public:
	virtual ~ObjectModuleFactory(void) {}
	virtual ITObjectPtr Create() = 0;
};

#define DECLARE_OBJECT_MODULE_FACTORY(ModuleName)          \
	class ModuleName##Factory : public ObjectModuleFactory \
	{                                                      \
	public:                                                \
		virtual ITObjectPtr Create()                       \
		{                                                  \
			return ITObjectPtr(new ModuleName());          \
		}                                                  \
	};
#ifndef NEW_MODULE_FACTORY
#define NEW_MODULE_FACTORY(ModuleName) \
	(new ModuleName##Factory())
#endif // !NEW_MODULE_FACTORY

class ITObject
{
public:
	ITObject();
	ITObject(QString szName, int nObjType, quint64 ullID);
	virtual ~ITObject();
	//设置状态值
	int getStatus(void) { return m_nStatus; };
	void setAddStatus(void) { m_nStatus = TStatus_Add; }
	virtual void setDelStatus(void) { m_nStatus = TStatus_Delete; }
	void setEditStatus(void)
	{
		if (m_nStatus == TStatus_Normal) m_nStatus = TStatus_Edit;
	}
	void setNomalStatus(void) { m_nStatus = TStatus_Normal; }

	//设置属性值
	void setObjectType(int nType);
	void setObjectID(quint64 nCode);
	void setObjectName(const QString& sName);
	void setObjectCode(int code);

	//获取属性值
	int getObjectCode();
	int getObjectType(void);
	quint64 getObjectID(void);
	QString getObjectName(void);
	QString getObjectDesc(void) { return m_sObjDesc; }
	void setObjectDsec(QString strDesc) { m_sObjDesc = strDesc; }

	quint64 id64() { return m_ullID; }
	void setObjectParent(ITObjectPtr parent) { m_parent = parent; }
	ITObjectPtr getObjectParent() { return m_parent; }

	ITObjectList GetAllChildObj() { return m_childList; }
	void addChildObj(ITObjectPtr childObj);
	void removeChildObj(ITObjectPtr childDev);
	void clearAllChildObj();
	bool _bUpdate = false; //是否更新

private:
	int m_nType = TObject_None;		//Obj类型		状态值为enum类型ObjectType
	int m_nStatus = TStatus_Normal; //Obj修改状态
	quint64 m_ullID = 0;			//id
	int m_gameCode = 0;				//游戏代码
	QString m_szName;				//名称
	ITObjectPtr m_parent;			//父设备
	ITObjectList m_childList;		//子列表
	QString m_sObjDesc;				//描述
};
DECLARE_OBJECT_MODULE_FACTORY(ITObject)
class ITGameBaseData : public ITObject
{
public:
	ITGameBaseData();
	ITGameBaseData(QString szName, int nType, quint64 ullID);
	virtual ~ITGameBaseData();

	int _level = 0;
	int	_xp = 0;
	int _maxxp = 0;
	int _hp = 0;
	int _maxhp = 0;
	int _mp = 0;
	int _maxmp = 0;
	int _health = 0;
	int _points_remain = 0;
	int _points_endurance = 0;
	int _points_strength = 0;
	int _points_defense = 0;
	int _points_agility = 0;
	int _points_magical = 0;
	int _value_attack = 0;
	int _value_defensive = 0;
	int _value_agility = 0;
	int _value_spirit = 0;
	int _value_recovery = 0;
	int _resist_poison = 0;
	int _resist_sleep = 0;
	int _resist_medusa = 0;
	int _resist_drunk = 0;
	int _resist_chaos = 0;
	int _resist_forget = 0;
	int _fix_critical = 0;
	int _fix_strikeback = 0;
	int _fix_accurancy = 0;
	int _fix_dodge = 0;
	int _element_earth = 0;
	int _element_water = 0;
	int _element_fire = 0;
	int _element_wind = 0;
};
class ITGamePet : public ITGameBaseData
{
public:
	ITGamePet();
	ITGamePet(QString szName, int nType, quint64 ullID);
	virtual ~ITGamePet();

	quint64 _character_id = 0;//角色绑定关系
	QString _realName;		//宠物真实名称
	int _loyality = 0;		//忠诚
	int _petNumber = 0;	   //宠物编号
	int _state = 0;			//当前状态 战斗 待命 休息
	int _skillslots = 0;	//技能格
	int _race = 0;			//种族
	int _grade = 0;			//档次
	int _lossMinGrade = 0;	//最少掉档
	int _lossMaxGrade = 0;	//最多掉档
	int _pos = 0;
	bool _bExist = true;	//是否存在

	ITGameSkillList _skillList;
	QHash<int, ITGameSkillPtr> _skillPosForSkill;
};
DECLARE_OBJECT_MODULE_FACTORY(ITGamePet)

class ITGameItem : public ITObject
{
public:
	ITGameItem();
	ITGameItem(QString szName, int nType, quint64 ullID);
	virtual ~ITGameItem();

	int _itemCount = 0;		//物品数量
	int _itemType = 0;	   //道具类型
	int _itemPrice = 0;	   //贩店价格
	int _itemPile = 0;	   //最大叠加数
	int _itemLevel = 0;	   //物品等级
	int _sellMinCount = 0; //卖店最少数量
	int _itemPos = 0;		//物品位置
	QString _itemAttr;	   //物品描述
	bool _bExist = true;	//是否存在
};
DECLARE_OBJECT_MODULE_FACTORY(ITGameItem)

class ITGameMap : public ITObject
{
public:
	ITGameMap() {}
	ITGameMap(QString szName, int nType, quint64 ullID);
	virtual ~ITGameMap();

	int _mapNumber = 0; //地图编号
	int _oftenMap = 0;	//是否常用地图

	ITGameGateMapList m_pGateMapList;
};
DECLARE_OBJECT_MODULE_FACTORY(ITGameMap)

class ITGameGateMap : public ITObject
{
public:
	ITGameGateMap();
	ITGameGateMap(QString szName, int nType, quint64 ullID);
	virtual ~ITGameGateMap();

	int _x = 0;				  //传送点
	int _y = 0;				  //传送点
	int _tx = 0;			  //传送到达点
	int _ty = 0;			  //传送到达点
	QString _mapName;		  //当前地图名称
	int _mapNumber = 0;		  //当前地图编号
	QString _targetMapName;	  //目标地图名称
	int _targetMapNumber = 0; //目标地图编号
	int _warpType = 0;		  //0踩点传送 1点击传送 2对话选择传送
	QStringList _npcSelect;	  //对话选择传送内容 n个对话选项

	ITGameGateMapPtr _parent = nullptr;		//回溯用
	ITGameGateMapPtr _subGateMap = nullptr; //单条用
	ITGameGateMapList _navigationList;		//寻路用 临时数据
	int _nCost = 0;							//可达目标的距离成本
	int _nTotalCost = 0;					//如果为起始路线节点 存放总耗费
};

DECLARE_OBJECT_MODULE_FACTORY(ITGameGateMap)
class ITRouteNode;
typedef QSharedPointer<ITRouteNode> ITRouteNodePtr;
typedef QList<ITRouteNodePtr> ITRouteNodeList;

struct ITRouteNode
{
	ITRouteNodePtr parent = nullptr;
	ITRouteNodePtr next = nullptr;
	ITGameGateMapPtr cur = nullptr;
	int nTotalCost = 0;
};
class ITLoginGameType : public ITObject
{
public:
	ITLoginGameType();
	ITLoginGameType(QString szName, int nType, quint64 ullID);
	virtual ~ITLoginGameType();
};
//账号集合 按某个人设置
class ITAccountIdentity : public ITObject
{
public:
	ITAccountIdentity();
	ITAccountIdentity(QString szName, int nType, quint64 ullID);
	virtual ~ITAccountIdentity();

	QString _identity;	//身份证号
	int _sex;
};
DECLARE_OBJECT_MODULE_FACTORY(ITAccountIdentity)
//登录账号 电信-网通-时长-怀旧可能公用同一个
class ITAccount : public ITObject
{
public:
	ITAccount();
	ITAccount(QString szName, int nType, quint64 ullID);
	virtual ~ITAccount();

	QString _userName; //登录账号
	QString _passwd;   //登录密码
	QString _identity;	//身份证号
};
DECLARE_OBJECT_MODULE_FACTORY(ITAccount)

//游戏id
class ITAccountGid : public ITObject
{
public:
	ITAccountGid();
	ITAccountGid(QString szName, int nType, quint64 ullID);
	virtual ~ITAccountGid();

	QString _userGid; //gid账号
	ITGidRoleList _roleList;
};

DECLARE_OBJECT_MODULE_FACTORY(ITAccountGid)
//游戏角色
class ITGidRole : public ITGameBaseData
{
public:
	ITGidRole();
	ITGidRole(QString szName, int nType, quint64 ullID);
	virtual ~ITGidRole();

	QString _gid;
	int _type = 0;
	int _imageid = 0;
	int	_sex = 0;
	int	_gold = 0;
	int	_bankgold = 0;
	int _souls = 0;
	int _score = 0;
	QString	_job;
	int _useTitle = 0;
	QStringList	_titles;
	int _skillslots = 0;
	int _avatar_id = 0;
	int _unitid = 0;
	int _petid = 0;
	bool _petriding = false;
	int _direction = 0;
	int _punchclock = 0;
	bool _usingpunchclock = false;
	int _manu_endurance = 0;
	int _manu_skillful = 0;
	int _manu_intelligence = 0;
	int _value_charisma = 0;
	int _x = 0;
	int _y = 0;
	QString _map_name = "";
	int _map_number = 0;
	int _server_line = 0;

	ITGamePetList _petList;	  //所有宠物信息
	ITGameItemList _itemList; //所有道具信息
	ITGameSkillList _skillList;
	QHash<int, ITGamePetPtr> _petPosForPet;//宠物位置和指针映射
	QHash<int, ITGameSkillPtr> _skillPosForSkill;


	QHash<int, ITGameItemPtr> _itemPosForPtr;//物品id和物品指针映射
	QHash<QString, ITGameItemPtr> _bankItemNameForPtr;//银行物品名称和物品指针映射
	QMutex _mutex;
};

DECLARE_OBJECT_MODULE_FACTORY(ITGidRole)

class ITGameSkill :public ITObject
{
public:
	ITGameSkill();
	ITGameSkill(QString szName, int nType, quint64 ullID);
	virtual ~ITGameSkill();

	QString _info;			//技能介绍
	int _id = 0;				//技能id
	int _cost = 0;			//耗魔
	int _flags = 0;			//
	int _index = 0;			//技能位置
	int _level = 0;			//等级
	int _maxLevel = 0;		//最高等级
	bool _available = 0;		//是否可用
	int _xp = 0;
	int _maxxp = 0;
	bool _bExist = true;	//是否存在

	ITGameSkillList _subskills; //技能子项
};
DECLARE_OBJECT_MODULE_FACTORY(ITGameSkill)



//游戏子id配置
class ITAccountGidRoleRunConfig : public ITObject
{
public:
	ITAccountGidRoleRunConfig();
	ITAccountGidRoleRunConfig(QString szName, int nType, quint64 ullID);
	virtual ~ITAccountGidRoleRunConfig();

	int _serverLine;	 //几线 1-10
	QString _configPath; //配置路径名称
	QString _scriptPath; //脚本路径
};
DECLARE_OBJECT_MODULE_FACTORY(ITAccountGidRoleRunConfig)


class ITCGPetPictorialBook : public ITObject
{
public:
	int _petNumber = 0;	   //宠物编号
	int _petRaceType = 0;  //宠物种族
	QString _petRace;	   //宠物种族
	int _petBaseBp = 0;	   //成长系数
	int _growVigor = 0;	   //体力成长
	int _growStrength = 0; //力量成长
	int _growDefense = 0;  //防御成长
	int _growAgility = 0;  //敏捷成长
	int _growMagic = 0;	   //魔法成长
	int _canCatch = 0;	//0=不可捕捉，1=可以捕捉
	int _cardType = 0;	// 0 = 无，1 = 银卡，2 = 金卡
	int _imageId = 0;	//图片id
	int _skillCount = 0;	//技能格
	int _elementEarth = 0;	//地
	int _elementWater = 0;	//水
	int _elementFire = 0;	//火
	int _elementWind = 0;	//风

	int rate_endurance; //0x000C 0=半颗星，3=2颗星，5=3颗星，9=5颗星
	int rate_strength;	//0x0010 力量
	int rate_defense;	//0x0014 防御
	int rate_agility;	//0x0018 敏捷
	int rate_magical;	//0x001C 魔法

};
DECLARE_OBJECT_MODULE_FACTORY(ITCGPetPictorialBook)