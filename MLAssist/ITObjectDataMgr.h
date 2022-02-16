#ifndef ITObjectDataMgr_H
#define ITObjectDataMgr_H

#include "../ThirdPart/qmqtt/x32/release/include/QtMqtt/QMqttClient"
#include "GameCtrl.h"
#include "GlobalDefine.h"
#include "ITDatabaseConn.h"
#include "ITObject.h"
#include <QIcon>
#include <QMap>
#include <QObject>
#include <QTimer>
class ObjectModuleRegisty
{
public:
	static ObjectModuleRegisty &GetInstance();
	~ObjectModuleRegisty(void);

	void RegisterModuleFactory(int nObjType, ObjectModuleFactory *pFactory, QString sIconPath = "");
	QHash<int, ObjectModuleFactory *> moduleInfoList;
	ITObjectPtr CreateNewObject(int objType);

private:
	ObjectModuleRegisty(void) {}
	ObjectModuleRegisty(const ObjectModuleRegisty &);
	ObjectModuleRegisty &operator=(const ObjectModuleRegisty &);
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
	//查找 缓存游戏道具信息
	void StoreServerItemData(GameItemPtr pItem);
	//查找 缓存游戏地图信息
	void StoreServerMapData(const QString &mapName, int mapNumber);
	//查找 缓存游戏宠物图鉴信息
	void StoreServerPetData(CGA::cga_picbook_info_t &picBook);
	ITObjectPtr FindObject(quint64 objid);
	///数据库连接判断
	bool connectToDB(const QString &strDBType, const QString &strHostName, const QString &strDBName, const QString &strUser, const QString &strPwd);
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
	bool pingToDestination(const QString &strIp);
	bool reconnectDB();

	static void loadDataBaseInfo(ITObjectDataMgr *pThis);
	static void SaveDataThread(ITObjectDataMgr *pThis);
	static ITObjectDataMgr &getInstance(void);

	void DebugNavigationRoute(ITGameGateMapPtr pRoute, int nRoute, int &nTotalCost);
	void StatisticsRoute(ITRouteNodePtr pRoute);
	bool IsReachableTargetNavigationRoute(ITGameGateMapPtr pRoute, int sx, int sy, int tx, int ty, int tgtIndex);
	//bmp格式 直接解析地图的
	bool LoadOffLineMapImageData(int index, QImage &mapImage);
	ITGamePetPtr GetGamePetFromNumber(int number) { return m_numberForPet.value(number); }
	void addObjectToObjectList(ITObjectPtr obj) { m_pObjectList.append(obj); }

	//地图加速查询
	void insertMapNumForObj(int num, ITGameMapPtr obj) { m_numberForGameMap.insert(num, obj); }

	void AddNewSubscribe(const QStringList &subscribe);
	void PublishOneTopic(const QString &topic, const QString &msg);
	QStringList GetLastPublishMsg();


protected:
	bool LoadItems();
	bool LoadPets();
	bool LoadGateMaps();
	bool LoadMaps();
	bool isLoading(); //是否加载中
	bool updateOneDeviceToDB(ITObjectPtr pObj);
	bool insertOneDeviceToDB(ITObjectPtr pObj);
	bool deleteOneDeviceFromDB(ITObjectPtr pObj);
	bool insertNewDataToDB();
	bool updateDataForDB();
	bool deleteDataFromDB();
	QHash<int, ITObjectPtr> GetTableObjectHash(QString sTable) { return m_tableForObj.value("sTable"); }

	bool FindTargetNavigationEx(ITGameGateMapPtr cRoute, int tgtIndex, QList<int> traceRouteList);
	bool IsReachableTarget(int mapIndex, int sx, int sy, int tx, int ty, int &ncost);
	//dat格式 要解析原魔力数据
	bool LoadOffLineMapData(int index);

	bool CheckExistAlikeWrap(int mapIndex, int tgtMapIndex, int sx, int sy, int tx, int ty);
	ITGameGateMapPtr BackMapTrace(ITGameGateMapPtr node);
	ITRouteNodePtr BackMapTraceRouteNode(ITRouteNodePtr node);
	QPoint GetRandomSpaceOffLine(int mapIndex, int x, int y, int distance /*= 1*/);
	QMap<QString, QSharedPointer<CGPetPictorialBook> > LoadPetBook();

	static void NormalThread(ITObjectDataMgr* pThis);

signals:
	void signal_loadDataFini();
	void signal_mqttMsg(const QString &, const QString &);
	void signal_publishMqttMsg(const QString &, const QString &);
	

public slots:
	void doLoadDBInfoOver();
	void on_attachGame_sucess();
	void OnNotifyGetItemsInfo(GameItemList items);
	void OnNotifyGetPetsInfo(GamePetList pets);
	void updateLogStateChange();
	void brokerDisconnected();
	void OnRecvMessage(const QByteArray &message, const QMqttTopicName &topic);
	void OnCheckConnectMqtt();
	void OnMqttConnected();
	void OnSubscribeState(QMqttSubscription::SubscriptionState s);
	void on_publishMqttMsg(const QString &, const QString &);

private:
	ITDataBaseConnPtr m_dbconn;								//数据库连接对象
	QTimer m_mqttTimer;										//定时检查发布订阅客户端状态
	QMutex m_storeItemMutex;								//item缓存
	QMutex m_storeMapMutex;									//地图缓存锁
	bool m_bIsLoading = false;								//是否加载数据中
	bool m_bDBConnState;									//数据库连接状态  是否已连接数据库
	QString m_strDBName;									//当前数据库名称
	QString m_strDBIp;										//当前数据库连接地址
	QString m_strDBType;									//当前数据库类型
	QString m_strDBUser;									//当前数据库用户名
	QString m_strDBPwd;										//当前数据库密码
	ITObjectList m_pObjectList;								//所有数据
	ITObjectList m_pAddObjectList;							//新增数据
	ITObjectList m_pDelObjectList;							//删除数据
	tMapHead _mapHead;										//地图结构体
	QMap<int, QImage> m_mapIndexForData;					//地图index和数据映射
	std::vector<int> _vTileData;							//暂不使用 解析原dat地图数据用 地表信息
	std::map<int, int> _mapTileMap;							//地表数字映射，映射为tiled map中tile层的新id
	std::vector<int> _vObjectData;							//实际就是第二个tiled层
	std::map<int, int> _mapObjectMap;						//物件数字映射，映射为tiled map中object层的新id
	QHash<QString, QHash<int, ITObjectPtr> > m_tableForObj; //加速查询
	QHash<int, ITGameItemPtr> m_codeForGameItem;			//代码和道具映射
	QMap<int, ITGameMapPtr> m_numberForGameMap;				//地图编码和地图对象映射
	QHash<int, QList<int> > m_warpHash;						//地图以及可到达目标
	QHash<int, ITGamePetPtr> m_numberForPet;				//编号映射宠物
	ITRouteNodeList m_reachableRouteList;					//路由列表
	QMqttClient *m_client = nullptr;						//发布订阅客户端
	QStringList m_subscribeList;							//软件自身 订阅列表 每次连接断开后，订阅会清空，此处记录所有订阅，等下次连接成功，重新订阅
	QStringList m_customSubscribeList;						//脚本 订阅列表 每次连接断开后，订阅会清空，此处记录所有订阅，等下次连接成功，重新订阅
	QStringList m_retrySubscribes;							//连接成功后，重新订阅列表
	QMutex m_mqttMutex;										//mqtt信号量
	QList<QPair<quint64, QStringList > > m_recvPublishMsgCache; //收到的发布消息缓存
};
#endif
