#include "BattleBase.h"
#include "GameCtrl.h"
extern CGA::CGAInterface *g_CGAInterface;

BattleBase::BattleBase()
{
	m_nDevType = dtNone;
}

BattleBase::BattleBase(int nDevType) :
		m_nDevType(nDevType)
{
}

BattleBase::~BattleBase()
{
}

CBattleCondition_Ignore::CBattleCondition_Ignore()
{
	m_nDevType = dtCondition_Ignore;
}

void CBattleCondition_Ignore::GetConditionName(QString &str)
{
	str = QObject::tr("Ignore");
}

bool CBattleCondition_Ignore::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	return true;
}

CBattleCondition_EnemyCount::CBattleCondition_EnemyCount(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_EnemyCount;
}

CBattleCondition_EnemyCount::CBattleCondition_EnemyCount()
{
	m_nDevType = dtCondition_EnemyCount;
}

void CBattleCondition_EnemyCount::GetConditionName(QString &str)
{
	str = QObject::tr("EnemyCount %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyCount::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			return context.m_iEnemyCount >= m_value;
		case dtCompare_GreaterThan:
			return context.m_iEnemyCount > m_value;
		case dtCompare_LessEqual:
			return context.m_iEnemyCount <= m_value;
		case dtCompare_LessThan:
			return context.m_iEnemyCount < m_value;
		case dtCompare_Equal:
			return context.m_iEnemyCount == m_value;
		case dtCompare_NotEqual:
			return context.m_iEnemyCount != m_value;
	}

	return false;
}

CBattleCondition_TeammateCount::CBattleCondition_TeammateCount(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_TeammateCount;
}

CBattleCondition_TeammateCount::CBattleCondition_TeammateCount()
{
	m_nDevType = dtCondition_TeammateCount;
}

void CBattleCondition_TeammateCount::GetConditionName(QString &str)
{
	str = QObject::tr("TeammateCount %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_TeammateCount::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			return context.m_iTeammateCount >= m_value;
		case dtCompare_GreaterThan:
			return context.m_iTeammateCount > m_value;
		case dtCompare_LessEqual:
			return context.m_iTeammateCount <= m_value;
		case dtCompare_LessThan:
			return context.m_iTeammateCount < m_value;
		case dtCompare_Equal:
			return context.m_iTeammateCount == m_value;
		case dtCompare_NotEqual:
			return context.m_iTeammateCount != m_value;
	}
	return false;
}

CBattleCondition_EnemySingleRowCount::CBattleCondition_EnemySingleRowCount(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_EnemySingleRowCount;
}

CBattleCondition_EnemySingleRowCount::CBattleCondition_EnemySingleRowCount()
{
	m_nDevType = dtCondition_EnemySingleRowCount;
}

void CBattleCondition_EnemySingleRowCount::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy SingleRow Count %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemySingleRowCount::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			if (context.m_iFrontCount >= m_value)
				return true;
			if (context.m_iBackCount >= m_value)
				return true;
			break;
		case dtCompare_GreaterThan:
			if (context.m_iFrontCount > m_value)
				return true;
			if (context.m_iBackCount > m_value)
				return true;
			break;
		case dtCompare_LessEqual:
			if (context.m_iFrontCount <= m_value)
				return true;
			if (context.m_iBackCount <= m_value)
				return true;
			break;
		case dtCompare_LessThan:
			if (context.m_iFrontCount < m_value)
				return true;
			if (context.m_iBackCount < m_value)
				return true;
			break;
		case dtCompare_Equal:
			if (context.m_iFrontCount == m_value)
				return true;
			if (context.m_iBackCount == m_value)
				return true;
			break;
		case dtCompare_NotEqual:
			if (context.m_iFrontCount != m_value)
				return true;
			if (context.m_iBackCount != m_value)
				return true;
			break;
	}

	return false;
}

CBattleCondition_PlayerHp::CBattleCondition_PlayerHp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_PlayerHp;
}

CBattleCondition_PlayerHp::CBattleCondition_PlayerHp()
{
	m_nDevType = dtCondition_PlayerHp;
}

void CBattleCondition_PlayerHp::GetConditionName(QString &str)
{
	str = QObject::tr("PlayerHP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
	if (m_percentage)
		str += QLatin1String("%");
}

bool CBattleCondition_PlayerHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	if (context.m_iPlayerPosition < 0 || context.m_iPlayerPosition >= 20)
		return false;

	int curv = context.m_UnitGroup[context.m_iPlayerPosition]->hp;
	int maxv = context.m_UnitGroup[context.m_iPlayerPosition]->maxhp;

	if (m_percentage && maxv > 0)
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv * 100 / maxv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv * 100 / maxv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv * 100 / maxv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv * 100 / maxv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv * 100 / maxv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv * 100 / maxv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}
	else
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}

	return false;
}

CBattleCondition_PlayerMp::CBattleCondition_PlayerMp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_PlayerMp;
}

CBattleCondition_PlayerMp::CBattleCondition_PlayerMp()
{
	m_nDevType = dtCondition_PlayerMp;
}

void CBattleCondition_PlayerMp::GetConditionName(QString &str)
{
	str = QObject::tr("PlayerMP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
	if (m_percentage)
		str += QLatin1String("%");
}
bool CBattleCondition_PlayerMp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	if (context.m_iPlayerPosition < 0 || context.m_iPlayerPosition >= 20)
		return false;
	int curv = context.m_UnitGroup[context.m_iPlayerPosition]->mp;
	int maxv = context.m_UnitGroup[context.m_iPlayerPosition]->maxmp;
	if (m_percentage && maxv > 0)
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv * 100 / maxv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv * 100 / maxv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv * 100 / maxv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv * 100 / maxv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv * 100 / maxv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv * 100 / maxv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}
	else
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}
	return false;
}

CBattleCondition_PetHp::CBattleCondition_PetHp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_PetHp;
}

CBattleCondition_PetHp::CBattleCondition_PetHp()
{
	m_nDevType = dtCondition_PetHp;
}

void CBattleCondition_PetHp::GetConditionName(QString &str)
{
	str = QObject::tr("PetHP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
	if (m_percentage)
		str += QLatin1String("%");
}

bool CBattleCondition_PetHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	if (context.m_iPetPosition < 0 || context.m_iPetPosition >= 20)
		return false;
	int curv = context.m_UnitGroup[context.m_iPetPosition]->hp;
	int maxv = context.m_UnitGroup[context.m_iPetPosition]->maxhp;
	if (m_percentage && maxv > 0)
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv * 100 / maxv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv * 100 / maxv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv * 100 / maxv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv * 100 / maxv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv * 100 / maxv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv * 100 / maxv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}
	else
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}
	return false;
}

CBattleCondition_PetMp::CBattleCondition_PetMp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_PetMp;
}

CBattleCondition_PetMp::CBattleCondition_PetMp()
{
	m_nDevType = dtCondition_PetMp;
}

void CBattleCondition_PetMp::GetConditionName(QString &str)
{
	str = QObject::tr("PetMP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
	if (m_percentage)
		str += QLatin1String("%");
}
bool CBattleCondition_PetMp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	if (context.m_iPetPosition < 0 || context.m_iPetPosition >= 20)
		return false;
	int curv = context.m_UnitGroup[context.m_iPetPosition]->mp;
	int maxv = context.m_UnitGroup[context.m_iPetPosition]->maxmp;
	if (m_percentage && maxv > 0)
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv * 100 / maxv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv * 100 / maxv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv * 100 / maxv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv * 100 / maxv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv * 100 / maxv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv * 100 / maxv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}
	else
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}
	return false;
}

CBattleCondition_TeammateHp::CBattleCondition_TeammateHp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_TeammateHp;
}

CBattleCondition_TeammateHp::CBattleCondition_TeammateHp()
{
	m_nDevType = dtCondition_TeammateHp;
}

