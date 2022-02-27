#pragma once

#include <QWidget>
#include "ui_MLDebugWgt.h"

class MLDebugWgt : public QWidget
{
	Q_OBJECT

public:
	MLDebugWgt(QWidget *parent = Q_NULLPTR);
	~MLDebugWgt();

public slots:
	void doAddOneDebugMsg(const QString& msg);

private:
	Ui::MLDebugWgt ui;
};
