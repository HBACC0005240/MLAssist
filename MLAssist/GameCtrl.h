#pragma once

#include "../CGALib/gameinterface.h"
#include "AutoBattle.h"
#include "BattleModuleRegisty.h"
#include "CGFunction.h"
#include "GameData.h"
#include "ITDataBaseConn.h"
#include "constDef.h"
#include <QHash>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QtConcurrent>

#ifdef _DEBUG
#pragma comment(lib, "..\\lib\\CGALibd.lib")
#else
#pragma comment(lib, "..\\lib\\CGALib.lib")
#endif // DEBUG
#include "GlobalDefine.h"
extern CGA::CGAInterface *g_CGAInterface;
class GameCtrl : public QObject
{
	Q_OBJECT

public:
	GameCtrl();
	~GameCtrl();
	static GameCtrl *getInstace();

	//初始化命令行解析
	void InitCmdParse();
	//运行解析的命令
	void RunParseCmd();
	//数据库
	//通过code查找物品名称
	QString FindItemCodeName(int nCode);
	//游戏进程
	void setGameProcess(DWORD pid) { m_gameProcessID = pid; }
	void setGameHwnd(HWND pHwnd) { m_gameHwnd = pHwnd; }
	void setGameBaseAddr(ULONG addr) { m_gameBaseAddr = addr; }
	void SetGamePort(int port) { m_nGamePort = port; }
	void SetGameThreadID(quint32 thID) { m_nGameThreadID = thID; }
	void SetGameCGAMutex(HANDLE mutex) { m_hGameMutex = mutex; }
	void SetSelfHttpServerPort(int port) { m_nSelfHttpServerPort = port; }
	void SetCreateLog(bool bFlag) { m_bCreateLog = bFlag; }
	void SetCGGameInstallPath(const QString &sPath) { m_sCGInstallPath = sPath; }

	void Disconnect();
	quint32 GetGameThreadID() { return m_nGameThreadID; }
	int GetGamePort() { return m_nGamePort; }
	DWORD getGamePID() { return m_gameProcessID; }
	HWND getGameHwnd() { return m_gameHwnd; }
	ULONG getGameBaseAddr() { return m_gameBaseAddr; }
	HANDLE GetGameCGAMutex() { return m_hGameMutex; }
	int GetSelfHttpServerPort() { return m_nSelfHttpServerPort; }
	bool GetCreateLog() { return m_bCreateLog; }
	QString GetCGGameInstallPath() { return m_sCGInstallPath; }

	void SetGameGid(const QString &gid);
	QString GetGameGid();

	//js脚本支持
	void HttpGetGameProcInfo(QJsonDocument *doc);

	void KillGameWndProcess();

	//是否退出游戏
	void SetExitGame(bool bRet) { m_bExit = bRet; }
	bool GetExitGame() { return m_bExit; }

	//获取信息
	QSharedPointer<Character> getGameCharacter() { return m_pGameCharacter; }  //获取人物信息
	GameSkillList getGameSkills() { return m_pGameSkills; }					   //获取人物技能信息
	GamePetList getGamePets() { return m_pGamePets; }						   //获取人物宠物信息
	GameItemList getGameItems() { return m_pGameItems; }					   //获取人物物品信息
	GameFirstAidCfg *getGameFirstAidCfg() { return m_pFirstAidCfg; }		   //获取人物急救设置
	GameHealCfg *getGameHealCfg() { return m_pHealCfg; }					   //获取人物治疗设置
	GameBattlePetCfg *getGameBattlePetCfg() { return m_pBattlePetCfg; }		   //获取出战宠物设置
	GameCompoundList getGameCompounds() { return m_pCompoundList; }			   //获取人物可以合成物品列表
	GameCfgBase *getGameTransformationCfg() { return m_pTransformation; }	   //获取人物变身配置
	GameCfgBase *getGameCosplayCfg() { return m_pCosplay; }					   //获取人物变装配置
	GameCfgBase *getGamePetRidingCfg() { return m_pAutoPetRiding; }			   //获取人物骑乘配置
	GamePickCfg *GetGamePickItemCfg() { return &m_bAutoPickCfg; }			   //获取人物自动捡物配置
	GameUpgradeCfg *GetUpgradePetCfg() { return m_upgradePetCfg; }			   //获取宠物升级自动加点配置
	GamePlayerUpgradeCfg *GetUpgradePlayerCfg() { return m_upgradePlayerCfg; } //获取人物升级自动加点配置

