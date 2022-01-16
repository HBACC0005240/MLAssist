#include "Tsp.h"
#include <QDebug>

double Tsp::dist(QPoint A, QPoint B)
{
	//调用A*算法 计算
	AStar aStarFindPath(true, true);
	_grid.ResetNodeState();
	QPoint frompos = A;
	QPoint topos = B;
	auto path = aStarFindPath.FindPath(frompos.x(), frompos.y(), topos.x(), topos.y(), &_grid);
	//压不压缩都行
	std::vector<std::pair<int, int> > findPath = AStarUtil::compressPath(path);
	return AStarUtil::pathLength(findPath);
	//	return sqrt((A.x() - B.x()) * (A.x() - B.x()) + (A.y() - B.y()) * (A.y() - B.y()));
}

void Tsp::Input(QVector<QPoint> p, QVector<short> map, int width, int height)
{
	qDebug() << "Tsp:input Pos" << p;
	_p = p;
	m_map = map;
	m_mapWidth = width;
	m_mapHeight = height;
	int nCount = _p.size();
	bestPath.citys.resize(nCount);
	_w.resize(nCount);
	for (size_t i = 0; i < nCount; i++)
	{
		_w[i].resize(nCount);
	}
	initDistance();
	GetPath();
}

void Tsp::initDistance()
{
	int w = m_mapWidth;
	int h = m_mapHeight;
	QColor qRgb(0, 0, 0);
	//x是列  y是行
	_grid.SetGridWH(w, h);
	int xtop = w;
	int ytop = h;
	for (int tmpy = 0; tmpy < ytop; ++tmpy) //列
	{
		for (int tmpx = 0; tmpx < xtop; ++tmpx) //行
		{
			//	qDebug() << tmpy <<"," << tmpx;
			int dataIndex = (tmpx + tmpy * w);
			if (m_map.at(dataIndex) == 1)				//不可通行 1
				_grid.SetWalkableAt(tmpx,tmpy, false); //灰色 不可行
			else
				_grid.SetWalkableAt(tmpx, tmpy, true);
		}
	}
	int count = _p.size();
	for (int i = 0; i < count; i++)
		for (int j = i + 1; j < count; j++)
			_w[i][j] = _w[j][i] = dist(_p[i], _p[j]);
}

void Tsp::GetPath()
{
	nCase = 0;
	bestPath.len = 0;
	int count = _p.size();
	for (int i = 0; i < count; i++)
	{
		bestPath.citys[i] = i;
		if (i != count - 1)
		{
			//			qDebug() <<i << "--->";
			bestPath.len += _w[i][i + 1];
		}
		//		else
		//			qDebug() <<i ;
	}
	//	qDebug() <<"Init path length is : " <<  bestPath.len;
	//	qDebug() <<"-----------------------------------";
}

void Tsp::Print(Path t, int n)
{
	//	qDebug() <<("Path is : ");
	//for (int i = 0; i < n; i++)
	//{
	//	if (i != n - 1)
	//		qDebug() << t.citys[i] << "-->";
	//	else
	//		qDebug() << t.citys[i];
	//}
	//	qDebug() <<"The path length is :" <<  t.len;
	//	qDebug() <<"-----------------------------------";
}

void Tsp::SetStart(QPoint start, QPoint end)
{
	m_startPos = start;
	m_endPos = end;
}

Path Tsp::GetNext(Path p, int n)
{
	if (n == 0 || n==1)
		return p;
	Path ans = p;
	int x = (int)(n * (rand() / (RAND_MAX + 1.0)));
	int y = (int)(n * (rand() / (RAND_MAX + 1.0)));
	int tryNum = 0;
	while (x == y)
	{
		if (tryNum>=10)
		{
			x = rand()%n;
			y = rand() % n;			
		}
		else if (tryNum >= 20)
			return p;
		else
		{
			x = (int)(n * (rand() / (RAND_MAX + 1.0)));
			y = (int)(n * (rand() / (RAND_MAX + 1.0)));
		}		
		tryNum++;
	}
	swap(ans.citys[x], ans.citys[y]);
	ans.len = 0;
	for (int i = 0; i < n - 1; i++)
		ans.len += _w[ans.citys[i]][ans.citys[i + 1]];
	//	cout << "nCase = " << nCase << endl;
	//增加起始点 这里距离增加第一个点到起点距离 最后一个点到起点距离
	//ans.len += dist(_p.at(ans.citys[0]), m_startPos);
	//ans.len += dist(_p.at(ans.citys[n - 1]), _p.at(ans.citys[0]));
	/*ans.len += dist(_p.at(ans.citys[n - 1]), m_startPos);
	ans.len += dist(m_startPos, m_endPos);*/
	ans.len += dist(m_startPos, _p.at(ans.citys[0]));
	ans.len += dist(_p.at(ans.citys[n - 1]), m_endPos);
	//	Print(ans, n);
	nCase++;
	return ans;
}

