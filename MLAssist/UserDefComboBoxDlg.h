#pragma once

#include "ui_UserDefComboBoxDlg.h"
#include <QDialog>

class UserDefComboBoxDlg : public QDialog
{
	Q_OBJECT

public:
	UserDefComboBoxDlg(QWidget *parent = Q_NULLPTR);
	~UserDefComboBoxDlg();
	void setLabelText(const QString &sVal);
	void setComboBoxItems(QStringList sItems);
	void setComboBoxItems(QMap<QString, int> tItems);
	void setCurrentSelect(const QString &sItem);
	QString getVal();
signals:
	void signal_input_val(const QVariant &sVal);

public slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();

private:
	Ui::UserDefComboBoxDlg ui;
	QString m_sDefaultVal;
};
