#include "GameData.h"
#include "../include/ITPublic.h"
#include "GlobalDefine.h"
#include "MLAssist.h"
#include "YunLai.h"
#include "stdafx.h"
#include <QMessageBox>
//#include "../include/XASM.h"
#include <QDebug>
//#include "../include/XEDParse.h"
//#include "./asmjit/asmjit.h"
//using namespace asmjit;
//using namespace asmjit::x86;
//#ifdef _WIN64
//#pragma comment (lib,"../lib/XEDParse/x64/XEDParse_x64.lib")
//#else
//#pragma comment (lib,"../lib/XEDParse/x32/XEDParse_x86.lib")
//#endif // _WIN64
GameData &GameData::getInstance()
{
	static GameData g_pGameData;
	return g_pGameData;
}

void GameData::initData()
{
	m_characterForVal.clear();
	m_characterForVal.insert("队", CHARACTER_Troop);
	m_characterForVal.insert("战", CHARACTER_Battle);
	m_characterForVal.insert("聊", CHARACTER_Chat);
	m_characterForVal.insert("名", CHARACTER_Card);
	m_characterForVal.insert("易", CHARACTER_Trade);
	m_characterForVal.insert("家", CHARACTER_Family);
	m_szPickSkillList.clear();
	m_indexForSkillName.clear();
	m_szPickSkillList << "鉴定"
					  << "挖矿"
					  << "伐木"
					  << "狩猎"
					  << "修理武器"
					  << "修理防具"
					  << "挖矿体验"
					  << "伐木体验"
					  << "狩猎体验";
	m_skillCodeForMyCode.insert(225, 104);
	m_skillCodeForMyCode.insert(226, 103);
	m_skillCodeForMyCode.insert(227, 102);
	//获取技能信息
	for (int i = 0; i < 15; ++i) //先10个  15个技能栏
	{
		DWORD pAddress = 0x00D84FEC;
		DWORD offset = i * 0x49FC;
		pAddress += offset;
		QString skillName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 100)));
		DWORD pSkillCodeAddress = pAddress + 0x30; //技能id
		int nSkillCode = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pSkillCodeAddress);
		DWORD pShowOrderAddress = pAddress + 0x38; //显示顺序
		int nShowIndex = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pShowOrderAddress);
		/*if (m_szPickSkillList.contains(skillName))
		{
			m_skillCodeForIndex.insert();
		}*/
		if (m_skillCodeForMyCode.contains(nSkillCode))
			m_skillCodeForIndex.insert(m_skillCodeForMyCode.value(nSkillCode), i);
		m_indexForSkillName.insert(i, skillName);
	}
	//ml[0] = "A2A414";//等级
	//ml[1] = "7B25C0";//坐标东
	//ml[2] = "7B25BC";//坐标南
	//ml[3] = "A2A4C8";//名字
	//ml[4] = "984FD0";//职业
	//ml[5] = "A2A560";//站位
	//ml[6] = "A2A4B4";//健康
	//ml[7] = "9CAC9A";//宠物起址
	//ml[8] = "7EA9D0";//人背包起始地址813A90
	//ml[9] = "1590e0";//战场起址
	//ml[10] = "A2A418";//血
	//ml[11] = "A2A41c";//血最大
	//ml[12] = "A2A420";//魔
	//ml[13] = "A2A424";//魔最大
	//ml[14] = "5181C0";//地图名
	//ml[15] = "A2A4BC";//钱
	//ml[16] = "7D0798";//走路物品栏是否打开(>5)
	//ml[17] = "7D0654";//采集用到(=2)
	//ml[18] = "A36314";//窗口状态//93_60"E1BBE0"
	//ml[19] = "7D0648";//技能栏是否打开
	//ml[20] = "7D069C";//制作窗口是否打开
	//ml[21] = "15F2F0";//战斗做到哪一步
	//ml[22] = "7D0BA4";//判断谁攻击用
	//ml[23] = "15F338";//战斗时技能窗口 \1949B0
	//ml[24] = "7FAB94";//组队队员1名字地址第二个+16进制11
	//ml[25] = "A2A438";//经验值
	//ml[26] = "A2A43C";//下一级经验值
	//ml[27] = "A2A42C";//卡时
	//ml[28] = "5181E8";//地图编号
	//ml[30] = "803EA4";//卖物品钱数 唯有"803EA4"
	//ml[31] = "7DEF08";//唯有回答问题答案"7DEF08"|异度验证码"7DEDBD"
	//ml[32] = "5D0BC";//步步 唯有"<pkml.dll>+5D0BC"|异度"4C4D8"
	//ml[33] = "5D0BD";//切图 唯有"<pkml.dll>+5D0BD"|异度"EA768"
	//ml[35] = "2E96B0";//时间凌晨0-255 ((ss > 15 && ss < 30) || (ss > 110 && ss < 125)); }//16-32 110-128
	//ml[36] = "A2A566";//装备
	//ml[37] = "1D95D1";//窗口状态信息
	m_world_status = CONVERT_GAMEVAR(int *, 0xA36314);						   //ok  word==2 game==1是断开
	m_game_status = CONVERT_GAMEVAR(int *, 0xA36338);						   //ok 1
	g_skill_base = CONVERT_GAMEVAR(skill_t *, 0x984FEC);					   //ok
	UI_OpenGatherDialog = CONVERT_GAMEVAR(void(__cdecl *)(int, int), 0x62EF8); //ok
	//走路基址4D000
	Move_Player = CONVERT_GAMEVAR(void(__cdecl *)(), 0x4D000);
}

