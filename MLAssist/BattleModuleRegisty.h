#pragma once
#include "BattleBase.h"
typedef QList<BattleBase*> ModuleList;

struct BatlleModuleInfo
{
	QString  strCode;
	int iStackIndex;
	BattleModuleFactory* factory;
	QString IconPath;

	BatlleModuleInfo()
		: strCode("")
		, iStackIndex(-1)
		, factory(NULL)
	{

	}
};

class BattleModuleRegisty
{
public:
	static BattleModuleRegisty& GetInstance();
	~BattleModuleRegisty(void);

	void RegisterModuleFactory(QString strCode,BattleModuleFactory* pFactory,QString sIconPath = "");
	QList<BatlleModuleInfo*> moduleInfoList;
	BattleBase* CreateNewBattleObj(const QString& szCode);
	BattleBase* CreateNewBattleObj(int nCode);
	int GetClassSize(int nCode);

private:
	BattleModuleRegisty(void);
	BattleModuleRegisty(const BattleModuleRegisty&);
	BattleModuleRegisty& operator = (const BattleModuleRegisty&);

};
#define g_battleModuleReg BattleModuleRegisty::GetInstance()