void CBattleCondition_TeammateHp::GetConditionName(QString &str)
{
	str = QObject::tr("TeammateHP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
	if (m_percentage)
		str += QLatin1String("%");
}

bool CBattleCondition_TeammateHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	for (int i = 0; i < 0xA; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		int curv = context.m_UnitGroup[i]->hp;
		//修复队友死亡后，一直用技能bug
		if (curv == 0)
			continue;

		int maxv = context.m_UnitGroup[i]->maxhp;
		//qDebug("curhp%d maxhp%d v%d r%d p%d", curv, maxv, m_value, m_relation, m_percentage?1:0);
		if (m_percentage && maxv > 0)
		{
			switch (m_relation)
			{
				case dtCompare_GreaterEqual:
					if (curv * 100 / maxv >= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_GreaterThan:
					if (curv * 100 / maxv > m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessEqual:
					if (curv * 100 / maxv <= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessThan:
					if (curv * 100 / maxv < m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_Equal:
					if (curv * 100 / maxv == m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_NotEqual:
					if (curv * 100 / maxv != m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
			}
		}
		else
		{
			switch (m_relation)
			{
				case dtCompare_GreaterEqual:
					if (curv >= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_GreaterThan:
					if (curv > m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessEqual:
					if (curv <= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessThan:
					if (curv < m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_Equal:
					if (curv == m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_NotEqual:
					if (curv != m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
			}
		}
	}
	return false;
}

CBattleCondition_TeammateMp::CBattleCondition_TeammateMp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_TeammateMp;
}

CBattleCondition_TeammateMp::CBattleCondition_TeammateMp()
{
	m_nDevType = dtCondition_TeammateMp;
}

void CBattleCondition_TeammateMp::GetConditionName(QString &str)
{
	str = QObject::tr("TeammateMP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
	if (m_percentage)
		str += QLatin1String("%");
}

bool CBattleCondition_TeammateMp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	for (int i = 0; i < 0xA; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		int curv = context.m_UnitGroup[i]->mp;
		int maxv = context.m_UnitGroup[i]->maxmp;
		//qDebug("curhp%d maxhp%d v%d r%d p%d", curv, maxv, m_value, m_relation, m_percentage?1:0);
		if (m_percentage && maxv > 0)
		{
			switch (m_relation)
			{
				case dtCompare_GreaterEqual:
					if (curv * 100 / maxv >= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_GreaterThan:
					if (curv * 100 / maxv > m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessEqual:
					if (curv * 100 / maxv <= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessThan:
					if (curv * 100 / maxv < m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_Equal:
					if (curv * 100 / maxv == m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_NotEqual:
					if (curv * 100 / maxv != m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
			}
		}
		else
		{
			switch (m_relation)
			{
				case dtCompare_GreaterEqual:
					if (curv >= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_GreaterThan:
					if (curv > m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessEqual:
					if (curv <= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessThan:
					if (curv < m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_Equal:
					if (curv == m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_NotEqual:
					if (curv != m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
			}
		}
	}
	return false;
}

CBattleCondition_EnemyMultiTargetHp::CBattleCondition_EnemyMultiTargetHp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_EnemyMultiTargetHp;
}

CBattleCondition_EnemyMultiTargetHp::CBattleCondition_EnemyMultiTargetHp()
{
	m_nDevType = dtCondition_EnemyMultiTargetHp;
}

void CBattleCondition_EnemyMultiTargetHp::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy MultiTarget HP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyMultiTargetHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	for (int i = 0xA; i < 20; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		int curv = context.m_UnitGroup[i]->hp;
		int maxv = context.m_UnitGroup[i]->maxhp;
		//qDebug("curhp%d maxhp%d v%d r%d p%d", curv, maxv, m_value, m_relation, m_percentage?1:0);
		if (m_percentage && maxv > 0)
		{
			switch (m_relation)
			{
				case dtCompare_GreaterEqual:
					if (curv * 100 / maxv >= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_GreaterThan:
					if (curv * 100 / maxv > m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessEqual:
					if (curv * 100 / maxv <= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessThan:
					if (curv * 100 / maxv < m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_Equal:
					if (curv * 100 / maxv == m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_NotEqual:
					if (curv * 100 / maxv != m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
			}
		}
		else
		{
			switch (m_relation)
			{
				case dtCompare_GreaterEqual:
					if (curv >= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_GreaterThan:
					if (curv > m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessEqual:
					if (curv <= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessThan:
					if (curv < m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_Equal:
					if (curv == m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_NotEqual:
					if (curv != m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
			}
		}
	}
	return false;
}

CBattleCondition_TeammateMultiTargetHp::CBattleCondition_TeammateMultiTargetHp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_TeammateMultiTargetHp;
}

CBattleCondition_TeammateMultiTargetHp::CBattleCondition_TeammateMultiTargetHp()
{
	m_nDevType = dtCondition_TeammateMultiTargetHp;
}

void CBattleCondition_TeammateMultiTargetHp::GetConditionName(QString &str)
{
	str = QObject::tr("Teammate MultiTarget HP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
	if (m_percentage)
		str += QLatin1String("%");
}

bool CBattleCondition_TeammateMultiTargetHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	for (int i = 0; i < 0xA; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		int curv = context.m_UnitGroup[i]->multi_hp;
		int maxv = context.m_UnitGroup[i]->multi_maxhp;
		qDebug("CBattleCondition_TeammateMultiTargetHp curhp%d maxhp%d v%d r%d p%d", curv, maxv, m_value, m_relation, m_percentage ? 1 : 0);
		if (m_percentage && maxv > 0)
		{
			switch (m_relation)
			{
				case dtCompare_GreaterEqual:
					if (curv * 100 / maxv >= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_GreaterThan:
					if (curv * 100 / maxv > m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessEqual:
					if (curv * 100 / maxv <= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessThan:
					if (curv * 100 / maxv < m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_Equal:
					if (curv * 100 / maxv == m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_NotEqual:
					if (curv * 100 / maxv != m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
			}
		}
		else
		{
			switch (m_relation)
			{
				case dtCompare_GreaterEqual:
					if (curv >= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_GreaterThan:
					if (curv > m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessEqual:
					if (curv <= m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_LessThan:
					if (curv < m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_Equal:
					if (curv == m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
				case dtCompare_NotEqual:
					if (curv != m_value)
					{
						conditionTarget = i;
						return true;
					}
					break;
			}
		}
	}
	return false;
}

CBattleCondition_EnemyAllHp::CBattleCondition_EnemyAllHp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_EnemyAllHp;
}

CBattleCondition_EnemyAllHp::CBattleCondition_EnemyAllHp()
{
	m_nDevType = dtCondition_EnemyAllHp;
}

void CBattleCondition_EnemyAllHp::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy All HP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyAllHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	int curv = 0, maxv = 0;
	for (int i = 0xA; i < 20; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		curv += context.m_UnitGroup[i]->hp;
		maxv += context.m_UnitGroup[i]->maxhp;
	}

	if (m_percentage && maxv > 0)
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv * 100 / maxv >= m_value)
				{
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv * 100 / maxv > m_value)
				{
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv * 100 / maxv <= m_value)
				{
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv * 100 / maxv < m_value)
				{
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv * 100 / maxv == m_value)
				{
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv * 100 / maxv != m_value)
				{
					return true;
				}
				break;
		}
	}
	else
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv >= m_value)
				{
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv > m_value)
				{
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv <= m_value)
				{
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv < m_value)
				{
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv == m_value)
				{
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv != m_value)
				{
					return true;
				}
				break;
		}
	}

	return false;
}

CBattleCondition_TeammateAllHp::CBattleCondition_TeammateAllHp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_TeammateAllHp;
}

CBattleCondition_TeammateAllHp::CBattleCondition_TeammateAllHp()
{
	m_nDevType = dtCondition_TeammateAllHp;
}

void CBattleCondition_TeammateAllHp::GetConditionName(QString &str)
{
	str = QObject::tr("Teammate All HP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
	if (m_percentage)
		str += QLatin1String("%");
}

bool CBattleCondition_TeammateAllHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	int curv = 0, maxv = 0;
	for (int i = 0; i < 0xA; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		curv += context.m_UnitGroup[i]->hp;
		maxv += context.m_UnitGroup[i]->maxhp;
	}

	if (m_percentage && maxv > 0)
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv * 100 / maxv >= m_value)
				{
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv * 100 / maxv > m_value)
				{
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv * 100 / maxv <= m_value)
				{
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv * 100 / maxv < m_value)
				{
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv * 100 / maxv == m_value)
				{
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv * 100 / maxv != m_value)
				{
					return true;
				}
				break;
		}
	}
	else
	{
		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv >= m_value)
				{
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv > m_value)
				{
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv <= m_value)
				{
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv < m_value)
				{
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv == m_value)
				{
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv != m_value)
				{
					return true;
				}
				break;
		}
	}
	return false;
}

CBattleCondition_EnemyUnit::CBattleCondition_EnemyUnit(int relation, QString &unitName)
{
	m_relation = relation;
	SetUnitName(unitName);
	//	m_UnitName = unitName;
	m_nDevType = dtCondition_EnemyUnit;
}

CBattleCondition_EnemyUnit::CBattleCondition_EnemyUnit()
{
	m_nDevType = dtCondition_EnemyUnit;
}

void CBattleCondition_EnemyUnit::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), m_UnitName);
}

bool CBattleCondition_EnemyUnit::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	for (int i = 0xA; i < 20; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		if (context.m_UnitGroup[i]->name == m_UnitName)
		{
			if (m_relation == dtCompare_Contain)
			{
				conditionTarget = i;
				return true;
			}
			else if (m_relation == dtCompare_NotContain)
			{
				return false;
			}
		}
	}
	return (m_relation == dtCompare_NotContain) ? true : false;
}

void CBattleCondition_EnemyUnit::SetUnitName(const QString &szName)
{
	if (szName.size() < 250)
	{
		strcpy(m_UnitName, szName.toStdString().c_str());
	}
}

void CBattleCondition_EnemyUnit::SetConditionTextData(QStringList data)
{
	if (data.size() > 0)
	{
		//		m_UnitName = data[0];
		SetUnitName(data[0]);
	}
}
CBattleCondition_TeammateUnit::CBattleCondition_TeammateUnit(int relation, QString &unitName)
{
	m_relation = relation;
	SetUnitName(unitName);
	//	m_UnitName = unitName;
	m_nDevType = dtCondition_TeammateUnit;
}

CBattleCondition_TeammateUnit::CBattleCondition_TeammateUnit()
{
	m_nDevType = dtCondition_TeammateUnit;
}

void CBattleCondition_TeammateUnit::GetConditionName(QString &str)
{
	str = QObject::tr("Teammates %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), m_UnitName);
}

bool CBattleCondition_TeammateUnit::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	for (int i = 0; i < 0xA; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;

		if (context.m_UnitGroup[i]->name == m_UnitName)
		{
			if (m_relation == dtCompare_Contain)
			{
				conditionTarget = i;
				return true;
			}
			else if (m_relation == dtCompare_NotContain)
			{
				return false;
			}
		}
	}
	return (m_relation == dtCompare_NotContain) ? true : false;
}

void CBattleCondition_TeammateUnit::SetUnitName(const QString &szName)
{
	if (szName.size() < 250)
	{
		strcpy(m_UnitName, szName.toStdString().c_str());
	}
}

void CBattleCondition_TeammateUnit::SetConditionTextData(QStringList data)
{

	if (data.size() > 0)
	{
		//m_ItemName = data[0];
		SetUnitName(data[0]);
	}
}

CBattleCondition_Round::CBattleCondition_Round(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_Round;
}

CBattleCondition_Round::CBattleCondition_Round()
{
	m_nDevType = dtCondition_Round;
}

void CBattleCondition_Round::GetConditionName(QString &str)
{
	str = QObject::tr("Round %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_Round::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			return context.m_iRoundCount + 1 >= m_value;
		case dtCompare_GreaterThan:
			return context.m_iRoundCount + 1 > m_value;
		case dtCompare_LessEqual:
			return context.m_iRoundCount + 1 <= m_value;
		case dtCompare_LessThan:
			return context.m_iRoundCount + 1 < m_value;
		case dtCompare_Equal:
			return context.m_iRoundCount + 1 == m_value;
		case dtCompare_NotEqual:
			return context.m_iRoundCount + 1 != m_value;
	}

	return false;
}

CBattleCondition_DoubleAction::CBattleCondition_DoubleAction()
{
	m_nDevType = dtCondition_DoubleAction;
}

void CBattleCondition_DoubleAction::GetConditionName(QString &str)
{
	str = QObject::tr("DoubleAction");
}

bool CBattleCondition_DoubleAction::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	if (context.m_bIsDouble)
		return true;

	return false;
}

CBattleCondition_TeammateDebuff::CBattleCondition_TeammateDebuff(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_TeammateDebuff;
}

CBattleCondition_TeammateDebuff::CBattleCondition_TeammateDebuff()
{
	m_nDevType = dtCondition_TeammateDebuff;
}

void CBattleCondition_TeammateDebuff::GetConditionName(QString &str)
{
	using namespace CGA;
	QString debuff;
	if (m_value == FL_DEBUFF_ANY)
	{
		debuff += QObject::tr("Any");
	}
	else
	{
		if (m_value & FL_DEBUFF_SLEEP)
		{
			if (!debuff.isEmpty())
				debuff += " ";
			debuff += QObject::tr("Sleep");
		}
		if (m_value & FL_DEBUFF_MEDUSA)
		{
			if (!debuff.isEmpty())
				debuff += " ";
			debuff += QObject::tr("Medusa");
		}
		if (m_value & FL_DEBUFF_DRUNK)
		{
			if (!debuff.isEmpty())
				debuff += " ";
			debuff += QObject::tr("Drunk");
		}
		if (m_value & FL_DEBUFF_CHAOS)
		{
			if (!debuff.isEmpty())
				debuff += " ";
			debuff += QObject::tr("Chaos");
		}
		if (m_value & FL_DEBUFF_FORGET)
		{
			if (!debuff.isEmpty())
				debuff += " ";
			debuff += QObject::tr("Forget");
		}
		if (m_value & FL_DEBUFF_POISON)
		{
			if (!debuff.isEmpty())
				debuff += " ";
			debuff += QObject::tr("Poison");
		}
	}

	str = QObject::tr("Teammate Debuff %1 %2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), debuff);
}

bool CBattleCondition_TeammateDebuff::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	switch (m_relation)
	{
		case dtCompare_Contain:
		{
			for (int i = 0; i < 10; ++i)
			{
				if (context.m_UnitGroup[i]->exist)
				{
					if (context.m_UnitGroup[i]->flags & m_value)
					{
						conditionTarget = i;
						return true;
					}
				}
			}
			return false;
		}
		case dtCompare_NotContain:
		{
			for (int i = 0; i < 10; ++i)
			{
				if (context.m_UnitGroup[i]->exist)
				{
					if (context.m_UnitGroup[i]->flags & m_value)
						return false;
				}
			}
			return true;
		}
	}

	return false;
}

CBattleCondition_EnemyLevel::CBattleCondition_EnemyLevel(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_EnemyLevel;
}

CBattleCondition_EnemyLevel::CBattleCondition_EnemyLevel()
{
	m_nDevType = dtCondition_EnemyLevel;
}

void CBattleCondition_EnemyLevel::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy Level %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyLevel::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	if (m_relation == dtCompare_NotContain || m_relation == dtCompare_Contain) //不包含
	{
		bool bFind = false;
		for (int i = 0xA; i < 20; ++i)
		{
			if (!context.m_UnitGroup[i]->exist)
				continue;
			int curv = context.m_UnitGroup[i]->level;
			if (curv == m_value)
			{
				conditionTarget = i;
				bFind = true;
				break;
			}
		}
		if (bFind && m_relation == dtCompare_Contain)
			return true;
		else if (!bFind && m_relation == dtCompare_NotContain)
			return true;
		return false;
	}
	for (int i = 0xA; i < 20; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		int curv = context.m_UnitGroup[i]->level;

		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv >= m_value)
				{
					conditionTarget = i;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv > m_value)
				{
					conditionTarget = i;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv <= m_value)
				{
					conditionTarget = i;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv < m_value)
				{
					conditionTarget = i;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv == m_value)
				{
					conditionTarget = i;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv != m_value)
				{
					conditionTarget = i;
					return true;
				}
				break;
		}
	}
	return false;
}

CBattleCondition_EnemyAvgLevel::CBattleCondition_EnemyAvgLevel(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_EnemyAvgLevel;
}

CBattleCondition_EnemyAvgLevel::CBattleCondition_EnemyAvgLevel()
{
	m_nDevType = dtCondition_EnemyAvgLevel;
}

void CBattleCondition_EnemyAvgLevel::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy Average Level %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyAvgLevel::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	int level = 0, count = 0;
	for (int i = 0xA; i < 20; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		level += context.m_UnitGroup[i]->level;
		count += 1;
	}

	float avg = (float)level / count;

	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			if (avg >= (float)m_value)
			{
				return true;
			}
			break;
		case dtCompare_GreaterThan:
			if (avg > (float)m_value)
			{
				return true;
			}
			break;
		case dtCompare_LessEqual:
			if (avg <= (float)m_value)
			{
				return true;
			}
			break;
		case dtCompare_LessThan:
			if (avg < (float)m_value)
			{
				return true;
			}
			break;
		case dtCompare_Equal:
			if (avg == (float)m_value)
			{
				return true;
			}
			break;
		case dtCompare_NotEqual:
			if (avg != (float)m_value)
			{
				return true;
			}
			break;
	}

	return false;
}

CBattleCondition_InventoryItem::CBattleCondition_InventoryItem(int relation, QString &itemName)
{
	m_relation = relation;
	//m_ItemName = itemName;
	SetItemName(itemName);
	m_nDevType = dtCondition_InventoryItem;
}

CBattleCondition_InventoryItem::CBattleCondition_InventoryItem()
{
	m_nDevType = dtCondition_InventoryItem;
}

void CBattleCondition_InventoryItem::GetConditionName(QString &str)
{
	str = QObject::tr("Inventory %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), m_ItemName);
}

bool CBattleCondition_InventoryItem::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	QString tItemName(m_ItemName);
	for (int i = 0; i < context.m_Items.size(); ++i)
	{
		if (context.m_Items.at(i)->exist == false)
		{
			continue;
		}
		if (context.m_Items.at(i)->name == tItemName)
		{
			if (m_relation == dtCompare_Contain)
			{
				return true;
			}
			else if (m_relation == dtCompare_NotContain)
			{
				return false;
			}
		}
		else if (tItemName[0] == '@')
		{
			bool ok = false;
			auto val = tItemName.mid(1).toInt(&ok);
			if (ok && context.m_Items.at(i)->type == val)
			{
				if (m_relation == dtCompare_Contain)
				{
					return true;
				}
				else if (m_relation == dtCompare_NotContain)
				{
					return false;
				}
			}
		}
		else if (tItemName[0] == '#')
		{
			bool ok = false;
			auto val = tItemName.mid(1).toInt(&ok);
			if (ok && context.m_Items.at(i)->id == val)
			{
				if (m_relation == dtCompare_Contain)
				{
					return true;
				}
				else if (m_relation == dtCompare_NotContain)
				{
					return false;
				}
			}
		}
	}
	return (m_relation == dtCompare_NotContain) ? true : false;
}

void CBattleCondition_InventoryItem::SetItemName(const QString &szText)
{
	if (szText.size() < 250)
	{
		strcpy(m_ItemName, szText.toStdString().c_str());
	}
}

void CBattleCondition_InventoryItem::SetConditionTextData(QStringList data)
{
	if (data.size() > 0)
	{
		//m_ItemName = data[0];
		SetItemName(data[0]);
	}
}

CBattleCondition_PlayerName::CBattleCondition_PlayerName(int relation, QString &playerName)
{
	m_relation = relation;
	SetPlayerName(playerName);
	m_nDevType = dtCondition_PlayerName;
}

CBattleCondition_PlayerName::CBattleCondition_PlayerName()
{
	m_nDevType = dtCondition_PlayerName;
}

void CBattleCondition_PlayerName::GetConditionName(QString &str)
{
	str = QObject::tr("PlayerName %1%2").arg(dtCondition_PlayerName).arg(m_PlayerName);
}

bool CBattleCondition_PlayerName::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	auto playerInfo = context.m_character;
	if (playerInfo)
	{
		if (m_relation == dtCompare_Contain)
		{
			if (QString(playerInfo->name).indexOf(m_PlayerName) >= 0)
			{
				conditionTarget = context.m_iPlayerPosition;
				return true;
			}
		}
		else if (m_relation == dtCompare_NotContain)
		{
			if (QString(playerInfo->name).indexOf(m_PlayerName) < 0)
			{
				conditionTarget = context.m_iPlayerPosition;
				return true;
			}
		}
	}
	return false;
}

void CBattleCondition_PlayerName::SetPlayerName(const QString &szName)
{
	if (szName.size() < 250)
	{
		strcpy(m_PlayerName, szName.toStdString().c_str());
	}
}

void CBattleCondition_PlayerName::SetConditionTextData(QStringList data)
{
	if (data.size() > 0)
	{
		SetPlayerName(data[0]);
	}
}

CBattleCondition_PlayerJob::CBattleCondition_PlayerJob(int relation, QString &playerJob)
{
	m_relation = relation;
	SetPlayerJob(playerJob);
	m_nDevType = dtCondition_PlayerJob;
}

CBattleCondition_PlayerJob::CBattleCondition_PlayerJob()
{
	m_nDevType = dtCondition_PlayerJob;
}

void CBattleCondition_PlayerJob::GetConditionName(QString &str)
{
	str = QObject::tr("PlayerJob %1%2").arg(m_relation).arg(m_PlayerJob);
}

bool CBattleCondition_PlayerJob::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	auto playerInfo = context.m_character;
	if (playerInfo)
	{
		if (m_relation == dtCompare_Contain)
		{
			if (playerInfo->job.indexOf(m_PlayerJob) >= 0)
			{
				conditionTarget = context.m_iPlayerPosition;
				return true;
			}
		}
		else if (m_relation == dtCompare_NotContain)
		{
			if (playerInfo->job.indexOf(m_PlayerJob) < 0)
			{
				conditionTarget = context.m_iPlayerPosition;
				return true;
			}
		}
	}
	return false;
}

void CBattleCondition_PlayerJob::SetPlayerJob(const QString &szName)
{
	if (szName.size() < 250)
	{
		strcpy(m_PlayerJob, szName.toStdString().c_str());
	}
}

void CBattleCondition_PlayerJob::SetConditionTextData(QStringList data)
{
	if (data.size() > 0)
	{
		SetPlayerJob(data[0]);
	}
}

CBattleCondition_PlayerGold::CBattleCondition_PlayerGold(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_PlayerGold;
}

CBattleCondition_PlayerGold::CBattleCondition_PlayerGold()
{
	m_nDevType = dtCondition_PlayerGold;
}

void CBattleCondition_PlayerGold::GetConditionName(QString &str)
{
	str = QObject::tr("PlayerGold %1%2").arg(m_relation).arg(QString::number(m_value));
}

bool CBattleCondition_PlayerGold::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	auto playerInfo = context.m_character;
	if (playerInfo)
	{
		int curv = playerInfo->gold;

		switch (m_relation)
		{
			case dtCompare_GreaterEqual:
				if (curv >= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_GreaterThan:
				if (curv > m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessEqual:
				if (curv <= m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_LessThan:
				if (curv < m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_Equal:
				if (curv == m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
			case dtCompare_NotEqual:
				if (curv != m_value)
				{
					conditionTarget = context.m_iPlayerPosition;
					return true;
				}
				break;
		}
	}
	return false;
}

CBattleCondition_BattleBGM::CBattleCondition_BattleBGM(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_PlayerBGM;
}

CBattleCondition_BattleBGM::CBattleCondition_BattleBGM()
{
	m_nDevType = dtCondition_PlayerBGM;
}

void CBattleCondition_BattleBGM::GetConditionName(QString &str)
{
	str = QObject::tr("BGM %1%2").arg(m_relation).arg(QString::number(m_value));
}

bool CBattleCondition_BattleBGM::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			return context.m_iBGMIndex >= m_value;
		case dtCompare_GreaterThan:
			return context.m_iBGMIndex > m_value;
		case dtCompare_LessEqual:
			return context.m_iBGMIndex <= m_value;
		case dtCompare_LessThan:
			return context.m_iBGMIndex < m_value;
		case dtCompare_Equal:
			return context.m_iBGMIndex == m_value;
		case dtCompare_NotEqual:
			return context.m_iBGMIndex != m_value;
	}

	return false;
}

CBattleAction_PlayerAttack::CBattleAction_PlayerAttack()
{
	m_nDevType = dtAction_PlayerAttack;
}

void CBattleAction_PlayerAttack::GetActionName(QString &str, bool config)
{
	str = QObject::tr("Attack");
}

bool CBattleAction_PlayerAttack::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	bool result = false;
	//qDebug() << "PlayerAttack: target" << target;

	g_CGAInterface->BattleNormalAttack(target, result);
	return result;
}

CBattleAction_PlayerGuard::CBattleAction_PlayerGuard()
{
	m_nDevType = dtAction_PlayerGuard;
}

void CBattleAction_PlayerGuard::GetActionName(QString &str, bool config)
{
	str = QObject::tr("Guard");
}

bool CBattleAction_PlayerGuard::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	bool result = false;
	//qDebug() << "PlayerGuard:";
	g_CGAInterface->BattleGuard(result);
	return result;
}

CBattleAction_PlayerEscape::CBattleAction_PlayerEscape()
{
	m_nDevType = dtAction_PlayerEscape;
}

void CBattleAction_PlayerEscape::GetActionName(QString &str, bool config)
{
	str = QObject::tr("Escape");
}

bool CBattleAction_PlayerEscape::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	bool result = false;
	//qDebug() << "PlayerEscape:";
	g_CGAInterface->BattleEscape(result);
	context.m_bIsPlayerEscaped = true;
	return result;
}

CBattleAction_PlayerExchangePosition::CBattleAction_PlayerExchangePosition()
{
	m_nDevType = dtAction_PlayerExchangePosition;
}

void CBattleAction_PlayerExchangePosition::GetActionName(QString &str, bool config)
{
	str = QObject::tr("Exchange Position");
}

bool CBattleAction_PlayerExchangePosition::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	bool result = false;
	//qDebug() << "PlayerExchangePosition:";
	g_CGAInterface->BattleExchangePosition(result);
	return result;
}

CBattleAction_PlayerChangePet::CBattleAction_PlayerChangePet(int type)
{
	m_Type = type;
	m_nDevType = dtAction_PlayerChangePet;
}

CBattleAction_PlayerChangePet::CBattleAction_PlayerChangePet(QString &petname)
{
	m_Type = 0;
	//m_PetName = petname;
	SetPetName(petname);
	m_nDevType = dtAction_PlayerChangePet;
}

CBattleAction_PlayerChangePet::CBattleAction_PlayerChangePet()
{
	m_nDevType = dtAction_PlayerChangePet;
}

void CBattleAction_PlayerChangePet::GetActionName(QString &str, bool config)
{
	QString type;
	switch (m_Type)
	{
		case 0:
			str = QObject::tr("Call pet %1").arg(m_PetName);
			break;
		case 1:
			str = QObject::tr("Recall pet");
			break;
		case 2:
			str = QObject::tr("Call pet with highest level");
			break;
		case 3:
			str = QObject::tr("Call pet with highest health");
			break;
		case 4:
			str = QObject::tr("Call pet with highest mana");
			break;
		case 5:
			str = QObject::tr("Call pet with highest loyalty");
			break;
		case 6:
			str = QObject::tr("Call pet with lowest level");
			break;
		case 7:
			str = QObject::tr("Call pet with lowest health");
			break;
		case 8:
			str = QObject::tr("Call pet with lowest mana");
			break;
		case 9:
			str = QObject::tr("Call pet with lowest loyalty");
			break;
	}
}

bool CBattleAction_PlayerChangePet::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	int petid = -1;
	bool found = false;
	switch (m_Type)
	{
		case TChangePet_Call:
		{
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->showname == m_PetName)
				{
					petid = pet->id;
					found = true;
					break;
				}
			}
			break;
		}
		case TChangePet_Recall:
		{
			petid = -1;
			found = true;
			break;
		}
		case TChangePet_HighestLv:
		{
			int highest = 0;
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->level > highest)
				{
					highest = pet->level;
					petid = pet->id;
					found = true;
				}
			}
			break;
		}
		case TChangePet_HighestHp:
		{
			int highest = 0;
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->hp > highest)
				{
					highest = pet->hp;
					petid = pet->id;
					found = true;
				}
			}
			break;
		}
		case TChangePet_HighestMp:
		{
			int highest = 0;
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->mp > highest)
				{
					highest = pet->mp;
					petid = pet->id;
					found = true;
				}
			}
			break;
		}
		case TChangePet_HighestLoyalty:
		{
			int highest = 0;
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->loyality > highest)
				{
					highest = pet->loyality;
					petid = pet->id;
					found = true;
				}
			}
			break;
		}
		case TChangePet_LowestLv:
		{
			int lowest = 9999;
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->level < lowest)
				{
					lowest = pet->level;
					petid = pet->id;
					found = true;
				}
			}
			break;
		}
		case TChangePet_LowestHp:
		{
			int lowest = 9999;
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->hp < lowest)
				{
					lowest = pet->hp;
					petid = pet->id;
					found = true;
				}
			}
			break;
		}
		case TChangePet_LowestMp:
		{
			int lowest = 9999;
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->mp < lowest)
				{
					lowest = pet->mp;
					petid = pet->id;
					found = true;
				}
			}
			break;
		}
		case TChangePet_LowestLoyalty:
		{
			int lowest = 9999;
			for (int i = 0; i < context.m_Pets.size(); ++i)
			{
				auto pet = context.m_Pets.at(i);
				if (pet->id == context.m_iPetId)
					continue;
				if (pet->hp == 0)
					continue;
				if ((pet->battle_flags & 3) == 0)
					continue;
				if (pet->loyality < lowest)
				{
					lowest = pet->loyality;
					petid = pet->id;
					found = true;
				}
			}
			break;
		}
	}
	//qDebug() << "PlayerChangePet: found" << found << " petid:" << petid;

	bool result = false;
	if (found)
	{
		g_CGAInterface->BattleChangePet(petid, result);
	}
	return result;
}

