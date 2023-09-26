#include "AttachGameWnd.h"
#include "GameCtrl.h"
#include "GameData.h"
#include "ITTabBarStyle.h"
#include "MINT.h"
#include <psapi.h>
#include <tlhelp32.h>
#include <QDebug>
#include <QListWidgetItem>
#include <QMap>
#include <QMessageBox>
#include <QString>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Psapi.lib")

AttachGameWnd::AttachGameWnd(QWidget *parent) :
		QWidget(parent)
{
	ui.setupUi(this);
	ui.listWidget->hide();
	ui.textEdit_update->hide();
	ui.pushButton_refresh->hide();
	ui.tabWidget->setStyle(new ITTabBarStyle);
	m_model = new CProcessTableModel(ui.tableView);
	ui.tableView->setModel(m_model);
	ui.tableView->setColumnWidth(0, 60);
	ui.tableView->setColumnWidth(1, 200);
	ui.tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
	ui.tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
	ui.tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);

	auto pTabBar = ui.tabWidget->tabBar();

	/*m_pLogBack = new QPushButton("回城", this);
	m_pLogOut = new QPushButton("登出", this);
	connect(m_pLogBack, SIGNAL(clicked()), this, SLOT(on_pushButton_logback()));
	connect(m_pLogOut, SIGNAL(clicked()), this, SLOT(on_pushButton_logout()));
	QWidget *pWidget = new QWidget(this);
	QHBoxLayout *pHLayout = new QHBoxLayout(pWidget);
	pHLayout->addWidget(m_pLogBack);
	pHLayout->addWidget(m_pLogOut);
	pHLayout->setContentsMargins(0, 0, 0, 0);
	ui.tabWidget->setCornerWidget(pWidget);*/

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(OnQueueQueryProcess()));
	timer->start(500);
	connect(timer, SIGNAL(timeout()), this, SLOT(OnCheckFreezeProcess()));
	timer->start(1000);
	connect(ui.checkBox_syncWindow, SIGNAL(stateChanged(int)), g_pGameCtrl, SLOT(OnSetSyncGameWindowFlag(int)), Qt::ConnectionType::QueuedConnection);
	connect(g_pGameCtrl, &GameCtrl::NotifyKillProcess, this, &AttachGameWnd::OnKillProcess);
	connect(g_pGameCtrl, &GameCtrl::NotifyAutoAttachProcess, this, &AttachGameWnd::OnAutoAttachProcess, Qt::QueuedConnection);
}

AttachGameWnd::~AttachGameWnd()
{
}

