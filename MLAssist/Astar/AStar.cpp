#include "AStar.h"
#include <QDebug>
AStar::AStar()
{

}

AStar::AStar(bool bAllowDiagonal /*= false */, bool bDontCrossCorners/*=false*/)
{
	m_bAllowDiagonal = bAllowDiagonal;
	m_bDontCrossCorners = bDontCrossCorners;
	m_pFun = AStarUtil::manhattan;

	if (!m_nDiagonalMovement) 
	{
		if (!m_bAllowDiagonal) 
		{
			m_nDiagonalMovement = DiagonalMovement::Never;
		}
		else 
		{
			if (m_bDontCrossCorners) 
			{
				m_nDiagonalMovement = DiagonalMovement::OnlyWhenNoObstacles;
			}
			else {
				m_nDiagonalMovement = DiagonalMovement::IfAtMostOneObstacle;
			}
		}
	}

	// When diagonal movement is allowed the manhattan heuristic is not
	//admissible. It should be octile instead 如果允许对角线移动，则不能使用曼哈顿试探法
	//允许。应该是密闭的
	if (m_nDiagonalMovement == DiagonalMovement::Never)
	{
		m_pFun = AStarUtil::manhattan;
	}
	//else if (m_nDiagonalMovement == DiagonalMovement::OnlyWhenNoObstacles)
	//{
	//	m_pFun = AStarUtil::chebyshev;//
	//}
	else
	{
		m_pFun = AStarUtil::octile;
	}
}

AStar::~AStar()
{

}
static bool GreaterSort(AStarNode* nodeA, AStarNode* nodeB)
{
	if (!nodeA || !nodeB)
		return false;
	
	return nodeA->f > nodeB->f;
}
vector<pair<int, int>> AStar::FindPath(int startX, int startY, int endX, int endY, AStarGrid* grid)
{
	AStarNodeVec openList;	
	AStarNode* startNode = grid->GetNodeAt(startX, startY);
	AStarNode* endNode = grid->GetNodeAt(endX, endY);
	if (startNode ==nullptr || endNode ==nullptr)
	{
		return vector<pair<int, int>>();
	}
	AStarNodeVec neighbors;
	AStarNode* neighbor = nullptr;
	int x=0,y=0;
	double ng=0;
	vector<pair<int, int>> tmpPath;
	// set the `g` and `f` value of the start node to be 0
	startNode->g = 0;
	startNode->f = 0;

	// push the start node into the open list
	openList.push_back(startNode);
	startNode->opened = true;
	AStarNode* node=nullptr;
	// while the open list is not empty 从起点开始遍历
	while (!openList.empty()) 
	{
		// pop the position of node which has the minimum `f` value.  取出点
		node =  openList.back();//openList.front();//
		openList.pop_back();
		node->closed = true;		//标记已经计算过

		// if reached the end position, construct the path and return it
		if (node == endNode)		//如果已经到了终点，回溯路线并返回  并回溯最佳路线
		{
			return AStarUtil::backtrace(endNode);
		}	
		// get neigbours of the current node 获取当前坐标周围的坐标
		neighbors = grid->GetNeighbors(node, m_nDiagonalMovement);
	//	qDebug() << "neighbors--" << neighbors.size();
		for (int i = 0; i < neighbors.size(); ++i)//遍历附近的节点 
		{
			neighbor = neighbors[i];

			if (neighbor->closed)			//已经走过             // 已经结算过的就忽略了
			{
				continue;
			}

			x = neighbor->x;
			y = neighbor->y;

			// get the distance between current node and the neighbor 获取当前节点和周围节点的距离 g(n), 实际距离, 实际上是 Euclidean distance
			// and calculate the next g score	计算代价
			ng = node->g + ((x - node->x ==  0 || y - node->y == 0) ? 1 : sqrt(2));

			// check if the neighbor has not been inspected yet, or	检查周围的节点 最小代价
			// can be reached with smaller cost from the current node可达目标 挑选其中最小代价
			// 两种情况需要计算:
			// 1. 这是一个新的节点
			// 2. 这个节点当前计算的 g(n) 更优
			if (!neighbor->opened || ng < neighbor->g)
			{
				neighbor->g = ng;					//从起点到这里的目标得分 goal score 更新到当前节点的得分
				// h = 权重 * 启发函数的计算结果				
				neighbor->h = neighbor->h ? neighbor->h : m_nWeight * m_pFun(abs(x - endX), abs(y - endY));
				neighbor->f = neighbor->g + neighbor->h;
				neighbor->parent = node;                // 到父节点的链接, 方便结果回溯

								// 更新到结果集
				if (!neighbor->opened)
				{
					openList.push_back(neighbor);
					sort(openList.begin(), openList.end(), GreaterSort);
					neighbor->opened = true;		//判断条件 已经加入过寻路列表
				}
				else
				{
					// the neighbor can be reached with smaller cost.
					// Since its f value has been updated, we have to
					// update its position in the open list
				//	openList.updateItem(neighbor);	
					sort(openList.begin(), openList.end(), GreaterSort);

				}
			}
		} // end for each neighbor
	} // end while not open list empty

	// fail to find the path
	return vector<pair<int, int>>();
}