void CBattleAction_PlayerChangePet::SetPetName(const QString &szName)
{
	if (szName.size() < 250)
	{
		strcpy(m_PetName, szName.toStdString().c_str());
	}
}

void CBattleAction_PlayerChangePet::SetActionTextData(QStringList data)
{
	if (data.size() > 0)
	{
		//m_PetName = data.at(0);
		SetPetName(data[0]);
	}
}

void CBattleAction_PlayerChangePet::SetActionData(QList<int> data)
{
	if (data.size() > 0)
	{
		m_Type = data.at(0);
	}
}

CBattleAction_PlayerSkillAttack::CBattleAction_PlayerSkillAttack(QString &skillName, int skillLevel)
{
	//m_SkillName = skillName;
	setSkillName(skillName);
	m_SkillLevel = skillLevel;
	m_nDevType = dtAction_PlayerSkillAttack;
}

CBattleAction_PlayerSkillAttack::CBattleAction_PlayerSkillAttack()
{
	m_nDevType = dtAction_PlayerSkillAttack;
}

void CBattleAction_PlayerSkillAttack::GetActionName(QString &str, bool config)
{
	str = m_SkillName;
	if (m_SkillLevel >= 1 && m_SkillLevel <= 10)
		str += QObject::tr(" Lv %1").arg(QString::number(m_SkillLevel));
	else
		str += QObject::tr(" Lv Max");
}

