#include "GameMapWgt.h"
#include "GameOftenCoordinateWgt.h"

GameMapWgt::GameMapWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_pOftenCoordinateWgtWgt = new GameOftenCoordinateWgt;
	m_pFloatCustomBattleDlg = new QDialog;
	m_pFloatCustomBattleDlg->setMinimumSize(640, 600);
	m_pFloatCustomBattleDlg->setWindowFlags(m_pFloatCustomBattleDlg->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	m_pFloatCustomBattleDlg->setWindowTitle("常用地图");
	m_pVBoxLayout = new QVBoxLayout(m_pFloatCustomBattleDlg);
	m_pFloatCustomBattleDlg->setLayout(m_pVBoxLayout);
	connect(m_pFloatCustomBattleDlg, &QDialog::finished, this, [&](int code)
			{
				ui.tabWidget->addTab(m_pOftenCoordinateWgtWgt, "常用地图");
				ui.tabWidget->setCurrentWidget(m_pOftenCoordinateWgtWgt);
			});

	connect(m_pOftenCoordinateWgtWgt, SIGNAL(signal_float_window()), this, SLOT(deal_float_widget()));
	ui.tabWidget->addTab(m_pOftenCoordinateWgtWgt, "常用地图");
}

GameMapWgt::~GameMapWgt()
{
}

void GameMapWgt::deal_float_widget()
{
	if (ui.tabWidget->count() >= 2)
	{
		ui.tabWidget->removeTab(1);
		m_pVBoxLayout->addWidget(m_pOftenCoordinateWgtWgt);
		m_pOftenCoordinateWgtWgt->show();
		m_pFloatCustomBattleDlg->show();
	}
	else
	{
		m_pFloatCustomBattleDlg->hide();
		ui.tabWidget->addTab(m_pOftenCoordinateWgtWgt, "常用地图");
		ui.tabWidget->setCurrentWidget(m_pOftenCoordinateWgtWgt);
	}
}
