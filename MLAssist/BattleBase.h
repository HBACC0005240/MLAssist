#pragma once
#include "../CGALib/gameinterface.h"
#include "GlobalDefine.h"
#include "class_db.h"
#include <QObject>
#include <QString>
//注意：为了支持memcpy 类中定义的变量不要有string这种类对象，用原始int char 来定义
//否则，在拷贝后，因为String没有调用构造函数，会导致不可预计的崩溃
#define ML_CLASS(ClsName, BaseCls)                                                              \
                                                                                                \
public:                                                                                         \
	inline static const QString get_type_name() { return static_cast<const char *>(#ClsName); } \
	inline static size_t get_id() { return typeid(ClsName).hash_code(); }                       \
	inline static size_t get_base_id() { return typeid(BaseCls).hash_code(); }                  \
	inline static const QString get_base_type_name() { return BaseCls::get_class_name(); }      \
                                                                                                \
private:

class BattleBase
{
public:
	BattleBase();
	BattleBase(int nDevType);
	virtual ~BattleBase();
	void SetDevType(int nType) { m_nDevType = nType; }
	int GetDevType() { return m_nDevType; }
	static QString get_class_name() { return "BattleBase"; }
	int m_nDevType; //类型
};

//战斗条件判断基类
class CBattleCondition : public BattleBase
{
	ML_CLASS(CBattleCondition, BattleBase)

public:
	CBattleCondition();
	virtual void GetConditionValue(QString &str) = 0;
	virtual int GetConditionRelId() = 0;
	virtual int GetConditionTypeId() { return m_nDevType; } //= 0;
	virtual void GetConditionName(QString &str) = 0;
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget) = 0;
	virtual void SetEnabled(bool bFlag) { m_bEnable = bFlag; }
	virtual bool GetEnabled() { return m_bEnable; }
	virtual QStringList GetConditionTextData() = 0;
	virtual QList<int> GetConditionData() = 0;
	virtual void SetConditionTextData(QStringList data) = 0;
	virtual void SetConditionData(QList<int> data) = 0;

protected:
	bool m_bEnable = true;
};
//战斗动作基类
class CBattleAction : public BattleBase
{
public:
	CBattleAction();
	virtual int GetActionTypeId() { return m_nDevType; };
	virtual void GetActionName(QString &str, bool config) = 0;
	virtual int GetTargetFlags(CGA_BattleContext_t &context) = 0;
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context) = 0;
	virtual QStringList GetActionTextData() = 0;
	virtual QList<int> GetActionData() = 0;
	virtual void SetActionTextData(QStringList data) = 0;
	virtual void SetActionData(QList<int> data) = 0;

protected:
};

//Target
//战斗目标基类
class CBattleTarget : public BattleBase
{
public:
	CBattleTarget();
	virtual int GetTargetTypeId() = 0;
	virtual int GetTargetSelectId() = 0;
	virtual void GetTargetName(QString &str) = 0;
	virtual int GetTarget(int unitpos, int flags, CGA_BattleContext_t &context) = 0;
	virtual void SetTargetVal(int val) = 0;
	virtual QString GetTargetSpecialName() = 0;
	virtual void SetTargetSpecialName(QString name) = 0;
};
//条件判断-忽略
class CBattleCondition_Ignore : public CBattleCondition
{
	ML_CLASS(CBattleCondition_Ignore, CBattleCondition)

public:
	CBattleCondition_Ignore &operator=(CBattleCondition_Ignore &a)
	{
		if (&a == this)
			return *this;
	}

