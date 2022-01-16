#pragma once

#include "TreeModel.h"
#include "ITObject.h"

class ITAccountTreeModel : public TreeModel
{
	Q_OBJECT

public:
	ITAccountTreeModel(QObject* parent = 0);
	ITAccountTreeModel(ITAccountPtr pDevRoot, int nAlnyType, QObject* parent = 0);
	~ITAccountTreeModel();
	virtual QVariant data(const QModelIndex& index, int role) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role /* = Qt::EditRole */);
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	void refreshData(QModelIndex& topLeft, QModelIndex& bottomRight);
	void SetupModelData(ITObjectPtr pDevice, TreeItem* Item);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	QStringList mimeTypes() const;

	void setAllCheckState(int flag);
	Qt::DropActions supportedDropActions() const;
signals:
	void resetModelSignal();
private:
	ITAccountPtr m_pDevRoot;
	QString			m_strDevName;
	//ITDeviceList	m_pSceneList;
	QList<QPersistentModelIndex> m_checkedItemList;
	int m_nArmyType;		//显示部队类型
};
