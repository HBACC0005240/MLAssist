#pragma once

#include <QWidget>
#include "ui_GameMapWgt.h"

class GameMapWgt : public QWidget
{
	Q_OBJECT

public:
	GameMapWgt(QWidget *parent = Q_NULLPTR);
	~GameMapWgt();

private:
	Ui::GameMapWgt ui;
};
