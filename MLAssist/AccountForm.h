#ifndef ACCOUNTFORM_H
#define ACCOUNTFORM_H

#include "GlobalDefine.h"
#include <windows.h>
#include <QMap>
#include <QMutex>
#include <QProcess>
#include <QTime>
#include <QWidget>

namespace Ui
{
class AccountForm;
}

class AccountForm : public QWidget
{
	Q_OBJECT

public:
	explicit AccountForm(QWidget *parent = nullptr);
	~AccountForm();

	bool QueryAccount(QString &label, QString &errorMsg);
	bool IsGltExpired();
	bool QueryAttachGameWnd();

	bool IsProcessAttached(quint32 ProcessId);
signals:
	void FetchPlayerData();

private slots:
	void OnPOLCNReadyReadStdOut();
	void OnPOLCNFinish(int exitCode, QProcess::ExitStatus exitStatus);
	void OnPOLCNStarted();
	void OnAutoLogin();
	void on_pushButton_getgid_clicked();
	void on_pushButton_logingame_clicked();
	void on_groupBox_createChara_clicked(bool bflag);
	void on_pushButton_Statistics_clicked();
	void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
	void OnFetchPlayerData();
	void on_checkBox_randomName_stateChanged(int state);

public slots:
	void OnNotifyFillAutoLogin(int game, QString user, QString pwd, QString gid,
			int bigserver, int server, int character,
			bool autologin, bool skipupdate, bool autochangeserver, bool autokillgame,
			bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color,
			QString create_chara_points, QString create_chara_elements, QString create_chara_name);
	void OnSetCreateCharaData(bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color,
			QString create_chara_points, QString create_chara_elements, QString create_chara_name);
	void OnHttpLoadAccount(QString query, QByteArray postdata, QJsonDocument *doc);
	void OnNotifyConnectionState(int state, QString msg);
	bool IsFetchGameData();
	void ResetFetchStatus();
	GameGoodsFetchPtr GetCurrentLoginGid();
	void OnOpenAutoLogin();
	void OnLoginGameServerEx(QString account, QString pwd, QString gid /*= ""*/, int gametype /*= 4*/, int rungame /*= 1*/, int skipupdate /*= 1*/);
	void on_pushButton_saveBat_clicked();
	void OnSaveLoginBat();
	void OnSaveLoginConfigBat();
	void OnSaveLoginScriptCfgBat();
	QString GetLoginArgs();
	void OnSwitchAccountGid(const QString &gid);
	void OnSwitchCharacter(int index);
	void OnSwitchLoginData(QString gid = "", int servr = 0, int line = 0, int character = 0);
	void on_pushButton_logout_clicked();
	void on_pushButton_logback_clicked();
	void on_account_changed();
	void on_account_comboBox_changed(int index);

private:
	Ui::AccountForm *ui;
	QProcess *m_POLCN;
	QByteArray m_StdOut;
	int m_serverid;
	quint32 m_game_pid;
	quint32 m_game_tid;
	QTime m_lastGameWndConnTime; //最后一个窗口连接失败时间

	QString m_glt;
	QString m_lastGameAccount;
	QTime m_loginquery;
	QTime m_loginresult;
	HANDLE m_polcn_lock;
	HANDLE m_polcn_map;
	HANDLE m_glt_lock;
	HANDLE m_glt_map;
	int m_login_failure = 0;

	bool m_bStatistics = false; //统计中？
	bool m_bStatisticsRunning = false;
	QMap<QString, GameGoodsFetchList> m_gidDatas;
	GameGoodsFetchList m_pFetchLoginList;
	bool m_bFirstGltExpired = false; //第一次获取失败
	QTime m_gltExpiredTime;			 //通行证一直获取失败 是卡界面了 把界面关掉重新开一个
	QTime m_attachExistGameWndTime;	 //附加已有窗口
	QMap<int, int> m_roleMapSex; //人物角色和性别对应  后面有几个没单独处理
	int m_loginInterval = 0;
	//先不启用 登录失败消息是一直返回的，所以会很快超过10次 关闭游戏窗口
	QTime m_loginFailureTime; //登录失败定时器 30秒 
	QString m_loginFailureMsg;
	int m_loginFailureState = 0;
};

#endif // ACCOUNTFORM_H
