#include "YunLai.h"
#include <tlhelp32.h>
#include <psapi.h> 
#include <QString>
#include <QMap>
#include <QDebug>
#include "MINT.h"
#include "../include/ITPublic.h"
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Psapi.lib")
//#include "LibApiImport.h"
static HWND m_lastHwnd = nullptr;
#define FORMAT_PATH(path) path.replace('\\','/').toLower()

virtual_buffer_t::virtual_buffer_t() : m_cbSize(0), m_pBuffer(NULL)
{
}
virtual_buffer_t::virtual_buffer_t(size_t size) : m_cbSize(size), m_pBuffer(VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE))
{
}

virtual_buffer_t::~virtual_buffer_t()
{
	if (m_pBuffer)
		VirtualFree(m_pBuffer, 0, MEM_RELEASE);
}

void* virtual_buffer_t::GetSpace(size_t needSize)
{
	if (m_cbSize < needSize)
	{
		if (m_pBuffer)
			VirtualFree(m_pBuffer, 0, MEM_RELEASE);
		m_pBuffer = VirtualAlloc(NULL, needSize, MEM_COMMIT, PAGE_READWRITE);
		m_cbSize = needSize;
	}
	return m_pBuffer;
}

crt_buffer_t::crt_buffer_t() : m_cbSize(0), m_pBuffer(NULL)
{
}

crt_buffer_t::crt_buffer_t(size_t size) : m_cbSize(size), m_pBuffer(malloc(size))
{
}

crt_buffer_t::~crt_buffer_t()
{
	if (m_pBuffer)
		free(m_pBuffer);
}

void* crt_buffer_t::GetSpace(size_t needSize)
{
	if (m_cbSize < needSize)
	{
		if (m_pBuffer)
			m_pBuffer = realloc(m_pBuffer, needSize);
		else
			m_pBuffer = malloc(needSize);
		m_cbSize = needSize;
	}
	return m_pBuffer;
}
YunLai::YunLai()
{
}

YunLai::~YunLai()
{

}
//ANSI转化成UNICODE
LPWSTR YunLai::ANSITOUNICODE1(const char* pBuf)
{
	int lenA = lstrlenA(pBuf);
	int lenW = 0;
	LPWSTR lpszFile = nullptr;
	lenW = MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, 0, 0);
	if (lenW > 0)
	{
		lpszFile = SysAllocStringLen(0, lenW);
		MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, lpszFile, lenW);
	}
	return lpszFile;
}
void YunLai::GetAllProcess(QMap<qint64, QString>& processInfo)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//获取进程列表失败
		return;
	}
	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		QString exeName = (QString::fromUtf16(reinterpret_cast<const unsigned short*>(pe32.szExeFile)));
		/*QString exePath = GetPathByProcessID(pe32.th32ProcessID);
		exePath = FORMAT_PATH(exePath);
		if (exePath.isEmpty())
		{

		}
		else
		{
			processInfo[pe32.th32ProcessID] = exePath;
		}*/
		processInfo[pe32.th32ProcessID] = exeName;
		bMore = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
}
QString YunLai::GetPathByProcessID(DWORD pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess)
	{

		return "";
	}
	WCHAR filePath[MAX_PATH];
	DWORD ret = GetModuleFileNameEx(hProcess, NULL, filePath, MAX_PATH);
	QString file = QString::fromStdWString(filePath);
	CloseHandle(hProcess);
	return ret == 0 ? "" : file;
}
void* YunLai::GetProcessImageBase1(DWORD dwProcessId)
{
	PVOID pProcessImageBase = NULL;
	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);
	// 获取指定进程全部模块的快照
	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (INVALID_HANDLE_VALUE == hModuleSnap)
	{
		//	m_lastEroMsg = "CreateToolhelp32Snapshot Ero";
		return pProcessImageBase;
	}
	// 获取快照中第一条信息
	BOOL bRet = ::Module32First(hModuleSnap, &me32);
	if (bRet)
	{
		// 获取加载基址
		pProcessImageBase = (PVOID)me32.modBaseAddr;
	}
	// 关闭句柄
	::CloseHandle(hModuleSnap);
	return pProcessImageBase;
}

void* YunLai::GetProcessImageBase2(DWORD dwProcessId)
{
	PVOID pProcessImageBase = NULL;
	//打开进程, 获取进程句柄
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (NULL == hProcess)
	{
		//	m_lastEroMsg = "OpenProcess Ero";
		return pProcessImageBase;
	}
	// 遍历进程模块,
	HMODULE hModule[100] = { 0 };
	DWORD dwRet = 0;
	BOOL bRet = ::EnumProcessModules(hProcess, (HMODULE*)(hModule), sizeof(hModule), &dwRet);
	if (FALSE == bRet)
	{
		::CloseHandle(hProcess);
		//	m_lastEroMsg = "EnumProcessModules Ero";
		return pProcessImageBase;
	}
	// 获取第一个模块加载基址
	pProcessImageBase = hModule[0];
	// 关闭句柄
	::CloseHandle(hProcess);
	return pProcessImageBase;
}

void YunLai::WindowTransparentShow(HWND dstHwnd, int maxShow /*= 255*/, int speed/* = 15*/)
{
	long rtn = GetWindowLong(dstHwnd, GWL_EXSTYLE);	//    '取的窗口原先的样式
	rtn = rtn | WS_EX_LAYERED;						//    '使窗体添加上新的样式WS_EX_LAYERED
	SetWindowLong(dstHwnd, GWL_EXSTYLE, rtn);		//     '把新的样式赋给窗体
	for (int i = 0; i < maxShow; i += speed)
	{
		SetLayeredWindowAttributes(dstHwnd, 0, i, LWA_ALPHA); //'把窗体设置成半透明样式,第二个参数表示透明程度	'取值范围0--255,为0时就是一个全透明的窗体了		
	}
	SetLayeredWindowAttributes(dstHwnd, 0, 255, LWA_ALPHA);
	//	'把窗体设置成半透明样式,第二个参数表示透明程度
	//	'取值范围0--255,为0时就是一个全透明的窗体了
}

void YunLai::WindowTransparentFade(HWND dstHwnd, int minShow /*= 0*/, int speed /*= -15*/)
{
	long rtn = GetWindowLong(dstHwnd, GWL_EXSTYLE);	//    '取的窗口原先的样式
	rtn = rtn | WS_EX_LAYERED;						//    '使窗体添加上新的样式WS_EX_LAYERED
	SetWindowLong(dstHwnd, GWL_EXSTYLE, rtn);		//     '把新的样式赋给窗体
	for (int i = 255; i > minShow; i += speed)
	{
		SetLayeredWindowAttributes(dstHwnd, 0, i, LWA_ALPHA); //'把窗体设置成半透明样式,第二个参数表示透明程度	'取值范围0--255,为0时就是一个全透明的窗体了		
	}
	SetLayeredWindowAttributes(dstHwnd, 0, 0, LWA_ALPHA);
}

