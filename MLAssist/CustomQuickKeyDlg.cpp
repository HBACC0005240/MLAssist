#include "CustomQuickKeyDlg.h"

CustomQuickKeyDlg::CustomQuickKeyDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//connect(ui.keySequenceEdit, &QKeySequenceEdit::editingFinished,
	//		this, &CustomQuickKeyDlg::truncateShortcut);	
}

CustomQuickKeyDlg::~CustomQuickKeyDlg()
{
}
//void CustomQuickKeyDlg::truncateShortcut()
//{
//	int value = ui.keySequenceEdit->keySequence()[0];
//	QKeySequence shortcut(value);
//	ui.keySequenceEdit->setKeySequence(shortcut);
//}
QString CustomQuickKeyDlg::GetInputKey()
{
	return ui.keySequenceEdit->keySequence().toString();
}

void CustomQuickKeyDlg::on_pushButton_clicked()
{
	accept();
}

void CustomQuickKeyDlg::on_pushButton_2_clicked()
{
	reject();
}
