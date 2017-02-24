/**

文件名：pathOrderOptimizer.h

时间：2016年8月1日 20:10:46
内容：路径，制度; 次序;优化，这里面是一个“路径次序优化类”
注：算法啊！坑爹的算法没看
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef PATHOPTIMIZER_H
#define PATHOPTIMIZER_H

#include <stdint.h>
#include "../utils/polygon.h"
#include "bridge.h"
namespace Engine {

	/**
	时间：2016年8月1日 19:46:29
	内容：路径次序优化
		数据成员：
			Point startPoint; --> 开始点
			vector<PolygonRef> polygons; --> 多边形链表
			vector<int> polyStart; -->  多边形开始点
			vector<int> polyOrder; --> 多边形 命令; 秩序; 规则，制度; 次序
	作者：TwoSilly
	*/
class PathOrderOptimizer
{
public:
    Point startPoint_;//开始点
    vector<PolygonRef> polygons_;//多边形链表
    vector<int> polyStart_;//多边形开始点
    vector<int> polyOrder_;//多边形 命令; 秩序; 规则，制度; 次序

	/**
	时间：2016年8月2日 10:52:10
	内容：路径次序优化类的构造函数
		参数：
			Point startPoint --> 开始点
	注：他的构造函数只赋值了，startPoint点！
	作者：TwoSilly
	*/
    PathOrderOptimizer(Point startPoint)
    {
        this->startPoint_ = startPoint;
    }

	/**
	时间：2016年8月2日 10:52:10
	内容：添加多边形！
		参数：
			PolygonRef polygon --> 
	注：直接把多边形压入多边形链表
	作者：TwoSilly
	*/
    void addPolygon(PolygonRef polygon)
    {
        this->polygons_.push_back(polygon);
    }

    /**
	时间：2016年8月2日 10:52:10
	内容：添加多边形！
		参数：
			Polygons polygons --> 
	注：把多边形复数类分成polygon简单多边形在全部压入this的多边形链表
	作者：TwoSilly
	*/
    void addPolygons(Polygons& polygons)
    {
        for(unsigned int i=0;i<polygons.size(); i++)
            this->polygons_.push_back(polygons[i]);
    }
    
    /**
	时间：2016年8月2日 10:52:10
	内容：优化！
		参数：
	注：
	作者：TwoSilly
	*/
    void optimize();
	//void optimizeTwo();
	//int getClosestPointInPolygon(Point prev_point, int poly_idx);
};

}//namespace Engine

#endif//PATHOPTIMIZER_H
