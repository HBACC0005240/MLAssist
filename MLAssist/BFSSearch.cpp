#include "BFSSearch.h"
#include <QDebug>

BFSNodePtr BFSSearch::FindTarget(int startID, int targetID, QHash<int, QList<int> > warpHash)
{
	QList<int> hasSearchList;
	BFSNodeList queue;

	BFSNodePtr startNode(new BFSNode(startID, nullptr));

	queue.push_back(startNode);
	while (!queue.isEmpty())
	{
		BFSNodePtr node = queue.front();
		queue.pop_front();

		if (hasSearchList.contains(node->id))
		{
			continue;
		}
		qDebug() << QString("判断节点:%1").arg(node->id);
		if (targetID == node->id)
		{
			return node;
		}
		hasSearchList.append(node->id);

		auto warpTgtList = warpHash.value(node->id);
		if (warpTgtList.length() > 0)
		{
			for (auto childId : warpTgtList)
			{
				queue.append(BFSNodePtr(new BFSNode(childId, node)));
			}
		}
	}
	return nullptr;
}

void BFSSearch::PrintSearPath(BFSNodePtr target)
{
	if (target != nullptr)
	{
		qDebug() << QString("找到了目标节点:%1 ").arg(target->id);

		QList<BFSNodePtr> searchPath;
		searchPath.append(target);

		BFSNodePtr node = target->parent;
		while (node != nullptr)
		{
			searchPath.append(node);
			node = node->parent;
		}
		QString path = "";
		for (int i = searchPath.size() - 1; i >= 0; i--)
		{
			path += QString::number(searchPath.at(i)->id);
			if (i != 0)
			{
				path += "-->";
			}
		}
		qDebug() << QString("步数最短：%1").arg(path);
	}
	else
	{
		qDebug() << ("未找到目标节点");
	}
}
