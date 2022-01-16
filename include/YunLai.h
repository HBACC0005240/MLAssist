#pragma once
#if _MSC_VER >= 1600          ///1600为vs2010编译器版本 
#pragma execution_character_set("utf-8")        ///c++11特性  设置执行编码 
#endif 

#include <windows.h>
#include <windef.h>
#include <qwindowdefs_win.h>
#include <string>
#include <functional>
//#include "MINT.h"
//#include <experimental/filesystem>
//#include <experimental/scope>
using namespace std;
//using namespace experimental;
#define MAX_MEMORY_TEXT_SIZE 10000


class virtual_buffer_t
{
public:
	virtual_buffer_t();
	virtual_buffer_t(size_t size);
	~virtual_buffer_t();
	void* GetSpace(size_t needSize);
	size_t GetLength() { return m_cbSize; }
	void* GetBuffer() { return m_pBuffer; }

	void* m_pBuffer;
	size_t m_cbSize;
};

class crt_buffer_t
{
public:
	crt_buffer_t();
	crt_buffer_t(size_t size);
	~crt_buffer_t();
	void* GetSpace(size_t needSize);
	size_t GetLength() { return m_cbSize; }
	void* GetBuffer() { return m_pBuffer; }

	void* m_pBuffer;
	size_t m_cbSize;
};
class YunLai
{
public:
	YunLai();
	~YunLai();
	struct handle_data
	{
		unsigned long process_id;
		HWND best_handle;
	};
	struct EnumChildWindowData
	{
		HWND hwnd;
		int status;
		int key;

	};
	static LPWSTR ANSITOUNICODE1(const char* pBuf);

	//进程
	static void GetAllProcess(QMap<qint64, QString>& processInfo);
	static QString GetPathByProcessID(DWORD pid);
	static void* GetProcessImageBase1(DWORD dwProcessId);
	static void* GetProcessImageBase2(DWORD dwProcessId);
	//动画显示 及 隐藏窗口
	static void WindowTransparentShow(HWND dstHwnd, int maxShow = 255, int speed = 15);
	static void WindowTransparentFade(HWND dstHwnd, int minShow = 0, int speed = -15);
	//激活指定窗口
	static bool ActiveWnd(HWND hwnd);
	//设置焦点	
	static bool SetFocusToWnd(HWND hwnd);
	static bool IsVisibleWnd(HWND hwnd);
	//显示窗口
	static bool SetWindowShow(HWND hwnd);
	//隐藏窗口
	static bool SetWindowHide(HWND hwnd);
	//设置窗口状态0 隐藏 9还原 6最小化 3最大化  具体值 参考windowsAPI
	static bool SetWindowVal(HWND hwnd, int state);


	//查找主窗口句柄
	static HWND FindMainWindow(unsigned long process_id);
	static BOOL IsMainWindow(HWND handle);
	static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);

	//写文本到指定窗口 hwnd窗口句柄 起始坐标x y 写的内容szText 颜色color 字体大小 fontsize 字体名称 fontName 加粗bold 斜体 italic 下划线 underLIne
	static bool WriteTextToWnd(HWND hwnd, int x, int y, const char* szText, DWORD color, int fontSize, char* fontName, bool bold, bool italic, bool underLine);

	//向指定窗口句柄的窗口中发送按键消息(无返回值) nKey按键值 nStatus方式 1按键 2按下放开 3按下 4放开  bInherit是否传递给子窗口
	static void SendKeyToWnd(HWND hwnd, int nKey, int nStatus = 0, bool bInherit = false);
	static BOOL CALLBACK EnumChildWindowsCallBack(HWND hwnd, LPARAM lParam);

	//取色深
	static int  GetColorDepth();

	//取颜色
	static DWORD GetScreenColor(int x, int y, HWND hwnd = nullptr);
	static DWORD GetScreenColorCapture(int x, int y, HWND hwnd = nullptr);
	static POINT FindBMPColor(uchar* pData, DWORD nColor, int nDVal, POINT nFindPoint);
	static POINT FindScreenColor(DWORD nColor, int nLeftX, int nLeftY, int nWidth, int nHeight, int nDVal, POINT nFindPoint, HWND hwnd = nullptr);

	//进程相关
	static DWORD GetProcessIDFromWnd(HWND hwnd);
	static const char* ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress, const char* offset1, const char* offset2, const char* offset3, const char* offset4);
	static char* ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress, int nLen);
	static char* ReadMemoryStrFromProcessID(DWORD processID, const char* szAddress, int nLen);
	static char* ReadMemoryStrFromProcessID(DWORD processID, DWORD pAddress, int nLen);

	static int   ReadMemoryIntFromWnd(HWND hwnd, const char* szAddress);
	static int   ReadMemoryIntFromProcessID(DWORD processID, const char* szAddress);
	static int   ReadMemoryIntFromProcessID(DWORD processID, DWORD pAddress);
	static WORD   ReadMemoryWordFromProcessID(DWORD processID, DWORD pAddress);

	static void  WriteMemoryIntToWnd(HWND hwnd, const char* szAddress, int nVal);
	static void  WriteMemoryIntToProcess(DWORD hProcessID, const char* szAddress, int nVal);
	static bool KillProcess(HWND hwnd);
	static bool KillProcessEx(DWORD hProcessID);

	//static long  ForceOpenProcess(DWORD dwDesiredAccess, bool bInhert, DWORD ProcessId);
	//static bool  ForceCloseProcess(DWORD hProcessID, DWORD nExitStatus);

	//dll模块
	static HMODULE GetLibraryHandle(const char* szLib);
	static FARPROC GetFunAddress(const char* szLib, const char* szFun);

	//键盘
	static string GetKeyTextFromKey(int nKey);
	static void KeyClickedEvent();
	static int MakeKeyLParam(int VirtualKey, int flag);
	static void SimKeyClick(UINT vk_Code, BOOL bDown);
	//鼠标
	static void MouseClickedEvent(int nType = 0, int nCtrl = 0);
	//整数型, 可空, 可空:为左键  1 #左键   2 #右键   3 #中键
	//.参数 控制, 整数型, 可空, 可空:为单击  1 #单击   2 #双击   3 #按下  4 #放开
	static void SimMouseClick(int nType = 0, int nCtrl = 0);
	//时间
	static void SuperTimeDelay(int timeDelay, int timeType);
	static char* HexTextToDecimalText(const char* szHex);
	char* MyItoa(int num, char* str, int radix);

	//截图
	using SnapshotCallback = std::function<void(void*, size_t, int, int, int)>;
	virtual_buffer_t m_snapshot_buffer;
	static uchar* ScreenCapture(HWND hwnd, int nLeft = 0, int nTop = 0, int nWidth = 0, int nHeight = 0, bool bClent = false);
	static HBITMAP CopyScreenToBitmap(HWND pHwnd);
	static bool SaveBitmapToFile(HBITMAP   hBitmap, string szfilename);
	//	static bool DIBSnapshot(HWND hWnd, int scale, const SnapshotCallback &callback);
	//	static bool DIBToCvMat(cv::Mat &mat, void *pBuffer, size_t cbBuffer, int width, int height, int bbp);
	//	int HexToDecimalInt();
	static QString m_lastEroMsg;
};

