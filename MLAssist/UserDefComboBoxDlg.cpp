#include "UserDefComboBoxDlg.h"
#include <QListView>

UserDefComboBoxDlg::UserDefComboBoxDlg(QWidget *parent) :
		QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui.comboBox->setView(new QListView());
}

UserDefComboBoxDlg::~UserDefComboBoxDlg()
{
}

void UserDefComboBoxDlg::setLabelText(const QString &sVal)
{
	ui.label->setText(sVal);
}

void UserDefComboBoxDlg::setComboBoxItems(QStringList sItems)
{
	if (sItems.size() < 1)
		return;
	ui.comboBox->clear();
	for (auto stext : sItems)
	{
		ui.comboBox->addItem(stext);
	}
	m_sDefaultVal = sItems.at(0);
}

void UserDefComboBoxDlg::setComboBoxItems(QMap<QString, int> sItems)
{
	if (sItems.size() < 1)
		return;
	ui.comboBox->clear();
	for (auto it = sItems.begin(); it != sItems.end(); ++it)
	{
		ui.comboBox->addItem(it.key(), it.value());
	}
	m_sDefaultVal = sItems.first();
}

void UserDefComboBoxDlg::setCurrentSelect(const QString &sItem)
{
	ui.comboBox->setCurrentIndex(ui.comboBox->findText(sItem));
}

QString UserDefComboBoxDlg::getVal()
{
	return ui.comboBox->currentText();
}

void UserDefComboBoxDlg::on_pushButton_clicked()
{
	emit signal_input_val(getVal());
	accept();
}

void UserDefComboBoxDlg::on_pushButton_2_clicked()
{
	emit signal_input_val(m_sDefaultVal);
	reject();
}