int CBattleAction_PlayerSkillAttack::GetTargetFlags(CGA_BattleContext_t &context)
{
	if (context.m_PlayerSkills.empty())
		return 0;

	int skill_pos, skill_level;
	if (!GetSkill(context, skill_pos, skill_level))
		return 0;

	for (int i = 0; i < context.m_PlayerSkills.size(); ++i)
	{
		auto skill = context.m_PlayerSkills.at(i);
		if (skill_pos == skill->index)
		{
			int flags = skill->subskills.at(skill_level)->flags;
			return flags;
		}
	}
	return 0;
}

void CBattleAction_PlayerSkillAttack::setSkillName(const QString &szText)
{
	if (szText.size() < 250)
	{
		strcpy(m_SkillName, szText.toStdString().c_str());
	}
}

void CBattleAction_PlayerSkillAttack::SetActionTextData(QStringList data)
{
	if (data.size() > 0)
	{
		setSkillName(data[0]);
		//		m_SkillName = data[0];
	}
}

void CBattleAction_PlayerSkillAttack::SetActionData(QList<int> data)
{
	if (data.size() > 0)
	{
		m_SkillLevel = data[0];
	}
}

bool CBattleAction_PlayerSkillAttack::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	int skill_pos, skill_level;

	bool result = false;
	if (context.m_bIsSkillPerformed && GetSkill(context, skill_pos, skill_level))
	{
		qDebug("BattleSkillAttack %d %d %d", skill_pos, skill_level, target);
		FixTarget(context, skill_pos, skill_level, target);
		g_CGAInterface->BattleSkillAttack(skill_pos, skill_level, target, context.m_bIsPlayerForceAction ? true : false, result);
	}

	if (!result)
	{
		qDebug("BattleNormalAttack %d %d", skill_pos, target);
		g_CGAInterface->BattleNormalAttack(defaultTarget, result);
	}
	return result;
}

bool CBattleAction_PlayerSkillAttack::GetSkill(CGA_BattleContext_t &context, int &skillpos, int &skilllevel)
{
	if (context.m_iPlayerPosition < 0 || context.m_iPlayerPosition >= 20)
		return false;
	if (context.m_PlayerSkills.empty())
		return false;
	for (int i = 0; i < context.m_PlayerSkills.size(); ++i)
	{
		auto skill = context.m_PlayerSkills.at(i);
		if (m_SkillName == skill->name)
		{
			skillpos = skill->index;
			skilllevel = skill->level;
			if (m_SkillLevel >= 1 && skilllevel > m_SkillLevel)
				skilllevel = m_SkillLevel;
			int playerMp = context.m_UnitGroup[context.m_iPlayerPosition]->mp;
			GameSkillList subsks = skill->subskills;
			for (int j = subsks.size() - 1; j >= 0; --j)
			{
				if (skilllevel >= subsks.at(j)->level && subsks.at(j)->available && playerMp >= subsks.at(j)->cost)
				{
					skilllevel = j;
					//qDebug("j = %d", skilllevel);
					return true;
				}
			}
			return false;
		}
	}

	return false;
}

void CBattleAction_PlayerSkillAttack::FixTarget(CGA_BattleContext_t &context, int skill_pos, int skill_level, int &target)
{
	if (context.m_PlayerSkills.empty())
		return;

	for (int i = 0; i < context.m_PlayerSkills.size(); ++i)
	{
		auto skill = context.m_PlayerSkills.at(i);
		if (skill_pos == skill->index)
		{
			int flags = skill->subskills.at(skill_level)->flags;

			if (!(flags & FL_SKILL_SELECT_TARGET))
			{
				target = context.m_iPlayerPosition;
				return;
			}
			if (flags & FL_SKILL_SINGLE)
			{
				return;
			}
			if (flags & FL_SKILL_MULTI)
			{
				target = target + 20;
				return;
			}
			if (flags & FL_SKILL_ALL)
			{
				target = (target >= 10 && target <= 19) ? 41 : 40;
				return;
			}
			if (flags & FL_SKILL_BOOM)
			{
				target = 42;
				return;
			}
			target = 255;
			return;
		}
	}
}

CBattleAction_PlayerUseItem::CBattleAction_PlayerUseItem(QString &itemName)
{
	m_nDevType = dtAction_PlayerUseItem;
	m_ItemId = 0;
	m_ItemType = 0;
	if (itemName[0] == '#')
	{
		bool bValue = false;
		int value = itemName.mid(1).toInt(&bValue);
		if (bValue)
			m_ItemId = value;
	}
	else if (itemName[0] == '@')
	{
		bool bValue = false;
		int value = itemName.mid(1).toInt(&bValue);
		if (bValue)
			m_ItemType = value;
	}
	else
	{
		//m_ItemName = itemName;
		if (itemName.size() < 250)
		{
			strcpy(m_ItemName, itemName.toStdString().c_str());
		}
	}
}

CBattleAction_PlayerUseItem::CBattleAction_PlayerUseItem()
{
	m_nDevType = dtAction_PlayerUseItem;
}

void CBattleAction_PlayerUseItem::GetActionName(QString &str, bool config)
{
	if (!config)
	{
		if (m_ItemId > 0)
		{
			str = QObject::tr("Use item id #%1").arg(m_ItemId);
		}
		else if (m_ItemType > 0)
		{
			str = QObject::tr("Use item type @%1").arg(m_ItemType);
		}
		else
		{
			str = QObject::tr("Use item %1").arg(m_ItemName);
		}
	}
	else
	{
		if (m_ItemId > 0)
		{
			str = QString("#%1").arg(m_ItemId);
		}
		else if (m_ItemType > 0)
		{
			str = QString("@%1").arg(m_ItemType);
		}
		else
		{
			str = m_ItemName;
		}
	}
}

