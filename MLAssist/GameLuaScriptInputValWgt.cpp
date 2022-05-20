#include "GameLuaScriptInputValWgt.h"
#include "GameCtrl.h"
#include "constDef.h"
#include "stdafx.h"
GameLuaScriptInputValWgt::GameLuaScriptInputValWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	m_pGridLayout = new QGridLayout;
	setLayout(m_pGridLayout);
	connect(g_pGameCtrl, &GameCtrl::signal_addOneScriptInputVar, this, &GameLuaScriptInputValWgt::AddOneInputVar);
}

GameLuaScriptInputValWgt::~GameLuaScriptInputValWgt()
{
}

void GameLuaScriptInputValWgt::ClearAllInputWidget()
{
	if (m_pAllInputWidget.size() > 0)
	{
		for (auto pWidget : m_pAllInputWidget)
		{
			m_pGridLayout->removeWidget(pWidget);
			SafeDelete(pWidget);
		}
		m_pAllInputWidget.clear();
	}
	m_lastRow = 0;
	m_lastCol = 0;
}

void GameLuaScriptInputValWgt::AddOneInputVar(int type, const QVariant &sMsg, const QVariant &vData, const QVariant &vData2)
{
	switch (type)
	{
		case TInputType_Edit:
		{
			QWidget *pWidget = new QWidget();
			QLabel *pLabel = new QLabel(sMsg.toString(), pWidget);
			//pLabel->setMinimumSize(50, 26);
			QLineEdit *pEdit = new QLineEdit(pWidget);
			//pEdit->setMinimumSize(100, 20);
			pEdit->setText(vData.toString());
			QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
			pLayout->addWidget(pLabel);
			pLayout->addWidget(pEdit);
			pLayout->addStretch();
			pLayout->setContentsMargins(0, 0, 0, 0);
			pWidget->setLayout(pLayout);
			m_pGridLayout->addWidget(pWidget, m_lastRow, m_lastCol);
			increaseRowCol();
			m_pAllInputWidget.append(pWidget);
			break;
		}
		case TInputType_ComboBox:
		{
			QWidget *pWidget = new QWidget();
			QLabel *pLabel = new QLabel(sMsg.toString(), pWidget);
			//pLabel->setMinimumSize(50, 26);
			QComboBox *pBox = new QComboBox(pWidget);
			//pEdit->setMinimumSize(100, 20);
			pBox->addItems(vData.toStringList());
			pBox->setCurrentIndex(pBox->findText(vData2.toString()));
			QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
			pLayout->addWidget(pLabel);
			pLayout->addWidget(pBox);
			pLayout->addStretch();
			pLayout->setContentsMargins(0, 0, 0, 0);
			pWidget->setLayout(pLayout);
			m_pGridLayout->addWidget(pWidget, m_lastRow, m_lastCol);
			increaseRowCol();
			m_pAllInputWidget.append(pWidget);
			break;
		}
		case TInputType_CheckBox:
		{
			QWidget *pWidget = new QWidget();
			QCheckBox *pBox = new QCheckBox(sMsg.toString(), pWidget);
			QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
			pLayout->addWidget(pBox);
			pLayout->addStretch();
			pLayout->setContentsMargins(0, 0, 0, 0);
			pWidget->setLayout(pLayout);
			m_pGridLayout->addWidget(pWidget, m_lastRow, m_lastCol);
			increaseRowCol();
			m_pAllInputWidget.append(pWidget);
			break;
		}
		default:
			break;
	}
}

void GameLuaScriptInputValWgt::increaseRowCol()
{
	if (m_lastCol >= 1)
	{
		m_lastCol = 0;
		m_lastRow++;
	}
	else
		m_lastCol++;
}
