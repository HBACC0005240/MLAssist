#include <QDateTime>
#include "MLWndTableModel.h"
#include "YunLai.h"

MLWndTableModel::MLWndTableModel(QObject* parent) : QAbstractTableModel(parent)
{
	m_HeaderString.append(tr("编号"));
	m_HeaderString.append(tr("进程ID"));
	m_HeaderString.append(tr("窗口标题"));
	m_HeaderString.append(tr("状态"));
	m_HeaderString.append(tr("可见"));
}

void MLWndTableModel::appendRow(const FzTableItemPtr& item)
{
	int row = rowCount();

	beginInsertRows(QModelIndex(), row, row);

	m_List.append(item);

	endInsertRows();
}

bool MLWndTableModel::removeRow(int position, const QModelIndex& parent)
{
	bool success = false;

	if (position >= 0 && position < m_List.size())
	{
		beginRemoveRows(parent, position, position);
		m_List.erase(m_List.begin() + position);
		success = true;
		endRemoveRows();
	}

	return success;
}

bool MLWndTableModel::removeRows(int position, int count, const QModelIndex& parent)
{
	bool success = false;

	if (count > 0 && position >= 0 && position + count - 1 < m_List.size())
	{
		beginRemoveRows(parent, position, position + count - 1);
		m_List.erase(m_List.begin() + position, m_List.begin() + position + count);
		success = true;
		endRemoveRows();
	}

	return success;
}

Qt::ItemFlags MLWndTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return 0;

	return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

int MLWndTableModel::rowCount(const QModelIndex& parent) const
{
	return (int)m_List.size();
}

int MLWndTableModel::columnCount(const QModelIndex& parent) const
{
	return m_HeaderString.size();
}

QModelIndex MLWndTableModel::index(int row, int column, const QModelIndex& parent) const
{
	if (row < 0 || column < 0 || row >= m_List.size())
		return QModelIndex();

	return createIndex(row, column, (void*)m_List[row].data());
}

QModelIndex MLWndTableModel::parent(const QModelIndex& index) const
{
	return QModelIndex();
}

FzTableItem* MLWndTableModel::ItemFromIndex(const QModelIndex& index) const
{
	if (index.isValid()) {
		return static_cast<FzTableItem*>(index.internalPointer());
	}
	else {
		return NULL;
	}
}

QVariant MLWndTableModel::data(const QModelIndex& index, int role) const
{
	const FzTableItem* modelItem = ItemFromIndex(index);
	if (modelItem)
	{
		if (role == Qt::DisplayRole)
		{
			switch (index.column())
			{
			case 0: return QString::number(index.row() + 1);
			case 1: return QString::number(modelItem->m_ProcessId);
			case 2: {
				return modelItem->m_sTitle;
			}case 3: {
				if (modelItem->m_nState==-1)
				{
					return "未响应";
				}else if (modelItem->m_nState == 0)
				{
					return "离线";
				}else if (modelItem->m_nState == 1)
				{
					return "正常";
				}
				return "";
			}case 4: {
				if (modelItem->m_visible & WS_MINIMIZE && modelItem->m_visible & WS_VISIBLE)
				{
					return "最小化";
				}
				else if (!(modelItem->m_visible & WS_VISIBLE))
				{
					return "隐藏";
				}
				else
				{
					return "正常";
				}
			}
			default:break;
			}
		}
		else if (role == Qt::DecorationRole)
		{
			//if(index.column() == 1)
			//    return *ev->GetUniqueProcess()->m_Icon;
		}
		else if (role == Qt::TextAlignmentRole)
		{
			return int(Qt::AlignLeft | Qt::AlignVCenter);
		}
	}
	return QVariant();
}

QVariant MLWndTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section >= 0 && section < m_HeaderString.size())
		return m_HeaderString[section];

	return QVariant();
}
