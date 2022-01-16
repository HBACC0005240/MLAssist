//#pragma once
//#include <QList>
//#include <QSharedPointer>
//class GraphNode;
//typedef QSharedPointer<GraphNode> GraphNodePtr;
//typedef QList<GraphNodePtr> GraphNodeList;
//
//class GraphNode
//{
//public:
//	int id=0;			//地图id
//	int num = 0;				 //保存与该顶点相邻的顶点个数
//	QList<int> nei;	 //与该顶点相邻的顶点序号
//	QList<int> flag; //与该顶点相邻的顶点是否访问过
//	bool isIn=false;		//该顶点是否入栈
//
//
//	GraphNodePtr parent = nullptr;
//
//	GraphNode(int id, GraphNodePtr parent)
//	{
//		this->id = id;
//		this->parent = parent;
//	}
//	int getOne()
//	{
//		int i = 0;
//		for (i = 0; i < num; i++)
//		{
//			if (flag[i] == 0) //判断是否访问过
//			{
//				flag[i] = 1; //表示这个顶点已经被访问，并将其返回
//				return nei[i];
//			}
//		}
//		return -1; //所有顶点都已访问过则返回-1
//	}
//	void resetFlag()
//	{
//		for (int i = 0; i < num; i++)
//		{
//			flag[i] = 0;
//		}
//	}
//};
//
//
//class ConnectGraphSearch
//{
//public:
//	ConnectGraphSearch(QHash<int, QList<int> > warpHash);
//	~ConnectGraphSearch();
//	int GetWeight(int i, int j);
//	int GetVertices();
//	GraphNodePtr GetValue(int i);
//	GraphNodePtr FindTarget(int startID, int targetID, QHash<int, QList<int> > warpHash);
//
//	QHash<int, GraphNodePtr> m_mapIndexForNode;
//};
