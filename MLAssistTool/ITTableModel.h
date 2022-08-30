#pragma once

#include <QAbstractTableModel>
#include "ITObjectDataMgr.h"

class ITTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	ITTableModel(QObject* parent = 0);
	~ITTableModel();

	//设置头数据
	void setHorizontalHeader(const QStringList& headers);

	//头数据
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	//单元数据
	QVariant data(const QModelIndex& index, int role) const;

	//单元标记
	Qt::ItemFlags flags(const QModelIndex& index) const;
	
	//数据索引
	QModelIndex index(int row, int column,const QModelIndex& parent = QModelIndex()) const;

	//行总数
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	//列总数
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	//设置表格数据
	void setTableRowDataIds(QStringList sIDs);

	//刷新
	void updateTable();
protected :
	QStringList	m_sHorizontalHeader;		//头数据
	QStringList m_sTableDataIds;			//行数据id索引
};