	//获取最后一次战斗己方宠物平均等级
	int GetLastBattlePetAvgLv();
	void RestLastBattlePetAvgLv();
	bool IsInBattle() { return m_isInBattle; }

	int GetLastBattlePetLv(int type);
	void RestLastBattlePetLv(int type);
	//设置自动扔宠
	void SetDropPetItemChecked(int nType, int state);	//设置扔宠判断项
	void SetDropPetItemValue(int nType, QVariant nVal); //设置扔宠判断值
	int GetPetPosition(int playerPos);

	//附加窗口句柄
	bool getGameConnectState(); //获取游戏连接状态 true连接有效 false 连接断开
	QString getGameState();		//获取游戏状态
	void GetBattleUnits();		//获取战斗信息
	//设置脚本运行状态
	void SetScriptRunState(int state);
	int GetScriptRunState() { return m_nRunScriptState; }
	void LoadConfig(const QString &path);
	void SaveConfig(const QString &path);
	int GetScriptDelayTime() { return m_nScriptDelayTime; }
	//线程定时器
	void StartUpdateTimer();
	void StopUpdateTimer();

	//重置游戏回调过来的连接状态信息
	void ResetGameConnectState();
	//获取登录游戏连接状态
	void GetLoginGameConnectState(int &state, QString &msg);

	//对话框
	bool HasNpcDlg() { return m_bHasNPCDlg; }

	GameItemList GetRenItemHash() { return m_pRenItemList; }
	GameItemList GetDieItemHash() { return m_pPileItemList; }
	GameItemList GetSaleItemList() { return m_pSaleItemList; }
	GameItemList GetPickItemList() { return m_pPickItemList; }

	GameSearchList GetSearchList() { return m_pSearchList; }
	void setRenItemIsChecked(const QString &name, int nVal);
	void setSaleItemIsChecked(const QString &name, int nVal);
	void SetPickItemIsChecked(const QString &name, int nVal);

	GameItemPtr setDieItemIsChecked(const QString &name, int nVal);
	void setSearchItem(const QString &name, int nVal);
	GameConditionCfg *GetPlayerEatCfg(int type);
	GameEquipProtectCfg *GetPlayerEquipProtectCfg();

	bool RenItem(GameItemPtr pItem); //丢弃指定物品
	bool RenItems();				 //物品丢弃
	bool DieItems();				 //物品叠加
	bool OnEatItem();				 //吃东西
	bool DoSaleItems();				 //卖东西
	void ReadItemData();
	void ClearRenItems();
	void ClearDieItems();
	void ClearSaleItems();
	//战后

	bool AutoFirstAid();		   //自动急救
	bool AutoHeal();			   //自动治疗
	bool AutoTransformation();	   //自动变身
	bool AutoCosplay();			   //自动变装
	bool AutoPetRiding();		   //自动骑乘
	void OverTimeCheck();		   //道具防超时检测
	bool AutoSetBattlePet();	   //设置战斗宠物
	bool AutoDropPet();			   //自动丢弃宠物
	bool AutoPickItems();		   //自动捡物
	bool AutoUpgradeAddPoint();	   //自动加点
	bool IsRealUpdateUi();		   //是否实时刷新界面
	bool IsEnabledDisplayDataUi(); //是否实时刷新界面
	void ResetRecvNpcDlgState();   //重置接收对话框状态
	//装备保护
	bool AutoEquipProtect(); //装备保护

	//获取最新对话框
	QSharedPointer<CGA_NPCDialog_t> GetLastNpcDialog();
	QSharedPointer<CGA::cga_trade_dialog_t> GetLastTradeDialog(int timeInteral = 5000);
	QSharedPointer<CGA::cga_working_result_t> GetLastWorkResult();
	QSharedPointer<CGA::cga_player_menu_items_t> GetLastPlayerMenuResult();
	QSharedPointer<CGA::cga_unit_menu_items_t> GetLastUnitMenuResult();
	QSharedPointer<CGA::cga_conn_state_t> GetLastConnectStateResult();
	QList<QSharedPointer<CGA::cga_trade_dialog_t> > GetAllRecvTopicMsgList();
	int GetLastBattleActionResult();

