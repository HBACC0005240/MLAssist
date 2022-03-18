#include "GameLuaScriptWgt.h"
#include "GameCtrl.h"
#include "ITObjectDataMgr.h"
#include "QAESEncryption.h"
#include "UserDefDialog.h"
#include <setjmp.h>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QTableWidgetItem>
jmp_buf g_jmpPlace;
GameLuaScriptWgt::GameLuaScriptWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	QMenu *saveMenu = new QMenu;
	saveMenu->setMinimumWidth(125);
	saveMenu->addAction(tr("普通"), this, [&]()
			{ on_save_script(); });
	saveMenu->addAction(tr("加密"), this, [&]()
			{ on_save_encryptscript(); });
	ui.pushButton_save->setMenu(saveMenu);
	QStringList saveHeadList;
	saveHeadList << ("序号") << ("脚本内容");
	ui.tableWidget->setColumnCount(saveHeadList.size());
	ui.tableWidget->setHorizontalHeaderLabels(saveHeadList);
	ui.tableWidget->horizontalHeader()->setStyleSheet("font:bold;");
	//	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
	//	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	//	ui.tableWidget->horizontalHeader()->setFixedHeight(30);
	ui.tableWidget->setColumnWidth(0, 38);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_customContextMenu(const QPoint &)));
	connect(ui.tableWidget, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(onTableItemClicked(QTableWidgetItem *)));

	connect(g_pGameCtrl, SIGNAL(signal_setUiScriptDesc(const QString &)), this, SLOT(setUiScriptDesc(const QString &)));
	connect(g_pGameCtrl, SIGNAL(signal_exit()), this, SLOT(DoStopScriptThread()));
	connect(g_pGameCtrl, SIGNAL(signal_updateScriptRunLine(int)), this, SLOT(doUpdateScriptRow(int)), Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, SIGNAL(signal_loadScript(const QString &)), this, SLOT(doRunNewScript(const QString &)));
	connect(g_pGameCtrl, &GameCtrl::NotifyFillLoadScript, this, &GameLuaScriptWgt::DoLoadScript, Qt::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_stopScriptRun, this, &GameLuaScriptWgt::on_pushButton_stop_clicked);
	connect(g_pGameCtrl, &GameCtrl::NotifyConnectionState, this, &GameLuaScriptWgt::OnNotifyConnectionState, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_addOneLogMsg, this, &GameLuaScriptWgt::AddLogMsg, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_addOneScriptLogMsg, this, &GameLuaScriptWgt::AddLogMsg, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_gameIsOnline, this, &GameLuaScriptWgt::GameOnlineStateChange, Qt::ConnectionType::QueuedConnection);

	connect(this, SIGNAL(runScriptFini()), this, SLOT(doRunScriptFini()));
	connect(this, SIGNAL(runScriptSignal()), this, SLOT(on_pushButton_start_clicked()));
	connect(this, SIGNAL(switchScript(const QString &)), this, SLOT(doSwitchScript(const QString &)));
	connect(&ITObjectDataMgr::getInstance(), &ITObjectDataMgr::signal_mqttMsg, this, &GameLuaScriptWgt::DealMqttTopicData, Qt::ConnectionType::QueuedConnection);
	ui.plainTextEdit->setMaximumBlockCount(10);
	ui.textEdit_log->setMaximumBlockCount(m_scriptLogMaxLine);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(OnAutoRestart()));
	timer->start(1000);
	//ui.textEdit_log->setDocument(ui.plainTextEdit->document());
	initScriptSystem();
	//lua_State* pLuaState = luaL_newstate();
	//luaL_openlibs(pLuaState);
	//RegisterFun(pLuaState);
	//m_luaFun.setLuaState(pLuaState);
	//luabridge::setGlobal(pLuaState,&m_luaFun, "cg");//注册test_lua对象到lua
}

GameLuaScriptWgt::~GameLuaScriptWgt()
{
}
void GameLuaScriptWgt::on_customContextMenu(const QPoint &pos)
{
	QTableWidgetItem *tempitem = ui.tableWidget->itemAt(pos);
	if (tempitem != NULL)
	{
		m_currentRow = ui.tableWidget->row(tempitem); //
		QMenu menu;
		menu.addAction(QString("跳到(暂时不能用)"), this, SLOT(gotoScriptRow()));
		menu.addAction(QString("开始跟踪"), this, SLOT(BeginTraceScriptRun()));

		menu.exec(QCursor::pos());
	}
}

void GameLuaScriptWgt::gotoScriptRow()
{
	UserDefDialog dlg;
	dlg.setLabelText("行：");
	dlg.setWindowTitle("跳转到指定行");
	dlg.exec();
	int nRow = dlg.getVal().toInt();
	ui.tableWidget->setCurrentCell(nRow, 0);
	//LuaObject objGlobal = (*m_pLuaState)->GetGlobals(); //注册全局函数
	//lua_Debug *pDebug;
	//(*m_pLuaState)->GetGlobal();
	//lua_getglobal(L, LUA_DEBUGGER_NAME);
	//if (!lua_istable(L, -1))
	//{
	//	const char *err_msg = "[C Module Error]:call_lua_function Get LUA_DEBUGGER_NAME error.\n";
	//	print_to_vscode(L, err_msg, 2);
	//	return -1;
	//}

	//lua_getfield(L, -1, lua_function_name);
	//if (!lua_isfunction(L, -1))
	//{
	//	char err_msg[100];
	//	snprintf(err_msg, sizeof(err_msg), "[C Module Error]:call_lua_function Get lua function '%s' error\n.", lua_function_name);
	//	print_to_vscode(L, err_msg, 2);
	//	return -1;
	//}

	//push_args(L, args...);
	//int err_code = lua_pcall(L, sizeof...(args), retCount, 0);
	//if (err_code)
	//{
	//	char err_msg[1024];
	//	const char *lua_error = lua_tostring(L, -1);
	//	snprintf(err_msg, sizeof(err_msg), "[C Module Error]:call_lua_function Call '%s' error. ErrorCode: %d, ErrorMessage: %s.\n", lua_function_name, err_code, lua_error);
	//	print_to_vscode(L, err_msg, 2);
	//	lua_pop(L, 1);
	//	return err_code;
	//}
}

void GameLuaScriptWgt::onUpdateUI()
{
	/*if (FZParseScript::getInstance().GetGameScriptCtrlStatus() == FZParseScript::SCRIPT_CTRL_PAUSE)
	{
		ui.pushButton_pause->setText("继续");
	}
	else if (FZParseScript::getInstance().GetGameScriptCtrlStatus() == FZParseScript::SCRIPT_CTRL_RUN)
	{
		ui.pushButton_pause->setText("暂停");
		ui.pushButton_start->setEnabled(false);
	}
	else if (FZParseScript::getInstance().GetGameScriptCtrlStatus() == FZParseScript::SCRIPT_CTRL_STOP)
	{
		ui.pushButton_pause->setText("暂停");
		ui.pushButton_start->setEnabled(true);
	}*/
}

void GameLuaScriptWgt::initTableWidget()
{
	int rowCounts = ui.tableWidget->rowCount();
	int colCounts = ui.tableWidget->columnCount();
	int nWidgetCol = -1;
	for (int i = 0; i < rowCounts - 1; i++)
	{
		for (int j = 0; j < colCounts; j++)
		{
			QTableWidgetItem *tableitem = ui.tableWidget->item(i, j);
			if (tableitem != NULL)
				delete tableitem;
		}
		ui.tableWidget->removeRow(0);
	}
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(m_scriptLineDataList.size());
	for (int i = 0; i < m_scriptLineDataList.size(); ++i)
	{
		QTableWidgetItem *pItem = new QTableWidgetItem(QString::number(i + 1));
		ui.tableWidget->setItem(i, 0, pItem);
		pItem = new QTableWidgetItem(m_scriptLineDataList.at(i));
		ui.tableWidget->setItem(i, 1, pItem);
	}
}
//变量加进去后，貌似搜索路径有了，但是不识别中文 英文测试ok
void GameLuaScriptWgt::LuaAddPath(lua_State *ls, QString pathName, QString addVal)
{
	lua_getglobal(ls, "package");
	lua_getfield(ls, -1, pathName.toStdString().c_str());
	auto oldPath = lua_tostring(ls, -1);
	QString sLuaModulePath = oldPath;
	sLuaModulePath += ";";
	sLuaModulePath += addVal;
	lua_pop(ls, 1);
	lua_pushstring(ls, sLuaModulePath.toUtf8()); //local8bit StdString都试过了
	lua_setfield(ls, -2, pathName.toStdString().c_str());
	lua_pop(ls, 1);
}

void GameLuaScriptWgt::AddScriptLogMsg(QPlainTextEdit *pEdit, const QString &sMsg)
{
	if (!pEdit)
		return;

	//if (m_scriptLogMaxLine > 0)
	//{
	//	while (pEdit->document()->lineCount() > m_scriptLogMaxLine)
	//	{
	//		QTextCursor txtcur = pEdit->textCursor();
	//		txtcur.movePosition(QTextCursor::Start);
	//		txtcur.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
	//		txtcur.removeSelectedText();
	//	}
	//}
	//pEdit->moveCursor(QTextCursor::End);
	pEdit->appendPlainText(sMsg);
}
//本来想加一个自定义的loader，初步有问题，先放下
void GameLuaScriptWgt::AddMyLuaLoader(lua_State *pState)
{
	lua_getglobal(pState, "package"); /* L: package，获得package，在栈定 */
	//lua_getfield(pState, -1, "loaded"); /* L: package loaded,获得表，在栈顶*/
	lua_pushnil(pState);			  /* L: package loaded nil */
	while (0 != lua_next(pState, -2)) /* L: package loaded, key, value，上一个栈顶为nil，弹出nil，获得表的第一个key和value，压入栈 */
	{
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
		//		lua_settable(m_pLuaState, -5); /*pakage,loaded(table),key,value,  将栈顶两个元素作为key和value设置给table，弹出栈顶两个元素*/
		//	}
		//}
		lua_pop(pState, 1); /*pakage,loaded(table),key  弹出value,留下key作为下一个next*/
	}
	lua_pop(pState, 2); /*栈平衡*/
}