	CBattleCondition_Ignore();
	virtual void GetConditionValue(QString &str)
	{
		str = QString();
	}
	virtual int GetConditionRelId() { return 0; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
	virtual QStringList GetConditionTextData() { return QStringList(); }
	virtual QList<int> GetConditionData() { return QList<int>(); }
	virtual void SetConditionTextData(QStringList data) {}
	virtual void SetConditionData(QList<int> data) {}

protected:
};
class CBattleCondition_Compare : public CBattleCondition
{
public:
	CBattleCondition_Compare();
	CBattleCondition_Compare(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual int GetConditionVal() { return m_value; }
	virtual int GetConditionTypeId() { return m_nDevType; }
	//virtual void GetConditionName(QString& str);
	//virtual bool Check(CGA_BattleContext_t& context, int& conditionTarget);
	virtual void setRelation(int val) { m_relation = val; }
	virtual void setConditionValue(int val) { m_value = val; }
	virtual void setConditionPercentage(bool flag) { m_percentage = flag; }
	virtual QStringList GetConditionTextData() { return QStringList(); }
	virtual QList<int> GetConditionData() { return QList<int>() << m_relation << m_value << m_percentage; }
	virtual void SetConditionTextData(QStringList data) {}
	virtual void SetConditionData(QList<int> data);

protected:
	int m_relation;
	int m_value;
	bool m_percentage;
};
//条件判断-敌人数
class CBattleCondition_EnemyCount : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyCount();
	CBattleCondition_EnemyCount(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};
//条件判断-队伍数
class CBattleCondition_TeammateCount : public CBattleCondition_Compare
{
public:
	CBattleCondition_TeammateCount();
	CBattleCondition_TeammateCount(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

//条件判断-每行敌人数
class CBattleCondition_EnemySingleRowCount : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemySingleRowCount();
	CBattleCondition_EnemySingleRowCount(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};
//条件判断-人物血
class CBattleCondition_PlayerHp : public CBattleCondition_Compare
{
public:
	CBattleCondition_PlayerHp();
	CBattleCondition_PlayerHp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};
//条件判断-人物魔
class CBattleCondition_PlayerMp : public CBattleCondition_Compare
{
public:
	CBattleCondition_PlayerMp();
	CBattleCondition_PlayerMp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_PetHp : public CBattleCondition_Compare
{
public:
	CBattleCondition_PetHp();
	CBattleCondition_PetHp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_PetMp : public CBattleCondition_Compare
{
public:
	CBattleCondition_PetMp();
	CBattleCondition_PetMp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_TeammateHp : public CBattleCondition_Compare
{
public:
	CBattleCondition_TeammateHp();
	CBattleCondition_TeammateHp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_TeammateMp : public CBattleCondition_Compare
{
public:
	CBattleCondition_TeammateMp();
	CBattleCondition_TeammateMp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_EnemyMultiTargetHp : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyMultiTargetHp();
	CBattleCondition_EnemyMultiTargetHp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_TeammateMultiTargetHp : public CBattleCondition_Compare
{
public:
	CBattleCondition_TeammateMultiTargetHp();
	CBattleCondition_TeammateMultiTargetHp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_EnemyAllHp : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyAllHp();
	CBattleCondition_EnemyAllHp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_EnemyLv1Hp : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyLv1Hp();
	CBattleCondition_EnemyLv1Hp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};
class CBattleCondition_EnemyLv1MaxHp : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyLv1MaxHp();
	CBattleCondition_EnemyLv1MaxHp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};
class CBattleCondition_EnemyLv1MaxMp : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyLv1MaxMp();
	CBattleCondition_EnemyLv1MaxMp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};
class CBattleCondition_TeammateAllHp : public CBattleCondition_Compare
{
public:
	CBattleCondition_TeammateAllHp();
	CBattleCondition_TeammateAllHp(int relation, int value, bool percentage);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
		if (m_percentage)
			str += QLatin1String("%");
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};
//敌人名称
class CBattleCondition_EnemyUnit : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyUnit();
	CBattleCondition_EnemyUnit(int relation, QString &unitName);
	virtual void GetConditionValue(QString &str)
	{
		str = m_UnitName;
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);

	virtual void SetUnitName(const QString &szName);
	virtual QString GetUnitName() { return m_UnitName; }
	virtual QStringList GetConditionTextData() { return QStringList(m_UnitName); }
	virtual QList<int> GetConditionData() { return QList<int>() << m_relation << m_value << m_percentage; }
	virtual void SetConditionTextData(QStringList data);

private:
	char m_UnitName[250] = { 0 };
};
class CBattleCondition_TeammateUnit : public CBattleCondition_Compare
{
public:
	CBattleCondition_TeammateUnit();
	CBattleCondition_TeammateUnit(int relation, QString &unitName);
	virtual void GetConditionValue(QString &str)
	{
		str = m_UnitName;
	}
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);

	virtual void SetUnitName(const QString &szName);
	virtual QString GetUnitName() { return m_UnitName; }
	virtual QStringList GetConditionTextData() { return QStringList(m_UnitName); }
	virtual QList<int> GetConditionData() { return QList<int>() << m_relation << m_value << m_percentage; }
	virtual void SetConditionTextData(QStringList data);

protected:
	int m_relation;
	char m_UnitName[250] = { 0 };
};

class CBattleCondition_Round : public CBattleCondition_Compare
{
public:
	CBattleCondition_Round();
	CBattleCondition_Round(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_DoubleAction : public CBattleCondition
{
public:
	CBattleCondition_DoubleAction();
	virtual void GetConditionValue(QString &str)
	{
		str = QString();
	}
	virtual int GetConditionRelId() { return 0; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
	virtual QStringList GetConditionTextData() { return QStringList(); }
	virtual QList<int> GetConditionData() { return QList<int>(); }
	virtual void SetConditionTextData(QStringList data) {}
	virtual void SetConditionData(QList<int> data) {}

protected:
};

class CBattleCondition_TeammateDebuff : public CBattleCondition_Compare
{
public:
	CBattleCondition_TeammateDebuff();
	CBattleCondition_TeammateDebuff(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_EnemyLevel : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyLevel();
	CBattleCondition_EnemyLevel(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_EnemyAvgLevel : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyAvgLevel();
	CBattleCondition_EnemyAvgLevel(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};
class CBattleCondition_EnemyType : public CBattleCondition_Compare
{
public:
	CBattleCondition_EnemyType();
	CBattleCondition_EnemyType(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
};

class CBattleCondition_InventoryItem : public CBattleCondition_Compare
{
public:
	CBattleCondition_InventoryItem();
	CBattleCondition_InventoryItem(int relation, QString &itemName);
	virtual void GetConditionValue(QString &str)
	{
		str = m_ItemName;
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);
	virtual void SetItemName(const QString &szText);
	virtual QStringList GetConditionTextData() { return QStringList(m_ItemName); }
	virtual QList<int> GetConditionData() { return QList<int>() << m_relation << m_value << m_percentage; }
	virtual void SetConditionTextData(QStringList data);

protected:
	char m_ItemName[250] = { 0 };
};

class CBattleCondition_PlayerName : public CBattleCondition_Compare
{
public:
	CBattleCondition_PlayerName();
	CBattleCondition_PlayerName(int relation, QString &itemName);
	virtual void GetConditionValue(QString &str)
	{
		str = m_PlayerName;
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);

	virtual void SetPlayerName(const QString &szName);
	virtual QString GetPlayerName() { return m_PlayerName; }
	virtual QStringList GetConditionTextData() { return QStringList(m_PlayerName); }
	virtual QList<int> GetConditionData() { return QList<int>() << m_relation << m_value << m_percentage; }
	virtual void SetConditionTextData(QStringList data);

protected:
	int m_relation;
	char m_PlayerName[250] = { 0 };
};

class CBattleCondition_PlayerJob : public CBattleCondition_Compare
{
public:
	CBattleCondition_PlayerJob();
	CBattleCondition_PlayerJob(int relation, QString &itemName);
	virtual void GetConditionValue(QString &str)
	{
		str = m_PlayerJob;
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);

	virtual void SetPlayerJob(const QString &szName);
	virtual QString GetPlayerJob() { return m_PlayerJob; }
	virtual QStringList GetConditionTextData() { return QStringList(m_PlayerJob); }
	virtual QList<int> GetConditionData() { return QList<int>() << m_relation << m_value << m_percentage; }
	virtual void SetConditionTextData(QStringList data);

protected:
	int m_relation;
	char m_PlayerJob[250] = { 0 };
};

class CBattleCondition_PlayerGold : public CBattleCondition_Compare
{
public:
	CBattleCondition_PlayerGold();
	CBattleCondition_PlayerGold(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);

protected:
	int m_relation;
	int m_value;
};

class CBattleCondition_BattleBGM : public CBattleCondition_Compare
{
public:
	CBattleCondition_BattleBGM();
	CBattleCondition_BattleBGM(int relation, int value);
	virtual void GetConditionValue(QString &str)
	{
		str = QString::number(m_value);
	}
	virtual int GetConditionRelId() { return m_relation; }
	virtual void GetConditionName(QString &str);
	virtual bool Check(CGA_BattleContext_t &context, int &conditionTarget);

protected:
	int m_relation;
	int m_value;
};
class CBattleAction_PlayerAttack : public CBattleAction
{
public:
	CBattleAction_PlayerAttack();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;
	}
	virtual QStringList GetActionTextData() { return QStringList(); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data) {}
	virtual void SetActionData(QList<int> data) {}
};

class CBattleAction_PlayerGuard : public CBattleAction
{
public:
	CBattleAction_PlayerGuard();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return 0;
	}
	virtual QStringList GetActionTextData() { return QStringList(); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data) {}
	virtual void SetActionData(QList<int> data) {}
};

class CBattleAction_PlayerEscape : public CBattleAction
{
public:
	CBattleAction_PlayerEscape();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return 0;
	}
	virtual QStringList GetActionTextData() { return QStringList(); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data) {}
	virtual void SetActionData(QList<int> data) {}
};

class CBattleAction_PlayerExchangePosition : public CBattleAction
{
public:
	CBattleAction_PlayerExchangePosition();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return 0;
	}
	virtual QStringList GetActionTextData() { return QStringList(); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data) {}
	virtual void SetActionData(QList<int> data) {}
};

class CBattleAction_PlayerChangePet : public CBattleAction
{
public:
	CBattleAction_PlayerChangePet();
	CBattleAction_PlayerChangePet(int type);
	CBattleAction_PlayerChangePet(QString &petname);
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return 0;
	}
	virtual void SetCallType(int nType) { m_Type = nType; }
	virtual void SetPetName(const QString &szName);
	virtual QStringList GetActionTextData() { return QStringList(m_PetName); }
	virtual QList<int> GetActionData() { return QList<int>() << m_Type; }
	virtual void SetActionTextData(QStringList data);
	virtual void SetActionData(QList<int> data);

protected:
	char m_PetName[250] = { 0 };
	int m_Type; //0 = pet, 1=recall, 2=most_level, 3=most_health
};

class CBattleAction_PlayerSkillAttack : public CBattleAction
{
public:
	CBattleAction_PlayerSkillAttack(QString &skillName, int skillLevel);
	CBattleAction_PlayerSkillAttack();

	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual QString GetSkillName() { return m_SkillName; }
	virtual int GetSkillLevel() { return m_SkillLevel; }
	virtual int GetTargetFlags(CGA_BattleContext_t &context);
	void setSkillName(const QString &szText);
	void setSkillLv(int nLv) { m_SkillLevel = nLv; }
	virtual QStringList GetActionTextData() { return QStringList(m_SkillName); }
	virtual QList<int> GetActionData() { return QList<int>() << m_SkillLevel; }
	virtual void SetActionTextData(QStringList data);
	virtual void SetActionData(QList<int> data);

private:
	bool GetSkill(CGA_BattleContext_t &context, int &skillpos, int &skilllevel);
	void FixTarget(CGA_BattleContext_t &context, int skillpos, int skilllevel, int &target);

protected:
	char m_SkillName[250] = { 0 };
	int m_SkillLevel; //0=max
};

class CBattleAction_PlayerUseItem : public CBattleAction
{
public:
	CBattleAction_PlayerUseItem(QString &itemName);
	CBattleAction_PlayerUseItem();

	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_SELF | FL_SKILL_TO_TEAMMATE | FL_SKILL_SELECT_TARGET;
	}
	void setItemName(const QString &szText);
	virtual QStringList GetActionTextData() { return QStringList(m_ItemName); }
	virtual QList<int> GetActionData() { return QList<int>() << m_ItemId << m_ItemType; }
	virtual void SetActionTextData(QStringList data);
	virtual void SetActionData(QList<int> data);

private:
	bool GetItemPosition(CGA_BattleContext_t &context, int &itempos);

protected:
	char m_ItemName[250] = { 0 }; //item name or #id
	int m_ItemId;
	int m_ItemType;
};

class CBattleAction_PlayerLogBack : public CBattleAction
{
public:
	CBattleAction_PlayerLogBack();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return 0;
	}
	virtual QStringList GetActionTextData() { return QStringList(); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data) {}
	virtual void SetActionData(QList<int> data) {}
};

class CBattleAction_PlayerRebirth : public CBattleAction
{
public:
	CBattleAction_PlayerRebirth();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return 0;
	}
	virtual QStringList GetActionTextData() { return QStringList(); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data) {}
	virtual void SetActionData(QList<int> data) {}
};

class CBattleAction_PlayerDoNothing : public CBattleAction
{
public:
	CBattleAction_PlayerDoNothing();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return 0;
	}
	virtual QStringList GetActionTextData() { return QStringList(); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data) {}
	virtual void SetActionData(QList<int> data) {}
};

class CBattleAction_PetSkillAttack : public CBattleAction
{
public:
	CBattleAction_PetSkillAttack(QString &skillName);
	CBattleAction_PetSkillAttack();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual QString GetSkillName() { return m_SkillName; }
	virtual int GetTargetFlags(CGA_BattleContext_t &context);
	virtual void SetSkillName(const QString &szName);
	virtual QStringList GetActionTextData() { return QStringList(m_SkillName); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data);
	virtual void SetActionData(QList<int> data) {}

private:
	bool GetSkill(CGA_BattleContext_t &context, int &skillpos, bool &bUseDefaultTarget);
	void FixTarget(CGA_BattleContext_t &context, int skillpos, int &target);

protected:
	char m_SkillName[250] = { 0 };
};
class CBattleAction_PetDoNothing : public CBattleAction
{
public:
	CBattleAction_PetDoNothing();
	virtual void GetActionName(QString &str, bool config);
	virtual bool DoAction(int target, int defaultTarget, CGA_BattleContext_t &context);
	virtual int GetTargetFlags(CGA_BattleContext_t &context)
	{
		return 0;
	}
	virtual QStringList GetActionTextData() { return QStringList(); }
	virtual QList<int> GetActionData() { return QList<int>(); }
	virtual void SetActionTextData(QStringList data) {}
	virtual void SetActionData(QList<int> data) {}
};
class CBattleTarget_Self : public CBattleTarget
{
public:
	CBattleTarget_Self();
	virtual int GetTargetSelectId() { return -1; }
	virtual int GetTargetTypeId() { return m_nDevType; }
	virtual void GetTargetName(QString &str);
	virtual int GetTarget(int unitpos, int flags, CGA_BattleContext_t &context);
	virtual void SetTargetVal(int val);
	virtual QString GetTargetSpecialName() { return ""; }
	virtual void SetTargetSpecialName(QString name) {}
};

class CBattleTarget_Pet : public CBattleTarget
{
public:
	CBattleTarget_Pet();
	virtual int GetTargetSelectId() { return -1; }
	virtual int GetTargetTypeId() { return m_nDevType; }
	virtual void GetTargetName(QString &str);
	virtual int GetTarget(int unitpos, int flags, CGA_BattleContext_t &context);
	virtual void SetTargetVal(int val);
	virtual QString GetTargetSpecialName() { return ""; }
	virtual void SetTargetSpecialName(QString name) {}
};

class CBattleTarget_Enemy : public CBattleTarget
{
public:
	CBattleTarget_Enemy();
	CBattleTarget_Enemy(int select, QString special = "");

