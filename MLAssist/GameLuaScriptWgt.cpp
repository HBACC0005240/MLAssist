#include "GameLuaScriptWgt.h"
#include "GameCtrl.h"
#include "ITNetworkFactory.h"
#include "ITObjectDataMgr.h"
#include "ITTabBarStyle.h"
#include "LuaCodeHighLighter.h"
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
	ui.tabWidget_luaTab->setStyle(new ITTabBarStyle);
	ui.pushButton_openEncrypt->hide();
	ui.pushButton_save->hide();
	m_pLuaCodeEditor = new LuaCodeEditorDlg;
	m_pLuaCodeEditor->setMinimumSize(400, 300);
	m_pLuaCodeEditor->GetLuaCodeEditor()->setMode(EditorMode::EDIT);
	//m_pLuaCodeEditor->setPlainText("int function()\n{\n--[[te\nst]]\nint a = a + b;\n\treturn 0;\n}");

	m_pLuaCodeHighLighter = new LuaCodeHighLighter();
	m_pLuaCodeHighLighter->setDocument(m_pLuaCodeEditor->GetLuaCodeEditor()->document());
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
	connect(g_pGameCtrl, SIGNAL(signal_exit()), this, SLOT(DoStopScriptThread()), Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, SIGNAL(signal_updateScriptRunLine(int)), this, SLOT(doUpdateScriptRow(int)), Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, SIGNAL(signal_loadScript(const QString &)), this, SLOT(doRunNewScript(const QString &)));
	connect(g_pGameCtrl, &GameCtrl::NotifyFillLoadScript, this, &GameLuaScriptWgt::DoLoadScript, Qt::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_stopScriptRun, this, &GameLuaScriptWgt::on_pushButton_stop_clicked);
	connect(g_pGameCtrl, &GameCtrl::NotifyConnectionState, this, &GameLuaScriptWgt::OnNotifyConnectionState, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_addOneLogMsg, this, &GameLuaScriptWgt::AddLogMsg, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_addOneScriptLogMsg, this, &GameLuaScriptWgt::AddLogMsg, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_gameIsOnline, this, &GameLuaScriptWgt::GameOnlineStateChange, Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::signal_setScriptStillRestartUI, this, [&](int v1, int v2)
			{
				ui.checkBox_noMove->setChecked(v1);
				if (v2 > 0)
				{
					m_noMoveTime = v2;
					ui.lineEdit_noMoveTime->setText(QString::number(v2));
				}
			});
	connect(g_pGameCtrl, &GameCtrl::signal_setScriptStopRestartUI, this, [&](int v1, int v2)
			{
				ui.checkBox_scriptRestart->setChecked(v1);
				if (v2 > 0)
				{
					m_restartScriptTime = v2;
					ui.lineEdit_scriptRestart->setText(QString::number(v2));
				}
			});
	connect(g_pGameCtrl, &GameCtrl::signal_setScriptStopLogbackRestartUI, this, [&](int v1)
			{ ui.checkBox_logBack->setChecked(v1); });
	connect(g_pGameCtrl, &GameCtrl::signal_setScriptStillLogoutUI, this, [&](int v1, int v2)
			{
				ui.checkBox_noMove_logOut->setChecked(v1);
				if (v2 > 0)
				{
					m_noMoveLogOutTime = v2;
					ui.lineEdit_noMoveTime_logOut->setText(QString::number(v2));
				}
			});

	connect(this, SIGNAL(runScriptFini()), this, SLOT(doRunScriptFini()));
	connect(this, SIGNAL(runScriptSignal()), this, SLOT(on_pushButton_start_clicked()));
	connect(this, SIGNAL(switchScript(const QString &)), this, SLOT(doSwitchScript(const QString &)));
	connect(ui.pythonScriptWgt,SIGNAL(addLogToLogWgt(const QString&)),this,SLOT(doAddLogToLogWgt(const QString &)));
	connect(ui.pythonScriptWgt,SIGNAL(clearLogWgtMsg()),this,SLOT(doClearLogWgtMsg()));

	
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
	lua_pushstring(ls, sLuaModulePath.toStdString().c_str()); //.toUtf8()); //local8bit StdString都试过了
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
		QString key = lua_tostring(pState, -2); /*这时候栈顶得下一个,是key*/
		QString val = lua_tostring(pState, -1); /*这时候栈顶得下一个,是key*/

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
	//QString sPath = QApplication::applicationDirPath() + "/脚本/?.lua";
	//qDebug() << sPath;
	//qDebug() << sPath.toUtf8();
	/*QTextCodec *tc = QTextCodec::codecForName("GBK");
	sPath = tc->fromUnicode(sPath);
	qDebug() << sPath;*/
	//LuaAddPath(ls, "path", sPath.replace("/", "\\"));
	//sPath = QApplication::applicationDirPath() + "/lua/?.lua";
	//LuaAddPath(ls, "path", sPath.replace("/", "\\"));

	//CMeLua* MeLua = new CMeLua;//注册对象
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "测试", m_luaFun, &CGLuaFun::Lua_Test);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "字符串转换", m_luaFun, &CGLuaFun::Lua_Translate);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "注册函数", m_luaFun, &CGLuaFun::Lua_RegisterLuaFun);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "utf8ToGbk", m_luaFun, &CGLuaFun::Lua_Translate);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取当前坐标", m_luaFun, &CGLuaFun::GetMapPos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取当前地图名", m_luaFun, &CGLuaFun::Lua_GetMapName);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取当前地图编号", m_luaFun, &CGLuaFun::Lua_GetMapNumber);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取当前地图数据", m_luaFun, &CGLuaFun::Lua_GetMapData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取当前楼层", m_luaFun, &CGLuaFun::Lua_GetMapFloorNumberFromName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取字符串中数字", m_luaFun, &CGLuaFun::Lua_GetNumberFromName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取程序路径", m_luaFun, &CGLuaFun::Lua_GetAppRunPath);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取物品数量", m_luaFun, &CGLuaFun::Lua_GetItemCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取物品叠加数量", m_luaFun, &CGLuaFun::Lua_GetItemPileCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取银行物品数量", m_luaFun, &CGLuaFun::Lua_GetBankItemCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取银行物品叠加数量", m_luaFun, &CGLuaFun::Lua_GetBankItemPileCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取包裹物品数量", m_luaFun, &CGLuaFun::Lua_GetItemCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取包裹物品叠加数量", m_luaFun, &CGLuaFun::Lua_GetItemPileCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取所有物品数量", m_luaFun, &CGLuaFun::Lua_GetAllItemCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取所有物品叠加数量", m_luaFun, &CGLuaFun::Lua_GetAllItemPileCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取包裹空格集合", m_luaFun, &CGLuaFun::Lua_GetItemNotUseSpacePos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取队伍人数", m_luaFun, &CGLuaFun::Lua_GetTeammatesCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取包裹空格", m_luaFun, &CGLuaFun::Lua_GetItemNotUseSpaceCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取已用格", m_luaFun, &CGLuaFun::Lua_GetBagUsedItemCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取好友名片", m_luaFun, &CGLuaFun::Lua_GetFriendCard);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取角色信息", m_luaFun, &CGLuaFun::Lua_GetTgtCharacterGameData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取角色数据", m_luaFun, &CGLuaFun::Lua_GetTgtCharacterGameData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "上报角色数据", m_luaFun, &CGLuaFun::Lua_UploadCharacterGameData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "上报角色信息", m_luaFun, &CGLuaFun::Lua_UploadCharacterGameData);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取脚本界面数据", m_luaFun, &CGLuaFun::Lua_GetScriptUISetData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取队伍宠物平均等级", m_luaFun, &CGLuaFun::Lua_GetTeamPetAvgLv);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取队伍宠物等级", m_luaFun, &CGLuaFun::Lua_GetTeamPetLv);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取迷宫出入口", m_luaFun, &CGLuaFun::Lua_GetAllMazeWarpList);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取迷宫远近坐标", m_luaFun, &CGLuaFun::Lua_GetNextMazeWarp);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取搜索路径", m_luaFun, &CGLuaFun::Lua_FindRandomSearchPath);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取周围信息", m_luaFun, &CGLuaFun::Lua_GetMapUnits);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取队长等待坐标", m_luaFun, &CGLuaFun::Lua_GetUnmannedMapUnitPosList);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取周围空地", m_luaFun, &CGLuaFun::Lua_GetRandomSpace);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "查周围信息", m_luaFun, &CGLuaFun::Lua_FindMapUnit);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "目标是否可达", m_luaFun, &CGLuaFun::Lua_IsReachableTarget);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "设置个人简介", m_luaFun, &CGLuaFun::Lua_SetPlayerInfo);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "世界状态", m_luaFun, &CGLuaFun::Lua_GetWorldStatus);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "游戏状态", m_luaFun, &CGLuaFun::Lua_GetGameStatus);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "游戏时间", m_luaFun, &CGLuaFun::Lua_GetSysTimeEx);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "游戏窗口状态", m_luaFun, &CGLuaFun::Lua_GetConnectGameWndStatus);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "打开游戏窗口", m_luaFun, &CGLuaFun::Lua_RunGameWnd);

	//this->RegisterLuaFun<CGLuaFun>(objGlobal,"取银行空格", m_luaFun, &CGLuaFun::Lua_GetItemNotUseSpaceCount);//不知道银行格数 除非80

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "是否队长", m_luaFun, &CGLuaFun::Lua_IsTeamLeader);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "加入队伍", m_luaFun, &CGLuaFun::Lua_AddTeammate);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "离开队伍", m_luaFun, &CGLuaFun::Lua_LeaveTeammate);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "人物动作", m_luaFun, &CGLuaFun::Lua_DoCharacterAction);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "删除技能", m_luaFun, &CGLuaFun::Lua_DeleteSkill);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "技能是否有效", m_luaFun, &CGLuaFun::Lua_IsSkillValid);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取合成信息", m_luaFun, &CGLuaFun::Lua_GetCraftInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取所有合成信息", m_luaFun, &CGLuaFun::Lua_GetCraftsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取合成状态", m_luaFun, &CGLuaFun::Lua_GetCraftStatus);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待入队", m_luaFun, &CGLuaFun::Lua_WaitTeammatesEx);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待指定入队", m_luaFun, &CGLuaFun::Lua_WaitTeammates);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "自动寻路", m_luaFun, &CGLuaFun::Lua_AutoMove);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "移动", m_luaFun, &CGLuaFun::Lua_MovePos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "移动到", m_luaFun, &CGLuaFun::Lua_WaitMovePos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "移动一格", m_luaFun, &CGLuaFun::Lua_MoveGo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "moveGo", m_luaFun, &CGLuaFun::Lua_MoveGo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "搜索地图", m_luaFun, &CGLuaFun::Lua_SearchMap);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "搜索范围迷宫", m_luaFun, &CGLuaFun::Lua_FindToRandomEntry);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "移动到目标附近", m_luaFun, &CGLuaFun::Lua_MoveToNpcNear);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "下载地图", m_luaFun, &CGLuaFun::Lua_DownloadMap);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "下载指定大小地图", m_luaFun, &CGLuaFun::Lua_DownloadDstSizeMap);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "下载周围地图", m_luaFun, &CGLuaFun::Lua_DownloadRoundMap);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "停止脚本", m_luaFun, &CGLuaFun::Lua_StopScript);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "加载脚本", m_luaFun, &CGLuaFun::Lua_LoadScript);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "切换脚本", m_luaFun, &CGLuaFun::Lua_SwitchScript);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "导入脚本", m_luaFun, &CGLuaFun::Lua_ImportScript);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "执行脚本", m_luaFun, &CGLuaFun::Lua_ExecScript);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "设置脚本简介", m_luaFun, &CGLuaFun::Lua_SetUIScriptDesc);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "开关", m_luaFun, &CGLuaFun::Lua_SetCharacterSwitch);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "清除系统消息", m_luaFun, &CGLuaFun::Lua_ClearSysCue);	 //所有消息 都会清除
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "系统消息", m_luaFun, &CGLuaFun::Lua_GetSysChatMsg);		 //不调用清除接口 则返回是接收的所有系统消息 ;分割
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "聊天", m_luaFun, &CGLuaFun::Lua_GetAllChatMsg);			 //不调用清除接口 则返回是接收的所有消息 ;分割
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "聊天信息", m_luaFun, &CGLuaFun::Lua_GetDetailAllChatMsg); //返回所有消息 带unit 以table形式返回

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "最新系统消息", m_luaFun, &CGLuaFun::Lua_GetLastSysChatMsg); //3秒内系统消息 不包括聊天消息
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "最新聊天", m_luaFun, &CGLuaFun::Lua_GetLastChatMsg);		   //3秒内聊天消息 不包括系统消息
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待系统消息", m_luaFun, &CGLuaFun::Lua_WaitSysMsg);		   //等待收到的最新消息 只是系统消息
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待聊天消息", m_luaFun, &CGLuaFun::Lua_WaitChatMsg);	   //等待收到的最新消息 只是聊天 不包括系统消息
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待最新消息", m_luaFun, &CGLuaFun::Lua_WaitSysAndChatMsg); //等待收到的最新消息 包括系统和聊天
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待订阅消息", m_luaFun, &CGLuaFun::Lua_WaitSubscribeMsg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "订阅消息", m_luaFun, &CGLuaFun::Lua_SubscribeMsg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取消订阅消息", m_luaFun, &CGLuaFun::Lua_RemoveSubscribeMsg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "发布消息", m_luaFun, &CGLuaFun::Lua_PublishMsg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "已接收订阅消息", m_luaFun, &CGLuaFun::Lua_GetTopicMsgList);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "已接收最新订阅消息", m_luaFun, &CGLuaFun::Lua_GetLastTopicMsg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "已接收所有订阅消息", m_luaFun, &CGLuaFun::Lua_GetAllRecvTopicMsgList);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "清空订阅消息", m_luaFun, &CGLuaFun::Lua_RemoveAllTopics);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "debugMsg", m_luaFun, &CGLuaFun::Lua_DebugMessage);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "用户输入框", m_luaFun, &CGLuaFun::Lua_UserDefDialog);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "编辑框", m_luaFun, &CGLuaFun::Lua_UserDefDialog);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "用户下拉框", m_luaFun, &CGLuaFun::Lua_UserDefComboBoxDlg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "用户勾选框", m_luaFun, &CGLuaFun::Lua_UserDefCheckBoxDlg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "用户复选框", m_luaFun, &CGLuaFun::Lua_UserDefCheckBoxDlg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "勾选框", m_luaFun, &CGLuaFun::Lua_UserDefCheckBoxDlg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "复选框", m_luaFun, &CGLuaFun::Lua_UserDefCheckBoxDlg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "下拉框", m_luaFun, &CGLuaFun::Lua_UserDefComboBoxDlg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "读取配置", m_luaFun, &CGLuaFun::Lua_LoadUserConfig);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "保存配置", m_luaFun, &CGLuaFun::Lua_SaveUserConfig);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "获取仓库信息", m_luaFun, &CGLuaFun::Lua_GatherAccountInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "保存仓库信息", m_luaFun, &CGLuaFun::Lua_SaveGatherAccountInfos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "上报仓库信息", m_luaFun, &CGLuaFun::Lua_UploadAccountInfos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "上报角色信息", m_luaFun, &CGLuaFun::Lua_UploadCharacterInfos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "上报银行信息", m_luaFun, &CGLuaFun::Lua_UploadBankInfos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "登出服务器", m_luaFun, &CGLuaFun::Lua_LogoutServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "获取游戏子账户", m_luaFun, &CGLuaFun::Lua_GetAccountGids);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "保存图鉴信息", m_luaFun, &CGLuaFun::Lua_SavePetPictorialBookToHtml);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "设置登录子账号", m_luaFun, &CGLuaFun::Lua_SetUIAccountGid);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "设置登录角色", m_luaFun, &CGLuaFun::Lua_SetUICharacter);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "设置创建角色信息", m_luaFun, &CGLuaFun::Lua_SetUICreateCharacterInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "登录游戏", m_luaFun, &CGLuaFun::Lua_LoginGame);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "重置登录状态", m_luaFun, &CGLuaFun::Lua_ResetLoginGameConnectState);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取登录状态", m_luaFun, &CGLuaFun::Lua_GetLoginGameConnectState);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "回到选择线路", m_luaFun, &CGLuaFun::Lua_BackSelectGameLine);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "切换登录信息", m_luaFun, &CGLuaFun::Lua_SwitchLoginData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "获取随机名称", m_luaFun, &CGLuaFun::Lua_CreateRandomRoleName);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "使用物品", m_luaFun, &CGLuaFun::Lua_UseItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "整理包裹", m_luaFun, &CGLuaFun::Lua_SortBagItems);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "整理银行", m_luaFun, &CGLuaFun::Lua_SortBankItems);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "交易", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待交易", m_luaFun, &CGLuaFun::Lua_WaitTrade);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "交易物品验证确认", m_luaFun, &CGLuaFun::Lua_TradeInternal);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "已接收最新交易消息", m_luaFun, &CGLuaFun::Lua_GetLastRecvTradeDlgInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "已接收所有交易消息", m_luaFun, &CGLuaFun::Lua_GetAllRecvTradeDlgInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "发起交易", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待交易对话框", m_luaFun, &CGLuaFun::Lua_WaitTradeDlg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "添加交易信息", m_luaFun, &CGLuaFun::Lua_TradeAddStuffs);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待对方交易信息", m_luaFun, &CGLuaFun::Lua_WaitTrade);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "交易金币", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "交易物品", m_luaFun, &CGLuaFun::Lua_LaunchTrade);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "交易宠物", m_luaFun, &CGLuaFun::Lua_LaunchTrade);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "扔", m_luaFun, &CGLuaFun::Lua_ThrowItemName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "扔指定位置物品", m_luaFun, &CGLuaFun::Lua_ThrowPosItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "丢", m_luaFun, &CGLuaFun::Lua_ThrowItemName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "捡", m_luaFun, &CGLuaFun::Lua_PickupItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "扔叠加物", m_luaFun, &CGLuaFun::Lua_ThrowNoFullItemName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "拆", m_luaFun, &CGLuaFun::Lua_SplitItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "拆指定位置物品", m_luaFun, &CGLuaFun::Lua_SplitPosItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "拆分物品", m_luaFun, &CGLuaFun::Lua_SplitItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "叠", m_luaFun, &CGLuaFun::Lua_PileItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "买", m_luaFun, &CGLuaFun::Lua_Shopping);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "解析购买列表", m_luaFun, &CGLuaFun::Lua_ParseBuyStoreMsg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "卖", m_luaFun, &CGLuaFun::Lua_Sale);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "出售", m_luaFun, &CGLuaFun::Lua_Sale);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "saleEx", m_luaFun, &CGLuaFun::Lua_SaleEx);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "商店鉴定", m_luaFun, &CGLuaFun::Lua_IdentifyItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "saleEx", m_luaFun, &CGLuaFun::Lua_IdentifyItemEx);
	//this->RegisterLuaFun<CGLuaFun>(objGlobal,"存钱", m_luaFun, &CGLuaFun::Lua_DepositGold);
	//this->RegisterLuaFun<CGLuaFun>(objGlobal,"取钱", m_luaFun, &CGLuaFun::Lua_WithdrawGold);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "扔钱", m_luaFun, &CGLuaFun::Lua_DropGold);
	/*this->RegisterLuaFun<CGLuaFun>(objGlobal,"存物", m_luaFun, &CGLuaFun::Lua_SaveToBankOnce);
	this->RegisterLuaFun<CGLuaFun>(objGlobal,"全存", m_luaFun, &CGLuaFun::Lua_SaveToBankAll);
	this->RegisterLuaFun<CGLuaFun>(objGlobal,"全取", m_luaFun, &CGLuaFun::Lua_WithdrawAllItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal,"取物", m_luaFun, &CGLuaFun::Lua_WithdrawItem);*/
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "扔宠", m_luaFun, &CGLuaFun::Lua_DropPet);
	/*this->RegisterLuaFun<CGLuaFun>(objGlobal,"存宠", m_luaFun, &CGLuaFun::Lua_DepositPet);
	this->RegisterLuaFun<CGLuaFun>(objGlobal,"取宠", m_luaFun, &CGLuaFun::Lua_WithdrawPet);*/
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "银行", m_luaFun, &CGLuaFun::Lua_BankOperation);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "耐久", m_luaFun, &CGLuaFun::Lua_GetItemDurability);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "交换物品", m_luaFun, &CGLuaFun::Lua_SwitchItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "移动物品", m_luaFun, &CGLuaFun::Lua_SwitchItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "装备物品", m_luaFun, &CGLuaFun::Lua_EquipItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "装备代码物品", m_luaFun, &CGLuaFun::Lua_EquipItemEx);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取下装备", m_luaFun, &CGLuaFun::Lua_UnEquipItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "取下代码装备", m_luaFun, &CGLuaFun::Lua_UnEquipItemEx);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "开始遇敌", m_luaFun, &CGLuaFun::Lua_BeginAutoAction);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "停止遇敌", m_luaFun, &CGLuaFun::Lua_EndAutoAction);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待", m_luaFun, &CGLuaFun::Lua_WaitTime);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "回城", m_luaFun, &CGLuaFun::Lua_TownPortalScroll);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "转向", m_luaFun, &CGLuaFun::Lua_TurnAbout);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "面向", m_luaFun, &CGLuaFun::Lua_TurnAboutEx2);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "转向坐标", m_luaFun, &CGLuaFun::Lua_TurnAboutEx);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "转坐标方向", m_luaFun, &CGLuaFun::Lua_TurnAboutPointDir);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "对话", m_luaFun, &CGLuaFun::Lua_TalkNpc);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "对话选择", m_luaFun, &CGLuaFun::Lua_Npc);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "对话选是", m_luaFun, &CGLuaFun::Lua_TalkNpcSelectYes);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "对话坐标选是", m_luaFun, &CGLuaFun::Lua_TalkNpcPosSelectYes);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "对话选否", m_luaFun, &CGLuaFun::Lua_TalkNpcSelectNo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "对话坐标选否", m_luaFun, &CGLuaFun::Lua_TalkNpcPosSelectNo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "菜单选择", m_luaFun, &CGLuaFun::Lua_PlayerMenuSelect);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "菜单项选择", m_luaFun, &CGLuaFun::Lua_UnitMenuSelect);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "回复", m_luaFun, &CGLuaFun::Lua_Renew);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "renew", m_luaFun, &CGLuaFun::Lua_Renew);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "人物", m_luaFun, &CGLuaFun::Lua_GetPlayerData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "人物信息", m_luaFun, &CGLuaFun::Lua_GetPlayerAllData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "下级称号数据", m_luaFun, &CGLuaFun::Lua_GetNextTitleData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "宠物信息", m_luaFun, &CGLuaFun::Lua_GetPetData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "图鉴信息", m_luaFun, &CGLuaFun::Lua_GetDstPetPicBooksInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "检查图鉴", m_luaFun, &CGLuaFun::Lua_CheckHavePetPicBooksInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "物品信息", m_luaFun, &CGLuaFun::Lua_GetAllItemData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "全部宠物信息", m_luaFun, &CGLuaFun::Lua_GetAllPetData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "装备信息", m_luaFun, &CGLuaFun::Lua_GetPlayereEquipData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "装备耐久", m_luaFun, &CGLuaFun::Lua_ParseEquipData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "宠物", m_luaFun, &CGLuaFun::Lua_GetBattlePetData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "队伍", m_luaFun, &CGLuaFun::Lua_GetTeamData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "队伍信息", m_luaFun, &CGLuaFun::Lua_GetAllTeammateData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "宠物更改", m_luaFun, &CGLuaFun::Lua_SetPetData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待服务器返回", m_luaFun, &CGLuaFun::Lua_WaitRecvHead);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待菜单返回", m_luaFun, &CGLuaFun::Lua_WaitRecvPlayerMenu);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待菜单项返回", m_luaFun, &CGLuaFun::Lua_WaitRecvPlayerMenuUnit);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待回补", m_luaFun, &CGLuaFun::Lua_WaitSupplyFini);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待空闲", m_luaFun, &CGLuaFun::Lua_WaitNormal);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待战斗结束", m_luaFun, &CGLuaFun::Lua_WaitBattleEnd);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待按键返回", m_luaFun, &CGLuaFun::Lua_WaitRecvGameWndKeyDown);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待战斗返回", m_luaFun, &CGLuaFun::Lua_WaitRecvBattleAction);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "nop", m_luaFun, &CGLuaFun::Lua_WaitNormal);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "是否战斗中", m_luaFun, &CGLuaFun::Lua_IsInBattle);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "是否空闲中", m_luaFun, &CGLuaFun::Lua_IsInNormalState);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "设置", m_luaFun, &CGLuaFun::Lua_SysConfig);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待到指定地图", m_luaFun, &CGLuaFun::Lua_Nowhile);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "调试", m_luaFun, &CGLuaFun::Lua_DebugMessage);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "日志", m_luaFun, &CGLuaFun::Lua_LogMessage);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "log", m_luaFun, &CGLuaFun::Lua_ScriptLogMessage);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "脚本日志", m_luaFun, &CGLuaFun::Lua_ScriptLogMessage);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "喊话", m_luaFun, &CGLuaFun::Lua_Chat);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "工作", m_luaFun, &CGLuaFun::Lua_Work);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "等待工作返回", m_luaFun, &CGLuaFun::Lua_WaitRecvWorkResult);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "合成", m_luaFun, &CGLuaFun::Lua_AllCompound);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "AllCompound", m_luaFun, &CGLuaFun::Lua_AllCompound);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "压矿", m_luaFun, &CGLuaFun::Lua_Exchange);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "穿墙", m_luaFun, &CGLuaFun::Lua_ThroughWall);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "切图", m_luaFun, &CGLuaFun::Lua_ThroughWall);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "方向穿墙", m_luaFun, &CGLuaFun::Lua_ThroughWallEx);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "是否目标附近", m_luaFun, &CGLuaFun::Lua_IsNearTarget);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "自动迷宫", m_luaFun, &CGLuaFun::Lua_AutoWalkMaze);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "自动穿越迷宫", m_luaFun, &CGLuaFun::Lua_AutoWalkRandomMaze);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "设置穿越迷宫范围", m_luaFun, &CGLuaFun::Lua_SetCrossMazeClipRange);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "发送邮件", m_luaFun, &CGLuaFun::Lua_SendMail);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "接收邮件", m_luaFun, &CGLuaFun::Lua_RecvMail);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "查看邮件", m_luaFun, &CGLuaFun::Lua_RecvMailEx);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "查看所有邮件", m_luaFun, &CGLuaFun::Lua_RecvAllMail);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "更新邮件状态", m_luaFun, &CGLuaFun::Lua_SetMailState);
	//网络部分
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "创建网络服务", m_luaFun, &CGLuaFun::Lua_CreateTcpServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "创建网络客户端", m_luaFun, &CGLuaFun::Lua_ConnectTcpServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "关闭网络服务", m_luaFun, &CGLuaFun::Lua_CloseTcpServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "关闭所有网络服务", m_luaFun, &CGLuaFun::Lua_CloseTcpServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "关闭所有网络客户端", m_luaFun, &CGLuaFun::Lua_CloseTcpClient);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "关闭网络客户端", m_luaFun, &CGLuaFun::Lua_CloseTcpClient);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "发送数据到目标服务", m_luaFun, &CGLuaFun::Lua_SendDataToServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "发送数据到目标客户端", m_luaFun, &CGLuaFun::Lua_SendDataToClient);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "接收目标服务数据", m_luaFun, &CGLuaFun::Lua_RecvDataFromServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "下发数据到所有客户端", m_luaFun, &CGLuaFun::Lua_SendDataToAllClient);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "接收所有客户端数据", m_luaFun, &CGLuaFun::Lua_RecvDataFromAllClient);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "清除服务端接收缓冲区", m_luaFun, &CGLuaFun::Lua_ClearServerRecvBuffer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "清除服务端发送缓冲区", m_luaFun, &CGLuaFun::Lua_ClearServerSendBuffer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "清除客户端接收缓冲区", m_luaFun, &CGLuaFun::Lua_ClearClientRecvBuffer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "清除客户端发送缓冲区", m_luaFun, &CGLuaFun::Lua_ClearClientSendBuffer);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "查询数据", m_luaFun, &CGLuaFun::Lua_SelectGidData);

	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetMapName", m_luaFun, &CGLuaFun::Lua_GetMapName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetGameStatus", m_luaFun, &CGLuaFun::Lua_GetGameStatus);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetWorldStatus", m_luaFun, &CGLuaFun::Lua_GetWorldStatus);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetSysTime", m_luaFun, &CGLuaFun::Lua_GetSysTimeEx);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetBGMIndex", m_luaFun, &CGLuaFun::Lua_GetBGMIndex);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetPlayerInfo", m_luaFun, &CGLuaFun::Lua_GetPlayerAllData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetPetsInfo", m_luaFun, &CGLuaFun::Lua_GetPetData);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetMapPos", m_luaFun, &CGLuaFun::GetMapPos);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetPlayerFlagEnabled", m_luaFun, &CGLuaFun::Lua_SetPlayerFlagEnabled);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "IsPlayerFlagEnabled", m_luaFun, &CGLuaFun::Lua_IsPlayerFlagEnabled);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "IsSkillValid", m_luaFun, &CGLuaFun::Lua_IsSkillValid);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetSkillInfo", m_luaFun, &CGLuaFun::Lua_GetSkillInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetSkillsInfo", m_luaFun, &CGLuaFun::Lua_GetSkillsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetSubSkillInfo", m_luaFun, &CGLuaFun::Lua_GetSubSkillInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetSubSkillsInfo", m_luaFun, &CGLuaFun::Lua_GetSubSkillsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "IsPetValid", m_luaFun, &CGLuaFun::Lua_IsPetValid);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetPetInfo", m_luaFun, &CGLuaFun::Lua_GetPetInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetBankPetsInfo", m_luaFun, &CGLuaFun::Lua_GetBankPetsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "IsPetSkillValid", m_luaFun, &CGLuaFun::Lua_IsPetSkillValid);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetPetSkillInfo", m_luaFun, &CGLuaFun::Lua_GetPetSkillInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetPetSkillsInfo", m_luaFun, &CGLuaFun::Lua_GetPetSkillsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetMapIndex", m_luaFun, &CGLuaFun::Lua_GetMapIndex);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetMoveSpeed", m_luaFun, &CGLuaFun::Lua_GetMoveSpeed);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetMouseXY", m_luaFun, &CGLuaFun::Lua_GetMouseXY);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetMouseOrientation", m_luaFun, &CGLuaFun::Lua_GetMouseOrientation);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "WalkTo", m_luaFun, &CGLuaFun::Lua_WalkTo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "TurnTo", m_luaFun, &CGLuaFun::Lua_TurnTo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetMoveSpeed", m_luaFun, &CGLuaFun::Lua_SetMoveSpeed);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "ForceMove", m_luaFun, &CGLuaFun::Lua_ForceMove);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "ForceMoveTo", m_luaFun, &CGLuaFun::Lua_ForceMoveTo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "IsMapCellPassable", m_luaFun, &CGLuaFun::Lua_IsMapCellPassable);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "LogBack", m_luaFun, &CGLuaFun::Lua_LogBack);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "LogOut", m_luaFun, &CGLuaFun::Lua_LogOut);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BackSelectServer", m_luaFun, &CGLuaFun::Lua_BackSelectServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SayWords", m_luaFun, &CGLuaFun::Lua_Chat);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "ChangeNickName", m_luaFun, &CGLuaFun::Lua_ChangeNickName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "ChangeTitleName", m_luaFun, &CGLuaFun::Lua_ChangeTitleName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "ChangePersDesc", m_luaFun, &CGLuaFun::Lua_ChangePersDesc);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "ChangePetName", m_luaFun, &CGLuaFun::Lua_ChangePetName);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetCardsInfo", m_luaFun, &CGLuaFun::Lua_GetCardsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetCardsRecvMsg", m_luaFun, &CGLuaFun::Lua_GetCardsRecvMsg);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetPicBooksInfo", m_luaFun, &CGLuaFun::Lua_GetPicBooksInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "IsItemValid", m_luaFun, &CGLuaFun::Lua_IsItemValid);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetItemInfo", m_luaFun, &CGLuaFun::Lua_GetItemInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetItemsInfo", m_luaFun, &CGLuaFun::Lua_GetItemsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetBankItemsInfo", m_luaFun, &CGLuaFun::Lua_GetBankItemsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetBankGold", m_luaFun, &CGLuaFun::Lua_GetBankGold);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "UseItem", m_luaFun, &CGLuaFun::Lua_UseItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "MoveItem", m_luaFun, &CGLuaFun::Lua_MoveItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "MovePet", m_luaFun, &CGLuaFun::Lua_MovePet);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "MoveGold", m_luaFun, &CGLuaFun::Lua_MoveGold);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "DropItem", m_luaFun, &CGLuaFun::Lua_DropItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "DropPet", m_luaFun, &CGLuaFun::Lua_DropPet);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "ChangePetState", m_luaFun, &CGLuaFun::Lua_ChangePetState);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "ClickNPCDialog", m_luaFun, &CGLuaFun::Lua_Npc);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SellNPCStore", m_luaFun, &CGLuaFun::Lua_SellNPCStore);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BuyNPCStore", m_luaFun, &CGLuaFun::Lua_BuyNPCStore);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "PlayerMenuSelect", m_luaFun, &CGLuaFun::Lua_PlayerMenuSelect);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "UnitMenuSelect", m_luaFun, &CGLuaFun::Lua_UnitMenuSelect);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "UpgradePlayer", m_luaFun, &CGLuaFun::Lua_UpgradePlayer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "UpgradePet", m_luaFun, &CGLuaFun::Lua_UpgradePet);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "IsBattleUnitValid", m_luaFun, &CGLuaFun::Lua_IsBattleUnitValid);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetBattleUnit", m_luaFun, &CGLuaFun::Lua_GetBattleUnit);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetBattleUnits", m_luaFun, &CGLuaFun::Lua_GetBattleUnits);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetBattleContext", m_luaFun, &CGLuaFun::Lua_GetBattleContext);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleNormalAttack", m_luaFun, &CGLuaFun::Lua_BattleNormalAttack);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleSkillAttack", m_luaFun, &CGLuaFun::Lua_BattleSkillAttack);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleRebirth", m_luaFun, &CGLuaFun::Lua_BattleRebirth);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleGuard", m_luaFun, &CGLuaFun::Lua_BattleGuard);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleEscape", m_luaFun, &CGLuaFun::Lua_BattleEscape);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleExchangePosition", m_luaFun, &CGLuaFun::Lua_BattleExchangePosition);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleDoNothing", m_luaFun, &CGLuaFun::Lua_BattleDoNothing);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleChangePet", m_luaFun, &CGLuaFun::Lua_BattleChangePet);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleUseItem", m_luaFun, &CGLuaFun::Lua_BattleUseItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattlePetSkillAttack", m_luaFun, &CGLuaFun::Lua_BattlePetSkillAttack);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "BattleSetHighSpeedEnabled", m_luaFun, &CGLuaFun::Lua_BattleSetHighSpeedEnabled);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetGameTextUIEnabled", m_luaFun, &CGLuaFun::Lua_SetGameTextUIEnabled);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetGameTextUICurrentScript", m_luaFun, &CGLuaFun::Lua_SetGameTextUICurrentScript);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetBattleEndTick", m_luaFun, &CGLuaFun::Lua_GetBattleEndTick);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetBattleEndTick", m_luaFun, &CGLuaFun::Lua_SetBattleEndTick);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetWorkDelay", m_luaFun, &CGLuaFun::Lua_SetWorkDelay);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "StartWork", m_luaFun, &CGLuaFun::Lua_StartWork);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetWorkAcceleration", m_luaFun, &CGLuaFun::Lua_SetWorkAcceleration);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetImmediateDoneWork", m_luaFun, &CGLuaFun::Lua_SetImmediateDoneWork);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetImmediateDoneWorkState", m_luaFun, &CGLuaFun::Lua_GetImmediateDoneWorkState);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "CraftItem", m_luaFun, &CGLuaFun::Lua_CraftItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "AssessItem", m_luaFun, &CGLuaFun::Lua_AssessItem);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetCraftInfo", m_luaFun, &CGLuaFun::Lua_GetCraftInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetCraftsInfo", m_luaFun, &CGLuaFun::Lua_GetCraftsInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetCraftStatus", m_luaFun, &CGLuaFun::Lua_GetCraftStatus);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "DoRequest", m_luaFun, &CGLuaFun::Lua_DoRequest);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "TradeAddStuffs", m_luaFun, &CGLuaFun::Lua_TradeAddStuffs);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "AddAllTradeItems", m_luaFun, &CGLuaFun::Lua_AddAllTradeItems);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "IsUIDialogPresent", m_luaFun, &CGLuaFun::Lua_IsUIDialogPresent);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetTeamPlayerInfo", m_luaFun, &CGLuaFun::Lua_GetTeamPlayerInfo);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "FixMapWarpStuck", m_luaFun, &CGLuaFun::Lua_FixMapWarpStuck);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetNoSwitchAnim", m_luaFun, &CGLuaFun::Lua_SetNoSwitchAnim);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetMoveHistory", m_luaFun, &CGLuaFun::Lua_GetMoveHistory);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "EnableFlags", m_luaFun, &CGLuaFun::Lua_EnableFlags);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SetWindowResolution", m_luaFun, &CGLuaFun::Lua_SetWindowResolution);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "RequestDownloadMap", m_luaFun, &CGLuaFun::Lua_RequestDownloadMap);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "GetNextAnimTickCount", m_luaFun, &CGLuaFun::Lua_GetNextAnimTickCount);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "LoginGameServer", m_luaFun, &CGLuaFun::Lua_LoginGameServer);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "CreateCharacter", m_luaFun, &CGLuaFun::Lua_CreateCharacter);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "PlayGesture", m_luaFun, &CGLuaFun::Lua_PlayGesture);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "DeleteCard", m_luaFun, &CGLuaFun::Lua_DeleteCard);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SendMail", m_luaFun, &CGLuaFun::Lua_SendMail);
	this->RegisterLuaFun<CGLuaFun>(objGlobal, "SendPetMail", m_luaFun, &CGLuaFun::Lua_SendPetMail);
	if (!m_bLuaCodeEditorInit)
	{
		m_bLuaCodeEditorInit = true;
		m_pLuaCodeHighLighter->initHighLighterRule();
	}
	//luaL_requiref(ls, "common", nullptr, true);
	(*m_pLuaState)->DoString("common=require(\"common\")");
	m_regLuaFunNames.removeDuplicates();
}
template <class Callee>
void GameLuaScriptWgt::RegisterLuaFun(LuaObject &objGlobal, const char *funcName, const Callee &callee, int (Callee::*func)(LuaState *), int nupvalues /*= 0*/)
{
	objGlobal.Register(funcName, callee, func);
	m_regLuaFunNames.append(funcName);
	if (!m_bLuaCodeEditorInit)
		m_pLuaCodeHighLighter->appendHighLightingFun(funcName);
}

