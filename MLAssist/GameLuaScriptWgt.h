#pragma once

#include "CGLuaFun.h"
#include "ui_GameLuaScriptWgt.h"
#include <QMutex>
#include <QWidget>
static QMutex g_luaHookMutex;

class GameLuaScriptWgt : public QWidget
{
	Q_OBJECT

public:
	GameLuaScriptWgt(QWidget *parent = Q_NULLPTR);
	~GameLuaScriptWgt();

	void initTableWidget();
	void initScriptSystem();
	QString GetLoginScriptData(int type = 0);
	static void doRunScriptThread(GameLuaScriptWgt *pThis);
	static void LuaHook(lua_State *L, lua_Debug *ar);
	static void doCheckGameStateStartScript(GameLuaScriptWgt *pThis);

signals:
	void runScriptFini();
	void switchScript(const QString &sName);
	void runScriptSignal();

protected:
	void ParseGameScript(const QString &szPath);
	void LuaAddPath(lua_State *ls, QString pathName, QString addVal);
	void AddScriptLogMsg(QPlainTextEdit *pEdit, const QString &sMsg);
	void AddMyLuaLoader(lua_State *pState);
	static int MyLoader(lua_State *pState);
	bool overrideLuaRequire(LuaStateOwner *pOwner);
	QString ParseScriptData(const QString &sPath);
	void RestartScript();

public slots:
	void on_pushButton_reloadCommon_clicked();
	void on_checkBox_noMove_stateChanged(int state);
	void on_checkBox_noMove_logOut_stateChanged(int state);
	void on_lineEdit_noMoveTime_editingFinished();
	void on_lineEdit_noMoveTime_logOut_editingFinished();
	void on_lineEdit_scriptRestart_editingFinished();
	void on_checkBox_UserInput_stateChanged(int state);
	void on_pushButton_pause_clicked();
	void on_pushButton_open_clicked();
	void on_pushButton_openEncrypt_clicked();
	bool on_pushButton_start_clicked();
	void on_pushButton_stop_clicked();
	void on_pushButton_save_clicked();
	void on_save_script();
	void on_save_encryptscript();
	void on_customContextMenu(const QPoint &pos);
	void gotoScriptRow();
	void onUpdateUI();
	void ShowMessage(const QString &szMessage);
	void ClearDebugMsg();
	void AddDebugMsg(const QString &sMessage, QString sColor = "#0000ff");
	void AddLogMsg(const QString &sMessage);
	void setUiScriptDesc(const QString &sText);
	void setUiScriptPath(const QString &sText);
	void BeginTraceScriptRun();						  //开始追踪
	void doUpdateScriptRow(int row);				  //脚本当前运行位置
	void onTableItemClicked(QTableWidgetItem *pItem); //用户点击表格项
	void on_checkBox_IsTrace_stateChanged(int state);
	void doRunScriptFini(); //脚本执行完成 回复ui界面
	void doRunNewScript(const QString &sName);
	void doSwitchScript(const QString &sName);
	void OnAutoRestart();
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void DoLoadScript(QString path, bool autorestart, bool freezestop, bool injuryprot, bool soulprot, int consolemaxlines, int logBackRestart, int transInput);
	void OnNotifyConnectionState(int state, QString msg);
	void GameOnlineStateChange(int state);
	void DoStopScriptThread();
	void DealMqttTopicData(const QString &topicName, const QString &msg);

private:
	Ui::GameLuaScriptWgt ui;
	QString m_sLuaScriptRunMsg;
	bool m_bTrace = true;
	int m_nLuaYieldResult;
	bool m_bStopRun = false;
	QString m_scriptPath;			  //脚本路径
	int m_currentRow;				  //当前行
	CGLuaFun m_luaFun;				  //lua脚本对象
	QStringList m_scriptLineDataList; //脚本数据
	QString m_scriptData;
	LuaStateOwner *m_pLuaState;		  //初始化lua的基本库，才能调用lua函数
	QFuture<void> m_scriptFuture; //获取人物信息线程
	int m_LastMapX;
	int m_LastMapY;
	int m_LastMapIndex;
	QTime m_LastMapChange;
	QTime m_LastMapChange2;
	QTime m_nLastStopScriptTime; //脚本停止时间
	int m_noMoveTime = 60;
	int m_noMoveLogOutTime = 60;
	int m_restartScriptTime = 160;
	int m_scriptLogMaxLine = 100; //默认100行
};