	//启动游戏是否隐藏
	void SetStartGameHide(int val) { m_startGameHide = val; }
	int GetStartGameHide() { return m_startGameHide; }
	void SetFollowGamePos(int val) { m_followGamePos = val; }
	int GetFollowGamePos() { return m_followGamePos; }

	void SetStartGameRepeatedGidExit(bool val) { m_repeatedGidExit = val; }
	bool GetStartGameRepeatedGidExit() { return m_repeatedGidExit; }
	void SetIsOpenNetToMLAssistTool(bool val) { m_bOpenNetToMTool = val; }
	bool GetIsOpenNetToMLAssistTool() { return m_bOpenNetToMTool; }
	void SetIsOpenSyncMap(bool val) { m_bOpenSyncMap = val; }
	bool GetIsOpenSyncMap() { return m_bOpenSyncMap; }
	void SetAutoLoginInterval(int timeout) { m_loginWaitInterval = timeout; }
	int GetAutoLoginInterval() { return m_loginWaitInterval; }

	//是否实时刷新玩家信息
	bool IsRealUpdatePlayerUi() { return m_bRealUpdatePlayerUi; }

	//地图是否可见-
	void SetMapIsVisible(bool bvisible) { m_bMapIsVisible = bvisible; } //地图是否可见 不可见时 不刷新地图 降低cpu 内存
	quint32 nativeKeycode(Qt::Key key);									//windows快捷键值转qt
	quint32 qNativeKeycode(quint32 key);								//qt快捷键值转windows
	QMap<int, quint32> GetQuickKeyMap() { return m_quickKeyMap; }		//获取快捷键
	void SetQuickKey(int val, QString sInputKey);						//设置快捷键
	bool WaitThreadFini();												//等待线程退出
	static void GetCharacterDataThread(GameCtrl *pThis);				//获取人物数据线程
	static void GetItemDataThread(GameCtrl *pThis);						//获取物品信息线程
	static void DownloadMapThread(GameCtrl *pThis);						//下载地图线程
	static void NormalThread(GameCtrl *pThis);							//正常状态线程 自动治疗 自动急救 装备保护
	static void NotifyTimeoutThread(GameCtrl *pThis);					//回调超时检测线程

private:
	QVariant GetPetVal(CGA::cga_pet_info_t &pPet, int nVal); //获取宠物值
	template <typename TData>
	void RemoveTimeoutCache(QMutex &pMutex, TData &tmpCache);

signals:
	void signal_exit();																			 //退出辅助
	void signal_ctrl_app(int);																	 //控制辅助窗口状态
	void signal_gameWndList(CProcessItemList wndList);											 //游戏窗口列表
	void signal_gameIsOnline(int);																 //游戏状态改变
	void signal_clearUiInfo();																	 //清理界面ui信息
	void signal_attachGame();																	 //附加游戏发送 发送一次
	void signal_compoundChanged();																 //合成物品列表更新通知
	void signal_loadScript(const QString &sName);												 //加载脚本
	void signal_stopScriptRun();																 //停止脚本
	void signal_updateScriptRunLine(int num);													 //更新脚本行
	void signal_setUiScriptDesc(const QString &sDesc);											 //设置脚本描述
	void signal_updateTrayToolTip(const QString &text);											 //更新系统托盘提示
	void signal_activeGameFZ();																	 //激活辅助
	void signal_loadUserConfig(const QString &path);											 //加载用户配置
	void signal_saveUserConfig(const QString &path);											 //保存用户配置
	void signal_addOneChat(const QString &chat);												 //增加一个聊天信息
	void signal_addOneDebugMsg(const QString &msg);												 //调试信息
	void signal_addOneLogMsg(const QString &msg);												 //日志信息
	void signal_addOneScriptLogMsg(const QString &msg);											 //脚本日志信息
	void signal_swicthAccountGid(const QString &gid);											 //切换游戏子账号
	void signal_swicthCharacter(int);															 //切换角色 0左边 1右边
	void signal_loginGame();																	 //登录游戏
	void signal_runGameWnd();																	 //拉起游戏窗口
	void signal_switchAutoDrop(int);															 //切换自动扔
	void signal_switchAutoPile(int);															 //切换自动叠加
	void signal_addRenItem(GameItemPtr, bool bCode = false);									 //添加自动扔项
	void signal_addDieItem(GameItemPtr);														 //添加自动叠加项
	void signal_switchAutoSupplyUI(bool);														 //打开关闭自动回复
	void signal_switchAutoSaleUI(bool);															 //打开关闭自动卖
	void signal_addSaleItemUI(bool, QString);													 //增加自动售出项
	void signal_switchFoolowPickItemUI(bool);													 //打开关闭地图跟随捡物
	void signal_switchAutoPickItemUI(bool);														 //打开关闭自动捡物
	void signal_addPickItemUI(bool, QString);													 //添加自动捡物
	void signal_switchAutoBattleUI(bool);														 //打开关闭自动战斗
	void signal_switchAutoHightSpeedBattleUI(bool);												 //打开关闭高速
	void signal_switchEncounterSpeedUI(int);													 //打开关闭高速遇敌
	void signal_switchAutoEncounterUI(bool);													 //打开关闭自动遇敌
	void signal_setHightSpeedBattleDelayUI(int);												 //设置高速战斗延时
	void signal_setBattleDelayUI(int);															 //设置战斗延时
	void signal_switchAllEncounterEscapeUI(bool);												 //打开关闭遇敌全跑
	void signal_switchNoLvlEncounterEscapeUI(bool);												 //打开关闭无1级全跑
	void signal_switchEscapeUI(int, bool, QString);												 //打开关闭逃跑项
	void signal_switchNoPetDoubleActionUI(bool);												 //打开关闭无宠二动
	void signal_switchNoPetActionUI(int, bool);													 //设置无宠二动选项
	void signal_switchAutoEatUi(int, bool);														 //设置自动吃 深蓝、狗粮、时水
	void signal_switchAutoUseSkillUi(int, bool);												 //设置自动治疗 自动急救 装备保护
	void signal_setMoveSpeedUI(int);															 //设置高速走路速度
	void signal_setScriptStillRestartUI(int, int);												 //设置脚本坐标静止重启
	void signal_setScriptStopRestartUI(int, int);												 //设置脚本停止重启
	void signal_setScriptStopLogbackRestartUI(int);												 //设置脚本停止回城重启
	void signal_setScriptStillLogoutUI(int, int);												 //设置脚本坐标静止登出
	void signal_addRenItemScript(QString name, bool bChecked = false);							 //增加自动扔物品项
	void signal_addDieItemScript(QString name, bool bChecked = false);							 //增加自动叠加物品项
	void signal_addSaleItem(GameItemPtr);														 //增加自动售卖项
	void signal_moveToTarget(int x, int y);														 //移动到目的地
	void signal_load_navpath(QVector<quint32> path);											 //加载寻路路径
	void signal_saveLoginBat(int ntype);														 //保存登录bat
	void signal_addOneScriptInputVar(int, const QVariant &, const QVariant &, const QVariant &); //增加脚本设置的变量
	void signal_createCharaData(bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color,
			QString create_chara_points, QString create_chara_elements, QString create_chara_name);
	void signal_switchLoginData(QString gid = "", int servr = 0, int line = 0, int character = 0); //切换登录界面信息
	void NotifyLoginProgressStart();															   //自动登录启动中
	void NotifyLoginProgressEnd();																   //自动登录结束
	void NotifyTradeDialog(QSharedPointer<CGA::cga_trade_dialog_t> dlg);						   //NPC交易弹框回调发送信号
	void NotifyTradeStuffs(QSharedPointer<CGA::cga_trade_stuff_info_t> items);					   //NPC交易物品回调发送信号
	void NotifyTradeState(int state);															   //NPC交易状态回调发送信号
	void NotifyGameWndKeyDown(unsigned int keyVal);												   //按键回调
	void NotifyBattleAction(int flags);															   //战斗回调信号
	void NotifyChatMsg(int unitid, QString msg, int size, int color);							   //系统消息信号
	void NotifyNPCDialog(const QSharedPointer<CGA_NPCDialog_t> &dlg);							   //NPC弹框回调发送信号
																								   //	void NotifyNPCDialog(const CGA_NPCDialog_t &dlg); //NPC弹框回调发送信号
	void NotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
	void NotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y);
	void NotifyTeamInfo(const QList<QSharedPointer<GameTeamPlayer> > &);
	void NotifyRefreshMapRegion(int xbase, int ybase, int xtop, int ytop, int index3);
	void NotifyConnectionState(int state, QString msg);
	void NotifyPlayerMenu(QSharedPointer<CGA::cga_player_menu_items_t> menu);
	void NotifyUnitMenu(QSharedPointer<CGA::cga_unit_menu_items_t> menu);
	void NotifyWorkingResult(const QSharedPointer<CGA::cga_working_result_t> &msg);
	void DelaySendNpcDialog(QSharedPointer<CGA_NPCDialog_t> dlg); //暂时没用到 可以删除
	void NotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
	void NotifyAutoAttachProcess(quint32 pid, quint32 tid);
	void NotifyKillProcess();
	void NotifyFillAutoLogin(int game, QString user, QString pwd, QString gid,
			int bigserver, int server, int character,
			bool autologin, bool skipupdate, bool autochangeserver, bool autokillgame,
			bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color,
			QString create_chara_points, QString create_chara_elements, QString create_chara_name);
	void NotifyFillLoadScript(QString path, bool autorestart, bool freezestop, bool injuryprot, bool soulprot, int consolemaxlines, int logBackRestart, int transInput);
	void NotifyFillStaticSettings(int freezetime, int chatmaxlines);
	void NotifyGameSkillsInfo(GameSkillList skills);
	void NotifyGameItemsInfo(GameItemList items);
	void NotifyGamePetsInfo(GamePetList pets);
	void NotifyGameCharacterInfo(CharacterPtr charinfo);
	void NotifyGameCompoundInfo(GameCompoundList compound);
	void NotifyGameBattlesInfo(GameBattleUnitList compound);
	void signal_updateBattleHpMp(int hp, int maxhp, int mp, int maxmp);
	void HttpGetSettings(QJsonDocument &doc);
	void HttpLoadSettings(QString query, QByteArray postdata, QJsonDocument *doc);
	void HttpLoadAccount(QString query, QByteArray postdata, QJsonDocument *doc);
	void HttpLoadScript(QString query, QByteArray postdata, QJsonDocument *doc);

	void signal_battleStateEnd();

