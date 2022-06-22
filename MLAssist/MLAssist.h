#pragma once

#include "YunLai.h"
#include "ui_MLAssist.h"
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QThread>
#include <QTimer>
#include <QtWidgets/QDialog>
class MLAssist : public QWidget
{
	Q_OBJECT

public:
	MLAssist(QWidget *parent = Q_NULLPTR);
	~MLAssist();

	void createSysTrayAction();
	void closeEvent(QCloseEvent *event);
signals:
protected:
	void killProcess();
	bool eventFilter(QObject *obj, QEvent *event);
	bool ParseSettings(const QByteArray &data, QJsonDocument &doc);
	QString CheckFileFormat(const QString &fileName);
	QString GetCorrectUnicode(const QByteArray &ba);

public slots:
	void on_ctrl_app(int val); //控制程序显示隐藏
	void doIconActivated(QSystemTrayIcon::ActivationReason reason);
	void quitAndDeleteAllInfo();
	void on_pushButton_min_clicked();
	void on_pushButton_close_clicked();
	void onUpdateTrayToolTip(const QString &szText);
	void on_pushButton_loadCfg_clicked();
	void on_pushButton_saveCfg_clicked();
	void on_load_config(const QString &sPath);
	void on_save_config(const QString &sPath);
	void load_json_config(QJsonDocument &doc);
	void save_json_config(QJsonDocument &doc);
	void OnHttpLoadSettings(QString query, QByteArray postdata, QJsonDocument *doc);

	void FollowGameWnd(long left, long top, long right, long bottom);
	void SaveLoginBat(int type);
	void OnNotifyLoginProgressStart();
	void OnNotifyLoginProgressEnd();
	void on_switch_ui_tab();

private:
	Ui::MLAssistClass ui;
	bool m_bmove;
	QPoint m_pressedpoint;
	QPoint m_windowspoint;
	QPoint m_oldmousepoint;

	bool m_bIsInLoginProgress = false;		//是否登录中
	QAction *m_pMinimizeAction;				//最小号按钮
	QAction *m_pMaximizeAction;				//最大化按钮
	QAction *m_pRestoreAction;				//恢复按钮
	QAction *m_pQuitAction;					//退出按钮
	QSystemTrayIcon *m_pTrayIcon = nullptr; //系统托盘图标
	QMenu *m_pTrayIconMenu = nullptr;		//系统托盘菜单
	QPushButton *m_pLoadBtn = nullptr;
	QPushButton *m_pSaveBtn = nullptr;
	QString m_lastOpenCfgPath;
};
/*
经验值：EXP（Experience）
战斗力：CE（Combat Effectiveness)
血量：HP(Health Point）
魔力值：MP(Magic Point)
级别 / 级数：LV(Level)
普通金钱：Cash / Money / Game Point(常用图标代替)
黄金：Gold(常用图标代替)
钻石：Diamond(常用图标代替)
武器：Weapon
主武器：Primary Weapon
副武器：Secondary Weapon
伤害：Damage
杀敌：Kill
弹药：AMMO / Ammunition
爆头：Head Shot
*/