HWND YunLai::FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.best_handle;
}

BOOL YunLai::IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 /*&& IsWindowVisible(handle)*/;
}

BOOL YunLai::EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	//if (data.process_id != process_id || !IsMainWindow(handle)) {
	//	return TRUE;
	//}
	if (data.process_id == process_id && IsMainWindow(handle))
	{
		data.best_handle = handle;
		return FALSE;
	}
	return TRUE;

}

bool YunLai::WriteTextToWnd(HWND hwnd, int x, int y, const char* szText, DWORD color, int fontSize, char* fontName, bool bold, bool italic, bool underLine)
{
	HDC pDC = GetDC(hwnd);
	char szFontName[32] = "";
	strcpy(szFontName, fontName);
	if (strlen(fontName) < 1)
		strcpy(szFontName, "宋体");
	LOGFONTA struFont;
	memset(&struFont, 0, sizeof(LOGFONTA));
	struFont.lfHeight = fontSize;
	if (bold)
		struFont.lfWeight = 900;
	else
		struFont.lfWeight = 0;
	if (italic)
		struFont.lfItalic = 1;
	else
		struFont.lfItalic = 0;
	if (underLine)
		struFont.lfUnderline = 1;
	else
		struFont.lfUnderline = 0;
	struFont.lfCharSet = 134;
	strcpy(struFont.lfFaceName, szFontName);
	HFONT hFont = CreateFontIndirectA(&struFont);
	HGDIOBJ hGDIObj = SelectObject(pDC, hFont);
	int backMode = SetBkMode(pDC, 1);
	COLORREF oldColor = SetTextColor(pDC, color);
	TextOutA(pDC, x, y, szText, strlen(szText));
	SetBkMode(pDC, backMode);			//还原回去
	SetTextColor(pDC, oldColor);
	SelectObject(pDC, hGDIObj);
	DeleteObject(hFont);
	ReleaseDC(hwnd, pDC);
	return true;
}

void YunLai::SendKeyToWnd(HWND hwnd, int nKey, int nStatus, bool bInherit /*= false*/)
{
	EnumChildWindowData data;
	data.hwnd = hwnd;
	data.status = nStatus;
	data.key = nKey;
	EnumChildWindowsCallBack(hwnd, (LPARAM)&data);
	if (bInherit)
		EnumChildWindows(hwnd, EnumChildWindowsCallBack, (LPARAM)&data);
}
BOOL YunLai::EnumChildWindowsCallBack(HWND hwnd, LPARAM lParam)
{
	EnumChildWindowData& data = *(EnumChildWindowData*)lParam;
	if (hwnd != nullptr)
	{
		switch (data.status)
		{
		case 1:PostMessage(hwnd, 258, data.key, 0); break;
		case 2:
		{
			PostMessage(hwnd, 260, data.key, 0);
			PostMessage(hwnd, 261, data.key, 0);
			break;
		}
		case 3:PostMessage(hwnd, 256, data.key, 0);	break;
		case 4:PostMessage(hwnd, 257, data.key, 0);	break;
		default:
		{
			PostMessage(hwnd, 256, data.key, 0);
			PostMessage(hwnd, 257, data.key, 0);
			break;
		}
		}
		return TRUE;
	}
	return FALSE;
}

int YunLai::GetColorDepth()
{
	DEVMODEW dMode;
	dMode.dmSize = sizeof(DEVMODE);
	dMode.dmDriverExtra = 0;
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dMode);
	return dMode.dmBitsPerPel;//以像素的位数为单位。例如，16色使用4位，256色使用8位，而65536色使用16位。
}
//屏幕取颜色, 整数型, , 取屏幕中指定坐标的颜色, 或屏幕位图中指定窗口中坐标的颜色.(返回10进制颜色值)
//.参数 水平位置, 整数型, , 指定点横坐标
//.参数 垂直位置, 整数型, , 指定点纵坐标
//.参数 窗口句柄, 整数型, 可空, 指定窗口句柄后, 则获取窗口客户区内指定坐标点颜色值
DWORD YunLai::GetScreenColor(int x, int y, HWND hwnd)
{
	POINT screenPoint;
	if (hwnd)
	{
		ClientToScreen(hwnd, &screenPoint);
	}
	HDC pHDC = GetDC(nullptr);//屏幕的
	DWORD dColor = GetPixel(pHDC, x + screenPoint.x, y + screenPoint.y);
	ReleaseDC(nullptr, pHDC);
	return dColor;
}
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
DWORD YunLai::GetScreenColorCapture(int x, int y, HWND hwnd/*=nullptr*/)
{
	HBITMAP hBitMap = CopyScreenToBitmap(hwnd);
	if (hBitMap == nullptr)
		return 0;
	BITMAP bitmap;
	int nWidth, nHeight;
	GetObject(hBitMap, sizeof(BITMAP), &bitmap);
	nWidth = bitmap.bmWidth;
	nHeight = bitmap.bmHeight;
	///////////////
	HDC hMemDC = CreateCompatibleDC(nullptr);
	if (hMemDC == nullptr)
		return 0;
	HGDIOBJ hOldObject = SelectObject(hMemDC, hBitMap);
	DWORD colorVal = GetPixel(hMemDC, x, y);
	DeleteObject(hBitMap);
	//DWORD dwSize = bitmap.bmHeight * bitmap.bmWidthBytes;
	//unsigned char* pBits = new unsigned char[dwSize];
	//LONG dl = GetBitmapBits(hBitMap, dwSize, pBits);
	//int nTempX = pBits[19] + pBits[20] * 256;
	//int nTempY = pBits[23] + pBits[24] * 256;
	//if (x > nTempX || y > nTempY || x < 0 || y < 0)
	//{
	//	return -1;//坐标点位与窗口之外
	//}
	//int nTransX = (((nTempY - y) * nTempX + x) * 4) + 1;
	//char bmpColor[4]="";
	//bmpColor[0] = pBits[54 + nTempX + 2];
	//bmpColor[1] = pBits[54 + nTempX + 1];
	//bmpColor[2] = pBits[54 + nTempX ];
	//QString szVal = bmpColor;	
	//DWORD nColor = atoi((const char*)bmpColor);
	//qDebug() << bmpColor[0] << bmpColor[1] << bmpColor[2] << nColor << szVal.toInt();
	//delete[] pBits;
	return colorVal;
}

