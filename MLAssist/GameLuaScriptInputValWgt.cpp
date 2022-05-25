#include "GameLuaScriptInputValWgt.h"
#include "GameCtrl.h"
#include "constDef.h"
#include "stdafx.h"
#include "CGFunction.h"
GameLuaScriptInputValWgt::GameLuaScriptInputValWgt(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	m_pGridLayout = new QGridLayout;
	ui.widget->setLayout(m_pGridLayout);
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
	m_pKeyForWidget.clear();
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
			m_pKeyForWidget.insert(sMsg, qMakePair(type, pEdit));
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
			m_pKeyForWidget.insert(sMsg, qMakePair(type, pBox));
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
			m_pKeyForWidget.insert(sMsg,qMakePair(type,pBox));
			break;
		}
		default:
			break;
	}
}

void GameLuaScriptInputValWgt::on_pushButton_save_clicked()
{
	for (auto it=m_pKeyForWidget.begin();it!=m_pKeyForWidget.end();++it)
	{
		switch (it.value().first)
		{
			case TInputType_Edit:
			{
				QLineEdit *pEdit = (QLineEdit *)it.value().first;
				g_pGameFun->SetScriptUiSetData(it.key().toString(), pEdit->text());
				break;
			}
			case TInputType_ComboBox:
			{
				QComboBox *pBox = (QComboBox *)it.value().first;
				g_pGameFun->SetScriptUiSetData(it.key().toString(), pBox->currentText());
				break;
			}
			case TInputType_CheckBox:
			{
				QCheckBox *pBox = (QCheckBox *)it.value().first;
				g_pGameFun->SetScriptUiSetData(it.key().toString(), pBox->isChecked());
				break;
			}
			default:
				break;
		}
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
