#pragma once

#include "constDef.h"
#include <QList>
#include <QMap>
#include <QSharedPointer>

class ITObject;
class ITGameItem;
class ITGamePet;
class ITGameMap;
class ITGameGateMap;
class ITAccountAssemble;
class ITAccount;
class ITAccountGid;
class ITAccountRole;
typedef QSharedPointer<ITObject> ITObjectPtr;
typedef QList<ITObjectPtr> ITObjectList;

typedef QSharedPointer<ITGameItem> ITGameItemPtr;
typedef QList<ITGameItemPtr> ITGameItemList;

typedef QSharedPointer<ITGamePet> ITGamePetPtr;
typedef QList<ITGamePetPtr> ITGamePetList;

typedef QSharedPointer<ITGameMap> ITGameMapPtr;
typedef QList<ITGameMapPtr> ITGameMapList;

typedef QSharedPointer<ITGameGateMap> ITGameGateMapPtr;
typedef QList<ITGameGateMapPtr> ITGameGateMapList;

typedef QSharedPointer<ITAccountAssemble> ITAccountAssemblePtr;
typedef QList<ITAccountAssemblePtr> ITAccountAssembleList;

typedef QSharedPointer<ITAccount> ITAccountPtr;
typedef QList<ITAccountPtr> ITAccountList;

typedef QSharedPointer<ITAccountGid> ITAccountGidPtr;
typedef QList<ITAccountGidPtr> ITAccountGidList;

typedef QSharedPointer<ITAccountRole> ITAccountRolePtr;
typedef QList<ITAccountRolePtr> ITAccountRoleList;

Q_DECLARE_METATYPE(ITGameItemPtr)
Q_DECLARE_METATYPE(ITGamePetPtr)
Q_DECLARE_METATYPE(ITGameMapPtr)
Q_DECLARE_METATYPE(ITGameGateMapPtr)
Q_DECLARE_METATYPE(ITAccountPtr)
Q_DECLARE_METATYPE(ITAccountGidPtr)
Q_DECLARE_METATYPE(ITAccountRolePtr)
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
	void setObjectName(const QString &sName);
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

class ITGamePet : public ITObject
{
public:
	ITGamePet();
	ITGamePet(QString szName, int nType, quint64 ullID);
	virtual ~ITGamePet();

	int _petNumber = 0;	   //宠物编号
	int _petRaceType = 0;  //宠物种族
	QString _petRace;	   //宠物种族
	int _petBaseBp = 0;	   //成长系数
	int _growVigor = 0;	   //体力成长
	int _growStrength = 0; //力量成长
	int _growDefense = 0;  //防御成长
	int _growAgility = 0;  //敏捷成长
	int _growMagic = 0;	   //魔法成长
	int _canCatch = 0;
	int _cardType = 0;
	int _imageId = 0;
	int _skillCount = 0;
	int _elementEarth = 0;
	int _elementWater = 0;
	int _elementFire = 0;
	int _elementWind = 0;
};
DECLARE_OBJECT_MODULE_FACTORY(ITGamePet)

class ITGameItem : public ITObject
{
public:
	ITGameItem();
	ITGameItem(QString szName, int nType, quint64 ullID);
	virtual ~ITGameItem();

	int _itemType = 0;	   //道具类型
	int _itemPrice = 0;	   //贩店价格
	int _itemPile = 0;	   //最大叠加数
	int _itemLevel = 0;	   //物品等级
	int _sellMinCount = 0; //卖店最少数量
	QString _itemAttr;	   //物品描述
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
class ITAccountAssemble : public ITObject
{
public:
	ITAccountAssemble();
	ITAccountAssemble(QString szName, int nType, quint64 ullID);
	virtual ~ITAccountAssemble();

};
DECLARE_OBJECT_MODULE_FACTORY(ITAccountAssemble)
//登录账号 电信-网通-时长-怀旧可能公用同一个
class ITAccount : public ITObject
{
public:
	ITAccount();
	ITAccount(QString szName, int nType, quint64 ullID);
	virtual ~ITAccount();

	QString _userName; //登录账号
	QString _passwd;   //登录密码
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
	ITAccountRoleList _roleList;
};

DECLARE_OBJECT_MODULE_FACTORY(ITAccountGid)
//游戏角色
class ITAccountRole : public ITObject
{
public:
	ITAccountRole();
	ITAccountRole(QString szName, int nType, quint64 ullID);
	virtual ~ITAccountRole();

	ITGamePetList _petList;	  //所有宠物信息
	ITGameItemList _itemList; //所有道具信息
};

DECLARE_OBJECT_MODULE_FACTORY(ITAccountRole)

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
