#pragma once

#include "GameCtrl.h"
#include "ui_GameChatWgt.h"
#include <QMutex>
#include <QSettings>
#include <QTimer>
#include <QWidget>

class GameChatWgt : public QWidget
{
	Q_OBJECT

public:
	GameChatWgt(QWidget *parent = Q_NULLPTR);
	~GameChatWgt();

	void init();
	void addOneChatData(const QString &chatData, int nType = 0, int color = 0);
	void doGameChat(const QString &chatData);
public slots:
	void doUpdateGameChat();
	void on_comboBox_OftenMsg_currentIndexChanged(int index);
	void on_pushButton_AddOftenMsg_clicked();
	void on_pushButton_chat_clicked();
	void on_pushButton_clearSendChat_clicked();
	void on_pushButton_mail_clicked();
	void doAddOneChat(const QString &chatData);
	void OnNotifyChatMsg(int unitid, QString msg, int size, int color);
	void on_lineEdit_chat_returnPressed();
	void on_lineEdit_chatCount_editingFinished();

	void OnCtrlTimer(int nFlag);
	void OnTimeChat();
	void doLoadUserConfig(QSettings &iniFile);
	void doSaveUserConfig(QSettings &iniFile);
	void doLoadJsConfig(QJsonObject &obj);
	void doSaveJsConfig(QJsonObject &obj);
	void on_comboBox_chatColor_currentIndexChanged(int index);
	void on_radioButton_big_toggled(bool checked);
	void on_radioButton_mid_toggled(bool checked);
	void on_radioButton_small_toggled(bool checked);
	void OnUpdateFriendCard();
	void doItemDoubleClicked(QListWidgetItem *);
	void OnNotifyGetPlayerInfo(CharacterPtr charinfo);
	void on_listWidget_customContextMenuRequested(const QPoint &);
	void on_checkBox_BlockChatMsgs_stateChanged(int state);
	void OnNotifyFillChatSettings(int blockchatmsgs);

private:
	Ui::GameChatWgt ui;
	int m_chatStoreCount = 100;
	QMutex m_tableMutex;
	QHash<QString, QListWidgetItem *> m_cardForItem;
	QTimer m_friendCardTimer;
	QTimer m_timer;
	int m_nLastIndex;		   //上次记录
	int m_nChatSize = 0;	   //大2  中0  小1
	int m_nChatColorIndex = 0; //颜色index
	CharacterPtr m_player;
};
