#pragma once
#include "AStarUtil.h"
#include <vector>
using namespace std;

vector<pair<int,int>> AStarUtil::interpolate(int x0, int y0, int x1, int y1)
{
	vector<pair<int, int>> line;
	int	sx, sy, dx, dy, err, e2;

	dx = abs(x1 - x0);
	dy = abs(y1 - y0);

	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;

	err = dx - dy;

	while (true) 
	{
		line.push_back(std::make_pair(x0, y0));

		if (x0 == x1 && y0 == y1)
		{
			break;
		}

		e2 = 2 * err;
		if (e2 > -dy) 
		{
			err = err - dy;
			x0 = x0 + sx;
		}
		if (e2 < dx) {
			err = err + dx;
			y0 = y0 + sy;
		}
	}
	return line;
}

vector<pair<int, int>> AStarUtil::expandPath(vector<pair<int, int>> path)
{
	vector<pair<int, int>> expanded;
	int	len = path.size();
	pair<int, int> coord0, coord1;
	vector<pair<int, int>> interpolated;
	int interpolatedLen;
	if (len < 2) 
	{
		return expanded;
	}

	for (int i = 0; i < len - 1; ++i) 
	{
		coord0 = path[i];
		coord1 = path[i + 1];

		interpolated = interpolate(coord0.first, coord0.second, coord1.first, coord1.second);
		interpolatedLen = interpolated.size();
		for (int j = 0; j < interpolatedLen - 1; ++j) 
		{
			expanded.push_back(interpolated[j]);
		}
	}
	expanded.push_back(path[len - 1]);
	return expanded;
}

vector<pair<int, int>> AStarUtil::smoothenPath(AStarGrid* grid, vector<pair<int, int>> path)
{
	int len = path.size();
	if (len < 2)
	{
		return vector<pair<int, int>>();
	}
	int	x0 = path[0].first;        // path start x
	int y0 = path[0].second;        // path start y
	int	x1 = path[len - 1].first;  // path end x
	int y1 = path[len - 1].second;  // path end y
	int	sx, sy,                 // current start coordinate
		ex, ey;                 // current end coordinate
	vector<pair<int, int>>	newPath;
	pair<int, int>	coord, testCoord;
	vector<pair<int, int>>	line;
	bool blocked = false;
	sx = x0;
	sy = y0;
	newPath.push_back(std::make_pair(sx, sy));

	for (int i = 2; i < len; ++i) 
	{
		coord = path[i];
		ex = coord.first;
		ey = coord.second;
		line = interpolate(sx, sy, ex, ey);

		blocked = false;
		for (int j = 1; j < line.size(); ++j)
		{
			testCoord = line[j];

			if (!grid->IsWalkableAt(testCoord.first, testCoord.second))
			{
				blocked = true;
				break;
			}
		}
		if (blocked) 
		{
			auto lastValidCoord = path[i - 1];
			newPath.push_back(lastValidCoord);
			sx = lastValidCoord.first;
			sy = lastValidCoord.second;
		}
	}
	newPath.push_back(std::make_pair(x1,y1));

	return newPath;
}

