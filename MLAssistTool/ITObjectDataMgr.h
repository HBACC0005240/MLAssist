#ifndef ITObjectDataMgr_H
#define ITObjectDataMgr_H

//#include "GlobalDefine.h"
#include "ITDatabaseConn.h"
#include "ITObject.h"
#include <QIcon>
#include <QMap>
#include <QObject>
#include <QTimer>
#include "RpcSocket.h"
#include <QMutex>
//#include "GameCtrl.h"

class ObjectModuleRegisty
{
public:
	static ObjectModuleRegisty& GetInstance();
	~ObjectModuleRegisty(void);

	void RegisterModuleFactory(int nObjType, ObjectModuleFactory* pFactory, QString sIconPath = "");
	QHash<int, ObjectModuleFactory*> moduleInfoList;
	ITObjectPtr CreateNewObject(int objType);

private:
	ObjectModuleRegisty(void) {}
	ObjectModuleRegisty(const ObjectModuleRegisty&);
	ObjectModuleRegisty& operator=(const ObjectModuleRegisty&);
};
#define g_objectModuleReg ObjectModuleRegisty::GetInstance()

class ITObjectDataMgr : public QObject
{
	Q_OBJECT
public:
	ITObjectDataMgr(void);

	~ITObjectDataMgr(void);

	bool init();
	bool Fini();
	void SetExitState(bool bVal) { m_bExit = bVal; }
	//查找部分
	//查找指定物品代码对应名称
	QString FindItemCodeName(int nCode);
	QString FindItemCodeNameFromDB(int nCode);
	ITGameItemPtr FindItemFromCode(int nCode);
	auto GetWarpMapHash() { return m_numberForGameMap; }
	auto GetWarpMapHashEx() { return m_warpHash; }

	//查找到到目的地的路由 测试，暂时用地图名寻
	ITGameGateMapList FindTargetNavigation(int tgtIndex, QPoint tgtPos);

	ITObjectList GetDstObjTypeList(int objType);
	ITObjectPtr FindObject(quint64 objid);
	///数据库连接判断
	bool connectToDB(const QString& strDBType, const QString& strHostName, const QString& strDBName, const QString& strUser, const QString& strPwd);
	bool getDBConnectState();
	bool refreshConnectToDB();
	///数据库操作部分
	///数据操作部分
	ITObjectPtr newOneObject(int type, ITObjectPtr pOwn = NULL);
	ITObjectPtr newOneObject(int devtype, quint64 devID); ///
	bool deleteOneObject(ITObjectPtr dev);
	quint64 getNewObjectID();

	//数据保存接口
	bool isNeedSaveData();
	bool saveAllData(QString strInfo = "");
	bool pingToDestination(const QString& strIp);
	bool reconnectDB();
	//更新缓存客户端发过来的数据
	QString StoreServerItemData(const ::CGData::CGStoreItemRequest* pItem);
	void StoreServerMapData(const QString& mapName, int mapNumber);
	void StoreUploadGidData(const ::CGData::UploadGidDataRequest* request);
	void StoreUploadGidBankData(const ::CGData::UploadGidBankDataRequest* request);

	static void loadDataBaseInfo(ITObjectDataMgr* pThis);
	static void SaveDataThread(ITObjectDataMgr* pThis);
	static ITObjectDataMgr& getInstance(void);

	bool LoadOffLineMapImageData(int index, QImage& mapImage);
	ITCGPetPictorialBookPtr GetGamePetFromNumber(int number) { return m_numberForPet.value(number); }

protected:
	bool LoadIdentification();
	bool LoadAccount();
	bool LoadAccountGid();
	bool LoadAccountRole();
	bool LoadGidItems();
	bool LoadGidPets();
	bool LoadGidSkills();
	bool LoadItems();
	bool LoadPets();
	bool LoadGateMaps();
	bool LoadMaps();

	bool updateOneDeviceToDB(ITObjectPtr pObj);
	bool insertOneDeviceToDB(ITObjectPtr pObj);
	bool deleteOneDeviceFromDB(ITObjectPtr pObj);
	bool insertNewDataToDB();
	bool updateDataForDB();
	bool deleteDataFromDB();
	QHash<int, ITObjectPtr> GetTableObjectHash(QString sTable) { return m_tableForObj.value("sTable"); }

	bool FindTargetNavigationEx(ITGameGateMapPtr cRoute, int tgtIndex, QList<int> traceRouteList);
	bool IsReachableTarget(int mapIndex, int sx, int sy, int tx, int ty, int& ncost);
	//dat格式 要解析原魔力数据
	bool LoadOffLineMapData(int index);


	ITGameGateMapPtr BackMapTrace(ITGameGateMapPtr node);
	ITRouteNodePtr BackMapTraceRouteNode(ITRouteNodePtr node);
	QPoint GetRandomSpaceOffLine(int mapIndex, int x, int y, int distance /*= 1*/);
	QString GetCharacterSexText(int image_id);
	int GetCharacterSex(int image_id);

signals:
	void signal_loadDataFini();
public slots:
	void doLoadDBInfoOver();

private:
	ITDataBaseConnPtr m_dbconn;
	bool m_bDBConnState;		   //数据库连接状态  是否已连接数据库
	QString m_strDBName;		   //当前数据库名称
	QString m_strDBIp;			   //当前数据库连接地址
	QString m_strDBType;		   //当前数据库类型
	QString m_strDBUser;		   //当前数据库用户名
	QString m_strDBPwd;			   //当前数据库密码
	ITObjectList m_pObjectList;	   //所有数据
	ITObjectList m_pAddObjectList; //新增数据
	ITObjectList m_pDelObjectList; //删除数据

	QMap<int, QImage> m_mapIndexForData; //地图index和数据映射
	//暂不使用 解析原dat地图数据用
	std::vector<int> _vTileData;	  // 地表信息
	std::map<int, int> _mapTileMap;	  // 地表数字映射，映射为tiled map中tile层的新id
	std::vector<int> _vObjectData;	  // 实际就是第二个tiled层
	std::map<int, int> _mapObjectMap; // 物件数字映射，映射为tiled map中object层的新id

	QHash<QString, QHash<int, ITObjectPtr> > m_tableForObj; //加速查询
	QHash<int, ITGameItemPtr> m_codeForGameItem;			//代码和道具映射
	QMap<int, ITGameMapPtr> m_numberForGameMap;				//地图编码和地图对象映射
	QHash<int, QList<int> > m_warpHash;						//地图以及可到达目标
	QHash<int, ITCGPetPictorialBookPtr> m_numberForPet;				//编号映射宠物


	QHash<QString, ITGidRolePtr> m_idForAccountRole;	//gid+name 对应指定游戏人物
	QHash<QString, ITAccountGidPtr> m_idForAccountGid;	//gid+name 对应指定gid
	ITRouteNodeList m_reachableRouteList;
	bool m_bExit = false;
	QMutex m_objMutex;
	QMutex m_rpcGidMutex;		
};
#endif