void GameLuaScriptWgt::UnRegisterLuaFun(LuaObject &objGlobal, const char *funcName)
{
	objGlobal.Unregister(funcName);
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
			(*pThis->m_pLuaState)->DoString("collectgarbage(\"collect\")"); //lua脚本内存回收
			(*pThis->m_pLuaState)->DoString("collectgarbage(\"collect\")"); //lua脚本内存回收
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
	//QString szPath = QFileDialog::getOpenFileName(nullptr, QString::fromLocal8Bit("打开"), m_scriptPath, "*.lua", nullptr, QFileDialog::DontUseNativeDialog); //*.script;
	QString szPath = QFileDialog::getOpenFileName(nullptr, QString::fromLocal8Bit("打开"), m_scriptPath, "*.lua"); //*.script;
	if (szPath.isEmpty())
		return;
	openScript(szPath);
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
	m_scriptLineDataList.append(firstLine);

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
		if (m_scriptPath.isEmpty())
			return false;
		openScript(m_scriptPath);
		ui.plainTextEdit->clear();
		ui.textEdit_log->clear();
		m_bStopRun = false;
		if (!m_pLuaState)
		{
			initScriptSystem();
		}
		//清空变量界面
		ui.luaScriptInputWgt->ClearAllInputWidget();

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
		LuaObject objGlobal = (*m_pLuaState)->GetGlobals();
		for (auto luaFun : m_regLuaFunNames)
		{
			objGlobal.Unregister(luaFun.toStdString().c_str());
		}
		m_regLuaFunNames.clear();
	}	
	SafeDelete(m_pLuaState);

	g_pNetworkFactory->CloseAllTcpServer();
	g_pNetworkFactory->CloseAllTcpClient();
}

