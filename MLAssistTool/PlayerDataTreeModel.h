#pragma once
#include "treemodel.h"
#include "ITObject.h"

class PlayerDataTreeModel : public TreeModel
{
	Q_OBJECT
public:
	PlayerDataTreeModel(QObject* parent = 0);
	virtual QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	void refreshData(QModelIndex& topLeft, QModelIndex& bottomRight);

	TreeItem* CreateTreeItem(TreeItem* parentItem, ITObjectPtr pObj);
	void SetupModelData(ITObjectList pObjList, TreeItem *treeItem);
	void SetupModelSubData(ITObjectPtr pObj, TreeItem* pItem);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	QStringList mimeTypes() const;
	bool SortStringFun(const QString &s1, const QString &s2);

	void SetFilterData(const QString& sData) { m_filterData = sData; }
signals:
	void resetModelSignal();
public:
	QString			m_strDevName;
	QString m_filterData;
};

