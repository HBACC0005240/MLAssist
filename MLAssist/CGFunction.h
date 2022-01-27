#pragma once
#include "GameCtrl.h"
#include "GlobalDefine.h"
#include "UserDefComboBoxDlg.h"
#include "UserDefDialog.h"
#include <QFuture>
#include <QJsonArray>
#include <QJsonObject>
#include <QPoint>
#include <QTimer>
#include <vector>
using namespace std;

#define A_FIND_PATH std::vector<std::pair<int, int> >
class CGFunction : public QObject
{
	Q_OBJECT
public:
	static CGFunction *GetInstance();
	CGFunction();
	~CGFunction();

	void StopFun(); //停止
	bool IsStop() { return m_bStop; }
	//这些信号槽 跨线程用的，子线程调用，g_变量主线程调用
signals:
	void signal_stopScript(); //停止脚本运行
	void signal_startTimer();
	void signal_StartAutoEncounterEnemy();		 //开启自动遇敌
	void signal_startAutoEncounterEnemySucess(); //开启成功通知
	void signal_stopAutoEncounterEnemy();		 //自动遇敌停止
	void signal_userInputDialog(const QString &sMsg, const QString &sVal);
	void signal_userComboBoxDialog(const QString &sMsg, const QStringList &sVal);
	void signal_returnUserInputVal(const QVariant &nVal);

	//工作界面
	void signal_workStart();						//新一轮工作开始
	void signal_workEnd();							//一轮工作结束
	void signal_workThreadEnd(const QString &sMsg); //整个线程结束

	//跨场景寻路通知
	void signal_crossMapFini(const QString &msg);
	//
	//临时用
	void signal_updateFetchGid();
	//游戏登录
	void signal_loginGameServerEx(QString account, QString pwd, QString gid, int gametype, int rungame, int skipupdate);

public slots:
	void RestFun(); //重置
	void StartTimer();
	void OnStartAutoEncounterEnemyThread(); //开启自动遇敌线程
	void OnPopupUserInputDialog(const QString &sMsg, const QString &sVal);
	void OnPopupUserComboBoxDialog(const QString &sMsg, const QStringList &sVal);
	void OnNotifyChatMsg(int unitid, QString msg, int size, int color);
	void OnRecvTopicMsg(const QString &topicName, const QString &msg);

public:
	//登出
	void Logout();
	//登出服务器
	void LogoutServer();

	QVariant UserInputDialog(QString sMsg, QString sVal);
	QVariant UserComboBoxDialog(QString sMsg, QStringList sVal);
	void SetUseDefaultInput(bool bFlag) { m_bUserDlgUseDefault = bFlag; }

	/*************物品相关*************/

	//查找物品在物品栏的位置 第一个物品
	int FindItemPos(const QString &sItemName);
	QSharedPointer<CGA::cga_item_info_t> FindBagItem(const QString &sItemName);
	int FindItemPosFromCode(int code);
	QSharedPointer<CGA::cga_item_info_t> FindBagItemFromCode(int code);
	GameItemList GetGameItems();
	//获取装备栏里的物品，返回数组
	int FindEquipItemPos(const QString &sItemName);
	GameItemPtr FindEquipItem(const QString &sItemName);
	int FindEquipItemPosFromCode(int code);
	GameItemPtr FindEquipItemFromCode(int code);

	//查找银行空闲位置
	int FindBankEmptySlot(QString itemName = "", int maxCount = 0);

	//寻找背包中的空闲格子
	int FindInventoryEmptySlot(QString itemName = "", int maxCount = 0);

	//获取背包中的空闲格子数量
	int GetInventoryEmptySlotCount();

	//获取已用格子数量
	int GetBagUsedItemCount();

	//实时获取背包物品信息
	auto GetInventoryItems();

	//扔物品
	void ThrowItemID(int id);
	void ThrowItemName(const QString &name);
	void ThrowItemPos(int pos);
	void ThrowNotFullItemName(const QString &name, int nCount);

	//堆叠
	void PileItem(QString name, int count);

