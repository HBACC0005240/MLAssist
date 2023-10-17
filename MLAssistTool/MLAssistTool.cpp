#include "MLAssistTool.h"
#include "YunLai.h"
#include <QSettings>
#include <QTextCodec>
#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include "RpcSocket.h"
#include "ITObjectDataMgr.h"

MLAssistTool::MLAssistTool(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(&m_refreshTimer, SIGNAL(timeout()), this, SLOT(onRefreshFzData()));
	init();
}

MLAssistTool::~MLAssistTool()
{
	ITObjectDataMgr::getInstance().SetExitState(true);
	RpcSocket::getInstance().fini();
}

void MLAssistTool::init()
{
	RpcSocket::getInstance();

	ui.comboBox->clear();
	QStringList comboBoxItems;
	comboBoxItems << "5" << "10" << "30" << "60" << "300" << "600";
	for (auto sItem : comboBoxItems)
	{
		ui.comboBox->addItem(QString("%1秒").arg(sItem), sItem.toInt());
	}
	ui.comboBox->setCurrentIndex(3);
	m_updateTime = ui.comboBox->currentData().toInt() * 1000;
	m_refreshTimer.start(m_updateTime);
	initTable();
	onRefreshFzData();
	QString sPath = QCoreApplication::applicationDirPath() + "//MLAssistTool.ini";
	QSettings iniFile(sPath, QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("UTF-8")); //这样分组下的键值可以读取中文  下面的是读取段的中文
	m_oneKeyStartBat = iniFile.value("config/allRun").toString();

}

void MLAssistTool::initTable()
{
	m_model = new FzTableModel(ui.tableView_fz);
	ui.tableView_fz->setModel(m_model);
	ui.tableView_fz->setColumnWidth(0, 40);
	ui.tableView_fz->setColumnWidth(1, 70);
	ui.tableView_fz->setColumnWidth(2, 350);
	ui.tableView_fz->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
	ui.tableView_fz->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Fixed);
	ui.tableView_fz->setSelectionBehavior(QAbstractItemView::SelectRows);
	//ui.tableView_fz->setSelectionMode(QAbstractItemView::SingleSelection);
	
	
	//ui.tableView_fz->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
	m_mlModl = new MLWndTableModel(ui.tableView_ml);
	ui.tableView_ml->setModel(m_mlModl);
	ui.tableView_ml->setColumnWidth(0, 40);
	ui.tableView_ml->setColumnWidth(1, 70);
	ui.tableView_ml->setColumnWidth(2, 350);
	ui.tableView_ml->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
	ui.tableView_ml->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Fixed);
	ui.tableView_ml->setSelectionBehavior(QAbstractItemView::SelectRows);
//	ui.tableView_ml->setSelectionMode(QAbstractItemView::SingleSelection);	

	//ui.tableView_ml->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
}

bool MLAssistTool::isInGame(DWORD pid)
{
	void* pBaseAddr = YunLai::GetProcessImageBase1(pid);
	//qDebug() << pBaseAddr;
	//0xBDBA78  0xE1E000  0xE1DFFC  GetWorldStatus() != 11 && GetWorldStatus() != 2;
	int inGame = YunLai::ReadMemoryIntFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xBDBA78); //
	int wordStatus = YunLai::ReadMemoryIntFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xE1E000); //
	int gameStatus = YunLai::ReadMemoryIntFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xE1DFFC); //
	return inGame && wordStatus != 11/* && gameStatus != 2*/;
}

void MLAssistTool::ctrlAllState(int val)
{
	auto fzItems = m_model->GetFzTableItemList();
	for (auto item : fzItems)
	{
		HWND itemHwnd = (HWND)item->m_hWnd;
		YunLai::SetWindowVal(itemHwnd, val);
	}
	auto mlItems = m_mlModl->GetFzTableItemList();
	for (auto item : mlItems)
	{
		HWND itemHwnd = (HWND)item->m_hWnd;
		YunLai::SetWindowVal(itemHwnd, val);
	}
	onRefreshFzData();
}

void MLAssistTool::ctrlAllStateTab(int index, int val)
{
	if (index == 0)
	{
		auto fzItems = m_model->GetFzTableItemList();
		for (auto item : fzItems)
		{
			HWND itemHwnd = (HWND)item->m_hWnd;
			YunLai::SetWindowVal(itemHwnd, val);
		}
	}
	else if (index == 1)
	{
		auto mlItems = m_mlModl->GetFzTableItemList();
		for (auto item : mlItems)
		{
			HWND itemHwnd = (HWND)item->m_hWnd;
			YunLai::SetWindowVal(itemHwnd, val);
		}
	}
	onRefreshFzData();
}

