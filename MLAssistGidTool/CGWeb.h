#pragma once

#include <QWidget>
#include <QTimer>
#include "ui_CGWeb.h"
class MyPage :public QWebEnginePage
{
	Q_OBJECT
public:
	MyPage();
protected:
	virtual void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& msg, int lineNumber, const QString& srcID)override;

};
class CGWeb : public QWidget
{
	Q_OBJECT

public:
	CGWeb(QWidget* parent = Q_NULLPTR);
	~CGWeb();

	void rotateImages(bool invert);
protected:
	void judgePageUrl();

public slots:
	void finishLoading(bool);
	void on_pushButton_2_clicked();
	void replyFinished(QNetworkReply*);
	void onUrlChanged(const QUrl& url);
	void onCheckCreateGid();
	void on_comboBox_type_currentIndexChanged(int index);
	void on_pushButton_myScript_clicked();
private:
	Ui::CGWeb ui;
	QTimer m_createGidTimer;
	QTime m_lastCreateGidTime;	//最后一次生产gid时间
	QString jQuery;
	QLineEdit* locationEdit;
	QAction* rotateAction;
	int m_precisionNum = 3;		//几位数拼接
	int m_nBeginNum = 0;
	int progress;
	bool m_bTele = true;			//电信
	bool m_bCreateGid = false;		//生成账号游戏id
	bool m_bCreateAccount = false;	//生成账号
	int m_nGameType = 13;
	QMap<int, QStringList> m_gameTypeForUrl;	//区服和链接
};