	//获取指定物品数量
	int GetItemCount(const QString &itemName);
	int GetItemPileCount(const QString &itemName);
	int GetBankItemCount(const QString &itemName);
	int GetBankItemPileCount(const QString &itemName);
	int GetAllItemCount(const QString &itemName);
	int GetAllItemPileCount(const QString &itemName);

	//解析物品耐久 存入pItem变量
	void ParseItemDurability(GameItemPtr pItem);
	void ParseItemDurabilityEx(QString sItemAttr, int &nCur, int &nMax);

	//获取物品耐久
	int GetItemDurability(const QString &name);
	int GetPosItemDurability(int pos);

	//解析购买物品列表
	QMap<QString, QVariant> ParseBuyStoreMsg(QString msg);

	//使用物品
	bool UseItem(const QString &itemName);
	bool UseItemPos(int npos);

	//装备
	void Equip(QString sName, int pos = -1);
	void EquipEx(int nCode, int pos = -1);

	void UnEquip(QString sName);
	void UnEquipEx(int nCode);
	void UnEquipPos(int pos);

	//压条
	bool Exchange(int x, int y, const QString &sName);
	bool ExchangeEx(int nDir, const QString &sName);

	//买物品
	bool Shopping(int index, int count);
	//扔钱
	bool DropGold(int nVal);
	//扔宠
	bool DropPet(const QString &petName);
	bool DropPetEx(int pos);
	//存钱
	bool DepositGold(int nVal);
	//存宠
	bool DepositPet(const QString &petName, bool bSensitive = true);
	//身上指定位置宠存银行
	bool DepositPetPos(int pos);
	//身上指定位置宠存银行指定位置
	bool DepositPetEx(int srcIndex, int bankIndex);

	//取钱
	bool WithdrawGold(int nVal);
	//取宠 精确匹配
	bool WithdrawPet(const QString &petName, bool bSensitive = true);
	bool WithdrawPetEx(int srcIndex, int bankIndex);
	//物品存银行
	bool SaveToBankOnce(QString itemName, int maxCount = 0);
	bool SaveToBankAll(QString itemName, int maxCount = 0);
	bool SavePosToBank(int pos);
	bool SaveDstItemCountToBank(QString itemName, int maxCount = 0);
	bool SaveBagAllItemsToBank();
	//银行取物品 物品名称或code 取的数量
	bool WithdrawItem(const QString &itemName, int count = 20);
	bool WithdrawItemAll(const QString &itemName, int count = 20);
	bool WithdrawItemAllEx();

	void SortBagItems(bool bFront = false);
	void SortBankItems(bool bFront = true);

	//获取料理item
	GameItemList GetMagicFoodItems();
	//获取血瓶ItemList
	GameItemList GetMedicamentFoodItems();
	/*************人物状态*************/

	//获取角色职业
	QString GetCharacterProfession(QString sJob = "");
	int GetCharacterRank(QString sJob = "");
	QString GetCharacterOathGroup(QString sJob = ""); //4转属组
	QString GetCharacterSex(int image_id);			  //性别
	CharacterPtr GetGameCharacter();

	//获取人物信息
	QVariant GetCharacterData(const QString &sType);

	//宠物信息
	int GetBattlePetData(const QString &sType, QString val = "", QString val2 = "");
	//获取下一级称号需要的数据
	QVariantMap GetNextTitleData(const QString &sTitle, int grade);
	//银行操作
	int BankOperation(const QString &sType);

	//取队伍信息
	QVariant GetTeamData(const QString &sType);

	int SetPetData(const QString &sType, QVariant petData, int petIndex = -1);
	GamePetPtr GetBattlePet();
	GamePetList GetGamePets();

	QSharedPointer<CGA::cga_picbook_info_s> GetPetCard(QString name);
	int GetGameServerLine();
	//通过好友名片，获取队长所在的游戏线路
	int GetFriendServerLine(QString name);
	//工作
	void Work(int index, int nSubIndex = 0, int nDelayTime = 6000, bool bImmediate = false);
	//工作 鉴定 挖掘 伐木 狩猎  修理 这些默认根据最高等级选subIndex的
	void WorkEx(const QString &skillName, const QString &itemName = "", int nDelayTime = 6000, bool bImmediate = false);
	//合成
	void AllCompound(QString name);