POINT YunLai::FindBMPColor(uchar* pData, DWORD nColor, int nDVal, POINT nFindPoint)
{
	return POINT();
	/*.局部变量 匿名局部变量_6697, 匿名数据类型_6732, , ,
		.局部变量 匿名局部变量_6698, 整数型, , ,
		.局部变量 匿名局部变量_6699, 坐标型, , ,
		.局部变量 匿名局部变量_6700, 整数型, , ,
		.局部变量 匿名局部变量_6701, 字节集, , ,
		.局部变量 匿名局部变量_6702, 字节集, , ,

		赋值(匿名局部变量_6699.x, -1)
		赋值(匿名局部变量_6699.y, -1)
		赋值(匿名局部变量_6697, 位图取色深(字节集))
		.如果真(等于(匿名局部变量_6697.匿名成员6735, 0))
		返回(匿名局部变量_6699)
		.如果真结束
		.如果(或者(小于(起始坐标.x, 1), 小于(起始坐标.y, 1)))
		赋值(匿名局部变量_6698, 55)
		.否则
		赋值(匿名局部变量_6698, 相加(相乘(相加(相乘(相减(匿名局部变量_6697.匿名成员6734, 起始坐标.y, 1), 匿名局部变量_6697.匿名成员6733), 起始坐标.x), 匿名局部变量_6697.匿名成员6737), 55, 匿名局部变量_6697.匿名成员6737))
		.如果结束
		赋值(匿名局部变量_6701, 到字节集(颜色))
		.如果(大于(误差, 0))
		.变量循环首(匿名局部变量_6698, 相减(取字节集长度(字节集), 2), 匿名局部变量_6697.匿名成员6737, 匿名局部变量_6700)
		.如果真(并且(小于(取绝对值(相减(字节集[相加(匿名局部变量_6700, 2)], 匿名局部变量_6701[1])), 相加(误差, 1)), 小于(取绝对值(相减(字节集[相加(匿名局部变量_6700, 1)], 匿名局部变量_6701[2])), 相加(误差, 1)), 小于(取绝对值(相减(字节集[匿名局部变量_6700], 匿名局部变量_6701[3])), 相加(误差, 1))))
		赋值(匿名局部变量_6699.x, 相减(求余数(相加(相除(相减(匿名局部变量_6700, 55), 匿名局部变量_6697.匿名成员6737), 1), 匿名局部变量_6697.匿名成员6733), 1))
		赋值(匿名局部变量_6699.y, 相减(匿名局部变量_6697.匿名成员6734, 整除(相加(相除(相减(匿名局部变量_6700, 55), 匿名局部变量_6697.匿名成员6737), 1), 匿名局部变量_6697.匿名成员6733), 1))
		返回(匿名局部变量_6699)
		.如果真结束

		.变量循环尾()
		返回(匿名局部变量_6699)
		.否则
		赋值(匿名局部变量_6702, { 0, 0, 0 })
		赋值(匿名局部变量_6702[1], 匿名局部变量_6701[3])
		赋值(匿名局部变量_6702[2], 匿名局部变量_6701[2])
		赋值(匿名局部变量_6702[3], 匿名局部变量_6701[1])
		赋值(匿名局部变量_6700, 寻找字节集(字节集, 匿名局部变量_6702, 匿名局部变量_6698))
		.判断循环首(大于(求余数(相减(匿名局部变量_6700, 55), 匿名局部变量_6697.匿名成员6737), 0))
		.如果真(等于(匿名局部变量_6700, -1))
		返回(匿名局部变量_6699)
		.如果真结束
		赋值(匿名局部变量_6700, 寻找字节集(字节集, 匿名局部变量_6702, 相加(匿名局部变量_6700, 1)))
		.判断循环尾()
		.如果真(等于(匿名局部变量_6700, -1))
		输出调试文本(匿名局部变量_6697.匿名成员6733)
		输出调试文本(匿名局部变量_6700)
		返回(匿名局部变量_6699)
		.如果真结束
		赋值(匿名局部变量_6699.x, 相减(求余数(相加(相除(相减(匿名局部变量_6700, 55), 匿名局部变量_6697.匿名成员6737), 1), 匿名局部变量_6697.匿名成员6733), 1))
		赋值(匿名局部变量_6699.y, 相减(匿名局部变量_6697.匿名成员6734, 整除(相加(相除(相减(匿名局部变量_6700, 55), 匿名局部变量_6697.匿名成员6737), 1), 匿名局部变量_6697.匿名成员6733), 1))
		返回(匿名局部变量_6699)
		.如果结束*/

}

POINT YunLai::FindScreenColor(DWORD nColor, int nLeftX, int nLeftY, int nWidth, int nHeight, int nDVal, POINT nFindPoint, HWND hwnd/*=nullptr*/)
{
	//	return FindBMPColor(nColor);
	return POINT();
}

char* YunLai::ReadMemoryStrFromProcessID(DWORD processID, const char* szAddress, int nLen)
{
	bool bRet = false;
	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	char pLen[MAX_MEMORY_TEXT_SIZE] = "";
	if (nLen > MAX_MEMORY_TEXT_SIZE)
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, pLen, MAX_MEMORY_TEXT_SIZE, 0);
	else
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, pLen, nLen, 0);
	//	qDebug()<<processID << szAddress << pLen << bRet << pAddress;
	return pLen;
}

char* YunLai::ReadMemoryStrFromProcessID(DWORD processID, DWORD pAddress, int nLen)
{
	bool bRet = false;
	char pLen[MAX_MEMORY_TEXT_SIZE] = "";
	if (nLen > MAX_MEMORY_TEXT_SIZE)
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), (LPCVOID)pAddress, pLen, MAX_MEMORY_TEXT_SIZE, 0);
	else
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), (LPCVOID)pAddress, pLen, nLen, 0);
	//	qDebug()<<processID << szAddress << pLen << bRet << pAddress;
	return pLen;
}