void GameLuaScriptWgt::on_pushButton_save_clicked()
{
}

void GameLuaScriptWgt::on_pushButton_edit_clicked()
{
	//m_pLuaCodeEditor->GetLuaCodeEditor()->setPlainText(m_scriptData);
	m_pLuaCodeEditor->SetOpenLuaScriptPath(m_scriptPath);
	m_pLuaCodeEditor->show();
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
								if (ui.checkBox_logoutKill->isChecked())
								{
									g_pGameCtrl->KillGameWndProcess();
								}
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
	ui.lineEdit_noMoveTime_logOut->setText(QString::number(m_noMoveLogOutTime));
	ui.lineEdit_scriptRestart->setText(QString::number(m_restartScriptTime));
	ui.checkBox_scriptRestart->setChecked(iniFile.value("ScriptNoRunChecked").toBool());
	ui.checkBox_noMove->setChecked(iniFile.value("PlayerNoMoveChecked").toBool());
	ui.checkBox_noMove_logOut->setChecked(iniFile.value("PlayerNoMoveLogoutChecked").toBool());
	ui.checkBox_logBack->setChecked(iniFile.value("RestartLogback").toBool());
	ui.checkBox_UserInput->setChecked(iniFile.value("TransUserInput").toBool());
	iniFile.endGroup();
	ui.gameScriptSetWgt->doLoadUserConfig(iniFile);
	ui.luaScriptInputWgt->doLoadUserConfig(iniFile);
}