	//开始工作
	void StartWork(int workType, int nDelayTime = 6000, QString compoundName = "");
	//停止工作
	void StopWork();
	//合成
	void Compound(const QString &itemName, int nVal = 0);
	//鉴定所有
	void AssessAllItem();
	//获取工作状态
	int GetWorkState();
	//获取合成状态
	int GetCompoundState();
	//获取合成物品相关技能信息
	QSharedPointer<GameCompoundSkill> GetItemCraftInfo(const QString &compoundName);

	GameSkillList GetGameSkills();
	static void WorkThread(CGFunction *pThis);
	//获取人物技能index 未找到返回-1
	int FindPlayerSkill(const QString &skillName);
	//获取技能
	GameSkillPtr FindPlayerSkillEx(const QString &skillName);
	//学习技能
	bool LearnPlayerSkill(int x, int y);
	//忘记技能
	bool ForgetPlayerSkill(int x, int y, QString skillName);
	bool ForgetPlayerSkillEx(int d, QString skillName);

	//人物当前状态
	//播放表情动作index允许的值是0~17
	void Gesture(int index);

	//转向四周nDir方向  MOVE_DIRECTION 北1 东北2 3 4 5//0=右上 1=右 2=右下 3=下 4=左下 5=左 6=左上 7=上
	void TurnAbout(int nDir);
	void TurnAboutEx(int x, int y);
	//转向坐标方向
	void TurnAboutPointDir(int x, int y);
	//东南西北 东北 东南 西北 西南 north east south west northeast southeast northwest southwest
	void TurnAboutEx2(QString sDir);

	//拾取
	void PickupItem(int nDir);

	//获取队伍人数
	int GetTeammatesCount();

	//获取指定坐标周边没有人的坐标
	QList<QPoint> GetUnmannedMapUnitPosList(QPoint pos);
	//加入某个人的队伍
	bool AddTeammate(const QString &sName, int timeout = 60000);
	//离开队伍
	bool LeaveTeammate();

	//等待组队完成
	bool WaitTeammates(QStringList teammateNames);

	//等待组队完成
	bool WaitTeammatesEx(int nCount);

	//发起交易
	bool LaunchTrade(const QString &sName = "", const QString &myTradeData = "", const QString &sTradeData = "", int timeout = 3000);
	//等待交易
	bool WaitTrade(const QString &sName = "", const QString &myTradeData = "", const QString &sTradeData = "", int timeout = 3000);
	//等待交易对话框
	QSharedPointer<CGA::cga_trade_dialog_t> WaitTradeDialog(int timeout = 10000);
	bool TradeInternal(const QString &sName = "", const QString &myTradeData = "", const QString &sTradeData = "", int timeout = 3000);
	//等待交易消息
	bool WaitTradeMsg();
	//等待交易物品
	QSharedPointer<GameTradeStuff> WaitTradeStuffs(bool *bRet = nullptr, int timeout = 3000);
	//包裹物品全部放入交易框

	//等待交易状态
	int WaitTradeState(int timeout = 10000);
	//指定方向坐标 方向 距离
	QPoint GetDirectionPos(int nDir, int nVal = 1);
	QPoint GetDirectionPosEx(QString sDir, int nVal = 1);
	QPoint GetCoordinateDirectionPos(int x, int y, int nDir, int nVal = 1);
	//是否队长
	bool IsTeamLeader(const QString &sName = "");

	/*************地图相关*************/

	//等待到达目标地图
	bool Nowhile(const QString &sName);
	bool NowhileEx(const QString &sName, int x, int y);
	bool NowhileMap(int mapNum, int timeout = 5000);
	bool NowhileMapEx(int mapNum, int x, int y, int timeout = 5000);
	//从地图名称中解析出当前楼层
	int GetNumberFromName(const QString &sMapName, bool bSerial = false, bool bBack = false);
	int GetMapFloorNumberFromName(bool bSerial = false, bool bBack = false);
	bool LoadOffLineMapImageData(int index, QImage &mapImage);
	//获取当前坐标
	QPoint GetMapCoordinate();

	//获取当前地图名称
	QString GetMapName();

