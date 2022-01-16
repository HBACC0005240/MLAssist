#include "BattleModuleRegisty.h"

BattleModuleRegisty::BattleModuleRegisty(void)
{
}

BattleModuleRegisty::~BattleModuleRegisty(void)
{
	for (int i = 0; i < moduleInfoList.size(); i++)
	{
		BatlleModuleInfo *pInfo = moduleInfoList[i];
		BattleModuleFactory *pModuleFactory = pInfo->factory;
		if (pModuleFactory)
			delete pModuleFactory;
		delete pInfo;
	}
	moduleInfoList.clear();
}

/*!
* @method GetInstance
* @brief 工程模块注册类的唯一实例
*/
BattleModuleRegisty &BattleModuleRegisty::GetInstance()
{
	static BattleModuleRegisty instance;
	return instance;
}

/*!
* @method RegisterModuleFactory
* @brief 注册工程模块工厂
* @param[in] En_ModuleID enModuleID
* @param[in] GBMProjectModuleFactory * pFactory
* @return void
*/
void BattleModuleRegisty::RegisterModuleFactory(QString strCode, BattleModuleFactory *pFactory, QString sIconPath)
{
	for (int i = 0; i < moduleInfoList.size(); i++)
	{
		if (moduleInfoList[i]->strCode == strCode)
			return;
	}
	BatlleModuleInfo *moduleInfo = new BatlleModuleInfo();
	moduleInfo->strCode = strCode;
	moduleInfo->factory = pFactory;
	moduleInfo->IconPath = sIconPath;
	moduleInfoList.append(moduleInfo);
}

BattleBase *BattleModuleRegisty::CreateNewBattleObj(const QString &szCode)
{
	for (int i = 0; i < moduleInfoList.size(); i++)
	{
		if (moduleInfoList[i]->strCode == szCode)
			return moduleInfoList[i]->factory->Create();
	}
	return nullptr;
}

BattleBase *BattleModuleRegisty::CreateNewBattleObj(int nCode)
{
	return CreateNewBattleObj(QString::number(nCode));
}

int BattleModuleRegisty::GetClassSize(int nCode)
{
	QString szCode = QString::number(nCode);
	for (int i = 0; i < moduleInfoList.size(); i++)
	{
		if (moduleInfoList[i]->strCode == szCode)
			return moduleInfoList[i]->factory->ClassSize();
	}
	return 0;
}
