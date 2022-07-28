#pragma once
#include "AStarNode.h"
#include "AStarDef.h"
class AStarGrid
{
public:
	AStarGrid();
	AStarGrid(int width, int height);
	~AStarGrid();

	void SetGridWH(int width, int height);

	void ResetNodeState();
	AStarNode* GetNodeAt(int x, int y);

	bool IsWalkableAt(int x, int y);
	bool IsInside(int x, int y);
	int  ChildIndex(int x, int y);
	void SetWalkableAt(int x, int y, bool walkable);
	AStarNodeVec GetNeighbors(AStarNode* node, DiagonalMovement nDiagonalMovement);
	AStarGrid* Clone();
	inline int RowCount() const { return m_rowCount; }             //��
	inline int ColumnCount() const { return m_columnCount; }       //��
	void ClearNodes();
protected:
	void buildNodes(int x, int y);
private:
	int m_rowCount = -1;      //y
	int m_columnCount = -1;  //x
	AStarNodeVec m_pChildNode;
};