	//获取地图编号
	int GetMapIndex();

	//移动到指定坐标 超时时间 秒
	bool MoveTo(int x, int y, int timeout = 10000);
	//根据路径寻路
	bool AutoNavigator(A_FIND_PATH path, bool isLoop = true);
	bool isMoveing() { return m_bMoveing; }
	//本地自动寻路
	int AutoMoveTo(int x, int y, int tomeout = 10000);
	int AutoMoveToEx(int x, int y, QString sMapName = "", int timeout = 10000);
	int AutoMoveToPath(std::vector<pair<int, int> > findPath, int timeout = 10000);
	int AutoMoveInternal(int x, int y, int timeout = 10000, bool isLoop = true);

	//跨地图寻路
	int AutoMoveToTgtMap(int tx, int ty, int tgtMapIndex, int timeout = 100);
	//寻找目标范围内传送石
	QList<QPoint> FindRandomEntryEx(int x, int y, int w, int h, QString filterPosList);
	QList<QPoint> FindRandomEntry(int x, int y, int w, int h, QList<QPoint> filterPosList);
	//获取迷宫遍历搜索路线
	QList<QPoint> FindRandomSearchPath(int tx, int ty);
	//地图全开
	QList<QPoint> MakeMapOpen();
	void MakeMapOpenEx();
	//下层寻路
	void MakeMapOpenContainNextEntrance(int isNearFar = 1);
	//1地图全开 2有2个迷宫出入口，并可达即可
	bool SearchAroundMapOpen(QList<QPoint> &allMoveAblePosList, int type = 1);
	bool SearchAroundMapUnit(QList<QPoint> &allMoveAblePosList, QString name, QPoint &findPos, QPoint &enterPos, QPoint &nextPos, int searchType = 1);
	//目标是否可达
	bool IsReachableTargetEx(int sx, int sy, int tx, int ty);
	bool IsReachableTarget(int tx, int ty);

	bool FindToRandomEntry(int x, int y, int w, int h, QList<QPoint> filterPosList);
	bool FindToRandomEntryEx(int x, int y, int w, int h, QString filterPosList);
	//搜索迷宫
	void SearchMaze(QString name);
	//移动到指定玩家NPC附近
	void MoveToNpc(const QString &npcName);
	void MoveToNpcEx(const QString &npcName, int nDir, int dis = 1);
	void MoveToNpcNear(int x, int y, int dis = 1);

	//获取目标坐标周围空闲坐标
	QPoint GetRandomSpace(int x, int y, int distance = 1, bool judgeReachTgt = false);
	QPoint GetRandomSpaceOffLine(QImage map, int x, int y, int distance = 1, bool judgeReachTgt = false);

	//搜索地图 名称 搜索物品或者npc或者玩家 0 1 2
	bool SearchMap(QString name, QPoint &findPos, QPoint &nextPos, int searchType = 1);
	bool SearchMapEx(QString name, QPoint &findPos, QPoint &nextPos, int searchType = 1);

	//合并坐标点，坐标点列表，合并距离10
	TSearchRectList MergePoint(QList<QPoint> posList, int nDis = 10);
	//获取可移动点列表
	QList<QPoint> GetMovablePoints(QPoint start);
	QList<QPoint> GetMovablePointsEx(QPoint start, int range = 10);
	bool FindByNextPoints(CGA::cga_map_cells_t &map, QPoint start, QList<QPoint> &foundedPoints, QRect tRect);

	QSharedPointer<CGA::cga_map_unit_t> FindMapUnit(const QString &name, int type = 1);

	//目标是否存在墙
	int IsTargetExistWall(int x, int y);

	//当前坐标是否在目标坐标一格范围内
	bool IsNearTarget(int x, int y, int dis = 1);
	bool IsNearTarget(int srcx, int srcy, int tgtx, int tgty, int dis = 1);

	//指定方向移动一格
	void MoveGo(int nDir);

	//获取一格(x,y)周围1x1区域内的空闲地形格子，并判断其方向
	int GetRandomSpaceDir(int x, int y);

	//获取目标在当前坐标方向
	int GetOrientation(int tx, int ty);

