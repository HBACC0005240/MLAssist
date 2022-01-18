#pragma once

//extern "C"
//{
//#include "../lua/lua.h"
//#include "../lua/lauxlib.h"
//#include "../lua/lualib.h"
//}
//#include "../include/LuaBridge/Source/LuaBridge/LuaBridge.h"
#include "LuaPlus.h"
using namespace LuaPlus;

#include "../CGALib/gameinterface.h"
#include "CGFunction.h"

extern CGA::CGAInterface *g_CGAInterface;
#include <QString>
#include <string>
using namespace std;
class CGLuaFun
{
public:
	CGLuaFun();
	~CGLuaFun();
	//注册函数
	//void RegisterFun(lua_State* pState);
	//测试
	int Lua_Test(LuaState *L);
	int Lua_Translate(LuaState *L);
	int Lua_RegisterLuaFun(LuaState *L);
	//脚本窗口提示 喊话提示 后台运行提示
	int Lua_LogMessage(LuaState *L);
	int Lua_ScriptLogMessage(LuaState *L);
	//获取脚本界面设置的值
	int Lua_GetScriptUISetData(LuaState *L);

	//后台提示
	int Lua_DebugMessage(LuaState *L);
	//系统设置选项
	int Lua_SysConfig(LuaState *L);
	//获取游戏时间
	int Lua_GetSysTimeEx(LuaState *L);
	int Lua_GetBGMIndex(LuaState *L);

	//设置战聊易
	int Lua_SetCharacterSwitch(LuaState *L);
	//发起pk 加队 交易 摆摊等
	int Lua_DoCharacterAction(LuaState *L);
	//删除技能
	int Lua_DeleteSkill(LuaState *L);
	//获取游戏世界状态
	int Lua_GetWorldStatus(LuaState *L);
	//获取游戏状态
	int Lua_GetGameStatus(LuaState *L);
	//游戏窗口状态 连接cga状态
	int Lua_GetConnectGameWndStatus(LuaState *L);
	//运行游戏窗口
	int Lua_RunGameWnd(LuaState *L);

	//读取用户配置
	int Lua_LoadUserConfig(LuaState *L);
	//保存用户配置
	int Lua_SaveUserConfig(LuaState *L);
	//获取自动登录的游戏id列表
	int Lua_GetAccountGids(LuaState *L);
	//设置ui界面游戏子账号
	int Lua_SetUIAccountGid(LuaState *L);
	//设置ui界面游戏角色
	int Lua_SetUICharacter(LuaState *L);
	//设置ui界面创建游戏角色信息
	int Lua_SetUICreateCharacterInfo(LuaState *L);
	//登录游戏
	int Lua_LoginGame(LuaState *L);
	//重置游戏连接状态信息
	int Lua_ResetLoginGameConnectState(LuaState *L);
	//获取登录游戏状态
	int Lua_GetLoginGameConnectState(LuaState *L);
	//回到选择游戏线路
	int Lua_BackSelectGameLine(LuaState *L);
	//切换登录信息
	int Lua_SwitchLoginData(LuaState *L);
	//收集账号仓库信息
	int Lua_GatherAccountInfo(LuaState *L);
	//保存账号仓库信息
	int Lua_SaveGatherAccountInfos(LuaState *L);
	//等待完成
	int Lua_Nowhile(LuaState *L);
	//喊话
	int Lua_Chat(LuaState *L);
	//穿墙
	int Lua_ThroughWall(LuaState *L);
	//穿墙
	int Lua_ThroughWallEx(LuaState *L);
	//加载执行脚本
	int Lua_LoadScript(LuaState *L);
	//导入脚本
	int Lua_ImportScript(LuaState *L);
	//加载脚本
	int Lua_SwitchScript(LuaState *L);
	//执行脚本
	int Lua_ExecScript(LuaState *L);
	//是否停止脚本
	int Lua_StopScript(LuaState *L);
	//是否空闲中
	int Lua_IsInNormalState(LuaState *L);
	//是否战斗中
	int Lua_IsInBattle(LuaState *L);
	//用户输入对话框  定义变量
	int Lua_UserDefDialog(LuaState *L);
	//用户输入对话框  定义变量
	int Lua_UserDefComboBoxDlg(LuaState *L);
	//清除系统提示
	int Lua_ClearSysCue(LuaState *L);
	//开始遇敌
	int Lua_BeginAutoAction(LuaState *L);
	//停止遇敌
	int Lua_EndAutoAction(LuaState *L);

