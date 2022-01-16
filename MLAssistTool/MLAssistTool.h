#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MLAssistTool.h"
#include <QTimer>
#include "FzTableModel.h"
#include "MLWndTableModel.h"
#include <windows.h>

class MLAssistTool : public QMainWindow
{
	Q_OBJECT

public:
	MLAssistTool(QWidget* parent = Q_NULLPTR);
	~MLAssistTool();
	void init();
	void initTable();
	bool isInGame(DWORD pid);
protected:
	void ctrlAllState(int val);
	void ctrlAllStateTab(int index, int val);
	bool checkKillHungGame(HWND wnd, DWORD pid);
	void statisticsGameState();

public slots:
	void on_pushButton_refresh_clicked();
	void onRefreshFzData();
	void OnNotifyQueryMLWnd(FzTableItemList list);
	void OnNotifyQueryFZWnd(FzTableItemList list);
	void on_comboBox_currentIndexChanged(int);
	void on_pushButton_openAll_clicked();
	void on_pushButton_closeAll_clicked();
	void on_pushButton_closeCur_clicked();
	void on_pushButton_allHide_clicked();
	void on_pushButton_allMin_clicked();
	void on_pushButton_allShow_clicked();
	void on_pushButton_curHide_clicked();
	void on_pushButton_curShow_clicked();
	void on_pushButton_curMin_clicked();
	void on_checkBox_defaultMin_stateChanged(int state);

private:
	Ui::MLAssistToolClass ui;
	bool m_bDefaultMinimize = false;
	int m_updateTime = 300000;	//30秒
	QString m_oneKeyStartBat;
	QTimer m_refreshTimer;
	FzTableModel* m_model;
	MLWndTableModel* m_mlModl;
	QMap<HWND, DWORD> m_gameWndForLastHungTime;	//窗口无响应上次检测时间
};