char* YunLai::ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress, int nLen)
{
	DWORD hProcessID = GetProcessIDFromWnd(hwnd);
	return ReadMemoryStrFromProcessID(hProcessID, szAddress, nLen);
}
const char* YunLai::ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress, const char* offset1, const char* offset2, const char* offset3, const char* offset4)
{
	DWORD hProcessID = GetProcessIDFromWnd(hwnd);

	/*int nAddress = ReadMemoryIntFromProcessID(hProcessID, HexTextToDecimalText(szAddress));
	if (strlen(offset1) < 1)
		return nAddress;
	*	赋值(匿名局部变量_3400, 内存读整数(匿名局部变量_3397, 转换十六到十(基址)))
		.如果真(是否为空(偏移1))
		返回(匿名局部变量_3400)
		.如果真结束
		赋值(匿名局部变量_3400, 内存读整数(匿名局部变量_3397, 相加(匿名局部变量_3400, 转换十六到十(偏移1))))
		.如果真(是否为空(偏移2))
		返回(匿名局部变量_3400)
		.如果真结束
		赋值(匿名局部变量_3400, 内存读整数(匿名局部变量_3397, 相加(匿名局部变量_3400, 转换十六到十(偏移2))))
		.如果真(是否为空(偏移3))
		返回(匿名局部变量_3400)
		.如果真结束
		赋值(匿名局部变量_3400, 内存读整数(匿名局部变量_3397, 相加(匿名局部变量_3400, 转换十六到十(偏移3))))
		.如果真(是否为空(偏移4))
		返回(匿名局部变量_3400)
		.如果真结束
		赋值(匿名局部变量_3400, 内存读整数(匿名局部变量_3397, 相加(匿名局部变量_3400, 转换十六到十(偏移4))))
		返回(匿名局部变量_3400)*/
	return "";
}

int YunLai::ReadMemoryIntFromWnd(HWND hwnd, const char* szAddress)
{
	//PROCESS_ALL_ACCESS = 所有权限 = 2035711
	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	DWORD hProcessID = GetProcessIDFromWnd(hwnd);
	int memoryData = 0;
	bool bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID), pAddress, &memoryData, 4, 0);
	//qDebug() << hProcessID << szAddress << memoryData << bRet << pAddress;
	return memoryData;
}

int YunLai::ReadMemoryIntFromProcessID(DWORD processID, const char* szAddress)
{
	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	int memoryData = 0;
	bool bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, &memoryData, 4, 0);
	//qDebug() << processID << szAddress << memoryData << bRet << pAddress;
	return memoryData;
}

int YunLai::ReadMemoryIntFromProcessID(DWORD processID, DWORD pAddress)
{
	int memoryData = 0;
	bool bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), (LPCVOID)pAddress, &memoryData, 4, 0);
	//qDebug() << processID << szAddress << memoryData << bRet << pAddress;
	return memoryData;
}

WORD YunLai::ReadMemoryWordFromProcessID(DWORD processID, DWORD pAddress)
{
	WORD memoryData = 0;
	bool bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), (LPCVOID)pAddress, &memoryData, 2, 0);
	//qDebug() << processID << szAddress << memoryData << bRet << pAddress;
	return memoryData;
}

void YunLai::WriteMemoryIntToWnd(HWND hwnd, const char* szAddress, int nVal)
{
	//	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	LPVOID pAddress = (LPVOID)strtoul(szAddress, NULL, 16);
	DWORD hProcessID = GetProcessIDFromWnd(hwnd);
	HANDLE hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID);
	WriteProcessMemory(hProcessHandle, pAddress, &nVal, 4, 0);
}
void YunLai::WriteMemoryIntToProcess(DWORD hProcessID, const char* szAddress, int nVal)
{
	LPVOID pAddress = (LPVOID)strtoul(szAddress, NULL, 16);
	HANDLE hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID);
	WriteProcessMemory(hProcessHandle, pAddress, &nVal, 4, 0);
}

bool YunLai::KillProcess(HWND hwnd)
{
	bool bRet = false;
	if (!hwnd)
		return bRet;

	if (!IsWindow(hwnd))
		return bRet;

	DWORD pid, tid;
	tid = GetWindowThreadProcessId(hwnd, &pid);
	if (!pid || !tid)
		return bRet;

	HANDLE ProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (ProcessHandle)
	{
		if (STATUS_SUCCESS == NtTerminateProcess(ProcessHandle, 0))
		{
			bRet = true;
		}
		CloseHandle(ProcessHandle);
	}
	return bRet;

}

bool YunLai::KillProcessEx(DWORD hProcessID)
{
	bool bRet = false;
	HANDLE ProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, hProcessID);
	if (ProcessHandle)
	{
		if (STATUS_SUCCESS == NtTerminateProcess(ProcessHandle, 0))
		{
			bRet = true;
		}
		CloseHandle(ProcessHandle);
	}
	return bRet;
}

