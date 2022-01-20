#ifndef AUTOBATTLE_H
#define AUTOBATTLE_H

#include "../CGALib/gameinterface.h"
#include "BattleBase.h"
#include "GameCtrl.h"
#include "constDef.h"
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>

class CBattleWorker : public QObject
{
	Q_OBJECT
public:
	CBattleWorker();
	static CBattleWorker *getInstace();

	void init();
	bool OnBattleAction(int flags);
	void AddOneCustomSetting(CBattleSettingPtr &item);					  //增加一个设置
	void AddSpecifNumEnemySetting(CBattleSettingPtr &item, int num = -1); //增加指定数量敌人设置
	void AddSpecifNumRoundSetting(CBattleSettingPtr &item, int num = -1); //增加指定回合敌人设置
	void RemoveSpecifNumEnemySetting(int num);							  //移除指定数量敌人设置
	void RemoveSpecifNumRoundSetting(int num);							  //移除指定回合设置
	void ClearSpecifNumEnemySetting();									  //清除所有指定数量敌人配置
	void ClearSpecifNumRoundSetting();									  //清除所有回合配置
	void ClearAllCustomSetting();										  //清除用户自定义配置

	bool OnSpecifNumEnemyBattle(); //1-10个敌人战斗调用
	bool OnSpecifNumRoundBattle(); //1-10回合战斗调用
	CBattleSettingPtr GetNoPetDoubleAction();
	void SetNoPetDoubleAction(CBattleSettingPtr set);

	void ResetBattleAnalysisData();				  //重置已解析的战斗信息
	int GetPetPosition(int playerPos);			  //获取宠物站位
	QString GetDevTypeText(int ntype);			  //获取类型对应文本
	CBattleSettingList GetSpecNumEnemySettings(); //返回1-10个敌人的战斗设置
	CBattleSettingList GetSpecNumRoundSettings(); //返回1-10回合的战斗设置
	CBattleSettingList GetCustomSettings();		  //返回自定义的战斗设置

	QHash<int, CBattleSettingPtr> GetInternalProtectSettings();			  //获取内置保护配置
	CBattleSettingPtr GetInternalProtectSetting(int ntype);				  //获取内置保护设置
	void AddInternalProtectSetting(int nType, CBattleSettingPtr setting); //增加内置保护设置
	void RemoveInternalProtectSetting(int nType);						  //移除一个内置保护设置
	void SetInternalProtectSettingEnabled(int nType, bool bFlag);		  //是否生效
	void ClearInternalProtectSetting();									  //移除所有保护设置

	CBattleSettingPtr GetEscapeSetting(int ntype);				 //获取内置逃跑设置
	void AddEscapeSetting(int nType, CBattleSettingPtr setting); //增加内置逃跑设置
	void RemoveEscapeSetting(int nType);						 //移除一个内置逃跑设置
	void RemoveEscapeSetting(int nType, const QString &name);	 //移除一个内置逃跑设置
	void ClearEscapeSetting();									 //移除所有内置保护

	CBattleSettingPtr GetHaveLv1Setting(int ntype);				  //获取内置1级怪设置
	void addHaveLv1Setting(int nType, CBattleSettingPtr setting); //1级怪设置
	QHash<int, CBattleSettingPtr> GetInternalLv1Settings();		  //获取内置l1
	void ClearHaveLv1Setting();
	void SetLv1FilterMaxHpVal(int nVal) { m_nCheckLv1MaxHpVal = nVal; }
	void SetLv1FilterMaxMpVal(int nVal) { m_nCheckLv1MaxMpVal = nVal; }

	QString GetBattleTypeText(int ntype) { return m_battleMapText.value(ntype); }
	int GetBattleTypeFromText(QString text) { return m_battleMapText.key(text); }
	void SetBattleDelay(bool bEnabled,int delay);

private:
	bool CheckProtect();		 //1级和boss战停止战斗
	bool CheckInternalProtect(); //检测内置保护设置
	bool CheckEscape();			 //检测逃跑设置
	bool CheckHaveLv1();		 //1级怪捉宠设置
	void AnalysisBattleData();	 //解析战斗信息
private slots:
	void OnPerformanceBattle();
	void OnLockCountdown();
public slots:
	void OnNotifyGetSkillsInfo(GameSkillList skills);
	void OnNotifyGetPetsInfo(GamePetList pets);
	void OnNotifyGetItemsInfo(GameItemList items);
	void OnNotifyGetBattleInfo(GameBattleUnitList units);
	void OnNotifyCharacterInfo(CharacterPtr chara);