int GameLuaScriptWgt::MyLoader(lua_State *pState)
{
	QString module = lua_tostring(pState, 1);
	module += ".lua";
	QString fullPath = module;
	QFile file(fullPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString err = "\n\tError - MyLoader could not find ";
		err += module;
		err += ".";
		lua_pushstring(pState, err.toStdString().c_str());
		return 1;
	}
	int size = file.size();
	if (size > 0)
	{
		QString scriptData;
		//先处理下脚本 空行  还有注释
		QTextCodec *tc = QTextCodec::codecForName("GBK");
		QByteArray firstLine = file.readLine();
		while (!file.atEnd())
		{
			QByteArray line = file.readLine();
			scriptData.append(line);
		}
		file.close();
		luaL_loadbuffer(pState, scriptData.toStdString().c_str(), scriptData.size(), fullPath.toStdString().c_str());
	}
	else
	{
		QString err = "\n\tError - MyLoader could not find ";
		err += module;
		err += ".";
		lua_pushstring(pState, err.toStdString().c_str());
	}
	return 1;
}

bool GameLuaScriptWgt::overrideLuaRequire(LuaStateOwner *pOwner)
{
	//主要就是把原来require的参数，通过自定义的utf8ToGbk函数 从utf8转为gbk
	//使lua能找到此路径，然后后续还是调用的lua自己的代码
	//这里没有改lua的加载机制，如果需要加密脚本，可以自定义lua装载器
	QString sOverLoadRequire =
			"local org_require = require\n\
		 local _require = function(var)\n\
				org_require(utf8ToGbk(var))\n\
		 end\n\
		 require = _require ";
	int runState = (*pOwner)->DoString(sOverLoadRequire.toStdString().c_str());
	if (runState != 0)
	{
		qDebug() << "overLoad Require Ero" << runState;
		int type = lua_type((*pOwner)->GetCState(), -1);
		if (type == 4)
		{
			QString error = lua_tostring((*pOwner)->GetCState(), -1);
			qDebug() << "Run Ero String" << error;
		}
		return false;
	}
	return true;
}

QString GameLuaScriptWgt::ParseScriptData(const QString &sPath)
{
	if (!QFile::exists(sPath))
		return "";
	QFile file(sPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return "";
	QString sData;
	//先处理下脚本 空行  还有注释
	QByteArray firstLine = file.readLine();
	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		sData.append(line);
	}
	file.close();
	return sData;
}