	//获取tx ty相对于xy的方向  xy为原点
	int GetDirection(int x, int y, int tx, int ty);

	//获取方向
	int GetDirectionEx(QPoint sPos, QPoint tPos);

	//获取方向对应的文本
	QString GetDirectionText(int nDirection);

	//获取当前坐标距目标坐标距离
	double GetDistance(int x, int y);
	double GetDistanceEx(int sx, int sy, int tx, int ty);
	double GetAStarDistance(int sx, int sy, int tx, int ty);

	//当前地图创建为image
	QImage CreateMapImage();
	bool CreateMapImage(QVector<short> &map, int &widgth, int &height);

	static void MoveToThread(CGFunction *pThis, int x, int y, int timeout);

	//穿越迷宫 默认地图已下载
	bool AutoWalkMaze(int isDownMap = 0, QString filterPosList = "", int isNearFar = 1);

	//穿越随机迷宫 检查下载 进行下载
	bool AutoWalkRandomMaze();
	bool AutoWalkRandomMazeEx();

	//设置目标迷宫名称
	void SetTargetMazeName(QString name) { m_sTargetMazeName = name; }
	void CelarTargetMazeName() { m_sTargetMazeName.clear(); }

	//获取距当前坐标最远的迷宫入口坐标点
	QPoint GetMazeEntrance(bool bNearFar = true);

	//获取当前迷宫所有入口点
	QList<QPoint> GetMazeEntranceList();
	//获取当前地图所有入口点
	QList<QPoint> GetMapEntranceList();
	int TransDirectionToCga(int nDir);

	//强制切图 遇敌重试，但一次在x或y上可同时最多移动5格,2格内不会触发npc战斗，超过会触发（比如熊男）
	bool ForceMoveTo(int x, int y, bool bShow = true);
	bool ForceMoveToEx(int nDirection, int nVal, bool bShow = true);

	//遇敌保护 停止自动遇敌
	static void AutoEncounterEnemyThread(CGFunction *pThis);

	//开启自动遇敌
	void begin_auto_action();
	void end_auto_action();
	void updateEndAutoAction();
	bool isEncounter() { return m_bAutoEncounterEnemy; }
	void waitEndAutoEncounterAction();
	void setAutoEncounterInterval(int nInterval) { m_nAutoEncounterEnemyInterval = nInterval; }
	void setAutoEncounterDir(int nDir) { m_nAutoEncounterDir = nDir; }
	void setAutoEncounterShowMove(bool bShow) { m_bIsShowAutoEncounterEnemy = bShow; }

	//计算路径
	A_FIND_PATH CalculatePath(int curX, int curY, int targetX, int targetY);
	A_FIND_PATH CalculatePathEx(QImage &mapImage, int sx, int sy, int tx, int ty);
	A_FIND_PATH CalculatePathEx2(QImage &mapImage, int sx, int sy, int tx, int ty);

	//当前所在地图是否已全部下载
	bool IsMapDownload();

	//下载当前所在地图数据
	bool DownloadMap();

	//下载当前地图指定大小数据
	bool DownloadMapEx(int xfrom, int yfrom, int x, int y);

	void JianDongXi(int nDirection);

	/*************查找附近NPC 物品等*************/

	//查找周边指定名称单位信息
	QSharedPointer<CGA_MapUnit_t> FindPlayerUnit(const QString &szName);

	//获取队伍成员信息
	QList<QSharedPointer<GameTeamPlayer> > GetTeamPlayers();

	//监听队友聊天信息
	void WaitTeammateChatSay();

	/*系统对话 */

	//回复血魔
	void Renew(int nDir);
	void RenewEx(int x, int y);
	void RenewEx2(QString sDir);

	//回复对话框选项
	bool RenewNpcClicked(QSharedPointer<CGA_NPCDialog_t> dlg);

	//判断是否需要补血
	bool NeedHPSupply(CGA::cga_player_info_t &pl);

	//是否需要补魔
	bool NeedMPSupply(CGA::cga_player_info_t &pl);

	//宠物是否需要补血魔
	bool NeedPetSupply(CGA::cga_pets_info_t &pets);

	//等待回补完成
	bool WaitSupplyFini(int timeout = 5);