	virtual int GetTargetSelectId() { return m_Select; }
	virtual int GetTargetTypeId() { return m_nDevType; }
	virtual void GetTargetName(QString &str);
	virtual int GetTarget(int unitpos, int flags, CGA_BattleContext_t &context);
	virtual void SetTargetSelect() {}
	virtual void SetTargetVal(int val);
	virtual QString GetTargetSpecialName() { return m_specialName; }
	virtual void SetTargetSpecialName(QString name);

protected:
	int m_Select;
	char m_specialName[250] = { 0 }; //指定怪物名称
};

class CBattleTarget_Teammate : public CBattleTarget
{
public:
	CBattleTarget_Teammate();
	CBattleTarget_Teammate(int select);
	virtual int GetTargetSelectId() { return m_Select; }
	virtual int GetTargetTypeId() { return m_nDevType; }
	virtual void GetTargetName(QString &str);
	virtual int GetTarget(int unitpos, int flags, CGA_BattleContext_t &context);
	virtual void SetTargetVal(int val);
	virtual QString GetTargetSpecialName() { return ""; }
	virtual void SetTargetSpecialName(QString name) {}

protected:
	int m_Select;
};

class CBattleTarget_Condition : public CBattleTarget
{
public:
	CBattleTarget_Condition();
	virtual int GetTargetSelectId() { return -1; }
	virtual int GetTargetTypeId() { return m_nDevType; }
	virtual void GetTargetName(QString &str);
	virtual int GetTarget(int unitpos, int flags, CGA_BattleContext_t &context);
	virtual void SetTargetVal(int val) {}
	virtual QString GetTargetSpecialName() { return ""; }
	virtual void SetTargetSpecialName(QString name) {}
};

typedef struct CGA_BattleContext_s CGA_BattleContext_t;

class CBattleSetting
{
public:
	CBattleSetting(CBattleCondition *cond, CBattleCondition *cond2,
			CBattleAction *playerAction, CBattleTarget *playerTarget,
			CBattleAction *petAction, CBattleTarget *petTarget /*,
	   CBattleAction *petAction2, CBattleTarget *petTarget2*/
	);
	~CBattleSetting();