void GameLuaScriptWgt::initScriptSystem()
{
	m_pLuaState = new LuaStateOwner(true);
	LuaObject objGlobal = (*m_pLuaState)->GetGlobals(); //注册全局函数
	auto ls = objGlobal.GetCState();
	//require默认不识别utf8，这里把参数转为gbk
	//overrideLuaRequire(m_pLuaState);
	//AddMyLuaLoader(ls);
	QString sPath = QApplication::applicationDirPath() + "/脚本/?.lua";
	LuaAddPath(ls, "path", sPath.replace("/", "\\"));
	//CMeLua* MeLua = new CMeLua;//注册对象
	objGlobal.Register("测试", m_luaFun, &CGLuaFun::Lua_Test);
	objGlobal.Register("字符串转换", m_luaFun, &CGLuaFun::Lua_Translate);
	objGlobal.Register("注册函数", m_luaFun, &CGLuaFun::Lua_RegisterLuaFun);
	objGlobal.Register("utf8ToGbk", m_luaFun, &CGLuaFun::Lua_Translate);
	objGlobal.Register("取当前坐标", m_luaFun, &CGLuaFun::GetMapPos);
	objGlobal.Register("取当前地图名", m_luaFun, &CGLuaFun::Lua_GetMapName);

	objGlobal.Register("取当前地图编号", m_luaFun, &CGLuaFun::Lua_GetMapNumber);
	objGlobal.Register("取当前地图数据", m_luaFun, &CGLuaFun::Lua_GetMapData);
	objGlobal.Register("取当前楼层", m_luaFun, &CGLuaFun::Lua_GetMapFloorNumberFromName);
	objGlobal.Register("取字符串中数字", m_luaFun, &CGLuaFun::Lua_GetNumberFromName);
	objGlobal.Register("取程序路径", m_luaFun, &CGLuaFun::Lua_GetAppRunPath);
	objGlobal.Register("取物品数量", m_luaFun, &CGLuaFun::Lua_GetItemCount);
	objGlobal.Register("取物品叠加数量", m_luaFun, &CGLuaFun::Lua_GetItemPileCount);
	objGlobal.Register("取银行物品数量", m_luaFun, &CGLuaFun::Lua_GetBankItemCount);
	objGlobal.Register("取银行物品叠加数量", m_luaFun, &CGLuaFun::Lua_GetBankItemPileCount);
	objGlobal.Register("取包裹物品数量", m_luaFun, &CGLuaFun::Lua_GetItemCount);
	objGlobal.Register("取包裹物品叠加数量", m_luaFun, &CGLuaFun::Lua_GetItemPileCount);
	objGlobal.Register("取所有物品数量", m_luaFun, &CGLuaFun::Lua_GetAllItemCount);
	objGlobal.Register("取所有物品叠加数量", m_luaFun, &CGLuaFun::Lua_GetAllItemPileCount);
	objGlobal.Register("取队伍人数", m_luaFun, &CGLuaFun::Lua_GetTeammatesCount);
	objGlobal.Register("取包裹空格", m_luaFun, &CGLuaFun::Lua_GetItemNotUseSpaceCount);
	objGlobal.Register("取已用格", m_luaFun, &CGLuaFun::Lua_GetBagUsedItemCount);
	objGlobal.Register("取好友名片", m_luaFun, &CGLuaFun::Lua_GetFriendCard);
	objGlobal.Register("取脚本界面数据", m_luaFun, &CGLuaFun::Lua_GetScriptUISetData);
	objGlobal.Register("取队伍宠物平均等级", m_luaFun, &CGLuaFun::Lua_GetTeamPetAvgLv);
	objGlobal.Register("取队伍宠物等级", m_luaFun, &CGLuaFun::Lua_GetTeamPetLv);
	objGlobal.Register("取迷宫出入口", m_luaFun, &CGLuaFun::Lua_GetAllMazeWarpList);
	objGlobal.Register("取迷宫远近坐标", m_luaFun, &CGLuaFun::Lua_GetNextMazeWarp);
	objGlobal.Register("取搜索路径", m_luaFun, &CGLuaFun::Lua_FindRandomSearchPath);
	objGlobal.Register("取周围信息", m_luaFun, &CGLuaFun::Lua_GetMapUnits);
	objGlobal.Register("取队长等待坐标", m_luaFun, &CGLuaFun::Lua_GetUnmannedMapUnitPosList);

	objGlobal.Register("取周围空地", m_luaFun, &CGLuaFun::Lua_GetRandomSpace);
	objGlobal.Register("查周围信息", m_luaFun, &CGLuaFun::Lua_FindMapUnit);
	objGlobal.Register("目标是否可达", m_luaFun, &CGLuaFun::Lua_IsReachableTarget);
	objGlobal.Register("设置个人简介", m_luaFun, &CGLuaFun::Lua_SetPlayerInfo);

	objGlobal.Register("世界状态", m_luaFun, &CGLuaFun::Lua_GetWorldStatus);

	objGlobal.Register("游戏状态", m_luaFun, &CGLuaFun::Lua_GetGameStatus);
	objGlobal.Register("游戏时间", m_luaFun, &CGLuaFun::Lua_GetSysTimeEx);

	objGlobal.Register("游戏窗口状态", m_luaFun, &CGLuaFun::Lua_GetConnectGameWndStatus);
	objGlobal.Register("打开游戏窗口", m_luaFun, &CGLuaFun::Lua_RunGameWnd);

	//objGlobal.Register("取银行空格", m_luaFun, &CGLuaFun::Lua_GetItemNotUseSpaceCount);//不知道银行格数 除非80

	objGlobal.Register("是否队长", m_luaFun, &CGLuaFun::Lua_IsTeamLeader);
	objGlobal.Register("加入队伍", m_luaFun, &CGLuaFun::Lua_AddTeammate);
	objGlobal.Register("离开队伍", m_luaFun, &CGLuaFun::Lua_LeaveTeammate);

	objGlobal.Register("人物动作", m_luaFun, &CGLuaFun::Lua_DoCharacterAction);
	objGlobal.Register("删除技能", m_luaFun, &CGLuaFun::Lua_DeleteSkill);
	objGlobal.Register("技能是否有效", m_luaFun, &CGLuaFun::Lua_IsSkillValid);
	objGlobal.Register("取合成信息", m_luaFun, &CGLuaFun::Lua_GetCraftInfo);
	objGlobal.Register("取所有合成信息", m_luaFun, &CGLuaFun::Lua_GetCraftsInfo);
	objGlobal.Register("取合成状态", m_luaFun, &CGLuaFun::Lua_GetCraftStatus);

	objGlobal.Register("等待入队", m_luaFun, &CGLuaFun::Lua_WaitTeammatesEx);
	objGlobal.Register("等待指定入队", m_luaFun, &CGLuaFun::Lua_WaitTeammates);

	objGlobal.Register("自动寻路", m_luaFun, &CGLuaFun::Lua_AutoMove);
	objGlobal.Register("移动", m_luaFun, &CGLuaFun::Lua_AutoMove);
	objGlobal.Register("移动一格", m_luaFun, &CGLuaFun::Lua_MoveGo);
	objGlobal.Register("moveGo", m_luaFun, &CGLuaFun::Lua_MoveGo);
	objGlobal.Register("搜索地图", m_luaFun, &CGLuaFun::Lua_SearchMap);
	objGlobal.Register("搜索范围迷宫", m_luaFun, &CGLuaFun::Lua_FindToRandomEntry);
	objGlobal.Register("移动到目标附近", m_luaFun, &CGLuaFun::Lua_MoveToNpcNear);
	objGlobal.Register("下载地图", m_luaFun, &CGLuaFun::Lua_DownloadMap);

	objGlobal.Register("停止脚本", m_luaFun, &CGLuaFun::Lua_StopScript);
	objGlobal.Register("加载脚本", m_luaFun, &CGLuaFun::Lua_LoadScript);
	objGlobal.Register("切换脚本", m_luaFun, &CGLuaFun::Lua_SwitchScript);
	objGlobal.Register("导入脚本", m_luaFun, &CGLuaFun::Lua_ImportScript);
	objGlobal.Register("执行脚本", m_luaFun, &CGLuaFun::Lua_ExecScript);

	objGlobal.Register("设置脚本简介", m_luaFun, &CGLuaFun::Lua_SetUIScriptDesc);
	objGlobal.Register("开关", m_luaFun, &CGLuaFun::Lua_SetCharacterSwitch);

	objGlobal.Register("清除系统消息", m_luaFun, &CGLuaFun::Lua_ClearSysCue);
	objGlobal.Register("系统消息", m_luaFun, &CGLuaFun::Lua_GetSysChatMsg);
	objGlobal.Register("最新系统消息", m_luaFun, &CGLuaFun::Lua_GetLastSysChatMsg);
	objGlobal.Register("聊天", m_luaFun, &CGLuaFun::Lua_GetAllChatMsg);
	objGlobal.Register("聊天信息", m_luaFun, &CGLuaFun::Lua_GetDetailAllChatMsg); //带unit
	objGlobal.Register("最新聊天", m_luaFun, &CGLuaFun::Lua_GetLastChatMsg);
	objGlobal.Register("等待系统消息", m_luaFun, &CGLuaFun::Lua_WaitSysMsg);
	objGlobal.Register("等待聊天消息", m_luaFun, &CGLuaFun::Lua_WaitChatMsg);
	objGlobal.Register("等待最新消息", m_luaFun, &CGLuaFun::Lua_WaitSysAndChatMsg);
	objGlobal.Register("等待订阅消息", m_luaFun, &CGLuaFun::Lua_WaitSubscribeMsg);
	objGlobal.Register("订阅消息", m_luaFun, &CGLuaFun::Lua_SubscribeMsg);
	objGlobal.Register("发布消息", m_luaFun, &CGLuaFun::Lua_PublishMsg);
	objGlobal.Register("已接收订阅消息", m_luaFun, &CGLuaFun::Lua_GetTopicMsgList);
	objGlobal.Register("已接收最新订阅消息", m_luaFun, &CGLuaFun::Lua_GetLastTopicMsg);

	objGlobal.Register("debugMsg", m_luaFun, &CGLuaFun::Lua_DebugMessage);
	objGlobal.Register("用户输入框", m_luaFun, &CGLuaFun::Lua_UserDefDialog);
	objGlobal.Register("用户下拉框", m_luaFun, &CGLuaFun::Lua_UserDefComboBoxDlg);
	objGlobal.Register("读取配置", m_luaFun, &CGLuaFun::Lua_LoadUserConfig);
	objGlobal.Register("保存配置", m_luaFun, &CGLuaFun::Lua_SaveUserConfig);

	objGlobal.Register("获取仓库信息", m_luaFun, &CGLuaFun::Lua_GatherAccountInfo);
	objGlobal.Register("保存仓库信息", m_luaFun, &CGLuaFun::Lua_SaveGatherAccountInfos);
	objGlobal.Register("登出服务器", m_luaFun, &CGLuaFun::Lua_LogoutServer);
	objGlobal.Register("获取游戏子账户", m_luaFun, &CGLuaFun::Lua_GetAccountGids);
	objGlobal.Register("保存图鉴信息", m_luaFun, &CGLuaFun::Lua_SavePetPictorialBookToHtml);
	objGlobal.Register("设置登录子账号", m_luaFun, &CGLuaFun::Lua_SetUIAccountGid);
	objGlobal.Register("设置登录角色", m_luaFun, &CGLuaFun::Lua_SetUICharacter);
	objGlobal.Register("设置创建角色信息", m_luaFun, &CGLuaFun::Lua_SetUICreateCharacterInfo);
	objGlobal.Register("登录游戏", m_luaFun, &CGLuaFun::Lua_LoginGame);
	objGlobal.Register("重置登录状态", m_luaFun, &CGLuaFun::Lua_ResetLoginGameConnectState);
	objGlobal.Register("取登录状态", m_luaFun, &CGLuaFun::Lua_GetLoginGameConnectState);
	objGlobal.Register("回到选择线路", m_luaFun, &CGLuaFun::Lua_BackSelectGameLine);
	objGlobal.Register("切换登录信息", m_luaFun, &CGLuaFun::Lua_SwitchLoginData);
	objGlobal.Register("获取随机名称", m_luaFun, &CGLuaFun::Lua_CreateRandomRoleName);

	objGlobal.Register("使用物品", m_luaFun, &CGLuaFun::Lua_UseItem);
	objGlobal.Register("整理包裹", m_luaFun, &CGLuaFun::Lua_SortBagItems);
	objGlobal.Register("整理银行", m_luaFun, &CGLuaFun::Lua_SortBankItems);
	objGlobal.Register("交易", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	objGlobal.Register("等待交易", m_luaFun, &CGLuaFun::Lua_WaitTrade);

	objGlobal.Register("发起交易", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	objGlobal.Register("等待交易对话框", m_luaFun, &CGLuaFun::Lua_WaitTradeDlg);
	objGlobal.Register("添加交易信息", m_luaFun, &CGLuaFun::Lua_TradeAddStuffs);
	objGlobal.Register("等待对方交易信息", m_luaFun, &CGLuaFun::Lua_WaitTrade);

	objGlobal.Register("交易金币", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	objGlobal.Register("交易物品", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	objGlobal.Register("交易宠物", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	objGlobal.Register("扔", m_luaFun, &CGLuaFun::Lua_ThrowItemName);
	objGlobal.Register("丢", m_luaFun, &CGLuaFun::Lua_ThrowItemName);
	objGlobal.Register("捡", m_luaFun, &CGLuaFun::Lua_PickupItem);
	objGlobal.Register("扔叠加物", m_luaFun, &CGLuaFun::Lua_ThrowNoFullItemName);
	objGlobal.Register("叠", m_luaFun, &CGLuaFun::Lua_PileItem);
	objGlobal.Register("买", m_luaFun, &CGLuaFun::Lua_Shopping);
	objGlobal.Register("解析购买列表", m_luaFun, &CGLuaFun::Lua_ParseBuyStoreMsg);
	objGlobal.Register("卖", m_luaFun, &CGLuaFun::Lua_Sale);
	objGlobal.Register("出售", m_luaFun, &CGLuaFun::Lua_Sale);
	objGlobal.Register("saleEx", m_luaFun, &CGLuaFun::Lua_SaleEx);
	objGlobal.Register("商店鉴定", m_luaFun, &CGLuaFun::Lua_IdentifyItem);
	objGlobal.Register("saleEx", m_luaFun, &CGLuaFun::Lua_IdentifyItemEx);
	//objGlobal.Register("存钱", m_luaFun, &CGLuaFun::Lua_DepositGold);
	//objGlobal.Register("取钱", m_luaFun, &CGLuaFun::Lua_WithdrawGold);
	objGlobal.Register("扔钱", m_luaFun, &CGLuaFun::Lua_DropGold);
	/*objGlobal.Register("存物", m_luaFun, &CGLuaFun::Lua_SaveToBankOnce);
	objGlobal.Register("全存", m_luaFun, &CGLuaFun::Lua_SaveToBankAll);
	objGlobal.Register("全取", m_luaFun, &CGLuaFun::Lua_WithdrawAllItem);
	objGlobal.Register("取物", m_luaFun, &CGLuaFun::Lua_WithdrawItem);*/
	objGlobal.Register("扔宠", m_luaFun, &CGLuaFun::Lua_DropPet);
	/*objGlobal.Register("存宠", m_luaFun, &CGLuaFun::Lua_DepositPet);
	objGlobal.Register("取宠", m_luaFun, &CGLuaFun::Lua_WithdrawPet);*/
	objGlobal.Register("银行", m_luaFun, &CGLuaFun::Lua_BankOperation);
	objGlobal.Register("耐久", m_luaFun, &CGLuaFun::Lua_GetItemDurability);
	objGlobal.Register("交换物品", m_luaFun, &CGLuaFun::Lua_SwitchItem);
	objGlobal.Register("移动物品", m_luaFun, &CGLuaFun::Lua_SwitchItem);
	objGlobal.Register("装备物品", m_luaFun, &CGLuaFun::Lua_EquipItem);
	objGlobal.Register("装备代码物品", m_luaFun, &CGLuaFun::Lua_EquipItemEx);
	objGlobal.Register("取下装备", m_luaFun, &CGLuaFun::Lua_UnEquipItem);
	objGlobal.Register("取下代码装备", m_luaFun, &CGLuaFun::Lua_UnEquipItemEx);
	objGlobal.Register("开始遇敌", m_luaFun, &CGLuaFun::Lua_BeginAutoAction);
	objGlobal.Register("停止遇敌", m_luaFun, &CGLuaFun::Lua_EndAutoAction);
	objGlobal.Register("等待", m_luaFun, &CGLuaFun::Lua_WaitTime);
	objGlobal.Register("回城", m_luaFun, &CGLuaFun::Lua_TownPortalScroll);
	objGlobal.Register("转向", m_luaFun, &CGLuaFun::Lua_TurnAbout);
	objGlobal.Register("面向", m_luaFun, &CGLuaFun::Lua_TurnAboutEx2);
	objGlobal.Register("转向坐标", m_luaFun, &CGLuaFun::Lua_TurnAboutEx);
	objGlobal.Register("转坐标方向", m_luaFun, &CGLuaFun::Lua_TurnAboutPointDir);
	objGlobal.Register("对话", m_luaFun, &CGLuaFun::Lua_TalkNpc);
	objGlobal.Register("对话选择", m_luaFun, &CGLuaFun::Lua_Npc);
	objGlobal.Register("对话选是", m_luaFun, &CGLuaFun::Lua_TalkNpcSelectYes);
	objGlobal.Register("对话坐标选是", m_luaFun, &CGLuaFun::Lua_TalkNpcPosSelectYes);
	objGlobal.Register("对话选否", m_luaFun, &CGLuaFun::Lua_TalkNpcSelectNo);
	objGlobal.Register("对话坐标选否", m_luaFun, &CGLuaFun::Lua_TalkNpcPosSelectNo);
	objGlobal.Register("菜单选择", m_luaFun, &CGLuaFun::Lua_PlayerMenuSelect);
	objGlobal.Register("菜单项选择", m_luaFun, &CGLuaFun::Lua_UnitMenuSelect);
	objGlobal.Register("回复", m_luaFun, &CGLuaFun::Lua_Renew);
	objGlobal.Register("renew", m_luaFun, &CGLuaFun::Lua_Renew);
	objGlobal.Register("人物", m_luaFun, &CGLuaFun::Lua_GetPlayerData);
	objGlobal.Register("人物信息", m_luaFun, &CGLuaFun::Lua_GetPlayerAllData);
	objGlobal.Register("下级称号数据", m_luaFun, &CGLuaFun::Lua_GetNextTitleData);
	objGlobal.Register("宠物信息", m_luaFun, &CGLuaFun::Lua_GetPetData);
	objGlobal.Register("物品信息", m_luaFun, &CGLuaFun::Lua_GetAllItemData);
	objGlobal.Register("全部宠物信息", m_luaFun, &CGLuaFun::Lua_GetAllPetData);
	objGlobal.Register("装备信息", m_luaFun, &CGLuaFun::Lua_GetPlayereEquipData);
	objGlobal.Register("装备耐久", m_luaFun, &CGLuaFun::Lua_ParseEquipData);
	objGlobal.Register("宠物", m_luaFun, &CGLuaFun::Lua_GetBattlePetData);
	objGlobal.Register("队伍", m_luaFun, &CGLuaFun::Lua_GetTeamData);
	objGlobal.Register("队伍信息", m_luaFun, &CGLuaFun::Lua_GetAllTeammateData);
	objGlobal.Register("宠物更改", m_luaFun, &CGLuaFun::Lua_SetPetData);
	objGlobal.Register("等待服务器返回", m_luaFun, &CGLuaFun::Lua_WaitRecvHead);
	objGlobal.Register("等待菜单返回", m_luaFun, &CGLuaFun::Lua_WaitRecvPlayerMenu);
	objGlobal.Register("等待菜单项返回", m_luaFun, &CGLuaFun::Lua_WaitRecvPlayerMenuUnit);
	objGlobal.Register("等待回补", m_luaFun, &CGLuaFun::Lua_WaitSupplyFini);
	objGlobal.Register("等待空闲", m_luaFun, &CGLuaFun::Lua_WaitNormal);
	objGlobal.Register("等待战斗结束", m_luaFun, &CGLuaFun::Lua_WaitBattleEnd);
	objGlobal.Register("等待按键返回", m_luaFun, &CGLuaFun::Lua_WaitRecvGameWndKeyDown);
	objGlobal.Register("等待战斗返回", m_luaFun, &CGLuaFun::Lua_WaitRecvBattleAction);
	objGlobal.Register("nop", m_luaFun, &CGLuaFun::Lua_WaitNormal);
	objGlobal.Register("是否战斗中", m_luaFun, &CGLuaFun::Lua_IsInBattle);
	objGlobal.Register("是否空闲中", m_luaFun, &CGLuaFun::Lua_IsInNormalState);
	objGlobal.Register("设置", m_luaFun, &CGLuaFun::Lua_SysConfig);
	objGlobal.Register("等待到指定地图", m_luaFun, &CGLuaFun::Lua_Nowhile);
	objGlobal.Register("调试", m_luaFun, &CGLuaFun::Lua_DebugMessage);
	objGlobal.Register("日志", m_luaFun, &CGLuaFun::Lua_LogMessage);
	objGlobal.Register("log", m_luaFun, &CGLuaFun::Lua_ScriptLogMessage);
	objGlobal.Register("脚本日志", m_luaFun, &CGLuaFun::Lua_ScriptLogMessage);
	objGlobal.Register("喊话", m_luaFun, &CGLuaFun::Lua_Chat);
	objGlobal.Register("工作", m_luaFun, &CGLuaFun::Lua_Work);
	objGlobal.Register("等待工作返回", m_luaFun, &CGLuaFun::Lua_WaitRecvWorkResult);
	objGlobal.Register("合成", m_luaFun, &CGLuaFun::Lua_AllCompound);
	objGlobal.Register("AllCompound", m_luaFun, &CGLuaFun::Lua_AllCompound);
	objGlobal.Register("压矿", m_luaFun, &CGLuaFun::Lua_Exchange);
	objGlobal.Register("穿墙", m_luaFun, &CGLuaFun::Lua_ThroughWall);
	objGlobal.Register("切图", m_luaFun, &CGLuaFun::Lua_ThroughWall);
	objGlobal.Register("方向穿墙", m_luaFun, &CGLuaFun::Lua_ThroughWallEx);
	objGlobal.Register("是否目标附近", m_luaFun, &CGLuaFun::Lua_IsNearTarget);
	objGlobal.Register("自动迷宫", m_luaFun, &CGLuaFun::Lua_AutoWalkMaze);
	objGlobal.Register("自动穿越迷宫", m_luaFun, &CGLuaFun::Lua_AutoWalkRandomMaze);

	objGlobal.Register("查询数据", m_luaFun, &CGLuaFun::Lua_SelectGidData);

	objGlobal.Register("GetMapName", m_luaFun, &CGLuaFun::Lua_GetMapName);
	objGlobal.Register("GetGameStatus", m_luaFun, &CGLuaFun::Lua_GetGameStatus);
	objGlobal.Register("GetWorldStatus", m_luaFun, &CGLuaFun::Lua_GetWorldStatus);
	objGlobal.Register("GetSysTime", m_luaFun, &CGLuaFun::Lua_GetSysTimeEx);
	objGlobal.Register("GetBGMIndex", m_luaFun, &CGLuaFun::Lua_GetBGMIndex);
	objGlobal.Register("GetPlayerInfo", m_luaFun, &CGLuaFun::Lua_GetPlayerAllData);
	objGlobal.Register("GetPetsInfo", m_luaFun, &CGLuaFun::Lua_GetPetData);
	objGlobal.Register("GetMapPos", m_luaFun, &CGLuaFun::GetMapPos);
	objGlobal.Register("SetPlayerFlagEnabled", m_luaFun, &CGLuaFun::Lua_SetPlayerFlagEnabled);
	objGlobal.Register("IsPlayerFlagEnabled", m_luaFun, &CGLuaFun::Lua_IsPlayerFlagEnabled);
	objGlobal.Register("IsSkillValid", m_luaFun, &CGLuaFun::Lua_IsSkillValid);
	objGlobal.Register("GetSkillInfo", m_luaFun, &CGLuaFun::Lua_GetSkillInfo);
	objGlobal.Register("GetSkillsInfo", m_luaFun, &CGLuaFun::Lua_GetSkillsInfo);
	objGlobal.Register("GetSubSkillInfo", m_luaFun, &CGLuaFun::Lua_GetSubSkillInfo);
	objGlobal.Register("GetSubSkillsInfo", m_luaFun, &CGLuaFun::Lua_GetSubSkillsInfo);
	objGlobal.Register("IsPetValid", m_luaFun, &CGLuaFun::Lua_IsPetValid);
	objGlobal.Register("GetPetInfo", m_luaFun, &CGLuaFun::Lua_GetPetInfo);
	objGlobal.Register("GetBankPetsInfo", m_luaFun, &CGLuaFun::Lua_GetBankPetsInfo);
	objGlobal.Register("IsPetSkillValid", m_luaFun, &CGLuaFun::Lua_IsPetSkillValid);
	objGlobal.Register("GetPetSkillInfo", m_luaFun, &CGLuaFun::Lua_GetPetSkillInfo);
	objGlobal.Register("GetPetSkillsInfo", m_luaFun, &CGLuaFun::Lua_GetPetSkillsInfo);
	objGlobal.Register("GetMapIndex", m_luaFun, &CGLuaFun::Lua_GetMapIndex);
	objGlobal.Register("GetMoveSpeed", m_luaFun, &CGLuaFun::Lua_GetMoveSpeed);
	objGlobal.Register("GetMouseXY", m_luaFun, &CGLuaFun::Lua_GetMouseXY);
	objGlobal.Register("GetMouseOrientation", m_luaFun, &CGLuaFun::Lua_GetMouseOrientation);
	objGlobal.Register("WalkTo", m_luaFun, &CGLuaFun::Lua_WalkTo);
	objGlobal.Register("TurnTo", m_luaFun, &CGLuaFun::Lua_TurnTo);
	objGlobal.Register("SetMoveSpeed", m_luaFun, &CGLuaFun::Lua_SetMoveSpeed);
	objGlobal.Register("ForceMove", m_luaFun, &CGLuaFun::Lua_ForceMove);
	objGlobal.Register("ForceMoveTo", m_luaFun, &CGLuaFun::Lua_ForceMoveTo);
	objGlobal.Register("IsMapCellPassable", m_luaFun, &CGLuaFun::Lua_IsMapCellPassable);
	objGlobal.Register("LogBack", m_luaFun, &CGLuaFun::Lua_LogBack);
	objGlobal.Register("LogOut", m_luaFun, &CGLuaFun::Lua_LogOut);
	objGlobal.Register("BackSelectServer", m_luaFun, &CGLuaFun::Lua_BackSelectServer);
	objGlobal.Register("SayWords", m_luaFun, &CGLuaFun::Lua_Chat);
	objGlobal.Register("ChangeNickName", m_luaFun, &CGLuaFun::Lua_ChangeNickName);
	objGlobal.Register("ChangeTitleName", m_luaFun, &CGLuaFun::Lua_ChangeTitleName);
	objGlobal.Register("ChangePersDesc", m_luaFun, &CGLuaFun::Lua_ChangePersDesc);
	objGlobal.Register("ChangePetName", m_luaFun, &CGLuaFun::Lua_ChangePetName);
	objGlobal.Register("GetCardsInfo", m_luaFun, &CGLuaFun::Lua_GetCardsInfo);
	objGlobal.Register("GetCardsRecvMsg", m_luaFun, &CGLuaFun::Lua_GetCardsRecvMsg);
	objGlobal.Register("GetPicBooksInfo", m_luaFun, &CGLuaFun::Lua_GetPicBooksInfo);
	objGlobal.Register("IsItemValid", m_luaFun, &CGLuaFun::Lua_IsItemValid);
	objGlobal.Register("GetItemInfo", m_luaFun, &CGLuaFun::Lua_GetItemInfo);
	objGlobal.Register("GetItemsInfo", m_luaFun, &CGLuaFun::Lua_GetItemsInfo);
	objGlobal.Register("GetBankItemsInfo", m_luaFun, &CGLuaFun::Lua_GetBankItemsInfo);
	objGlobal.Register("GetBankGold", m_luaFun, &CGLuaFun::Lua_GetBankGold);
	objGlobal.Register("UseItem", m_luaFun, &CGLuaFun::Lua_UseItem);
	objGlobal.Register("MoveItem", m_luaFun, &CGLuaFun::Lua_MoveItem);
	objGlobal.Register("MovePet", m_luaFun, &CGLuaFun::Lua_MovePet);
	objGlobal.Register("MoveGold", m_luaFun, &CGLuaFun::Lua_MoveGold);
	objGlobal.Register("DropItem", m_luaFun, &CGLuaFun::Lua_DropItem);
	objGlobal.Register("DropPet", m_luaFun, &CGLuaFun::Lua_DropPet);
	objGlobal.Register("ChangePetState", m_luaFun, &CGLuaFun::Lua_ChangePetState);
	objGlobal.Register("ClickNPCDialog", m_luaFun, &CGLuaFun::Lua_Npc);
	objGlobal.Register("SellNPCStore", m_luaFun, &CGLuaFun::Lua_SellNPCStore);
	objGlobal.Register("BuyNPCStore", m_luaFun, &CGLuaFun::Lua_BuyNPCStore);
	objGlobal.Register("PlayerMenuSelect", m_luaFun, &CGLuaFun::Lua_PlayerMenuSelect);
	objGlobal.Register("UnitMenuSelect", m_luaFun, &CGLuaFun::Lua_UnitMenuSelect);
	objGlobal.Register("UpgradePlayer", m_luaFun, &CGLuaFun::Lua_UpgradePlayer);
	objGlobal.Register("UpgradePet", m_luaFun, &CGLuaFun::Lua_UpgradePet);
	objGlobal.Register("IsBattleUnitValid", m_luaFun, &CGLuaFun::Lua_IsBattleUnitValid);
	objGlobal.Register("GetBattleUnit", m_luaFun, &CGLuaFun::Lua_GetBattleUnit);
	objGlobal.Register("GetBattleUnits", m_luaFun, &CGLuaFun::Lua_GetBattleUnits);
	objGlobal.Register("GetBattleContext", m_luaFun, &CGLuaFun::Lua_GetBattleContext);
	objGlobal.Register("BattleNormalAttack", m_luaFun, &CGLuaFun::Lua_BattleNormalAttack);
	objGlobal.Register("BattleSkillAttack", m_luaFun, &CGLuaFun::Lua_BattleSkillAttack);
	objGlobal.Register("BattleRebirth", m_luaFun, &CGLuaFun::Lua_BattleRebirth);
	objGlobal.Register("BattleGuard", m_luaFun, &CGLuaFun::Lua_BattleGuard);
	objGlobal.Register("BattleEscape", m_luaFun, &CGLuaFun::Lua_BattleEscape);
	objGlobal.Register("BattleExchangePosition", m_luaFun, &CGLuaFun::Lua_BattleExchangePosition);
	objGlobal.Register("BattleDoNothing", m_luaFun, &CGLuaFun::Lua_BattleDoNothing);
	objGlobal.Register("BattleChangePet", m_luaFun, &CGLuaFun::Lua_BattleChangePet);
	objGlobal.Register("BattleUseItem", m_luaFun, &CGLuaFun::Lua_BattleUseItem);
	objGlobal.Register("BattlePetSkillAttack", m_luaFun, &CGLuaFun::Lua_BattlePetSkillAttack);
	objGlobal.Register("BattleSetHighSpeedEnabled", m_luaFun, &CGLuaFun::Lua_BattleSetHighSpeedEnabled);
	objGlobal.Register("SetGameTextUIEnabled", m_luaFun, &CGLuaFun::Lua_SetGameTextUIEnabled);
	objGlobal.Register("SetGameTextUICurrentScript", m_luaFun, &CGLuaFun::Lua_SetGameTextUICurrentScript);
	objGlobal.Register("GetBattleEndTick", m_luaFun, &CGLuaFun::Lua_GetBattleEndTick);
	objGlobal.Register("SetBattleEndTick", m_luaFun, &CGLuaFun::Lua_SetBattleEndTick);
	objGlobal.Register("SetWorkDelay", m_luaFun, &CGLuaFun::Lua_SetWorkDelay);
	objGlobal.Register("StartWork", m_luaFun, &CGLuaFun::Lua_StartWork);
	objGlobal.Register("SetWorkAcceleration", m_luaFun, &CGLuaFun::Lua_SetWorkAcceleration);
	objGlobal.Register("SetImmediateDoneWork", m_luaFun, &CGLuaFun::Lua_SetImmediateDoneWork);
	objGlobal.Register("GetImmediateDoneWorkState", m_luaFun, &CGLuaFun::Lua_GetImmediateDoneWorkState);
	objGlobal.Register("CraftItem", m_luaFun, &CGLuaFun::Lua_CraftItem);
	objGlobal.Register("AssessItem", m_luaFun, &CGLuaFun::Lua_AssessItem);
	objGlobal.Register("GetCraftInfo", m_luaFun, &CGLuaFun::Lua_GetCraftInfo);
	objGlobal.Register("GetCraftsInfo", m_luaFun, &CGLuaFun::Lua_GetCraftsInfo);
	objGlobal.Register("GetCraftStatus", m_luaFun, &CGLuaFun::Lua_GetCraftStatus);
	objGlobal.Register("DoRequest", m_luaFun, &CGLuaFun::Lua_DoRequest);
	objGlobal.Register("TradeAddStuffs", m_luaFun, &CGLuaFun::Lua_TradeAddStuffs);
	objGlobal.Register("AddAllTradeItems", m_luaFun, &CGLuaFun::Lua_AddAllTradeItems);
	objGlobal.Register("IsUIDialogPresent", m_luaFun, &CGLuaFun::Lua_IsUIDialogPresent);
	objGlobal.Register("GetTeamPlayerInfo", m_luaFun, &CGLuaFun::Lua_GetTeamPlayerInfo);
	objGlobal.Register("FixMapWarpStuck", m_luaFun, &CGLuaFun::Lua_FixMapWarpStuck);
	objGlobal.Register("SetNoSwitchAnim", m_luaFun, &CGLuaFun::Lua_SetNoSwitchAnim);
	objGlobal.Register("GetMoveHistory", m_luaFun, &CGLuaFun::Lua_GetMoveHistory);
	objGlobal.Register("EnableFlags", m_luaFun, &CGLuaFun::Lua_EnableFlags);
	objGlobal.Register("SetWindowResolution", m_luaFun, &CGLuaFun::Lua_SetWindowResolution);
	objGlobal.Register("RequestDownloadMap", m_luaFun, &CGLuaFun::Lua_RequestDownloadMap);
	objGlobal.Register("GetNextAnimTickCount", m_luaFun, &CGLuaFun::Lua_GetNextAnimTickCount);
	objGlobal.Register("LoginGameServer", m_luaFun, &CGLuaFun::Lua_LoginGameServer);
	objGlobal.Register("CreateCharacter", m_luaFun, &CGLuaFun::Lua_CreateCharacter);
	objGlobal.Register("PlayGesture", m_luaFun, &CGLuaFun::Lua_PlayGesture);
	objGlobal.Register("DeleteCard", m_luaFun, &CGLuaFun::Lua_DeleteCard);
	;
	objGlobal.Register("SendMail", m_luaFun, &CGLuaFun::Lua_SendMail);
	objGlobal.Register("SendPetMail", m_luaFun, &CGLuaFun::Lua_SendPetMail);

	//luaL_requiref(ls, "common", nullptr, true);
	(*m_pLuaState)->DoString("common=require(\"common\")");
}

QString GameLuaScriptWgt::GetLoginScriptData(int type)
{
	//正在运行js脚本，bat默认js路径
	//if (ui.jsWidget->IsRuningScript())
	//{
	//	return ui.jsWidget
	//}
	////当前没有运行脚本，但lua脚本路径为空，js有，则生成js脚本
	//if (m_scriptPath.isEmpty() && !ui.jsWidget->GetScriptPath().isEmpty())
	//{
	//	return ui.jsWidget
	//}
	//默认lua脚本
	if (type == 3)
		return ui.jsWidget->GetLoginScriptData();
	QString sLoginArg;
	sLoginArg = QString(" -loadscript=\"%2\" -scriptautorestart=%3 -scriptfreezestop=%4 -scriptautoterm=%5"
						" -injuryprotect=%6 -soulprotect=%7 -logbackrestart=%8 -transuserinput=%9 ")
						.arg(m_scriptPath)
						.arg(ui.checkBox_scriptRestart->isChecked())
						.arg(ui.checkBox_noMove->isChecked())
						.arg(false)
						.arg(false)
						.arg(false)
						.arg(ui.checkBox_logBack->isChecked())
						.arg(ui.checkBox_UserInput->isChecked());
	return sLoginArg;
}

void GameLuaScriptWgt::doRunScriptThread(GameLuaScriptWgt *pThis)
{
	if (!pThis)
		return;
	pThis->m_sLuaScriptRunMsg.clear();
	(*pThis->m_pLuaState)->SetHook(LuaHook, LUA_MASKLINE, 0);
	g_pGameFun->RestFun();
	g_pGameCtrl->SetScriptRunState(SCRIPT_CTRL_RUN);
	if (setjmp(g_jmpPlace) == 0)
	{
		qDebug() << "doRunScriptThread Begin";
		//qDebug() << "doRunScriptThread Script" << pThis->m_scriptData;
		try
		{
			//pThis->overrideLuaRequire(pThis->m_pLuaState);
			int runState = (*pThis->m_pLuaState)->DoString(pThis->m_scriptData.toStdString().c_str());
			if (runState != 0)
			{
				qDebug() << "Lua Run State" << runState;
				int type = lua_type((*pThis->m_pLuaState)->GetCState(), -1);
				/*	if (type != 4)
					return;*/
				if (type == 4)
				{
					std::string error = lua_tostring((*pThis->m_pLuaState)->GetCState(), -1);
					qDebug() << "Run Ero String" << QString::fromStdString(error);
					pThis->m_sLuaScriptRunMsg = QString::fromStdString(error);
				}
			}
		}
		catch (const std::exception &e)
		{
			qDebug() << "Try catch RunScriptThread Ero" << e.what();
		}
		catch (...)
		{
			qDebug() << "Try catch RunScriptThread Ero";
		}
	}
	pThis->m_luaFun.ClearRegisterFun();
	qDebug() << "doRunScriptThread End";
	g_pGameCtrl->SetScriptRunState(SCRIPT_CTRL_STOP);
	emit pThis->runScriptFini();
}

void GameLuaScriptWgt::on_pushButton_reloadCommon_clicked()
{
	QString sPath = QApplication::applicationDirPath() + "/lua/reload.lua";
	QString sData = ParseScriptData(sPath);
	int runState = (*m_pLuaState)->DoString(sData.toStdString().c_str());
	if (runState != LUA_OK)
	{
		qDebug() << "Lua Reload State" << runState;
		int type = lua_type((*m_pLuaState)->GetCState(), -1);
		if (type == 4)
		{
			std::string error = lua_tostring((*m_pLuaState)->GetCState(), -1);
			qDebug() << "Reload Ero String" << QString::fromStdString(error);
		}
	}
	(*m_pLuaState)->DoString("function reload( moduleName)\
	package.loaded[moduleName] = nil\
	return require(moduleName) \
	end\
	common=reload(\" common \")");
}

void GameLuaScriptWgt::on_checkBox_noMove_stateChanged(int state)
{
	if (state == Qt::Checked)
		on_lineEdit_noMoveTime_editingFinished();
}

void GameLuaScriptWgt::on_checkBox_noMove_logOut_stateChanged(int state)
{
	if (state == Qt::Checked)
		on_lineEdit_noMoveTime_logOut_editingFinished();
}

void GameLuaScriptWgt::on_lineEdit_noMoveTime_editingFinished()
{
	m_noMoveTime = ui.lineEdit_noMoveTime->text().toInt();
}

void GameLuaScriptWgt::on_lineEdit_noMoveTime_logOut_editingFinished()
{
	m_noMoveLogOutTime = ui.lineEdit_noMoveTime_logOut->text().toInt();
}

void GameLuaScriptWgt::on_lineEdit_scriptRestart_editingFinished()
{
	m_restartScriptTime = ui.lineEdit_scriptRestart->text().toInt();
}

void GameLuaScriptWgt::on_checkBox_UserInput_stateChanged(int state)
{
	g_pGameFun->SetUseDefaultInput(state == Qt::Checked ? true : false);
}

void GameLuaScriptWgt::on_pushButton_pause_clicked()
{
	int nScriptRunState = g_pGameCtrl->GetScriptRunState();
	if (nScriptRunState == SCRIPT_CTRL_RUN) //当前正在运行 先暂停
	{
		ui.pushButton_pause->setText("继续");
		g_pGameCtrl->SetScriptRunState(SCRIPT_CTRL_PAUSE);
	}
	else if (nScriptRunState == SCRIPT_CTRL_PAUSE)
	{
		ui.pushButton_pause->setText("暂停");
		g_pGameCtrl->SetScriptRunState(SCRIPT_CTRL_RUN);
	} //脚本停止 不操作
}

void GameLuaScriptWgt::on_pushButton_open_clicked()
{
	if (m_scriptPath.isEmpty())
		m_scriptPath = QCoreApplication::applicationDirPath() + "//脚本//";
	QString szPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开"), m_scriptPath, "*.lua"); //*.script;
	if (szPath.isEmpty())
		return;
	m_scriptPath = szPath;
	ui.pushButton_start->setEnabled(true);
	ui.pushButton_pause->setText("暂停");
	if (szPath.endsWith(".lua"))
	{
		ParseGameScript(szPath);
		initTableWidget();
		ui.tableWidget->selectRow(ui.tableWidget->rowCount() - 1); //跳到最后一行
	}
}

void GameLuaScriptWgt::ParseGameScript(const QString &szPath)
{
	if (!QFile::exists(szPath))
		return;
	QFile file(szPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	m_scriptData.clear();
	m_scriptLineDataList.clear();
	int nRowIndex = 0;
	//先处理下脚本 空行  还有注释
	QTextCodec *tc = QTextCodec::codecForName("GBK");
	//m_szScriptDesc = tc->toUnicode(file.readLine());
	QByteArray firstLine = file.readLine();
	setUiScriptDesc(firstLine);
	setUiScriptPath(szPath);

	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		m_scriptData.append(line);
		QString szData = tc->toUnicode(line);
		szData = QString::fromLocal8Bit(line);
		//szData = szData.simplified();
		//szData = szData.remove(QRegExp("\\s"));
		//m_scriptData.append(szData);
		szData = szData.simplified(); //空格移除
		m_scriptLineDataList.append(szData);
	}
	file.close();
}
void GameLuaScriptWgt::on_pushButton_openEncrypt_clicked()
{
	QString defaultPath = QCoreApplication::applicationDirPath() + "//脚本//";
	QString szPath = QFileDialog::getOpenFileName(this, QString::fromLatin1("打开"), defaultPath, "*.luae");
	if (szPath.isEmpty())
		return;
	ui.pushButton_start->setEnabled(true);
	ui.pushButton_pause->setText("暂停");

	if (!QFile::exists(szPath))
		return;
	QFile file(szPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	auto scriptData = file.readAll();
	file.close();

	QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::ECB, QAESEncryption::ZERO);

	QString key = "wzq";
	//QString date = "20220110";
	QString data = scriptData; //+date;

	//QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::ECB, QAESEncryption::ZERO);
	QByteArray hashKey2 = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5);
	QByteArray decodedText = encryption.decode(QByteArray::fromBase64(data.toLatin1()), hashKey2);
	QString decodeSciptData = decodedText;
	//qDebug() << "decodedText:" << QString::fromLatin1(decodedText);

	m_scriptData.clear();
	m_scriptLineDataList.clear();
	int nRowIndex = 0;
	//先处理下脚本 空行  还有注释
	QTextCodec *tc = QTextCodec::codecForName("GBK");
	//m_szScriptDesc = tc->toUnicode(file.readLine());
	QStringList scriptLineDataList = decodeSciptData.split("\n");
	QString firstLine = scriptLineDataList.first();
	setUiScriptDesc(firstLine);
	setUiScriptPath(szPath);
	scriptLineDataList.pop_front();

	//解密后脚本，不显示在脚本界面了
	//for (int i = 0; i < scriptLineDataList.size(); ++i)
	//{
	//	//m_scriptData.append(scriptLineDataList[i]);
	//	QString szData = scriptLineDataList[i].simplified(); //空格移除
	//	m_scriptLineDataList.append(szData);
	//}
	m_scriptData = scriptLineDataList.join("\n");
	initTableWidget();
	ui.tableWidget->selectRow(ui.tableWidget->rowCount() - 1); //跳到最后一行
}

bool GameLuaScriptWgt::on_pushButton_start_clicked()
{
	if (m_scriptFuture.isFinished())
	{
		//m_luaFun.LoadScript(m_scriptPath);
		ui.plainTextEdit->clear();
		ui.textEdit_log->clear();
		m_bStopRun = false;
		if (!m_pLuaState)
		{
			initScriptSystem();
		}
		m_scriptFuture = QtConcurrent::run(doRunScriptThread, this);
		//	(*m_pLuaState)->DoString(m_scriptData.toStdString().c_str());
		ui.pushButton_start->setEnabled(false);
		return true;
	}
	else
	{
		AddDebugMsg("已有脚本在运行！");
		//QMessageBox::information(this, "提示：", "已有脚本在运行！", QMessageBox::Ok);
		return false;
	}
}

void GameLuaScriptWgt::on_pushButton_stop_clicked()
{
	/*if (g_pGameCtrl->GetScriptRunState() == SCRIPT_CTRL_STOP)
		return;*/

	m_bStopRun = true;
	g_pGameFun->StopFun();
	g_pGameCtrl->SetScriptRunState(SCRIPT_CTRL_STOP);
	g_pGameFun->end_auto_action(); //停止遇敌
	ui.pushButton_start->setEnabled(true);
	ui.pushButton_pause->setText("暂停");
	if (!m_scriptFuture.isFinished())
	{
		m_scriptFuture.waitForFinished();
	}
	if (m_pLuaState)
	{
		SafeDelete(m_pLuaState);
	}
}

void GameLuaScriptWgt::on_pushButton_save_clicked()
{
}

void GameLuaScriptWgt::on_save_script()
{
	QString savePath = QFileDialog::getSaveFileName(this, tr("保存文件路径"), tr("./"), "*.lua");
	if (savePath.isEmpty()) return;
	QString scriptData = ui.textEdit->toPlainText();
	scriptData += "\n";
	scriptData += m_scriptData;

	QFile xmlFile(savePath);
	if (!xmlFile.open(QIODevice::ReadWrite))
	{
		QString title = QString("打开%1文件").arg(savePath);
		QString text = QString("打开%1文件失败,可能不存在该文件!").arg(savePath);
		QMessageBox::warning(NULL, title, text);
	}
	QTextStream readstream(&xmlFile);					   //构建文本流
	QTextCodec *codec = QTextCodec::codecForName("UTF-8"); //设置编码
	readstream.setCodec(codec);
	xmlFile.resize(0);
	readstream << scriptData;
	xmlFile.close();
}

void GameLuaScriptWgt::on_save_encryptscript()
{
	QString savePath = QFileDialog::getSaveFileName(this, tr("保存文件路径"), tr("./"), "*.luae");
	if (savePath.isEmpty()) return;
	QString scriptData = ui.textEdit->toPlainText();
	//scriptData += "\n";
	scriptData += m_scriptData;

	QFile xmlFile(savePath);
	if (!xmlFile.open(QIODevice::ReadWrite))
	{
		QString title = QString("打开%1文件").arg(savePath);
		QString text = QString("打开%1文件失败,可能不存在该文件!").arg(savePath);
		QMessageBox::warning(NULL, title, text);
	}
	QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::ECB, QAESEncryption::ZERO);

	QString key = "wzq";
	QString date = "20220110";
	QString data = scriptData + date;

	QByteArray hashKey = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5);
	QByteArray encodedText = encryption.encode(data.toUtf8(), hashKey);
	QString str_encode_text = QString::fromLatin1(encodedText.toBase64());
	qDebug() << "encodedText:" << str_encode_text;

	QTextStream readstream(&xmlFile);					   //构建文本流
	QTextCodec *codec = QTextCodec::codecForName("UTF-8"); //设置编码
	readstream.setCodec(codec);
	xmlFile.resize(0);
	readstream << str_encode_text; //	scriptData;
	xmlFile.close();
}

void GameLuaScriptWgt::onTableItemClicked(QTableWidgetItem *pItem)
{
	m_bTrace = false;
	ui.checkBox_IsTrace->setChecked(false);
}

void GameLuaScriptWgt::on_checkBox_IsTrace_stateChanged(int state)
{
	m_bTrace = (state == Qt::Checked ? true : false);
}

void GameLuaScriptWgt::doRunScriptFini()
{
	if (!m_scriptFuture.isRunning())
	{
		m_bStopRun = true;
		ui.pushButton_start->setEnabled(true);
		ui.pushButton_pause->setText("暂停");
		if (!m_sLuaScriptRunMsg.isEmpty())
		{
			AddDebugMsg(m_sLuaScriptRunMsg);
		}
		if (m_pLuaState)
		{
			SafeDelete(m_pLuaState);
		}
	}
}

void GameLuaScriptWgt::doRunNewScript(const QString &sName)
{
	QFileInfo fileinfo(m_scriptPath);
	QString curDir = fileinfo.absolutePath();
	QString newScriptPath = sName;
	if (QFile::exists(newScriptPath) == false)
	{
		newScriptPath = curDir + "//" + sName;
	}
	//全路径
	if (QFile::exists(newScriptPath) == false)
	{
		newScriptPath = QApplication::applicationDirPath() + "//" + sName;
	}
	if (QFile::exists(newScriptPath) == false)
	{
		qDebug() << sName << "切换脚本不存在";
		return;
	}
	if (g_pGameCtrl->GetScriptRunState() == SCRIPT_CTRL_RUN || g_pGameCtrl->GetScriptRunState() == SCRIPT_CTRL_PAUSE)
	{
		on_pushButton_stop_clicked(); //停止之前的脚本
									  /*QEventLoop loop;
		qDebug() << "doRunNewScript Start";
		QTimer::singleShot(10000, &loop, &QEventLoop::quit);
		connect(this, &GameLuaScriptWgt::runScriptFini, &loop, &QEventLoop::quit);
		loop.exec();*/
	}
	//	emit switchScript(newScriptPath);
	doSwitchScript(newScriptPath);
}

void GameLuaScriptWgt::doSwitchScript(const QString &sName)
{
	qDebug() << "加载脚本" << sName;
	m_scriptPath = sName;
	ui.pushButton_start->setEnabled(true);
	ui.pushButton_pause->setText("暂停");
	if (sName.endsWith(".lua"))
	{
		ParseGameScript(sName);
		initTableWidget();
		on_pushButton_start_clicked();
	}
}

void GameLuaScriptWgt::OnAutoRestart()
{
	if (g_CGAInterface->IsConnected())
	{
		int ingame = 0;
		if (g_CGAInterface->IsInGame(ingame) && ingame)
		{
			//if (ui.checkBox_noMove->isChecked() /*&& !g_pGameFun->isEncounter()*/) //不在自动遇敌中
			if (ui.checkBox_noMove->isChecked() || ui.checkBox_noMove_logOut->isChecked())
			{
				int x, y, index1, index2, index3;
				std::string filemap;
				if (g_CGAInterface->GetMapXY(x, y) && g_CGAInterface->GetMapIndex(index1, index2, index3, filemap))
				{
					if (x != m_LastMapX || y != m_LastMapY || index3 != m_LastMapIndex)
					{
						m_LastMapChange = QTime::currentTime();
						m_LastMapChange2 = QTime::currentTime();
						m_LastMapX = x;
						m_LastMapY = y;
						m_LastMapIndex = index3;
					}
					else
					{
						if (ui.checkBox_noMove->isChecked())
						{
							if (m_LastMapChange.elapsed() > m_noMoveTime * 1000)
							{
								m_LastMapChange = QTime::currentTime();
								qDebug() << m_noMoveTime << "秒坐标未动，重启脚本！";
								RestartScript();
								return;
							}
						}
						if (ui.checkBox_noMove_logOut->isChecked())
						{
							if (m_LastMapChange2.elapsed() > m_noMoveLogOutTime * 1000)
							{
								m_LastMapChange2 = QTime::currentTime();
								qDebug() << m_noMoveLogOutTime << "秒坐标未动，登出！";
								g_pGameFun->LogoutServer();
								return;
							}
						}
					}
				}
			}
			else
			{
				m_LastMapChange = QTime::currentTime();
				m_LastMapChange2 = QTime::currentTime();
			}

			if (ui.checkBox_scriptRestart->isChecked() && g_pGameCtrl->GetScriptRunState() == SCRIPT_CTRL_STOP && !m_scriptPath.isEmpty())
			{
				if (m_nLastStopScriptTime.elapsed() <= 0)
				{
					m_nLastStopScriptTime = QTime::currentTime();
				}
				//已经判断了 脚本不是运行状态
				if (m_nLastStopScriptTime.elapsed() > m_restartScriptTime * 1000)
				{
					qDebug() << "到达脚本停止重新启动时间！";
					RestartScript();
					m_nLastStopScriptTime = QTime::currentTime();
				}
			}
			else
			{
				m_nLastStopScriptTime = QTime::currentTime();
			}
		}
		//else
		//{
		//	if (g_pGameCtrl->GetScriptRunState() == SCRIPT_CTRL_RUN)
		//	{
		//		//游戏 不在线，停止脚本	不要加这个，否则一些游戏外脚本会失效
		//		on_pushButton_stop_clicked();
		//	}
		//}
	}
}

void GameLuaScriptWgt::doLoadUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("Script");
	m_restartScriptTime = iniFile.value("ScriptNoRunTime").toInt();
	m_noMoveTime = iniFile.value("PlayerNoMoveTime").toInt();
	m_noMoveLogOutTime = iniFile.value("PlayerNoMoveLogoutTime").toInt();
	ui.lineEdit_noMoveTime->setText(QString::number(m_noMoveTime));
	ui.lineEdit_scriptRestart->setText(QString::number(m_restartScriptTime));
	ui.checkBox_scriptRestart->setChecked(iniFile.value("ScriptNoRunChecked").toBool());
	ui.checkBox_noMove->setChecked(iniFile.value("PlayerNoMoveChecked").toBool());
	ui.checkBox_logBack->setChecked(iniFile.value("RestartLogback").toBool());
	ui.checkBox_UserInput->setChecked(iniFile.value("TransUserInput").toBool());
	iniFile.endGroup();
	ui.gameScriptSetWgt->doLoadUserConfig(iniFile);
}

void GameLuaScriptWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("Script");
	iniFile.setValue("ScriptNoRunTime", m_restartScriptTime);
	iniFile.setValue("PlayerNoMoveTime", m_noMoveTime);
	iniFile.setValue("PlayerNoMoveLogoutTime", m_noMoveLogOutTime);
	iniFile.setValue("ScriptNoRunChecked", ui.checkBox_scriptRestart->isChecked());
	iniFile.setValue("PlayerNoMoveChecked", ui.checkBox_noMove->isChecked());
	iniFile.setValue("RestartLogback", ui.checkBox_logBack->isChecked());
	iniFile.setValue("TransUserInput", ui.checkBox_UserInput->isChecked());
	iniFile.endGroup();
	ui.gameScriptSetWgt->doSaveUserConfig(iniFile);
}

