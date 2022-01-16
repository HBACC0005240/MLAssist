#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <map>
#include <QStringList>
#include "TreeItem.h"
using namespace std;

class TreeItem;
class ITDevice;

//树形模型的基类
//派生类须实现以下内容
//   1. 实现data()函数，用于显示每个item的名称和图标
//   2. 初始化时指定m_columnCnt，确定列的数量
//   3. 如果有header，初始化时要设置每一列的headerTitle，写入m_headerData
class TreeModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	TreeModel(QObject* parent = 0);
	~TreeModel();

	virtual QVariant data(const QModelIndex& index, int role) const = 0;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
		const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	TreeItem* m_rootItem;			//树形的根节点，不在view里显示
protected:
	int		m_columnCnt;
	QStringList	m_headerData;
};
#endif