bool CBattleAction_PlayerUseItem::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	int itempos = -1;
	bool result = false;
	if (GetItemPosition(context, itempos))
	{
		//qDebug() << "PlayerUseItem itempos:" << itempos << " target:" << target;
		g_CGAInterface->BattleUseItem(itempos, target, result);
	}
	return result;
}

void CBattleAction_PlayerUseItem::setItemName(const QString &szText)
{
	m_ItemId = 0;
	m_ItemType = 0;
	if (szText[0] == '#')
	{
		bool bValue = false;
		int value = szText.mid(1).toInt(&bValue);
		if (bValue)
			m_ItemId = value;
	}
	else if (szText[0] == '@')
	{
		bool bValue = false;
		int value = szText.mid(1).toInt(&bValue);
		if (bValue)
			m_ItemType = value;
	}
	else
	{
		//	m_ItemName = szText;
		if (szText.size() < 250)
		{
			strcpy(m_ItemName, szText.toStdString().c_str());
		}
	}
}

void CBattleAction_PlayerUseItem::SetActionTextData(QStringList data)
{
	if (data.size() > 0)
	{
		//m_ItemName = data[0];
		if (data[0].size() < 250)
		{
			strcpy(m_ItemName, data[0].toStdString().c_str());
		}
	}
}

void CBattleAction_PlayerUseItem::SetActionData(QList<int> data)
{
	if (data.size() > 1)
	{
		m_ItemId = data[0];
		m_ItemType = data[1];
	}
}

bool CBattleAction_PlayerUseItem::GetItemPosition(CGA_BattleContext_t &context, int &itempos)
{
	if (context.m_Items.empty())
		return false;
	QString sItemName(m_ItemName);
	for (int i = 0; i < context.m_Items.size(); ++i)
	{
		auto item = context.m_Items.at(i);
		if (item->exist == false)
		{
			continue;
		}
		if (!sItemName.isEmpty() && item->name == sItemName && item->assessed)
		{
			itempos = item->pos;
			return true;
		}
		if (m_ItemId > 0 && item->id == m_ItemId && item->assessed)
		{
			itempos = item->pos;
			return true;
		}
		if (m_ItemType > 0 && item->type == m_ItemType && item->assessed)
		{
			itempos = item->pos;
			return true;
		}
	}

	return false;
}

CBattleAction_PlayerLogBack::CBattleAction_PlayerLogBack()
{
	m_nDevType = dtAction_PlayerLogBack;
}

void CBattleAction_PlayerLogBack::GetActionName(QString &str, bool config)
{
	str = QObject::tr("LogBack");
}

bool CBattleAction_PlayerLogBack::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	//qDebug() << "PlayerLogBack:";
	return g_CGAInterface->LogBack();
}

CBattleAction_PlayerRebirth::CBattleAction_PlayerRebirth()
{
	m_nDevType = dtAction_PlayerRebirth;
}

void CBattleAction_PlayerRebirth::GetActionName(QString &str, bool config)
{
	str = QObject::tr("Rebirth");
}

bool CBattleAction_PlayerRebirth::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	bool result = false;
	//qDebug() << "PlayerRebirth:";

	if (g_CGAInterface->BattleRebirth(result))
		return result;

	return false;
}

CBattleAction_PlayerDoNothing::CBattleAction_PlayerDoNothing()
{
	m_nDevType = dtAction_PlayerDoNothing;
}

void CBattleAction_PlayerDoNothing::GetActionName(QString &str, bool config)
{
	str = QObject::tr("Do Nothing");
}

bool CBattleAction_PlayerDoNothing::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	bool result = false;
	//qDebug() << "PlayerDoNothing:";

	if (g_CGAInterface->BattleDoNothing(result))
		return result;

	return false;
}

CBattleAction_PetSkillAttack::CBattleAction_PetSkillAttack(QString &skillName)
{
	//m_SkillName = skillName;
	SetSkillName(skillName);
	m_nDevType = dtAction_PetSkillAttack;
}

CBattleAction_PetSkillAttack::CBattleAction_PetSkillAttack()
{
	m_nDevType = dtAction_PetSkillAttack;
}

void CBattleAction_PetSkillAttack::GetActionName(QString &str, bool config)
{
	str = m_SkillName;
}

int CBattleAction_PetSkillAttack::GetTargetFlags(CGA_BattleContext_t &context)
{
	if (context.m_iPetId == -1)
		return 0;

	if (context.m_Pets.empty())
		return 0;

	for (int i = 0; i < context.m_Pets.size(); ++i)
	{
		auto pet = context.m_Pets.at(i);
		if (pet->id == context.m_iPetId)
		{
			GameSkillList petskills = pet->skills;

			for (int j = 0; j < petskills.size(); ++j)
			{
				if (petskills.at(j)->name == m_SkillName)
				{
					return petskills.at(j)->flags;
				}
			}
			break;
		}
	}

	return 0;
}

void CBattleAction_PetSkillAttack::SetSkillName(const QString &szName)
{
	if (szName.size() < 250)
	{
		strcpy(m_SkillName, szName.toStdString().c_str());
	}
}

void CBattleAction_PetSkillAttack::SetActionTextData(QStringList data)
{
	if (data.size() > 0)
	{
		//m_SkillName = data[0];
		SetSkillName(data[0]);
	}
}

bool CBattleAction_PetSkillAttack::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	int skillpos = -1;

	bool result = false;
	bool bUseDefaultTarget = false;

	auto donothing = QObject::tr("Do Nothing");
	if (donothing == m_SkillName)
	{
		//qDebug() << "PetSkillAttack: Do Find Skill name " << m_SkillName;
		g_CGAInterface->BattleDoNothing(result);
		return result;
	}

	if (GetSkill(context, skillpos, bUseDefaultTarget))
	{
		//qDebug("target1=%d", target);
		target = bUseDefaultTarget ? defaultTarget : target;
		//qDebug("target2=%d", target);
		FixTarget(context, skillpos, target);
		//qDebug("target3=%d", target);
		//人物不逃跑 宠物勾选二动  则进行二动 如果抓宠并且有1级敌人 则不二动
		if (context.m_bIsPetDoubleAction && !context.m_bIsPlayerEscaped &&
				!(context.m_bIsHaveLv1Enemy && context.m_bIsSealLv1Enemy))
		{
			//qDebug() << "二动" << skillpos << target << m_SkillName << true;
			g_CGAInterface->BattlePetSkillAttack(skillpos, target, true, result);
			g_CGAInterface->BattlePetSkillAttack(skillpos, target, true, result);
		}
		else
		{
			//qDebug() << "普通" << skillpos << target << m_SkillName << true;
			//	qDebug() << skillpos << target << m_SkillName << false;
			g_CGAInterface->BattlePetSkillAttack(skillpos, target, false, result);
		}
	}
	//qDebug("BattlePetSkillAttack %d %d", skillpos, target);
	return result;
}

bool CBattleAction_PetSkillAttack::GetSkill(CGA_BattleContext_t &context, int &skillpos, bool &bUseDefaultTarget)
{
	if (context.m_iPetId == -1)
		return false;

	if (context.m_Pets.empty())
		return false;
	if (context.m_iPetPosition < 0 || context.m_iPetPosition >= 20)
		return false;
	const unsigned char szAttack[] = { 0xB9, 0xA5, 0xBB, 0xF7, 0 };
	QString attack = QString::fromLocal8Bit((const char *)szAttack);

	const unsigned char szGuard[] = { 0xB7, 0xC0, 0xD3, 0xF9, 0 };
	QString guard = QString::fromLocal8Bit((const char *)szGuard);

	for (int i = 0; i < context.m_Pets.size(); ++i)
	{
		auto pet = context.m_Pets.at(i);
		if (pet->id == context.m_iPetId)
		{
			GameSkillList petskills = pet->skills;

			for (int j = 0; j < petskills.size(); ++j)
			{
				if (
						petskills.at(j)->name == m_SkillName &&
						context.m_UnitGroup[context.m_iPetPosition]->mp >= petskills.at(j)->cost &&
						(context.m_iPetSkillAllowBit & (1 << petskills.at(j)->index)))
				{
					skillpos = petskills.at(j)->index;
					return true;
				}
			}

			for (int j = 0; j < petskills.size(); ++j)
			{
				if (
						petskills.at(j)->name == attack &&
						(context.m_iPetSkillAllowBit & (1 << petskills.at(j)->index)))
				{

					//qDebug("find attack");
					skillpos = petskills.at(j)->index;
					bUseDefaultTarget = true;
					return true;
				}
			}

			for (int j = 0; j < petskills.size(); ++j)
			{
				if (
						petskills.at(j)->name == guard &&
						(context.m_iPetSkillAllowBit & (1 << petskills.at(j)->index)))
				{

					//qDebug("find guard");
					skillpos = petskills.at(j)->index;
					bUseDefaultTarget = true;
					return true;
				}
			}

			break;
		}
	}

	return false;
}

void CBattleAction_PetSkillAttack::FixTarget(CGA_BattleContext_t &context, int skillpos, int &target)
{
	if (context.m_Pets.empty())
		return;

	for (int i = 0; i < context.m_Pets.size(); ++i)
	{
		auto pet = context.m_Pets.at(i);
		if (pet->id == context.m_iPetId)
		{
			GameSkillList petskills = pet->skills;
			for (int j = 0; j < petskills.size(); ++j)
			{
				if (petskills.at(j)->index == skillpos)
				{
					int flags = petskills.at(j)->flags;

					if (!(flags & FL_SKILL_SELECT_TARGET))
					{
						target = context.m_iPetPosition;
						return;
					}
					if (flags & FL_SKILL_SINGLE)
						return;
					if (flags & FL_SKILL_MULTI)
					{
						//qDebug("multi skill");
						target = target + 20;
						return;
					}
					if (flags & FL_SKILL_ALL)
					{
						target = (target >= 10 && target <= 19) ? 41 : 40;
						return;
					}
					if (flags & FL_SKILL_BOOM)
					{
						target = 42;
						return;
					}
					target = 255;
					return;
				}
			}
		}
	}
}

CBattleAction_PetDoNothing::CBattleAction_PetDoNothing()
{
	m_nDevType = dtAction_PetDoNothing;
}

void CBattleAction_PetDoNothing::GetActionName(QString &str, bool config)
{
	str = QObject::tr("Do Nothing");
}

bool CBattleAction_PetDoNothing::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
	bool result = false;
	//qDebug() << "PetDoNothing:";
	if (g_CGAInterface->BattleDoNothing(result))
		return result;
	return false;
}
CBattleTarget_Self::CBattleTarget_Self()
{
	m_nDevType = dtTarget_Self;
}

void CBattleTarget_Self::GetTargetName(QString &str)
{
	str += g_pAutoBattleCtrl->GetDevTypeText(m_nDevType);
}

int CBattleTarget_Self::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
	return context.m_iPlayerPosition;
}

void CBattleTarget_Self::SetTargetVal(int val)
{
}

CBattleTarget_Pet::CBattleTarget_Pet()
{
	m_nDevType = dtTarget_Pet;
}

