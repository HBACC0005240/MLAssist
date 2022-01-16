#pragma once

#include "ui_PetDoubleActionSetDlg.h"
#include <QDialog>

class PetDoubleActionSetDlg : public QDialog
{
	Q_OBJECT

public:
	PetDoubleActionSetDlg(QWidget *parent = Q_NULLPTR);
	~PetDoubleActionSetDlg();

public slots:
	void on_checkBox_attack1_stateChanged(int state);
	void on_checkBox_attack2_stateChanged(int state);
	void on_pushButton_attack1_clicked();
	void on_pushButton_attack2_clicked();
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();

private:
	Ui::PetDoubleActionSetDlg ui;
};
