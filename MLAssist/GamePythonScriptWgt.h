#pragma once

#include <QWidget>
#include "ui_GamePythonScriptWgt.h"
//#include "PythonQt.h"
//#include "PythonQt_QtAll.h"
//#include "gui/PythonQtScriptingConsole.h"
class GamePythonScriptWgt : public QWidget
{
	Q_OBJECT

public:
	GamePythonScriptWgt(QWidget *parent = Q_NULLPTR);
	~GamePythonScriptWgt();
	
public slots:
	void on_pushButton_load_clicked();
	void on_pushButton_stop_clicked();
private:
	Ui::GamePythonScriptWgt ui;
	//PythonQtObjectPtr m_pythonMainModule;
	//PythonQtScriptingConsole* m_pPythonConsole;
	//PythonQtObjectPtr m_scriptCode;
	bool m_bStop = false;
};
