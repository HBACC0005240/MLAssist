#pragma once

#include <QtWidgets/QDialog>
#include "ui_MLAssistGidTool.h"

class MLAssistGidTool : public QDialog
{
    Q_OBJECT

public:
    MLAssistGidTool(QWidget *parent = Q_NULLPTR);

private:
    Ui::MLAssistGidToolClass ui;
};
