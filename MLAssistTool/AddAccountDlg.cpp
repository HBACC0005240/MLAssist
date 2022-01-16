#include "AddAccountDlg.h"
#include <QMessageBox>
#include "ITObjectDataMgr.h"
#include "ITObject.h"
AddAccountDlg::AddAccountDlg(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

AddAccountDlg::~AddAccountDlg()
{
}

void AddAccountDlg::on_pushButton_clicked()
{
	QString sUser = ui.lineEdit->text();
	QString sUserPwd = ui.lineEdit_2->text();
	QString sShowText = ui.lineEdit_3->text();
	if (sShowText.isEmpty())
	{
		sShowText = sUser;
	}
	if (sUser.isEmpty())
	{
		QMessageBox::information(this, "提示：", "登录账号不能为空!");
		return;
	}
	auto pObj = ITObjectDataMgr::getInstance().newOneObject(TObject_Account, _accountAssemblePtr);
	if (pObj)
	{
		pObj->setObjectName(sShowText);
		ITAccountPtr pAccount = qSharedPointerCast<ITAccount>(pObj);
		pAccount->_userName = sUser;
		pAccount->_passwd = sUserPwd;
	}
	accept();
}

void AddAccountDlg::on_pushButton_2_clicked()
{
	reject();
}
