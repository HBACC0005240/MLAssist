#pragma once
#include "./AStar/AStar.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <QHash>
#include <QImage>
#include <QVector>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

//#define N     30      //城市数量
//#define T     3000    //初始温度
#define EPS 1e-8   //终止温度
#define DELTA 0.98 //温度衰减率 退火速度

#define LIMIT 1000 //概率选择上限
#define OLOOP 20   //外循环次数
#define ILOOP 100  //内循环次数 马可夫链长度
//定义路线结构体
struct Path
{
	QVector<int> citys;
	double len=0;
};

class Tsp
{
public:
	void GetPath();
	void Input(QVector<QPoint> p, QVector<short> map, int width, int height);
	double dist(QPoint A, QPoint B);
	Path GetBestPath() { return bestPath; }
	void SetStart(QPoint start, QPoint end);
	Path GetNext(Path p, int n);
	void Print(Path t, int n);

	void SA();

protected:
	void initDistance();
	int GetPosToOtherLen(Path* newPath, int index, QList<QPoint> existCalcPosList);

private:
	Path bestPath;		  //记录最优路径
	int m_mapWidth = 0;	  //宽
	int m_mapHeight = 0;  //高
	QPoint m_startPos;	  //起始点
	QPoint m_endPos;	  //终止点
	int nCase;			  //测试次数
	QVector<short> m_map; //计算距离用
	AStarGrid _grid;
	QVector<QPoint> _p;		   //每个城市的坐标
	int _cityCount = 0;		   //城市数量
	int _initT = 3000;		   //初始温度
	int _finiEps = 1e-8;	   //终止温度
	double _delta = 0.98;	   //温度衰减率
	int _limit = 1000;		   //概率选择上限
	int _outLoop = 20;		   //外循环次数
	int _inLoop = 100;		   //内循环次数
	QVector<QVector<int> > _w; //两两城市之间路径长度
};
