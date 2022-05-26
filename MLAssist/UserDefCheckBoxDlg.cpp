#include "UserDefCheckBoxDlg.h"
#include <QListView>

UserDefCheckBoxDlg::UserDefCheckBoxDlg(QWidget *parent) :
		QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

UserDefCheckBoxDlg::~UserDefCheckBoxDlg()
{
}

void UserDefCheckBoxDlg::setLabelText(const QString &sVal)
{
	ui.checkBox->setText(sVal);
}

void UserDefCheckBoxDlg::setDefaultVal(const QString &sVal)
{
	ui.checkBox->setChecked(sVal.toInt());
}


QString UserDefCheckBoxDlg::getVal()
{
	return ui.checkBox->isChecked();
}

void UserDefCheckBoxDlg::on_pushButton_clicked()
{
	emit signal_input_val(getVal());
	accept();
}

void UserDefCheckBoxDlg::on_pushButton_2_clicked()
{
	emit signal_input_val(m_sDefaultVal);
	reject();
}