void GameLuaScriptWgt::doSaveUserConfig(QSettings &iniFile)
{
	iniFile.beginGroup("Script");
	iniFile.setValue("ScriptNoRunTime", m_restartScriptTime);
	iniFile.setValue("PlayerNoMoveTime", m_noMoveTime);
	iniFile.setValue("PlayerNoMoveLogoutTime", m_noMoveLogOutTime);
	iniFile.setValue("ScriptNoRunChecked", ui.checkBox_scriptRestart->isChecked());
	iniFile.setValue("PlayerNoMoveChecked", ui.checkBox_noMove->isChecked());
	iniFile.setValue("PlayerNoMoveLogoutChecked", ui.checkBox_noMove_logOut->isChecked());
	iniFile.setValue("RestartLogback", ui.checkBox_logBack->isChecked());
	iniFile.setValue("TransUserInput", ui.checkBox_UserInput->isChecked());
	iniFile.endGroup();
	ui.gameScriptSetWgt->doSaveUserConfig(iniFile);
	ui.luaScriptInputWgt->doSaveUserConfig(iniFile);
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
	emit g_pGameCtrl->signal_updateScriptRunLine(ar->currentline + 1); //第一条注释
	int nScriptRunState = g_pGameCtrl->GetScriptRunState();
	if (nScriptRunState == SCRIPT_CTRL_STOP)
	{
		qDebug() << "LuaHook 停止脚本";
		//		longjmp(g_jmpPlace, 1);  //跳转到位置并传递1 退出当前脚本
		luaL_error(L, "用户停止脚本");
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
	on_pushButton_stop_clicked();
	//if (m_scriptFuture.isRunning())
	//{
	//	LuaObject objGlobal = (*m_pLuaState)->GetGlobals(); //注册全局函数
	//	auto ls = objGlobal.GetCState();
	//	qDebug() << "辅助退出，停止脚本!";
	//	luaL_error(ls, "User Stop Script");
	//}
}

void GameLuaScriptWgt::DealMqttTopicData(const QString &topicName, const QString &msg)
{
	return; //调用时候，因为luaDebug缘故，会崩溃 还是改成同步把，调用wait过一段时间等待
	LuaObject objGlobal = (*m_pLuaState)->GetGlobals();
	auto ls = objGlobal.GetState();
	m_luaFun.CallRegisterFun(ls, topicName, msg);
}

void GameLuaScriptWgt::doAddLogToLogWgt(const QString & msg)
{
	AddScriptLogMsg(ui.textEdit_log, msg);
}

void GameLuaScriptWgt::doClearLogWgtMsg()
{
	ui.textEdit_log->clear();
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

void GameLuaScriptWgt::openScript(const QString &sPath)
{
	m_scriptPath = sPath;
	ui.pushButton_start->setEnabled(true);
	ui.pushButton_pause->setText("暂停");
	if (sPath.endsWith(".lua"))
	{
		ParseGameScript(sPath);
		initTableWidget();
		ui.tableWidget->selectRow(ui.tableWidget->rowCount() - 1); //跳到最后一行
	}
}
