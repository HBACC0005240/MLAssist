#include "ITAccountTreeModel.h"
#include "treeitem.h"
#include <QIcon>
#include <QMimeData>
ITAccountTreeModel::ITAccountTreeModel(QObject* parent)
	: TreeModel(parent)
{
	m_columnCnt = 1;
	m_rootItem = new TreeItem(NULL);
	/*TreeItem* tempOwnItem = new TreeItem(NULL);
	m_rootItem->appendChild(tempOwnItem);*/
	//ITDeviceList pDevList = ITDeviceDataMgr::getInstance().getAllLibrarys();
	//foreach(ITDevice * pDev, pDevList)
	//{
	//	if (!pDev)
	//		continue;
	//	if (GETDEVCLASS(pDev->getDeviceType()) == dtEquip_Radar)
	//	{
	//		SetupModelData(pDev, m_rootItem);
	//	}
	//}
	//	SetupModelData((ITTask*)m_pDevRoot, tempitem);
}
ITAccountTreeModel::ITAccountTreeModel(ITAccountPtr pDevRoot, int almyType, QObject* parent)
	: TreeModel(parent)
{
	m_columnCnt = 1;
	m_pDevRoot = pDevRoot;				//根节点
	m_nArmyType = almyType;
	m_rootItem = new TreeItem("账号");
	TreeItem* tempitem = new TreeItem(pDevRoot->getObjectName());
	m_rootItem->appendChild(tempitem);
	SetupModelData(m_pDevRoot.dynamicCast<ITObject>(), tempitem);
}
ITAccountTreeModel::~ITAccountTreeModel()
{
}
QVariant ITAccountTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();
	TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
	switch (role)
	{
	case Qt::DisplayRole:
	{
		QString tempdevname = item->data(Qt::DisplayRole).toString();
		return tempdevname;
	}
	break;
	//case Qt::ForegroundRole:
	//	{
	//	
	//	}
	//case Qt::CheckStateRole:
	//{
	//	if (index.column() == 0) //判断显示的对象是checkbox，并且位于第一列  
	//	{
	//		if (m_checkedItemList.contains(index))    //在m_checkedList中查找，如果有，显示checkbox被选中  
	//			return Qt::Checked;
	//		else
	//			return Qt::Unchecked;             //如果没有显示checkbox没被选中  
	//	}
	//}
	case Qt::DecorationRole:
	{
		QIcon tempicon;
		/*if (GETDEVCLASS(itemData->getDeviceType()) == dtTask)
		{
			tempicon = QIcon(":/RadarCtrlSysMain/Resources/depart.png");
		}
		else if (itemData->getDeviceType() == dtArmy)
		{
			tempicon = QIcon(":/RadarCtrlSysMain/Resources/area.png");
		}
		else if (itemData->getDeviceType() == dtEquip)
		{
			tempicon = QIcon(":/RadarCtrlSysMain/Resources/interval.ico");
		}
		else if (itemData->getDeviceType() == dtEquip_Airplane)
		{
			tempicon = QIcon(":/RadarCtrlSysMain/Resources/equipment.png");
		}
		else if (itemData->getDeviceType() == dtEquip_Radar)
		{
			tempicon = QIcon(":/MITSCameraTool/Resources/scene.png");
		}*/
		return tempicon;
	}
	break;
	}
	return QVariant();
}

QVariant ITAccountTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section == 0)
		{
			/*ITDevice* itemData = (ITDevice*)m_rootItem->data();
			if(itemData)
				return itemData->getDeviceName();*/
			return  QVariant("账号信息");
		}
	}
	return QVariant();
}

