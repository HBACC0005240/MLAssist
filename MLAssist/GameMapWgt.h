#pragma once

#include <QWidget>
#include "ui_GameMapWgt.h"

class GameOftenCoordinateWgt;
class GameMapWgt : public QWidget
{
	Q_OBJECT

public:
	GameMapWgt(QWidget *parent = Q_NULLPTR);
	~GameMapWgt();

public slots:
	void deal_float_widget();

private:
	Ui::GameMapWgt ui;
	QDialog *m_pFloatCustomBattleDlg;
	QVBoxLayout *m_pVBoxLayout;
	GameOftenCoordinateWgt *m_pOftenCoordinateWgtWgt;
};