void AttachGameWnd::OnAutoAttachProcess(quint32 ProcessId, quint32 ThreadId)
{
	m_AutoAttachPID = ProcessId;
	m_AutoAttachTID = ThreadId;
}
//定时扫描魔力宝贝进程
void AttachGameWnd::OnQueueQueryProcess()
{
	CProcessItemList list;
	//if (g_pGameCtrl->getGameHwnd() != nullptr) //已附加的 不刷新了 打开的话，后面的附加列没更新，其他打开是已更新的
	//	return;
	const wchar_t szFindGameClass[] = { 39764, 21147, 23453, 36125, 0 };

	HWND hWnd = NULL;
	DWORD pid, tid;
	WCHAR szText[256];

	while ((hWnd = FindWindowExW(NULL, hWnd, szFindGameClass, NULL)) != NULL)
	{
		if ((tid = GetWindowThreadProcessId(hWnd, (LPDWORD)&pid)) != 0 && pid != GetCurrentProcessId())
		{
			if (GetWindowTextW(hWnd, szText, 256))
			{
				auto wndTitle = QString::fromWCharArray(szText);
				bool attached = IsProcessAttached(pid);
				void *pBaseAddr = YunLai::GetProcessImageBase1(pid); //这个占cpu
				QString sUserName = QString(" [%1] ").arg(QString::fromWCharArray(YunLai::ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xE12D30, 17))));
				int nGameServerType = g_pGameFun->GetGameServerType(QString::fromWCharArray(YunLai::ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xB9E858, 15))));
				QString sGameServerType = g_pGameFun->GetGameServerTypeTextFromType(nGameServerType);
				if (!attached && !wndTitle.contains("#"))
				{
					//登录成功后 读取用户账号
					//	qDebug() << pBaseAddr;
					wndTitle += " ";
					wndTitle += sGameServerType;
					wndTitle += " ";
					wndTitle += sUserName;
				}						
				QString szLoginUserID = YunLai::ReadMemoryStrFromProcessID(pid, (ULONG_PTR)pBaseAddr + 0xBDB488, 100); //游戏id
				CProcessItemPtr item(new CProcessItem((quint32)pid, (quint32)tid, (quint32)hWnd, wndTitle, attached, szLoginUserID, nGameServerType));
				list.append(item);

				if (!attached && m_AutoAttachPID == pid && m_AutoAttachTID == tid)
				{
					if (g_pGameCtrl->GetStartGameHide())
					{
						YunLai::SetWindowVal(hWnd, 2);
					}
					OnQueueAttachProcess((quint32)pid, (quint32)tid, (quint32)hWnd, QString("CGAHook.dll"));
					m_AutoAttachPID = 0;
					m_AutoAttachTID = 0;
				}
			}
		}
	}
	emit g_pGameCtrl->signal_gameWndList(list);
	qSort(list.begin(), list.end(), [&](CProcessItemPtr p1, CProcessItemPtr p2) { return !p1->m_bAttached;});
	OnNotifyQueryProcess(list);
}
void AttachGameWnd::OnNotifyQueryProcess(CProcessItemList list)
{
	for (int i = 0; i < list.size(); ++i)
	{
		const CProcessItemPtr &newItem = list.at(i);
		if (i < m_model->rowCount())
		{
			CProcessItem *item = m_model->ItemFromIndex(m_model->index(i, 0));

			item->m_ProcessId = newItem->m_ProcessId;
			item->m_ThreadId = newItem->m_ThreadId;
			item->m_Title = newItem->m_Title;
			item->m_hWnd = newItem->m_hWnd;
			item->m_bAttached = newItem->m_bAttached;

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
bool AttachGameWnd::ReadSharedData(quint32 ProcessId, int &port, quint32 &hWnd)
{
	bool bSuccess = false;

	WCHAR szLockName[32];
	wsprintfW(szLockName, L"CGASharedDataLock_%d", ProcessId);

	HANDLE hDataLock = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, szLockName);
	if (hDataLock)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(hDataLock, 500))
		{
			WCHAR szMappingName[32];
			wsprintfW(szMappingName, L"CGASharedData_%d", ProcessId);
			HANDLE hFileMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY, 0, sizeof(CGA::CGAShare_t), szMappingName);
			if (hFileMapping)
			{
				LPVOID pViewOfFile = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
				if (pViewOfFile)
				{
					CGA::CGAShare_t *data = (CGA::CGAShare_t *)pViewOfFile;

					port = data->Port;
					hWnd = (quint32)data->hWnd;
					bSuccess = true;
					UnmapViewOfFile(pViewOfFile);
				}
				CloseHandle(hFileMapping);
			}
			ReleaseMutex(hDataLock);
			CloseHandle(hDataLock);
		}
	}

	return bSuccess;
}

void AttachGameWnd::doLoadUserConfig(QSettings &iniFile)
{
	//ui.sysCfgWgt->doLoadUserConfig(iniFile);
}

void AttachGameWnd::doSaveUserConfig(QSettings &iniFile)
{
	//ui.sysCfgWgt->doSaveUserConfig(iniFile);
}

void AttachGameWnd::on_pushButton_logback()
{
	g_CGAInterface->LogBack();
}

void AttachGameWnd::on_pushButton_logout()
{
	g_CGAInterface->LogOut();
}

void AttachGameWnd::on_pushButton_attach_clicked()
{
	if (!ui.tableView->currentIndex().isValid())
		return;

	CProcessItem *item = m_model->ItemFromIndex(ui.tableView->currentIndex());
	if (!item)
		return;
	OnQueueAttachProcess((quint32)item->m_ProcessId, (quint32)item->m_ThreadId, (quint32)item->m_hWnd, QString("cgahook.dll"));
}

