/**
文件名：polygonOptimizer.h

时间：2016年7月13日 09:38:09
内容：优化多边型
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/
#ifndef POLYGON_OPTIMIZER_H
#define POLYGON_OPTIMIZER_H

#include "../utils/polygon.h"

namespace Engine {

	/**
	时间：2016年7月13日 09:40:01
	内容：优化多边型
	-----参数：
	--------PolygonRef poly --> 多边形类的基类里面封装了对多边形的所有基本操作
	作者：TwoSilly
	*/
void optimizePolygon(PolygonRef poly);


	/**
	时间：2016年7月13日 10:18:02
	内容：优化多边型
	-----参数：
	--------Polygons& polys --> 多边形类，他是PolygonRef的友元类
	作者：TwoSilly
	*/
void optimizePolygons(Polygons& polys);

}//namespace Engine

#endif//POLYGON_OPTIMIZER_H
