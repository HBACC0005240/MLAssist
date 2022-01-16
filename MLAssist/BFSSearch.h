#pragma once

#include <QList>
#include <QSharedPointer>

class BFSNode;
typedef QSharedPointer<BFSNode> BFSNodePtr;
typedef QList<BFSNodePtr> BFSNodeList;

class BFSNode
{
public:
	int id; //地图id

	BFSNodePtr parent = nullptr;

	BFSNode(int id, BFSNodePtr parent)
	{
		this->id = id;
		this->parent = parent;
	}
};

class BFSSearch
{
public:
	static BFSNodePtr FindTarget(int startID, int targetID, QHash<int, QList<int> > warpHash);
	static void PrintSearPath(BFSNodePtr target);
};
