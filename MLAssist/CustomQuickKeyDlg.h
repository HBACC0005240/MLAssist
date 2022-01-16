#pragma once

#include <QDialog>
#include "ui_CustomQuickKeyDlg.h"
#include <QKeySequence>
class CustomQuickKeyDlg : public QDialog
{
	Q_OBJECT

public:
	CustomQuickKeyDlg(QWidget *parent = Q_NULLPTR);
	~CustomQuickKeyDlg();

	QString GetInputKey();
public slots:
	//void truncateShortcut();
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();

private:
	Ui::CustomQuickKeyDlg ui;
};