POINT GameData::GetGamePersonCoordinate()
{
	POINT pointPerson;
	pointPerson.x = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
	pointPerson.y = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
	return pointPerson;
}

QString GameData::GetGameMapName()
{
	QString szMapName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "009181C0", 100)));
	return szMapName;
}

int GameData::getPersionHP()
{
	int nHP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A418");
	return nHP;
}

int GameData::getPersionMP()
{
	int nMP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A420");
	return nMP;
}

int GameData::IsBattering()
{
	return YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "008E5444");
}

void GameData::moveToPoint(int x, int y)
{
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "008E543C", x);
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "008E5440", y);
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
	DWORD tId;
	CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044D000), NULL, 0, &tId);
	return;
	LPVOID CallBase = VirtualAllocEx(hTarget, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (CallBase == NULL) //如果内存申请失败
	{
		//关闭进程句柄
		CloseHandle(hTarget);
		return;
	}
	bool bRet = WriteProcessMemory(hTarget, CallBase, (LPCVOID)RemoteMoveCall, 0x1000, NULL);
	if (bRet == false)
	{
		//释放内存空间
		VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
		CloseHandle(hTarget);
		return;
	}
	//动态装载指定的DLL文件
	HANDLE hthread = CreateRemoteThread(hTarget, NULL, 0, LPTHREAD_START_ROUTINE(CallBase), NULL, 0, NULL);
	if (hthread == NULL) //创建加载DLL的线程失败
	{
		//释放内存空间
		VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
		CloseHandle(hTarget);
		return;
	}
	WaitForSingleObject(hthread, INFINITE);
	//释放资源
	VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
	CloseHandle(hthread);
	CloseHandle(hTarget);
}

