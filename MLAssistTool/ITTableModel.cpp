#include "ITTableModel.h"

ITTableModel::ITTableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

ITTableModel::~ITTableModel()
{
}

void ITTableModel::setHorizontalHeader(const QStringList& headers)
{
	m_sHorizontalHeader = headers;
}

int ITTableModel::columnCount(const QModelIndex& parent) const
{
	return m_sHorizontalHeader.size();
//	return m_columnCnt;
}

void ITTableModel::setTableRowDataIds(QStringList sIDs)
{
	m_sTableDataIds = sIDs;
	//emit dataChanged();
}

void ITTableModel::updateTable()
{
	beginResetModel();
	endResetModel();
}

Qt::ItemFlags ITTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return 0;

	if (index.column() == 1)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ITTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (m_sHorizontalHeader.size() > section)
			return m_sHorizontalHeader.at(section);
	}

	return QVariant();
}

QVariant ITTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();
	int ncol = index.column();
	int nrow = index.row();
	//if (nrow >= m_sTableDataIds.size())
	if (nrow >= m_pCharacterList.size())
	{
		return QVariant();
	}
	auto pRole = m_pCharacterList[nrow].dynamicCast<ITGameCharacter>();
	if (role == Qt::DisplayRole) 
	{	
		switch (ncol) 
		{
		case 0: return nrow+1;
		case 1: return pRole->getObjectName();							//名称
		case 2: return pRole->_baseData->_level;						//等级
		case 3: return pRole->_gold;									//金币
		case 4: return pRole->_map_name;								//地图
		case 5: return QString("%1,%2").arg(pRole->_x).arg(pRole->_y);	//坐标
		case 6: return pRole->_connectState ? "在线" : "离线";			//连接状态
		case 7: return QString::number(pRole->_server_line);					//线路
		case 8: return ITObjectDataMgr::getInstance().GetGameServerTypeText(pRole->_big_line); //大区
		default:break;
		}
	}
	else if (role == Qt::BackgroundRole)
	{
		switch (ncol)
		{
			//case 1:
				//return QColor(255, 0, 0); //名称
			//case 2: return pRole->_level;								   //等级
			//case 3: return pRole->_gold;								   //金币
			//case 4: return pRole->_map_name;							   //地图
			//case 5: return QString("%1,%2").arg(pRole->_x).arg(pRole->_y); //坐标
			//case 6: return pRole->_connectState ? QColor(0, 255, 0) : QColor(255,0, 0);
			default: break;
		}
	}
	else if (role == Qt::TextColorRole)
	{
		switch (ncol)
		{	
			//case 2: return pRole->_level;								   //等级
			//case 3: return pRole->_gold;								   //金币
			//case 4: return pRole->_map_name;							   //地图
			//case 5: return QString("%1,%2").arg(pRole->_x).arg(pRole->_y); //坐标
			case 6: return pRole->_connectState ? QColor(0, 255, 0) : QColor(255, 0, 0);
			default: break;
		}
	}
	else if (role == Qt::TextAlignmentRole)
	{
		return Qt::AlignCenter;
	}
	return QVariant();
}

QModelIndex ITTableModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	QModelIndex newIndex = createIndex(row, column);
	return newIndex;
}


int ITTableModel::rowCount(const QModelIndex& parent) const
{
	//return m_sTableDataIds.size();
	return m_pCharacterList.size();
	//	return ITObjectDataMgr::getInstance().GetAlreadyConnectedData().size();
}