bool MLAssistTool::checkKillHungGame(HWND wnd, DWORD pid)
{
	if (m_gameWndForLastHungTime.contains(wnd) && (GetTickCount() - m_gameWndForLastHungTime.value(wnd) > 30000))
	{
		qDebug() << "程序30秒未响应,结束进程！" << QString::number(pid);
		YunLai::KillProcess(wnd);
		return true;
	}
	return false;
}

void MLAssistTool::statisticsGameState()
{
	auto fzList = m_model->GetFzTableItemList();
	ui.label_fzCount->setText(QString::number(fzList.size()));
	//ui.label_fzOnline->setText(QString::number(mlOnline));
	//ui.label_fzOffline->setText(QString::number(mlOffline));

	auto mlList = m_mlModl->GetFzTableItemList();
	int mlOnline = 0;
	int mlOffline = 0;
	for (auto tmpMl : mlList)
	{
		if (tmpMl->m_nState == 0)
			mlOffline++;
		else if (tmpMl->m_nState == 1)
			mlOnline++;
	}
	ui.label_mlOffLine->setText(QString::number(mlOffline));
	ui.label_mlOnline->setText(QString::number(mlOnline));
	ui.label_mlCount->setText(QString::number(mlList.size()));
}

void MLAssistTool::on_pushButton_refresh_clicked()
{
	onRefreshFzData();
}

