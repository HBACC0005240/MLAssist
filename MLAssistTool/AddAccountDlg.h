#pragma once

#include <QDialog>
#include "ui_AddAccountDlg.h"
#include "ITObject.h"

class AddAccountDlg : public QDialog
{
	Q_OBJECT

public:
	AddAccountDlg(QWidget* parent = Q_NULLPTR);
	~AddAccountDlg();

	void SetAccountAssemblePtr(ITAccountIdentityPtr pAassem) { _accountAssemblePtr = pAassem; }
public slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();

private:
	Ui::AddAccountDlg ui;
	ITAccountIdentityPtr _accountAssemblePtr;
};