	//等待区域地图更新消息
	QList<int> WaitRefreshMapRegion(int timeout = 5000);

	//等待接收NPC对话框
	QSharedPointer<CGA_NPCDialog_t> WaitRecvNpcDialog(int timeout = 5000);
	QSharedPointer<CGA::cga_working_result_t> WaitRecvWorkResult(int timeout = 9000); //
	QSharedPointer<CGA::cga_player_menu_items_t> WaitRecvPlayerMenu(int timeout = 5000);
	QSharedPointer<CGA::cga_unit_menu_items_t> WaitRecvPlayerMenuUnit(int timeout = 5000);

	int WaitRecvBattleAction(int timeout = 5000);
	int WaitRecvGameWndKeyDown(int timeout = 5000);
	int WaitRecvConnectionState(int timeout = 5000);
	//int WaitRecvServerShutdown();

	//监听系统消息
	std::tuple<int, QString> WaitSysMsg(int timeout = 5000);
	//监听聊天消息
	std::tuple<int, QString> WaitChatMsg(int timeout = 5000);
	//监听系统和聊天消息
	std::tuple<int, QString> WaitSysAndChatMsg(int timeout = 5000);
	std::tuple<QString, QString> WaitSubscribeMsg(int timeout = 5000);

	//需要卖物品
	bool NeedSale();

	/**************系统消息******************/

	//清除记录的系统消息列表
	void ClearSysCue()
	{
		QMutexLocker lock(&m_charMutex);
		m_systemCueList.clear();
		m_chatMsgList.clear();
	}
	//清除接收到的订阅信息
	void ClearTopicMsg()
	{
		QMutexLocker locker(&m_topicMutex);
		m_topicMsg.clear();
	}

	QList<QPair<QString, QString> > GetTopicMsgList() { return m_topicMsg; }
	QPair<QString, QString> GetLastTopicMsgList() { return m_topicMsg.last(); }
	//判断最后一个系统消息是否包含指定信息
	bool ContainSysCue(const QString &cue) { return m_systemCueList.last().contains(cue); }
	//判断聊天内容包含指定消息
	bool ContainChatMsg(const QString &cue);

	//获取系统提示内容
	QString GetSysChatMsg();
	QString GetLastSysChatMsg();
	//获取所有聊天消息
	QString GetAllChatMsg(int count = 0);
	QList<QPair<int, QString> > GetDetailAllChatMsg(int count = 0);
	//获取最新聊天消息
	QString GetLastChatMsg();

	//设置人物开关 队战聊名易家
	bool SetCharacterSwitch(int nType, bool bState);
	bool SetCharacterSwitchEx(QString type, bool bState);

	//获取人物状态 队战聊名易家
	int GetCharacterSwitch(int nType);

	//人物发起动作 加入队伍 交易等
	bool DoCharacterAction(int nType);

	//系统设置
	bool SysConfig(QVariant type, QVariant data1 = 0, QVariant data2 = 0);

	//喊话
	void Chat(const QString &sText, int v1, int v2, int v3);

	//返回当前系统时间 "黎明", "白天", "黄昏", "夜晚"
	QString getSysTimeEx();

	//返回当前系统时间+状态 18:54:23黄昏
	QString getSystemTime();

	//NPC对话回复选项
	void Npc(int option, int index = 0);

	//设置等待服务器返回数据类型
	void SetWaitRecv(int nType) { m_nWaitRecvType = nType; }

	//等待返回上次指定的头 否则阻塞在那
	QSharedPointer<CGA_NPCDialog_t> WaitRecvHead(int timeout = 10000);
	bool WaitInNormalState(int timeout = 10000);
	//等待战斗结束
	bool WaitBattleEnd(int timeout = 30000);

	//对话
	bool TalkNpc(int x, int y);
	bool TalkNpcEx(int dir);
	bool TalkNpcSelectYes(int x, int y, int count = 32);
	bool TalkNpcPosSelectYes(int x, int y, int count = 32);
	bool TalkNpcSelectYesEx(int dir, int count = 32);
	bool TalkNpcSelectNo(int x, int y, int count = 32);
	bool TalkNpcPosSelectNo(int x, int y, int count = 32);
	bool TalkNpcSelectNoEx(int dir, int count = 32);
	bool TalkNpcClicked(QSharedPointer<CGA_NPCDialog_t> dlg, int selectVal);

