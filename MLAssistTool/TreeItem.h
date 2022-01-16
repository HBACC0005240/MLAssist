#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QDataStream>
class UserItemData
{
public:
	inline UserItemData() : role(-1) {}
	inline UserItemData(int r, const QVariant& v) : role(r), value(v) {}
	int role;
	QVariant value;
	inline bool operator==(const UserItemData& other) const { return role == other.role && value == other.value; }
};
Q_DECLARE_TYPEINFO(UserItemData, Q_MOVABLE_TYPE);
inline QDataStream& operator>>(QDataStream& in, UserItemData& data)
{
	in >> data.role;
	in >> data.value;
	return in;
}

inline QDataStream& operator<<(QDataStream& out, const UserItemData& data)
{
	out << data.role;
	out << data.value;
	return out;
}
class TreeItem
{
public:
	TreeItem(const QString& text);
	~TreeItem();

	void appendChild(TreeItem* child);
	void removeAllChild();
	void setData(int role, const QVariant& value);
	QVariant data(int role) const;
	void setAllChildItems(QList<TreeItem*> childList) { childItems = childList; }
	void setParentItem(TreeItem* parentitem) { parentItem = parentitem; }
	TreeItem* getParentItem() { return parentItem; }
	TreeItem* child(int row);
	int childCount() const;
	//int columnCount() const;
	int row() const;
	TreeItem* parent();
	//	TreeItem* FindItem(INT64 DevID);

	QList<TreeItem*> childItems;
private:
	TreeItem* parentItem;
	QVector<UserItemData> m_userValues;

};
#endif
