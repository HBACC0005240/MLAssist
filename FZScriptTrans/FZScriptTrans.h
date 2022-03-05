#pragma once

#include <QtWidgets/QDialog>
#include "ui_FZScriptTrans.h"

class FZScriptTrans : public QDialog
{
	Q_OBJECT

public:
	FZScriptTrans(QWidget* parent = Q_NULLPTR);



public slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_openFz_clicked();
	void on_pushButton_saveML_clicked();
	void on_pushButton_trans_clicked();
	void on_pushButton_replaceRex_clicked();
	void on_textEdit_cursorPositionChanged();
	void on_textEdit_2_cursorPositionChanged();

private:
	Ui::FZScriptTransClass ui;
	QString m_szFzPath;
	QString m_szSaveDir;
	QString m_operationData;

};
