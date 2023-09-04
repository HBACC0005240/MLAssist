#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(QObject* parent)
	: QAbstractItemModel(parent)
{
}

TreeModel::~TreeModel()
{
	delete m_rootItem;
}

int TreeModel::columnCount(const QModelIndex& parent) const
{
	return m_columnCnt;
}

TreeItem *TreeModel::root()
{
	return m_rootItem;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return 0;

	if (index.column() == 1)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (m_headerData.size() > section)
			return m_headerData.at(section);
	}

	return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem* parentItem;
	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem* childItem = parentItem->child(row);
	if (childItem)
	{
		QModelIndex newIndex = createIndex(row, column, childItem);
		return newIndex;
	}
	else
		return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
	TreeItem* parentItem = childItem->parent();

	if (parentItem == m_rootItem)
		return QModelIndex();
	if (!parentItem)
		return QModelIndex();
	return createIndex(parentItem->row(), 0, parentItem);
}


int TreeModel::rowCount(const QModelIndex& parent) const
{
	TreeItem* parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}