	virtual bool DoAction(CGA_BattleContext_t &context);
	virtual void GetConditionName(QString &str);
	virtual int GetConditionRelId();
	virtual int GetConditionTypeId();
	virtual void GetConditionValue(QString &str);

	virtual void GetCondition2Name(QString &str);
	virtual int GetCondition2RelId();
	virtual int GetCondition2TypeId();
	virtual void GetCondition2Value(QString &str);

	virtual void GetPlayerActionName(QString &str, bool config);
	virtual void GetPlayerTargetName(QString &str);

	virtual void GetPetActionName(QString &str, bool config);
	virtual void GetPetTargetName(QString &str);

	//virtual void GetPetAction2Name(QString &str);
	//virtual void GetPetTarget2Name(QString &str);

	virtual int GetPlayerActionTypeId();
	virtual QString GetPlayerSkillName();
	virtual int GetPlayerSkillLevel();
	virtual int GetPlayerTargetTypeId();
	virtual int GetPlayerTargetSelectId();

	virtual int GetPetActionTypeId();
	virtual QString GetPetSkillName();
	virtual int GetPetTargetTypeId();
	virtual int GetPetTargetSelectId();

	/*virtual int GetPetAction2TypeId();
	virtual QString GetPetSkill2Name();
	virtual int GetPetTarget2TypeId();
	virtual int GetPetTarget2SelectId();*/