void GameData::turn_about(int nDirection)
{
	//调用call方式
	POINT points = GetGamePersonCoordinate();
	switch (nDirection)
	{
		case MOVE_DIRECTION_North:
		{
			points.x = points.x + 1;
			points.y = points.y - 1;
			break;
		}
		case MOVE_DIRECTION_South:
		{
			points.x = points.x - 1;
			points.y = points.y + 1;
			break;
		}
		case MOVE_DIRECTION_West:
		{
			points.x = points.x - 1;
			points.y = points.y - 1;
			break;
		}
		case MOVE_DIRECTION_East:
		{
			points.x = points.x + 1;
			points.y = points.y + 1;
		}
		case MOVE_DIRECTION_NorthWest: points.y = points.y - 1; break;
		case MOVE_DIRECTION_NorthEast:
		{
			points.x = points.x + 1;
			break;
		}
		case MOVE_DIRECTION_SouthWest:
		{
			points.x = points.x - 1;
			break;
		}
		case MOVE_DIRECTION_SouthEast:
		{
			points.y = points.y + 1;
			break;
		}
		default: return;
	}
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00BC7B58", points.x);
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00BC7B5C", points.y);
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
	DWORD tId;
	CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044CCC0), NULL, 0, &tId);
	return;

	//计算鼠标像素点 模拟按键方式
	//POINT points;
	//long resX = 320;
	//long resY = 240;// -MOVE_STEP_DIAGONAL;
	//switch (nDirection)
	//{
	//case MOVE_DIRECTION_North:resY = 240 - MOVE_STEP_DIAGONAL; break;
	//case MOVE_DIRECTION_South:resY = 240 + MOVE_STEP_DIAGONAL; break;
	//case MOVE_DIRECTION_West:resX = 320 - MOVE_STEP_DIAGONAL; break;
	//case MOVE_DIRECTION_RIGHT:resX = 320 + MOVE_STEP_DIAGONAL; break;
	//case MOVE_DIRECTION_WestUP:
	//{
	//	resX = 320 - MOVE_STEP_DIAGONAL;
	//	resY = 240 - MOVE_STEP_DIAGONAL; break;
	//}
	//case MOVE_DIRECTION_NorthEast:
	//{
	//	resX = 320 + MOVE_STEP_DIAGONAL;
	//	resY = 240 - MOVE_STEP_DIAGONAL; break;
	//}
	//case MOVE_DIRECTION_WestDOWN:
	//{
	//	resX = 320 - MOVE_STEP_DIAGONAL;
	//	resY = 240 + MOVE_STEP_DIAGONAL; break;
	//}
	//case MOVE_DIRECTION_SouthEast:
	//{
	//	resX = 320 + MOVE_STEP_DIAGONAL;
	//	resY = 240 + MOVE_STEP_DIAGONAL; break;
	//}default:return;
	//}
	//points.x = resX;
	//points.y = resY;
	////ClientToScreen(m_gameHwnd, &points);
	//	//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	//LPARAM newl = MAKELPARAM(points.x, points.y);
	//PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	//Sleep(GetDoubleClickTime());
	//PostMessage(m_gameHwnd, WM_RBUTTONDOWN, WM_RBUTTONDOWN, newl);
	//PostMessage(m_gameHwnd, WM_RBUTTONUP, WM_RBUTTONUP, newl);
	//return;
	//::ClientToScreen(m_gameHwnd, &points);
	//::SetCursorPos(points.x, points.y);
	////YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	//YunLai::SimMouseClick(2, 1);
}

void GameData::RemoteMoveCall()
{
	_asm
	{
		/*push 0x1
		push 0x3
		mov ecx, 0x00ce5758*/
		mov eax, 0x0044D000
		call eax
	}
}

bool GameData::WaitToMove(int x, int y)
{
	int nTime = 100; //100秒
	moveToPoint(x, y);
	for (int i = 0; i < nTime; ++i)
	{
		POINT curPoint = GetGamePersonCoordinate();
		if (curPoint.x == x && curPoint.y == y)
			return true;
		Sleep(1000);
	}
	return false;
}

void GameData::JianDongXi(int nDirection)
{
	POINT points = GetGamePersonCoordinate();
	switch (nDirection)
	{
		case MOVE_DIRECTION_North:
		{
			points.x = points.x + 1;
			points.y = points.y - 1;
			break;
		}
		case MOVE_DIRECTION_South:
		{
			points.x = points.x - 1;
			points.y = points.y + 1;
			break;
		}
		case MOVE_DIRECTION_West:
		{
			points.x = points.x - 1;
			points.y = points.y - 1;
			break;
		}
		case MOVE_DIRECTION_East:
		{
			points.x = points.x + 1;
			points.y = points.y + 1;
		}
		case MOVE_DIRECTION_NorthWest: points.y = points.y - 1; break;
		case MOVE_DIRECTION_NorthEast:
		{
			points.x = points.x + 1;
			break;
		}
		case MOVE_DIRECTION_SouthWest:
		{
			points.x = points.x - 1;
			break;
		}
		case MOVE_DIRECTION_SouthEast:
		{
			points.y = points.y + 1;
			break;
		}
		default: return;
	}
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00BC7B58", points.x);
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00BC7B5C", points.y);
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
	DWORD tId;
	CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044CCC0), NULL, 0, &tId);
	return;
}
//void RemoteWorkCall()
//{
//	//	DWORD pAddress = 0x00462EF8;
//	_asm
//	{
//		mov eax, 0
//		mov ecx, 0
//		mov edx, 0x00462EF8
//		call edx
//	}
//}

