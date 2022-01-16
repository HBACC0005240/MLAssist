#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#pragma once

//Event table

#include <QAbstractTableModel>
#include <QList>
#include "toolDef.h"
#include <QTableView>

class FzTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	//impl
	explicit FzTableModel(QObject* parent = Q_NULLPTR);
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	//util
	FzTableItem* ItemFromIndex(const QModelIndex& index) const;
	void appendRow(const FzTableItemPtr& item);
	bool removeRow(int position, const QModelIndex& parent = QModelIndex());
	bool removeRows(int position, int count, const QModelIndex& parent = QModelIndex());
	FzTableItemList GetFzTableItemList() { return m_List; }
private:
	FzTableItemList m_List;
	QStringList m_HeaderString;
	QTableView* m_linkView = nullptr;
};

#endif // PROCESSTABLE_H
