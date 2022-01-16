#pragma once
#include <list>
#include <math.h>
#include "AStarNode.h"
#include "AStarGrid.h"
#include "AStarDef.h"
using namespace std;
#define M_POINT int
#define A_PATH vector<pair<int, int>>


class AStarUtil
{
public:
	static double manhattan(double dx,double dy);
	static double euclidean(double dx, double dy);
	static double octile(double dx, double dy);
	static double chebyshev(double dx, double dy);
	static vector<pair<int, int>> backtrace(AStarNode* node);
	static vector<pair<int, int>> biBacktrace(AStarNode* nodeA, AStarNode* nodeB);
	static double pathLength(vector<pair<int, int>> path);
	static vector<pair<int, int>> interpolate(int x0,int y0,int x1,int y1);
	static vector<pair<int, int>> expandPath(vector<pair<int, int>> path);
	static vector<pair<int, int>> smoothenPath(AStarGrid* grid, vector<pair<int, int>> path);
	static vector<pair<int, int>> compressPath(vector<pair<int, int>> path);
	//static vector<pair<int, int>> expandPathDstVal(vector<pair<int, int>> path);
	//static vector<pair<int, int>> interpolateDstVal(int x0, int y0, int x1, int y1,int dis);


};