	//获取坐标
	int GetMapPos(LuaState *L);
	//是否目标附近一格
	int Lua_IsNearTarget(LuaState *L);
	//取当前地图名
	int Lua_GetMapName(LuaState *L);
	//取当前地图名
	int Lua_GetMapNumber(LuaState *L);
	//取当前楼层
	int Lua_GetMapFloorNumberFromName(LuaState *L);
	int Lua_GetNumberFromName(LuaState *L);
	//取当前地图数据
	int Lua_GetMapData(LuaState *L);
	//走迷宫
	int Lua_AutoWalkMaze(LuaState *L);
	//穿越整个迷宫
	int Lua_AutoWalkRandomMaze(LuaState *L);
	//取程序路径
	int Lua_GetAppRunPath(LuaState *L);
	//登出
	int Lua_TownPortalScroll(LuaState *L);
	//登出到服务器
	int Lua_LogoutServer(LuaState *L);
	//等待
	int Lua_WaitTime(LuaState *L);
	//自动寻路
	int Lua_AutoMove(LuaState *L);
	//指定方向移动一格
	int Lua_MoveGo(LuaState *L);
	//移动到目标周围一格
	int Lua_MoveToNpcNear(LuaState *L);
	//搜索地图
	int Lua_SearchMap(LuaState *L);
	//下载当前地图
	int Lua_DownloadMap(LuaState *L);
	//迷宫下层坐标 距离最远的
	int Lua_GetNextMazeWarp(LuaState *L);
	//迷宫所有传送坐标
	int Lua_GetAllMazeWarpList(LuaState *L);
	//获取迷宫遍历搜索路线
	int Lua_FindRandomSearchPath(LuaState *L);
	//获取周围信息
	int Lua_GetMapUnits(LuaState *L);
	//查找周围信息
	int Lua_FindMapUnit(LuaState *L);
	//目标是否可达
	int Lua_IsReachableTarget(LuaState *L);
	//获取周围指定距离坐标空格
	int Lua_GetRandomSpace(LuaState *L);

	//查找随机迷宫
	int Lua_FindToRandomEntry(LuaState *L);
	//使用物品
	int Lua_UseItem(LuaState *L);
	//交易
	int Lua_LaunchTrade(LuaState *L);
	//等待交易
	int Lua_WaitTrade(LuaState *L);
	//等待交易对话框
	int Lua_WaitTradeDlg(LuaState *L);
	//添加交易信息
	int Lua_TradeAddStuffs(LuaState *L);
	//扔物品
	int Lua_ThrowItemName(LuaState *L);
	//扔不满指定数量物品
	int Lua_ThrowNoFullItemName(LuaState *L);
	//拾取
	int Lua_PickupItem(LuaState *L);
	//叠加物品
	int Lua_PileItem(LuaState *L);
	//交换物品
	int Lua_SwitchItem(LuaState *L);
	//装备物品
	int Lua_EquipItem(LuaState *L);
	int Lua_EquipItemEx(LuaState *L);

	//脱下装备
	int Lua_UnEquipItem(LuaState *L);
	int Lua_UnEquipItemEx(LuaState *L);
	//耐久
	int Lua_GetItemDurability(LuaState *L);

	//转向
	int Lua_TurnAbout(LuaState *L);
	//转向
	int Lua_TurnAboutEx(LuaState *L);
	//转向方向
	int Lua_TurnAboutEx2(LuaState *L);
	//转向坐标方向
	int Lua_TurnAboutPointDir(LuaState *L);
	//Npc对话选择
	int Lua_Npc(LuaState *L);
	//NPC对话
	int Lua_TalkNpc(LuaState *L);
	//Npc对话选是
	int Lua_TalkNpcSelectYes(LuaState *L);
	int Lua_TalkNpcPosSelectYes(LuaState *L);
	//Npc对话选否
	int Lua_TalkNpcSelectNo(LuaState *L);
	int Lua_TalkNpcPosSelectNo(LuaState *L);

	//取人物数据
	int Lua_GetPlayerData(LuaState *L);
	//取人物所有数据
	int Lua_GetPlayerAllData(LuaState *L);
	//身上物品信息
	int Lua_GetAllItemData(LuaState *L);
	//下一级称号数据
	int Lua_GetNextTitleData(LuaState *L);

