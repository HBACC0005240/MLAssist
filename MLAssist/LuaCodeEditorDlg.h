#pragma once

#include <QDialog>
#include "LuaCodeEditor.h"
#include "ui_LuaCodeEditorDlg.h"

class LuaCodeEditorDlg : public QDialog
{
	Q_OBJECT

public:
	LuaCodeEditorDlg(QWidget *parent = Q_NULLPTR);
	~LuaCodeEditorDlg();

	LuaCodeEditor *GetLuaCodeEditor() { return ui.plainTextEdit; }

	void SetOpenLuaScriptPath(const QString &sPath);

public slots:
	void on_toolButton_open_clicked();
	void on_toolButton_save_clicked();
	void on_toolButton_saveAs_clicked();

protected:
	bool openScriptFile(const QString& sPath);

private:
	Ui::LuaCodeEditorDlg ui;
	QString m_sOpenPath;
	QString m_operationData;
};