void AttachGameWnd::ConnectToServer(quint32 ProcessId, quint32 ThreadId, int port, quint32 hwnd)
{
	if (!CreateAttachMutex(ProcessId, ThreadId))
		return;

	if (g_CGAInterface->Connect(port))
	{
		emit g_pGameCtrl->NotifyAttachProcessOk(ProcessId, ThreadId, port, hwnd);
		g_pGameCtrl->SetGameThreadID(ThreadId);
		void *pBaseAddr = YunLai::GetProcessImageBase1(ProcessId);
		qDebug() << pBaseAddr;
		g_pGameCtrl->setGameProcess(ProcessId);
		g_pGameCtrl->SetGamePort(port);
		g_pGameCtrl->setGameHwnd((HWND)hwnd);
		g_pGameCtrl->setGameBaseAddr((ULONG)pBaseAddr);
		g_pGameCtrl->StartUpdateTimer();
		ui.tabWidget->setCurrentIndex(1);
		RECT gameRect;
		GetWindowRect((HWND)hwnd, &gameRect);
		emit signal_followGameWnd(gameRect.left, gameRect.top, gameRect.right, gameRect.bottom);

		QListWidgetItem *pCurItem = ui.listWidget->currentItem();
		if (pCurItem == nullptr)
			return;
		QString sItemText = pCurItem->text();
		if (!sItemText.contains("已附加"))
		{
			sItemText += " 已附加";
			pCurItem->setText(sItemText);
		}
	}
	else
	{
		
		QMessageBox msg;
		msg.setText("Could not connect to local RPC server.");
		QTimer::singleShot(15000, &msg, &QMessageBox::reject); //15秒
		msg.exec();
	}
}

bool AttachGameWnd::CreateAttachMutex(quint32 ProcessId, quint32 ThreadId)
{
	WCHAR szMutex[32];
	wsprintfW(szMutex, L"CGAAttachMutex_%d", ProcessId);
	auto AttachMutex = CreateMutexW(NULL, TRUE, szMutex); //查找及创建指定互斥体 如果存在 获得句柄 不存在则创建
	if (AttachMutex == NULL)
	{
		//NotifyAttachProcessFailed(ProcessId, ThreadId, -4, tr("Game already attached by another instance of CGAssistant."));
		return false;
	}
	int lastEro = GetLastError();
	/*if (lastEro == ERROR_ALREADY_EXISTS)
	{
		QMessageBox::information(this, "提示：", "游戏已经被附加!", "确定");
		return false;
	}*/
	g_pGameCtrl->SetGameCGAMutex(AttachMutex);
	return true;
}

bool AttachGameWnd::InjectByMsgHook(quint32 ThreadId, quint32 hWnd, QString &dllPath, int &errorCode, QString &errorString)
{
	HMODULE hModule = NULL;
	do
	{
		hModule = LoadLibraryW((LPCWSTR)dllPath.utf16());
		if (!hModule)
		{
			errorCode = GetLastError();
			errorString = tr("InjectByMsgHook failed with LoadLibraryW, errorCode: %1").arg(errorCode);
			break;
		}

		PVOID pfnProc = (PVOID)GetProcAddress(hModule, "_GetMsgProc@12");
		if (!pfnProc)
		{
			errorCode = GetLastError();
			errorString = tr("InjectByMsgHook failed with GetProcAddress, errorCode: %1").arg(errorCode);
			break;
		}

		HHOOK hHook = SetWindowsHookExW(WH_GETMESSAGE, (HOOKPROC)pfnProc, hModule, ThreadId);
		if (!hHook)
		{
			errorCode = GetLastError();
			errorString = tr("InjectByMsgHook failed with SetWindowsHookExW, errorCode: %1").arg(errorCode);
			break;
		}

		if (!PostMessageW((HWND)hWnd, WM_NULL, 0, 0))
		{
			errorCode = GetLastError();
			errorString = tr("InjectByMsgHook failed with PostMessageW, errorCode: %1").arg(errorCode);
			break;
		}

		errorCode = 0;
	} while (0);

	if (hModule)
		FreeLibrary(hModule);

	return (errorCode == 0) ? true : false;
}

QString AttachGameWnd::GetLoginGameData()
{
	return ui.accountForm->GetLoginArgs();
}

