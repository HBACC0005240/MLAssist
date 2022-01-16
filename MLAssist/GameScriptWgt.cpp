#include "stdafx.h"
#include "GameScriptWgt.h"
#include <QFileDialog>
#include "FZParseScript.h"
#include <QTableWidgetItem>
GameScriptWgt::GameScriptWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QStringList saveHeadList;
	saveHeadList << ("序号") << ("脚本内容");
	ui.tableWidget->setColumnCount(saveHeadList.size());
	ui.tableWidget->setHorizontalHeaderLabels(saveHeadList);
	ui.tableWidget->horizontalHeader()->setStyleSheet("font:bold;");
	//	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
	//	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	//	ui.tableWidget->horizontalHeader()->setFixedHeight(30);
	ui.tableWidget->setColumnWidth(0, 30);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(&FZParseScript::getInstance(),SIGNAL(updateScriptRow(int)),this,SLOT(doUpdateScriptRow(int)));
	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_customContextMenu(const QPoint&)));
	connect(&FZParseScript::getInstance(), SIGNAL(refreshScriptUI()), this, SLOT(onUpdateUI()));
}

GameScriptWgt::~GameScriptWgt()
{
}
void GameScriptWgt::on_customContextMenu(const QPoint& pos)
{
	QTableWidgetItem* tempitem = ui.tableWidget->itemAt(pos);
	if (tempitem != NULL)
	{
		m_currentRow = ui.tableWidget->row(tempitem);   //
		QMenu menu;
		menu.addAction(QString("跳到"), this, SLOT(gotoScriptRow()));
		menu.addAction(QString("开始跟踪"), this, SLOT(BeginTraceScriptRun()));

		menu.exec(QCursor::pos());
	}
}


void GameScriptWgt::gotoScriptRow()
{
	FZParseScript::getInstance().SetScriptRunRow(m_currentRow);
}

void GameScriptWgt::onUpdateUI()
{
	if (FZParseScript::getInstance().GetGameScriptCtrlStatus() == SCRIPT_CTRL_PAUSE)
	{		
		ui.pushButton_pause->setText("继续");
	}
	else if(FZParseScript::getInstance().GetGameScriptCtrlStatus() == SCRIPT_CTRL_RUN)
	{
		ui.pushButton_pause->setText("暂停");
		ui.pushButton_start->setEnabled(false);
	}
	else if (FZParseScript::getInstance().GetGameScriptCtrlStatus() == SCRIPT_CTRL_STOP)
	{
		ui.pushButton_pause->setText("暂停");
		ui.pushButton_start->setEnabled(true);
	}

}

void GameScriptWgt::BeginTraceScriptRun()
{
}

void GameScriptWgt::initTableWidget()
{
	QStringList szScripts = FZParseScript::getInstance().GetGameScriptData();
	int rowCounts = ui.tableWidget->rowCount();
	int colCounts = ui.tableWidget->columnCount();
	int nWidgetCol = -1;
	for (int i = 0; i < rowCounts - 1; i++)
	{
		for (int j = 0; j < colCounts; j++)
		{
			QTableWidgetItem* tableitem = ui.tableWidget->item(i, j);
			if (tableitem != NULL)
				delete tableitem;
		}	
		ui.tableWidget->removeRow(0);
	}
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(szScripts.size());
	for (int i=0;i<szScripts.size();++i)
	{
		QTableWidgetItem* pItem = new QTableWidgetItem(QString::number(i+1));
		ui.tableWidget->setItem(i, 0, pItem);
		pItem = new QTableWidgetItem(szScripts.at(i));
		ui.tableWidget->setItem(i, 1, pItem);
	}
}

void GameScriptWgt::on_pushButton_pause_clicked()
{
	if (FZParseScript::getInstance().GetGameScriptCtrlStatus() != SCRIPT_CTRL_PAUSE)
	{
		FZParseScript::getInstance().PauseScript();
		ui.pushButton_pause->setText("继续");
	}
	else
	{
		FZParseScript::getInstance().RunScript();
		ui.pushButton_pause->setText("暂停");
	}
}

void GameScriptWgt::on_pushButton_open_clicked()
{
	QString szPath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开"), "./", "*.script");//*.script;
	if (szPath.isEmpty())
		return;
	ui.pushButton_start->setEnabled(true);
	ui.pushButton_pause->setText("暂停");	
	FZParseScript::getInstance().StopScript();
	FZParseScript::getInstance().ParseGameScript(szPath);
	ui.textEdit->setText(FZParseScript::getInstance().getGameDescript());
	initTableWidget();	
}

void GameScriptWgt::on_pushButton_openEncrypt_clicked()
{
	QString szPath = QFileDialog::getOpenFileName(this, QString::fromLatin1("打开"), "./", "*.script;*.lua");
	if (szPath.isEmpty())
		return;
	ui.pushButton_start->setEnabled(true);
	ui.pushButton_pause->setText("暂停");
	FZParseScript::getInstance().StopScript();
	FZParseScript::getInstance().ParseGameScript(szPath);
		
}

void GameScriptWgt::on_pushButton_start_clicked()
{
	FZParseScript::getInstance().RunScript();
	ui.pushButton_start->setEnabled(false);
}

void GameScriptWgt::on_pushButton_stop_clicked()
{
	FZParseScript::getInstance().StopScript();
	ui.pushButton_start->setEnabled(true);
}

void GameScriptWgt::doUpdateScriptRow(int row)
{
	m_currentRow = row;
	ui.tableWidget->selectRow(row);
//	ui.tableWidget->setCurrentCell(row,0);
	ui.label_curRow->setText(QString("所在行:%1").arg(row+1));
}
