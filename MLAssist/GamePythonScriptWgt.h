#pragma once

#include "ui_GamePythonScriptWgt.h"
#include <QWidget>

#include <QPlainTextEdit>
#include <QProcess>
#include <QTime>
#include <QWidget>

class GamePythonScriptWgt : public QWidget
{
	Q_OBJECT

public:
	GamePythonScriptWgt(QWidget *parent = Q_NULLPTR);
	~GamePythonScriptWgt();
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

	void UpdateGameTextUI(bool show);
	QString GetScriptPath() { return m_scriptPath; }
	bool IsRuningScript();
	QString GetLoginScriptData();

signals:
	void ReportNavigatorPath(QString json);
	void ReportNavigatorFinish(int exitCode);
	void addLogToLogWgt(const QString& msg);
	void clearLogWgtMsg();

public slots:
	void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
	void on_pushButton_load_clicked();
	void on_pushButton_run_clicked();
	void on_pushButton_debug_clicked();
	void on_pushButton_term_clicked();
	void OnNotifyFillLoadScript(QString path, bool autorestart, bool freezestop, bool injuryprot, bool soulprot,
			int consolemaxlines, int logBackRestart, int transInput, int scriptfreezeDuration);
	void RunNavigatorScript(int x, int y, int enter, QString *result);
	void StopNavigatorScript();
	void OnHttpLoadScript(QString query, QByteArray postdata, QJsonDocument *doc);

private slots:
	void OnPythonStarted();
	void OnPythonStartError(QProcess::ProcessError error);
	void OnPythonReadyRead();
	void OnPythonFinish(int exitCode, QProcess::ExitStatus exitStatus);
	void OnCloseWindow();
	void OnAutoRestart();
	void on_pushButton_suspend_clicked();

private:
	Ui::GamePythonScriptWgt ui;
	QPlainTextEdit *m_output;
	QString m_scriptPath;
	QString m_chromePath;
	QProcess *m_python;

	QString m_PathString;
	bool m_bPathBegin;
	bool m_bNavigating;
	bool m_bListening;
	bool m_bDebugging;
	bool m_bSuspending;
	quint32 m_port;

	int m_ConsoleMaxLines;

	int m_LastMapX;
	int m_LastMapY;
	int m_LastMapIndex;
	QTime m_LastMapChange;
};
