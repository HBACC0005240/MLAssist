#include "CGLuaFun.h"
#include "GPCalc.h"
#include "GameCtrl.h"
#include "ITNetworkFactory.h"
#include "ITObjectDataMgr.h"
#include "RpcSocketClient.h"
#include <windows.h>
#include <QApplication>
CGLuaFun::CGLuaFun()
{
	//m_pLuaState = luaL_newstate();
	//luaL_openlibs(m_pLuaState);
	//RegisterFun(m_pLuaState);
}

CGLuaFun::~CGLuaFun()
{
	//lua_close(m_pLuaState);
}

int CGLuaFun::Lua_Test(LuaState *L)
{
	lua_State *pState = L->GetCState();
	lua_getglobal(pState, "package");	/* L: package，获得package，在栈定 */
	lua_getfield(pState, -1, "loaded"); /* L: package loaded,获得表，在栈顶*/
	lua_pushnil(pState);				/* L: package loaded nil */
	while (0 != lua_next(pState, -2))	/* L: package loaded, key, value，上一个栈顶为nil，弹出nil，获得表的第一个key和value，压入栈 */
	{
		//CCLOG("%s - %s \n", tolua_tostring(stack, -2, ""), lua_typename(stack, lua_type(stack, -1)));
		QString key = lua_tostring(pState, -2, ""); /*这时候栈顶得下一个,是key*/
		QString val = lua_tostring(pState, -1, ""); /*这时候栈顶得下一个,是key*/

		QString tableKey = key; /*下面是对key的一段处理*/
		qDebug() << key << val;
		//int found = tableKey.rfind(".lua");
		//if (found != std::string::npos)
		//	tableKey = tableKey.substr(0, found);
		//tableKey = replaceAll(tableKey, ".", "/");
		//tableKey = replaceAll(tableKey, "\\", "/");
		//tableKey.append(".lua");
		//found = fileName.rfind(tableKey);
		//if (0 == found || (found != std::string::npos && fileName.at(found - 1) == '/'))
		//{
		//	lua_pushstring(pState, key.c_str()); /*package loaded, key, value，newkey, 将key,压入栈顶*/
		//	lua_pushnil(pState);				/* pakage,loaded(table)(-5),key(-4),value(-3),key(-2),nil(-1)*/
		//	if (lua_istable(pState, -5))		/*判读栈顶往下第五个是不是table*/
		//	{
		//		/*结果将key对应的值置为nil*/
		//		lua_settable(pState, -5); /*pakage,loaded(table),key,value,  将栈顶两个元素作为key和value设置给table，弹出栈顶两个元素*/
		//	}
		//}
		lua_pop(pState, 1); /*pakage,loaded(table),key  弹出value,留下key作为下一个next*/
	}
	lua_pop(pState, 2); /*栈平衡*/
	return 0;
	LuaStack args(L);
	if (args[1].IsTable())
	{
		qDebug() << "是表";
	}
	//lua_istable(GetCState(), m_stackIndex);
	LuaObject tblData = args[1];
	//.GetByIndex(1);
	for (LuaTableIterator it(tblData); it; it.Next())
	{
		const char *key = it.GetKey().GetString();
		int num = it.GetValue().GetInteger();
		qDebug() << "Key:" << key << "Num:" << num;
	}
	//LuaObject globalsObj = L->GetGlobals()["tableData"];
	//LuaObject x = globalsObj["x"];
	//qDebug() << x.GetInteger();
	//LuaObject y = globalsObj["y"];
	//qDebug() << y.GetInteger();
	//LuaObject obj = L->GetGlobals()["tableData"];
	//for (LuaTableIterator it(obj); it; it.Next())
	//{
	//	const char *key = it.GetKey().GetString();
	//	int num = it.GetValue().GetInteger();
	//	qDebug() << "Key:" << key << "Num:" << num;
	//}

	return 0;
}