void CBattleTarget_Pet::GetTargetName(QString &str)
{
	str += g_pAutoBattleCtrl->GetDevTypeText(m_nDevType);
}

int CBattleTarget_Pet::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
	return context.m_iPetPosition;
}

void CBattleTarget_Pet::SetTargetVal(int val)
{
}

CBattleTarget_Condition::CBattleTarget_Condition()
{
	m_nDevType = dtTarget_Condition;
}

void CBattleTarget_Condition::GetTargetName(QString &str)
{
	str += g_pAutoBattleCtrl->GetDevTypeText(m_nDevType);
}

int CBattleTarget_Condition::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
	return context.m_iConditionTarget;
}

CBattleTarget_Enemy::CBattleTarget_Enemy(int select, QString special)
{
	m_Select = select;
	//m_specialName = special;
	SetTargetSpecialName(special);
	m_nDevType = dtTarget_Enemy;
}

CBattleTarget_Enemy::CBattleTarget_Enemy()
{
	m_nDevType = dtTarget_Enemy;
}

void CBattleTarget_Enemy::GetTargetName(QString &str)
{
	str += g_pAutoBattleCtrl->GetDevTypeText(m_nDevType);
	str += QLatin1String(", ");
	str += g_pAutoBattleCtrl->GetDevTypeText(m_Select);
}

bool compareFront(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->pos > b->pos;
}

bool compareBack(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->pos < b->pos;
}

bool compareLowHP(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->hp < b->hp;
}

bool compareHighHP(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->hp > b->hp;
}

bool compareLowHPPercent(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return (float)a->hp / (float)a->maxhp < (float)b->hp / (float)b->maxhp;
}

bool compareHighHPPercent(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->hp > b->hp;
}

bool compareLowLv(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->level < b->level;
}

bool compareHighLv(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->level > b->level;
}

bool compareSingleDebuff(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->flags > b->flags;
}

bool compareMultiDebuff(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->debuff > b->debuff;
}

bool compareMultiMagic(GameBattleUnitPtr a, GameBattleUnitPtr &b)
{
	return a->multi_hp > b->multi_hp;
}

int CBattleTarget_Enemy::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
	if (unitpos < 0 || unitpos >= 20)
		return -1;
	GameBattleUnitList newGroup;
	if (context.m_iPlayerPosition >= 0xA)
	{
		for (int i = 0; i < 20; ++i)
		{
			auto unit = context.m_UnitGroup[i];
			if (unit->exist && unit->pos < 0xA)
			{

				if ((flags & FL_SKILL_FRONT_ONLY) && unit->isback && context.m_UnitGroup[unitpos]->isback &&
						((unitpos == context.m_iPlayerPosition && (context.m_iWeaponAllowBit & 0x80)) || (unitpos == context.m_iPetPosition)))
				{
					continue;
				}

				newGroup.append(unit);
			}
		}
	}
	else
	{
		for (int i = 0; i < 20; ++i)
		{
			auto unit = context.m_UnitGroup[i];
			if (unit->exist && unit->pos >= 0xA)
			{

				//qDebug("flags %X isback %d unitpos %d", flags, unit.isback ? 1 : 0, unit.pos);
				if ((flags & FL_SKILL_FRONT_ONLY) && unit->isback && context.m_UnitGroup[unitpos]->isback &&
						((unitpos == context.m_iPlayerPosition && (context.m_iWeaponAllowBit & 0x80)) || (unitpos == context.m_iPetPosition)))
				{
					//qDebug("u%d ignored", unit.pos);
					continue;
				}

				newGroup.append(unit);
			}
		}
	}
	if (!newGroup.size())
		return -1;
	QString specialName(m_specialName);
	if (!specialName.isEmpty())
	{
		for (auto tmpUnit : newGroup)
		{
			if (tmpUnit->name == specialName)
			{
				return tmpUnit->pos; //直接返回 不接受下面规则 没找到才接受下面规则
			}
		}
	}
	switch (m_Select)
	{
		case dtTargetCondition_Random:
			return newGroup[qrand() % newGroup.size()]->pos;
		case dtTargetCondition_Front:
			std::sort(newGroup.begin(), newGroup.end(), compareFront);
			break;
		case dtTargetCondition_Back:
			std::sort(newGroup.begin(), newGroup.end(), compareBack);
			break;
		case dtTargetCondition_LowHP:
			std::sort(newGroup.begin(), newGroup.end(), compareLowHP);
			break;
		case dtTargetCondition_HighHP:
			std::sort(newGroup.begin(), newGroup.end(), compareHighHP);
			break;
		case dtTargetCondition_LowHPPercent:
			std::sort(newGroup.begin(), newGroup.end(), compareLowHPPercent);
			break;
		case dtTargetCondition_HighHPPercent:
			std::sort(newGroup.begin(), newGroup.end(), compareHighHPPercent);
			break;
		case dtTargetCondition_LowLv:
			std::sort(newGroup.begin(), newGroup.end(), compareLowLv);
			break;
		case dtTargetCondition_HighLv:
			std::sort(newGroup.begin(), newGroup.end(), compareHighLv);
			break;
		case dtTargetCondition_Goatfarm:
		{
			const char cname_a[] = { -69, -16, -47, -26, -59, -93, -51, -73, -71, -42 };
			QString name_a = QString::fromLocal8Bit(cname_a);
			if (context.m_UnitGroup[0x10]->exist && context.m_UnitGroup[0x10]->name == name_a)
			{
				return context.m_UnitGroup[0x10]->pos;
			}
			else if (context.m_UnitGroup[0x11]->exist && context.m_UnitGroup[0x11]->name == name_a)
			{
				return context.m_UnitGroup[0x11]->pos;
			}
			else
			{
				const char cname_b[] = { -59, -93, -51, -73, -71, -42 };
				QString name_b = QString::fromLocal8Bit(cname_b);
				if (context.m_UnitGroup[0x10]->exist && context.m_UnitGroup[0x10]->name == name_b)
					return context.m_UnitGroup[0x10]->pos;
				else if (context.m_UnitGroup[0x11]->exist && context.m_UnitGroup[0x11]->name == name_b)
					return context.m_UnitGroup[0x11]->pos;
			}
			return newGroup[qrand() % newGroup.size()]->pos;
		}
		case dtTargetCondition_Boomerang:
			if (context.m_iFrontCount > context.m_iBackCount)
				std::sort(newGroup.begin(), newGroup.end(), compareFront);
			else
				std::sort(newGroup.begin(), newGroup.end(), compareBack);
			break;
		case dtTargetCondition_LessUnitRow:
			if (context.m_iBackCount > context.m_iFrontCount)
				std::sort(newGroup.begin(), newGroup.end(), compareFront);
			else
				std::sort(newGroup.begin(), newGroup.end(), compareBack);
			break;
		case dtTargetCondition_MultiMagic:
			std::sort(newGroup.begin(), newGroup.end(), compareMultiMagic);
			break;
		case dtTargetCondition_AimOrderUpDown:
		{
			//从前往后 从上到下
			QList<int> upDown;
			if (context.m_iPlayerPosition >= 0xA)
			{
				upDown << 8 << 6 << 5 << 7 << 9;
				upDown << 3 << 1 << 0 << 2 << 4;
			}
			else
			{
				upDown << 18 << 16 << 15 << 17 << 19;
				upDown << 13 << 11 << 10 << 12 << 14;
			}
			for (auto tgtPos : upDown)
			{
				for (auto unit : newGroup)
				{
					if (tgtPos == unit->pos)
						return unit->pos;
				}
			}
			break;
		}
		case dtTargetCondition_AimOrderDownUp:
		{
			//从前往后 从上到下
			QList<int> orderCounts;
			if (context.m_iPlayerPosition >= 0xA)
			{
				orderCounts << 9 << 7 << 5 << 6 << 8;
				orderCounts << 4 << 2 << 0 << 1 << 3;
			}
			else
			{
				orderCounts << 19 << 17 << 15 << 16 << 18;
				orderCounts << 14 << 12 << 10 << 11 << 13;
			}
			for (auto tgtPos : orderCounts)
			{
				for (auto unit : newGroup)
				{
					if (tgtPos == unit->pos)
						return unit->pos;
				}
			}
			break;
		}
		case dtTargetCondition_AimOrderCenter:
		{
			std::sort(newGroup.begin(), newGroup.end(), compareFront);
			break;
		}
	}

	return newGroup[0]->pos;
}

void CBattleTarget_Enemy::SetTargetVal(int val)
{
	m_Select = val;
}

void CBattleTarget_Enemy::SetTargetSpecialName(QString name)
{
	if (name.size() < 250)
	{
		strcpy(m_specialName, name.toStdString().c_str());
	}
}

CBattleTarget_Teammate::CBattleTarget_Teammate(int select)
{
	m_Select = select;
	m_nDevType = dtTarget_Teammate;
}

CBattleTarget_Teammate::CBattleTarget_Teammate()
{
	m_nDevType = dtTarget_Teammate;
}

void CBattleTarget_Teammate::GetTargetName(QString &str)
{
	str += g_pAutoBattleCtrl->GetDevTypeText(m_nDevType);
	str += QLatin1String(", ");
	str += g_pAutoBattleCtrl->GetDevTypeText(m_Select);
}

int CBattleTarget_Teammate::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
	GameBattleUnitList newGroup;

	if (context.m_iPlayerPosition >= 0xA)
	{
		for (int i = 0; i < 20; ++i)
		{
			auto unit = context.m_UnitGroup[i];
			if (unit->exist && unit->pos >= 0xA)
				newGroup.append(unit);
		}
	}
	else
	{
		for (int i = 0; i < 20; ++i)
		{
			auto unit = context.m_UnitGroup[i];
			if (unit->exist && unit->pos < 0xA)
				newGroup.append(unit);
		}
	}

	if (!newGroup.size())
		return -1;

	switch (m_Select)
	{
		case dtTargetCondition_Random:
			return newGroup[qrand() % newGroup.size()]->pos;
		case dtTargetCondition_Front:
			std::sort(newGroup.begin(), newGroup.end(), compareFront);
			break;
		case dtTargetCondition_Back:
			std::sort(newGroup.begin(), newGroup.end(), compareBack);
			break;
		case dtTargetCondition_LowHP:
			std::sort(newGroup.begin(), newGroup.end(), compareLowHP);
			break;
		case dtTargetCondition_HighHP:
			std::sort(newGroup.begin(), newGroup.end(), compareHighHP);
			break;
		case dtTargetCondition_LowHPPercent:
			std::sort(newGroup.begin(), newGroup.end(), compareLowHPPercent);
			break;
		case dtTargetCondition_HighHPPercent:
			std::sort(newGroup.begin(), newGroup.end(), compareHighHPPercent);
			break;
		case dtTargetCondition_LowLv:
			std::sort(newGroup.begin(), newGroup.end(), compareLowLv);
			break;
		case dtTargetCondition_HighLv:
			std::sort(newGroup.begin(), newGroup.end(), compareHighLv);
			break;
		case dtTargetCondition_SingleDebuff:
			std::sort(newGroup.begin(), newGroup.end(), compareSingleDebuff);
			break;
		case dtTargetCondition_MulTDebuff:
			std::sort(newGroup.begin(), newGroup.end(), compareMultiDebuff);
			break;
	}

	return newGroup[0]->pos;
}

