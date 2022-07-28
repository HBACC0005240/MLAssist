#include "AStarGrid.h"
#ifndef SafeDelete
#define SafeDelete(p) \
	if (nullptr != p) \
	{                 \
		delete p;     \
		p = nullptr;  \
	}
#endif // ! SafeDelete

AStarGrid::AStarGrid()
{
}

AStarGrid::AStarGrid(int width, int height)
{
	m_rowCount = height;
	m_columnCount = width;

	/*m_rowCount = width;
	m_columnCount = height;*/
	buildNodes(width, height);
}

AStarGrid::~AStarGrid()
{
	ClearNodes();
}

void AStarGrid::SetGridWH(int width, int height)
{
	ClearNodes();
	m_rowCount = height;
	m_columnCount = width;

	/*m_rowCount = width;
	m_columnCount = height;*/
	buildNodes(width, height);
}

void AStarGrid::ResetNodeState()
{
	for (AStarNode* pNode : m_pChildNode)
	{
		pNode->f = 0;
		pNode->g = 0;
		pNode->h = 0;
		pNode->opened = false;
		pNode->closed = false;
		pNode->parent = nullptr;
	}
}

int AStarGrid::ChildIndex(int x, int y)
{
	if ((x < 0) || (y < 0) || (x >= ColumnCount()) || (y >= RowCount()))
	{
		return -1;
	}
	return (y * ColumnCount()) + x;
}

void AStarGrid::SetWalkableAt(int x, int y, bool walkable)
{
	AStarNode* pNode = GetNodeAt(x, y);
	if (pNode)
	{
		pNode->SetWalkAble(walkable);
	}
}

AStarNodeVec AStarGrid::GetNeighbors(AStarNode* node, DiagonalMovement nDiagonalMovement)
{
	AStarNodeVec neighbors;
	int x = node->x;
	int y = node->y;
	bool s0 = false, d0 = false,
		s1 = false, d1 = false,
		s2 = false, d2 = false,
		s3 = false, d3 = false;

	// ↑
	if (this->IsWalkableAt(x, y - 1))
	{
		neighbors.push_back(GetNodeAt(x, y - 1));
		s0 = true;
	}
	// →
	if (this->IsWalkableAt(x + 1, y))
	{
		neighbors.push_back(GetNodeAt(x + 1, y));
		s1 = true;
	}
	// ↓
	if (this->IsWalkableAt(x, y + 1))
	{
		neighbors.push_back(GetNodeAt(x, y + 1));
		s2 = true;
	}
	// ←
	if (this->IsWalkableAt(x - 1, y))
	{
		neighbors.push_back(GetNodeAt(x - 1, y));
		s3 = true;
	}

	if (nDiagonalMovement == DiagonalMovement::Never)
	{
		return neighbors;
	}

	if (nDiagonalMovement == DiagonalMovement::OnlyWhenNoObstacles)
	{
		d0 = s3 && s0; //←和↑能穿越 则可以↖
		d1 = s0 && s1;
		d2 = s1 && s2;
		d3 = s2 && s3;
	}
	else if (nDiagonalMovement == DiagonalMovement::IfAtMostOneObstacle)
	{
		d0 = s3 || s0;
		d1 = s0 || s1;
		d2 = s1 || s2;
		d3 = s2 || s3;
	}
	else if (nDiagonalMovement == DiagonalMovement::Always)
	{
		d0 = true;
		d1 = true;
		d2 = true;
		d3 = true;
	}
	else
	{
		//throw new Error('Incorrect value of diagonalMovement');
	}

	// ↖
	if (d0 && this->IsWalkableAt(x - 1, y - 1))
	{
		neighbors.push_back(GetNodeAt(x - 1, y - 1));
	}
	// ↗
	if (d1 && this->IsWalkableAt(x + 1, y - 1))
	{
		neighbors.push_back(GetNodeAt(x + 1, y - 1));
	}
	// ↘
	if (d2 && this->IsWalkableAt(x + 1, y + 1))
	{
		neighbors.push_back(GetNodeAt(x + 1, y + 1));
	}
	// ↙
	if (d3 && this->IsWalkableAt(x - 1, y + 1))
	{
		neighbors.push_back(GetNodeAt(x - 1, y + 1));
	}
	return neighbors;
}

AStarGrid* AStarGrid::Clone()
{
	AStarGrid* newGrid = new AStarGrid(m_columnCount, m_rowCount);
	for (int i = 0; i < m_rowCount; ++i)
	{
		for (int j = 0; j < m_columnCount; ++j)
		{
			AStarNode* pNode = GetNodeAt(i, j);
			AStarNode* pNewNode = newGrid->GetNodeAt(i, j);
			if (pNewNode && pNode)
				pNewNode->SetWalkAble(pNode->IsWalkAble());
		}
	}
	return newGrid;
}

void AStarGrid::ClearNodes()
{
	for (AStarNode* pNode : m_pChildNode)
	{
		SafeDelete(pNode);
	}
	m_pChildNode.clear();
}

AStarNode* AStarGrid::GetNodeAt(int x, int y)
{
	int index = ChildIndex(x, y);
	if (index == -1)
		return nullptr;
	return m_pChildNode.at(index);
}

bool AStarGrid::IsWalkableAt(int x, int y)
{
	return this->IsInside(x, y) && this->GetNodeAt(x, y)->IsWalkAble();
}

bool AStarGrid::IsInside(int x, int y)
{
	//	qDebug() << "IsInside--" << QString("x:%1-y:%2-col:%3-row:%4").arg(x).arg(y).arg(m_columnCount).arg(m_rowCount);
	return (x >= 0 && x < this->m_columnCount) && (y >= 0 && y < this->m_rowCount);
}

void AStarGrid::buildNodes(int x, int y)
{
	for (int i = 0; i < y; ++i)
	{
		for (int j = 0; j < x; ++j)
		{
			AStarNode* pNode = new AStarNode(j, i);
			m_pChildNode.push_back(pNode);
		}
	}
}