public slots:
	void OnGetCharacterData();													//获取人物信息
	void OnGetTeamData();														//获取队伍信息
	void OnGetMapData();														//获取地图信息
	void OnGetItemsData();														//获取人物物品信息
	void OnQueueDownloadMap();													//下载地图
	void OnNotifyGameWndKeyDown(unsigned int keyVal);							//按键回调
	void OnNotifyBattleAction(int flags);										//战斗回调通知
	void OnNotifyConnectionState(int state, QString msg);						//连接状态处理
	void NotifyChatMsgCallback(CGA::cga_chat_msg_t msg);						//聊天回调通知
	void NotifyNPCDialogCallback(CGA::cga_npc_dialog_t npcdlg);					//Npc弹框回调通知
	void NotifyDownloadMapCallback(CGA::cga_download_map_t down);				//地图下载回调
	void NotifyConnectionStateCallback(CGA::cga_conn_state_t msg);				//连接状态回调
	void NotifyUnitMenuCallback(CGA::cga_unit_menu_items_t items);				//菜单回调
	void NotifyPlayerMenuCallback(CGA::cga_player_menu_items_t items);			//人物菜单回调
	void NotifyWorkingResultCallback(CGA::cga_working_result_t msg);			//工作状态回调
	void NotifyServerShutdown(int port);										//游戏关闭回调
	void NotifyTradeStuffsCallback(CGA::cga_trade_stuff_info_t items);			//交易物品回调
	void NotifyTradeDialogCallback(CGA::cga_trade_dialog_t dlg);				//交易对话框回调
	void NotifyTradeStateCallback(int state);									//交易状态回调
	void NotifyBattleActionCallBack(int flags);									//战斗回调信号
	void OnNotifyNPCDialog(const QSharedPointer<CGA_NPCDialog_t> &dlg);			//处理弹框槽函数
	void OnNotifyPlayerMenu(QSharedPointer<CGA::cga_player_menu_items_t> menu); //吃药等人物选择菜单
	void OnNotifyUnitMenu(QSharedPointer<CGA::cga_unit_menu_items_t> menu);		//二级选择菜单
	void OnNotifyChatMsg(int unitid, QString msg, int size, int color);			//游戏聊天
	void OnSetSyncGameWindowFlag(int state);									//同步游戏窗口选项
	void OnSyncGameWindow(bool bFlags);											//是否打开同步游戏窗口
	void OnSetAutoSupply(int state);											//是否开启自动补血
	void OnSetAutoSale(int state);												//是否开启自动卖魔石
	void OnSetOverTimeT(int state);												//防超时被T
	void OnSetAutoRenItems(bool state);											//是否开启自动扔物品
	void OnSetAutoDieItems(bool state);											//是否开启自动叠加物品
	void OnSetAutoFirstAid(bool state);											//是否开启自动急救
	void OnSetAutoHeal(bool state);												//是否开启自动治疗
	void OnDownloadMap(int xsize, int ysize);									//下载地图
	void OnSetAutoDropPet(int state);											//是否开启自动扔宠
	void OnSetScirptDelayTime(int nTime);										//设置脚本每行执行时间
	void OnSetWorkDelayTime(int nTime);											//设置工作延时时间
	void OnSetFollowPickItem(int state);										//是否本地图跟随捡物
	void OnSetAutoPickItems(bool bFlag);										//是否自动捡物
	void OnSetAutoTalkNpc(bool state);											//自动对话
	void OnSetAutoTalkNpcYesOrNo(bool state);									//自动对话选是或否
	void OnSetRealUpdateUi(int state);											//是否实时刷新ui界面数据 主要是npc 玩家 和物品
	void OnSetRealUpdatePlayerUi(int state);									//是否实时刷新ui界面数据 主要是npc 玩家 和物品
	void OnEnableDataDisplayUi(int state);										//是否启用ui界面数据 主要是npc 玩家 和物品
	void OnSetAutoEatDeepBlue(int state);										//打开关闭自动吃深蓝
	void OnSetAutoEatTimeCrystal(int state);									//打开关闭自动吃时水
	void OnSetAutoEatDogFood(int state);										//打开关闭自动吃狗粮
	void OnSetAutoUploadBankData(int state);									//打开关闭自动上传银行数据