	virtual bool HasPlayerAction();
	virtual bool HasPetAction();

public:
	CBattleCondition *m_condition, *m_condition2;
	CBattleAction *m_playerAction;
	CBattleTarget *m_playerTarget;
	CBattleAction *m_petAction;
	//CBattleAction *m_petAction2;
	CBattleTarget *m_petTarget;
	//CBattleTarget *m_petTarget2;
	CBattleTarget *m_defaultTarget;
	bool bEnabled = true; //是否启用
};

typedef QSharedPointer<CBattleSetting> CBattleSettingPtr;
typedef QList<CBattleSettingPtr> CBattleSettingList;

/*!
* @brief 工程模块工厂
*/
class BattleModuleFactory
{
public:
	virtual ~BattleModuleFactory(void) {}
	virtual BattleBase *Create() = 0;
	virtual int ClassSize() = 0;
};

#define DECLARE_MODULE_FACTORY(ModuleName)                 \
	class ModuleName##Factory : public BattleModuleFactory \
	{                                                      \
	public:                                                \
		virtual BattleBase *Create()                       \
		{                                                  \
			return new ModuleName();                       \
		}                                                  \
		virtual int ClassSize()                            \
		{                                                  \
			return sizeof(ModuleName);                     \
		}                                                  \
	};

#define NEW_MODULE_FACTORY(ModuleName) \
	(new ModuleName##Factory())

DECLARE_MODULE_FACTORY(CBattleCondition_Ignore)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyCount)
DECLARE_MODULE_FACTORY(CBattleCondition_TeammateCount)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemySingleRowCount)
DECLARE_MODULE_FACTORY(CBattleCondition_PlayerHp)
DECLARE_MODULE_FACTORY(CBattleCondition_PlayerMp)
DECLARE_MODULE_FACTORY(CBattleCondition_PetHp)
DECLARE_MODULE_FACTORY(CBattleCondition_PetMp)
DECLARE_MODULE_FACTORY(CBattleCondition_TeammateHp)
DECLARE_MODULE_FACTORY(CBattleCondition_TeammateMp)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyMultiTargetHp)
DECLARE_MODULE_FACTORY(CBattleCondition_TeammateMultiTargetHp)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyAllHp)
DECLARE_MODULE_FACTORY(CBattleCondition_TeammateAllHp)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyUnit)
DECLARE_MODULE_FACTORY(CBattleCondition_TeammateUnit)
DECLARE_MODULE_FACTORY(CBattleCondition_PlayerName)
DECLARE_MODULE_FACTORY(CBattleCondition_PlayerJob)
DECLARE_MODULE_FACTORY(CBattleCondition_PlayerGold)
DECLARE_MODULE_FACTORY(CBattleCondition_BattleBGM)
DECLARE_MODULE_FACTORY(CBattleCondition_Round)
DECLARE_MODULE_FACTORY(CBattleCondition_DoubleAction)
DECLARE_MODULE_FACTORY(CBattleCondition_TeammateDebuff)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyLevel)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyLv1Hp)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyLv1MaxHp)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyLv1MaxMp)
DECLARE_MODULE_FACTORY(CBattleCondition_EnemyType)

DECLARE_MODULE_FACTORY(CBattleCondition_EnemyAvgLevel)
DECLARE_MODULE_FACTORY(CBattleCondition_InventoryItem)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerAttack)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerGuard)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerEscape)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerExchangePosition)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerChangePet)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerSkillAttack)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerUseItem)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerLogBack)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerRebirth)
DECLARE_MODULE_FACTORY(CBattleAction_PlayerDoNothing)
DECLARE_MODULE_FACTORY(CBattleAction_PetDoNothing)
DECLARE_MODULE_FACTORY(CBattleAction_PetSkillAttack)
DECLARE_MODULE_FACTORY(CBattleTarget_Self)
DECLARE_MODULE_FACTORY(CBattleTarget_Pet)
DECLARE_MODULE_FACTORY(CBattleTarget_Enemy)
DECLARE_MODULE_FACTORY(CBattleTarget_Teammate)
DECLARE_MODULE_FACTORY(CBattleTarget_Condition)