vector<pair<int, int>> AStarUtil::compressPath(vector<pair<int, int>> path)
{
	// nothing to compress
	if (path.size() < 3) 
	{
		return path;
	}

	vector<pair<int, int>> compressed;
	int	sx = path[0].first, // start x
		sy = path[0].second, // start y
		px = path[1].first, // second point x
		py = path[1].second; // second point y
	double	dx = px - sx,	// direction between the two points
		dy = py - sy;		// direction between the two points
	int	lx, ly;
	double	ldx, ldy, sq;
	double xDis, yDis;
	// normalize the direction
	sq = sqrt(dx * dx + dy * dy);
	dx /= sq;
	dy /= sq;

	// start the new path
	compressed.push_back(std::make_pair(sx,sy));

	for (int i = 2; i < path.size(); i++) 
	{

		// store the last point
		lx = px;
		ly = py;

		// store the last direction
		ldx = dx;
		ldy = dy;

		// next point
		px = path[i].first;
		py = path[i].second;

		// next direction
		xDis = dx = px - lx;
		yDis = dy = py - ly;

		// normalize
		sq = sqrt(dx * dx + dy * dy);
		dx /= sq;
		dy /= sq;

		// if the direction has changed, store the point
		if (dx != ldx || dy != ldy) 
		{
			compressed.push_back(std::make_pair(lx,ly));
		}
		else if (xDis > 7 || yDis > 7)//新增两点之间差值判断，距离过远增加进去
		{
			compressed.push_back(std::make_pair(lx, ly));
		}
		else //判断最后一次压缩点坐标 和当前坐标距离 判断有问题，如果点差太多 应该把前面的也要加上
		{
			auto lastPos = compressed.back();	
			xDis = abs( lx - lastPos.first);	//判断之前点坐标
			yDis = abs( ly - lastPos.second);
			if (xDis > 7 || yDis > 7)
			{
				compressed.push_back(std::make_pair(lx, ly));
			}
			else			//判断最新点坐标
			{
				xDis = abs(px - lastPos.first);
				yDis = abs(py - lastPos.second);
				if (xDis > 7 || yDis > 7)
				{
					compressed.push_back(std::make_pair(lx, ly));
				}
			}
		}
	}

	// store the last point
	compressed.push_back(std::make_pair(px, py)); 
	return compressed;
}
//
//vector<pair<int, int>> AStarUtil::expandPathDstVal(vector<pair<int, int>> path)
//{
//	vector<pair<int, int>> expanded;
//	int	len = path.size();
//	pair<int, int> coord0, coord1;
//	vector<pair<int, int>> interpolated;
//	int interpolatedLen;
//	if (len < 2)
//	{
//		return path;
//	}
//
//	for (int i = 0; i < len - 1; ++i)
//	{
//		coord0 = path[i];
//		coord1 = path[i + 1];
//
//		interpolated = interpolateDstVal(coord0.first, coord0.second, coord1.first, coord1.second);
//		interpolatedLen = interpolated.size();
//		for (int j = 0; j < interpolatedLen - 1; ++j)
//		{
//			expanded.push_back(interpolated[j]);
//		}
//	}
//	expanded.push_back(path[len - 1]);
//	return expanded;
//}
//
//vector<pair<int, int>> AStarUtil::interpolateDstVal(int x0, int y0, int x1, int y1, int dis)
//{
//	vector<pair<int, int>> line;
//	int	sx, sy, dx, dy, err, e2;
//
//	dx = abs(x1 - x0);
//	dy = abs(y1 - y0);
//
//	sx = (x0 < x1) ? 1 : -1;
//	sy = (y0 < y1) ? 1 : -1;
//
//	err = dx - dy;
//
//	while (true)
//	{
//		line.push_back(std::make_pair(x0, y0));
//
//		if (x0 == x1 && y0 == y1)
//		{
//			break;
//		}
//
//		e2 = 2 * err;
//		if (e2 > -dy)
//		{
//			err = err - dy;
//			x0 = x0 + sx;
//		}
//		if (e2 < dx) {
//			err = err + dx;
//			y0 = y0 + sy;
//		}
//	}
//	return line;
//}

//曼哈顿距离 如果只能四方向(上下左右)移动， 曼哈顿距离（Manhattan distance）是最合适的启发函数.
double AStarUtil::manhattan(double dx, double dy)
{
	return dx + dy;
}
//欧几里得距离 如果地图不再被划分成网格，玩家可以在任意位置向任意方向移动的话，就可以用欧几里得距离了。
double AStarUtil::euclidean(double dx, double dy)
{
	return sqrt(dx * dx + dy * dy);
}
//欧式距离 如果地图中允许任意方向移动 此时使用 欧式距离（Euclidean distance）来作为启发函数比较合适.
//如果开平方根消耗的资源仍然非常显著，那么我们可以用快速近似开平方根算法来代替一般的开平方根算法，或者干脆用 octile 距离来近似代替欧几里得距离。
double AStarUtil::octile(double dx, double dy)
{
	double F = sqrt(2) - 1;
	return (dx < dy) ? F * dx + dy : F * dy + dx;
}
//切比雪夫距离（如果网格地图可以八方向(包括斜对角)移动, 使用 切比雪夫距离（Chebyshev distance）作为启发函数比较合适.
double AStarUtil::chebyshev(double dx, double dy)
{
	return max(dx, dy);
}

vector<pair<int, int>> AStarUtil::backtrace(AStarNode* node)
{
	vector<pair<int, int>> path;
	path.push_back(std::make_pair(node->x, node->y));
	while (node->parent) 
	{
		node = node->parent;
		path.push_back(std::make_pair(node->x, node->y));
	}
	reverse(path.begin(), path.end());
	return path;
}

vector<pair<int, int>> AStarUtil::biBacktrace(AStarNode* nodeA, AStarNode* nodeB)
{
	vector<pair<int, int>> pathA = backtrace(nodeA);
	vector<pair<int, int>> pathB = backtrace(nodeB);
	reverse(pathB.begin(), pathB.end());
	pathA.insert(pathA.end(), pathB.begin(), pathB.end());
	return pathA;
}

double AStarUtil::pathLength(vector<pair<int, int>> path)
{
	double sum = 0;
	pair<int, int> a, b;
	double dx, dy;
	for (int i = 1; i < path.size(); ++i) 
	{
		a = path.at(i - 1);
		b = path[i];
		dx = a.first - b.first;
		dy = a.second - b.second;
		sum += sqrt(dx * dx + dy * dy);
	}
	return sum;
}