	//判断是否在战斗状态
	bool IsInBattle();

	//判断是否在正常状态（非切图非战斗状态）
	bool IsInNormalState();

	int GetGameStatus();
	int GetWorldStatus();
	//等待时间  毫秒可脚本随时终止
	void WaitTime(int timeout);
	//等待时间 毫秒
	void WaitTimeEx(int timeout);

	//游戏是否在线
	bool IsOnline();

	//卖东西
	bool Sale(int x, int y, const QString &itemName);
	bool SaleEx(int nDir, const QString &itemName);
	bool SaleEx2(QString sDir, const QString &itemName);
	//鉴定
	bool IdentifyItem(int x, int y, const QString &itemName);
	bool IdentifyItemEx(int nDir, const QString &itemName);
	bool IdentifyItemEx2(QString sDir, const QString &itemName);

	//急救
	bool FirstAid(const QString &name, const QString &subName, int lv = 0);

	//停止遇敌条件判断
	GameConditionCfg *GetStopEncounter(int type);
	//停止脚本判断
	GameConditionCfg *GetStopScriptCfg(int type);

	//判断是否需要停止自动遇敌
	bool IsNeedStopEncounter();
	bool IsNeedStopScript();

	//登录服务器
	bool LoginGameServerEx(QString account, QString pwd, QString gid = "", int gametype = 4, int rungame = 1, int skipupdate = 1);
	bool LoginGameServer(QString gid, QString glt, int serverid, int bigServerIndex, int serverIndex, int character);

	/**********宠物部分**************************/

	//宠物算档
	QStringList GetPetCalcBpData(GamePetPtr pet);
	QStringList GetPetCalcBpData(const CGA::cga_pet_info_t &pet);
	//血魔攻防敏
	QString GetPetCalcFiveAttribute(const CGA::cga_pet_info_t &pet, QString splitData = " ");
	//日志用格式血魔攻防敏 精神 回复 bp 体 力 防 敏 魔
	QString GetLogPetAttribute(const CGA::cga_pet_info_t &pet, QString splitData = " ");
	//返回用于算档的宠物数据(BP：血攻防敏魔回复精神+Bp)
	QString GetPetCalcBpAttribute(const CGA::cga_pet_info_t &pet, QString splitData = " ");
	QString GetPlayerAttribute(CGA::cga_player_info_t info, QString splitData = " ");
	//宠物修正
	QString GetPetFixVal(const CGA::cga_pet_info_t &pet, QString splitData = " ");
	QString GetPlayerFixVal(CGA::cga_player_info_t info, QString splitData = " ");

	bool RestPetState(int petindex, int petstate);

	//获取身上宠物数量
	int GetPetCount();

	//获取所有图鉴信息
	QList<CGPetPictorialBookPtr> GetAllPetPictorialBook();
	CGPetPictorialBookPtr GetPetPictorialBook(const QString &sName);
	bool SavePetPictorialBookToHtml(QString path = "", bool bInPic = false);

	/******** 界面UI配置等***************/

	//设置自动扔开关 state 0关 1开启    name不为空，则控制指定物品开关
	bool SetDropItemState(int state, QString name = "");
	bool SetPileItemState(int state, QString name = "");

	//采集账号信息
	QList<QStringList> GatherAccountInfo();
	void SetAccountGIDs(QStringList sGid) { m_sAccountGidList = sGid; }
	QStringList GetAccountGIDs() { return m_sAccountGidList; }

	//保存信息到
	bool SaveGatherAccountInfos();
	void GoBank();
	QString CreateHtmlRow(QVariantList sTextList);
	QString CreateHtmlRows(QList<QVariantList> sTextLists);
	QString CreateHtmlTable(QVariantList sTextList);

	//加载配置
	void ReadUserConfig(QString cfg);
	void WriteUserConfig(QString cfg);