//
//long YunLai::ForceOpenProcess(DWORD dwDesiredAccess, bool bInhert,DWORD ProcessId)
//{
//	bool bRet = false;
//	POBJECT_ATTRIBUTES attribute;
//	memset(&attribute, 0, sizeof(POBJECT_ATTRIBUTES));
//	attribute->Length = 24;
//	if (bInhert)
//		attribute->Attributes = attribute->Attributes|2;
//	HANDLE hProcessHandle;
//	PCLIENT_ID clientID;
//	clientID->UniqueProcess =(HANDLE)(ProcessId + 1);
//    NTSTATUS openRet = ZwOpenProcess_Import()(&hProcessHandle, dwDesiredAccess, attribute, clientID);
//	if (openRet >= 0)
//		return openRet;
//	/*赋值(匿名局部变量_7587, 1)
//		.循环判断首()
//		赋值(匿名局部变量_7588, 取空白字节集(匿名局部变量_7587))
//		匿名子程序_27995(“ntdll.dll”, “ZwQuerySystemInformation”)
//		赋值(匿名局部变量_7592, ZwQuerySystemInformation_5853(16, 匿名局部变量_7588, 匿名局部变量_7587, 0))
//		.如果(等于(匿名局部变量_7592, #匿名常量_5852))
//		赋值(匿名局部变量_7587, 相乘(匿名局部变量_7587, 2))
//		赋值(匿名局部变量_7588, 取空白字节集(匿名局部变量_7587))
//		.否则
//		跳出循环()
//		.如果结束
//
//		.循环判断尾(等于(匿名局部变量_7592, #匿名常量_5852))
//		赋值(匿名局部变量_7589, lstrcpyn_1750(匿名局部变量_7588, 匿名局部变量_7588, 0))
//		RtlMoveMemory_5858(匿名局部变量_7578, 匿名局部变量_7589, 4)
//		赋值(匿名局部变量_7589, 相加(匿名局部变量_7589, 4))
//		.计次循环首(匿名局部变量_7578, 匿名局部变量_7580)
//		RtlMoveMemory_7672(匿名局部变量_7585, 匿名局部变量_7589, 16)
//		.如果真(等于(匿名局部变量_7585.匿名成员7654, #匿名常量_7677))
//		赋值(匿名局部变量_7576.匿名成员7635, 匿名局部变量_7585.匿名成员7652)
//		匿名子程序_27995(“ntdll.dll”, “ZwOpenProcess”)
//		赋值(匿名局部变量_7575, ZwOpenProcess_7665(匿名局部变量_7581, #匿名常量_1092, 匿名局部变量_7577, 匿名局部变量_7576))
//		.如果真(大于或等于(匿名局部变量_7575, 0))
//		匿名子程序_27995(“ntdll.dll”, “ZwDuplicateObject”)
//		赋值(匿名局部变量_7575, ZwDuplicateObject_7678(匿名局部变量_7581, 匿名局部变量_7585.匿名成员7656, #匿名常量_7686, 匿名局部变量_7582, #匿名常量_1096, 0, #匿名常量_7689))
//		.如果真(大于或等于(匿名局部变量_7575, 0))
//		匿名子程序_27995(“ntdll.dll”, “ZwQueryInformationProcess”)
//		赋值(匿名局部变量_7575, ZwQueryInformationProcess_7690(匿名局部变量_7582, 0, 匿名局部变量_7579, 24, 0))
//		.如果真(大于或等于(匿名局部变量_7575, 0))
//		.如果真(等于(匿名局部变量_7579.匿名成员7649, ProcessId))
//		匿名子程序_27995(“ntdll.dll”, “ZwDuplicateObject”)
//		赋值(匿名局部变量_7575, ZwDuplicateObject_7678(匿名局部变量_7581, 匿名局部变量_7585.匿名成员7656, #匿名常量_7686, 匿名局部变量_7583, dwDesiredAccess, #匿名常量_7659, #匿名常量_7689))
//		.如果真(大于或等于(匿名局部变量_7575, 0))
//		赋值(匿名局部变量_7592, 匿名局部变量_7583)
//		.如果真结束
//
//		.如果真结束
//
//		.如果真结束
//
//		.如果真结束
//		匿名子程序_27995(“ntdll.dll”, “ZwClose”)
//		赋值(匿名局部变量_7575, ZwClose_7697(匿名局部变量_7582))
//		.如果真结束
//		匿名子程序_27995(“ntdll.dll”, “ZwClose”)
//		赋值(匿名局部变量_7575, ZwClose_7697(匿名局部变量_7581))
//		.如果真结束
//		赋值(匿名局部变量_7589, 相加(匿名局部变量_7589, 16))
//		.计次循环尾()
//		返回(匿名局部变量_7592)*/
//}
//
//bool YunLai::ForceCloseProcess(DWORD hProcessID, DWORD nExitStatus)
//{
//	bool bRet = false;
//	if (ZwCreateJobObject_Import() == nullptr)
//		return false;
//	HANDLE hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID);
//
//	PHANDLE jobHandle;
//	ACCESS_MASK accessMask = 2031647;
//	POBJECT_ATTRIBUTES attribute ;
//	memset(&attribute, 0, sizeof(POBJECT_ATTRIBUTES));
//	attribute->Length = 24;
//
//	NTSTATUS jobObj = ZwCreateJobObject_Import()(jobHandle, accessMask, attribute);
//	if (jobObj >= 0)
//	{
//		NTSTATUS assignProcessObj = ZwAssignProcessToJobObject_Import()((HANDLE)jobObj, hProcessHandle);
//		if (assignProcessObj >= 0)
//		{
//			NTSTATUS terminateJobRet =ZwTerminateJobObject_Import()((HANDLE)jobObj,nExitStatus);
//			if (terminateJobRet >= 0)
//			{
//				bRet = true;
//			}
//		}
//		ZwClose_Import()((HANDLE)jobObj);
//	}
//	if (bRet==false)
//	{
//		NTSTATUS terminateProcessRet = ZwTerminateProcess_Import()(hProcessHandle, nExitStatus);
//		if (terminateProcessRet >= 0)
//		{
//			bRet = true;
//		}
//	}
//	return bRet;		
//}
//获取库句柄  参数 库名  系统库是库名 否则全路径
HMODULE YunLai::GetLibraryHandle(const char* szLib)
{
	HMODULE hModule = ::GetModuleHandle((LPCWSTR)szLib);
	if (NULL == hModule) hModule = ::LoadLibrary((LPCWSTR)szLib);
	if (NULL == hModule) throw;
	return hModule;
}

FARPROC YunLai::GetFunAddress(const char* szLib, const char* szFun)
{
	HMODULE hModel = GetLibraryHandle(szLib);
	if (hModel == nullptr)
		return nullptr;
	FARPROC funAddr = GetProcAddress(hModel, (LPCSTR)szFun);
	if (funAddr == nullptr)
		return nullptr;
	return funAddr;
}

string YunLai::GetKeyTextFromKey(int nKey)
{
	switch (nKey)
	{
	case VK_F1:return "F1";
	case VK_F2:return "F2";
	case VK_F3:return "F3";
	case VK_F4:return "F4";
	case VK_F5:return "F5";
	case VK_F6:return "F6";
	case VK_F7:return "F7";
	case VK_F8:return "F8";
	case VK_F9:return "F9";
	case VK_F10:return "F10";
	case VK_F11:return "F11";
	case VK_F12:return "F12";
	default:; break;
	}
	return "";
}

void YunLai::KeyClickedEvent()
{
	//PostMessage(aHandle, WM_char, ord(s[i]), 0); //发送字符
	keybd_event(13, 0, 0, 0);//回车
}

int YunLai::MakeKeyLParam(int VirtualKey, int flag)
{
	UINT sCode;
	//Firstbyte ; lparam 参数的 24-31位
	UINT Firstbyte;
	switch (flag)
	{
	case WM_KEYDOWN:    Firstbyte = 0;   break;
	case WM_KEYUP:      Firstbyte = 0xC0; break;
	case WM_CHAR:       Firstbyte = 0x20; break;
	case WM_SYSKEYDOWN: Firstbyte = 0x20; break;
	case WM_SYSKEYUP:   Firstbyte = 0xE0; break;
	case WM_SYSCHAR:    Firstbyte = 0xE0; break;
	}
	// 键的扫描码; lparam 参数 的 16-23位
	// 16–23 Specifies the scan code. 
	UINT iKey = MapVirtualKeyW(VirtualKey, 0);
	// 1为 lparam 参数的 0-15位，即发送次数
	// 0–15 Specifies the repeat count for the current message. 
	sCode = (Firstbyte << 24) + 1 + (iKey << 16) + 1;
	return sCode;
}