void CBattleTarget_Teammate::SetTargetVal(int val)
{
	m_Select = val;
}

CBattleSetting::CBattleSetting(CBattleCondition *cond, CBattleCondition *cond2,
		CBattleAction *playerAction, CBattleTarget *playerTarget,
		CBattleAction *petAction, CBattleTarget *petTarget /*,
    CBattleAction *petAction2, CBattleTarget *petTarget2*/
)
{
	m_condition = cond;
	m_condition2 = cond2;
	m_playerAction = playerAction;
	m_playerTarget = playerTarget;
	m_petAction = petAction;
	m_petTarget = petTarget;
	/*m_petAction2 = petAction2;
    m_petTarget2 = petTarget2;*/
	m_defaultTarget = new CBattleTarget_Enemy(dtTargetCondition_Random);
}

CBattleSetting::~CBattleSetting()
{
	SafeDelete(m_condition);
	SafeDelete(m_condition2);
	SafeDelete(m_playerAction);
	SafeDelete(m_playerTarget);
	SafeDelete(m_petAction);
	SafeDelete(m_petTarget);
	SafeDelete(m_defaultTarget);
}

bool CBattleSetting::DoAction(CGA_BattleContext_t &context)
{
	int conditionTarget = -1, condition2Target = -1;

	//qDebug("checking condition %d %d", GetConditionTypeId(), GetCondition2TypeId());

	if (!m_condition && !m_condition2)
		return false;

	if (m_condition && !m_condition->Check(context, conditionTarget))
		return false;

	//qDebug("checking condition 1 pass");

	if (m_condition2 && !m_condition2->Check(context, condition2Target))
		return false;

	//qDebug("checking condition 2 pass");

	if (conditionTarget != -1)
	{
		context.m_iConditionTarget = conditionTarget;
		//qDebug("m_iConditionTarget = %d", conditionTarget);
	}
	else if (condition2Target != -1)
	{
		context.m_iConditionTarget = condition2Target;
		//qDebug("m_iConditionTarget = %d", condition2Target);
	}
	else
	{
		context.m_iConditionTarget = -1;
	}

	bool bHasPet = context.m_iPetPosition >= 0 && context.m_iPetId != -1;
	bool bIsPetAction = (context.m_iPlayerStatus == 4) ? true : false;

	bool bRet = false;
	if (!bIsPetAction)
	{
		//二动时候 会优先发送什么也不做，加上逃跑判断
		if (m_playerAction && context.m_bIsPetDoubleAction && bHasPet && m_playerAction->GetDevType() != dtAction_PlayerEscape && !(context.m_bIsHaveLv1Enemy && context.m_bIsSealLv1Enemy))
		{
			//自定义二动还是有问题 先屏蔽
			//自定义宠物二动技能 第一攻 默认就使用设置的
			if (/*g_pAutoBattleCtrl->m_bPetDoubleActionDefault1 && */m_petAction && m_petAction->GetDevType() != dtAction_PetDoNothing)
			{
				int flags = 0;
				int target = -1;
				int defaultTarget = -1;
				int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

				//    qDebug("m_petAction");

				flags = m_petAction->GetTargetFlags(context);

				if (m_petTarget)
					target = m_petTarget->GetTarget(context.m_iPetPosition, flags, context);

				if (m_defaultTarget)
					defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

				if (m_petAction->DoAction(target, defaultTarget, context))
				{
					//      qDebug("m_petAction ok");
				}
			}
			////第一攻不使用默认 看是否设置了技能
			//else if (!g_pAutoBattleCtrl->m_bPetDoubleActionDefault1 && g_pAutoBattleCtrl->m_petDoubleAction1 )
			//{
			//	CBattleAction *customPetAction = g_pAutoBattleCtrl->m_petDoubleAction1->m_petAction;
			//	if (customPetAction && customPetAction->GetDevType() != dtAction_PetDoNothing)
			//	{
			//		int flags = 0;
			//		int target = -1;
			//		int defaultTarget = -1;
			//		int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;
			//		qDebug("宠二动第一攻自定义");
			//		flags = customPetAction->GetTargetFlags(context);
			//		if (g_pAutoBattleCtrl->m_petDoubleAction1->m_petTarget)
			//			target = g_pAutoBattleCtrl->m_petDoubleAction1->m_petTarget->GetTarget(context.m_iPetPosition, flags, context);

			//		if (m_defaultTarget)
			//			defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

			//		if (customPetAction->DoAction(target, defaultTarget, context))
			//		{
			//			qDebug("宠二动第一攻自定义Ok");
			//			//      qDebug("m_petAction ok");
			//		}
			//	}
			//}
			//////检查是否二动 并且是否自定义了宠物二动 第二攻
			//////宠二动 第二攻
			//if (g_pAutoBattleCtrl->m_bPetDoubleActionDefault2 && m_petAction && m_petAction->GetDevType() != dtAction_PetDoNothing)
			//{
			//	int flags = 0;
			//	int target = -1;
			//	int defaultTarget = -1;
			//	int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

			//	qDebug("宠二动第二攻默认");

			//	flags = m_petAction->GetTargetFlags(context);

			//	if (m_petTarget)
			//		target = m_petTarget->GetTarget(context.m_iPetPosition, flags, context);

			//	if (m_defaultTarget)
			//		defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

			//	if (m_petAction->DoAction(target, defaultTarget, context))
			//	{
			//		qDebug("宠二动第二攻默认Ok");					
			//	}
			//}
			//else if (!g_pAutoBattleCtrl->m_bPetDoubleActionDefault2 && g_pAutoBattleCtrl->m_petDoubleAction2)
			//{
			//	CBattleAction *customPetAction = g_pAutoBattleCtrl->m_petDoubleAction2->m_petAction;
			//	if (customPetAction)
			//	{
			//		int flags = 0;
			//		int target = -1;
			//		int defaultTarget = -1;
			//		int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;
			//		qDebug("宠二动第二攻自定义");
			//		flags = customPetAction->GetTargetFlags(context);
			//		if (g_pAutoBattleCtrl->m_petDoubleAction2->m_petTarget)
			//			target = g_pAutoBattleCtrl->m_petDoubleAction2->m_petTarget->GetTarget(context.m_iPetPosition, flags, context);

			//		if (m_defaultTarget)
			//			defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

			//		if (customPetAction->DoAction(target, defaultTarget, context))
			//		{
			//			qDebug("宠二动第二攻自定义Ok");
			//		}
			//	}
			//}
			//////宠二动 第二攻结束 上述正常 不会进入下面，否则进入
		}
		//人物技能
		if (m_playerAction)
		{
			int flags = 0;
			int target = -1;
			int defaultTarget = -1;
			int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

			flags = m_playerAction->GetTargetFlags(context);

			if (m_playerTarget)
				target = m_playerTarget->GetTarget(context.m_iPlayerPosition, flags, context);

			if (m_defaultTarget)
				defaultTarget = m_defaultTarget->GetTarget(context.m_iPlayerPosition, defaultFlags, context);

			//qDebug("m_playerAction DoAction");

			if (m_playerAction->DoAction(target, defaultTarget, context))
			{
				context.m_bIsPlayerActionPerformed = true;
				bRet = true;
				return true;
			}
			else
			{
				bool result = false;
				if (g_CGAInterface->BattleDoNothing(result) && result)
				{
					context.m_bIsPlayerActionPerformed = true;
					bRet = true;
					return true;
				}
			}
			qDebug("m_playerAction failed to DoAction");
			//context.m_bIsPetDoubleAction = false;

			//if (context.m_bIsPetDoubleAction && bHasPet && m_playerAction->GetDevType() != dtAction_PlayerEscape && !(context.m_bIsHaveLv1Enemy && context.m_bIsSealLv1Enemy))
			//{
			//	////检查是否二动 并且是否自定义了宠物二动 第二攻
			//	////宠二动 第二攻
			//	if (g_pAutoBattleCtrl->m_bPetDoubleActionDefault2 && m_petAction && m_petAction->GetDevType() != dtAction_PetDoNothing)
			//	{
			//		int flags = 0;
			//		int target = -1;
			//		int defaultTarget = -1;
			//		int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

			//		qDebug("宠二动第二攻默认");

			//		flags = m_petAction->GetTargetFlags(context);

			//		if (m_petTarget)
			//			target = m_petTarget->GetTarget(context.m_iPetPosition, flags, context);

			//		if (m_defaultTarget)
			//			defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

			//		if (m_petAction->DoAction(target, defaultTarget, context))
			//		{
			//			qDebug("宠二动第二攻默认Ok");
			//		}
			//	}
			//	else if (!g_pAutoBattleCtrl->m_bPetDoubleActionDefault2 && g_pAutoBattleCtrl->m_petDoubleAction2)
			//	{
			//		CBattleAction *customPetAction = g_pAutoBattleCtrl->m_petDoubleAction2->m_petAction;
			//		if (customPetAction)
			//		{
			//			int flags = 0;
			//			int target = -1;
			//			int defaultTarget = -1;
			//			int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;
			//			qDebug("宠二动第二攻自定义");
			//			flags = customPetAction->GetTargetFlags(context);
			//			if (g_pAutoBattleCtrl->m_petDoubleAction2->m_petTarget)
			//				target = g_pAutoBattleCtrl->m_petDoubleAction2->m_petTarget->GetTarget(context.m_iPetPosition, flags, context);

			//			if (m_defaultTarget)
			//				defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

			//			if (customPetAction->DoAction(target, defaultTarget, context))
			//			{
			//				qDebug("宠二动第二攻自定义Ok");
			//			}
			//		}
			//	}
				////宠二动 第二攻结束 上述正常 不会进入下面，否则进入
			//}
		
		}
		else
		{
			qDebug("m_playerAction not present, try next");
			return false;
		}
	}

	if (bIsPetAction && m_petAction)
	{
		//qDebug("宠物回合");
		if (context.m_bIsPlayerForceAction && m_playerAction && !context.m_bIsPlayerActionPerformed)
		{
			int flags = 0;
			int target = -1;
			int defaultTarget = -1;
			int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

			flags = m_playerAction->GetTargetFlags(context);

			if (m_playerTarget)
				target = m_playerTarget->GetTarget(context.m_iPlayerPosition, flags, context);

			if (m_defaultTarget)
				defaultTarget = m_defaultTarget->GetTarget(context.m_iPlayerPosition, defaultFlags, context);

			//qDebug("m_playerAction DoAction");

			if (m_playerAction->DoAction(target, defaultTarget, context))
			{
				context.m_bIsPlayerActionPerformed = true;
			}
			else
			{
				bool result = false;
				if (g_CGAInterface->BattleDoNothing(result) && result)
				{
					context.m_bIsPlayerActionPerformed = true;
				}
			}
		}

		
		int flags = 0;
		int target = -1;
		int defaultTarget = -1;
		int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;
		context.m_bIsPetDoubleAction = false;
		flags = m_petAction->GetTargetFlags(context);
		if (m_petTarget)
			target = m_petTarget->GetTarget(context.m_iPetPosition, flags, context);
		if (m_defaultTarget)
			defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);
		if (m_petAction->DoAction(target, defaultTarget, context))
		{
			return true;
		}
		else
		{
			bool result = false;
			if (g_CGAInterface->BattleDoNothing(result) && result)
				return true;
		}
		
	
	}
	return bRet;
	qDebug("action failed");
	return false;
}