void GameLuaScriptWgt::DoLoadScript(QString path, bool autorestart, bool freezestop, bool injuryprot, bool soulprot, int consolemaxlines, int logBackRestart, int transInput)
{
	if (path.isEmpty())
		return;
	if (!path.endsWith("lua") && !path.endsWith("luae"))
		return;
	QString tmpPath = path;
	//全路径
	if (QFile::exists(tmpPath) == false)
	{
		tmpPath = QApplication::applicationDirPath() + "//" + tmpPath;
	}
	if (QFile::exists(tmpPath) == false)
	{
		qDebug() << path << tmpPath << "切换脚本不存在";
		return;
	}
	m_scriptLogMaxLine = consolemaxlines;
	ui.plainTextEdit->setMaximumBlockCount(10);
	ui.textEdit_log->setMaximumBlockCount(m_scriptLogMaxLine);

	//m_scriptPath = path;
	ui.checkBox_scriptRestart->setChecked(autorestart);
	ui.checkBox_noMove->setChecked(freezestop);
	ui.checkBox_logBack->setChecked(logBackRestart);
	ui.checkBox_UserInput->setChecked(transInput);
	on_pushButton_stop_clicked();
	if (ui.checkBox_logBack->isChecked())
		g_CGAInterface->LogBack();
	//doSwitchScript(path);
	qDebug() << "加载脚本" << tmpPath;
	m_scriptPath = tmpPath;
	ui.pushButton_start->setEnabled(true);
	ui.pushButton_pause->setText("暂停");
	if (tmpPath.endsWith(".lua"))
	{
		ParseGameScript(tmpPath);
		initTableWidget();
		QtConcurrent::run(doCheckGameStateStartScript, this);
		//		on_pushButton_start_clicked();
	}
	//	on_pushButton_start_clicked();
}