int CGLuaFun::Lua_Translate(LuaState *L)
{
	LuaStack args(L);
	QString sLog = args[1].GetString();
	QTextCodec *tc = QTextCodec::codecForName("GBK");
	auto sData = tc->fromUnicode(sLog);
	L->PushString(sData.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_RegisterLuaFun(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}

	m_lastRegisterFunName = args[1].GetString();
	lua_getglobal(L->GetCState(), m_lastRegisterFunName.toStdString().c_str());
	lua_pushstring(L->GetCState(), "TestTopic");
	lua_pushstring(L->GetCState(), "TradeMsg");
	lua_pcall(L->GetCState(), 2, 1, 0); //2代表lua函数2个参数 1代表返回1个
	LuaStack args2(L);					//这个拿不到调用函数返回值
	qDebug() << args2[1].GetString();
	qDebug() << lua_tostring(L->GetCState(), -1); //可以拿到调用函数返回值  这里是反着接数据的，lua最后一个 这里就是第一个
	qDebug() << lua_tostring(L->GetCState(), -2); //可以拿到调用函数返回值
	lua_pop(L->GetCState(), 2);					  //上面压了两个参数  这里出栈
	L->PushString("c++ ret");					  //当前函数返回给lua 也可以拿到
	QString sMsgData = QString("%1 %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg("Lua_RegisterLuaFun Fini");
	qDebug() << sMsgData;
	return 1;
}

int CGLuaFun::Lua_LogMessage(LuaState *L)
{
	LuaStack args(L);
	QString sLog = args[1].GetString();
	int ntype = args.Count() > 1 ? args[2].GetInteger() : 0;
	emit g_pGameCtrl->signal_addOneLogMsg(sLog);
	if (ntype > 0)
		g_pGameFun->Chat(sLog, 2, 3, 5);
	return 0;
}

int CGLuaFun::Lua_ScriptLogMessage(LuaState *L)
{
	LuaStack args(L);
	QString sLog = args[1].GetString();
	emit g_pGameCtrl->signal_addOneScriptLogMsg(sLog);
	return 0;
}

int CGLuaFun::Lua_GetScriptUISetData(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	QString sUserType = args[1].GetString();
	QVariant val;
	if (args[1].IsString())
	{
		val = g_pGameFun->GetScriptUiSetData(args[1].GetString());
		bool bTrans = args.Count() >= 2 ? args[2].GetBoolean() : true;
		TransVariantToLua(L, val, bTrans);
	}
	else
	{
		L->PushInteger(0);
	}
	return 1;
}

//void CGLuaFun::RegisterFun(lua_State* pState)
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

int CGLuaFun::GetMapPos(LuaState *pState)
{
	QPoint pos = g_pGameFun->GetMapCoordinate();
	//返回参数1
	//lua_pushnumber(L, pos.x());
	////返回参数1
	//lua_pushnumber(L, pos.y());
	//返回参数个数
	pState->PushInteger(pos.x());
	pState->PushInteger(pos.y());
	qDebug() << "GetMapPos";
	return 2;
}

int CGLuaFun::Lua_IsNearTarget(LuaState *L)
{
	LuaStack args(L);
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	int dis = args.Count() > 2 ? args[3].GetInteger() : 1;
	bool bRet = g_pGameFun->IsNearTarget(x, y, dis);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_DebugMessage(LuaState *L)
{
	LuaStack args(L);
	QString sDesc = args[1].GetString();
	//int nDesc = args[1].GetInteger();
	qDebug() << sDesc;
	return 0;
}

int CGLuaFun::Lua_SysConfig(LuaState *L)
{
	LuaStack args(L);
	QVariant vType = L->IsString(1) ? QVariant(args[1].GetString()) : args[1].GetNumber();
	QVariant vData1 = args.Count() > 1 ? (L->IsString(2) ? QVariant(args[2].GetString()) : args[2].GetNumber()) : 0;
	QVariant vData2;
	if (args.Count() > 2)
		vData2 = args.Count() > 2 ? (L->IsString(3) ? QVariant(args[3].GetString()) : args[3].GetNumber()) : 0;
	bool bRet = g_pGameFun->SysConfig(vType, vData1, vData2);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_GetSysTimeEx(LuaState *L)
{

	QString sTime = g_pGameFun->getSysTimeEx();
	L->PushString(sTime.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_GetBGMIndex(LuaState *L)
{
	int index = -1;
	g_CGAInterface->GetBGMIndex(index);
	L->PushInteger(index);
	return 1;
}

int CGLuaFun::Lua_SetCharacterSwitch(LuaState *L)
{
	LuaStack args(L);
	int nType = args[1].GetInteger();
	int bFlag = args[2].GetInteger();
	bool bRet = g_pGameFun->SetCharacterSwitch(nType, bFlag);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_DoCharacterAction(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	
	QVariant vType = args[1].GetString();
	bool transData = false;
	if (vType.toInt(&transData) && transData)
	{
		int nType = args[1].GetInteger();
		g_pGameFun->DoCharacterAction(nType);	
	}
	else
	{
		QString sType = vType.toString();
		int nVal = args.Count() > 1 ? args[2].GetInteger() : 0;
		if (g_pGameFun->m_playerActionHash.value(sType) == TCharacter_Action_Gesture)
		{
			g_pGameFun->Gesture(nVal);
		}
		else
			g_pGameFun->DoCharacterAction(g_pGameFun->m_playerActionHash.value(sType));
	}
	return 0;
}

int CGLuaFun::Lua_DeleteSkill(LuaState *L)
{
	LuaStack args(L);
	int v1 = args[1].IsInteger() ? args[1].GetInteger() : -1;
	if (v1 < 0)
		return 0;
	if (args.Count() == 2)
	{
		QString v2 = args[2].IsString() ? args[2].GetString() : "";
		g_pGameFun->ForgetPlayerSkillEx(v1, v2);
	}
	else if (args.Count() == 3)
	{
		int v2 = args[2].IsInteger() ? args[2].GetInteger() : -1;
		QString v3 = args[3].IsString() ? args[3].GetString() : "";
		g_pGameFun->ForgetPlayerSkill(v1, v2, v3);
	}
	return 0;
}

int CGLuaFun::Lua_GetWorldStatus(LuaState *L)
{
	int val = g_pGameFun->GetWorldStatus();
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetGameStatus(LuaState *L)
{
	int val = g_pGameFun->GetGameStatus();
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetConnectGameWndStatus(LuaState *L)
{
	bool bRet = g_CGAInterface->IsConnected();
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_RunGameWnd(LuaState *L)
{
	emit g_pGameCtrl->signal_runGameWnd();
	return 0;
}

int CGLuaFun::Lua_LoadUserConfig(LuaState *L)
{
	LuaStack args(L);
	QString sVal = args[1].GetString();
	emit g_pGameCtrl->signal_loadUserConfig(sVal);
	return 0;
}

int CGLuaFun::Lua_SaveUserConfig(LuaState *L)
{
	LuaStack args(L);
	QString sVal = args[1].GetString();
	emit g_pGameCtrl->signal_saveUserConfig(sVal);
	return 0;
}

int CGLuaFun::Lua_GetAccountGids(LuaState *L)
{
	auto sGids = g_pGameFun->GetAccountGIDs();
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	int nSize = sGids.size();
	int nIndex = 0;
	for (auto sGid : sGids)
	{
		tableObj.SetString(++nIndex, sGid.toStdString().c_str());
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_SetUIAccountGid(LuaState *L)
{
	LuaStack args(L);
	QString sVal = args[1].GetString();
	emit g_pGameCtrl->signal_swicthAccountGid(sVal);
	return 0;
}

int CGLuaFun::Lua_SetUICharacter(LuaState *L)
{
	LuaStack args(L);
	int index = args[1].GetInteger();
	emit g_pGameCtrl->signal_swicthCharacter(index);
	return 0;
}

int CGLuaFun::Lua_SetUICreateCharacterInfo(LuaState *L)
{
	LuaStack args(L);
	bool isCreate = args[1].GetBoolean();
	QString sName = args[2].GetString();
	int nCharType = args[3].GetInteger();
	int nEye = args[4].GetInteger();
	int nMouth = args[5].GetInteger();
	int nColor = args[6].GetInteger();
	QString sPoint = args[7].GetString();
	QString sCrystal = args[8].GetString();
	emit g_pGameCtrl->signal_createCharaData(isCreate, nCharType, nEye, nMouth, nColor, sPoint, sCrystal, sName);
	return 0;
}

int CGLuaFun::Lua_LoginGame(LuaState *L)
{
	emit g_pGameCtrl->signal_loginGame();
	return 0;
}

int CGLuaFun::Lua_ResetLoginGameConnectState(LuaState *L)
{
	g_pGameCtrl->ResetGameConnectState();
	return 0;
}

int CGLuaFun::Lua_GetLoginGameConnectState(LuaState *L)
{
	int state = -1;
	QString msg = "";
	g_pGameCtrl->GetLoginGameConnectState(state, msg);
	L->PushInteger(state);
	L->PushString(msg.toStdString().c_str());
	return 2;
}

int CGLuaFun::Lua_BackSelectGameLine(LuaState *L)
{
	g_CGAInterface->BackSelectServer();
	return 0;
}

int CGLuaFun::Lua_SwitchLoginData(LuaState *L)
{
	LuaStack args(L);
	QString gid = args.Count() > 0 ? args[1].GetString() : "";
	int server = args.Count() > 1 ? args[2].GetInteger() : 0;
	int line = args.Count() > 2 ? args[3].GetInteger() : 0;
	int character = args.Count() > 3 ? args[4].GetInteger() : 0;
	emit g_pGameCtrl->signal_switchLoginData(gid, server, line, character);
	return 0;
}

int CGLuaFun::Lua_GatherAccountInfo(LuaState *L)
{
	LuaStack args(L);
	g_pGameFun->GatherAccountInfo();
	return 0;
}

int CGLuaFun::Lua_SaveGatherAccountInfos(LuaState *L)
{
	LuaStack args(L);
	g_pGameFun->SaveGatherAccountInfos();
	return 0;
}

int CGLuaFun::Lua_Nowhile(LuaState *L)
{
	LuaStack args(L);
	if (!L->IsString(1))
	{
		luaL_error(L->GetCState(), "第一个参数错误，不是字符串");
		return 0;
	}
	//qDebug() << "Type:" << L->Type(1) << " TypeName:" << L->TypeName(1);
	//QString sData = (char*)args[1].GetUserdata();
	if (args.Count() >= 1 && args.Count() <= 2)
	{
		g_pGameFun->Nowhile(args[1].GetString());
	}
	else if (args.Count() > 2)
	{

		int x = args[2].GetInteger();
		int y = args[3].GetInteger();
		g_pGameFun->NowhileEx(args[1].GetString(), x, y);
	}
	return 0;
}
//
int CGLuaFun::Lua_Chat(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() != 4)
	{
		return 0;
	}
	QString sVal = args[1].GetString();
	int v1 = args[2].GetInteger();
	int v2 = args[3].GetInteger();
	int v3 = args[4].GetInteger();
	g_pGameFun->Chat(sVal, v1, v2, v3);
	return 0;
}

int CGLuaFun::Lua_ThroughWall(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() != 2)
	{
		return 0;
	}
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	bool bShow = args.Count() > 2 ? args[3].GetBoolean() : true;
	//g_pGameFun->ForceMoveTo(v1, v2);
	bool bResult = false;
	g_CGAInterface->ForceMoveTo(x, y, bShow, bResult);
	return 0;
}

int CGLuaFun::Lua_ThroughWallEx(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() != 2)
	{
		return 0;
	}
	int v1 = args[1].GetInteger();
	int v2 = args[2].GetInteger();
	bool bShow = args.Count() > 2 ? args[3].GetBoolean() : true;
	bool bResult = false;
	int nDir = g_pGameFun->TransDirectionToCga(v1);

	while (v2 > 0)
	{
		g_CGAInterface->ForceMove(nDir, bShow, bResult);
		Sleep(500);
		g_pGameFun->WaitInNormalState();
		v2--;
	}
	//	g_pGameFun->ForceMoveToEx(v1, v2);
	return 0;
}

int CGLuaFun::Lua_IsInNormalState(LuaState *L)
{
	bool bRet = g_pGameFun->IsInNormalState();
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_IsInBattle(LuaState *L)
{
	bool bRet = g_pGameFun->IsInBattle();
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_UserDefDialog(LuaState *L)
{
	LuaStack args(L);
	QString sMsg = args[1].GetString();
	QString sDefaultVal = args[2].GetString();
	QVariant val = g_pGameFun->UserInputDialog(sMsg, sDefaultVal);
	bool bTrans = args.Count() >= 3 ? args[3].GetBoolean() : true;
	TransVariantToLua(L, val, bTrans);
	g_pGameFun->SetScriptInputVarData(sMsg, val);
	emit g_pGameCtrl->signal_addOneScriptInputVar(TInputType_Edit, sMsg, val, "");
	return 1;
}

int CGLuaFun::Lua_UserDefComboBoxDlg(LuaState *L)
{
	LuaStack args(L);
	QString sMsg = args[1].GetString();
	QStringList sItemList;
	QString sDefaultVal;
	if (args[2].IsTable())
	{
		LuaObject tblData = args[2];
		for (LuaTableIterator it(tblData); it; it.Next())
		{
			sItemList << it.GetValue().GetString();
		}
	}
	else
	{
		QString tmpItemList = args[2].GetString();		
		sItemList << tmpItemList.split("|");
	}
	if (args.Count() >= 3)
		sDefaultVal = args[3].GetString();
	else if (sItemList.size() > 0)
		sDefaultVal = sItemList[0];
	QVariant val = g_pGameFun->UserComboBoxDialog(sMsg, sItemList, sDefaultVal);
	bool bTrans = args.Count() >= 4 ? args[4].GetBoolean() : true;
	TransVariantToLua(L, val, bTrans);
	g_pGameFun->SetScriptInputVarData(sMsg, val);
	emit g_pGameCtrl->signal_addOneScriptInputVar(TInputType_ComboBox, sMsg, sItemList, val);
	return 1;
}

int CGLuaFun::Lua_UserDefCheckBoxDlg(LuaState *L)
{
	LuaStack args(L);
	QString sMsg = args[1].GetString();
	QString sDefaultVal = args[2].GetString();
	QVariant val = g_pGameFun->UserCheckBoxDialog(sMsg, sDefaultVal);
	val = val.toBool();
	bool bTrans = args.Count() >= 3 ? args[3].GetBoolean() : true;
	TransVariantToLua(L, val, bTrans);
	g_pGameFun->SetScriptInputVarData(sMsg, val);
	emit g_pGameCtrl->signal_addOneScriptInputVar(TInputType_CheckBox, sMsg, val, "");
	return 1;
}

int CGLuaFun::Lua_ClearSysCue(LuaState *L)
{
	g_pGameFun->ClearSysCue();
	return 0;
}

int CGLuaFun::Lua_BeginAutoAction(LuaState *L)
{
	g_pGameFun->begin_auto_action();
	return 0;
}

int CGLuaFun::Lua_EndAutoAction(LuaState *L)
{
	g_pGameFun->end_auto_action();
	g_pGameFun->waitEndAutoEncounterAction();
	return 0;
}

int CGLuaFun::Lua_GetMapName(LuaState *L)
{
	QString sName = g_pGameFun->GetMapName();
	//	qDebug() << sName;
	L->PushString(sName.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_GetMapNumber(LuaState *L)
{
	int nNumber = g_pGameFun->GetMapIndex();
	//	qDebug() << sName;
	L->PushInteger(nNumber);
	return 1;
}

int CGLuaFun::Lua_GetMapFloorNumberFromName(LuaState *L)
{
	LuaStack args(L);
	bool bSerial = args.Count() > 0 ? args[1].GetBoolean() : false;
	bool bBack = args.Count() > 1 ? args[2].GetBoolean() : false;
	int floor = g_pGameFun->GetMapFloorNumberFromName(bSerial, bBack);
	L->PushInteger(floor);
	return 1;
}

int CGLuaFun::Lua_GetNumberFromName(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() == 0)
		return 0;

	QString sName = args[1].GetString();
	bool bSerial = args.Count() >= 2 ? args[2].GetBoolean() : false;
	bool bBack = args.Count() >= 3 ? args[3].GetBoolean() : false;
	int floor = g_pGameFun->GetNumberFromName(sName, bSerial, bBack);
	L->PushInteger(floor);
	return 1;
}

int CGLuaFun::Lua_GetMapData(LuaState *L)
{
	CGA::cga_map_cells_t cells;
	if (g_CGAInterface->GetMapCollisionTable(true, cells))
	{
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		tableObj.SetInteger("x", cells.x_bottom);
		tableObj.SetInteger("y", cells.y_bottom);
		tableObj.SetInteger("width", cells.x_size);
		tableObj.SetInteger("height", cells.y_size);
		tableObj.SetString("filename", cells.filename.c_str());

		LuaObject subTableObj(L);
		subTableObj.AssignNewTable();
		for (int i = 0; i < cells.cell.size(); ++i)
		{
			subTableObj.SetInteger(i + 1, cells.cell.at(i));
		}
		tableObj.SetObject("data", subTableObj);
		tableObj.Push(L); //这个不能省 省了就无效报错了
		return 1;
	}
	return 0;
}

int CGLuaFun::Lua_AutoWalkMaze(LuaState *L)
{
	LuaStack args(L);
	bool bRet = false;
	int isDownMap = 0;
	int isNearFar = 1;
	QString filterList = "";
	if (args.Count() > 0)
		isDownMap = args[1].GetInteger();
	if (args.Count() > 1)
		filterList = args[2].GetString();
	if (args.Count() > 2)
		isNearFar = args[3].GetInteger();
	bRet = g_pGameFun->AutoWalkMaze(isDownMap, filterList, isNearFar);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_AutoWalkRandomMaze(LuaState *L)
{
	LuaStack args(L);
	QString tgtName = args.Count() > 0 ? args[1].GetString() : "";
	g_pGameFun->SetTargetMazeName(tgtName);
	g_pGameFun->AutoWalkRandomMaze();
	return 0;
}

int CGLuaFun::Lua_GetAppRunPath(LuaState *L)
{
	QString sName = QApplication::applicationDirPath();
	sName += "//";
	//	qDebug() << sName;
	L->PushString(sName.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_LoadScript(LuaState *L)
{
	LuaStack args(L);
	QString strScriptName = args[1].GetString();
	QString scriptPath = strScriptName;
	//查看传进来路径是否存在，不存在先查找当前脚本目录下
	if (QFile::exists(scriptPath) == false)
	{
		QFileInfo fileinfo(m_lastScriptPath);
		QString curDir = fileinfo.absolutePath();
		scriptPath = curDir + "//" + scriptPath;
	}
	if (QFile::exists(scriptPath) == false)
	{
		scriptPath = QApplication::applicationDirPath() + "//" + strScriptName;
	}
	QFile file(scriptPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Lua_LoadScript 文件不存在" << strScriptName;
		return 0;
	}
	QString scriptData;
	QTextCodec *tc = QTextCodec::codecForName("GBK");
	QByteArray firstLine = file.readLine(); //不要第一行

	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		scriptData.append(line);
	}
	file.close();
	int val = L->LoadStringData(scriptData.toStdString().c_str());
	/*if (val == LUA_OK)
	{
		L->GetFEnv()
		L->Push();
	}*/
	//L->DoString(scriptData.toStdString().c_str());
	//	emit g_pGameCtrl->signal_loadScript(QString::fromStdString(strScriptName));
	return 1;
}

int CGLuaFun::Lua_ImportScript(LuaState *L)
{
	LuaStack args(L);
	QString strScriptName = args[1].GetString();
	QString scriptPath = strScriptName;
	//查看传进来路径是否存在，不存在先查找当前脚本目录下
	if (QFile::exists(scriptPath) == false)
	{
		QFileInfo fileinfo(m_lastScriptPath);
		QString curDir = fileinfo.absolutePath();
		scriptPath = curDir + "//" + scriptPath;
	}
	if (QFile::exists(scriptPath) == false)
	{
		scriptPath = QApplication::applicationDirPath() + "//" + strScriptName;
	}
	QFile file(scriptPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Lua_LoadScript 文件不存在" << strScriptName;
		return 0;
	}
	QString scriptData;
	QTextCodec *tc = QTextCodec::codecForName("GBK");
	QByteArray firstLine = file.readLine(); //不要第一行

	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		scriptData.append(line);
	}
	file.close();
	//	return luaL_loadbuffer(LuaState_to_lua_State(this), str, strlen(str), str);
	//	L->LoadBuffer(scriptData.toStdString().c_str(), scriptData.size(), scriptData.toStdString().c_str());
	L->LoadStringData(scriptData.toStdString().c_str());
	//	emit g_pGameCtrl->signal_loadScript(QString::fromStdString(strScriptName));
	return 1;
}

int CGLuaFun::Lua_SwitchScript(LuaState *L)
{
	LuaStack args(L);
	QString strScriptName = args[1].GetString();
	emit g_pGameCtrl->signal_loadScript(strScriptName);
	return 0;
}

int CGLuaFun::Lua_ExecScript(LuaState *L)
{
	LuaStack args(L);
	QString strScriptName = args[1].GetString();
	QString scriptPath = strScriptName;
	//查看传进来路径是否存在，不存在先查找当前脚本目录下
	if (QFile::exists(scriptPath) == false)
	{
		QFileInfo fileinfo(m_lastScriptPath);
		QString curDir = fileinfo.absolutePath();
		scriptPath = curDir + "//" + scriptPath;
	}
	if (QFile::exists(scriptPath) == false)
	{
		scriptPath = QApplication::applicationDirPath() + "//" + strScriptName;
	}
	QFile file(scriptPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Lua_LoadScript 文件不存在" << strScriptName;
		return 0;
	}
	QString scriptData;
	QTextCodec *tc = QTextCodec::codecForName("GBK");
	QByteArray firstLine = file.readLine(); //不要第一行

	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		scriptData.append(line);
	}
	file.close();
	L->DoString(scriptData.toStdString().c_str());
	//	emit g_pGameCtrl->signal_loadScript(QString::fromStdString(strScriptName));
	return 1;
}

int CGLuaFun::Lua_StopScript(LuaState *L)
{
	return 0;
}

int CGLuaFun::Lua_TownPortalScroll(LuaState *L)
{
	g_pGameFun->Logout();
	return 0;
}

int CGLuaFun::Lua_LogoutServer(LuaState *L)
{
	g_pGameFun->LogoutServer();
	return 0;
}

int CGLuaFun::Lua_WaitTime(LuaState *L)
{
	LuaStack args(L);
	int ntime = args[1].GetInteger();
	g_pGameFun->WaitTime(ntime);
	return 0;
}

int CGLuaFun::Lua_AutoMove(LuaState *L)
{
	LuaStack args(L);

	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	QString sMapName;
	if (args.Count() > 2)
	{
		sMapName = args[3].GetString();
	}
	int ntimeout = 0;
	if (args.Count() > 3)
	{
		ntimeout = args[4].GetInteger();
	}
	qDebug() << "Lua_AutoMove";
	g_pGameFun->AutoMoveToEx(x, y, sMapName, ntimeout);
	return 0;
}

int CGLuaFun::Lua_MovePos(LuaState *L)
{
	LuaStack args(L);

	int x = args[1].GetInteger();
	int y = args[2].GetInteger();	
	qDebug() << "Lua_MovePos";
	g_CGAInterface->WalkTo(x, y);
	return 0;
}

int CGLuaFun::Lua_WaitMovePos(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	QString sMapName = args.Count() >= 3 ? args[3].GetString() : "";
	int tgtx = args.Count() >= 4 ? args[4].GetInteger():0;
	int tgty = args.Count() >= 5 ? args[5].GetInteger():0;
	int timeOut = args.Count() >=6 ? args[6].GetInteger():10000;
	g_pGameFun->MoveTo(x, y,sMapName,tgtx,tgty,timeOut);
	return 0;
}

int CGLuaFun::Lua_MoveGo(LuaState *L)
{
	LuaStack args(L);
	int ndir = args[1].GetInteger();
	g_pGameFun->MoveGo(ndir);
	return 0;
}

int CGLuaFun::Lua_MoveToNpcNear(LuaState *L)
{
	LuaStack args(L);
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	int d = args[3].GetInteger();
	g_pGameFun->MoveToNpcNear(x, y, d);
	return 0;
}
//修改搜索函数 增加回调lua函数，用来判断是否中断搜索
//目前没有太多测试数据，先打开此功能
int CGLuaFun::Lua_SearchMap(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	int searchType = args.Count() > 1 ? args[2].GetInteger() : 1;			   //默认搜索NPC
	QString sFilterPos = args.Count() > 2 ? args[3].GetString() : "";		   //过滤点
	QString lastRegisterFunName = args.Count() > 3 ? args[4].GetString() : ""; //回调函数名

	std::function<QVariantList(QPoint findPos, QPoint nextPos)> callBackFun = [&](QPoint findPos, QPoint nextPos) -> QVariantList
	{
		lua_getglobal(L->GetCState(), lastRegisterFunName.toStdString().c_str());
		qDebug() << "回调" << findPos << nextPos;
		int lua_ret_argc = 3; //lua函数返回数量
		lua_pushinteger(L->GetCState(), findPos.x());
		lua_pushinteger(L->GetCState(), findPos.y());
		lua_pushinteger(L->GetCState(), nextPos.x());
		lua_pushinteger(L->GetCState(), nextPos.y());
		if (lua_pcall(L->GetCState(), 4, lua_ret_argc, 0) == LUA_OK) //调用无错  则栈上拿值返回
		{
			bool bRet = lua_toboolean(L->GetCState(), -3);
			int findx = lua_tointeger(L->GetCState(), -2);
			int findy = lua_tointeger(L->GetCState(), -1);
			lua_pop(L->GetCState(), lua_ret_argc);
			return QVariantList() << bRet << findx << findy;
		}
		return QVariantList();
	};
	QPoint nextPos;
	QPoint findPos;
	bool bRet = g_pGameFun->SearchMapEx(sName, findPos, nextPos, searchType, sFilterPos, callBackFun);
	L->PushBoolean(bRet);
	L->PushInteger(findPos.x());
	L->PushInteger(findPos.y());
	L->PushInteger(nextPos.x());
	L->PushInteger(nextPos.y());
	return 5;
}

int CGLuaFun::Lua_DownloadMap(LuaState *L)
{
	g_pGameFun->DownloadMap();
	return 0;
}

int CGLuaFun::Lua_GetNextMazeWarp(LuaState *L)
{
	LuaStack args(L);
	QPoint tgtPos;
	if (args.Count() > 0)
	{
		bool bNearFar = args[1].GetBoolean();
		tgtPos = g_pGameFun->GetMazeEntrance(bNearFar);
	}
	else
	{
		tgtPos = g_pGameFun->GetMazeEntrance();
	}
	L->PushInteger(tgtPos.x());
	L->PushInteger(tgtPos.y());
	return 2;
}

int CGLuaFun::Lua_GetAllMazeWarpList(LuaState *L)
{
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	auto mazeWarpList = g_pGameFun->GetMazeEntranceList();
	for (int i = 0; i < mazeWarpList.size(); ++i)
	{
		LuaObject subTableObj(L);
		subTableObj.AssignNewTable();
		subTableObj.SetInteger("x", mazeWarpList[i].x());
		subTableObj.SetInteger("y", mazeWarpList[i].y());
		tableObj.SetObject(i + 1, subTableObj);
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_FindRandomSearchPath(LuaState *L)
{
	LuaStack args(L);
	int tx = args.Count() > 0 ? args[1].GetInteger() : 0;
	int ty = args.Count() > 1 ? args[2].GetInteger() : 0;
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	auto mazeWarpList = g_pGameFun->FindRandomSearchPath(tx, ty);
	qDebug() << "路线列表：" << mazeWarpList.size();
	for (int i = 0; i < mazeWarpList.size(); ++i)
	{
		LuaObject subTableObj(L);
		subTableObj.AssignNewTable();
		subTableObj.SetInteger("x", mazeWarpList[i].x());
		subTableObj.SetInteger("y", mazeWarpList[i].y());
		//subTableObj.Push(L);	//这个可以省 下面调用时候把对象设置进去了
		tableObj.SetObject(i + 1, subTableObj);
	}
	tableObj.Push(L); //这个不能省 省了就无效报错了
	return 1;
}

int CGLuaFun::Lua_GetMapUnits(LuaState *L)
{
	CGA::cga_map_units_t units;
	g_CGAInterface->GetMapUnits(units);
	CGA::cga_map_unit_t mapUnit;
	if (units.size() > 0)
	{
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		int index = 0;
		foreach (auto unit, units)
		{
			if (unit.valid && unit.model_id != 0)
			{
				LuaObject pTable(L);
				pTable.AssignNewTable();
				pTable.SetInteger("valid", unit.valid);
				pTable.SetInteger("type", unit.type);
				pTable.SetInteger("unit_id", unit.unit_id);
				pTable.SetInteger("model_id", unit.model_id);
				pTable.SetInteger("x", unit.xpos);
				pTable.SetInteger("y", unit.ypos);
				pTable.SetInteger("item_count", unit.item_count);
				pTable.SetInteger("injury", unit.injury);
				pTable.SetInteger("level", unit.level);
				pTable.SetInteger("flags", unit.flags);
				pTable.SetString("unit_name", unit.unit_name.c_str());
				pTable.SetString("nick_name", unit.nick_name.c_str());
				pTable.SetString("title_name", unit.title_name.c_str());
				pTable.SetString("item_name", unit.item_name.c_str());
				pTable.SetInteger("icon", unit.icon);
				tableObj.SetObject(++index, pTable);
			}
		}
		tableObj.Push(L);
		return 1;
	}
	return 0;
}

int CGLuaFun::Lua_FindMapUnit(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	int type = args.Count() > 1 ? args[2].GetInteger() : 1;
	auto unit = g_pGameFun->FindMapUnit(sName, type);
	if (unit)
	{
		LuaObject pObj(L);
		pObj.AssignNewTable();
		pObj.SetInteger("valid", unit->valid);
		pObj.SetInteger("type", unit->type);
		pObj.SetInteger("unit_id", unit->unit_id);
		pObj.SetInteger("model_id", unit->model_id);
		pObj.SetInteger("x", unit->xpos);
		pObj.SetInteger("y", unit->ypos);
		pObj.SetInteger("item_count", unit->item_count);
		pObj.SetInteger("injury", unit->injury);
		pObj.SetInteger("level", unit->level);
		pObj.SetInteger("flags", unit->flags);
		pObj.SetString("unit_name", unit->unit_name.c_str());
		pObj.SetString("nick_name", unit->nick_name.c_str());
		pObj.SetString("title_name", unit->title_name.c_str());
		pObj.SetString("item_name", unit->item_name.c_str());
		pObj.SetInteger("icon", unit->icon);
		pObj.Push(L);
		return 1;
	}
	return 0;
}

int CGLuaFun::Lua_IsReachableTarget(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;

	int x1 = args[1].GetInteger();
	int y1 = args[2].GetInteger();
	bool bRet = false;
	if (args.Count() > 3)
	{
		int x2 = args[3].GetInteger();
		int y2 = args[4].GetInteger();
		bRet = g_pGameFun->IsReachableTargetEx(x1, y1, x2, y2);
	}
	else
		bRet = g_pGameFun->IsReachableTarget(x1, y1);

	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_GetRandomSpace(LuaState *L)
{
	LuaStack args(L);
	QPoint tgtPos;
	if (args.Count() < 1)
	{
		QPoint curPos = g_pGameFun->GetMapCoordinate();
		//没有参数 默认查找当前周围1格空格
		tgtPos = g_pGameFun->GetRandomSpace(curPos.x(), curPos.y());
	}
	else if (args.Count() == 2)
	{
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		tgtPos = g_pGameFun->GetRandomSpace(x, y);
	}
	else if (args.Count() > 2)
	{
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		int d = args[3].GetInteger();
		tgtPos = g_pGameFun->GetRandomSpace(x, y, d);
	}

	L->PushInteger(tgtPos.x());
	L->PushInteger(tgtPos.y());
	return 2;
}

int CGLuaFun::Lua_FindToRandomEntry(LuaState *L)
{
	LuaStack args(L);
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	int w = args[3].GetInteger();
	int h = args[4].GetInteger();
	QString sFilter;
	if (args.Count() >= 5)
	{
		sFilter = args[5].GetString();
	}
	bool bRet = g_pGameFun->FindToRandomEntryEx(x, y, w, h, sFilter);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_UseItem(LuaState *L)
{
	LuaStack args(L);
	if (L->IsString(1))
	{
		QString sName = args[1].GetString();
		g_pGameFun->UseItem(sName);
	}
	else
	{
		int nPos = args[1].GetInteger();
		g_pGameFun->UseItemPos(nPos);
	}
	return 0;
}

int CGLuaFun::Lua_LaunchTrade(LuaState *L)
{
	LuaStack args(L);

	QString sName = args.Count() > 0 ? args[1].GetString() : "";
	QString myTradeData = args.Count() > 1 ? args[2].GetString() : "";
	QString sTradeData = args.Count() > 1 ? args[3].GetString() : "";
	int nTimeOut = args.Count() > 2 ? args[4].GetInteger() : 3000;
	bool bRet = g_pGameFun->LaunchTrade(sName, myTradeData, sTradeData, nTimeOut);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_WaitTrade(LuaState *L)
{
	LuaStack args(L);
	QString sName = args.Count() > 0 ? args[1].GetString() : "";
	QString myTradeData = args.Count() > 1 ? args[2].GetString() : "";
	QString sTradeData = args.Count() > 2 ? args[3].GetString() : "";
	int nTimeOut = args.Count() > 3 ? args[4].GetInteger() : 3000;
	bool bRet = g_pGameFun->WaitTrade(sName, myTradeData, sTradeData, nTimeOut);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_TradeInternal(LuaState *L)
{
	LuaStack args(L);
	QString sName = args.Count() > 0 ? args[1].GetString() : "";
	QString myTradeData = args.Count() > 1 ? args[2].GetString() : "";
	QString sTradeData = args.Count() > 2 ? args[3].GetString() : "";
	int nTimeOut = args.Count() > 3 ? args[4].GetInteger() : 5000;
	bool bRet = g_pGameFun->TradeInternal(sName, myTradeData, sTradeData, nTimeOut);
	L->PushBoolean(bRet);
	return 1;
}

//已接收最新交易消息
int CGLuaFun::Lua_GetLastRecvTradeDlgInfo(LuaState *L)
{
	LuaStack args(L);
	int timeInterval = args.Count() > 0 ? args[1].GetInteger() : 5000;
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	auto tradeInfo = g_pGameCtrl->GetLastTradeDialog(timeInterval);
	if (tradeInfo)
	{
		tableObj.SetString("name", tradeInfo->name.c_str());
		tableObj.SetInteger("level", tradeInfo->level);
	}
	else
	{
		tableObj.SetString("name", "");
		tableObj.SetString("level", 0);
	}
	tableObj.Push(L);
	return 1;
}
//已接收所有交易消息
int CGLuaFun::Lua_GetAllRecvTradeDlgInfo(LuaState *L)
{
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	auto pDlgList = g_pGameCtrl->GetAllRecvTopicMsgList();
	for (int i = 0; i < pDlgList.size(); ++i)
	{
		auto tradeInfo = pDlgList[i];	
		LuaObject subObj(L);
		subObj.AssignNewTable();
		tableObj.SetString("name", tradeInfo->name.c_str());
		tableObj.SetInteger("level", tradeInfo->level);
		tableObj.SetObject(i + 1, subObj);
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_WaitTradeDlg(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 10000;
	auto result = g_pGameFun->WaitTradeDialog(timeout);
	if (result)
	{
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		tableObj.SetInteger("level", result->level);
		tableObj.SetString("name", result->name.c_str());
		tableObj.Push(L);
		return 1;
	}
	return 0;
}

int CGLuaFun::Lua_TradeAddStuffs(LuaState *L)
{
	LuaStack args(L);
	//3个参数 可以为空
	//if (args.Count() < 3)
	//{
	//	qDebug() << "Lua_TradeAddStuffs 参数错误！";
	//	return 0;
	//}
	//物品 宠物 金币
	int myGold = 0;
	CGA::cga_sell_items_t cgaTradeItems;
	CGA::cga_sell_pets_t myPets;

	LuaObject itemData = args[1];
	if (!itemData.IsNil())
	{
		for (LuaTableIterator it(itemData); it; it.Next())
		{
			auto itData = it.GetValue();
			if (itData.IsTable())
			{
				int itemid = itData.GetByName("id").ToInteger();
				int itempos = itData.GetByName("pos").ToInteger();
				int itemcount = itData.GetByName("count").ToInteger();
				CGA::cga_sell_item_t cgaItem;
				cgaItem.itemid = itemid;
				cgaItem.itempos = itempos;
				cgaItem.count = itemcount;
				cgaTradeItems.push_back(cgaItem);
			}
		}
	}
	LuaObject petData = args[2];
	if (!petData.IsNil())
	{
		for (LuaTableIterator it(petData); it; it.Next())
		{
			auto itData = it.GetValue().ToInteger();
		}
	}
	int nGoldData = args.Count() > 2 ? args[3].GetInteger() : 0;
	g_CGAInterface->TradeAddStuffs(cgaTradeItems, myPets, myGold);
	return 0;
}

int CGLuaFun::Lua_ThrowItemName(LuaState *L)
{
	LuaStack args(L);
	if (L->IsInteger(1))
	{
		int nPos = args[1].GetInteger();
		g_pGameFun->ThrowItemPos(nPos);
	}
	else
	{
		QString sName = args[1].GetString();
		g_pGameFun->ThrowItemName(sName);
	}
	return 0;
}

int CGLuaFun::Lua_ThrowPosItem(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	
	int nPos = args[1].GetInteger();
	g_pGameFun->ThrowItemPos(nPos);
	return 0;
}

int CGLuaFun::Lua_ThrowNoFullItemName(LuaState *L)
{
	LuaStack args(L);
	QString sName = L->IsString(1) ? args[1].GetString() : "";
	int nCount = L->IsInteger(2) ? args[2].GetInteger() : 0;
	g_pGameFun->ThrowNotFullItemName(sName, nCount);
	return 0;
}

int CGLuaFun::Lua_PickupItem(LuaState *L)
{
	LuaStack args(L);
	int ndir = L->IsInteger(1) ? args[1].GetInteger() : 0;
	g_pGameFun->PickupItem(ndir);
	return 0;
}

int CGLuaFun::Lua_PileItem(LuaState *L)
{
	LuaStack args(L);
	QString sName = L->IsString(1) ? args[1].GetString() : "";
	int nCount = L->IsInteger(2) ? args[2].GetInteger() : 0;
	g_pGameFun->PileItem(sName, nCount);
	return 0;
}

int CGLuaFun::Lua_SplitItem(LuaState *L)
{
	LuaStack args(L);
	QString sName = args.Count() > 0 ? args[1].GetString() : "";
	int nCount = args.Count() > 1 ? args[2].GetInteger() : 0;
	int nFlags = args.Count() > 2 ? args[3].GetInteger() : 0;	
	g_pGameFun->SplitItem(sName, nCount,nFlags);
	return 0;
}
int CGLuaFun::Lua_SplitPosItem(LuaState *L)
{
	LuaStack args(L);
	int nPos = args.Count() > 0 ? args[1].GetInteger() : -1;
	int nCount = args.Count() > 1 ? args[2].GetInteger() : 0;
	int nFlags = args.Count() > 2 ? args[3].GetInteger() : 0;
	g_pGameFun->SplitPosItem(nPos, nCount, nFlags);
	return 0;
}

int CGLuaFun::Lua_SwitchItem(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;

	int srcPos = args[1].GetInteger();
	int dstPos = args[2].GetInteger();
	int nCount = args.Count() > 2 ? args[3].GetInteger() : -1;
	bool bRet = false;
	g_CGAInterface->MoveItem(srcPos, dstPos, nCount, bRet);
	return 0;
}

int CGLuaFun::Lua_EquipItem(LuaState *L)
{
	LuaStack args(L);
	int nPos = -1;
	if (args.Count() > 1)
		nPos = args[2].GetInteger();
	if (L->IsString(1))
	{
		QString sName = args[1].GetString();
		g_pGameFun->Equip(sName, nPos);
	}
	else if (L->IsInteger(1))
	{
		int nCode = args[1].GetInteger();
		g_pGameFun->EquipEx(nCode, nPos);
	}
	return 0;
}

int CGLuaFun::Lua_EquipItemEx(LuaState *L)
{
	LuaStack args(L);
	int nCode = args[1].GetInteger();
	int nPos = -1;
	if (args.Count() > 1)
		nPos = args[2].GetInteger();
	g_pGameFun->EquipEx(nCode, nPos);
	return 0;
}

int CGLuaFun::Lua_UnEquipItem(LuaState *L)
{
	LuaStack args(L);
	if (L->IsString(1))
	{
		QString sName = args[1].GetString();
		g_pGameFun->UnEquip(sName);
	}
	else if (L->IsInteger(1))
	{
		int nCode = args[1].GetInteger();
		g_pGameFun->UnEquipEx(nCode);
	}
	return 0;
}

int CGLuaFun::Lua_UnEquipItemEx(LuaState *L)
{
	LuaStack args(L);
	int nCode = args[1].GetInteger();
	g_pGameFun->UnEquipEx(nCode);
	return 0;
}

int CGLuaFun::Lua_GetItemDurability(LuaState *L)
{
	LuaStack args(L);
	int nVal = -1;
	if (L->IsInteger(1))
	{
		int nPos = args[1].GetInteger();
		nVal = g_pGameFun->GetPosItemDurability(nPos);
	}
	else if (L->IsString(1))
	{
		QString sName = args[1].GetString();
		nVal = g_pGameFun->GetItemDurability(sName);
	}
	L->PushInteger(nVal);
	return 1;
}

int CGLuaFun::Lua_TurnAbout(LuaState *L)
{
	LuaStack args(L);
	int nDir = args[1].GetInteger();
	g_pGameFun->TurnAbout(nDir);
	return 0;
}

int CGLuaFun::Lua_TurnAboutEx(LuaState *L)
{
	LuaStack args(L);
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();

	g_pGameFun->TurnAboutEx(x, y);
	return 0;
}

int CGLuaFun::Lua_TurnAboutEx2(LuaState *L)
{
	LuaStack args(L);
	QString sDir = args[1].GetString();
	g_pGameFun->TurnAboutEx2(sDir);
	return 0;
}

int CGLuaFun::Lua_TurnAboutPointDir(LuaState *L)
{
	LuaStack args(L);
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();

	g_pGameFun->TurnAboutPointDir(x, y);
	return 0;
}

int CGLuaFun::Lua_Npc(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;

	int opt = args[1].GetInteger();
	int index = args[2].GetInteger();
	g_pGameFun->Npc(opt, index);
	return 0;
}

int CGLuaFun::Lua_TalkNpc(LuaState *L)
{
	LuaStack args(L);
	bool bRet = false;
	if (args.Count() >= 2)
	{ //x y 次数
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		bRet = g_pGameFun->TalkNpc(x, y);
	}
	else
	{
		int dir = args[1].GetInteger();
		bRet = g_pGameFun->TalkNpcEx(dir);
	}
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_TalkNpcSelectYes(LuaState *L)
{
	LuaStack args(L);
	bool bRet = false;
	if (args.Count() == 2)
	{ //x y 次数
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		bRet = g_pGameFun->TalkNpcSelectYes(x, y, 100);
	}
	else if (args.Count() == 1) //2个以及以下 默认都是方向
	{
		int dir = args[1].GetInteger();
		bRet = g_pGameFun->TalkNpcSelectYesEx(dir, 100);
	}
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_TalkNpcPosSelectYes(LuaState *L)
{
	LuaStack args(L);
	bool bRet = false;
	if (args.Count() >= 2)
	{ //x y 次数
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		int count = args.Count() > 2 ? args[3].GetInteger() : 32;
		bRet = g_pGameFun->TalkNpcPosSelectYes(x, y, count);
	}

	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_TalkNpcSelectNo(LuaState *L)
{
	LuaStack args(L);
	bool bRet = false;
	if (args.Count() == 2)
	{ //x y 次数
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		bRet = g_pGameFun->TalkNpcSelectNo(x, y, 100);
	}
	else if (args.Count() == 1) //2个以及以下 默认都是方向
	{
		int dir = args[1].GetInteger();
		bRet = g_pGameFun->TalkNpcSelectNoEx(dir, 100);
	}
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_TalkNpcPosSelectNo(LuaState *L)
{
	LuaStack args(L);
	bool bRet = false;
	if (args.Count() >= 2)
	{ //x y 次数
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		int count = args.Count() > 2 ? args[3].GetInteger() : 32;
		bRet = g_pGameFun->TalkNpcPosSelectNo(x, y, count);
	}
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_GetPlayerData(LuaState *L)
{
	LuaStack args(L);
	QString opt = args[1].GetString();
	QVariant val = g_pGameFun->GetCharacterData(opt);
	bool bTrans = args.Count() >= 2 ? args[2].GetBoolean() : true;
	TransVariantToLua(L, val, bTrans);
	return 1;
}

int CGLuaFun::Lua_GetPlayerAllData(LuaState *L)
{
	LuaStack args(L);
	QString opt = args[1].GetString();

	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	CGA::cga_player_info_t info;
	if (g_CGAInterface->GetPlayerInfo(info))
	{
		tableObj.SetString("gid", info.gid.c_str());
		tableObj.SetString("name", info.name.c_str());
		tableObj.SetString("job", info.job.c_str());
		tableObj.SetInteger("level", info.level);
		tableObj.SetInteger("gold", info.gold);
		tableObj.SetInteger("hp", info.hp);
		tableObj.SetInteger("maxhp", info.maxhp);
		tableObj.SetInteger("mp", info.mp);
		tableObj.SetInteger("maxmp", info.maxmp);
		tableObj.SetInteger("xp", info.xp);
		tableObj.SetInteger("maxxp", info.maxxp);
		tableObj.SetInteger("petid", info.petid);
		tableObj.SetInteger("unitid", info.unitid);
		tableObj.SetInteger("punchclock", info.punchclock);
		tableObj.SetInteger("usingpunchclock", info.usingpunchclock);
		tableObj.SetInteger("health", info.health);
		tableObj.SetInteger("souls", info.souls);
		tableObj.SetInteger("direction", info.direction);
		tableObj.SetInteger("manu_endurance", info.manu_endurance);
		tableObj.SetInteger("manu_skillful", info.manu_skillful);
		tableObj.SetInteger("manu_intelligence", info.manu_intelligence);
		tableObj.SetInteger("value_charisma", info.value_charisma);
		LuaObject titleObj(L);
		titleObj.AssignNewTable();
		for (size_t i = 0; i < info.titles.size(); ++i)
		{
			std::string sTitle = info.titles.at(i);
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", sTitle.c_str());
			titleObj.SetObject(i + 1, subObj);
		}
		tableObj.SetObject("titles", titleObj);
		LuaObject subObj(L);
		subObj.AssignNewTable();
		subObj.SetInteger("points_remain", info.detail.points_remain);
		subObj.SetInteger("points_endurance", info.detail.points_endurance);
		subObj.SetInteger("points_strength", info.detail.points_strength);
		subObj.SetInteger("points_defense", info.detail.points_defense);
		subObj.SetInteger("points_agility", info.detail.points_agility);
		subObj.SetInteger("points_magical", info.detail.points_magical);
		subObj.SetInteger("value_attack", info.detail.value_attack);
		subObj.SetInteger("value_defensive", info.detail.value_defensive);
		subObj.SetInteger("value_agility", info.detail.value_agility);
		subObj.SetInteger("value_spirit", info.detail.value_spirit);
		subObj.SetInteger("value_recovery", info.detail.value_recovery);
		subObj.SetInteger("resist_poison", info.detail.resist_poison);
		subObj.SetInteger("resist_sleep", info.detail.resist_sleep);
		subObj.SetInteger("resist_medusa", info.detail.resist_medusa);
		subObj.SetInteger("resist_drunk", info.detail.resist_drunk);
		subObj.SetInteger("resist_chaos", info.detail.resist_chaos);
		subObj.SetInteger("resist_forget", info.detail.resist_forget);
		subObj.SetInteger("fix_critical", info.detail.fix_critical);
		subObj.SetInteger("fix_strikeback", info.detail.fix_strikeback);
		subObj.SetInteger("fix_accurancy", info.detail.fix_accurancy);
		subObj.SetInteger("fix_dodge", info.detail.fix_dodge);
		subObj.SetInteger("element_earth", info.detail.element_earth);
		subObj.SetInteger("element_water", info.detail.element_water);
		subObj.SetInteger("element_fire", info.detail.element_fire);
		subObj.SetInteger("element_wind", info.detail.element_wind);
		tableObj.SetObject("detail", subObj);
		LuaObject descObj(L);
		descObj.AssignNewTable();
		descObj.SetInteger("changeBits", info.persdesc.changeBits);
		descObj.SetInteger("sellIcon", info.persdesc.sellIcon);
		descObj.SetString("sellString", info.persdesc.sellString.c_str());
		descObj.SetInteger("buyIcon", info.persdesc.buyIcon);
		descObj.SetString("buyString", info.persdesc.buyString.c_str());
		descObj.SetInteger("wantIcon", info.persdesc.wantIcon);
		descObj.SetString("wantString", info.persdesc.wantString.c_str());
		descObj.SetString("descString", info.persdesc.descString.c_str());
		tableObj.SetObject("persdesc", descObj);
	}
	LuaObject skillObj(L);
	skillObj.AssignNewTable();
	CGA::cga_skills_info_t skillsinfo;
	if (g_CGAInterface->GetSkillsInfo(skillsinfo))
	{
		for (size_t i = 0; i < skillsinfo.size(); ++i)
		{
			const CGA::cga_skill_info_t &skinfo = skillsinfo.at(i);
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", skinfo.name.c_str());
			subObj.SetInteger("index", skinfo.index);
			subObj.SetInteger("lv", skinfo.lv);
			subObj.SetInteger("maxlv", skinfo.maxlv);
			subObj.SetInteger("xp", skinfo.xp);
			subObj.SetInteger("maxxp", skinfo.maxxp);
			subObj.SetInteger("pos", skinfo.pos);

			CGA::cga_subskills_info_t subskillsinfo;
			LuaObject subSkillObj(L);
			subSkillObj.AssignNewTable();
			//获取合成物品信息
			CGA::cga_craft_info_t craftInfo;
			if (g_CGAInterface->GetSubSkillsInfo(skinfo.index, subskillsinfo))
			{
				for (size_t j = 0; j < subskillsinfo.size(); ++j)
				{
					const CGA::cga_subskill_info_t &subskinfo = subskillsinfo.at(j);

					LuaObject subTbObj(L);
					subTbObj.AssignNewTable();
					subTbObj.SetString("name", subskinfo.name.c_str());
					subTbObj.SetString("info", subskinfo.info.c_str());
					subTbObj.SetInteger("level", subskinfo.level);
					subTbObj.SetInteger("cost", subskinfo.cost);
					subTbObj.SetInteger("flags", subskinfo.flags);
					subTbObj.SetBoolean("available", subskinfo.available);
					subSkillObj.SetObject(j + 1, subTbObj);
				}
			}
			subObj.SetObject("subskill", subSkillObj);

			skillObj.SetObject(i + 1, subObj);
		}
	}
	tableObj.SetObject("skill", skillObj);
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetAllItemData(LuaState *L)
{
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	CGA::cga_items_info_t itemsinfo;
	if (g_CGAInterface->GetItemsInfo(itemsinfo))
	{
		for (size_t i = 0; i < itemsinfo.size(); ++i)
		{
			const CGA::cga_item_info_t &info = itemsinfo.at(i);
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", info.name.c_str());
			subObj.SetString("attr", info.attr.c_str());
			subObj.SetString("info", info.info.c_str());
			subObj.SetInteger("itemid", info.itemid);
			subObj.SetInteger("count", info.count);
			subObj.SetInteger("pos", info.pos);
			subObj.SetInteger("level", info.level);
			subObj.SetInteger("type", info.type);
			subObj.SetBoolean("assessed", info.assessed);
			int curDura = 0;
			int maxDura = 0;
			g_pGameFun->ParseItemDurabilityEx(QString::fromStdString(info.attr), curDura, maxDura);
			subObj.SetInteger("durability", curDura);
			subObj.SetInteger("maxdurability", maxDura);
			tableObj.SetObject(i + 1, subObj);
		}
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetNextTitleData(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;

	QString sTitle = args[1].GetString();
	int grade = args[2].GetInteger();
	auto nextData = g_pGameFun->GetNextTitleData(sTitle, grade);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetInteger("gold", nextData.value("nextGold").toInt());
	tableObj.SetString("title", nextData.value("nextTitle").toString().toStdString().c_str());
	tableObj.SetInteger("grade", nextData.value("nextGrade").toInt());
	tableObj.SetInteger("time", nextData.value("nextTime").toInt());
	tableObj.SetInteger("count", nextData.value("nextCount").toInt());
	//for (auto it=nextData.begin();it!= nextData.end();++it)
	//{
	//	tableObj.SetString(it.key().toStdString().c_str(), it.value().toString().toStdString().c_str());
	//}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetPetData(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int petId = args.Count() > 0 ? args[1].GetInteger() : 0;
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetString("name", "");
	tableObj.SetString("realname", "");
	tableObj.SetInteger("level", 0);
	tableObj.SetInteger("hp",0);
	tableObj.SetInteger("maxhp", 0);
	tableObj.SetInteger("mp", 0);
	tableObj.SetInteger("maxmp", 0);
	tableObj.SetInteger("xp", 0);
	tableObj.SetInteger("maxxp", 0);
	tableObj.SetInteger("health", 0);
	tableObj.SetInteger("flags", 0);
	tableObj.SetInteger("race", 0);
	tableObj.SetInteger("loyality", 0);
	tableObj.SetInteger("battle_flags", 0);
	tableObj.SetInteger("state", 0);
	tableObj.SetInteger("index", 0);
	tableObj.SetInteger("grade", 0);
	tableObj.SetInteger("minGrade", 0);
	tableObj.SetInteger("maxGrade", 0);
	CGA::cga_pet_info_t info;
	if (g_CGAInterface->GetPetInfo(petId, info))
	{
		
		tableObj.SetString("name", info.name.c_str());
		tableObj.SetString("realname", info.realname.c_str());
		tableObj.SetInteger("level", info.level);
		tableObj.SetInteger("hp", info.hp);
		tableObj.SetInteger("maxhp", info.maxhp);
		tableObj.SetInteger("mp", info.mp);
		tableObj.SetInteger("maxmp", info.maxmp);
		tableObj.SetInteger("xp", info.xp);
		tableObj.SetInteger("maxxp", info.maxxp);
		tableObj.SetInteger("health", info.health);
		tableObj.SetInteger("flags", info.flags);
		tableObj.SetInteger("race", info.race);
		tableObj.SetInteger("loyality", info.loyality);
		tableObj.SetInteger("battle_flags", info.battle_flags);
		tableObj.SetInteger("state", info.state);
		tableObj.SetInteger("index", info.index);
		if (info.level == 1) //暂时只有1级 有档次计算
		{
			auto pCalcData = g_pGamePetCalc->ParseLine(g_pGameFun->GetPetCalcBpData(info));
			if (pCalcData)
			{
				tableObj.SetInteger("grade", pCalcData->lossMin);
				tableObj.SetInteger("minGrade", pCalcData->lossMin);
				tableObj.SetInteger("maxGrade", pCalcData->lossMax);
			}
		}
		
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetAllPetData(LuaState *L)
{
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	CGA::cga_pets_info_t petsinfo;
	if (g_CGAInterface->GetPetsInfo(petsinfo))
	{
		for (size_t i = 0; i < petsinfo.size(); ++i)
		{
			const CGA::cga_pet_info_t &info = petsinfo.at(i);
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", info.name.c_str());
			subObj.SetString("realname", info.realname.c_str());
			subObj.SetInteger("level", info.level);
			subObj.SetInteger("hp", info.hp);
			subObj.SetInteger("maxhp", info.maxhp);
			subObj.SetInteger("mp", info.mp);
			subObj.SetInteger("maxmp", info.maxmp);
			subObj.SetInteger("xp", info.xp);
			subObj.SetInteger("maxxp", info.maxxp);
			subObj.SetInteger("health", info.health);
			subObj.SetInteger("flags", info.flags);
			subObj.SetInteger("race", info.race);
			subObj.SetInteger("loyality", info.loyality);
			subObj.SetInteger("battle_flags", info.battle_flags);
			subObj.SetInteger("state", info.state);
			subObj.SetInteger("index", info.index);
			if (info.level == 1) //暂时只有1级 有档次计算
			{
				auto pCalcData = g_pGamePetCalc->ParseLine(g_pGameFun->GetPetCalcBpData(info));
				if (pCalcData)
				{
					subObj.SetInteger("grade", pCalcData->lossMin);
					subObj.SetInteger("minGrade", pCalcData->lossMin);
					subObj.SetInteger("maxGrade", pCalcData->lossMax);
				}
			}
			tableObj.SetObject(i + 1, subObj);
		}
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_SetPetData(LuaState *L)
{
	LuaStack args(L);
	QString opt = args[1].GetString();
	QVariant data = args.Count() > 1 ? args[2].GetString() : "";
	int petIndex = args.Count() > 2 ? args[3].GetInteger() : -1;
	int val = g_pGameFun->SetPetData(opt, data, petIndex);
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetPlayereEquipData(LuaState *L)
{
	LuaStack args(L);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	CGA::cga_items_info_t itemsinfo;
	g_CGAInterface->GetItemsInfo(itemsinfo);
	int nIndex = 0;
	for (size_t i = 0; i < itemsinfo.size(); ++i)
	{
		const CGA::cga_item_info_t &info = itemsinfo.at(i);
		if (info.pos >= 0 && info.pos < 8)
		{
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", info.name.c_str());
			subObj.SetString("attr", info.attr.c_str());
			subObj.SetString("info", info.info.c_str());
			subObj.SetInteger("itemid", info.itemid);
			subObj.SetInteger("count", info.count);
			subObj.SetInteger("pos", info.pos);
			subObj.SetInteger("level", info.level);
			subObj.SetInteger("type", info.type);
			subObj.SetBoolean("assessed", info.assessed);
			tableObj.SetObject(++nIndex, subObj);
		}
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_ParseEquipData(LuaState *L)
{
	LuaStack args(L);
	QString attr = args[1].GetString();
	int nCur = 0, nMax = 0;
	g_pGameFun->ParseItemDurabilityEx(attr, nCur, nMax);
	L->PushInteger(nCur);
	L->PushInteger(nMax);
	return 2;
}

int CGLuaFun::Lua_GetBattlePetData(LuaState *L)
{
	LuaStack args(L);
	QString opt = args[1].GetString();
	QString nVal = args.Count() > 1 ? args[2].GetString() : "";
	QString nVal2 = args.Count() > 2 ? args[3].GetString() : "";
	int val = g_pGameFun->GetBattlePetData(opt, nVal, nVal2);
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetTeamData(LuaState *L)
{
	LuaStack args(L);
	QString opt = args[1].GetString();
	QVariant val = g_pGameFun->GetTeamData(opt);
	bool bTrans = args.Count() >= 2 ? args[2].GetBoolean() : true;
	TransVariantToLua(L, val, bTrans);
	return 1;
}

int CGLuaFun::Lua_GetAllTeammateData(LuaState *L)
{
	auto allTeamPlayers = g_pGameFun->GetTeamPlayers();
	// 返回全部
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	for (int i = 0; i < allTeamPlayers.size(); ++i)
	{
		auto teamPlayer = allTeamPlayers[i];
		LuaObject subObj(L);
		subObj.AssignNewTable();
		subObj.SetString("name", teamPlayer->name.toStdString().c_str());
		subObj.SetString("nick_name", teamPlayer->nick_name.toStdString().c_str());
		subObj.SetString("title_name", teamPlayer->title_name.toStdString().c_str());
		subObj.SetInteger("hp", teamPlayer->hp);
		subObj.SetInteger("maxhp", teamPlayer->maxhp);
		subObj.SetInteger("mp", teamPlayer->mp);
		subObj.SetInteger("maxmp", teamPlayer->maxmp);
		subObj.SetInteger("x", teamPlayer->x);
		subObj.SetInteger("y", teamPlayer->y);
		subObj.SetInteger("unit_id", teamPlayer->unit_id);
		subObj.SetInteger("injury", teamPlayer->injury);
		subObj.SetInteger("is_me", teamPlayer->is_me);
		subObj.SetInteger("level", teamPlayer->level);
		tableObj.SetObject(i + 1, subObj);
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_Renew(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() == 2)
	{
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		g_pGameFun->RenewEx(x, y);
	}
	else if (L->IsInteger(1)) //必须先判断inter再判断String 否则如果参数可转换为字符串 就进入字符串了
	{
		int nDir = L->IsInteger(1) ? args[1].GetInteger() : 0;
		g_pGameFun->Renew(nDir);
	}
	else
	{
		QString sDir = args[1].GetString();
		g_pGameFun->RenewEx2(sDir);
	}
	return 0;
}

int CGLuaFun::Lua_GetFriendCard(LuaState *L)
{
	LuaStack args(L);
	QString friendName = args.Count() > 0 ? args[1].GetString() : "";

	CGA::cga_cards_info_t cards;
	g_CGAInterface->GetCardsInfo(cards);
	if (!friendName.isEmpty())
	{
		for (auto info : cards)
		{
			if (info.name == friendName.toStdString())
			{
				LuaObject subObj(L);
				subObj.AssignNewTable();
				subObj.SetString("name", info.name.c_str());
				subObj.SetString("title", info.nickname.c_str());
				subObj.SetInteger("index", info.index);
				subObj.SetInteger("level", info.level);
				subObj.SetInteger("avatar", info.avatar);
				subObj.SetInteger("server", info.server);
				subObj.Push(L);
				return 1;
			}
		}
		return 0;
	}
	else
	{
		// 返回全部
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		for (size_t i = 0; i < cards.size(); i++)
		{
			CGA::cga_card_info_t info = cards[i];
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", info.name.c_str());
			subObj.SetString("title", info.nickname.c_str());
			subObj.SetInteger("index", info.index);
			subObj.SetInteger("level", info.level);
			subObj.SetInteger("avatar", info.avatar);
			subObj.SetInteger("server", info.server);
			tableObj.SetObject(i + 1, subObj);
		}
		tableObj.Push(L);
		return 1;
	}
	return 0;
}

int CGLuaFun::Lua_SetPlayerInfo(LuaState *L)
{
	//个人简介
	//CGA::cga_pers_desc_s
	//g_CGAInterface->ChangePersDesc()
	//更改称号名称 顺序
	//g_CGAInterface->ChangeTitleName();
	LuaStack args(L);
	QString opt = args[1].GetString();
	QString sLower = opt.toLower();
	if (sLower == "玩家称号" || sLower == "title")
	{
		if (args[2].IsNil())
		{
			return 0;
		}
		if (args[2].IsString())
		{
			std::string titleName = args.Count() > 1 ? args[2].GetString() : "";
			bool bRet = false;
			g_CGAInterface->ChangeNickName(titleName, bRet);
			L->PushBoolean(bRet);
			return 1;
		}
	}
	else if (sLower == "简介")
	{
		//8个参数
		if (args.Count() < 9)
		{
			return 0;
		}
		int changeBits = args[2].GetInteger();
		int sellIcon = args[3].GetInteger();
		std::string sellString = args[4].GetString();
		int buyIcon = args[5].GetInteger();
		std::string buyString = args[6].GetString();
		int wantIcon = args[7].GetInteger();
		std::string wantString = args[8].GetString();
		std::string descString = args[9].GetString();
		CGA::cga_pers_desc_t desc;
		desc.sellIcon = sellIcon;
		desc.sellString = sellString;

		desc.buyIcon = buyIcon;
		desc.buyString = buyString;
		desc.wantIcon = wantIcon;
		desc.wantString = wantString;
		desc.descString = descString;
		if (sellIcon)
			changeBits |= 1;
		if (!sellString.empty())
			changeBits |= 2;
		if (buyIcon)
			changeBits |= 4;
		if (!buyString.empty())
			changeBits |= 8;
		if (wantIcon)
			changeBits |= 0x10;
		if (!wantString.empty())
			changeBits |= 0x20;
		if (!descString.empty())
			changeBits |= 0x40;
		desc.changeBits = changeBits;

		g_CGAInterface->ChangePersDesc(desc);
	}
	return 0;
}

int CGLuaFun::Lua_GetItemCount(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	int val = g_pGameFun->GetItemCount(sName);
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetItemPileCount(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	int val = g_pGameFun->GetItemPileCount(sName);
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetBankItemCount(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	int val = g_pGameFun->GetBankItemCount(sName);
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetBankItemPileCount(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	int val = g_pGameFun->GetBankItemPileCount(sName);
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetAllItemCount(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	int val = g_pGameFun->GetAllItemCount(sName);
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetAllItemPileCount(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	int val = g_pGameFun->GetAllItemPileCount(sName);
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetItemNotUseSpaceCount(LuaState *L)
{
	LuaStack args(L);
	int val = g_pGameFun->GetInventoryEmptySlotCount();
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetItemNotUseSpacePos(LuaState *L)
{
	LuaStack args(L);
	auto posList = g_pGameFun->GetInventoryEmptySlotPosList();
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	for (auto i=0;i<posList.size();++i)
	{		
		LuaObject subTblObj(L);
		subTblObj.AssignInteger(L, posList[i]);
		//subTblObj.AssignNewTable();
		//subTblObj.SetInteger("pos", posList[i]);
		tblObj.SetObject(i + 1, subTblObj);
	}
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetBagUsedItemCount(LuaState *L)
{
	LuaStack args(L);
	int val = g_pGameFun->GetBagUsedItemCount();
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetBankItemNotUseSpaceCount(LuaState *L)
{
	LuaStack args(L);
	int val = g_pGameFun->FindBankEmptySlot();
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetTeammatesCount(LuaState *L)
{
	LuaStack args(L);
	int val = g_pGameFun->GetTeammatesCount();
	L->PushInteger(val);
	return 1;
}

int CGLuaFun::Lua_GetTeamPetAvgLv(LuaState *L)
{
	int avgLv = g_pGameCtrl->GetLastBattlePetAvgLv();
	L->PushInteger(avgLv);
	return 1;
}

int CGLuaFun::Lua_GetTeamPetLv(LuaState *L)
{
	LuaStack args(L);
	QString sType = args[1].GetString();
	int avgLv = 0;
	if (sType == "最低")
	{
		avgLv = g_pGameCtrl->GetLastBattlePetLv(1);
	}
	L->PushInteger(avgLv);
	return 1;
}

int CGLuaFun::Lua_AddTeammate(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	g_pGameFun->AddTeammate(sName);
	return 0;
}
int CGLuaFun::Lua_GetUnmannedMapUnitPosList(LuaState *L)
{
	LuaStack args(L);

	QList<QPoint> unmannedPosList;
	if (args.Count() < 1)
	{
		auto curPos = g_pGameFun->GetMapCoordinate();
		unmannedPosList = g_pGameFun->GetUnmannedMapUnitPosList(curPos);
	}
	else
	{
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		unmannedPosList = g_pGameFun->GetUnmannedMapUnitPosList(QPoint(x, y));
	}
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	int index = 0;
	for (auto tmpPos : unmannedPosList)
	{
		LuaObject subTbl(L);
		subTbl.AssignNewTable();
		subTbl.SetInteger("x", tmpPos.x());
		subTbl.SetInteger("y", tmpPos.y());
		tableObj.SetObject(++index, subTbl);
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_LeaveTeammate(LuaState *L)
{
	bool bIsMe = g_pGameFun->LeaveTeammate();
	L->PushBoolean(bIsMe);
	return 1;
}

int CGLuaFun::Lua_IsTeamLeader(LuaState *L)
{
	bool bIsMe = g_pGameFun->IsTeamLeader();
	L->PushBoolean(bIsMe);
	return 1;
}

int CGLuaFun::Lua_WaitTeammates(LuaState *L)
{
	LuaStack args(L);
	QString sName = args[1].GetString();
	QStringList sNames = sName.split("|");
	g_pGameFun->WaitTeammates(sNames);
	return 0;
}

int CGLuaFun::Lua_WaitTeammatesEx(LuaState *L)
{
	LuaStack args(L);
	int nCount = args[1].GetInteger();
	g_pGameFun->WaitTeammatesEx(nCount);
	return 0;
}

int CGLuaFun::Lua_WaitRecvHead(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 10000;
	auto dlg = g_pGameFun->WaitRecvHead(timeout);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetInteger("type", 0);
	tableObj.SetInteger("options", 0);
	tableObj.SetInteger("dialog_id", 0);
	tableObj.SetInteger("npc_id", 0);
	tableObj.SetString("message", "");
	tableObj.SetString("msg", "");
	if (dlg)
	{
		tableObj.SetInteger("type", dlg->type);
		tableObj.SetInteger("options", dlg->options);
		tableObj.SetInteger("dialog_id", dlg->dialog_id);
		tableObj.SetInteger("npc_id", dlg->npc_id);
		tableObj.SetString("message", dlg->message.toStdString().c_str());
		tableObj.SetString("msg", dlg->message.toStdString().c_str());
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_WaitRecvWorkResult(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 90000;
	auto result = g_pGameFun->WaitRecvWorkResult(timeout);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetInteger("type", 0);
	tableObj.SetString("name", "");
	tableObj.SetBoolean("success", false);
	if (result)
	{
		tableObj.SetInteger("type", result->type);
		tableObj.SetString("name", result->name.c_str());
		tableObj.SetBoolean("success", result->success);
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_WaitRecvPlayerMenu(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 5000;
	auto result = g_pGameFun->WaitRecvPlayerMenu(timeout);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	if (result)
	{
		for (int i = 0; i < result->size(); ++i)
		{
			auto menu = result->at(i);
			LuaObject subTbl(L);
			subTbl.AssignNewTable();
			subTbl.SetInteger("color", menu.color);
			subTbl.SetInteger("index", menu.index);
			subTbl.SetString("name", menu.name.c_str());
			tableObj.SetObject(i + 1, subTbl);
		}
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_WaitRecvPlayerMenuUnit(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 5000;
	auto result = g_pGameFun->WaitRecvPlayerMenuUnit(timeout);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	if (result)
	{
		for (int i = 0; i < result->size(); ++i)
		{
			auto menu = result->at(i);
			LuaObject subTbl(L);
			subTbl.AssignNewTable();
			subTbl.SetInteger("level", menu.level);
			subTbl.SetInteger("health", menu.health);
			subTbl.SetInteger("hp", menu.hp);
			subTbl.SetInteger("maxhp", menu.maxhp);
			subTbl.SetInteger("mp", menu.mp);
			subTbl.SetInteger("maxmp", menu.maxmp);
			subTbl.SetInteger("color", menu.color);
			subTbl.SetInteger("index", menu.index);
			subTbl.SetString("name", menu.name.c_str());
			tableObj.SetObject(i + 1, subTbl);
		}
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_WaitSupplyFini(LuaState *L)
{
	g_pGameFun->WaitSupplyFini();
	return 0;
}

int CGLuaFun::Lua_WaitNormal(LuaState *L)
{
	LuaStack args(L);
	int timeout = args[1].GetInteger();
	bool bRet = g_pGameFun->WaitInNormalState(timeout);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_WaitBattleEnd(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 30000;
	bool bRet = g_pGameFun->WaitBattleEnd(timeout);
	L->PushInteger(bRet);
	return 1;
}

int CGLuaFun::Lua_WaitRecvGameWndKeyDown(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 30000;
	int nVal = g_pGameFun->WaitRecvGameWndKeyDown(timeout);
	L->PushInteger(nVal);
	return 1;
}

int CGLuaFun::Lua_WaitRecvBattleAction(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 30000;
	int nVal = g_pGameFun->WaitRecvBattleAction(timeout);
	L->PushInteger(nVal);
	return 1;
}

int CGLuaFun::Lua_PlayerMenuSelect(LuaState *L)
{
	LuaStack args(L);
	int index = args.Count() > 0 ? args[1].GetInteger() : -1;
	QString name = args.Count() > 1 ? args[2].GetString() : "";
	bool bResult = false;
	g_CGAInterface->PlayerMenuSelect(index, name.toStdString(), bResult);
	return 0;
}

int CGLuaFun::Lua_UnitMenuSelect(LuaState *L)
{
	LuaStack args(L);
	int index = args.Count() > 0 ? args[1].GetInteger() : -1;
	bool bResult = false;
	g_CGAInterface->UnitMenuSelect(index, bResult);
	return 0;
}

int CGLuaFun::Lua_Work(LuaState *L)
{
	LuaStack args(L);
	QString skillname = args[1].GetString();
	int ntimeout = args.Count() > 2 ? args[3].GetInteger() : 6000;
	int isImmediate = args.Count() > 3 ? args[4].GetBoolean() : -1;
	if (args[2].IsString())
	{
		QString itemName = args[2].IsString() ? args[2].GetString() : "";
		g_pGameFun->WorkEx(skillname, itemName, ntimeout, isImmediate);
	}
	else
	{
		int nval = args[2].IsString() ? 0 : args[2].GetInteger();
		int nIndex = g_pGameFun->FindPlayerSkill(skillname);
		if (nIndex < 0)
			return 0;
		g_pGameFun->Work(nIndex, nval, ntimeout, isImmediate);
	}
	return 0;
}

int CGLuaFun::Lua_AllCompound(LuaState *L)
{
	LuaStack args(L);
	QString skillname = args[1].GetString();
	g_pGameFun->AllCompound(skillname);
	return 0;
}

int CGLuaFun::Lua_Exchange(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() == 3)
	{
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		QString itemName = args[3].GetString();
		g_pGameFun->Exchange(x, y, itemName);
	}
	else if (args.Count() == 2)
	{
		int nDir = args[1].GetInteger();
		QString itemName = args[2].GetString();
		g_pGameFun->ExchangeEx(nDir, itemName);
	}
	return 0;
}

int CGLuaFun::Lua_Shopping(LuaState *L)
{
	LuaStack args(L);
	int index = args[1].GetInteger();
	int count = args[2].GetInteger();
	g_pGameFun->Shopping(index, count);
	return 0;
}

int CGLuaFun::Lua_ParseBuyStoreMsg(LuaState *L)
{
	LuaStack args(L);

	QString sMsg = args[1].GetString();
	auto itemMap = g_pGameFun->ParseBuyStoreMsg(sMsg);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();

	tableObj.SetInteger("storeid", itemMap.value("storeid").toInt());
	tableObj.SetString("name", itemMap.value("name").toString().toStdString().c_str());
	tableObj.SetString("welcome", itemMap.value("welcome").toString().toStdString().c_str());
	tableObj.SetString("insuff_funds", itemMap.value("insuff_funds").toString().toStdString().c_str());
	tableObj.SetString("insuff_inventory", itemMap.value("insuff_inventory").toString().toStdString().c_str());
	QList<QVariant> itemList = itemMap.value("items").toList();
	int index = 0;
	LuaObject itemtableObj(L);
	itemtableObj.AssignNewTable();
	for (auto item : itemList)
	{
		QMap<QString, QVariant> itemData = item.toMap();
		LuaObject subTbl(L);
		subTbl.AssignNewTable();
		subTbl.SetString("name", itemData.value("name").toString().toStdString().c_str());
		subTbl.SetInteger("image_id", itemData.value("image_id").toInt());
		subTbl.SetInteger("cost", itemData.value("cost").toInt());
		subTbl.SetString("attr", itemData.value("attr").toString().toStdString().c_str());
		subTbl.SetInteger("unk1", itemData.value("unk1").toInt());
		subTbl.SetInteger("max_buy", itemData.value("max_buy").toInt());
		itemtableObj.SetObject(++index, subTbl);
	}
	tableObj.SetObject("items", itemtableObj);
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_Sale(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() >= 3)
	{
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		QString itemName = L->IsString(3) ? args[3].GetString() : "";
		g_pGameFun->Sale(x, y, itemName);
	}
	else
	{
		QString itemName = L->IsString(2) ? args[2].GetString() : "";
		if (L->IsInteger(1))
		{
			int nDir = args[1].GetInteger();
			g_pGameFun->SaleEx(nDir, itemName);
		}
		else if (L->IsString(1))
		{
			QString sDir = args[1].GetString();
			g_pGameFun->SaleEx2(sDir, itemName);
		}
	}
	return 0;
}

int CGLuaFun::Lua_Sale2(LuaState *L)
{
	/*LuaStack args(L);
	if (args.Count() >= 4)
	{
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		QString itemName = L->IsString(3) ? args[3].GetString() : "";
		int count = args[4].GetInteger();

		g_pGameFun->Sale(x, y, itemName);
	}
	else
	{
		QString itemName = L->IsString(2) ? args[2].GetString() : "";
		if (L->IsInteger(1))
		{
			int nDir = args[1].GetInteger();
			g_pGameFun->SaleEx(nDir, itemName);
		}
		else if (L->IsString(1))
		{
			QString sDir = args[1].GetString();
			g_pGameFun->SaleEx2(sDir, itemName);
		}
	}*/
	return 0;
}

int CGLuaFun::Lua_SaleEx(LuaState *L)
{
	LuaStack args(L);
	QString sDir = args[1].GetString();
	QString itemName = args[2].GetString();
	g_pGameFun->SaleEx2(sDir, itemName);
	return 0;
}

int CGLuaFun::Lua_IdentifyItem(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() >= 3)
	{
		int x = args[1].GetInteger();
		int y = args[2].GetInteger();
		QString itemName = L->IsString(3) ? args[3].GetString() : "";
		g_pGameFun->IdentifyItem(x, y, itemName);
	}
	else
	{
		QString itemName = L->IsString(2) ? args[2].GetString() : "";
		if (L->IsInteger(1))
		{
			int nDir = args[1].GetInteger();
			g_pGameFun->IdentifyItemEx(nDir, itemName);
		}
		else if (L->IsString(1))
		{
			QString sDir = args[1].GetString();
			g_pGameFun->IdentifyItemEx2(sDir, itemName);
		}
	}
	return 0;
}

int CGLuaFun::Lua_IdentifyItemEx(LuaState *L)
{
	LuaStack args(L);
	QString sDir = args[1].GetString();
	QString itemName = args[2].GetString();
	g_pGameFun->IdentifyItemEx2(sDir, itemName);
	return 0;
}

int CGLuaFun::Lua_SaveToBankAll(LuaState *L)
{
	LuaStack args(L);
	QString itemName = args[1].GetString();
	int count = args.Count() > 1 ? args[2].GetInteger() : 0;
	g_pGameFun->SaveToBankAll(itemName, count);
	return 0;
}

int CGLuaFun::Lua_SaveToBankOnce(LuaState *L)
{
	LuaStack args(L);
	QString itemName = args[1].GetString();
	int count = args.Count() > 1 ? args[2].GetInteger() : 0;
	g_pGameFun->SaveToBankOnce(itemName, count);
	return 0;
}

int CGLuaFun::Lua_WithdrawItem(LuaState *L)
{
	LuaStack args(L);
	QString itemName = L->IsString(1) ? args[1].GetString() : "";
	int count = 1;
	if (args.Count() > 1)
	{
		count = args[2].GetInteger();
	}
	g_pGameFun->WithdrawItem(itemName, count);
	return 0;
}

int CGLuaFun::Lua_WithdrawAllItem(LuaState *L)
{
	LuaStack args(L);
	QString itemName = args[1].GetString();
	int count = args.Count() > 1 ? args[2].GetInteger() : 20; //默认全取最大是20个
	g_pGameFun->WithdrawItemAll(itemName, count);
	return 0;
}

int CGLuaFun::Lua_SortBagItems(LuaState *L)
{
	LuaStack args(L);
	bool bFront = args.Count() > 0 ? args[1].GetBoolean() : false;
	g_pGameFun->SortBagItems(bFront);
	return 0;
}

int CGLuaFun::Lua_SortBankItems(LuaState *L)
{
	LuaStack args(L);
	bool bFront = args.Count() > 0 ? args[1].GetBoolean() : true;
	g_pGameFun->SortBankItems(bFront);
	return 0;
}

int CGLuaFun::Lua_DropGold(LuaState *L)
{
	LuaStack args(L);
	int val = args[1].GetInteger();
	g_pGameFun->DropGold(val);
	return 0;
}

int CGLuaFun::Lua_DepositGold(LuaState *L)
{
	LuaStack args(L);
	int val = args[1].GetInteger();
	g_pGameFun->DepositGold(val);
	return 0;
}

int CGLuaFun::Lua_WithdrawGold(LuaState *L)
{
	LuaStack args(L);
	int val = args[1].GetInteger();
	g_pGameFun->WithdrawGold(val);
	return 0;
}

int CGLuaFun::Lua_DropPet(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() > 0 && args[1].IsInteger())
	{
		int nPos = args.Count() > 0 ? args[1].GetInteger() : -1;
		bool bRet = g_pGameFun->DropPetEx(nPos);
		L->PushBoolean(bRet);
		return 1;
	}
	else if (args.Count() > 0 && args[1].IsString())
	{
		QString sPet = args[1].GetString();
		bool bRet = g_pGameFun->DropPet(sPet);
		L->PushBoolean(bRet);
		return 1;
	}
	return 0;
}

int CGLuaFun::Lua_DepositPet(LuaState *L)
{
	LuaStack args(L);

	if (args.Count() > 0 && args[1].IsInteger())
	{
		int nSrcIndex = args.Count() > 0 ? args[1].GetInteger() : -1;
		bool bRet = g_pGameFun->DepositPetPos(nSrcIndex);
		L->PushBoolean(bRet);
		return 1;
	}
	else if (args.Count() > 0 && args[1].IsString())
	{
		QString sPet = args[1].GetString();
		bool bRet = g_pGameFun->DepositPet(sPet);
		L->PushBoolean(bRet);
		return 1;
	}
	else if (args.Count() > 1 && args[1].IsInteger())
	{
		int nSrcIndex = args.Count() > 0 ? args[1].GetInteger() : 0;
		int nDstIndex = args.Count() > 1 ? args[2].GetInteger() : 0;
		bool bRet = g_pGameFun->DepositPetEx(nSrcIndex, nDstIndex);
		L->PushBoolean(bRet);
		return 1;
	}
	return 0;
}

int CGLuaFun::Lua_WithdrawPet(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() > 0 && args[1].IsString())
	{
		QString sPet = args[1].GetString();
		bool bRet = g_pGameFun->WithdrawPet(sPet);
		L->PushBoolean(bRet);
		return 1;
	}
	else if (args.Count() > 1 && args[1].IsString())
	{
		int nSrcIndex = args.Count() > 0 ? args[1].GetInteger() : 0;
		int nDstIndex = args.Count() > 1 ? args[2].GetInteger() : 0;
		bool bRet = g_pGameFun->WithdrawPetEx(nSrcIndex, nDstIndex);
		L->PushBoolean(bRet);
		return 1;
	}
	return 0;
}

int CGLuaFun::Lua_BankOperation(LuaState *L)
{
	LuaStack args(L);
	QString opt = args[1].GetString();
	QString sLower = opt.toLower();
	if (sLower == "金币" || sLower == "gold")
	{ //返回银行金币
		int bankGold = 0;
		g_CGAInterface->GetBankGold(bankGold);
		L->PushInteger(bankGold);
		return 1;
	}
	else if (sLower == "宠物" || sLower == "pet")
	{
		//返回银行宠物信息
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		CGA::cga_pets_info_t bankPets;
		g_CGAInterface->GetBankPetsInfo(bankPets);
		for (size_t i = 0; i < bankPets.size(); ++i)
		{
			const CGA::cga_pet_info_t &info = bankPets.at(i);
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", info.name.c_str());
			subObj.SetString("realname", info.realname.c_str());
			subObj.SetInteger("level", info.level);
			subObj.SetInteger("hp", info.hp);
			subObj.SetInteger("maxhp", info.maxhp);
			subObj.SetInteger("mp", info.mp);
			subObj.SetInteger("xp", info.xp);
			subObj.SetInteger("maxmp", info.maxmp);
			subObj.SetInteger("health", info.health);
			subObj.SetInteger("flags", info.flags);
			subObj.SetInteger("race", info.race);
			subObj.SetInteger("loyality", info.loyality);
			subObj.SetInteger("battle_flags", info.battle_flags);
			subObj.SetInteger("state", info.state);
			subObj.SetInteger("index", info.index);
			tableObj.SetObject(i + 1, subObj);
		}
		tableObj.Push(L);
		return 1;
	}
	else if (sLower == "宠物数" || sLower == "petcount")
	{
		CGA::cga_pets_info_t bankPets;
		g_CGAInterface->GetBankPetsInfo(bankPets);
		int nCount = bankPets.size();
		L->PushInteger(nCount);
		return 1;
	}
	else if (sLower == "所有物品" || sLower == "item")
	{
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		CGA::cga_items_info_t myinfos;
		g_CGAInterface->GetBankItemsInfo(myinfos);
		for (int i = 0; i < myinfos.size(); i++)
		{
			CGA::cga_item_info_t info = myinfos.at(i);
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", info.name.c_str());
			subObj.SetString("attr", info.attr.c_str());
			subObj.SetString("info", info.info.c_str());
			subObj.SetInteger("itemid", info.itemid);
			subObj.SetInteger("count", info.count);
			subObj.SetInteger("pos", info.pos);
			subObj.SetInteger("level", info.level);
			subObj.SetInteger("type", info.type);
			subObj.SetInteger("assessed", info.assessed);
			tableObj.SetObject(i + 1, subObj);
		}
		tableObj.Push(L);
		return 1;
	}
	else if (sLower == "已用空格" || sLower == "itemcount")
	{
		CGA::cga_items_info_t myinfos;
		g_CGAInterface->GetBankItemsInfo(myinfos);
		int nCount = myinfos.size();
		L->PushInteger(nCount);
		return 1;
	}
	else if (sLower == "物品数量")
	{
		QString itemname = args.Count() > 1 ? args[2].GetString() : "";
		int nCount = g_pGameFun->GetBankItemCount(itemname);
		L->PushInteger(nCount);
		return 1;
	}
	else if (sLower == "物品叠加数量")
	{
		QString itemname = args.Count() > 1 ? args[2].GetString() : "";
		int nCount = g_pGameFun->GetBankItemPileCount(itemname);
		L->PushInteger(nCount);
		return 1;
	}
	else if (sLower == "存钱" || sLower == "depositgold")
	{
		int nGold = args.Count() > 1 ? args[2].GetInteger() : 0;
		g_pGameFun->DepositGold(nGold);
		return 0;
	}
	else if (sLower == "取钱" || sLower == "withdrawgold")
	{
		int nGold = args.Count() > 1 ? args[2].GetInteger() : 0;
		g_pGameFun->WithdrawGold(nGold);
		return 0;
	}
	else if (sLower == "存包裹位置" || sLower == "deposititempos")
	{
		int pos = args.Count() > 1 ? args[2].GetInteger() : 0;
		bool bRet = g_pGameFun->SavePosToBank(pos);
		L->PushBoolean(bRet);
		return 1;
	}
	/*else if (sLower == "存指定数量" || sLower == "deposititemcount")
	{
		QString sItem = args.Count() > 1 ? args[2].GetString() : "";
		int nCount = args.Count() > 2 ? args[3].GetInteger() : 0;
		bool bRet = g_pGameFun->SaveToBankOnce(sItem, nCount);
		L->PushBoolean(bRet);
		return 1;
	}*/
	else if (sLower == "存物" || sLower == "deposititem")
	{
		QString sItem = args.Count() > 1 ? args[2].GetString() : "";
		int nCount = args.Count() > 2 ? args[3].GetInteger() : 0;
		bool bRet = g_pGameFun->SaveToBankOnce(sItem, nCount);
		L->PushBoolean(bRet);
		return 1;
	}
	else if (sLower == "全存")
	{
		QString sItem = args.Count() > 1 ? args[2].GetString() : "";
		int nCount = args.Count() > 2 ? args[3].GetInteger() : 0;
		bool bRet = false;
		if (args.Count() == 1) //没有参数 包裹全存
			bRet = g_pGameFun->SaveBagAllItemsToBank();
		else
			bRet = g_pGameFun->SaveToBankAll(sItem, nCount); //这个有延时判断 所以慢
		L->PushBoolean(bRet);
		return 1;
	}
	else if (sLower == "全取") //默认从银行刚开始取，所以不能精确控制
	{
		QString sItem = args.Count() > 1 ? args[2].GetString() : "";
		int nCount = args.Count() > 2 ? args[3].GetInteger() : 20;
		bool bRet = false;
		if (args.Count() == 1) //没有参数 包裹全存
			bRet = g_pGameFun->WithdrawItemAllEx();
		else
			bRet = g_pGameFun->WithdrawItemAll(sItem, nCount);
		L->PushBoolean(bRet);
		return 1;
	}
	/*else if (sLower == "取指定数量" || sLower == "withdrawitemcount")
	{
		QString sItem = args.Count() > 1 ? args[2].GetString() : "";
		int nCount = args.Count() > 2 ? args[3].GetInteger() : 0;
		bool bRet = g_pGameFun->WithdrawItem(sItem, nCount);
		L->PushBoolean(bRet);
		return 1;
	}*/
	else if (sLower == "取物" || sLower == "withdrawitem")
	{
		QString sItem = args.Count() > 1 ? args[2].GetString() : "";
		int nCount = args.Count() > 2 ? args[3].GetInteger() : 0;
		bool bRet = g_pGameFun->WithdrawItem(sItem, nCount);
		L->PushBoolean(bRet);
		return 1;
	}
	else if (sLower == "存宠" || sLower == "depositpet")
	{
		bool bRet = false;
		if (args.Count() > 2 && args[2].IsInteger()) //3个参数 并且第二个为pos 第三个为pos
		{
			int nSrcIndex = args[2].GetInteger();
			int nDstIndex = args[3].GetInteger();
			if (nSrcIndex != -1 && nDstIndex != -1)
				bRet = g_pGameFun->DepositPetEx(nSrcIndex, nDstIndex);
			L->PushBoolean(bRet);
			return 1;
		}
		else if (args.Count() > 1 && args[2].IsInteger())
		{
			int nSrcIndex = args[2].GetInteger();
			bool bRet = g_pGameFun->DepositPetPos(nSrcIndex);
			L->PushBoolean(bRet);
			return 1;
		}
		else if (args.Count() > 1 && args[2].IsString())
		{
			QString sPet = args[2].GetString();
			bool bRet = g_pGameFun->DepositPet(sPet);
			L->PushBoolean(bRet);
			return 1;
		}
	}
	else if (sLower == "取宠" || sLower == "withdrawpet")
	{
		if (args.Count() > 2 && args[2].IsInteger())
		{
			int nSrcIndex = args[2].GetInteger();
			int nDstIndex = args[3].GetInteger();
			bool bRet = g_pGameFun->WithdrawPetEx(nSrcIndex, nDstIndex);
			L->PushBoolean(bRet);
			return 1;
		}
		else if (args.Count() > 1 && args[2].IsString())
		{
			QString sPet = args[2].GetString();
			bool bRet = g_pGameFun->WithdrawPet(sPet);
			L->PushBoolean(bRet);
			return 1;
		}
	}
	return 0;
}

int CGLuaFun::Lua_SavePetPictorialBookToHtml(LuaState *L)
{
	LuaStack args(L);
	QString sPath = args[1].GetString();
	bool bInPic = args.Count() > 1 ? args[2].GetBoolean() : false;
	bool bRet = g_pGameFun->SavePetPictorialBookToHtml(sPath, bInPic);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_SetUIScriptDesc(LuaState *L)
{
	LuaStack args(L);
	QString sDesc = args[1].GetString();
	emit g_pGameCtrl->signal_setUiScriptDesc(sDesc);
	return 0;
}

int CGLuaFun::Lua_CreateRandomRoleName(LuaState *L)
{
	LuaStack args(L);
	int nsex = args.Count() > 0 ? args[1].GetInteger() : 0;
	QString sMsg = g_pGameFun->CreateRandomName(nsex);
	L->PushString(sMsg.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_GetSysChatMsg(LuaState *L)
{
	QString sMsg = g_pGameFun->GetSysChatMsg();
	L->PushString(sMsg.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_GetLastSysChatMsg(LuaState *L)
{
	QString sMsg = g_pGameFun->GetJustSysChatMsg();
	L->PushString(sMsg.toStdString().c_str());
	return 1;
}
int CGLuaFun::Lua_GetAllChatMsg(LuaState *L)
{
	LuaStack args(L);
	int ncount = args.Count() > 0 ? args[1].GetInteger() : 0;
	QString sMsg = g_pGameFun->GetAllChatMsg(ncount);
	L->PushString(sMsg.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_GetDetailAllChatMsg(LuaState *L)
{
	LuaStack args(L);
	int ncount = args.Count() > 0 ? args[1].GetInteger() : 0;
	auto detailMsg = g_pGameFun->GetDetailAllChatMsg(ncount);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	for (int i = 0; i < detailMsg.size(); ++i)
	{
		LuaObject subObj(L);
		subObj.AssignNewTable();
		subObj.SetString("msg", detailMsg[i].second.toStdString().c_str());
		subObj.SetInteger("unitid", detailMsg[i].first);
		tableObj.SetObject(i + 1, subObj);
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetLastChatMsg(LuaState *L)
{
	auto sLastMsg = g_pGameFun->GetJustChatMsg();
	QString sMsg;
	if (sLastMsg.size() >= 2)
	{
		sMsg = sLastMsg[1];
	}
	L->PushString(sMsg.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_ClearSysChatMsg(LuaState *L)
{
	g_pGameFun->ClearSysCue();
	return 0;
}

int CGLuaFun::Lua_WaitSysMsg(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 5000;
	std::tuple<int, QString> sMsgData = g_pGameFun->WaitSysMsg(timeout);
	QString sMsg = std::get<1>(sMsgData);
	L->PushString(sMsg.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_WaitChatMsg(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 5000;
	std::tuple<int, QString> sMsgData = g_pGameFun->WaitChatMsg(timeout);
	QString sMsg = std::get<1>(sMsgData);
	L->PushString(sMsg.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_WaitSysAndChatMsg(LuaState *L)
{
	LuaStack args(L);
	int timeout = args.Count() > 0 ? args[1].GetInteger() : 5000;
	std::tuple<int, QString> sMsgData = g_pGameFun->WaitSysAndChatMsg(timeout);
	QString sMsg = std::get<1>(sMsgData);
	L->PushString(sMsg.toStdString().c_str());
	return 1;
}

int CGLuaFun::Lua_WaitSubscribeMsg(LuaState *L)
{
	LuaStack args(L);
	int timeInterval = args.Count() > 0 ? args[1].GetInteger() : 5000;
	int timeout = args.Count() > 1 ? args[2].GetInteger() : 5000;
	std::tuple<QString, QString> sMsgData = g_pGameFun->WaitSubscribeMsg(timeInterval,timeout);
	QString sTopic = std::get<0>(sMsgData);
	QString sMsg = std::get<1>(sMsgData);
	L->PushString(sTopic.toStdString().c_str());
	L->PushString(sMsg.toStdString().c_str());
	return 2;
}

int CGLuaFun::Lua_SubscribeMsg(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		qDebug() << "订阅失败，参数为空！";
		L->PushBoolean(false);
		return 1;
	}
	QStringList sDefaultVal;
	if (args[1].IsTable())
	{
		LuaObject tblData = args[1];
		for (LuaTableIterator it(tblData); it; it.Next())
		{
			sDefaultVal << it.GetValue().GetString();
		}
	}
	else
	{
		sDefaultVal << args[1].GetString();
	}
	ITObjectDataMgr::getInstance().AddNewSubscribe(sDefaultVal);
	L->PushBoolean(true);
	return 1;
}

int CGLuaFun::Lua_RemoveSubscribeMsg(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		qDebug() << "取消订阅失败，参数为空！";
		L->PushBoolean(false);
		return 1;
	}
	QStringList sDefaultVal;
	if (args[1].IsTable())
	{
		LuaObject tblData = args[1];
		for (LuaTableIterator it(tblData); it; it.Next())
		{
			sDefaultVal << it.GetValue().GetString();
		}
	}
	else
	{
		sDefaultVal << args[1].GetString();
	}
	ITObjectDataMgr::getInstance().RemoveSubscribe(sDefaultVal);
	L->PushBoolean(true);
	return 1;
}

int CGLuaFun::Lua_PublishMsg(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2 || !args[1].IsString() || !args[2].IsString())
	{
		qDebug() << "发布失败，参数错误！";
		L->PushBoolean(false);
		return 1;
	}
	emit ITObjectDataMgr::getInstance().signal_publishMqttMsg(args[1].GetString(), args[2].GetString());
	L->PushBoolean(true);
	return 1;
}

int CGLuaFun::Lua_GetAllRecvTopicMsgList(LuaState *L)
{
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	auto pMsgList = ITObjectDataMgr::getInstance().GetAllRecvTopicMsgList();
	for (int i = 0; i < pMsgList.size(); ++i)
	{		
		auto it = pMsgList[i];
		if (it.size() < 2)
			continue;
		
		LuaObject subObj(L);
		subObj.AssignNewTable();
		subObj.SetString("topic", it[0].toStdString().c_str());
		subObj.SetString("msg", it[1].toStdString().c_str());
		tableObj.SetObject(i + 1, subObj);
	}
	tableObj.Push(L);
	return 1;
}
int CGLuaFun::Lua_GetTopicMsgList(LuaState *L)
{
	LuaStack args(L);
	QString sTgtToipc = args.Count() > 0 ? args[1].GetString() : "";
	auto pMsgList = ITObjectDataMgr::getInstance().GetLastTgtPublishMsg(sTgtToipc);
	if (pMsgList.size() >= 2)
	{
		L->PushString(pMsgList[0].toStdString().c_str());
		L->PushString(pMsgList[1].toStdString().c_str());
	}
	else
	{
		L->PushString("");
		L->PushString("");
	}
	return 2;
}

int CGLuaFun::Lua_GetLastTopicMsg(LuaState *L)
{
	LuaStack args(L);
	int timeInterval = args.Count() > 0 ? args[1].GetInteger() : 5000;
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	auto topicMsg = ITObjectDataMgr::getInstance().GetLastPublishMsg(timeInterval);
	if (topicMsg.size() >=2)
	{
		tableObj.SetString("topic", topicMsg[0].toStdString().c_str());
		tableObj.SetString("msg", topicMsg[1].toStdString().c_str());
	}
	else
	{
		tableObj.SetString("topic", "");
		tableObj.SetString("msg", "");
	}
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_RemoveAllTopics(LuaState *L)
{
	ITObjectDataMgr::getInstance().RemoveAllTopics();
	return 0;
}

int CGLuaFun::Lua_SellNPCStore(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	CGA::cga_sell_items_t cgaSaleItems;
	if (args[1].IsTable())
	{
		LuaObject itemData = args[1];
		if (!itemData.IsNil())
		{
			for (LuaTableIterator it(itemData); it; it.Next())
			{
				auto itData = it.GetValue();
				if (itData.IsTable())
				{
					int itemid = itData.GetByName("id").ToInteger();
					int itempos = itData.GetByName("pos").ToInteger();
					int itemcount = itData.GetByName("count").ToInteger();
					CGA::cga_sell_item_t cgaItem;
					cgaItem.itemid = itemid;
					cgaItem.itempos = itempos;
					cgaItem.count = itemcount;
					cgaSaleItems.push_back(cgaItem);
				}
			}
		}
	}
	bool bResult = false;
	g_CGAInterface->SellNPCStore(cgaSaleItems, bResult);
	return 0;
}

int CGLuaFun::Lua_BuyNPCStore(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	CGA::cga_buy_items_t items; //vector  index count
	if (args[1].IsTable())
	{
		LuaObject itemData = args[1];
		if (!itemData.IsNil())
		{
			for (LuaTableIterator it(itemData); it; it.Next())
			{
				auto itData = it.GetValue();
				if (itData.IsTable())
				{
					int index = itData.GetByName("index").ToInteger();
					int count = itData.GetByName("count").ToInteger();
					CGA::cga_buy_item_t cgaItem;
					cgaItem.index = index;
					cgaItem.count = count;
					items.push_back(cgaItem);
				}
			}
		}
	}
	bool bResult = false;
	g_CGAInterface->BuyNPCStore(items, bResult);
	return 0;
}
//0 体力 1力量 2强度 3敏捷 4魔法
int CGLuaFun::Lua_UpgradePlayer(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int type = args[1].GetInteger();
	g_CGAInterface->UpgradePlayer(type);
	return 0;
}

int CGLuaFun::Lua_UpgradePet(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	int type = args[2].GetInteger();
	g_CGAInterface->UpgradePet(index, type);
	return 0;
}

int CGLuaFun::Lua_GetBankGold(LuaState *L)
{
	int gold = 0;
	g_CGAInterface->GetBankGold(gold);
	L->PushInteger(gold);
	return 1;
}

int CGLuaFun::Lua_MoveItem(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 3)
		return 0;
	int srcPos = args[1].GetInteger();
	int dstPos = args[2].GetInteger();
	int count = args[3].GetInteger();
	bool bRes = false;
	g_CGAInterface->MoveItem(srcPos, dstPos, count, bRes);
	return 0;
}

int CGLuaFun::Lua_MovePet(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int srcPos = args[1].GetInteger();
	int dstPos = args[2].GetInteger();
	bool bRes = false;
	g_CGAInterface->MovePet(srcPos, dstPos, bRes);
	return 0;
}

int CGLuaFun::Lua_MoveGold(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int gold = args[1].GetInteger();
	int opt = args[2].GetInteger();
	bool bRes = false;
	g_CGAInterface->MoveGold(gold, opt, bRes);
	return 0;
}

int CGLuaFun::Lua_DropItem(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int pos = args[1].GetInteger();
	bool bRes = false;
	g_CGAInterface->DropItem(pos, bRes);
	return 0;
}

int CGLuaFun::Lua_ChangePetState(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	int state = args[2].GetInteger();
	bool bRes = false;
	g_CGAInterface->ChangePetState(index, state, bRes);
	return 0;
}

int CGLuaFun::Lua_SetPlayerFlagEnabled(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	bool bEnable = args[2].GetBoolean();
	g_CGAInterface->SetPlayerFlagEnabled(index, bEnable);
	return 0;
}

int CGLuaFun::Lua_IsPlayerFlagEnabled(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	int index = args[1].GetInteger();
	bool bEnable = false;
	g_CGAInterface->IsPlayerFlagEnabled(index, bEnable);
	L->PushBoolean(bEnable);
	return 1;
}

int CGLuaFun::Lua_IsSkillValid(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	int skillid = args[1].GetInteger();
	bool valid = false;
	g_CGAInterface->IsSkillValid(skillid, valid);
	L->PushBoolean(valid);
	return 1;
}

int CGLuaFun::Lua_GetSkillInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}

	int skillid = args[1].GetInteger();
	CGA::cga_skill_info_t skill;
	g_CGAInterface->GetSkillInfo(skillid, skill);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetString("name", skill.name.c_str());
	tableObj.SetInteger("lv", skill.lv);
	tableObj.SetInteger("maxlv", skill.maxlv);
	tableObj.SetInteger("xp", skill.xp);
	tableObj.SetInteger("maxxp", skill.maxxp);
	tableObj.SetInteger("skill_id", skill.skill_id);
	tableObj.SetInteger("type", skill.type);
	tableObj.SetInteger("pos", skill.pos);
	tableObj.SetInteger("index", skill.index);
	tableObj.SetInteger("slotsize", skill.slotsize);
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetSkillsInfo(LuaState *L)
{
	LuaObject skillObj(L);
	skillObj.AssignNewTable();
	CGA::cga_skills_info_t skillsinfo;
	if (g_CGAInterface->GetSkillsInfo(skillsinfo))
	{
		for (size_t i = 0; i < skillsinfo.size(); ++i)
		{
			const CGA::cga_skill_info_t &skinfo = skillsinfo.at(i);
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", skinfo.name.c_str());
			subObj.SetInteger("index", skinfo.index);
			subObj.SetInteger("lv", skinfo.lv);
			subObj.SetInteger("maxlv", skinfo.maxlv);
			subObj.SetInteger("xp", skinfo.xp);
			subObj.SetInteger("maxxp", skinfo.maxxp);
			subObj.SetInteger("pos", skinfo.pos);

			CGA::cga_subskills_info_t subskillsinfo;
			LuaObject subSkillObj(L);
			subSkillObj.AssignNewTable();
			//获取合成物品信息
			CGA::cga_craft_info_t craftInfo;
			if (g_CGAInterface->GetSubSkillsInfo(skinfo.index, subskillsinfo))
			{
				for (size_t j = 0; j < subskillsinfo.size(); ++j)
				{
					const CGA::cga_subskill_info_t &subskinfo = subskillsinfo.at(j);

					LuaObject subTbObj(L);
					subTbObj.AssignNewTable();
					subTbObj.SetString("name", subskinfo.name.c_str());
					subTbObj.SetString("info", subskinfo.info.c_str());
					subTbObj.SetInteger("level", subskinfo.level);
					subTbObj.SetInteger("cost", subskinfo.cost);
					subTbObj.SetInteger("flags", subskinfo.flags);
					subTbObj.SetBoolean("available", subskinfo.available);
					subSkillObj.SetObject(j + 1, subTbObj);
				}
			}
			subObj.SetObject("subskill", subSkillObj);

			skillObj.SetObject(i + 1, subObj);
		}
	}
	skillObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetSubSkillInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
	{
		return 0;
	}

	int skillid = args[1].GetInteger();
	int stage = args[2].GetInteger();
	CGA::cga_subskill_info_t skill;
	g_CGAInterface->GetSubSkillInfo(skillid, stage, skill);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetString("name", skill.name.c_str());
	tableObj.SetString("info", skill.info.c_str());
	tableObj.SetInteger("cost", skill.cost);
	tableObj.SetInteger("flags", skill.flags);
	tableObj.SetInteger("level", skill.level);
	tableObj.SetBoolean("available", skill.available);
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetSubSkillsInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}

	int skillid = args[1].GetInteger();
	CGA::cga_subskills_info_t skills;
	g_CGAInterface->GetSubSkillsInfo(skillid, skills);
	LuaObject skillObj(L);
	skillObj.AssignNewTable();
	for (int i = 0; i < skills.size(); ++i)
	{
		const CGA::cga_subskill_info_t &skill = skills[i];
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		tableObj.SetString("name", skill.name.c_str());
		tableObj.SetString("info", skill.info.c_str());
		tableObj.SetInteger("cost", skill.cost);
		tableObj.SetInteger("flags", skill.flags);
		tableObj.SetInteger("level", skill.level);
		tableObj.SetBoolean("available", skill.available);
		skillObj.SetObject(i + 1, tableObj);
	}
	skillObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_IsPetValid(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	int petid = args[1].GetInteger();

	bool avid = false;
	g_CGAInterface->IsPetValid(petid, avid);
	L->PushBoolean(avid);
	return 1;
}

int CGLuaFun::Lua_GetPetInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	int petid = args[1].GetInteger();
	CGA::cga_pet_info_t petdata;
	g_CGAInterface->GetPetInfo(petid, petdata);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetString("name", petdata.name.c_str());
	tableObj.SetString("realname", petdata.realname.c_str());
	tableObj.SetInteger("hp", petdata.hp);
	tableObj.SetInteger("maxhp", petdata.maxhp);
	tableObj.SetInteger("mp", petdata.mp);
	tableObj.SetInteger("maxmp", petdata.maxmp);
	tableObj.SetInteger("xp", petdata.xp);
	tableObj.SetInteger("maxxp", petdata.maxxp);
	tableObj.SetInteger("health", petdata.health);
	tableObj.SetInteger("level", petdata.level);
	tableObj.SetInteger("flags", petdata.flags);
	tableObj.SetInteger("race", petdata.race);
	tableObj.SetInteger("loyality", petdata.loyality);
	tableObj.SetInteger("skillslots", petdata.skillslots);
	tableObj.SetInteger("battle_flags", petdata.battle_flags);
	tableObj.SetInteger("state", petdata.state);
	tableObj.SetInteger("index", petdata.index);

	LuaObject subTblObj(L);
	subTblObj.AssignNewTable();
	subTblObj.SetInteger("points_remain", petdata.detail.points_remain);
	subTblObj.SetInteger("points_endurance", petdata.detail.points_endurance);
	subTblObj.SetInteger("points_strength", petdata.detail.points_strength);
	subTblObj.SetInteger("points_defense", petdata.detail.points_defense);
	subTblObj.SetInteger("points_agility", petdata.detail.points_agility);
	subTblObj.SetInteger("points_magical", petdata.detail.points_magical);
	subTblObj.SetInteger("value_attack", petdata.detail.value_attack);
	subTblObj.SetInteger("value_defensive", petdata.detail.value_defensive);
	subTblObj.SetInteger("value_agility", petdata.detail.value_agility);
	subTblObj.SetInteger("value_spirit", petdata.detail.value_spirit);
	subTblObj.SetInteger("value_recovery", petdata.detail.value_recovery);
	subTblObj.SetInteger("resist_poison", petdata.detail.resist_poison);
	subTblObj.SetInteger("resist_sleep", petdata.detail.resist_sleep);
	subTblObj.SetInteger("resist_medusa", petdata.detail.resist_medusa);
	subTblObj.SetInteger("resist_drunk", petdata.detail.resist_drunk);
	subTblObj.SetInteger("resist_chaos", petdata.detail.resist_chaos);
	subTblObj.SetInteger("resist_forget", petdata.detail.resist_forget);
	subTblObj.SetInteger("fix_critical", petdata.detail.fix_critical);
	subTblObj.SetInteger("fix_strikeback", petdata.detail.fix_strikeback);
	subTblObj.SetInteger("fix_accurancy", petdata.detail.fix_accurancy);
	subTblObj.SetInteger("fix_dodge", petdata.detail.fix_dodge);
	subTblObj.SetInteger("element_earth", petdata.detail.element_earth);
	subTblObj.SetInteger("element_water", petdata.detail.element_water);
	subTblObj.SetInteger("element_fire", petdata.detail.element_fire);
	subTblObj.SetInteger("element_wind", petdata.detail.element_wind);
	tableObj.SetObject("detail", subTblObj);
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetBankPetsInfo(LuaState *L)
{
	CGA::cga_pets_info_t pets;
	g_CGAInterface->GetBankPetsInfo(pets);
	LuaObject allPetObj(L);
	allPetObj.AssignNewTable();
	for (int i = 0; i < pets.size(); ++i)
	{
		const CGA::cga_pet_info_t petdata = pets[i];
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		tableObj.SetString("name", petdata.name.c_str());
		tableObj.SetString("realname", petdata.realname.c_str());
		tableObj.SetInteger("hp", petdata.hp);
		tableObj.SetInteger("maxhp", petdata.maxhp);
		tableObj.SetInteger("mp", petdata.mp);
		tableObj.SetInteger("maxmp", petdata.maxmp);
		tableObj.SetInteger("xp", petdata.xp);
		tableObj.SetInteger("maxxp", petdata.maxxp);
		tableObj.SetInteger("health", petdata.health);
		tableObj.SetInteger("level", petdata.level);
		tableObj.SetInteger("flags", petdata.flags);
		tableObj.SetInteger("race", petdata.race);
		tableObj.SetInteger("loyality", petdata.loyality);
		tableObj.SetInteger("skillslots", petdata.skillslots);
		tableObj.SetInteger("battle_flags", petdata.battle_flags);
		tableObj.SetInteger("state", petdata.state);
		tableObj.SetInteger("index", petdata.index);

		LuaObject subTblObj(L);
		subTblObj.AssignNewTable();
		subTblObj.SetInteger("points_remain", petdata.detail.points_remain);
		subTblObj.SetInteger("points_endurance", petdata.detail.points_endurance);
		subTblObj.SetInteger("points_strength", petdata.detail.points_strength);
		subTblObj.SetInteger("points_defense", petdata.detail.points_defense);
		subTblObj.SetInteger("points_agility", petdata.detail.points_agility);
		subTblObj.SetInteger("points_magical", petdata.detail.points_magical);
		subTblObj.SetInteger("value_attack", petdata.detail.value_attack);
		subTblObj.SetInteger("value_defensive", petdata.detail.value_defensive);
		subTblObj.SetInteger("value_agility", petdata.detail.value_agility);
		subTblObj.SetInteger("value_spirit", petdata.detail.value_spirit);
		subTblObj.SetInteger("value_recovery", petdata.detail.value_recovery);
		subTblObj.SetInteger("resist_poison", petdata.detail.resist_poison);
		subTblObj.SetInteger("resist_sleep", petdata.detail.resist_sleep);
		subTblObj.SetInteger("resist_medusa", petdata.detail.resist_medusa);
		subTblObj.SetInteger("resist_drunk", petdata.detail.resist_drunk);
		subTblObj.SetInteger("resist_chaos", petdata.detail.resist_chaos);
		subTblObj.SetInteger("resist_forget", petdata.detail.resist_forget);
		subTblObj.SetInteger("fix_critical", petdata.detail.fix_critical);
		subTblObj.SetInteger("fix_strikeback", petdata.detail.fix_strikeback);
		subTblObj.SetInteger("fix_accurancy", petdata.detail.fix_accurancy);
		subTblObj.SetInteger("fix_dodge", petdata.detail.fix_dodge);
		subTblObj.SetInteger("element_earth", petdata.detail.element_earth);
		subTblObj.SetInteger("element_water", petdata.detail.element_water);
		subTblObj.SetInteger("element_fire", petdata.detail.element_fire);
		subTblObj.SetInteger("element_wind", petdata.detail.element_wind);
		tableObj.SetObject("detail", subTblObj);
		allPetObj.SetObject(i + 1, tableObj);
	}
	allPetObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_IsPetSkillValid(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
	{
		L->PushBoolean(false);
		return 1;
	}
	int petid = args[1].GetInteger();
	int skillid = args[2].GetInteger();
	bool avoid = false;
	g_CGAInterface->IsPetSkillValid(petid, skillid, avoid);
	L->PushBoolean(avoid);
	return 1;
}

int CGLuaFun::Lua_GetPetSkillInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
	{
		return 0;
	}
	int petid = args[1].GetInteger();
	int skillid = args[2].GetInteger();
	CGA::cga_pet_skill_info_t skill;
	g_CGAInterface->GetPetSkillInfo(petid, skillid, skill);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetString("name", skill.name.c_str());
	tableObj.SetString("info", skill.info.c_str());
	tableObj.SetInteger("cost", skill.cost);
	tableObj.SetInteger("flags", skill.flags);
	tableObj.SetInteger("index", skill.index);
	tableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetPetSkillsInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	int petid = args[1].GetInteger();
	CGA::cga_pet_skills_info_t skills;
	g_CGAInterface->GetPetSkillsInfo(petid, skills);
	LuaObject skillObj(L);
	skillObj.AssignNewTable();
	for (int i = 0; i < skills.size(); ++i)
	{
		const CGA::cga_pet_skill_info_t skill = skills[i];
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		tableObj.SetString("name", skill.name.c_str());
		tableObj.SetString("info", skill.info.c_str());
		tableObj.SetInteger("cost", skill.cost);
		tableObj.SetInteger("flags", skill.flags);
		tableObj.SetInteger("index", skill.index);
		skillObj.SetObject(i + 1, tableObj);
	}
	skillObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetMapIndex(LuaState *L)
{
	int index1, index2, index3;
	std::string filemap;
	g_CGAInterface->GetMapIndex(index1, index2, index3, filemap);
	L->PushInteger(index1);
	L->PushInteger(index2);
	L->PushInteger(index3);
	L->PushString(filemap.c_str());
	return 4;
}

int CGLuaFun::Lua_GetMoveSpeed(LuaState *L)
{
	float x = 0, y = 0;
	g_CGAInterface->GetMoveSpeed(x, y);
	L->PushNumber(x);
	L->PushNumber(y);
	return 2;
}

int CGLuaFun::Lua_GetMouseXY(LuaState *L)
{
	int x = 0, y = 0;
	g_CGAInterface->GetMouseXY(x, y);
	L->PushInteger(x);
	L->PushInteger(y);
	return 2;
}

int CGLuaFun::Lua_GetMouseOrientation(LuaState *L)
{
	int dir;
	g_CGAInterface->GetMouseOrientation(dir);
	L->PushInteger(dir);
	return 1;
}

int CGLuaFun::Lua_WalkTo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	g_CGAInterface->WalkTo(x, y);
	return 0;
}

int CGLuaFun::Lua_TurnTo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	g_CGAInterface->TurnTo(x, y);
	return 0;
}

int CGLuaFun::Lua_SetMoveSpeed(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int speed = args[1].GetInteger();
	g_CGAInterface->SetMoveSpeed(speed);
	return 0;
}

int CGLuaFun::Lua_ForceMove(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int dir = args[1].GetInteger();
	bool bShow = args[2].GetBoolean();
	bool bRes = false;
	g_CGAInterface->ForceMove(dir, bShow, bRes);
	return 0;
}

int CGLuaFun::Lua_ForceMoveTo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 3)
		return 0;
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	bool bShow = args[3].GetBoolean();
	bool bRes = false;
	g_CGAInterface->ForceMoveTo(x, y, bShow, bRes);
	return 0;
}

int CGLuaFun::Lua_IsMapCellPassable(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int x = args[1].GetInteger();
	int y = args[2].GetInteger();
	bool bRes = false;
	g_CGAInterface->IsMapCellPassable(x, y, bRes);
	return 0;
}

int CGLuaFun::Lua_LogBack(LuaState *L)
{
	g_CGAInterface->LogBack();
	return 0;
}

int CGLuaFun::Lua_LogOut(LuaState *L)
{
	g_CGAInterface->LogOut();
	return 0;
}

int CGLuaFun::Lua_BackSelectServer(LuaState *L)
{
	g_CGAInterface->BackSelectServer();
	return 0;
}

int CGLuaFun::Lua_ChangeNickName(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	std::string titleName = args[1].GetString();
	bool bRet = false;
	g_CGAInterface->ChangeNickName(titleName, bRet);
	return 0;
}

int CGLuaFun::Lua_ChangeTitleName(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int index = args[1].GetInteger();
	bool bRet = false;
	g_CGAInterface->ChangeTitleName(index, bRet);
	return 0;
}

int CGLuaFun::Lua_ChangePersDesc(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 8)
	{
		return 0;
	}
	int changeBits = args[1].GetInteger();
	int sellIcon = args[2].GetInteger();
	std::string sellString = args[3].GetString();
	int buyIcon = args[4].GetInteger();
	std::string buyString = args[5].GetString();
	int wantIcon = args[6].GetInteger();
	std::string wantString = args[7].GetString();
	std::string descString = args[8].GetString();
	CGA::cga_pers_desc_t desc;
	desc.sellIcon = sellIcon;
	desc.sellString = sellString;
	desc.buyIcon = buyIcon;
	desc.buyString = buyString;
	desc.wantIcon = wantIcon;
	desc.wantString = wantString;
	desc.descString = descString;
	if (sellIcon)
		changeBits |= 1;
	if (!sellString.empty())
		changeBits |= 2;
	if (buyIcon)
		changeBits |= 4;
	if (!buyString.empty())
		changeBits |= 8;
	if (wantIcon)
		changeBits |= 0x10;
	if (!wantString.empty())
		changeBits |= 0x20;
	if (!descString.empty())
		changeBits |= 0x40;
	desc.changeBits = changeBits;
	g_CGAInterface->ChangePersDesc(desc);
	return 0;
}

int CGLuaFun::Lua_ChangePetName(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	std::string name = args[2].GetString();
	bool bRet = false;
	g_CGAInterface->ChangePetName(index, name, bRet);
	return 0;
}

int CGLuaFun::Lua_GetCardsInfo(LuaState *L)
{
	CGA::cga_cards_info_t cards;
	g_CGAInterface->GetCardsInfo(cards);
	LuaObject cardsObj(L);
	cardsObj.AssignNewTable();
	for (int i = 0; i < cards.size(); ++i)
	{
		CGA::cga_card_info_t card = cards[i];
		LuaObject tblObj(L);
		tblObj.AssignNewTable();
		tblObj.SetString("name", card.name.c_str());
		tblObj.SetString("nickname", card.nickname.c_str());
		tblObj.SetString("familyname", card.familyname.c_str());
		tblObj.SetInteger("server", card.server);
		tblObj.SetInteger("avatar", card.avatar);
		tblObj.SetInteger("level", card.level);
		tblObj.SetInteger("index", card.index);
		cardsObj.SetObject(i + 1, tblObj);
	}
	cardsObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetCardsRecvMsg(LuaState *L)
{
	CGA::cga_cards_recv_msg_t cards;
	g_CGAInterface->GetCardsRecvMsg(cards);
	LuaObject cardsObj(L);
	cardsObj.AssignNewTable();
	for (int i = 0; i < cards.size(); ++i)
	{
		CGA::cga_card_recv_msg_t card = cards[i];
		LuaObject tblObj(L);
		tblObj.AssignNewTable();
		tblObj.SetString("name", card.name.c_str());
		tblObj.SetInteger("index", card.index);
		LuaObject allMsgObj(L);
		allMsgObj.AssignNewTable();
		for (int n = 0; n < 10; ++n)
		{
			LuaObject msgObj(L);
			msgObj.AssignNewTable();
			msgObj.SetInteger("state", card.msgs[n].state);
			msgObj.SetString("date", card.msgs[n].date.c_str());
			msgObj.SetString("msg", card.msgs[n].msg.c_str());
			allMsgObj.SetObject(n + 1, msgObj);
		}
		tblObj.SetObject("msgs", allMsgObj);
		cardsObj.SetObject(i + 1, tblObj);
	}
	cardsObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetPicBooksInfo(LuaState *L)
{
	CGA::cga_picbooks_info_t books;
	g_CGAInterface->GetPicBooksInfo(books);
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	for (int i = 0; i < books.size(); ++i)
	{
		LuaObject subObj(L);
		subObj.AssignNewTable();
		subObj.SetString("name", books[i].name.c_str());
		subObj.SetInteger("can_catch", books[i].can_catch);
		subObj.SetInteger("card_type", books[i].card_type);
		subObj.SetInteger("race", books[i].race);
		subObj.SetInteger("index", books[i].index);
		subObj.SetInteger("image_id", books[i].image_id);
		subObj.SetInteger("rate_endurance", books[i].rate_endurance);
		subObj.SetInteger("rate_strength", books[i].rate_strength);
		subObj.SetInteger("rate_defense", books[i].rate_defense);
		subObj.SetInteger("rate_agility", books[i].rate_agility);
		subObj.SetInteger("rate_magical", books[i].rate_magical);
		subObj.SetInteger("element_earth", books[i].element_earth);
		subObj.SetInteger("element_water", books[i].element_water);
		subObj.SetInteger("element_fire", books[i].element_fire);
		subObj.SetInteger("element_wind", books[i].element_wind);
		subObj.SetInteger("skill_slots", books[i].skill_slots);
		tblObj.SetObject(i + 1, subObj);
	}
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_CheckHavePetPicBooksInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	bool bHave = false;
	std::string name = args[1].GetString();
	CGA::cga_picbooks_info_t books;
	g_CGAInterface->GetPicBooksInfo(books);
	for (auto &pet:books)
	{
		if (pet.name == name)
		{
			bHave = true;
			break;
		}
	}
	L->PushBoolean(bHave);
	return 1;
}

int CGLuaFun::Lua_GetDstPetPicBooksInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	bool bHave = false;
	std::string name = args[1].GetString();
	CGA::cga_picbooks_info_t books;
	g_CGAInterface->GetPicBooksInfo(books);
	for (int i=0;i< books.size();++i)
	{
		if (books[i].name == name)
		{
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetString("name", books[i].name.c_str());
			subObj.SetInteger("can_catch", books[i].can_catch);
			subObj.SetInteger("card_type", books[i].card_type);
			subObj.SetInteger("race", books[i].race);
			subObj.SetInteger("index", books[i].index);
			subObj.SetInteger("image_id", books[i].image_id);
			subObj.SetInteger("rate_endurance", books[i].rate_endurance);
			subObj.SetInteger("rate_strength", books[i].rate_strength);
			subObj.SetInteger("rate_defense", books[i].rate_defense);
			subObj.SetInteger("rate_agility", books[i].rate_agility);
			subObj.SetInteger("rate_magical", books[i].rate_magical);
			subObj.SetInteger("element_earth", books[i].element_earth);
			subObj.SetInteger("element_water", books[i].element_water);
			subObj.SetInteger("element_fire", books[i].element_fire);
			subObj.SetInteger("element_wind", books[i].element_wind);
			subObj.SetInteger("skill_slots", books[i].skill_slots);
			subObj.Push(L);
			bHave = true;
			break;
		}
	}
	if (bHave)
		return 1;
	else
		return 0;
}

int CGLuaFun::Lua_IsItemValid(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	int pos = args[1].GetInteger();
	bool bvaild = false;
	g_CGAInterface->IsItemValid(pos, bvaild);
	L->PushBoolean(bvaild);
	return 1;
}

int CGLuaFun::Lua_GetItemInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	int pos = args[1].GetInteger();
	CGA::cga_item_info_t iteminfo;
	g_CGAInterface->GetItemInfo(pos, iteminfo);
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetString("name", iteminfo.name.c_str());
	tblObj.SetString("attr", iteminfo.attr.c_str());
	tblObj.SetString("info", iteminfo.info.c_str());
	tblObj.SetInteger("itemid", iteminfo.itemid);
	tblObj.SetInteger("count", iteminfo.count);
	tblObj.SetInteger("pos", iteminfo.pos);
	tblObj.SetInteger("level", iteminfo.level);
	tblObj.SetInteger("type", iteminfo.type);
	tblObj.SetBoolean("assessed", iteminfo.assessed);
	tblObj.SetInteger("assess_flags", iteminfo.assess_flags);
	tblObj.SetInteger("image_id", iteminfo.image_id);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetItemsInfo(LuaState *L)
{
	CGA::cga_items_info_t iteminfos;

	g_CGAInterface->GetItemsInfo(iteminfos);
	LuaObject allTblObj(L);
	allTblObj.AssignNewTable();
	for (int i = 0; i < iteminfos.size(); ++i)
	{
		const CGA::cga_item_info_t iteminfo = iteminfos[i];
		LuaObject tblObj(L);
		tblObj.AssignNewTable();
		tblObj.SetString("name", iteminfo.name.c_str());
		tblObj.SetString("attr", iteminfo.attr.c_str());
		tblObj.SetString("info", iteminfo.info.c_str());
		tblObj.SetInteger("itemid", iteminfo.itemid);
		tblObj.SetInteger("count", iteminfo.count);
		tblObj.SetInteger("pos", iteminfo.pos);
		tblObj.SetInteger("level", iteminfo.level);
		tblObj.SetInteger("type", iteminfo.type);
		tblObj.SetBoolean("assessed", iteminfo.assessed);
		tblObj.SetInteger("assess_flags", iteminfo.assess_flags);
		tblObj.SetInteger("image_id", iteminfo.image_id);
		allTblObj.SetObject(i + 1, tblObj);
	}
	allTblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetBankItemsInfo(LuaState *L)
{
	CGA::cga_items_info_t iteminfos;
	g_CGAInterface->GetBankItemsInfo(iteminfos);
	LuaObject allTblObj(L);
	allTblObj.AssignNewTable();
	for (int i = 0; i < iteminfos.size(); ++i)
	{
		const CGA::cga_item_info_t iteminfo = iteminfos[i];
		LuaObject tblObj(L);
		tblObj.AssignNewTable();
		tblObj.SetString("name", iteminfo.name.c_str());
		tblObj.SetString("attr", iteminfo.attr.c_str());
		tblObj.SetString("info", iteminfo.info.c_str());
		tblObj.SetInteger("itemid", iteminfo.itemid);
		tblObj.SetInteger("count", iteminfo.count);
		tblObj.SetInteger("pos", iteminfo.pos);
		tblObj.SetInteger("level", iteminfo.level);
		tblObj.SetInteger("type", iteminfo.type);
		tblObj.SetBoolean("assessed", iteminfo.assessed);
		tblObj.SetInteger("assess_flags", iteminfo.assess_flags);
		tblObj.SetInteger("image_id", iteminfo.image_id);
		allTblObj.SetObject(i + 1, tblObj);
	}
	allTblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_SendMail(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = -1;
	if (args[1].IsString())
	{
		QString friendName = args[1].GetString();
		CGA::cga_cards_info_t cards;
		g_CGAInterface->GetCardsInfo(cards);
		if (!friendName.isEmpty())
		{
			for (auto info : cards)
			{
				if (info.name == friendName.toStdString())
				{
					index = info.index;
					break;
				}
			}
		}
	}
	else
	{
		index = args[1].GetInteger();
	}
	std::string msg = args[2].GetString();
	bool bRet = false;
	g_CGAInterface->SendMail(index, msg, bRet);
	return 0;
}

int CGLuaFun::Lua_SendPetMail(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	int petIndex = args[2].GetInteger();
	int itempos = args[3].GetInteger();
	std::string msg = args[4].GetString();
	bool bRet = false;
	g_CGAInterface->SendPetMail(index, petIndex, itempos, msg, bRet);
	return 0;
}
//接收邮件 按顺序读 读过的 置0
int CGLuaFun::Lua_RecvMail(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int index = -1;
	QString friendName;
	if (args[1].IsString())
	{
		friendName = args[1].GetString();
	}
	else
	{
		index = args[1].GetInteger();
	}
	CGA::cga_cards_recv_msg_t recv_msgs;
	g_CGAInterface->GetCardsRecvMsg(recv_msgs);
	for (int i = 0; i < recv_msgs.size(); ++i)
	{
		if (recv_msgs[i].index == index || recv_msgs[i].name == friendName.toStdString())
		{
			for (int n = 9; n >= 0; --n)
			{
				if (recv_msgs[i].msgs[n].state)
				{
					LuaObject tblObj(L);
					tblObj.AssignNewTable();
					tblObj.SetString("name", recv_msgs[i].name.c_str());
					tblObj.SetInteger("index", recv_msgs[i].index);
					tblObj.SetString("msg", recv_msgs[i].msgs[n].msg.c_str());
					tblObj.SetString("date", recv_msgs[i].msgs[n].date.c_str());
					tblObj.SetInteger("state", recv_msgs[i].msgs[n].state);
					tblObj.Push(L);
					g_CGAInterface->SetCardRecvMsgState(recv_msgs[i].index, n, 0);
					return 1;
				}
			}
			//都读了 默认返回第一条
			LuaObject tblObj(L);
			tblObj.AssignNewTable();
			tblObj.SetString("name", recv_msgs[i].name.c_str());
			tblObj.SetInteger("index", recv_msgs[i].index);
			tblObj.SetString("msg", recv_msgs[i].msgs[0].msg.c_str());
			tblObj.SetString("date", recv_msgs[i].msgs[0].date.c_str());
			tblObj.SetInteger("state", recv_msgs[i].msgs[0].state);
			tblObj.Push(L);
			return 1;
		}
	}
	return 0;
}

int CGLuaFun::Lua_SetMailState(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 3)
		return 0;
	int index = -1;
	QString friendName;
	if (args[1].IsInteger())
	{
		index = args[1].GetInteger();
	}
	else if (args[1].IsString())
	{
		QString friendName = args[1].GetString();
		CGA::cga_cards_info_t cards;
		g_CGAInterface->GetCardsInfo(cards);
		if (!friendName.isEmpty())
		{
			for (auto info : cards)
			{
				if (info.name == friendName.toStdString())
				{
					index = info.index;
					break;
				}
			}
		}
	}
	int item = args[2].GetInteger();
	int state = args[3].GetInteger();
	g_CGAInterface->SetCardRecvMsgState(index, item, state);
	return 0;
}

int CGLuaFun::Lua_RecvMailEx(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = -1;
	int mailNum = args[2].GetInteger(); //查看第几个邮件 0-9
	QString friendName;
	if (args[1].IsString())
	{
		friendName = args[1].GetString();
	}
	else
	{
		index = args[1].GetInteger();
	}
	if (mailNum > 9 || mailNum < 0)
		mailNum = 0;
	CGA::cga_cards_recv_msg_t recv_msgs;
	g_CGAInterface->GetCardsRecvMsg(recv_msgs);
	for (int i = 0; i < recv_msgs.size(); ++i)
	{
		if (recv_msgs[i].index == index || recv_msgs[i].name == friendName.toStdString())
		{
			LuaObject tblObj(L);
			tblObj.AssignNewTable();
			tblObj.SetString("name", recv_msgs[i].name.c_str());
			tblObj.SetInteger("index", recv_msgs[i].index);
			tblObj.SetString("msg", recv_msgs[i].msgs[mailNum].msg.c_str());
			tblObj.SetString("date", recv_msgs[i].msgs[mailNum].date.c_str());
			tblObj.SetInteger("state", recv_msgs[i].msgs[mailNum].state);
			tblObj.Push(L);
			return 1;
		}
	}
	return 0;
}

int CGLuaFun::Lua_RecvAllMail(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int index = -1;
	QString friendName;
	if (args[1].IsString())
	{
		friendName = args[1].GetString();
	}
	else
	{
		index = args[1].GetInteger();
	}
	CGA::cga_cards_recv_msg_t recv_msgs;
	g_CGAInterface->GetCardsRecvMsg(recv_msgs);
	for (int i = 0; i < recv_msgs.size(); ++i)
	{
		if (recv_msgs[i].index == index || recv_msgs[i].name == friendName.toStdString())
		{
			LuaObject allTblObj(L);
			allTblObj.AssignNewTable();
			allTblObj.SetString("name", recv_msgs[i].name.c_str());
			allTblObj.SetInteger("index", recv_msgs[i].index);
			LuaObject allMsgObj(L);
			allMsgObj.AssignNewTable();
			for (int n = 0; n < 10; ++n)
			{
				LuaObject tblObj(L);
				tblObj.AssignNewTable();
				tblObj.SetString("msg", recv_msgs[i].msgs[n].msg.c_str());
				tblObj.SetString("date", recv_msgs[i].msgs[n].date.c_str());
				tblObj.SetInteger("state", recv_msgs[i].msgs[n].state);
				tblObj.Push(L);
				allMsgObj.SetObject(n + 1, tblObj);
			}
			allTblObj.SetObject("msgs", allMsgObj);
			allTblObj.Push(L);
			return 1;
		}
	}
	return 0;
}

int CGLuaFun::Lua_DeleteCard(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	bool packed = args[2].GetInteger();
	bool bRet = false;
	g_CGAInterface->DeleteCard(index, packed, bRet);
	return 0;
}

int CGLuaFun::Lua_PlayGesture(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int index = args[1].GetInteger();
	bool bRet = false;
	g_CGAInterface->PlayGesture(index);
	return 0;
}

int CGLuaFun::Lua_CreateCharacter(LuaState *L)
{
	//LuaStack args(L);
	//bool isCreate = args[1].GetBoolean();
	//QString sName = args[2].GetString();
	//int nCharType = args[3].GetInteger();
	//int nEye = args[4].GetInteger();
	//int nMouth = args[5].GetInteger();
	//int nColor = args[6].GetInteger();
	//QString sPoint = args[7].GetString();
	//QString sCrystal = args[8].GetString();
	//CGA::cga_create_chara_t create_char;
	//g_CGAInterface->CreateCharacter(create_char);
	return 0;
}

int CGLuaFun::Lua_LoginGameServer(LuaState *L)
{
	//	g_CGAInterface->LoginGameServer(index);
	return 0;
}

int CGLuaFun::Lua_StartWork(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	int subIndex = args[2].GetInteger();
	bool bRet = false;
	g_CGAInterface->StartWork(index, subIndex, bRet);
	return 0;
}

int CGLuaFun::Lua_SetWorkAcceleration(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int percent = args[1].GetInteger();
	g_CGAInterface->SetWorkAcceleration(percent);
	return 0;
}

int CGLuaFun::Lua_SetImmediateDoneWork(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	bool bEnabled = args[1].GetBoolean();
	g_CGAInterface->SetImmediateDoneWork(bEnabled);
	return 0;
}

int CGLuaFun::Lua_GetImmediateDoneWorkState(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int state = args[1].GetInteger();
	g_CGAInterface->GetImmediateDoneWorkState(state);
	return 0;
}

int CGLuaFun::Lua_CraftItem(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	CGA::cga_craft_item_t craft;
	if (!args[1].IsTable())
		return 0;
	LuaObject luaTable = args[1];
	int skill_index = luaTable.GetByName("skill_index").ToInteger();
	int subskill_index = luaTable.GetByName("subskill_index").ToInteger();
	int sub_type = luaTable.GetByName("sub_type").ToInteger();
	LuaObject itemposTbl = luaTable.GetByName("count");
	craft.skill_index = skill_index;
	craft.subskill_index = subskill_index;
	craft.sub_type = sub_type;
	craft.itempos[0] = itemposTbl.GetByIndex(0).GetInteger();
	craft.itempos[1] = itemposTbl.GetByIndex(1).GetInteger();
	craft.itempos[2] = itemposTbl.GetByIndex(2).GetInteger();
	craft.itempos[3] = itemposTbl.GetByIndex(3).GetInteger();
	craft.itempos[4] = itemposTbl.GetByIndex(4).GetInteger();
	craft.itempos[5] = itemposTbl.GetByIndex(5).GetInteger();
	bool bRes = false;
	g_CGAInterface->CraftItem(craft, bRes);
	return 0;
}

int CGLuaFun::Lua_AssessItem(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	int pos = args[2].GetInteger();
	bool bRes = false;
	g_CGAInterface->AssessItem(index, pos, bRes);
	return 0;
}

int CGLuaFun::Lua_GetCraftInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	int index = args[1].GetInteger();
	int subIndex = args[2].GetInteger();
	CGA::cga_craft_info_t craft;
	g_CGAInterface->GetCraftInfo(index, subIndex, craft);
	LuaObject tableObj(L);
	tableObj.AssignNewTable();
	tableObj.SetInteger("id", craft.id);
	tableObj.SetInteger("cost", craft.cost);
	tableObj.SetInteger("level", craft.level);
	tableObj.SetInteger("itemid", craft.itemid);
	tableObj.SetInteger("index", craft.index);
	tableObj.SetString("name", craft.name.c_str());
	tableObj.SetString("info", craft.info.c_str());
	tableObj.SetBoolean("available", craft.available);

	LuaObject subTableObj(L);
	subTableObj.AssignNewTable();
	for (int i = 0; i < 5; ++i)
	{
		LuaObject mtTbObj(L);
		mtTbObj.AssignNewTable();
		mtTbObj.SetInteger("itemid", craft.materials[i].itemid);
		mtTbObj.SetInteger("count", craft.materials[i].count);
		mtTbObj.SetString("name", craft.materials[i].name.c_str());
		subTableObj.SetObject(i + 1, mtTbObj);
	}
	tableObj.SetObject("materials", subTableObj);
	tableObj.Push(L); //这个不能省 省了就无效报错了
	return 1;
}

int CGLuaFun::Lua_GetCraftsInfo(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int index = args[1].GetInteger();
	CGA::cga_crafts_info_t crafts;
	g_CGAInterface->GetCraftsInfo(index, crafts);
	LuaObject allTableObj(L);
	allTableObj.AssignNewTable();
	for (int n = 0; n < crafts.size(); ++n)
	{
		const CGA::cga_craft_info_t &craft = crafts[n];
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		tableObj.SetInteger("id", craft.id);
		tableObj.SetInteger("cost", craft.cost);
		tableObj.SetInteger("level", craft.level);
		tableObj.SetInteger("itemid", craft.itemid);
		tableObj.SetInteger("index", craft.index);
		tableObj.SetString("name", craft.name.c_str());
		tableObj.SetString("info", craft.info.c_str());
		tableObj.SetBoolean("available", craft.available);

		LuaObject subTableObj(L);
		subTableObj.AssignNewTable();
		for (int i = 0; i < 5; ++i)
		{
			LuaObject mtTbObj(L);
			mtTbObj.AssignNewTable();
			mtTbObj.SetInteger("itemid", craft.materials[i].itemid);
			mtTbObj.SetInteger("count", craft.materials[i].count);
			mtTbObj.SetString("name", craft.materials[i].name.c_str());
			subTableObj.SetObject(i + 1, mtTbObj);
		}
		tableObj.SetObject("materials", subTableObj);
		allTableObj.SetObject(n + 1, tableObj);
	}
	allTableObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetCraftStatus(LuaState *L)
{
	int status = 0;
	g_CGAInterface->GetCraftStatus(status);
	L->PushInteger(status);
	return 1;
}

int CGLuaFun::Lua_DoRequest(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	int type = args[1].GetInteger();
	bool bRes = false;
	g_CGAInterface->DoRequest(type, bRes);
	L->PushBoolean(bRes);
	return 1;
}

int CGLuaFun::Lua_AddAllTradeItems(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	int step = args[1].GetInteger();
	g_CGAInterface->AddAllTradeItems(step);
	return 0;
}

int CGLuaFun::Lua_IsUIDialogPresent(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	int dlg = args[1].GetInteger();
	bool bRet = false;
	g_CGAInterface->IsUIDialogPresent(dlg, bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_GetTeamPlayerInfo(LuaState *L)
{
	CGA::cga_team_players_t teams;
	g_CGAInterface->GetTeamPlayerInfo(teams);
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	for (int i = 0; i < teams.size(); ++i)
	{
		CGA::cga_team_player_t team = teams[i];
		LuaObject subTblObj(L);
		subTblObj.AssignNewTable();
		subTblObj.SetString("name", team.name.c_str());
		subTblObj.SetInteger("unit_id", team.unit_id);
		subTblObj.SetInteger("hp", team.hp);
		subTblObj.SetInteger("maxhp", team.maxhp);
		subTblObj.SetInteger("mp", team.mp);
		subTblObj.SetInteger("maxmp", team.maxmp);
		subTblObj.SetInteger("xpos", team.xpos);
		subTblObj.SetInteger("ypos", team.ypos);
		tblObj.SetObject(i + 1, subTblObj);
	}
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_FixMapWarpStuck(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	int type = args[1].GetInteger();
	g_CGAInterface->FixMapWarpStuck(type);
	return 0;
}

int CGLuaFun::Lua_SetNoSwitchAnim(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	bool state = args[1].GetBoolean();
	g_CGAInterface->SetNoSwitchAnim(state);
	return 0;
}

int CGLuaFun::Lua_GetMoveHistory(LuaState *L)
{
	//g_CGAInterface->GetMoveHistory();
	return 0;
}

int CGLuaFun::Lua_EnableFlags(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
	{
		L->PushBoolean(false);
		return 1;
	}
	int type = args[1].GetInteger();
	bool state = args[2].GetBoolean();
	bool bRes = false;
	g_CGAInterface->EnableFlags(type, state, bRes);
	L->PushBoolean(bRes);
	return 1;
}

int CGLuaFun::Lua_SetWindowResolution(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
	{
		return 0;
	}
	int w = args[1].GetInteger();
	int h = args[2].GetInteger();
	g_CGAInterface->SetWindowResolution(w, h);
	return 0;
}

int CGLuaFun::Lua_RequestDownloadMap(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 4)
	{
		return 0;
	}
	int xbottom = args[1].GetInteger();
	int ybottom = args[2].GetInteger();
	int xsize = args[3].GetInteger();
	int ysize = args[4].GetInteger();

	g_CGAInterface->RequestDownloadMap(xbottom, ybottom, xsize, ysize);
	return 0;
}

int CGLuaFun::Lua_GetNextAnimTickCount(LuaState *L)
{
	double dVal = 0;
	g_CGAInterface->GetNextAnimTickCount(dVal);
	L->PushNumber(dVal);
	return 1;
}

int CGLuaFun::Lua_IsBattleUnitValid(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	int tgtpos = args[1].GetInteger();
	bool bRes = false;
	g_CGAInterface->IsBattleUnitValid(tgtpos, bRes);
	L->PushBoolean(bRes);
	return 1;
}

int CGLuaFun::Lua_GetBattleUnit(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		return 0;
	}
	int tgtpos = args[1].GetInteger();
	CGA::cga_battle_unit_t unit;
	g_CGAInterface->GetBattleUnit(tgtpos, unit);
	LuaObject subTblObj(L);
	subTblObj.AssignNewTable();
	subTblObj.SetString("name", unit.name.c_str());
	subTblObj.SetInteger("modelid", unit.modelid);
	subTblObj.SetInteger("level", unit.level);
	subTblObj.SetInteger("curhp", unit.curhp);
	subTblObj.SetInteger("maxhp", unit.maxhp);
	subTblObj.SetInteger("curmp", unit.curmp);
	subTblObj.SetInteger("maxmp", unit.maxmp);
	subTblObj.SetInteger("flags", unit.flags);
	subTblObj.SetInteger("pos", unit.pos);
	subTblObj.SetInteger("petriding_modelid", unit.petriding_modelid);
	subTblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetBattleUnits(LuaState *L)
{
	CGA::cga_battle_units_t battleUnits;
	g_CGAInterface->GetBattleUnits(battleUnits);
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	for (int i = 0; i < battleUnits.size(); ++i)
	{
		const CGA::cga_battle_unit_t &unit = battleUnits[i];
		LuaObject subTblObj(L);
		subTblObj.AssignNewTable();
		subTblObj.SetString("name", unit.name.c_str());
		subTblObj.SetInteger("modelid", unit.modelid);
		subTblObj.SetInteger("level", unit.level);
		subTblObj.SetInteger("curhp", unit.curhp);
		subTblObj.SetInteger("maxhp", unit.maxhp);
		subTblObj.SetInteger("curmp", unit.curmp);
		subTblObj.SetInteger("maxmp", unit.maxmp);
		subTblObj.SetInteger("flags", unit.flags);
		subTblObj.SetInteger("pos", unit.pos);
		subTblObj.SetInteger("petriding_modelid", unit.petriding_modelid);
		tblObj.SetObject(i + 1, subTblObj);
	}
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_GetBattleContext(LuaState *L)
{
	CGA::cga_battle_context_t battleCtx;
	g_CGAInterface->GetBattleContext(battleCtx);
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetInteger("round_count", battleCtx.round_count);
	tblObj.SetInteger("player_pos", battleCtx.player_pos);
	tblObj.SetInteger("player_status", battleCtx.player_status);
	tblObj.SetInteger("skill_performed", battleCtx.skill_performed);
	tblObj.SetInteger("skill_allowbit", battleCtx.skill_allowbit);
	tblObj.SetInteger("petskill_allowbit", battleCtx.petskill_allowbit);
	tblObj.SetInteger("weapon_allowbit", battleCtx.weapon_allowbit);
	tblObj.SetInteger("petid", battleCtx.petid);
	tblObj.SetInteger("effect_flags", battleCtx.effect_flags);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_BattleNormalAttack(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
	{
		L->PushBoolean(false);
		return 1;
	}
	int tgtpos = args[1].GetInteger();
	bool bRet = false;
	g_CGAInterface->BattleNormalAttack(tgtpos, bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleSkillAttack(LuaState *L)
{

	LuaStack args(L);
	if (args.Count() < 4)
	{
		L->PushBoolean(false);
		return 1;
	}
	int pos = args[1].GetInteger();
	int lv = args[2].GetInteger();
	int tgtpos = args[3].GetInteger();
	bool packet = args[4].GetBoolean();
	bool bRet = false;
	g_CGAInterface->BattleSkillAttack(pos, lv, tgtpos, packet, bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleRebirth(LuaState *L)
{
	bool bRet = false;
	g_CGAInterface->BattleRebirth(bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleGuard(LuaState *L)
{
	bool bRet = false;
	g_CGAInterface->BattleGuard(bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleEscape(LuaState *L)
{
	bool bRet = false;
	g_CGAInterface->BattleEscape(bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleExchangePosition(LuaState *L)
{
	bool bRet = false;
	g_CGAInterface->BattleExchangePosition(bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleDoNothing(LuaState *L)
{
	bool bRet = false;
	g_CGAInterface->BattleDoNothing(bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleChangePet(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
	{
		L->PushBoolean(false);
		return 1;
	}
	int pos = args[1].GetInteger();
	bool bRet = false;
	g_CGAInterface->BattleChangePet(pos, bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleUseItem(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
	{
		L->PushBoolean(false);
		return 1;
	}
	int pos = args[1].GetInteger();
	int tgtpos = args[2].GetInteger();
	bool bRet = false;
	g_CGAInterface->BattleUseItem(pos, tgtpos, bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattlePetSkillAttack(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 3)
	{
		L->PushBoolean(false);
		return 1;
	}
	int skillpos = args[1].GetInteger();
	int tgtpos = args[2].GetInteger();
	bool bPack = args[3].GetBoolean();
	bool bRet = false;
	g_CGAInterface->BattlePetSkillAttack(skillpos, tgtpos, bPack, bRet);
	L->PushBoolean(bRet);
	return 1;
}

int CGLuaFun::Lua_BattleSetHighSpeedEnabled(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	bool bState = args[1].GetBoolean();
	g_CGAInterface->BattleSetHighSpeedEnabled(bState);
	return 0;
}

int CGLuaFun::Lua_SetGameTextUIEnabled(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	bool bState = args[1].GetBoolean();
	g_CGAInterface->SetGameTextUIEnabled(bState);
	return 0;
}

int CGLuaFun::Lua_SetGameTextUICurrentScript(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	std::string script = args[1].GetString();
	g_CGAInterface->SetGameTextUICurrentScript(script);
	return 0;
}

int CGLuaFun::Lua_GetBattleEndTick(LuaState *L)
{
	int msec = 0;
	g_CGAInterface->GetBattleEndTick(msec);
	L->PushInteger(msec);
	return 1;
}

int CGLuaFun::Lua_SetBattleEndTick(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int msec = args[1].GetInteger();
	g_CGAInterface->SetBattleEndTick(msec);
	return 0;
}

int CGLuaFun::Lua_SetWorkDelay(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 1)
		return 0;
	int delay = args[1].GetInteger();
	g_CGAInterface->SetWorkDelay(delay);
	return 0;
}

int CGLuaFun::Lua_SelectGidData(LuaState *L)
{
	LuaStack args(L);
	if (args.Count() < 2)
		return 0;
	QString sGid = args[1].GetString();
	int roleIndex = args[2].GetInteger();
	if (sGid.isEmpty())
		return 0;
	CGData::SelectGidDataResponse reply;
	bool bRet = RpcSocketClient::getInstance().SelectGidData(sGid, roleIndex, reply);
	if (bRet)
	{
		LuaObject tableObj(L);
		tableObj.AssignNewTable();
		auto info = reply.character_data();
		{
			tableObj.SetString("gid", reply.gid().c_str());
			tableObj.SetString("name", reply.character_name().c_str());
			tableObj.SetString("job", info.job().c_str());
			tableObj.SetInteger("level", info.level());
			tableObj.SetInteger("gold", info.gold());
			tableObj.SetInteger("bankgold", info.bank_gold());
			tableObj.SetInteger("hp", info.base_data().hp());
			tableObj.SetInteger("maxhp", info.base_data().maxhp());
			tableObj.SetInteger("mp", info.base_data().mp());
			tableObj.SetInteger("maxmp", info.base_data().maxmp());
			tableObj.SetInteger("xp", info.base_data().xp());
			tableObj.SetInteger("maxxp", info.base_data().maxxp());
			tableObj.SetInteger("petid", info.petid());
			tableObj.SetInteger("unitid", info.unitid());
			tableObj.SetInteger("punchclock", info.punchclock());
			tableObj.SetInteger("usingpunchclock", info.usingpunchclock());
			tableObj.SetInteger("health", info.base_data().health());
			tableObj.SetInteger("souls", info.souls());
			tableObj.SetInteger("direction", info.direction());
			tableObj.SetInteger("manu_endurance", info.manu_endurance());
			tableObj.SetInteger("manu_skillful", info.manu_skillful());
			tableObj.SetInteger("manu_intelligence", info.manu_intelligence());
			tableObj.SetInteger("value_charisma", info.value_charisma());
			LuaObject titleObj(L);
			titleObj.AssignNewTable();
			for (size_t i = 0; i < info.titles_size(); ++i)
			{
				std::string sTitle = info.titles().at(i);
				LuaObject subObj(L);
				subObj.AssignNewTable();
				subObj.SetString("name", sTitle.c_str());
				titleObj.SetObject(i + 1, subObj);
			}
			tableObj.SetObject("titles", titleObj);
			tableObj.SetInteger("titlescount", info.titles_size());
			LuaObject subObj(L);
			subObj.AssignNewTable();
			subObj.SetInteger("points_remain", info.detail().points_remain());
			subObj.SetInteger("points_endurance", info.detail().points_endurance());
			subObj.SetInteger("points_strength", info.detail().points_strength());
			subObj.SetInteger("points_defense", info.detail().points_defense());
			subObj.SetInteger("points_agility", info.detail().points_agility());
			subObj.SetInteger("points_magical", info.detail().points_magical());
			subObj.SetInteger("value_attack", info.detail().value_attack());
			subObj.SetInteger("value_defensive", info.detail().value_defensive());
			subObj.SetInteger("value_agility", info.detail().value_agility());
			subObj.SetInteger("value_spirit", info.detail().value_spirit());
			subObj.SetInteger("value_recovery", info.detail().value_recovery());
			subObj.SetInteger("resist_poison", info.detail().resist_poison());
			subObj.SetInteger("resist_sleep", info.detail().resist_sleep());
			subObj.SetInteger("resist_medusa", info.detail().resist_medusa());
			subObj.SetInteger("resist_drunk", info.detail().resist_drunk());
			subObj.SetInteger("resist_chaos", info.detail().resist_chaos());
			subObj.SetInteger("resist_forget", info.detail().resist_forget());
			subObj.SetInteger("fix_critical", info.detail().fix_critical());
			subObj.SetInteger("fix_strikeback", info.detail().fix_strikeback());
			subObj.SetInteger("fix_accurancy", info.detail().fix_accurancy());
			subObj.SetInteger("fix_dodge", info.detail().fix_dodge());
			subObj.SetInteger("element_earth", info.detail().element_earth());
			subObj.SetInteger("element_water", info.detail().element_water());
			subObj.SetInteger("element_fire", info.detail().element_fire());
			subObj.SetInteger("element_wind", info.detail().element_wind());
			tableObj.SetObject("detail", subObj);
			LuaObject descObj(L);
			descObj.AssignNewTable();
			descObj.SetInteger("changeBits", info.pers_desc().changebits());
			descObj.SetInteger("sellIcon", info.pers_desc().sellicon());
			descObj.SetString("sellString", info.pers_desc().sellstring().c_str());
			descObj.SetInteger("buyIcon", info.pers_desc().buyicon());
			descObj.SetString("buyString", info.pers_desc().buystring().c_str());
			descObj.SetInteger("wantIcon", info.pers_desc().wanticon());
			descObj.SetString("wantString", info.pers_desc().wantstring().c_str());
			descObj.SetString("descString", info.pers_desc().descstring().c_str());
			tableObj.SetObject("persdesc", descObj);

			LuaObject itemsObj(L);
			itemsObj.AssignNewTable();
			for (size_t i = 0; i < reply.items_size(); ++i)
			{
				auto tItem = reply.items().at(i);
				LuaObject subObj(L);
				subObj.AssignNewTable();
				subObj.SetString("name", tItem.name().c_str());
				subObj.SetString("attr", tItem.attr().c_str());
				subObj.SetInteger("count", tItem.count());
				subObj.SetInteger("itemid", tItem.item_id());
				subObj.SetInteger("pos", tItem.pos());
				subObj.SetInteger("type", tItem.type());
				itemsObj.SetObject(i + 1, subObj);
			}
			tableObj.SetObject("items", itemsObj);
			tableObj.SetInteger("itemscount", reply.items_size());
			LuaObject petsObj(L);
			petsObj.AssignNewTable();
			for (size_t i = 0; i < reply.pet_data_size(); ++i)
			{
				auto tPet = reply.pet_data().at(i);
				LuaObject subObj(L);
				subObj.AssignNewTable();
				subObj.SetString("name", tPet.base_data().name().c_str());
				subObj.SetString("realname", tPet.real_name().c_str());
				subObj.SetInteger("hp", tPet.base_data().hp());
				subObj.SetInteger("maxhp", tPet.base_data().maxhp());
				subObj.SetInteger("mp", tPet.base_data().mp());
				subObj.SetInteger("maxmp", tPet.base_data().maxmp());
				subObj.SetInteger("level", tPet.base_data().level());
				subObj.SetInteger("xp", tPet.base_data().xp());
				subObj.SetInteger("maxxp", tPet.base_data().maxxp());
				subObj.SetInteger("health", tPet.base_data().health());
				subObj.SetInteger("grade", tPet.grade());
				subObj.SetInteger("index", tPet.index());
				subObj.SetInteger("loyality", tPet.loyality());
				subObj.SetInteger("state", tPet.state());
				subObj.SetInteger("lossmingrade", tPet.lossmingrade());
				subObj.SetInteger("lossmaxgrade", tPet.lossmaxgrade());
				subObj.SetInteger("skillslots", tPet.skillslots());
				subObj.SetInteger("race", tPet.race());
				petsObj.SetObject(i + 1, subObj);
			}
			tableObj.SetObject("pets", petsObj);
			tableObj.SetInteger("petscount", reply.pet_data_size());
		}
		//LuaObject skillObj(L);
		//skillObj.AssignNewTable();
		//CGA::cga_skills_info_t skillsinfo;
		//if (g_CGAInterface->GetSkillsInfo(skillsinfo))
		//{
		//	for (size_t i = 0; i < skillsinfo.size(); ++i)
		//	{
		//		const CGA::cga_skill_info_t &skinfo = skillsinfo.at(i);
		//		LuaObject subObj(L);
		//		subObj.AssignNewTable();
		//		subObj.SetString("name", skinfo.name.c_str());
		//		subObj.SetInteger("index", skinfo.index);
		//		subObj.SetInteger("lv", skinfo.lv);
		//		subObj.SetInteger("maxlv", skinfo.maxlv);
		//		subObj.SetInteger("xp", skinfo.xp);
		//		subObj.SetInteger("maxxp", skinfo.maxxp);
		//		subObj.SetInteger("pos", skinfo.pos);

		//		CGA::cga_subskills_info_t subskillsinfo;
		//		LuaObject subSkillObj(L);
		//		subSkillObj.AssignNewTable();
		//		//获取合成物品信息
		//		CGA::cga_craft_info_t craftInfo;
		//		if (g_CGAInterface->GetSubSkillsInfo(skinfo.index, subskillsinfo))
		//		{
		//			for (size_t j = 0; j < subskillsinfo.size(); ++j)
		//			{
		//				const CGA::cga_subskill_info_t &subskinfo = subskillsinfo.at(j);

		//				LuaObject subTbObj(L);
		//				subTbObj.AssignNewTable();
		//				subTbObj.SetString("name", subskinfo.name.c_str());
		//				subTbObj.SetString("info", subskinfo.info.c_str());
		//				subTbObj.SetInteger("level", subskinfo.level);
		//				subTbObj.SetInteger("cost", subskinfo.cost);
		//				subTbObj.SetInteger("flags", subskinfo.flags);
		//				subTbObj.SetBoolean("available", subskinfo.available);
		//				subSkillObj.SetObject(j + 1, subTbObj);
		//			}
		//		}
		//		subObj.SetObject("subskill", subSkillObj);

		//		skillObj.SetObject(i + 1, subObj);
		//	}
		//}
		//tableObj.SetObject("skill", skillObj);
		tableObj.Push(L);
		return 1;
	}
	return 0;
}

void CGLuaFun::PauseScript()
{
	int nResult = 0;
	lua_yield(m_pLuaState, nResult);
}

void CGLuaFun::ResumeScript()
{
	int arg = 0;
	int nRes = 0;
	lua_resume(m_pLuaState, nullptr, arg, &nRes);
}

int CGLuaFun::GetLuaCurrentLine()
{
	return 0;
}

int CGLuaFun::LoadScript(QString scriptPath)
{
	//	QtConcurrent::run(LoadScriptThread, this, scriptPath);
	luaL_dofile(m_pLuaState, scriptPath.toStdString().c_str()); //执行脚本文件
	return 0;
}

void CGLuaFun::StopScript()
{
}

void CGLuaFun::LoadScriptThread(CGLuaFun *pThis, QString scriptPath)
{
	luaL_dofile(pThis->m_pLuaState, scriptPath.toStdString().c_str()); //执行脚本文件
																	   //	pThis->m_pLuaState->
																	   //lua_getinfo(pThis->m_pLuaState,)

	/*lua_Debug ar;
	lua_getstack(pThis->m_pLuaState, 1, &ar);
	lua_getinfo(pThis->m_pLuaState, "nSl", &ar);*/
	//	lua_sethook(pThis->m_pLuaState, &ar);
	//	int line = ar.currentline
}

void CGLuaFun::moveto(int x, int y)
{
	g_pGameFun->MoveTo(x, y);
}

void CGLuaFun::LUA_Sleep(int nNum)
{
	Sleep(nNum);
}
int CGLuaFun::LUA_GetTickCount()
{
	DWORD nVal = GetTickCount();

	return nVal;
}

bool CGLuaFun::LUA_AutoRunToTarget(int x, int y, const QString &mapName)
{
	DWORD dwWaitTime = GetTickCount(); //第一次记录时间，人物此时是非跑动状态
	DWORD dwBGTime = GetTickCount();   //第一次记录时间
	while (!g_pGameCtrl->GetExitGame())
	{
		DWORD dwCurTime = GetTickCount();	 //人物不动的状态
		if (dwCurTime - dwWaitTime > 600000) //10分钟没有走到目标场景，可能是走向了不可寻路场景
		{
			//dbgPrint("跨图寻路10分钟没走到目标，可能是走向了未知地点");
			break;
		}
		if (g_pGameFun->GetMapName() == mapName)
		{
			if (LUA_MoveToTarget(x, y)) //跨图之后，要用本地寻路走到目标
				return true;
		}
	}
	return true;
}
bool CGLuaFun::LUA_MoveToTarget(int x, int y)
{
	bool bMov = false;
	int nCount = 0;
	bool nCHange = false;
	float fOldX = 0.0f, fOldY = 0.0f;

	DWORD dwWaitTime = GetTickCount(); //第一次记录时间，人物此时是非跑动状态
	DWORD dwBGTime = GetTickCount();   //第一次记录时间
	while (!g_pGameCtrl->GetExitGame())
	{
		DWORD dwCurTime = GetTickCount();	 //人物不动的状态
		if (dwCurTime - dwWaitTime > 180000) //
		{
			//dbgPrint("寻路3分钟没走到目标，可能是走向了未知地点");
			break;
		}
		int gameState;
		g_CGAInterface->GetGameStatus(gameState);
		if (gameState == 9)
		{
			Sleep(3000);
			break;
		}
		Sleep(200);
	}

	return false;
}

void CGLuaFun::LUA_Renew(int x, int y)
{
	g_pGameFun->TurnAboutEx(x, y);
}

void CGLuaFun::CallRegisterFun(LuaState *L, const QString &topic, const QString &msg)
{
	if (m_lastRegisterFunName.isEmpty())
		return;
	lua_getglobal(L->GetCState(), m_lastRegisterFunName.toStdString().c_str());
	lua_pushstring(L->GetCState(), topic.toStdString().c_str());
	lua_pushstring(L->GetCState(), msg.toStdString().c_str());
	lua_pcall(L->GetCState(), 2, 1, 0);
	lua_pop(L->GetCState(), 1);
}

int CGLuaFun::Lua_CreateTcpServer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();

	LuaStack args(L);
	if (args.Count() < 1)
	{
		tblObj.SetBoolean("state", false);
		tblObj.SetString("msg", "未指定端口号");
		tblObj.SetInteger("id", -1);
		tblObj.Push(L);
		return 1;
	}
	int nPort = args[1].GetInteger();
	//ITTcpServer *pTcpServer = g_pNetworkFactory->CreateNewTcpServer(nPort);
	auto tmpRetServ = g_pNetworkFactory->CreateNewTcpServer(nPort);
	ITTcpServer *pTcpServer = std::get<0>(tmpRetServ);
	bool tRetSate = std::get<1>(tmpRetServ);
	QString tRetMsg = std::get<2>(tmpRetServ);
	qDebug() << pTcpServer << tRetSate << tRetMsg;
	bool bRet = tRetSate;
	//pTcpServer->listen(QHostAddress::Any, nPort);
	if (!bRet)
	{
		tblObj.SetBoolean("state", false);
		tblObj.SetString("msg", "指定端口监听失败，请查看端口是否已被占用!");
		tblObj.SetInteger("id", -1);
		tblObj.Push(L);
		return 1;
	}
	tblObj.SetBoolean("state", true);
	tblObj.SetString("msg", "创建Tcp网络服务成功!");
	tblObj.SetInteger("id", (int)pTcpServer);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_ConnectTcpServer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	LuaStack args(L);
	if (args.Count() < 2)
	{
		tblObj.SetBoolean("state", false);
		tblObj.SetString("msg", "未指定IP地址和端口号");
		tblObj.SetInteger("id", -1);
		tblObj.Push(L);
		return 1;
	}
	QString sName = args[3].GetString();
	QString sIp = args[1].GetString();
	if (sIp.isEmpty())
		sIp = "127.0.0.1";
	int nPort = args[2].GetInteger();
	auto tmpRetClent = g_pNetworkFactory->CreateNewTcpClient(sIp, nPort, sName);

	ITNetAgent *pNetAgent = std::get<0>(tmpRetClent);
	bool tRetSate = std::get<1>(tmpRetClent);
	QString tRetMsg = std::get<2>(tmpRetClent);
	qDebug() << pNetAgent << tRetSate << tRetMsg;
	if (!tRetSate)
	{
		tblObj.SetBoolean("state", false);
		tblObj.SetString("msg", "创建TCP客户端失败!");
		tblObj.SetInteger("id", -1);
		tblObj.Push(L);
		return 1;
	}
	tblObj.SetBoolean("state", true);
	tblObj.SetString("msg", "创建Tcp客户端成功!");
	tblObj.SetInteger("id", (int)pNetAgent);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_CloseTcpServer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	LuaStack args(L);
	do
	{
		if (args.Count() < 1)
			break;
		int serverID = args[1].GetInteger();
		if (serverID == -1 || serverID == 0)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		ITTcpServer *pServer = (ITTcpServer *)serverID;
		if (!pServer)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		g_pNetworkFactory->CloseServer(pServer);
		tblObj.SetBoolean("state", true);
		tblObj.SetString("msg", "关闭服务端成功");
		tblObj.SetInteger("id", serverID);
	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_CloseAllTcpServer(LuaState *L)
{
	g_pNetworkFactory->CloseAllTcpServer();
	return 0;
}

int CGLuaFun::Lua_CloseTcpClient(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	LuaStack args(L);
	do
	{
		if (args.Count() < 1)
			break;
		int clientID = args[1].GetInteger();
		if (clientID == -1 || clientID == 0)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		ITNetAgent *pNetAgent = (ITNetAgent *)clientID;
		if (!pNetAgent)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		g_pNetworkFactory->CloseClient(pNetAgent);
		tblObj.SetBoolean("state", true);
		tblObj.SetString("msg", "关闭客户端成功");
		tblObj.SetInteger("id", clientID);
	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_CloseAllTcpClient(LuaState *L)
{
	g_pNetworkFactory->CloseAllTcpClient();
	return 0;
}

int CGLuaFun::Lua_SendDataToServer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	LuaStack args(L);
	do
	{
		if (args.Count() < 2)
			break;
		int clientID = args[1].GetInteger();
		if (clientID == -1)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		QByteArray sData = args[2].GetString();
		ITNetAgent *pNetAgent = (ITNetAgent *)clientID;
		if (!pNetAgent)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		pNetAgent->AddToSendQ(sData);
		tblObj.SetBoolean("state", true);
		tblObj.SetString("msg", "已加入发送队列");
		tblObj.SetInteger("id", clientID);
	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_SendDataToClient(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	tblObj.SetString("data", "");
	LuaStack args(L);
	do
	{
		if (args.Count() < 2)
			break;
		int serverID = args[1].GetInteger();
		int clientID = args[2].GetInteger();
		if (serverID == -1 || serverID == 0)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		if (clientID == -1 || clientID == 0)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		QByteArray tmpSendData = args[3].GetString();
		ITTcpServer *pNetServer = (ITTcpServer *)serverID;
		if (!pNetServer)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		pNetServer->SendDataToDstClient(clientID, tmpSendData, tmpSendData.size());
		tblObj.SetBoolean("state", true);

	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_RecvDataFromServer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	tblObj.SetString("data", "");
	LuaStack args(L);
	do
	{
		if (args.Count() < 1)
			break;
		int clientID = args[1].GetInteger();
		if (clientID == -1)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		ITNetAgent *pNetAgent = (ITNetAgent *)clientID;
		if (!pNetAgent)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		QString recvData = pNetAgent->ReadNextRecvData();
		if (recvData.size() > 0)
		{
			tblObj.SetString("data", recvData.toStdString().c_str());
			tblObj.SetBoolean("state", true);
			tblObj.SetString("msg", "接收数据成功");
			tblObj.SetInteger("id", clientID);
		}
	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_SendDataToAllClient(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	tblObj.SetString("data", "");
	LuaStack args(L);
	do
	{
		if (args.Count() < 2)
			break;
		int serverID = args[1].GetInteger();
		if (serverID == -1)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		QByteArray tmpSendData = args[2].GetString();
		ITTcpServer *pNetServer = (ITTcpServer *)serverID;
		if (!pNetServer)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		pNetServer->sendDataToAllClients(tmpSendData, tmpSendData.size());
		tblObj.SetBoolean("state", true);

	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_RecvDataFromAllClient(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	tblObj.SetString("data", "");
	LuaStack args(L);
	do
	{
		if (args.Count() < 1)
			break;
		int serverID = args[1].GetInteger();
		if (serverID == -1)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		ITTcpServer *pNetServer = (ITTcpServer *)serverID;
		if (!pNetServer)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		int clientHandle = 0;
		QString recvData = pNetServer->ReadNextRecvData(clientHandle);
		if (recvData.size() > 0)
		{
			tblObj.SetInteger("id", clientHandle);
			tblObj.SetString("data", recvData.toStdString().c_str());
			tblObj.SetBoolean("state", true);
		}
	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_ClearServerRecvBuffer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	tblObj.SetString("data", "");
	LuaStack args(L);
	do
	{
		if (args.Count() < 1)
			break;
		int serverID = args[1].GetInteger();
		if (serverID == -1 || serverID == 0)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		ITTcpServer *pNetServer = (ITTcpServer *)serverID;
		if (!pNetServer)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		int clientHandle = 0;
		pNetServer->ClearRecvBuffer();
		tblObj.SetBoolean("state", true);
		tblObj.SetInteger("id", clientHandle);
		tblObj.SetString("data", "");
	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_ClearServerSendBuffer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	tblObj.SetString("data", "");
	LuaStack args(L);
	do
	{
		if (args.Count() < 1)
			break;
		int serverID = args[1].GetInteger();
		if (serverID == -1 || serverID == 0)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		ITTcpServer *pNetServer = (ITTcpServer *)serverID;
		if (!pNetServer)
		{
			tblObj.SetString("msg", "服务端ID错误");
			break;
		}
		int clientHandle = 0;
		pNetServer->ClearSendBuffer();
		tblObj.SetBoolean("state", true);
		tblObj.SetInteger("id", clientHandle);
		tblObj.SetString("data", "");
	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_ClearClientRecvBuffer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	tblObj.SetString("data", "");
	LuaStack args(L);
	do
	{
		if (args.Count() < 1)
			break;
		int clientID = args[1].GetInteger();
		if (clientID == -1)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		ITNetAgent *pNetAgent = (ITNetAgent *)clientID;
		if (!pNetAgent)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		pNetAgent->ClearRecvBuffer();
		tblObj.SetString("data", "");
	} while (0);
	tblObj.Push(L);
	return 1;
}

int CGLuaFun::Lua_ClearClientSendBuffer(LuaState *L)
{
	LuaObject tblObj(L);
	tblObj.AssignNewTable();
	tblObj.SetBoolean("state", false);
	tblObj.SetString("msg", "参数错误");
	tblObj.SetInteger("id", -1);
	tblObj.SetString("data", "");
	LuaStack args(L);
	do
	{
		if (args.Count() < 1)
			break;
		int clientID = args[1].GetInteger();
		if (clientID == -1)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		ITNetAgent *pNetAgent = (ITNetAgent *)clientID;
		if (!pNetAgent)
		{
			tblObj.SetString("msg", "客户端ID错误");
			break;
		}
		pNetAgent->ClearSendBuffer();
		tblObj.SetString("data", "");
	} while (0);
	tblObj.Push(L);
	return 1;
}

void CGLuaFun::TransVariantToLua(LuaState *L, QVariant &val, bool bTransToInt /*=true*/)
{
	if (val.type() == QVariant::Int || val.type() == QVariant::UInt)
	{
		L->PushInteger(val.toInt());
	}
	else if (val.type() == QVariant::Bool)
	{
		L->PushBoolean(val.toBool());
	}
	else if (val.type() == QVariant::Double || val.type() == QVariant::LongLong || val.type() == QVariant::ULongLong)
	{
		L->PushNumber(val.toDouble());
	}
	else if (val.type() == QVariant::String)
	{
		if (val.toString().isEmpty())
			L->PushInteger(0);
		else
		{
			if (bTransToInt)
			{
				bool bTransState = false;
				int iVal = val.toInt(&bTransState);
				if (bTransState)
				{
					L->PushInteger(iVal);
				}
				else
					L->PushString(val.toString().toStdString().c_str());
			}
			else
				L->PushString(val.toString().toStdString().c_str());
		}
	} //这个应该和int一样的
	else
	{
		L->PushInteger(0);
	}
}
