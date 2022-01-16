#pragma once

#include <string>
#include <algorithm>
//#include <xfunctional>
#define I_INVALID_HANDLE 0
typedef unsigned __int64 UINT64, *PUINT64;
typedef unsigned int UINT, *PUINT;
typedef unsigned __int64 ITOBJECTID;

class ITObjectID
{
public:
	//初始化查询表，在任何NewID()之前调用
	static void InitSeed();

	//生成新的OBjectID
	static unsigned __int64 NewID();

	static unsigned __int64 NewID(std::string & seed);

	//生成新的OBjectID, 不靠谱的 ID, 为了 兼容32位整型ID 考虑
	static unsigned int NewID32();
};