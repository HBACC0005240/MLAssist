//#include "ConnectGraphSearch.h"
//
// ConnectGraphSearch::ConnectGraphSearch(QHash<int, QList<int> > warpHash)
//{
//	 for (auto it=warpHash.begin();it!=warpHash.end();++it)
//	 {
//		 GraphNodePtr tmpNode(new GraphNode(it.key(), nullptr));
//		 tmpNode->nei = it.value();
//		 tmpNode->flag.reserve(tmpNode->nei.size());
//		 tmpNode->num = tmpNode->nei.size();
//		 m_mapIndexForNode.insert(it.key(),tmpNode);
//	 }
//
//}
//
//GraphNodePtr ConnectGraphSearch::FindTarget(int startID, int targetID, QHash<int, QList<int> > warpHash)
//{
//	QList<int> hasSearchList;
//	GraphNodeList queue;
//
//	GraphNodePtr startNode = m_mapIndexForNode.value(startID);
//	queue.push_back(startNode);	//将起点压栈
//
//	startNode->isIn = (true); //标记为已入栈
//	int path_num = 0;
//	while (!queue.isEmpty()) //判断栈是否空
//	{
//		GraphNodePtr node = queue.front();
//		queue.pop_front();
//		int flag = node->getOne(); //得到相邻的顶点
//		if (flag == -1)							  //如果相邻顶点全部访问过
//		{
//			node = queue.front();//栈弹出一个元素
//			queue.pop_front();					
//			node->resetFlag();			//该顶点相邻的顶点标记为未访问
//			node->isIn = (false);	    //该顶点标记为未入栈
//			continue;					//取栈顶的相邻节点
//		}
//		auto neiNode = m_mapIndexForNode.value(flag);
//		if (neiNode && neiNode->isIn) //若已经在栈中，取下一个顶点
//		{
//			continue;
//		}
//		if (stack.getSize() > maxver - 1) //判断栈中个数是否超过了用户要求的 ，这里是限制了一条路径节点的最大个数
//		{
//			int pop = stack.pop();
//			vertex[pop].resetFlag();
//			vertex[pop].setIsin(false);
//			continue;
//		}
//		stack.push(flag); //将该顶点入栈
//
//		vertex[flag].setIsin(true); //记为已入栈
//
//		if (stack.peek() == selection2) //如果栈顶已经为所求，将此路径记录
//		{
//			int *path = stack.getPath();
//			//保存路径的代码省略
//			int pop = stack.pop();		//将其弹出，继续探索
//			vertex[pop].setIsin(false); //清空入栈的标志位
//		}
//	}
//}
