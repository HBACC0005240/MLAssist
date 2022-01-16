#pragma once

#include "ui_UserDefDialog.h"
#include <QDialog>

class UserDefDialog : public QDialog
{
	Q_OBJECT

public:
	UserDefDialog(QWidget *parent = Q_NULLPTR);
	~UserDefDialog();

	void setLabelText(const QString &sVal);
	void setDefaultVal(const QString &sVal);
	QString getVal();
signals:
	void signal_input_val(const QVariant &sVal);

public slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();

private:
	Ui::UserDefDialog ui;
	QString m_sDefaultVal;
};