__declspec(naked) void RemoteWorkCall()
{
	//	DWORD pAddress = 0x00462EF8;
	_asm
	{
		mov eax, 0
		mov ecx, 0
		mov edx, 0x00462EF8
		call edx
		ret
	}
}
//__declspec(naked) void plant1(void)
//{
//	__asm
//	{
//		mov eax, 0
//		mov ecx, 0
//		mov edx, 0x00462EF8
//		call edx
//		ret
//	}
//}
//
// //提升进程访问权限
bool EnableDebugPriv()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(),
				TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		return false;
	}
	return true;
}
//void GameData::Work(int code, const QString itemName/*=""*/)
//{
//	//// TODO: 在此添加控件通知处理程序代码
//	//DWORD byWrite;
//	////游戏进程句柄
//	//EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
//	//HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, false, m_gameProcessID);
//	////在目标进程分配内存空间 以方便写入要执行的代码
//	//PVOID FarCall = VirtualAllocEx(hp, NULL, 0x8FFF, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
//	////向目标进程的 目标地址写入我们要执行的代码
//	//WriteProcessMemory(hp, FarCall, plant1, 0x8FFF, &byWrite);
//	////在目标进程 指定地址 执行代码
//	//qDebug() << QString::number((DWORD)FarCall, 16);
//	////TRACE("\n addr=%x \n", FarCall);
//	//CreateRemoteThread(hp, NULL, NULL, (LPTHREAD_START_ROUTINE)FarCall, NULL, NULL, NULL);
//
//	//value1 = 101 鉴定
//	//value1 = 201 变身鉴定
//	//value1 = 102 挖掘
//	//value1 = 202 变身挖掘
//	//value1 = 103 狩猎
//	//value1 = 203 变身狩猎
//	//value1 = 104 伐木
//	//value1 = 204 变身伐木
//	//value1 = 105 修理武器
//	//value1 = 205 变身修理武器
//	//value1 = 106 修理防具
//	//value1 = 206 变身修理防具
//	int nIndex = m_skillCodeForIndex.value(code);
//	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
//
//	DWORD nThreadSize = 0x8FFF;
//	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
//	if (callBase == nullptr)
//	{
//		QMessageBox::information(nullptr, "提示：", "申请空间失败", QMessageBox::Ok);
//		return;
//	}
//	BYTE buf[40] = { 0 };
//	QStringList szAssemble;
//	szAssemble << QString("mov eax, %1").arg(nIndex)
//		<< "mov ecx, 0"
//		<< "mov edx, 0x00462EF8"
//		<< "call edx";
//	bool bTAssable=transAssemble(callBase,buf, nThreadSize, szAssemble);
//	if (bTAssable == false)
//		return;
//	//DWORD  nWirteSize = ((DWORD)RemoteWorkCallEnd - (DWORD)RemoteWorkCall);
//	//WriteProcessMemory(hp, FarCall, plant1, 0x8FFF, &byWrite);
////	bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)RemoteWorkCall, nThreadSize, 0);
//	bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)buf, nThreadSize, nullptr);
////	bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)RemoteWorkCall, 4096, nullptr);
//	if (bRet == false)
//	{
//		QMessageBox::information(nullptr, "提示：", QString("写入代码失败%1").arg(GetLastError()), QMessageBox::Ok);
//		return;
//	}
//	DWORD tId;
//	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, NULL, 0, &tId);
//	if (hRemoteThread == nullptr)
//	{
//		QMessageBox::information(nullptr, "提示：", "远程调用代码失败", QMessageBox::Ok);
//		return;
//	}
//	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
//	VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
//	CloseHandle(hRemoteThread);
//	CloseHandle(hProcess);
//}
void GameData::Work(int code, const QString itemName /*=""*/)
{
	//value1 = 101 鉴定
	//value1 = 201 变身鉴定
	//value1 = 102 挖掘
	//value1 = 202 变身挖掘
	//value1 = 103 狩猎
	//value1 = 203 变身狩猎
	//value1 = 104 伐木
	//value1 = 204 变身伐木
	//value1 = 105 修理武器
	//value1 = 205 变身修理武器
	//value1 = 106 修理防具
	//value1 = 206 变身修理防具
	int nIndex = m_skillCodeForIndex.value(code);
	EnableDebugPriv(); //提升进程权限 VS2008以后的版本才需要
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);

	DWORD nThreadSize = 0x8FFF;
	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
	if (callBase == nullptr)
	{
		QMessageBox::information(nullptr, "提示：", "申请空间失败", QMessageBox::Ok);
		return;
	}
	BYTE buf[40] = { 0 };
	QStringList szAssemble;
	szAssemble << QString("mov eax, %1").arg(nIndex)
			   << "mov ecx, 0"
			   << "mov edx, 0x00462EF8"
			   << "call edx";
	bool bTAssable = transAssemble(callBase, buf, nThreadSize, szAssemble);
	if (bTAssable == false)
		return;
	bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)RemoteWorkCall, nThreadSize, 0);
	if (bRet == false)
	{
		QMessageBox::information(nullptr, "提示：", QString("写入代码失败%1").arg(GetLastError()), QMessageBox::Ok);
		return;
	}
	DWORD tId;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, NULL, 0, &tId);
	if (hRemoteThread == nullptr)
	{
		QMessageBox::information(nullptr, "提示：", "远程调用代码失败", QMessageBox::Ok);
		return;
	}
	WaitForSingleObject(hRemoteThread, 0xFFFFFFF); //等待 ...
	VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
}
void GameData::Work(const QString &skillName, const QString itemName /*= ""*/)
{
	int nIndex = m_indexForSkillName.key(skillName);
	int mySkillCode = m_skillCodeForIndex.key(nIndex);
	Work(mySkillCode, itemName);
}