void GameLuaScriptWgt::ShowMessage(const QString &szMessage)
{
}

void GameLuaScriptWgt::ClearDebugMsg()
{
	ui.plainTextEdit->clear();
	ui.textEdit_log->clear();
}

void GameLuaScriptWgt::AddDebugMsg(const QString &sMessage, QString sColor)
{
	//QString sMsgData = QString("<font color=\"%1\">[%2] %3</font><br>")
	//						   .arg(QColor(sColor).name())
	//						   .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
	//						   .arg(sMessage);
	QString sMsgData = QString("%1 %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(sMessage);
	//if (m_scriptLogMaxLine > 0)
	//{
	//	while (ui.textEdit_2->document()->lineCount() > m_scriptLogMaxLine)
	//	{
	//		QTextCursor txtcur = ui.textEdit_2->textCursor();
	//		txtcur.movePosition(QTextCursor::Start);
	//		txtcur.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
	//		txtcur.removeSelectedText();
	//	}
	//}
	//ui.plainTextEdit->moveCursor(QTextCursor::End);
	//ui.plainTextEdit->insertHtml(sMsgData);
	//ui.textEdit_log->moveCursor(QTextCursor::End);
	//ui.textEdit_log->insertHtml(sMsgData);
	AddScriptLogMsg(ui.plainTextEdit, sMsgData); //不两份了 不然卡
	AddScriptLogMsg(ui.textEdit_log, sMsgData);
}

void GameLuaScriptWgt::AddLogMsg(const QString &sMessage)
{
	AddDebugMsg(sMessage);
}

void GameLuaScriptWgt::setUiScriptDesc(const QString &sText)
{
	ui.textEdit->setText(sText);
}

void GameLuaScriptWgt::setUiScriptPath(const QString &sText)
{
	QFontMetrics fontWidth(ui.label_scriptPath->font());
	QString showText = fontWidth.elidedText(sText, Qt::ElideMiddle, ui.label_scriptPath->width());
	ui.label_scriptPath->setText(showText);
	ui.label_scriptPath->setToolTip(sText);
}

void GameLuaScriptWgt::BeginTraceScriptRun()
{
	m_bTrace = true;
	ui.checkBox_IsTrace->setChecked(true);
}

void GameLuaScriptWgt::LuaHook(lua_State *L, lua_Debug *ar)
{
	/*if (!g_CGAInterface->IsConnected())
	{
		qDebug() << "游戏窗口关闭，停止脚本";
		luaL_error(L, "User Stop Script");
		return;
	}
	if (!g_pGameFun->IsOnline())
	{
		qDebug() << "游戏掉线，停止脚本";
		luaL_error(L, "User Stop Script");
		return;
	}*/
	//qDebug() << "LuaHook" << ar->currentline;
	//<< ar->name << ar->namewhat << ar->what << ar->source;
	QMutexLocker locker(&g_luaHookMutex);
	if (ar->currentline > 10000)
		return;
	emit g_pGameCtrl->signal_updateScriptRunLine(ar->currentline);
	int nScriptRunState = g_pGameCtrl->GetScriptRunState();
	if (nScriptRunState == SCRIPT_CTRL_STOP)
	{
		qDebug() << "LuaHook 停止脚本";
		//		longjmp(g_jmpPlace, 1);  //跳转到位置并传递1 退出当前脚本
		luaL_error(L, "User Stop Script");
	}
	else if (nScriptRunState == SCRIPT_CTRL_PAUSE) //先不用lua自身暂停
	{
		while (true)
		{
			nScriptRunState = g_pGameCtrl->GetScriptRunState();
			if (nScriptRunState == SCRIPT_CTRL_RUN || nScriptRunState == SCRIPT_CTRL_STOP)
				break;
			Sleep(1000);
		}
	}
	else // (nScriptRunState == SCRIPT_CTRL_PAUSE) //继续运行
	{
	}
	Sleep(g_pGameCtrl->GetScriptDelayTime());
}

void GameLuaScriptWgt::doCheckGameStateStartScript(GameLuaScriptWgt *pThis)
{
	if (!pThis)
		return;
	//30秒启动脚本
	for (size_t i = 0; i < 60; i++)
	{
		if (g_pGameFun->IsInNormalState())
		{
			break;
		}
		Sleep(1000);
	}
	emit pThis->runScriptSignal();
}

void GameLuaScriptWgt::doUpdateScriptRow(int row)
{
	if (!m_bTrace)
		return;

	m_currentRow = row;
	ui.tableWidget->selectRow(m_currentRow - 1);
	//	ui.tableWidget->setCurrentCell(row,0);
	ui.label_curRow->setText(QString("所在行:%1").arg(row));
}

//连接状态 1 2登录成功 10000（NewUI_ShowLostConnectionDialog） 0（貌似断线？） 登录失败 3没有人物
void GameLuaScriptWgt::OnNotifyConnectionState(int state, QString msg)
{
	if ((state == 10000 || state == 0) && !msg.isEmpty())
	{
		//if (msg.contains("角色数据读取失败")) //这个id跳过
		//	"角色数据读取失败，其他窗口已登录！";
	}
	else if (state == 1 || state == 2)
	{
		//登录成功
		qDebug() << g_pGameFun->GetWorkState() << "gameState" << g_pGameFun->GetGameStatus();
		qDebug() << "GameLuaScriptWgt 登录成功";
	}
	else if (state == 3)
	{
		qDebug() << "没有游戏角色！";
	}
}

void GameLuaScriptWgt::GameOnlineStateChange(int state)
{
	if (state == 1) //0掉线 1成功连上
	{				//有些脚本 不是在游戏状态用的 这里还是不重启脚本了
	}
}

void GameLuaScriptWgt::DoStopScriptThread()
{
	if (m_scriptFuture.isRunning())
	{
		LuaObject objGlobal = (*m_pLuaState)->GetGlobals(); //注册全局函数
		auto ls = objGlobal.GetCState();
		qDebug() << "辅助退出，停止脚本!";
		luaL_error(ls, "User Stop Script");
	}
}

void GameLuaScriptWgt::DealMqttTopicData(const QString &topicName, const QString &msg)
{
	return; //调用时候，因为luaDebug缘故，会崩溃 还是改成同步把，调用wait过一段时间等待
	LuaObject objGlobal = (*m_pLuaState)->GetGlobals();
	auto ls = objGlobal.GetState();
	m_luaFun.CallRegisterFun(ls, topicName, msg);
}

void GameLuaScriptWgt::RestartScript()
{
	//确认游戏状态 是不是正常
	if (g_pGameFun->IsInNormalState())
	{
		qDebug() << "重新启动脚本";
		on_pushButton_stop_clicked();
		if (ui.checkBox_logBack->isChecked())
			g_CGAInterface->LogBack();
		on_pushButton_start_clicked();
	}
	else
	{ //回城卡战斗 则直接回城重启 或者登出服务器
		QString mapName = g_pGameFun->GetMapName();
		if (mapName == "法兰城" || mapName == "艾尔莎岛")
		{
			qDebug() << "卡战斗 重新启动脚本";
			on_pushButton_stop_clicked();
			g_CGAInterface->LogBack(); //强制回城重启脚本 还不行就登出
			on_pushButton_start_clicked();
		}
	}
}