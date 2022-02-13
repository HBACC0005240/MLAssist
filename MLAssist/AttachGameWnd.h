#pragma once

#include "ProcessTable.h"
#include "YunLai.h"
#include "ui_AttachGameWnd.h"
#include <QTimer>
#include <QWidget>
//游戏附加窗口
class CRetryAttachProcessTimer : public QTimer
{
	Q_OBJECT
public:
	explicit CRetryAttachProcessTimer(quint32 ProcessId, quint32 ThreadId, QObject *parent = Q_NULLPTR) :
			QTimer(parent)
	{
		m_ProcessId = ProcessId;
		m_ThreadId = ThreadId;
		m_retry = 0;
	}

	quint32 m_ProcessId;
	quint32 m_ThreadId;
	int m_retry;
};
class AttachGameWnd : public QWidget
{
	Q_OBJECT

public:
	AttachGameWnd(QWidget *parent = Q_NULLPTR);
	~AttachGameWnd();
	DWORD GetGameProcessId(wchar_t *ProcessNmae);
	bool InJectDll(DWORD ProcessId, const char *DllName);
	bool InJectDll2(DWORD ProcessId, const char *DllName);
	bool ReadSharedData(quint32 ProcessId, int &port, quint32 &hWnd);
	bool CreateAttachMutex(quint32 ProcessId, quint32 ThreadId);
	void ConnectToServer(quint32 ProcessId, quint32 ThreadId, int port, quint32 hwnd);
	bool IsProcessAttached(quint32 ProcessId);
	void Disconnect();
	bool InjectByMsgHook(quint32 ThreadId, quint32 hWnd, QString &dllPath, int &errorCode, QString &errorString);
	QString GetLoginGameData();
signals:
	void signal_followGameWnd(long left, long top, long right, long bottom);

protected:
	//自动登录需要
	void OnQueueAttachProcess(quint32 ProcessId, quint32 ThreadId, quint32 hWnd, QString dllPath);
public slots:
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void on_pushButton_logback();
	void on_pushButton_logout();
	void on_pushButton_attach_clicked();
	void on_pushButton_refresh_clicked();
	void on_pushButton_unAttach_clicked();
	void on_pushButton_login_clicked();
	void OnCheckFreezeProcess(); //检查进程生命周期
	void OnRetryAttachProcess();
	void OnKillProcess();
	void OnAutoAttachProcess(quint32 ProcessId, quint32 ThreadId);
	void OnQueueQueryProcess();
	void OnNotifyQueryProcess(CProcessItemList list);

private:
	Ui::AttachGameWnd ui;
	CProcessTableModel *m_model;
	bool m_sync = false;
	double m_LastGameAnimTick = 0;
	int m_AnimTickFreezeTime = 0;
	int m_MaxFreezeTime = 30;

	QPushButton *m_pLogBack;
	QPushButton *m_pLogOut;
	quint32 m_AutoAttachPID = 0;
	quint32 m_AutoAttachTID = 0;
};
