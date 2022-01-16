#pragma once

#include <QWidget>
#include "ui_GameScriptWgt.h"
#include "CGLuaFun.h"
class GameScriptWgt : public QWidget
{
	Q_OBJECT

public:
	GameScriptWgt(QWidget *parent = Q_NULLPTR);
	~GameScriptWgt();

	void initTableWidget();
public slots:
	void on_pushButton_pause_clicked();
	void on_pushButton_open_clicked();
	void on_pushButton_openEncrypt_clicked();
	void on_pushButton_start_clicked();
	void on_pushButton_stop_clicked();
	void doUpdateScriptRow(int row);
	void on_customContextMenu(const QPoint& pos);
	void gotoScriptRow();
	void onUpdateUI();
	void BeginTraceScriptRun();
private:
	Ui::GameScriptWgt ui;
	int m_currentRow;
	bool m_bTrace = false;
};
