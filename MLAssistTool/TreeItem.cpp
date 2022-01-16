#include <QStringList>
#include "treeitem.h"


TreeItem::TreeItem(const QString& text)
{
	parentItem = NULL;
	setData(Qt::DisplayRole, text);
}

TreeItem::~TreeItem()
{
	removeAllChild();
}

void TreeItem::appendChild(TreeItem* item)
{
	childItems.append(item);
	item->parentItem = this;
}

void TreeItem::removeAllChild()
{
	qDeleteAll(childItems);
	childItems.clear();
	parentItem = NULL;
}
TreeItem* TreeItem::child(int row)
{
	return childItems.value(row);
}

int TreeItem::childCount() const
{
	return childItems.count();
}

// int TreeItem::columnCount() const
// {
//     return itemData.count();
// }

TreeItem* TreeItem::parent()
{
	return parentItem;
}

int TreeItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

void TreeItem::setData(int role, const QVariant& value)
{
	bool found = false;
	role = (role == Qt::EditRole ? Qt::DisplayRole : role);
	for (int i = 0; i < m_userValues.count(); ++i) {
		if (m_userValues.at(i).role == role) {
			if (m_userValues[i].value == value)
				return;

			m_userValues[i].value = value;
			found = true;
			break;
		}
	}
	if (!found)
		m_userValues.append(UserItemData(role, value));
}
QVariant TreeItem::data(int role) const
{
	role = (role == Qt::EditRole ? Qt::DisplayRole : role);
	for (const auto& value : m_userValues) {
		if (value.role == role)
			return value.value;
	}
	return QVariant();
}