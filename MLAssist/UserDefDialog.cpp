#include "UserDefDialog.h"

UserDefDialog::UserDefDialog(QWidget *parent) :
		QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

UserDefDialog::~UserDefDialog()
{
}

void UserDefDialog::setLabelText(const QString &sVal)
{
	ui.label->setText(sVal);
}

void UserDefDialog::setDefaultVal(const QString &sVal)
{
	m_sDefaultVal = sVal;
	ui.lineEdit->setText(sVal);
}

QString UserDefDialog::getVal()
{
	return ui.lineEdit->text();
}

void UserDefDialog::on_pushButton_clicked()
{
	emit signal_input_val(getVal());
	accept();
}

void UserDefDialog::on_pushButton_2_clicked()
{
	emit signal_input_val(m_sDefaultVal);
	reject();
}