void MLAssistTool::onRefreshFzData()
{
	const char szFindFZGameClass[] = "MLAssist.exe";
	const char szFindGameClass[] = "cg_item_6000.exe";

	HWND hWnd = NULL;
	DWORD pid, tid;
	WCHAR szText[256];
	FzTableItemList list, mlList;

	QMap<qint64, QString> processData;
	YunLai::GetAllProcess(processData);

	for (auto it = processData.begin(); it != processData.end(); ++it)
	{
		QString sVal = it.value();
		//qDebug() << sVal;
		if (it.value() == (szFindFZGameClass))
		{
			//qDebug() << sVal;

			memset(szText, 0, 256);
			pid = it.key();
			hWnd = YunLai::FindMainWindow(pid);
			//	GetClassNameW(hWnd, szText, 256);
			//	qDebug() << QString::fromWCharArray(szText);
			if (GetWindowTextW(hWnd, szText, 256))
			{
				/*		WINDOWPLACEMENT wp;
						wp.length = sizeof(WINDOWPLACEMENT);
						GetWindowPlacement(hWnd, &wp);*/
				bool bHung = IsHungAppWindow(hWnd);
				bool bShowTable = true;
				int nState = 1; //正常
				if (bHung)
				{
					nState = -1;	//挂起
					if (!checkKillHungGame(hWnd, pid))
						m_gameWndForLastHungTime.insert(hWnd, GetTickCount());
					else
						bShowTable = false;	//结束进程的 不显示
				}
				long szLng = GetWindowLong(hWnd, GWL_STYLE);	//    '取的窗口原先的样式				
			//	SetWindowLong(dstHwnd, GWL_EXSTYLE, rtn);		//     '把新的样式赋给窗体
				auto wndTitle = QString::fromWCharArray(szText);
				FzTableItemPtr item(new FzTableItem((quint32)pid, (quint32)0, (quint32)hWnd, wndTitle, nState, szLng));//wp.showCmd));
				list.append(item);
			}
		}
		//else if (it.value() == szFindGameClass)
		//{
		//	qDebug() << sVal;

		//	memset(szText, 0, 256);
		//	pid = it.key();
		//	hWnd = YunLai::FindMainWindow(pid);
		//	if (GetWindowTextW(hWnd, szText, 256))
		//	{
		//		/*		WINDOWPLACEMENT wp;
		//				wp.length = sizeof(WINDOWPLACEMENT);
		//				GetWindowPlacement(hWnd, &wp);*/
		//		long szLng = GetWindowLong(hWnd, GWL_STYLE);	//    '取的窗口原先的样式				
		//	//	SetWindowLong(dstHwnd, GWL_EXSTYLE, rtn);		//     '把新的样式赋给窗体
		//		auto wndTitle = QString::fromWCharArray(szText);
		//		FzTableItemPtr item(new FzTableItem((quint32)pid, (quint32)0, (quint32)hWnd, wndTitle, 0, szLng));//wp.showCmd));
		//		mlList.append(item);
		//	}
		//}
	}

	OnNotifyQueryFZWnd(list);
	const wchar_t szFindGameWndClass[] = { 39764, 21147, 23453, 36125, 0 };
	hWnd = nullptr;
	while ((hWnd = FindWindowExW(NULL, hWnd, szFindGameWndClass, NULL)) != NULL)
	{
		if ((tid = GetWindowThreadProcessId(hWnd, (LPDWORD)&pid)) != 0 && pid != GetCurrentProcessId())
		{
			//		GetClassNameW(hWnd, szText, 256);
			//		qDebug() << QString::fromWCharArray(szText);
			if (GetWindowTextW(hWnd, szText, 256))
			{
				bool bHung = IsHungAppWindow(hWnd);
				bool bShowTable = true;
				int nState = 1; //正常
				if (bHung)
				{
					nState = -1;	//挂起
					if (!checkKillHungGame(hWnd, pid))
						m_gameWndForLastHungTime.insert(hWnd, GetTickCount());
					else
						bShowTable = false;	//结束进程的 不显示
				}
				else
				{
					if (!isInGame(pid))
					{
						nState = 0;		//离线
					}
				}
				//SendMessageTimeout(hWnd,) IsHungAppWindow废弃时候 用这个
				if (bShowTable)
				{
					long szLng = GetWindowLong(hWnd, GWL_STYLE);	//    '取的窗口原先的样式				
					auto wndTitle = QString::fromWCharArray(szText);
					FzTableItemPtr item(new FzTableItem((quint32)pid, (quint32)0, (quint32)hWnd, wndTitle, nState, szLng));//wp.showCmd));
					mlList.append(item);
				}
			}
		}
	}
	OnNotifyQueryMLWnd(mlList);
	statisticsGameState();
}
void MLAssistTool::OnNotifyQueryMLWnd(FzTableItemList list)
{
	if (m_bDefaultMinimize)
	{
		for (int i = 0; i < list.size(); ++i)
		{
			const FzTableItemPtr& newItem = list.at(i);
			if (newItem->m_visible & WS_VISIBLE)
			{

				YunLai::SetWindowVal((HWND)newItem->m_hWnd, SW_SHOWMINIMIZED);
				long szLng = GetWindowLong((HWND)newItem->m_hWnd, GWL_STYLE);
				//	qDebug() << newItem->m_visible << szLng;
				newItem->m_visible = szLng;
			}
		}
	}
	for (int i = 0; i < list.size(); ++i)
	{
		const FzTableItemPtr& newItem = list.at(i);
		if (i < m_mlModl->rowCount())
		{
			FzTableItem* item = m_mlModl->ItemFromIndex(m_mlModl->index(i, 0));

			item->m_ProcessId = newItem->m_ProcessId;
			item->m_ThreadId = newItem->m_ThreadId;
			item->m_sTitle = newItem->m_sTitle;
			item->m_hWnd = newItem->m_hWnd;
			item->m_nState = newItem->m_nState;
			item->m_visible = newItem->m_visible;
			m_mlModl->dataChanged(m_mlModl->index(i, 0), m_mlModl->index(i, 2));
		}
		else
		{
			m_mlModl->appendRow(newItem);
		}
	}

	if (m_mlModl->rowCount() > list.size())
	{
		m_mlModl->removeRows(list.size(), m_mlModl->rowCount() - list.size());
	}
}

void MLAssistTool::OnNotifyQueryFZWnd(FzTableItemList list)
{
	for (int i = 0; i < list.size(); ++i)
	{
		const FzTableItemPtr& newItem = list.at(i);
		if (i < m_model->rowCount())
		{
			FzTableItem* item = m_model->ItemFromIndex(m_model->index(i, 0));

			item->m_ProcessId = newItem->m_ProcessId;
			item->m_ThreadId = newItem->m_ThreadId;
			item->m_sTitle = newItem->m_sTitle;
			item->m_hWnd = newItem->m_hWnd;
			item->m_nState = newItem->m_nState;
			item->m_visible = newItem->m_visible;


			m_model->dataChanged(m_model->index(i, 0), m_model->index(i, 2));
		}
		else
		{
			m_model->appendRow(newItem);
		}
	}
	if (m_model->rowCount() > list.size())
	{
		m_model->removeRows(list.size(), m_model->rowCount() - list.size());
	}
}

void MLAssistTool::on_comboBox_currentIndexChanged(int)
{
	m_updateTime = ui.comboBox->currentData().toInt() * 1000;
	m_refreshTimer.start(m_updateTime);
}