bool AttachGameWnd::InJectDll2(DWORD ProcessId, const char *DllName)
{
	HMODULE hModule = NULL;
	int errorCode = 0;
	QString errorString;
	do
	{
		hModule = LoadLibraryW(YunLai::ANSITOUNICODE1(DllName));
		if (!hModule)
		{
			int errorCode = GetLastError();
			errorString = tr("InjectByMsgHook failed with LoadLibraryW, errorCode: %1").arg(errorCode);
			break;
		}

		PVOID pfnProc = (PVOID)GetProcAddress(hModule, "_GetMsgProc@12");
		if (!pfnProc)
		{
			errorCode = GetLastError();
			errorString = tr("InjectByMsgHook failed with GetProcAddress, errorCode: %1").arg(errorCode);
			break;
		}
		HWND hWnd = YunLai::FindMainWindow(ProcessId);
		DWORD tid = GetWindowThreadProcessId(hWnd, (LPDWORD)&ProcessId);

		HHOOK hHook = SetWindowsHookExW(WH_GETMESSAGE, (HOOKPROC)pfnProc, hModule, tid);
		if (!hHook)
		{
			errorCode = GetLastError();
			errorString = tr("InjectByMsgHook failed with SetWindowsHookExW, errorCode: %1").arg(errorCode);
			break;
		}

		if (!PostMessageW((HWND)hWnd, WM_NULL, 0, 0))
		{
			errorCode = GetLastError();
			errorString = tr("InjectByMsgHook failed with PostMessageW, errorCode: %1").arg(errorCode);
			break;
		}
		errorCode = 0;
	} while (0);

	if (hModule)
		FreeLibrary(hModule);
	if (!errorString.isEmpty())
		qDebug() << errorString;
	return (errorCode == 0) ? true : false;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//功能：提升进程权限
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
bool EnablePrivilege(WCHAR PrivilegeName[], BOOL IsEnable)

{
	HANDLE hToken;

	TOKEN_PRIVILEGES tp;

	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_READ, &hToken))
	{
		return false;
	}

	if (!LookupPrivilegeValue(NULL, PrivilegeName, &luid))
	{
		return false;
	}

	tp.PrivilegeCount = 1;

	tp.Privileges[0].Luid = luid;

	tp.Privileges[0].Attributes = (IsEnable) ? SE_PRIVILEGE_ENABLED : 0;

	BOOL bSucc = AdjustTokenPrivileges(hToken, FALSE, &tp, NULL, NULL, NULL);

	CloseHandle(hToken);

	return (GetLastError() == ERROR_SUCCESS);
}

unsigned int ThreadFunc(void *p)
{

	STARTUPINFO sif;
	PROCESS_INFORMATION pi;
	ZeroMemory(&sif, sizeof(sif));
	ZeroMemory(&pi, sizeof(pi));
	sif.cb = sizeof(sif.cb);

	QString CStrPath = "D://game.exe"; //游戏文件路径

	//启动游戏进程
	if (!::CreateProcess(NULL, (LPWSTR)(LPCWSTR)CStrPath.toStdString().c_str(), NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
				0, (LPWSTR)(LPCWSTR)CStrPath.left(CStrPath.size() - 12).toStdString().c_str(), &sif, &pi))
	{
		QString CStr = QString::number(GetLastError());
		::MessageBox(0, L"Game.exe启动失败", (LPWSTR)(LPCWSTR)CStr.toStdString().c_str(), MB_OK + MB_ICONERROR);
		return 1;
	}
	//注入游戏
	//	InJectDll(pi.dwProcessId, "D://ml.dll");

	Sleep(500);
	//让游戏运行
	ResumeThread(pi.hThread);

	return 0;
}

DWORD AttachGameWnd::GetGameProcessId(wchar_t *ProcessNmae)
{
	DWORD ProcessId = -1;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);

	BOOL bOk = Process32First(hSnapshot, &pe);

	for (; bOk; bOk = Process32Next(hSnapshot, &pe))
	{
		if (_wcsicmp(ProcessNmae, pe.szExeFile) == 0)
		{
			ProcessId = pe.th32ProcessID;
		}
	}

	::CloseHandle(hSnapshot);
	return ProcessId;
}
bool AttachGameWnd::InJectDll(DWORD ProcessId, const char *DllName)
{
	if (ProcessId == -1)
	{
		return FALSE;
	}
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
	if (!hProcess)
	{
		return FALSE;
	}

	//目标程序中申请一片内存
	LPVOID lpBaseAddress = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!lpBaseAddress)
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	CHAR lpBuffer[MAX_PATH] = { 0 };

	GetModuleFileNameA(NULL, lpBuffer, MAX_PATH);

	CHAR *lpPath = strrchr(lpBuffer, '\\');
	strcpy(++lpPath, "CGAHook.dll");

	LPWSTR dllPath = YunLai::ANSITOUNICODE1(DllName);
	//DLL路径写到目标进程
	BOOL bWrite = WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, MAX_PATH, NULL);
	if (!bWrite)
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	//开启远程线程执行加载DLL的函数
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, lpBaseAddress, 0, NULL);
	if (!hThread)
	{
		CloseHandle(hProcess);
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);

	//清理在目标程序中申请的那一片内存
	VirtualFreeEx(hProcess, lpBaseAddress, 4096, MEM_DECOMMIT);
	CloseHandle(hProcess);
	return TRUE;
}

