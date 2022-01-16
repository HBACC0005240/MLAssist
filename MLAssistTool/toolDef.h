#pragma once

#include <qglobal.h>
#include <QString>
#include <QSharedPointer>
#include <QList>

class FzTableItem
{
public:
	FzTableItem(quint32 pid, quint32 tid, quint32 hWnd, const QString& szTitle, int state, long visible)
	{
		m_ProcessId = pid;
		m_ThreadId = tid;
		m_hWnd = hWnd;
		m_sTitle = szTitle; //QString::fromWCharArray(szTitle);
		m_nState = state;
		m_visible = visible;
	}
	quint32 m_ProcessId;
	quint32 m_ThreadId;
	quint32 m_hWnd;
	QString m_sTitle;
	int m_nState;//状态 -1未响应 0 正常 
	long m_visible;
	QString m_gameState;	//状态
};

typedef QSharedPointer<FzTableItem> FzTableItemPtr;
typedef QList<FzTableItemPtr> FzTableItemList;