void MLAssistTool::on_pushButton_openAll_clicked()
{
	if (!QFile::exists(m_oneKeyStartBat))
	{
		QMessageBox::information(this, "提示：", QString("脚本启动不存在:%1").arg(m_oneKeyStartBat), QMessageBox::Ok);
		return;
	}
	//QProcess::startDetached("cmd.exe", QStringList() << m_oneKeyStartBat);
	QProcess::startDetached(m_oneKeyStartBat, QStringList());
	//QProcess proc;
	//QStringList arguments;	
	//arguments <<  m_oneKeyStartBat;
	//proc.start("cmd.exe", arguments);
	//// 等待进程启动   
	//if (!proc.waitForStarted())
	//{
	//	QMessageBox::information(this,"提示","启动失败\n",QMessageBox::Ok);
	//	return ;
	//}

}

void MLAssistTool::on_pushButton_closeAll_clicked()
{
	if (QMessageBox::information(this, "提示：", "确定关闭所有吗？", "确定", "取消") == QString("确定").toInt())
	{
		qDebug() << "关闭所有！";
		auto fzItems = m_model->GetFzTableItemList();
		for (auto item : fzItems)
		{
			YunLai::KillProcessEx(item->m_ProcessId);
		}
		auto mlItems = m_mlModl->GetFzTableItemList();
		for (auto item : mlItems)
		{
			YunLai::KillProcessEx(item->m_ProcessId);
		}
		onRefreshFzData();
	}

}

void MLAssistTool::on_pushButton_closeCur_clicked()
{
	int tabIndex = ui.tabWidget_ctrl->currentIndex();
	if (tabIndex == 0)//Fz
	{
		auto selectIndexs =ui.tableView_fz->selectionModel()->selectedIndexes();
		for (auto& index:selectIndexs)
		{
			auto item = m_model->ItemFromIndex(index);
			if (item)
			{
				YunLai::KillProcessEx(item->m_ProcessId);
			}
		}		
		onRefreshFzData();

	}
	else if (tabIndex == 1) //ML
	{
		auto selectIndexs = ui.tableView_ml->selectionModel()->selectedIndexes();
		for (auto &index : selectIndexs)
		{
			auto item = m_model->ItemFromIndex(index);
			if (item)
			{
				YunLai::KillProcessEx(item->m_ProcessId);
			}
		}
		onRefreshFzData();

	}

}

void MLAssistTool::on_pushButton_allHide_clicked()
{
	int tabIndex = ui.tabWidget_ctrl->currentIndex();
	ctrlAllStateTab(tabIndex, SW_HIDE);
	/*auto fzItems = m_model->GetFzTableItemList();
	for (auto item : fzItems)
	{
		HWND itemHwnd = (HWND)item->m_hWnd;
		YunLai::WindowTransparentFade(itemHwnd, 125);
	}*/
}

void MLAssistTool::on_pushButton_allMin_clicked()
{
	int tabIndex = ui.tabWidget_ctrl->currentIndex();
	ctrlAllStateTab(tabIndex, SW_SHOWMINIMIZED);
}

void MLAssistTool::on_pushButton_allShow_clicked()
{
	int tabIndex = ui.tabWidget_ctrl->currentIndex();
	ctrlAllStateTab(tabIndex, SW_RESTORE);

	//auto fzItems = m_model->GetFzTableItemList();
	//for (auto item : fzItems)
	//{
	//	HWND itemHwnd = (HWND)item->m_hWnd;
	//	YunLai::WindowTransparentShow(itemHwnd, 255);
	//}
}

void MLAssistTool::on_pushButton_curHide_clicked()
{
	auto index = ui.tableView_fz->currentIndex();
	auto item = m_model->ItemFromIndex(index);
	if (item)
	{
		HWND itemHwnd = (HWND)item->m_hWnd;
		YunLai::SetWindowHide(itemHwnd);
	}
	onRefreshFzData();
}

void MLAssistTool::on_pushButton_curShow_clicked()
{
	auto index = ui.tableView_fz->currentIndex();
	auto item = m_model->ItemFromIndex(index);
	if (item)
	{
		HWND itemHwnd = (HWND)item->m_hWnd;
		YunLai::SetWindowShow(itemHwnd);
	}
	onRefreshFzData();
}

void MLAssistTool::on_pushButton_curMin_clicked()
{
	auto index = ui.tableView_fz->currentIndex();
	auto item = m_model->ItemFromIndex(index);
	if (item)
	{
		HWND itemHwnd = (HWND)item->m_hWnd;
		YunLai::SetWindowVal(itemHwnd, 2);
	}
	onRefreshFzData();
}

void MLAssistTool::on_checkBox_defaultMin_stateChanged(int state)
{
	m_bDefaultMinimize = (state == Qt::Checked ? true : false);
}