void AttachGameWnd::on_pushButton_refresh_clicked()
{
	ui.listWidget->clear();
	QMap<qint64, QString> processData;
	YunLai::GetAllProcess(processData);

	for (auto it = processData.begin(); it != processData.end(); ++it)
	{
		if (it.value().contains("qfmoli") || it.value().contains("cg_item_6000.exe"))
		{
			bool attached = IsProcessAttached(it.key());
			QString szInfo, szLoginUser;
			if (attached)
				szInfo = "已附加";
			else
				szInfo = " ";
			if (it.value().contains("qfmoli"))
				szLoginUser = YunLai::ReadMemoryStrFromProcessID(it.key(), "00E39EE4", 100);
			else if (it.value().contains("cg_item_6000.exe"))
			{
				void *pBaseAddr = YunLai::GetProcessImageBase1(it.key());
				qDebug() << pBaseAddr;
				szLoginUser = YunLai::ReadMemoryStrFromProcessID(it.key(), (ULONG_PTR)pBaseAddr + 0xBDB488, 100); //
			}
			QListWidgetItem *pItem = new QListWidgetItem(QString("%1 %2 %3 %4").arg(it.key()).arg(it.value()).arg(szLoginUser).arg(szInfo)); //szLoginUser));
			pItem->setData(Qt::UserRole, it.key());
			ui.listWidget->addItem(pItem);
		}
	}
}

void AttachGameWnd::on_pushButton_unAttach_clicked()
{
	qint64 gameProcessID = g_pGameCtrl->getGamePID();
	int port = 0;
	quint32 hwnd = 0;
	if (ReadSharedData(gameProcessID, port, hwnd))
	{
		//Already attached to game
		Disconnect();
	}
	//QListWidgetItem *pCurItem = ui.listWidget->currentItem();
	//if (pCurItem == nullptr)
	//	return;

	//qint64 gameProcessID = pCurItem->data(Qt::UserRole).toInt(); //ui.comboBox->currentData(Qt::UserRole).toInt();
	//int port = 0;
	//quint32 hwnd = 0;
	//if (ReadSharedData(gameProcessID, port, hwnd))
	//{
	//	//Already attached to game
	//	Disconnect();
	//}
}

void AttachGameWnd::on_pushButton_login_clicked()
{
	ui.tabWidget->setCurrentIndex(1);
	//	GameData::getInstance().JianDongXi(MOVE_DIRECTION_SouthWest);
}

bool AttachGameWnd::IsProcessAttached(quint32 ProcessId)
{
	bool bAttached = false;
	WCHAR szMutex[32];
	wsprintfW(szMutex, L"CGAAttachMutex_%d", ProcessId);
	HANDLE hAttachMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, szMutex);
	if (hAttachMutex)
	{
		bAttached = true;
		CloseHandle(hAttachMutex);
	}
	return bAttached;
}

void AttachGameWnd::Disconnect()
{
	//停止相关的线程数据，需要时候再开启
	g_pGameCtrl->StopUpdateTimer();
	g_pGameCtrl->WaitThreadFini();
	qDebug() << "线程结束成功，断开游戏连接";
	g_CGAInterface->Disconnect();
	emit g_pGameCtrl->signal_clearUiInfo();
}