private:
	QMutex m_gidMutex;
	QString m_sGid;																			   //gid
	HANDLE m_hGameMutex;																	   //CGA附加到游戏上的锁
	DWORD m_gameProcessID = 0;																   //游戏进程
	HWND m_gameHwnd;																		   //游戏窗口句柄
	ULONG m_gameBaseAddr;																	   //游戏基址
	int m_nGamePort = 0;																	   //注册dll通讯端口
	int m_nSelfHttpServerPort = 0;															   //自身http服务通讯端口
	quint32 m_nGameThreadID;																   //线程ID
	bool m_bExit;																			   //退出游戏
	QTimer m_updateTimer;																	   //定时更新定时器
	QTimer m_characterTimer;																   //人物角色获取定时器
	QTimer m_mapTimer;																		   //地图信息获取定时器
	GameItemList m_pRenItemList;															   //扔物品列表
	GameItemList m_pPileItemList;															   //扔物品列表
	GameSearchList m_pSearchList;															   //搜索列表
	GameItemList m_pSaleItemList;															   //自动卖物品列表
	GameItemList m_pPickItemList;															   //自动捡物列表
	GameItemList m_pGameItems;																   //人物物品信息  0-7人物佩戴 8-27包裹 5左饰 0头 6右饰 2手 3手 1身 4脚 7水晶
	QSharedPointer<Character> m_pGameCharacter = nullptr;									   //当前游戏人物信息
	GamePetList m_pGamePets;																   //人物宠物信息
	GameSkillList m_pGameSkills;															   //人物技能信息
	GameCompoundList m_pCompoundList;														   //可以合成的物品列表 其他地方取对象 不要保存
	int m_nRunScriptState = SCRIPT_CTRL_STOP;												   //脚本运行状态
	int m_nMoveSpeed;																		   //移动速度
	int m_nWorkDelay = 6500;																   //工作延时
	int m_nWorkAcc = 100;																	   //工作
	bool m_bAutoSupply = true;																   //自动补血
	bool m_bAutoSale = true;																   //自动卖
	bool m_bOverTimeT = true;																   //道具防超时被T
	QTime m_nLastOverTime;																	   //道具超时检测时间
	QTime m_nLastAutoRenTime;																   //上次自动扔时间
	bool m_bAutoRenItems = true;															   //自动扔物品
	bool m_bAutoDieItems = true;															   //自动叠加物品
	bool m_bAutoFirstAid = true;															   //自动急救
	bool m_bAutoEatDeepBlue = false;														   //自动深蓝
	bool m_bAutoEatDogFood = false;															   //自动吃狗粮
	bool m_bAutoEatTimeCrystal = false;														   //自动吃时水
	GamePickCfg m_bAutoPickCfg;																   //自动捡物
	int m_nWorkType = TWork_None;															   //当前工作类型
	GameFirstAidCfg *m_pFirstAidCfg = nullptr;												   //急救设置
	GameHealCfg *m_pHealCfg = nullptr;														   //治疗设置
	GameCfgBase *m_pTransformation = nullptr;												   //变身设置
	GameCfgBase *m_pCosplay = nullptr;														   //变装设置
	GameCfgBase *m_pAutoPetRiding = nullptr;												   //骑乘
	bool m_bAutoDropPet = false;															   //自动扔宠
	QTime m_nLastAutoDropPetTime;															   //上次自动扔时间
	QMap<int, bool> m_bDropPetCheckItem;													   //自动扔宠检查项
	QMap<int, QVariant> m_nDropPetCheckVal;													   //自动扔宠检查值
	GameBattlePetCfg *m_pBattlePetCfg = nullptr;											   //出战宠物设置
	bool m_bHasNPCDlg = true;																   //是否有NPC对话框弹出 有True处理后false
	bool m_bAutoClickNpc = false;															   //自动点击NPC
	bool m_bAutoTalkNpcYesOrNo = true;														   //对话NPC选是或否
	bool m_bAutoUpLoadBankData = true;														   //自动上传银行信息
	GameUpgradeCfg *m_upgradePetCfg;														   //宠物升级加点
	GamePlayerUpgradeCfg *m_upgradePlayerCfg;												   //人物升级加点
	QTime m_lastUpdateTeamTime;																   //获取队伍信息间隔
	bool m_bRealUpdateUi = false;															   //是否实时刷新ui界面数据
	bool m_bRealUpdatePlayerUi = false;														   //是否实时刷新玩家信息
	bool m_bEnabledDataDisplayUi = false;													   //是否启用ui界面Npc 物品 玩家显示功能
	bool m_bMapIsVisible = false;															   //当前是否显示地图
	QMap<int, GameConditionCfg *> m_pEatFoodJudge;											   //人物吃料理判断
	QTime m_uLastUseItemTime;																   //最后一次使用物品时间
	GameConditionCfg *m_pEatFoodCfg = nullptr;												   //人物使用物品配置
	GameEquipProtectCfg *m_pEquipProtectCfg = nullptr;										   //装备保护配置
	QStringList m_sEquipProtectFilters;														   //装备保护过滤名称
	bool m_bAutoHeal = false;																   //自动治疗
	QString m_NurseMessage;																	   //补血相关消息
	int m_NurseNPCId = 0;																	   //补血NpcID
	int m_saleNpcID = 0;																	   //卖货NpcID
	bool m_bSyncGameWindow = true;															   //辅助和游戏窗口同步
	QFuture<void> m_characterFuture;														   //获取人物信息线程
	QFuture<void> m_itemsFuture;															   //获取物品栏信息线程
	QFuture<void> m_mapFuture;																   //地图下载线程
	QFuture<void> m_normalFuture;															   //常态线程
	QFuture<void> m_notifyTimeoutFuture;													   //回调检测线程
	QMap<int, quint32> m_quickKeyMap;														   //快捷键表
	int m_nScriptDelayTime = 100;															   //脚本执行延时时间，每行脚本延时时间 最小100
	int m_DownloadMapX;																		   //下载地图x
	int m_DownloadMapY;																		   //下载地图y
	int m_DownloadMapXSize;																	   //下载地图宽
	int m_DownloadMapYSize;																	   //下载地图高
	bool m_IsDownloadingMap;																   //是否下载地图中
	QCommandLineParser m_cmdParser;															   //bat命令行解析
	QHash<int, QCommandLineOption *> m_commandMap;											   //命令解析映射
	QMap<int, QString> m_upgradeTypeText;													   //升级类型对应
	int m_lastGameConnState = -1;															   //最后一次连接状态
	QString m_lastGameConnMsg;																   //最后一次连接提示信息
	int m_lastBattleAvgTeamPetLv = 0;														   //计算上次战斗平均宠物等级
	int m_lastBattleTroopPetMinLv = 0;														   //最后一次战斗后 宠物最低等级
	QTime m_lastNormalState;																   //判断正常状态 防止卡主
	QTime m_lastRecvNpcDlgTime;																   //判断对话框状态 防止卡主
	bool m_isInBattle = false;																   //是否战斗中
	QTime m_lastUploadTime;																	   //最后一次上报时间
	bool m_bNeedUploadBank = false;															   //是否需要上传银行信息
	int m_startGameHide = 0;																   //启动游戏隐藏
	int m_followGamePos = 0;																   //跟随游戏显示位置
	QVector<QPair<quint64, QSharedPointer<CGA_NPCDialog_t> > > m_npcDlgCache;				   //对话框缓存
	QVector<QPair<quint64, QSharedPointer<CGA::cga_working_result_t> > > m_workResCache;	   //工作缓存
	QVector<QPair<quint64, QSharedPointer<CGA::cga_trade_dialog_t> > > m_tradeDlgCache;		   //交易对话框缓存
	QVector<QPair<quint64, QSharedPointer<CGA::cga_player_menu_items_t> > > m_playerMenuCache; //菜单选择缓存
	QVector<QPair<quint64, QSharedPointer<CGA::cga_unit_menu_items_t> > > m_unitMenuCache;	   //菜单列表选择缓存
	QVector<QPair<quint64, int> > m_battleActionCache;										   //对战状态缓存
	QVector<QPair<quint64, QSharedPointer<CGA::cga_conn_state_t> > > m_connectStateCache;	   //连接状态缓存
	QMutex m_npcDlgMutex;																	   //npc对话框锁
	QMutex m_tradeDlgMutex;																	   //交易对话框锁
	QMutex m_workResMutex;																	   //工作返回锁
	QMutex m_playerMenuResMutex;															   //菜单锁
	QMutex m_unitMenuResMutex;																   //菜单项锁
	QMutex m_battleResMutex;																   //战斗状态返回锁
	QMutex m_connectResMutex;																   //连接状态返回锁
	bool m_repeatedGidExit = true;															   //重复挂接是否退出
	int m_loginWaitInterval = 3000;															   //自动登录间隔
	bool m_bCreateLog = false;																   //日志开关
	bool m_bOpenNetToMTool = false;															   //是否打开和MLAssistTool通信功能
	QString m_sCGInstallPath;																   //魔力安装目录
	bool m_bOpenSyncMap = false;															   //是否打开了同步地图功能
};

template <typename TData>
void GameCtrl::RemoveTimeoutCache(QMutex &pMutex, TData &tmpCache)
{
	QMutexLocker locker(&pMutex);
	quint64 curTime = GetTickCount();
	for (auto it = tmpCache.begin(); it != tmpCache.end();)
	{
		if ((curTime - it->first) > 30000)
		{
			it = tmpCache.erase(it);
		}
		else
			++it;
	}
}

#define g_pGameCtrl GameCtrl::getInstace()