bool GameData::isWorking()
{
	return YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0xBD06FC");
}

#define BYTE_MAX 4096
//#define ASMJIT_BUILD_X86
//#define ASMJIT_STATIC
bool GameData::transAssemble(void *pAddr, BYTE *buf, int nLen, QStringList szCmdList)
{
	QString szMsg;
	//foreach (QString szCmd,szCmdList)
	//{
	//	XEDPARSE xed = { 0 };
	//	xed.cip;
	//}
	//char error[256] = { 0 };
	//int count = szCmdList.size();
	//int j = 0;
	//int m = 0;
	//for (int i = 0;i < count;i++)
	//{
	//	char cmd[256] = { 0 };
	//	QString szCmd = szCmdList.at(i);
	//	int len = szCmd.size();// m_edit.GetLine(i, cmd, 256);
	//	if (len == 0) continue;
	//	memcpy(cmd,szCmd.toStdString().c_str(),len);
	//	cmd[len] = '\0';
	//	t_asmmodel t_asm;
	//	j = m_asm.Assemble(cmd, (DWORD)pAddr + m, &t_asm, 0, 4, error);
	//	if (j <= 0)
	//	{
	//		szMsg.append(QString("error=\"%1\"").arg(error));
	//	}
	//	for (int k = 0; k < j; k++)
	//	{
	//		buf[m] = (BYTE)t_asm.code[k];
	//		szMsg.append(QString("%1").arg(buf[m]));
	//		m = m + 1;
	//		if (m > nLen)
	//			return false;//超出内存地址
	//	}
	//}
	//if ((m+2) > nLen)
	//	return false;//超出内存地址
	//buf[m] = 0x0c2;//ret 4
	//buf[m + 1] = 0x04;
	//buf[m + 2] = 0x00;
	//JitRuntime runtime;
	//CodeHolder code;                        // Holds code and relocation information.
	//code.init(runtime.codeInfo());               // Initialize to the same arch as JIT runti
	//x86::Assembler a(&code);                // Create and attach x86::Assembler to `code`.
	//a.mov(x86::eax, 1);                     // Move one to 'eax' register.
	//a.ret();
	//X86::Compiler c(&runtime);
	return true;
}