void AttachGameWnd::OnCheckFreezeProcess()
{
	auto attachHwnd = g_pGameCtrl->getGameHwnd();
	if (!attachHwnd)
		return;

	if (!IsWindow(attachHwnd))
		return;

	DWORD pid, tid;
	tid = GetWindowThreadProcessId(attachHwnd, &pid);
	if (!pid || !tid)
	{
		g_pGameCtrl->setGameHwnd(nullptr);
		g_pGameCtrl->SetGamePort(0);
		return;
	}

	bool freeze = false;
	double tick;
	if (g_CGAInterface->IsConnected() && g_CGAInterface->GetNextAnimTickCount(tick))
	{
		if (m_LastGameAnimTick == tick)
		{
			m_AnimTickFreezeTime++;
			if (m_AnimTickFreezeTime >= m_MaxFreezeTime)
				freeze = true;
		}
		else
		{
			m_AnimTickFreezeTime = 0;
			m_LastGameAnimTick = tick;
		}
	}
	else
	{
		m_AnimTickFreezeTime++;
		if (m_AnimTickFreezeTime >= m_MaxFreezeTime)
			freeze = true;
	}

	//the game is freezed somehow, kill it
	//游戏已经释放，杀掉进程
	if (freeze)
	{
		HANDLE ProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
		if (ProcessHandle)
		{
			if (STATUS_SUCCESS == NtTerminateProcess(ProcessHandle, 0))
			{
				Disconnect();
			}
			CloseHandle(ProcessHandle);
		}
	}
}

void AttachGameWnd::OnRetryAttachProcess()
{
	auto timer = (CRetryAttachProcessTimer *)sender();

	int port = 0;
	quint32 hwnd = 0;
	quint32 processID = timer->m_ProcessId;
	if (!ReadSharedData(timer->m_ProcessId, port, hwnd))
	{
		if (timer->m_retry > 10)
		{
			timer->stop();
			timer->deleteLater();
			//QMessageBox::information(this, "提示:", "读取共享数据超时，重新加载！");
			qDebug() << "读取共享数据超时，重新加载！";
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
			if (NULL == hProcess)
			{
				//	m_lastEroMsg = "OpenProcess Ero";
				qDebug() << "打开进程失败";
				return;
			}
			TerminateProcess(hProcess, 0);				
			return;
		}
		else
		{
			++timer->m_retry;
		}
	}
	else
	{
		timer->stop();
		timer->deleteLater();
		ConnectToServer(timer->m_ProcessId, timer->m_ThreadId, port, hwnd);
		/*HWND hWnd = YunLai::FindMainWindow(timer->m_ProcessId);
		DWORD tid = GetWindowThreadProcessId(hWnd, (LPDWORD)timer->m_ProcessId);
		ConnectToServer(timer->m_ProcessId, tid, port, hwnd);*/
	}
}

void AttachGameWnd::OnKillProcess()
{
	auto attachHwnd = g_pGameCtrl->getGameHwnd();
	if (!attachHwnd)
		return;

	if (!IsWindow(attachHwnd))
		return;
	
	DWORD pid, tid;
	tid = GetWindowThreadProcessId(attachHwnd, &pid);
	if (!pid || !tid)
	{
		g_pGameCtrl->SetGamePort(0);
		g_pGameCtrl->setGameProcess(0);
		g_pGameCtrl->setGameHwnd(nullptr);
		return;
	}
	HANDLE ProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (ProcessHandle)
	{
		if (STATUS_SUCCESS == NtTerminateProcess(ProcessHandle, 0))
		{
			Disconnect();
		}
		CloseHandle(ProcessHandle);
	}
	//调整顺序，否则那边还没干掉，这边进程已经重置为0了
	g_pGameCtrl->SetGamePort(0);
	g_pGameCtrl->setGameProcess(0);
	g_pGameCtrl->setGameHwnd(nullptr);
}

void AttachGameWnd::OnQueueAttachProcess(quint32 ProcessId, quint32 ThreadId, quint32 hWnd, QString dllPath)
{
	int port = 0;
	quint32 hwnd = 0;
	if (!ReadSharedData(ProcessId, port, hwnd))
	{
		int errorCode = -1;
		QString errorString;
		if (!InjectByMsgHook(ThreadId, hWnd, dllPath, errorCode, errorString))
		{
			//	NotifyAttachProcessFailed(ProcessId, ThreadId, errorCode, errorString);
			return;
		}
		CRetryAttachProcessTimer *timer = new CRetryAttachProcessTimer(ProcessId, ThreadId, this);
		connect(timer, SIGNAL(timeout()), this, SLOT(OnRetryAttachProcess()));
		timer->start(500);
	}
	else
	{
		//Already attached to game
		Disconnect();
		ConnectToServer(ProcessId, ThreadId, port, hwnd);
	}
}