void ITAccountTreeModel::SetupModelData(ITObjectPtr pDevice, TreeItem* Item)
{
	if (pDevice == NULL)
		return;
	int devtype = pDevice->getObjectType();
	if (GETDEVCLASS(devtype) == TObject_Account)
	{
		TreeItem* tempitem = new TreeItem(pDevice->getObjectName());
		Item->appendChild(tempitem);
	}
}
bool ITAccountTreeModel::setData(const QModelIndex& index, const QVariant& value, int role /* = Qt::EditRole */)
{
	if (role == Qt::CheckStateRole /*&& index.column()==0*/)
	{
		if (value == Qt::Unchecked)
		{
			if (m_checkedItemList.contains(index))
				m_checkedItemList.removeOne(index);
			emit(dataChanged(index, index));
		}
		else if (value == Qt::Checked)
		{
			if (!m_checkedItemList.contains(index))
				m_checkedItemList.append(index);
			emit(dataChanged(index, index));
		}
		int childCount = rowCount(index);
		if (childCount > 0)                    //判断是否有子节点  
		{
			for (int i = 0; i < childCount; i++)
			{
				QModelIndex child = this->index(i, 0, index); //获得子节点的index  
				setData(child, value, Qt::CheckStateRole);    //递归，将子节点的checkbox设为选中状态  
			}
			return true;
		}
	}
	return false;
}
Qt::ItemFlags ITAccountTreeModel::flags(const QModelIndex& index)const
{
	if (!index.isValid())
		return 0;
	Qt::ItemFlags defaultFlags = TreeModel::flags(index);
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable*/ | defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}


QMimeData* ITAccountTreeModel::mimeData(const QModelIndexList& indexes) const		//点击的Qmodel节点
{
	QMimeData* mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	foreach(QModelIndex index, indexes)
	{
		if (index.isValid())
		{
			QString text = data(index, Qt::DisplayRole).toString();
			qint64 addr = (qint64)index.internalPointer();
			stream << addr;
		}
	}
	mimeData->setData("text/plain", encodedData);
	return mimeData;
}
QStringList ITAccountTreeModel::mimeTypes() const
{
	QStringList types;
	types << "text/plain";
	return types;
}
Qt::DropActions ITAccountTreeModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}
bool ITAccountTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	if (action == Qt::IgnoreAction)
		return true;
	if (!parent.isValid())
		return false;
	if (!data->hasFormat("text/plain"))
		return false;
	if (data->hasFormat("application/x-fridgemagnet"))
		return false;
	/*QByteArray encodedData = data->data("text/plain");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QHash<qint64, QMap<int, QHash<int, QString> > > newItems;
	while (!stream.atEnd())
	{
		qint64 addr;
		stream >> addr;
		TreeItem* destinationItem = static_cast<TreeItem*>(parent.internalPointer());
		ITDevice* destinationDev = ITDeviceDataMgr::getInstance().findDevice(destinationItem->userData());
		TreeItem* sourceItem = (TreeItem*)addr;
		ITDevice* sourceDev = ITDeviceDataMgr::getInstance().findDevice(sourceItem->userData());
		int ordinal = sourceDev->getOrdinal();
		if (destinationDev->getDeviceType() == dtArmy && GETDEVSUPPERCLASS(sourceDev->getDeviceType()) == dtEquip)
		{
			if (sourceDev->getDeviceOwner() && sourceDev->getDeviceOwner()->getDeviceType() == dtArmy)
			{
				((ITArmy*)sourceDev->getDeviceOwner())->removeEquip((ITEquip*)sourceDev);
				sourceDev->setDeviceOwner(NULL);
			}
			else if (sourceDev->getDeviceOwner() && sourceDev->getDeviceOwner()->getDeviceType() == dtTask)
			{
				((ITTask*)sourceDev->getDeviceOwner())->removeOneEquip((ITEquip*)sourceDev);
				sourceDev->setDeviceOwner(NULL);
			}
			((ITArmy*)destinationDev)->addChildEquip((ITEquip*)sourceDev);
			((ITEquip*)sourceDev)->setDeviceOwner(destinationDev);
			ITDeviceDataMgr::getInstance().setDevEdit(sourceDev);
			ITDeviceDataMgr::getInstance().setDevEdit(destinationDev);
			TreeItem* tempitem = new TreeItem(sourceDev->getDeviceCode());
			destinationItem->appendChild(tempitem);
			emit resetModelSignal();
		}
	}*/
	return false;
}