void YunLai::SimKeyClick(UINT vk_Code, BOOL bDown)
{
	DWORD dwFlages = 0;
	switch (vk_Code)
	{
	default:
		break;
	case(VK_NUMLOCK):
	case(VK_CAPITAL):
	case(VK_SCROLL):
	case(VK_CONTROL):
	case(VK_LCONTROL):
	case(VK_RCONTROL):
	case(VK_SHIFT):
	case(VK_LSHIFT):
	case(VK_RSHIFT):
	case(VK_MENU):
	case(VK_LMENU):
	case(VK_RMENU):
		dwFlages |= KEYEVENTF_EXTENDEDKEY;
	}
	WORD wScan = MapVirtualKeyW(vk_Code, 0);
	INPUT Input[1] = { 0 };
	Input[0].type = INPUT_KEYBOARD;
	Input[0].ki.wVk = vk_Code;
	Input[0].ki.wScan = wScan;
	Input[0].ki.dwFlags = (bDown) ? dwFlages : dwFlages | KEYEVENTF_KEYUP;
	SendInput(1, Input, sizeof(INPUT));
}
//整数型, 可空, 可空:为左键  1 #左键   2 #右键   3 #中键
//.参数 控制, 整数型, 可空, 可空:为单击  1 #单击   2 #双击   3 #按下  4 #放开
void YunLai::MouseClickedEvent(int nType, int nCtrl)
{
	DWORD mouseDown = 2;//MOUSEEVENTF_LEFTDOWN
	DWORD mouseUp = 4;//MOUSEEVENTF_LEFTUP
	switch (nType)
	{
	case 1:
	{
		mouseDown = 2;
		mouseUp = 4;
		break;
	}
	case 2:
	{
		mouseDown = 8;
		mouseUp = 16;
		break;
	}
	case 3:
	{
		mouseDown = 32;
		mouseUp = 64;
		break;
	}
	default:
		break;
	}
	switch (nCtrl)
	{
	case 2:
	{
		mouse_event(mouseDown, 0, 0, 0, 0);
		mouse_event(mouseUp, 0, 0, 0, 0);
		uint clickTime = GetDoubleClickTime();
		Sleep(clickTime);
		mouse_event(mouseDown, 0, 0, 0, 0);
		mouse_event(mouseUp, 0, 0, 0, 0);
		break;
	}
	case 3:
	{
		mouse_event(mouseDown, 0, 0, 0, 0);	break;
	}
	case 4:
	{
		mouse_event(mouseUp, 0, 0, 0, 0);	break;
	}
	case 1:
	default:
	{
		mouse_event(mouseDown, 0, 0, 0, 0);
		mouse_event(mouseUp, 0, 0, 0, 0);
		break;
	}
	}
}
//模拟鼠标点击一次
void YunLai::SimMouseClick(int nType, int nCtrl)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dx = 0;
	input.mi.dy = 0;
	input.mi.mouseData = 0;
	//input.mi.dwFlags = bDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
	input.mi.time = 0;
	input.mi.dwExtraInfo = 0;

	DWORD mouseDown = MOUSEEVENTF_LEFTDOWN;
	DWORD mouseUp = MOUSEEVENTF_LEFTUP;
	switch (nType)
	{
	case 1:
	{
		mouseDown = MOUSEEVENTF_LEFTDOWN;
		mouseUp = MOUSEEVENTF_LEFTUP;
		break;
	}
	case 2:
	{
		mouseDown = MOUSEEVENTF_RIGHTDOWN;
		mouseUp = MOUSEEVENTF_RIGHTUP;
		break;
	}
	case 3:
	{
		mouseDown = MOUSEEVENTF_MIDDLEDOWN;
		mouseUp = MOUSEEVENTF_MIDDLEUP;
		break;
	}
	default:
		break;
	}
	switch (nCtrl)
	{
	case 2:
	{
		input.mi.dwFlags = mouseDown;
		SendInput(1, &input, sizeof(INPUT));
		input.mi.dwFlags = mouseUp;
		SendInput(1, &input, sizeof(INPUT));
		uint clickTime = GetDoubleClickTime();
		Sleep(clickTime);
		input.mi.dwFlags = mouseDown;
		SendInput(1, &input, sizeof(INPUT));
		input.mi.dwFlags = mouseUp;
		SendInput(1, &input, sizeof(INPUT));
		break;
	}
	case 3:
	{
		input.mi.dwFlags = mouseDown;
		SendInput(1, &input, sizeof(INPUT));
		break;

	}
	case 4:
	{
		input.mi.dwFlags = mouseUp;
		SendInput(1, &input, sizeof(INPUT));
		break;
	}
	case 1:
	default:
	{
		input.mi.dwFlags = mouseDown;
		SendInput(1, &input, sizeof(INPUT));
		input.mi.dwFlags = mouseUp;
		SendInput(1, &input, sizeof(INPUT));
		break;
	}
	}
}

//超级延时, , , 高精度延时, cpu占用低, 窗口不卡死, 一次最大可延时几年(无返回值)
//.参数 延时间隔, 整数型, , 1000微秒 = 1毫秒 ； 1000毫秒 = 1秒
//.参数 延时单位, 整数型, 可空, 可空:毫秒  0 毫秒  1 微秒  2 秒  3 分  4 小时  5 天
void YunLai::SuperTimeDelay(int timeDelay, int timeType)
{
	int nTimeUnit = 0;
	if (timeType == 0)
		nTimeUnit = 1;
	else if (timeType == 1)//微妙 直接调用
	{
		HANDLE hHandle = CreateWaitableTimerA(0, false, 0);
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = timeDelay * (-10);
		SetWaitableTimer(hHandle, &liDueTime, 0, 0, 0, false);
		if (MsgWaitForMultipleObjects(1, &hHandle, false, -1, 255) != 0)
		{
			//		处理事件()
		}
		CloseHandle(hHandle);
	}
	else if (timeType == 2)
		nTimeUnit = 1000;
	else if (timeType == 3)
		nTimeUnit = 1000 * 60;
	else if (timeType == 4)
		nTimeUnit = 1000 * 60 * 60;
	else if (timeType == 5)
		nTimeUnit = 1000 * 60 * 60 * 24;
	for (int i = 0; i < nTimeUnit; ++i)
	{
		HANDLE hHandle = CreateWaitableTimerA(0, false, 0);
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = timeDelay * (-10) * 1000;
		SetWaitableTimer(hHandle, &liDueTime, 0, 0, 0, false);
		if (MsgWaitForMultipleObjects(1, &hHandle, false, INFINITE, QS_ALLINPUT) != 0)
		{
			//		处理事件()
		}
		CloseHandle(hHandle);
		break;
	}
}

char* YunLai::HexTextToDecimalText(const char* szHex)
{
	int nAdd = atoi(szHex);
	char szAddr[25];
	itoa(nAdd, szAddr, 16);
	return szAddr;
}