	//取指定宠物数据
	int Lua_GetPetData(LuaState *L);
	//取所有宠物数据
	int Lua_GetAllPetData(LuaState *L);
	//修改宠物数据 作战
	int Lua_SetPetData(LuaState *L);
	//取装备信息
	int Lua_GetPlayereEquipData(LuaState *L);
	//解析装备耐久
	int Lua_ParseEquipData(LuaState *L);
	//取宠物血量
	int Lua_GetBattlePetData(LuaState *L);
	//取队伍信息
	int Lua_GetTeamData(LuaState *L);
	//取全部队伍信息
	int Lua_GetAllTeammateData(LuaState *L);
	//回复
	int Lua_Renew(LuaState *L);
	//取好友名片
	int Lua_GetFriendCard(LuaState *L);

	//更改个人信息
	int Lua_SetPlayerInfo(LuaState *L);

	//取人物包裹指定物品数量
	int Lua_GetItemCount(LuaState *L);
	//取人物包裹指定物品叠加数量
	int Lua_GetItemPileCount(LuaState *L);
	//取人物银行指定物品数量
	int Lua_GetBankItemCount(LuaState *L);
	//取人物银行指定物品叠加数量
	int Lua_GetBankItemPileCount(LuaState *L);
	//取人物身上指定物品数量
	int Lua_GetAllItemCount(LuaState *L);
	//取人物身上指定物品叠加数量
	int Lua_GetAllItemPileCount(LuaState *L);

	//取人物包裹未使用空格
	int Lua_GetItemNotUseSpaceCount(LuaState *L);
	//取人物包裹已使用空格
	int Lua_GetBagUsedItemCount(LuaState *L);
	//取银行未使用空格
	int Lua_GetBankItemNotUseSpaceCount(LuaState *L);
	//取队伍人数
	int Lua_GetTeammatesCount(LuaState *L);
	//取队伍宠物平均等级
	int Lua_GetTeamPetAvgLv(LuaState *L);
	//取队伍宠物等级
	int Lua_GetTeamPetLv(LuaState *L);
	//加入队伍
	int Lua_AddTeammate(LuaState *L);
	//离开队伍
	int Lua_LeaveTeammate(LuaState *L);
	//获取目标地无人坐标
	int Lua_GetUnmannedMapUnitPosList(LuaState *L);
	//是否队伍队长
	int Lua_IsTeamLeader(LuaState *L);
	//等待组队完成
	int Lua_WaitTeammates(LuaState *L);
	//等待组队完成
	int Lua_WaitTeammatesEx(LuaState *L);
	//等待对话框
	int Lua_WaitRecvHead(LuaState *L);
	//等待工作状态回复
	int Lua_WaitRecvWorkResult(LuaState *L);
	//等待选择菜单
	int Lua_WaitRecvPlayerMenu(LuaState *L);
	//等待选择菜单项
	int Lua_WaitRecvPlayerMenuUnit(LuaState *L);
	//等待回补完成
	int Lua_WaitSupplyFini(LuaState *L);
	//等待空闲
	int Lua_WaitNormal(LuaState *L);
	//等待战斗结束
	int Lua_WaitBattleEnd(LuaState *L);
	//等待游戏窗口快捷键
	int Lua_WaitRecvGameWndKeyDown(LuaState *L);
	//等待战斗返回
	int Lua_WaitRecvBattleAction(LuaState *L);

	//人物菜单选择
	int Lua_PlayerMenuSelect(LuaState *L);
	//菜单项选择
	int Lua_UnitMenuSelect(LuaState *L);
	//工作
	int Lua_Work(LuaState *L);
	//合成
	int Lua_AllCompound(LuaState *L);
	//压矿
	int Lua_Exchange(LuaState *L);
	//买东西
	int Lua_Shopping(LuaState *L);
	//解析购买列表
	int Lua_ParseBuyStoreMsg(LuaState *L);
	//卖东西
	int Lua_Sale(LuaState *L);
	//卖东西
	int Lua_SaleEx(LuaState *L);
	//鉴定东西
	int Lua_IdentifyItem(LuaState *L);
	//鉴定东西
	int Lua_IdentifyItemEx(LuaState *L);
	//存物品-全
	int Lua_SaveToBankAll(LuaState *L);
	//存物品-单次
	int Lua_SaveToBankOnce(LuaState *L);
	//取物品
	int Lua_WithdrawItem(LuaState *L);
	//取物品-全
	int Lua_WithdrawAllItem(LuaState *L);

