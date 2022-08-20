#pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("UTF-8")
#endif
#include <windows.h>
#include "DbgHelp.h"
#include <QDateTime>
#include <QDir>
#include <OleAuto.h>
//ANSI转化成UNICODE
static LPWSTR ANSITOUNICODE(const char* pBuf)
{
	int lenA = lstrlenA(pBuf);
	int lenW = 0;
	LPWSTR lpszFile;
	lenW = MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, 0, 0);
	if (lenW > 0)
	{
		lpszFile = SysAllocStringLen(0, lenW);	//申请一个指定字符长度的 BSTR 指针，并初始化为一个字符串
		MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, lpszFile, lenW);	//
	}
	return lpszFile;
}
typedef	BOOL(WINAPI * MINIDUMP_WRITE_DUMP)(
	IN HANDLE			hProcess,
	IN DWORD			ProcessId,
	IN HANDLE			hFile,
	IN MINIDUMP_TYPE	DumpType,
	IN CONST PMINIDUMP_EXCEPTION_INFORMATION	ExceptionParam, OPTIONAL
	IN PMINIDUMP_USER_STREAM_INFORMATION		UserStreamParam, OPTIONAL
	IN PMINIDUMP_CALLBACK_INFORMATION			CallbackParam OPTIONAL
	);
static void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件	
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// Dump信息
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;
	MINIDUMP_WRITE_DUMP	MiniDumpWriteDump_;
	HMODULE hDbgHelp = LoadLibrary(ANSITOUNICODE("DBGHELP.DLL"));
	MiniDumpWriteDump_ = (MINIDUMP_WRITE_DUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	MiniDumpWriteDump_(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}
// 处理Unhandled Exception的回调函数
//
static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	QDir temp;
	QString errorPath;
	bool exist = temp.exists(QCoreApplication::applicationDirPath() + "/Log");
	if (!exist)
	{
		bool ok = temp.mkdir(QCoreApplication::applicationDirPath() + "/Log");
		if (!ok)	//失败 放在执行路径下
			errorPath = QCoreApplication::applicationDirPath() + QString("/%1BreakLog.dmp").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss"));
	}
	errorPath = QCoreApplication::applicationDirPath() + QString("/Log/%11BreakLog.dmp").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss"));
	// 这里弹出一个错误对话框并退出程序
	CreateDumpFile(ANSITOUNICODE(errorPath.toStdString().c_str()), pException);
//	FatalAppExit(-1, ANSITOUNICODE("*** Unknown Error! ***"));
	return EXCEPTION_EXECUTE_HANDLER;
}
typedef struct
{
	unsigned int Year;
	unsigned int Month;
	unsigned int Date;
	unsigned int Hours;
	unsigned int Minutes;
	unsigned int Seconds;
}BuidDateTime;

const unsigned char MonthStr[12][4] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };

static void GetBuildDateTime(BuidDateTime *pBuilDateTime)
{
	unsigned char temp_str[4] = { 0,0,0,0 }, i = 0;
	sscanf(__DATE__, "%s %2d %4d", temp_str, &(pBuilDateTime->Date), &(pBuilDateTime->Year));
	sscanf(__TIME__, "%2d:%2d:%2d", &(pBuilDateTime->Hours), &(pBuilDateTime->Minutes), &(pBuilDateTime->Seconds));

	for (i = 0; i < 12; i++)
	{
		if (temp_str[0] == MonthStr[i][0] && temp_str[1] == MonthStr[1][1] && temp_str[2] == MonthStr[i][2])
		{
			pBuilDateTime->Month = i + 1;
			break;
		}
	}
}