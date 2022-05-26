#pragma once

#include "ui_UserDefCheckBoxDlg.h"
#include <QDialog>

class UserDefCheckBoxDlg : public QDialog
{
	Q_OBJECT

public:
	UserDefCheckBoxDlg(QWidget *parent = Q_NULLPTR);
	~UserDefCheckBoxDlg();
	void setLabelText(const QString &sVal);
	void setDefaultVal(const QString &sVal);
	QString getVal();
signals:
	void signal_input_val(const QVariant &sVal);

public slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();

private:
	Ui::UserDefCheckBoxDlg ui;
	QString m_sDefaultVal;
};
