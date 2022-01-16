#include "AStarNode.h"

AStarNode::AStarNode()
{

}

AStarNode::AStarNode(int tx, int ty, bool walkAble/*=false*/)
{
	x = tx;
	y = ty;
	bWalkAble = walkAble;
}

AStarNode::~AStarNode()
{

}