char* YunLai::MyItoa(int num, char* str, int radix)
{/*索引表*/
	char index[] = "0123456789ABCDEF";
	unsigned unum;/*中间变量*/
	int i = 0, j, k;
	/*确定unum的值*/
	if (radix == 10 && num < 0)/*十进制负数*/
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else unum = (unsigned)num;/*其他情况*/
	/*转换*/
	do {
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';
	/*逆序*/
	if (str[0] == '-')
		k = 1;/*十进制负数*/
	else
		k = 0;

	for (j = k; j <= (i - 1) / 2; j++)
	{
		char temp;
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}

uchar* YunLai::ScreenCapture(HWND hwnd, int nLeft/*=0*/, int nTop/*=0*/, int nWidth, int nHeight, bool bClient/*=false*/)
{
	int nCapLeft = nLeft;
	int nCapTop = nTop;
	int nCapWidth = nWidth;
	int nCapHeight = nHeight;
	HDC hDC = ::GetDC(hwnd);
	if (hDC)
	{
		HDC hMemDC = ::CreateCompatibleDC(hDC);
		if (hMemDC)
		{
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			HBITMAP hbitmap = ::CreateCompatibleBitmap(hDC, rc.right - rc.left, rc.bottom - rc.top);
			if (hbitmap)
			{
				HGDIOBJ gidObj = ::SelectObject(hMemDC, hbitmap);
				if (::PrintWindow(hwnd, hMemDC, 0) == FALSE)
				{
					::DeleteObject(hbitmap);
					::DeleteObject(hMemDC);
					::ReleaseDC(hwnd, hDC);
					return nullptr;
				}
				else
				{
					RECT clientRect;
					POINT clientPoint;
					if (bClient)
					{
						clientPoint.x = clientPoint.y = 0;
						//连续赋值(0, 匿名局部变量_6492.矩形宽度, 匿名局部变量_6492.矩形高度, 匿名局部变量_6493.x, 匿名局部变量_6493.y)
						GetClientRect(hwnd, &clientRect);
						ClientToScreen(hwnd, &clientPoint);
						RECT hwndOutRect;
						GetWindowRect(hwnd, &hwndOutRect);//外边框
						nCapLeft = abs(clientPoint.x - hwndOutRect.left) + nCapLeft;
						nCapTop = abs(clientPoint.y - hwndOutRect.top) + nCapTop;
						nCapHeight = nCapHeight < clientRect.bottom ? nCapHeight : clientRect.bottom;
						nCapWidth = nCapHeight < clientRect.right ? nCapWidth : clientRect.right;
					}
					//HDC hCapMemDC = ::CreateCompatibleDC(hDC);
					//HBITMAP hCapBitMap = ::CreateCompatibleBitmap(hDC, nCapWidth, nCapHeight);
					//::SelectObject(hCapMemDC, hCapBitMap);
					//::BitBlt(hCapMemDC, 0, 0, nCapWidth, nCapHeight, hMemDC, 0, 0, SRCCOPY);
					//SelectObject(hMemDC, gidObj);
					//DeleteObject(hbitmap);
					//DeleteDC(hMemDC);
					//BITMAP bitmap;
					//GetObjectA(hCapBitMap, 5 * 4 + 2 + 2, &bitmap);
					//BITMAPINFO bitmapInfo;
					//GetDIBits(hCapMemDC, hCapBitMap,0,0,0,&bitmapInfo,0);
					////memmove();
					//uchar* pData = new uchar[BITMAPINFO.];
					//GetDIBits(hCapMemDC, hCapBitMap,0,bitmap.bmHeight,
				}
			}
			::DeleteObject(hMemDC);
		}
		::ReleaseDC(hwnd, hDC);
	}
	return nullptr;

}

//
//bool YunLai::DIBSnapshot(HWND hWnd, int scale, const SnapshotCallback &callback)
//{
//	HDC hDC = GetDC(hWnd);
//	if (!hDC)
//	{
//		return false;
//	}
//
//	const auto hDCScope = std::experimental::make_scope_exit([hWnd, hDC] { ReleaseDC(hWnd, hDC); });
//
//	RECT rcScreen;
//
//	if (hWnd == GetDesktopWindow())
//	{
//		rcScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
//		rcScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
//		rcScreen.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//		rcScreen.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
//	}
//	else
//	{
//		GetWindowRect(hWnd, &rcScreen);
//	}
//
//	int destW = (scale == 100) ? (rcScreen.right - rcScreen.left) : (rcScreen.right - rcScreen.left) * scale / 100;
//	int destH = (scale == 100) ? (rcScreen.bottom - rcScreen.top) : (rcScreen.bottom - rcScreen.top) * scale / 100;
//
//	BITMAPINFOHEADER bi;
//	memset(&bi, 0, sizeof(bi));
//	bi.biSize = sizeof(bi);
//	bi.biWidth = destW;
//	bi.biHeight = -destH;
//	bi.biPlanes = 1;
//	bi.biBitCount = 32;
//	bi.biCompression = BI_RGB;
//
//	PVOID pv = NULL;
//	HBITMAP hBitmap = CreateDIBSection(hDC, (BITMAPINFO *)&bi, DIB_RGB_COLORS, &pv, 0, 0);
//	if (!hBitmap)
//	{
//		OutputDebugStringA("CreateDIBSection failed");
//		return false;
//	}
//	const auto hBitmapScope = std::experimental::make_scope_exit([hBitmap] { DeleteObject(hBitmap); });
//
//	HDC hMemDC = CreateCompatibleDC(hDC);
//	if (!hMemDC)
//	{
//		OutputDebugStringA("CreateCompatibleDC failed");
//		return false;
//	}
//	const auto hMemDCScope = std::experimental::make_scope_exit([hMemDC] { DeleteDC(hMemDC); });
//
//	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBitmap);
//
//	const auto hOldBmpScope = std::experimental::make_scope_exit([hMemDC, hOldBmp] { SelectObject(hMemDC, hOldBmp); });
//
//	if (scale != 100)
//	{
//		SetStretchBltMode(hMemDC, STRETCH_HALFTONE);
//
//		if (!StretchBlt(hMemDC, 0, 0, destW, destH, hDC, 0, 0, rcScreen.right, rcScreen.bottom, SRCCOPY))
//		{
//			OutputDebugStringA("StretchBlt failed");
//			return false;
//		}
//	}
//	else
//	{
//		if (!BitBlt(hMemDC, 0, 0, rcScreen.right, rcScreen.bottom, hDC, 0, 0, SRCCOPY))
//		{
//			OutputDebugStringA("BitBlt failed");
//			return false;
//		}
//	}
//
//	BITMAP bitmap;
//	if (!GetObjectW(hBitmap, sizeof(BITMAP), &bitmap))
//	{
//		OutputDebugStringA("GetObjectW failed");
//		return false;
//	}
//	PVOID pBuffer = m_snapshot_buffer.GetSpace(bitmap.bmHeight * bitmap.bmWidthBytes);
//	if (!pBuffer)
//	{
//		OutputDebugStringA("m_snapshot_buffer.GetSpace failed");
//		return false;
//	}
//
//	if (GetBitmapBits(hBitmap, bitmap.bmHeight * bitmap.bmWidthBytes, pBuffer) <= 0)
//	{
//		OutputDebugStringA("GetBitmapBits failed");
//		return false;
//	}
//
//	callback(pBuffer, bitmap.bmHeight * bitmap.bmWidthBytes, bitmap.bmWidth, bitmap.bmHeight, bitmap.bmBitsPixel);
//
//	return true;
//}

DWORD YunLai::GetProcessIDFromWnd(HWND hwnd)
{
	DWORD hProcessID = 0;
	DWORD dwTID = GetWindowThreadProcessId(hwnd, &hProcessID);//返回线程号
	return hProcessID;
}

bool YunLai::ActiveWnd(HWND hwnd)
{
	DWORD hProcessID = GetWindowThreadProcessId(hwnd, 0);
	AttachThreadInput(GetCurrentThreadId(), hProcessID, true);
	SetActiveWindow(hwnd);
	AttachThreadInput(GetCurrentThreadId(), hProcessID, false);
	return SetForegroundWindow(hwnd);
}

bool YunLai::SetFocusToWnd(HWND hwnd)
{
	DWORD hProcessID = GetWindowThreadProcessId(hwnd, 0);
	AttachThreadInput(GetCurrentThreadId(), hProcessID, true);
	HWND focusWnd = SetFocus(hwnd);
	AttachThreadInput(GetCurrentThreadId(), hProcessID, false);
	if (GetFocus() == focusWnd)	//焦点窗口等于之前的焦点窗口 则设置失败
		return false;
	return true;
}

bool YunLai::IsVisibleWnd(HWND hwnd)
{
	return IsWindowVisible(hwnd);
}

bool YunLai::SetWindowShow(HWND hwnd)
{
	if (hwnd == nullptr)
	{
		if (m_lastHwnd == nullptr)
			m_lastHwnd;
	}
	else
		m_lastHwnd = hwnd;
	/*if (IsVisibleWnd(m_lastHwnd) == false)
	{
		::ShowWindow(m_lastHwnd, 9);
		return true;
	}
	return false;*/
	::ShowWindow(m_lastHwnd, 9);
	return true;
}
bool YunLai::SetWindowHide(HWND hwnd)
{
	if (hwnd == nullptr)
	{
		if (m_lastHwnd == nullptr)
			m_lastHwnd;
	}
	else
		m_lastHwnd = hwnd;
	//if (IsVisibleWnd(m_lastHwnd))
	//{
	//	::ShowWindow(m_lastHwnd, 0);
	//	return true;
	//}
	//return false;
	::ShowWindow(m_lastHwnd, 0);
	return true;
}


bool YunLai::SetWindowVal(HWND hwnd, int state)
{
	if (hwnd == nullptr)
	{
		if (m_lastHwnd == nullptr)
			m_lastHwnd;
	}
	else
		m_lastHwnd = hwnd;

	::ShowWindow(m_lastHwnd, state);
	return true;
}

//
//bool YunLai::DIBToCvMat(cv::Mat &mat, void *pBuffer, size_t cbBuffer, int width, int height, int bbp)
//{
//	mat.create(height, width, CV_8UC3);
//
//	int nChannels = bbp / 8;
//
//	int nStep = nChannels * width;
//
//	for (int nRow = 0; nRow < height; nRow++)
//	{
//		auto pucRow = (mat.ptr<uchar>(nRow));
//		for (int nCol = 0; nCol < width; nCol++)
//		{
//			pucRow[nCol * 3 + 0] = *((uchar *)pBuffer + nRow * nStep + nCol * nChannels + 0);
//			pucRow[nCol * 3 + 1] = *((uchar *)pBuffer + nRow * nStep + nCol * nChannels + 1);
//			pucRow[nCol * 3 + 2] = *((uchar *)pBuffer + nRow * nStep + nCol * nChannels + 2);
//		}
//	}
//
//	return true;
//}
//参数1：矩形RECT的指针即要截图的区域
HBITMAP YunLai::CopyScreenToBitmap(HWND pHwnd)
{
	HDC       hScrDC, hMemDC;// 屏幕和内存设备描述表	
	HBITMAP    hBitmap, hOldBitmap;// 位图句柄	
	int       nX, nY, nX2, nY2;// 选定区域坐标	
	int       nWidth, nHeight;// 确保选定区域不为空矩形

	RECT wndRect;
	GetClientRect(pHwnd, &wndRect);
	if (IsRectEmpty(&wndRect))
		return NULL;
	//为屏幕创建设备描述表
	hScrDC = GetDC(pHwnd);
	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = wndRect.left;
	nY = wndRect.top;
	nX2 = wndRect.right;
	nY2 = wndRect.bottom;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中	
	BitBlt(hMemDC, 0, 0, nWidth, nHeight,//截取选中区域
		hScrDC, nX, nY, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);//得到屏幕位图的句柄	
	//清除 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	/*time_t curTime = time(nullptr);
	char fileName[255];
	itoa(curTime,fileName,255);
	SaveBitmapToFile(hBitmap, fileName);*/
	return hBitmap;
}

bool YunLai::SaveBitmapToFile(HBITMAP hBitmap, string szfilename)
{
	HDC     hDC;
	//当前分辨率下每象素所占字节数            
	int     iBits;
	//位图中每象素所占字节数            
	WORD     wBitCount;
	//定义调色板大小，     位图中像素字节大小     ，位图文件大小     ，     写入文件字节数                
	DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//位图属性结构                
	BITMAP     Bitmap;
	//位图文件头结构            
	BITMAPFILEHEADER     bmfHdr;
	//位图信息头结构                
	BITMAPINFOHEADER     bi;
	//指向位图信息头结构                    
	LPBITMAPINFOHEADER     lpbi;
	//定义文件，分配内存句柄，调色板句柄                
	HANDLE     fh, hDib, hPal, hOldPal = NULL;

	//计算位图文件每个像素所占字节数                
	hDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else  if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//为位图内容分配内存                
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//     处理调色板                    
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	//     获取该调色板下新的像素值                
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	//恢复调色板                    
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件                    
	fh = CreateFileA(szfilename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)         return     FALSE;

	//     设置位图文件头                
	bmfHdr.bfType = 0x4D42;     //     "BM"                
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	//     写入位图文件头                
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//     写入位图文件其余内容                
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//清除                    
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return     TRUE;
}