void GameData::selectRenew()
{
	POINT points;
	points.x = g_defResumeHMP.x();
	points.y = g_defResumeHMP.y();
	//ClientToScreen(m_gameHwnd, &points);
	//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	Sleep(500); //GetDoubleClickTime());
	points.x = g_defYes.x();
	points.y = g_defYes.y();
	newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	Sleep(500); //GetDoubleClickTime());
	points.x = g_defOk.x();
	points.y = g_defOk.y();
	newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}

void GameData::SetCharacterState(int state)
{
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00E2A554", state);
}

void GameData::SetCharacterSwitch(int v1, int v2, int v3, int v4, int v5, int v6)
{
	int nVal = 0;
	if (v1)
		nVal += CHARACTER_Battle;
	if (v2)
		nVal += CHARACTER_Chat;
	if (v3)
		nVal += CHARACTER_Troop;
	if (v4)
		nVal += CHARACTER_Card;
	if (v5)
		nVal += CHARACTER_Trade;
	if (v6)
		nVal += CHARACTER_Family;
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "00E2A554", nVal);
}

void GameData::Ren(const QString &itemName)
{
}

void GameData::Ren(int nIndex)
{
	EnableDebugPriv();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
	DWORD nThreadSize = 0x8FFF;
	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
	if (callBase == nullptr)
	{
		QMessageBox::information(nullptr, "提示：", "申请空间失败", QMessageBox::Ok);
		return;
	}
	BYTE buf[300] = { 0 };
	QStringList szAssemble;
	szAssemble << QString("mov ecx, 0x%1").arg(nIndex, 0, 16)
			   << "mov dword ptr ds:[0x53B064],ecx"
			   << "push 0x004689C0"
			   << "push ecx"
			   << "push 0x2"
			   << "call 0x00456A70"
			   << "add esp,0xC"
			   << QString("mov edx, 0x%1").arg(nIndex, 0, 16)
			   << "push edx"
			   << "push 0"
			   << "push 2"
			   << "call 0x004689c0"
			   << "add esp, 0xC"
			   << "ret";

	bool bTAssable = transAssemble(callBase, buf, nThreadSize, szAssemble);
	if (bTAssable == false)
		return;
	bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)buf, nThreadSize, nullptr);
	if (bRet == false)
	{
		int nEroCode = GetLastError();
		QMessageBox::information(nullptr, "提示：", "写入代码失败", QMessageBox::Ok);
		return;
	}
	DWORD tId;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, NULL, 0, &tId);
	if (hRemoteThread == nullptr)
	{
		QMessageBox::information(nullptr, "提示：", "远程调用代码失败", QMessageBox::Ok);
		return;
	}
	WaitForSingleObject(hRemoteThread, 0xFFFFFFF); //等待 ...
	VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
}

void GameData::Chat(const QString &szText)
{
	//wstring szChat = szText.toStdWString();
	//for (int i = 0; i < szChat.size(); i++)
	//{
	//	::SendMessage(m_gameHwnd/*目标窗体的句柄*/, WM_CHAR, szChat.at(i), 0);
	//}
	////	keybd_event(13,0,0,0);
	//SendMessage(m_gameHwnd, WM_KEYDOWN, VK_RETURN, 0);
	//SendMessage(m_gameHwnd, WM_KEYUP, VK_RETURN, 0);

	POINT points;
	points.x = 384;
	points.y = 187;
	//ClientToScreen(m_gameHwnd, &points);
	//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, newl);
}