	//扔钱
	int Lua_DropGold(LuaState *L);
	//存钱
	int Lua_DepositGold(LuaState *L);
	//取钱
	int Lua_WithdrawGold(LuaState *L);
	//扔宠
	int Lua_DropPet(LuaState *L);
	//存宠
	int Lua_DepositPet(LuaState *L);
	//取宠
	int Lua_WithdrawPet(LuaState *L);
	//银行
	int Lua_BankOperation(LuaState *L);
	//保存图鉴信息
	int Lua_SavePetPictorialBookToHtml(LuaState *L);
	//设置界面脚本简介
	int Lua_SetUIScriptDesc(LuaState *L);
	//创建随机名字
	int Lua_CreateRandomRoleName(LuaState *L);

	//获取系统提示内容
	int Lua_GetSysChatMsg(LuaState *L);
	int Lua_GetLastSysChatMsg(LuaState *L);
	int Lua_GetAllChatMsg(LuaState *L);
	int Lua_GetDetailAllChatMsg(LuaState *L);
	int Lua_GetLastChatMsg(LuaState *L);
	int Lua_ClearSysChatMsg(LuaState *L);
	int Lua_WaitSysMsg(LuaState *L);
	int Lua_WaitChatMsg(LuaState *L);
	int Lua_WaitSysAndChatMsg(LuaState *L);
	int Lua_WaitSubscribeMsg(LuaState *L);
	//订阅消息
	int Lua_SubscribeMsg(LuaState *L);
	//发布消息
	int Lua_PublishMsg(LuaState *L);
	//获取已接收到的订阅消息
	int Lua_GetTopicMsgList(LuaState *L);
	int Lua_GetLastTopicMsg(LuaState *L);

	void PauseScript();
	void ResumeScript();

	int GetLuaCurrentLine();
	int LoadScript(QString scriptPath);
	void StopScript();
	static void LoadScriptThread(CGLuaFun *pThis, QString scriptPath);
	void moveto(int x, int y);
	void LUA_Sleep(int nNum);
	int LUA_GetTickCount();
	bool LUA_AutoRunToTarget(int x, int y, const QString &mapName);
	bool LUA_MoveToTarget(int x, int y);
	void LUA_Renew(int x, int y);

	void setLuaState(lua_State *pState) { m_pLuaState = pState; }
	lua_State *getLuaState() { return m_pLuaState; }
	void SetLastScriptPath(const QString &path) { m_lastScriptPath = path; }
	void ClearRegisterFun() { m_lastRegisterFunName.clear(); }
	void CallRegisterFun(LuaState *L, const QString &topic, const QString &msg);

private:
	//QVariant转换为lua 错误默认push 0
	void TransVariantToLua(LuaState *L, QVariant &val, bool bTransToInt = true);

	lua_State *m_pLuaState = nullptr;
	QString m_lastScriptPath;
	QString m_lastRegisterFunName;
};
//
//static void RegisterFun(lua_State* pState)
//{
//	luabridge::getGlobalNamespace(pState)
//		.beginNamespace("test")
//		.beginClass<CGLuaFun>("CGLuaFun")
//		.addConstructor<void(*) (void)>()//无参构造函数的注册
//		.addFunction("moveto", &CGLuaFun::moveto)//注册test、方法到lua（addStaticFunction静态函数注册也类似）
//		.addFunction("Renew", &CGLuaFun::LUA_Renew)//注册test、方法到lua（addStaticFunction静态函数注册也类似）
//		.addFunction("移动到", &CGLuaFun::moveto)//注册test、方法到lua（addStaticFunction静态函数注册也类似）
//		.addFunction("自动寻路", &CGFunction::AutoMoveTo)//注册test、方法到lua（addStaticFunction静态函数注册也类似）
//		.addCFunction("取人物坐标", &CGLuaFun::GetMapPos)
//		.addFunction("WaitTime", &CGLuaFun::WaitTime)
//		.endClass()
//		.endNamespace();
//}
