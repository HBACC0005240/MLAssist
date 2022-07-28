#pragma once
#include <list>
#include <vector>
using namespace std;
class AStarNode
{
public:
	AStarNode();
	AStarNode(int x, int y,bool walkAble=false);
	~AStarNode();

	bool IsWalkAble() { return bWalkAble; }
	void SetWalkAble(bool bAble) { bWalkAble = bAble; }

	double   f = 0;		//代价	
	double   g = 0;		//已经付出的代价
	double   h = 0;		//预计要付出的代价
	bool opened = false;//开放
	bool closed = false;//关闭
	int x = -1;
	int y = -1;
	//bool wailkable = false;

	bool bWalkAble=false;
	AStarNode* parent=nullptr;
	
};
typedef list<AStarNode*> AStarNodeList;
typedef vector<AStarNode*> AStarNodeVec;