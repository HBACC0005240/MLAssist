#include "MLAssistTool.h"
#include <QtWidgets/QApplication>
#include "ITObjectDataMgr.h"
#include "../CGALib/gameinterface.h"
#include "RpcSocket.h"
#include "ITDebugDump.h"

extern CGA::CGAInterface* g_CGAInterface;

#ifdef _DEBUG
#pragma comment(lib, "..\\lib\\CGALibd.lib")
#else
#pragma comment(lib, "..\\lib\\CGALib.lib")
#endif // DEBUG
CGA::CGAInterface* g_CGAInterface = NULL;
int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	g_CGAInterface = CGA::CreateInterface();
	ITObjectDataMgr::getInstance();
	MLAssistTool w;
	w.show();
	return a.exec();
}