void GameData::Renew()
{
	cga_player_info_t playerinfo = GetPlayerInfo();
	bool bNeedHP = NeedHPSupply(playerinfo);
	bool bNeedMP = NeedMPSupply(playerinfo);
	if (bNeedHP && (!bNeedMP || playerinfo.gold < playerinfo.maxmp - playerinfo.mp))
	{
		ClickNPCDialog(0, 2);
	}
	else if (bNeedMP && playerinfo.gold >= playerinfo.maxmp - playerinfo.mp)
	{
		ClickNPCDialog(0, 0);
	}

	/*if (NeedPetSupply(petsinfo))
	{
		ClickNPCDialog(0, 4);
	}*/
}
bool GameData::NeedHPSupply(cga_player_info_t &pl)
{
	return (pl.hp < pl.maxhp) ? true : false;
}

bool GameData::NeedMPSupply(cga_player_info_t &pl)
{
	return (pl.mp < pl.maxmp) ? true : false;
}

cga_player_info_t GameData::GetPlayerInfo()
{
	cga_player_info_t info;
	SendMessageA(m_gameHwnd, WM_CGA_GET_PLAYER_INFO, (WPARAM)&info, 0);
	return info;
}

bool GameData::ClickNPCDialog(int option, int index)
{
	return SendMessageA(m_gameHwnd, WM_CGA_CLICK_NPC_DIALOG, option, index) ? true : false;
}

void GameData::readBattleInfo()
{
	memset(m_Infos, 0, sizeof(Character) * 20);
	char *pText = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "0x005590E0", 1000); //0x005590E4
	if (strlen(pText) < 1)
		return;
	LPWSTR wText = ANSITOUNICODE1(pText);
	QString szBattleInfo = QString::fromStdWString(wText);	// YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "0x005590E0", 1000));
	QStringList szBattleInfoList = szBattleInfo.split("|"); //11个为一组战斗信息
	int nPersonSize = szBattleInfoList.size() / 11;
	for (int i = 0; i < nPersonSize; ++i)
	{
		QStringList szTemps = szBattleInfoList.mid(i * 11, 11);
		//	qDebug() << szTemps;
		int nPos = szTemps.at(2).toInt(nullptr, 16);
		QString szName = szTemps.at(3);
		m_Infos[nPos].name, szName;
		m_Infos[nPos].level = szTemps.at(5).toInt(nullptr, 16);
		m_Infos[nPos].hp = szTemps.at(6).toInt(nullptr, 16);
		m_Infos[nPos].maxhp = szTemps.at(7).toInt(nullptr, 16);
		m_Infos[nPos].mp = szTemps.at(8).toInt(nullptr, 16);
		m_Infos[nPos].maxmp = szTemps.at(9).toInt(nullptr, 16);
	}
}

void GameData::getPersonInfo(Character *pPerson)
{
	if (pPerson == nullptr) return;
	memcpy(pPerson, m_Infos, sizeof(m_Infos));
}

int GameData::GetWorldStatus()
{
	int worldStatus = *m_world_status;
	return worldStatus;
}

int GameData::GetGameStatus()
{
	//if (*g_logback)
	//	return 200;

	int gameStatus = *m_game_status;

	return gameStatus;
}
void GameData::remoteCall(DWORD processID, void *pCall)
{
	//EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
	//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	////DWORD nThreadSize = 0x400;
	////LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
	////if (callBase == nullptr)
	////{
	////	//std::cout << "erorCode:" << GetLastError() << ("提示：申请空间失败\n");
	////	return;
	////}
	//int nTestArg[2];
	//nTestArg[0]=
	//LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);
	//DWORD byWrite;
	////bool bRet = WriteProcessMemory(hProcess, callBase, pCall, nThreadSize, &byWrite);
	////if (bRet == false)
	////{
	//////	std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
	////	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
	////	CloseHandle(hProcess);
	////	return;
	////}
	//DWORD tId;
	//HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)UI_OpenGatherDialog, nullptr, 0, &tId);
	//if (hRemoteThread == nullptr)
	//{
	//	printf("提示：远程调用代码失败\n");
	//	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
	//	CloseHandle(hRemoteThread);
	//	CloseHandle(hProcess);
	//	return;
	//}
	//WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
	//VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
	//CloseHandle(hRemoteThread);
	//CloseHandle(hProcess);
}