	void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
	//ui战斗设置 同步
	void OnSetAutoBattle(int);							 //是否自动战斗
	void OnSetHighSpeed(int state);						 //是否高速战斗
	void OnSetFRND(int state);							 //第一回合延迟
	void OnSetAllEscape(int state);						 //全跑
	void OnSetNoLv1Escape(int state);					 //无1级逃跑
	void OnSetSpecialEscape(int state);					 //指定怪逃跑
	void OnSetLv1Protect(int state);					 //1级停止战斗
	void OnSetBOSSProtect(int state);					 //boos战停止战斗
	void OnSetLockCountdown(int state);					 //锁定倒计时
	void OnSetNoSwitchAnim(int state);					 //切图
	void OnSetPetDoubleAction(int state);				 //宠物二动
	void OnSetPlayerForceAction(int state);				 //人强制行动
	void OnSetShowHPMPEnabled(int state);				 //显示血魔
	void OnSetHave1LvEnemy(int state);					 //有1级怪
	void OnSetHave1LvAction(int nAction, bool bEnabled); //1级怪应对措施项
	void OnSetNoPetFirstAction(int state);				//人物二动 是否第一动使用面板操作 攻击 防御 逃跑

	void OnSetHightSpeedDelayVal(const QString &speed); //高速延时值
	void OnSetHightSpeedDelayVal(int speed);			//高速延时值

	void OnSetDelayFrom(int val);
	void OnSetDelayTo(int val);
	void OnSyncList(CBattleSettingList list);
signals:
	void NotifyBattleAction(int flags);
	void PlayAlarmWav();
	void StopAlarmWav();

public:
	bool m_bAutoBattle = false;			//是否自动战斗
	bool m_bHighSpeed = false;			//是否高速战斗
	bool m_bWaitAfterBattle = false;	//是否战斗结束延时
	bool m_bFirstRoundNoDelay = true;	//是否第一回合加速
	bool m_bAllEscape = false;			//全跑
	bool m_bNoLv1Escape = false;		//无1级逃跑
	bool m_bSpecialEnemyEscape = false; //指定怪逃跑
	bool m_bPlayAlarm = false;			//是否正在播放音乐
	bool m_bLevelOneProtect = false;	//是否1级停止战斗
	bool m_bBOSSProtect = false;		//是否Boss战停止战斗
	bool m_bLockCountdown = false;		//是否锁定倒计时
	bool m_bShowHPMP = false;			//是否显示血魔
	bool m_bPetDoubleAction = true;		//宠物二动
	bool m_bPlayerForceAction = false;	//人强制行动
	bool m_bNoSwitchAnim = false;		//切图
	bool m_bHave1LvEnemy = false;		//有1级怪
	bool m_bHave1LvRoundOne = false;	//有1级怪时 第一回合设置
	bool m_bHave1LvHpSet = false;		//有1级怪时 怪物当前血设置
	bool m_bHave1LvClearEnemy = false;	//有1级怪时 清除非1级怪
	bool m_bHave1LvLastSet = false;		//有1级怪时 最后设置
	int m_iDelayFrom;
	int m_iDelayTo;
	int m_nHightSpeedDelay = 4;			//高速延时值 默认4
	int m_nBattleDelay = 5;				//战斗延时值 默认5

	uint64_t m_LastWarpMap202;
	CGA_BattleContext_t m_BattleContext;
	bool m_bPetDoubleActionDefault1 = true; //宠物二动第一攻默认
	bool m_bPetDoubleActionDefault2 = true; //宠物二动第二攻默认 默认使用配置里的技能
	CBattleSettingPtr m_petDoubleAction1;	//自定义宠物二动 第一攻
	CBattleSettingPtr m_petDoubleAction2;	//自定义宠物二动 第二攻

private:
	CBattleSettingList m_SettingList;			   //自定义配置
	CBattleSettingList m_pSpecNumEnemySettingList; //1-10个敌人时候的设置
	CBattleSettingList m_pSpecNumRoundSettingList; //1-10回合时候的设置
	QHash<int, QString> m_battleMapText;		   //战斗类型文本映射
	CBattleSettingPtr m_noPetDoubleAction;		   //不带宠二动设置
												   //宠自定义二动设置

	QMap<int, QList<int> > m_battleTPos;					 //战斗单元T字形站位 pos
	QHash<int, QString> m_devTypeText;						 //类型对应文本
	QHash<int, CBattleSettingPtr> m_pIntProtSettings;		 //内置保护配置
	QMultiHash<int, CBattleSettingPtr> m_pEscapeSettingList; //逃跑设置
	QHash<int, CBattleSettingPtr> m_pLv1SettingList;		 //一级怪应对
	bool m_bCheckLv1MaxHp = false;
	bool m_bCheckLv1MaxMp = false;
	int m_nCheckLv1MaxHpVal = 0;
	int m_nCheckLv1MaxMpVal = 0;
};

#define g_pAutoBattleCtrl CBattleWorker::getInstace()
#endif // AUTOBATTLE_H