	QString CreateRandomName(int sex);
	QString CreateBoyName();
	QString CreateGirlName();
	QVariant GetScriptUiSetData(const QString &type) { return m_scriptUiSetData.value(type); }
	void SetScriptUiSetData(const QString &type, QVariant data) { m_scriptUiSetData.insert(type, data); }
	QMap<QString, QVariant> GetScriptUiSet() { return m_scriptUiSetData; }
	void ClearScriptUiSetData() { m_scriptUiSetData.clear(); }

	QHash<QString, int> m_playerActionHash; //pk 加队 交易 摆摊映射
	QStringList m_sPrestigeList;			//玩家声望称号列表

protected:
	bool InternalAutoNavigator();
	bool readCreateRandomNameJson();
	bool readProfessionJson();
	bool readTitleJson();

private:
	QMap<QString, QVariantList> m_createRandomName;
	QJsonArray m_professions;							 //职业列表
	QJsonObject m_titles;								 //称号列表
	QStringList m_sAccountGidList;						 //游戏id列表 如果设置自动登录后 记录下来，后续脚本用
	QHash<QString, int> m_sysConfigMap;					 //自动设置映射
	bool m_bMapMoveing = false;							 //跨地图寻路中
	bool m_bMoveing = false;							 //正在移动中
	int m_navigatorLoopCount = 0;						 //当前寻路中递归次数
	QString m_sTargetMazeName;							 //自动迷宫目标名称
	bool m_bStop = false;								 //停止当前执行的函数
	QTimer m_resetTimer;								 //重置定时器
	int m_nAutoEncounterDir;							 //自动遇敌方向
	bool m_bAutoEncounterEnemy = false;					 //是否自动遇敌
	QMap<int, GameConditionCfg *> m_pStopEncounterJudge; //人物停止遇敌判断
	QMap<int, GameConditionCfg *> m_pStopScriptJudge;	 //脚本停止判断
	uint64_t m_uLastUseItemTime;						 //最后一次使用物品时间
	int m_nAutoEncounterEnemyInterval = 800;			 //自动遇敌间隔200-800
	bool m_bIsShowAutoEncounterEnemy = false;			 //自动遇敌是否显示移动
	QStringList m_systemCueList;						 //系统消息列表
	QList<QPair<int, QString> > m_chatMsgList;			 //聊天信息列表
	int m_nWaitRecvType;								 //等待服务器返回value1指定的头才进行下一句。
	bool m_bWorking = false;							 //生产工作中
	int m_nCurrentWorkType = TWork_None;				 //当前工作类型
	int m_nCurrentWorkSkillIndex = -1;					 //当前工作技能index
	int m_nCurrentWorkDelayTime = 6500;					 //当前工作延时
	QMap<int, QString> m_workTypeForText;				 //工作类型对应技能名
	QString m_sCurrentCompoundName;						 //合成技能名称
	QFuture<void> m_workFuture;							 //工作线程
	QMap<QString, int> m_characterMap;					 //人物开关映射
	QMap<int, QString> m_petRaceMap;					 //系统种族对应文本
	QFuture<void> m_encounterFuture;					 //遇敌线程
	bool m_bUserDlgUseDefault = false;					 //脚本 是否默认转换
	UserDefDialog *m_pUserDlg;							 //用户输入对话框
	UserDefComboBoxDlg *m_pUserComboBoxDlg;				 //用户下拉对话框
	QMap<QString, QVariant> m_scriptUiSetData;			 //脚本界面设置的数据
	QHash<QString, int> m_returnGameDataHash;			 //游戏数据返回映射
	bool m_tradeFinished = false;						 //交易状态
	QString m_saveTradeObjName;							 //交易对象名称
	bool m_getInTradeStuffs = false;					 //获取到交易物品
	QHash<int, QString> m_tradeState;					 //交易状态
	QMap<int, QVector<int> > m_boyOrGirl;				 //人物是男还是女
	tMapHead _mapHead;									 //地图结构体
	QMutex m_charMutex;									 //人物聊天信息锁
	QMutex m_topicMutex;								 //订阅信息锁
	QList<QPair<QString, QString> > m_topicMsg;			 //接收到的主题信息
};
#define g_pGameFun CGFunction::GetInstance()