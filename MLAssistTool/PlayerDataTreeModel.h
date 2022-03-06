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

	void SetupModelData(ITObjectList pObjList);
	void SetupModelSubData(ITObjectPtr pObj, TreeItem* pItem);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	QStringList mimeTypes() const;
signals:
	void resetModelSignal();
public:
	QString			m_strDevName;
};