void CBattleSetting::GetPlayerActionName(QString &str, bool config)
{
	if (m_playerAction)
	{
		m_playerAction->GetActionName(str, config);
	}
}

void CBattleSetting::GetPlayerTargetName(QString &str)
{
	if (m_playerTarget)
		m_playerTarget->GetTargetName(str);
}

void CBattleSetting::GetPetActionName(QString &str, bool config)
{
	if (m_petAction)
		m_petAction->GetActionName(str, config);
}

void CBattleSetting::GetPetTargetName(QString &str)
{
	if (m_petTarget)
		m_petTarget->GetTargetName(str);
}

/*void CBattleSetting::GetPetAction2Name(QString &str)
{
    if(m_petAction2)
        m_petAction2->GetActionName(str);
}

void CBattleSetting::GetPetTarget2Name(QString &str)
{
    if(m_petTarget2)
        m_petTarget2->GetTargetName(str);
}*/

void CBattleSetting::GetConditionName(QString &str)
{
	if (m_condition)
		m_condition->GetConditionName(str);
	/*if (m_condition)
		str = g_pAutoBattleCtrl->GetBattleTypeText(m_condition->GetDevType());*/
}

int CBattleSetting::GetConditionTypeId()
{
	if (m_condition)
		return m_condition->GetConditionTypeId();
	/*if (m_condition)
		return m_condition->GetDevType();*/

	return -1;
}

int CBattleSetting::GetConditionRelId()
{
	if (m_condition)
		return m_condition->GetConditionRelId();

	return -1;
}

void CBattleSetting::GetConditionValue(QString &str)
{
	if (m_condition)
		m_condition->GetConditionValue(str);
}

void CBattleSetting::GetCondition2Name(QString &str)
{
	if (m_condition2)
		m_condition2->GetConditionName(str);
	/*if (m_condition2)
		str = g_pAutoBattleCtrl->GetBattleTypeText(m_condition2->GetDevType());*/
}

int CBattleSetting::GetCondition2TypeId()
{
	if (m_condition2)
		return m_condition2->GetConditionTypeId();

	return -1;
}

int CBattleSetting::GetCondition2RelId()
{
	if (m_condition2)
		return m_condition2->GetConditionRelId();

	return -1;
}

void CBattleSetting::GetCondition2Value(QString &str)
{
	if (m_condition2)
		m_condition2->GetConditionValue(str);
}

int CBattleSetting::GetPlayerActionTypeId()
{
	if (m_playerAction)
		return m_playerAction->GetActionTypeId();
	return -1;
}

QString CBattleSetting::GetPlayerSkillName()
{
	if (m_playerAction && m_playerAction->GetActionTypeId() == dtAction_PlayerSkillAttack)
	{
		auto sk = dynamic_cast<CBattleAction_PlayerSkillAttack *>(m_playerAction);
		return sk->GetSkillName();
	}
	return QString();
}

int CBattleSetting::GetPlayerSkillLevel()
{
	if (m_playerAction && m_playerAction->GetActionTypeId() == dtAction_PlayerSkillAttack)
	{
		auto sk = dynamic_cast<CBattleAction_PlayerSkillAttack *>(m_playerAction);
		return sk->GetSkillLevel();
	}
	return -1;
}

int CBattleSetting::GetPlayerTargetTypeId()
{
	if (m_playerTarget)
		return m_playerTarget->GetTargetTypeId();
	return -1;
}

int CBattleSetting::GetPlayerTargetSelectId()
{
	if (m_playerTarget)
		return m_playerTarget->GetTargetSelectId();
	return -1;
}

int CBattleSetting::GetPetActionTypeId()
{
	if (m_petAction)
		return m_petAction->GetActionTypeId();
	return -1;
}

QString CBattleSetting::GetPetSkillName()
{
	if (m_petAction && m_petAction->GetActionTypeId() == dtAction_PetSkillAttack)
	{
		auto sk = dynamic_cast<CBattleAction_PetSkillAttack *>(m_petAction);
		return sk->GetSkillName();
	}
	return QString();
}

int CBattleSetting::GetPetTargetTypeId()
{
	if (m_petTarget)
		return m_petTarget->GetTargetTypeId();
	return -1;
}

int CBattleSetting::GetPetTargetSelectId()
{
	if (m_petTarget)
		return m_petTarget->GetTargetSelectId();
	return -1;
}

/*int CBattleSetting::GetPetAction2TypeId()
{
    if(m_petAction2)
        return m_petAction2->GetActionTypeId();
    return -1;
}

QString CBattleSetting::GetPetSkill2Name()
{
    if(m_petAction && m_petAction->GetActionTypeId() == dtAction_PetSkillAttack)
    {
        auto sk = dynamic_cast<CBattleAction_PetSkillAttack *>(m_petAction2);
        return sk->GetSkillName();
    }
    return QString();
}

int CBattleSetting::GetPetTarget2TypeId()
{
    if(m_petTarget2)
        return m_petTarget2->GetTargetTypeId();
    return -1;
}

int CBattleSetting::GetPetTarget2SelectId()
{
    if(m_petTarget2)
        return m_petTarget2->GetTargetSelectId();
    return -1;
}*/

bool CBattleSetting::HasPlayerAction()
{
	return (m_playerAction) ? true : false;
}

bool CBattleSetting::HasPetAction()
{
	return (m_petAction) ? true : false;
}

CBattleCondition::CBattleCondition()
{
	m_nDevType = dtCondition;
}

CBattleAction::CBattleAction()
{
	m_nDevType = dtAction;
}

CBattleTarget::CBattleTarget()
{
	m_nDevType = dtTarget;
}

CBattleCondition_Compare::CBattleCondition_Compare()
{
	m_nDevType = dtCondition_Compare;
}

CBattleCondition_Compare::CBattleCondition_Compare(int relation, int value)
{
	m_nDevType = dtCondition_Compare;
}

void CBattleCondition_Compare::SetConditionData(QList<int> data)
{
	if (data.size() > 2)
	{
		m_relation = data[0];
		m_value = data[1];
		m_percentage = data[2];
	}
}

CBattleCondition_EnemyLv1Hp::CBattleCondition_EnemyLv1Hp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_EnemyLv1Hp;
}

CBattleCondition_EnemyLv1Hp::CBattleCondition_EnemyLv1Hp()
{
	m_nDevType = dtCondition_EnemyLv1Hp;
}

void CBattleCondition_EnemyLv1Hp::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy HP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyLv1Hp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	int curHp = 0, maxv = 0;
	for (int i = 0xA; i < 20; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		if (context.m_UnitGroup[i]->level == 1)
		{
			curHp = context.m_UnitGroup[i]->hp;
			break;
			;
		}
	}
	//1级 不设百分比
	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			if (curHp >= m_value)
			{
				return true;
			}
			break;
		case dtCompare_GreaterThan:
			if (curHp > m_value)
			{
				return true;
			}
			break;
		case dtCompare_LessEqual:
			if (curHp <= m_value)
			{
				return true;
			}
			break;
		case dtCompare_LessThan:
			if (curHp < m_value)
			{
				return true;
			}
			break;
		case dtCompare_Equal:
			if (curHp == m_value)
			{
				return true;
			}
			break;
		case dtCompare_NotEqual:
			if (curHp != m_value)
			{
				return true;
			}
			break;
	}

	return false;
}

CBattleCondition_EnemyLv1MaxHp::CBattleCondition_EnemyLv1MaxHp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_EnemyLv1MaxHp;
}

CBattleCondition_EnemyLv1MaxHp::CBattleCondition_EnemyLv1MaxHp()
{
	m_nDevType = dtCondition_EnemyLv1MaxHp;
}

void CBattleCondition_EnemyLv1MaxHp::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy HP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyLv1MaxHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	int curHp = 0, maxv = 0;
	for (int i = 0xA; i < 20; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		if (context.m_UnitGroup[i]->level == 1)
		{
			curHp = context.m_UnitGroup[i]->maxhp;
			break;
			;
		}
	}
	//1级 不设百分比
	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			if (curHp >= m_value)
			{
				return true;
			}
			break;
		case dtCompare_GreaterThan:
			if (curHp > m_value)
			{
				return true;
			}
			break;
		case dtCompare_LessEqual:
			if (curHp <= m_value)
			{
				return true;
			}
			break;
		case dtCompare_LessThan:
			if (curHp < m_value)
			{
				return true;
			}
			break;
		case dtCompare_Equal:
			if (curHp == m_value)
			{
				return true;
			}
			break;
		case dtCompare_NotEqual:
			if (curHp != m_value)
			{
				return true;
			}
			break;
	}

	return false;
}

CBattleCondition_EnemyLv1MaxMp::CBattleCondition_EnemyLv1MaxMp(int relation, int value, bool percentage)
{
	m_relation = relation;
	m_value = value;
	m_percentage = percentage;
	m_nDevType = dtCondition_EnemyLv1MaxMp;
}

CBattleCondition_EnemyLv1MaxMp::CBattleCondition_EnemyLv1MaxMp()
{
	m_nDevType = dtCondition_EnemyLv1MaxMp;
}

void CBattleCondition_EnemyLv1MaxMp::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy HP %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyLv1MaxMp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	int curHp = 0, maxv = 0;
	for (int i = 0xA; i < 20; ++i)
	{
		if (!context.m_UnitGroup[i]->exist)
			continue;
		if (context.m_UnitGroup[i]->level == 1)
		{
			curHp = context.m_UnitGroup[i]->maxmp;
			break;
			;
		}
	}
	//1级 不设百分比
	switch (m_relation)
	{
		case dtCompare_GreaterEqual:
			if (curHp >= m_value)
			{
				return true;
			}
			break;
		case dtCompare_GreaterThan:
			if (curHp > m_value)
			{
				return true;
			}
			break;
		case dtCompare_LessEqual:
			if (curHp <= m_value)
			{
				return true;
			}
			break;
		case dtCompare_LessThan:
			if (curHp < m_value)
			{
				return true;
			}
			break;
		case dtCompare_Equal:
			if (curHp == m_value)
			{
				return true;
			}
			break;
		case dtCompare_NotEqual:
			if (curHp != m_value)
			{
				return true;
			}
			break;
	}

	return false;
}

CBattleCondition_EnemyType::CBattleCondition_EnemyType(int relation, int value)
{
	m_relation = relation;
	m_value = value;
	m_nDevType = dtCondition_EnemyType;
}

CBattleCondition_EnemyType::CBattleCondition_EnemyType()
{
	m_nDevType = dtCondition_EnemyType;
}

void CBattleCondition_EnemyType::GetConditionName(QString &str)
{
	str = QObject::tr("Enemy Type %1%2").arg(g_pAutoBattleCtrl->GetDevTypeText(m_relation), QString::number(m_value));
}

bool CBattleCondition_EnemyType::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
	int bgm = 0;
	bool isBoss = false;
	if (g_CGAInterface->GetBGMIndex(bgm) && bgm == 14)
	{
		isBoss = true;
	}
	//qDebug() << "bgm" << bgm;	//没有声卡，获取到的bgm是-1 不能用此判断
	switch (m_relation)
	{
		case dtCompare_Equal:
			if (isBoss == (bool)m_value)
			{
				return true;
			}
			break;
		case dtCompare_NotEqual:
			if (isBoss != (bool)m_value)
			{
				return true;
			}
			break;
	}

	return false;
}