int Tsp::GetPosToOtherLen(Path* newPath, int index, QList<QPoint> existCalcPosList)
{
	//计算从起点开始 到此点的全部路径
	//求最短
	int minPath = 100000;
	int city = -1;
	for (int i = 0; i < _p.size(); ++i)
	{
		if (!existCalcPosList.contains(_p[i])) //已经走过 跳过
		{
			if (minPath > _w[index][i])
			{
				minPath = _w[index][i];
				city = i;
			}
		}
	}
	if (city != -1)
	{
		newPath->len += minPath;
		newPath->citys.append(city);
		existCalcPosList.append(_p[city]);
		GetPosToOtherLen(newPath, city, existCalcPosList);
	}
	return 0;
}

void Tsp::SA()
{
	//重新写下搜索算法 贪心算法
	//1、从起点计算 最短路径
	//2、到达目的地后，以此计算路径
	/*
	QList<Path*> allPath;
	for (int i = 0; i < _p.size(); i++)
	{
		QList<QPoint> existCalcPosList;
		Path* newPath=new Path;
		newPath->len += dist(_p[i], m_startPos);
		newPath->citys.append(i);
		existCalcPosList.append(_p[i]);
		allPath.append(newPath);
		//计算从起点开始 到此点的全部路径
		GetPosToOtherLen(newPath, i, existCalcPosList);		
	}
	 
	//试了只判断第一层最近点 上面是第一层所有点中最短路径
	//QList<QPoint> existCalcPosList;
	//Path *tNewPath = new Path;
	//int tmpMinLen = 10000;
	//int tmpIndex = 0;
	//for (int i = 0; i < _p.size(); i++)
	//{		
	//	int ttLen = dist(_p[i], m_startPos);
	//	if (ttLen < tmpMinLen)
	//	{
	//		tmpMinLen = ttLen;
	//		tmpIndex = i;
	//	}
	//}
	//if (tmpMinLen != 10000)
	//{
	//	tNewPath->len += tmpMinLen;
	//	tNewPath->citys.append(tmpIndex);
	//	existCalcPosList.append(_p[tmpIndex]);
	//	allPath.append(tNewPath);
	//	//计算从起点开始 到此点的全部路径
	//	GetPosToOtherLen(tNewPath, tmpIndex, existCalcPosList);		
	//}
	//筛选出最短路径的
	int minDistance = 100000;
	Path *shortPath = nullptr;
	for (int i=0;i<allPath.size();++i)
	{
		//End Pos Add
		//allPath[i]->len += dist(_p[allPath[i]->citys.last()], m_endPos);
		if (allPath[i]->len < minDistance)
		{
			minDistance = allPath[i]->len;
			shortPath = allPath[i];
		}
	}
	if (minDistance != 0 && shortPath != nullptr)
	{
		bestPath.citys = shortPath->citys;
		bestPath.len = shortPath->len;
	}
	for (int i = 0; i < allPath.size(); ++i)
	{
		delete allPath[i];
		allPath[i] = nullptr;
	}
	return;*/
	double t = 100 * _p.size(); //T;	//初始温度取数量*100
	if (t < 3000)
	{
		t = 3000;
	}
	srand((unsigned)(time(NULL)));
	Path curPath = bestPath;
	Path newPath = bestPath;
	int P_L = 0;
	int P_F = 0;
	int nCount = _p.size();
	while (1) //外循环，主要更新参数t，模拟退火过程
	{
		for (int i = 0; i < ILOOP; i++) //内循环，寻找在一定温度下的最优值
		{
			newPath = GetNext(curPath, nCount);
			double dE = newPath.len - curPath.len;
			if (dE < 0) //如果找到更优值，直接更新
			{
				curPath = newPath;
				P_L = 0;
				P_F = 0;
			}
			else
			{
				double rd = rand() / (RAND_MAX + 1.0);
				auto dtval = exp(dE / t);
				//如果找到比当前更差的解，以一定概率接受该解，并且这个概率会越来越小
				if (dtval > rd && dtval < 1)
					curPath = newPath;
				P_L++;
			}
			if (P_L > LIMIT)
			{
				P_F++;
				break;
			}
		}
		if (curPath.len < bestPath.len)
			bestPath = curPath;
		//if (P_F > OLOOP || t < EPS)
		if (P_F > OLOOP || t > EPS) //退火应该用上面< 进行递减，这里发现取得路线计算不太准，用上面还耗时间，还是先用这个，外部只循环一次
			break;
		t *= DELTA;
	}
}
