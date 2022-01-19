#include "PlayerDataTreeModel.h"
#include "treeitem.h"
#include <QMimeData>
#include "ITObjectDataMgr.h"
PlayerDataTreeModel::PlayerDataTreeModel(QObject* parent)
	: TreeModel(parent)
{
	m_columnCnt = 1;
	m_rootItem = new TreeItem("根节点");
	SetupModelData();
}

QVariant PlayerDataTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();
	TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
	auto id = item->data(Qt::UserRole).toULongLong();
	auto itemData = ITObjectDataMgr::getInstance().FindObject(id);
	if (!itemData)
		return QVariant();
	switch (role)
	{
	case Qt::DisplayRole:
	{
		QString tempdevname = QString(itemData->getObjectName());
		return tempdevname;
	}
	break;
	//case Qt::ForegroundRole:
	//	{
	//	
	//	}
	case Qt::DecorationRole:
	{
		/*QIcon tempicon = MITSDeviceDataMgr::getMITSDeviceDataInstance().getDevIconByDevType(itemData->getDeviceType());
		return tempicon;*/
		//		
	}
	break;
	}
	return QVariant();
}

QVariant PlayerDataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section == 0)
		{
			/*INMSDevice* itemData = (INMSDevice*)m_rootItem->data();
			return itemData->getDeviceName();*/
			return m_rootItem->data(Qt::UserRole);
		}
	}
	return QVariant();
}

void PlayerDataTreeModel::SetupModelData()
{
	auto pObjList = ITObjectDataMgr::getInstance().GetDstObjTypeList(TObject_GidRole);
	for (ITObjectPtr tObj : pObjList)
	{
		TreeItem* tempitem = new TreeItem(tObj->getObjectName());
		tempitem->setData(Qt::UserRole, tObj->getObjectID());
		m_rootItem->appendChild(tempitem);
		//auto pAccountAsse = qSharedPointerDynamicCast<ITGidRole>(tObj);
		//auto pAccountList = pAccountAsse->GetAllChildObj();
		//for (auto pAccount : pAccountList)
		//{
		//	TreeItem* subitem = new TreeItem(pAccount->getObjectName());
		//	subitem->setData(Qt::UserRole, pAccount->getObjectID());

		//	tempitem->appendChild(subitem);
		//	//SetupModelSubData(pAccount, subitem);
		//}
	}

}


void PlayerDataTreeModel::SetupModelSubData(ITObjectPtr pObj, TreeItem* pItem)
{
	if (!pObj || !pItem)
		return;
	int devtype = pObj->getObjectType();
	if (devtype == TObject_Account)
	{
		ITAccountPtr pAccount = qSharedPointerDynamicCast<ITAccount>(pObj);
		auto pGidList = pAccount->GetAllChildObj();
		//		qSort(childdepts.begin(), childdepts.end(), sortQListDepts);		//越小越在前面
		for (int i = 0; i < pGidList.size(); i++)
		{
			auto pGidObj = pGidList[i];
			TreeItem* tempitem = new TreeItem(pGidObj->getObjectName());
			tempitem->setData(Qt::UserRole, pGidObj->getObjectID());

			pItem->appendChild(tempitem);
			SetupModelSubData(pGidObj, tempitem);
		}
	}
	else if (devtype == TObject_AccountGid)
	{
		ITAccountGidPtr pAccountGid = qSharedPointerDynamicCast<ITAccountGid>(pObj);
		auto pRoleList = pAccountGid->GetAllChildObj();
		//		qSort(childdepts.begin(), childdepts.end(), sortQListDepts);		//越小越在前面
		for (int i = 0; i < pRoleList.size(); i++)
		{
			auto pRoleObj = pRoleList[i];
			TreeItem* tempitem = new TreeItem(pRoleObj->getObjectName());
			tempitem->setData(Qt::UserRole, pRoleObj->getObjectID());

			pItem->appendChild(tempitem);
			//		SetupModelData(pRoleObj, tempitem);
		}
	}
}

bool PlayerDataTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	if (action == Qt::IgnoreAction)
		return true;
	if (!parent.isValid())
		return false;
	if (!data->hasFormat("text/plain"))
		return false;
	if (data->hasFormat("application/x-fridgemagnet"))
		return false;
	QByteArray encodedData = data->data("text/plain");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QHash<qint64, QMap<int, QHash<int, QString> > > newItems;
	while (!stream.atEnd())
	{
		int addr;
		stream >> addr;
		/*TreeItem* destinationItem = static_cast<TreeItem*>(parent.internalPointer());
		INMSDevice* destinationDev = (INMSDevice*)destinationItem->data();
		TreeItem* sourceItem = (TreeItem*)addr;
		INMSDevice* sourceDev = (INMSDevice*)sourceItem->data();
		int ordinal = sourceDev->getOrdinal();
		if (destinationItem->getParentItem() == sourceItem->getParentItem())
		{
			sourceDev->setOrdial(destinationDev->getOrdinal());
			destinationDev->setOrdial(ordinal);
			MITSDeviceDataMgr::getMITSDeviceDataInstance().setDevEdit(destinationDev);
			MITSDeviceDataMgr::getMITSDeviceDataInstance().setDevEdit(sourceDev);
			emit resetModelSignal();
		}*/
	}
	return true;
}

QMimeData* PlayerDataTreeModel::mimeData(const QModelIndexList& indexes) const		//点击的Qmodel节点
{
	QMimeData* mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	foreach(QModelIndex index, indexes)
	{
		if (index.isValid())
		{
			QString text = data(index, Qt::DisplayRole).toString();
			int addr = (int)index.internalPointer();
			stream << addr;
		}
	}
	mimeData->setData("text/plain", encodedData);
	return mimeData;
}
Qt::ItemFlags PlayerDataTreeModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags defaultFlags = TreeModel::flags(index);

	if (index.isValid())
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	else
		return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList PlayerDataTreeModel::mimeTypes() const
{
	QStringList types;
	types << "text/plain";
	return types;
}








