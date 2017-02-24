/**
文件名：polygonOptimizer.cpp

时间：2016年7月13日 09:38:09
内容：优化多边型
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/
#include "polygonOptimizer.h"

namespace Engine {


	/**
	时间：2016年7月13日 09:40:01
	内容：优化多边型
	判断多边形中相邻的两个点的差值是否在 10微米 内
	如果是就将当前点删除出多边形， 
	-----参数：
	--------PolygonRef poly --> 多边形类的基类里面封装了对多边形的所有基本操作
	作者：TwoSilly
	*/
void optimizePolygon(PolygonRef poly)
{
	//当前多边形中的最后一个点
    Point p0 = poly[poly.size()-1];

	//遍历当前多边形的每个点
    for(unsigned int i=0;i<poly.size();i++)
    {
		//得到当前多边形中的一个点
        Point p1 = poly[i];

		//判断当前两个点的差值是否在 10微米 内
		//如果是就将当前点删除出多边形， 且下标减一
        if (shorterThen(p0 - p1, MICRON2INT(10)))
        {
            poly.remove(i);
            i --;
			//判断当前两个点的差值是否  小于或等于 500微米
        }else if (shorterThen(p0 - p1, MICRON2INT(500)))
        {
            Point p2;
            if (i < poly.size() - 1)
                p2 = poly[i+1];
            else
                p2 = poly[0];
            
            Point diff0 = normal(p1 - p0, 10000000);
            Point diff2 = normal(p1 - p2, 10000000);
            
            int64_t d = dot(diff0, diff2);
            if (d < -99999999999999LL)
            {
                poly.remove(i);
                i --;
            }else{
                p0 = p1;
            }
        }else{
            p0 = p1;
        }
    }
}


	/**
	时间：2016年7月13日 10:18:02
	内容：优化多边型
	-----参数：
	--------Polygons& polys --> 多边形类，他是PolygonRef的友元类
	作者：TwoSilly
	*/
void optimizePolygons(Polygons& polys)
{
    for(unsigned int n=0;n<polys.size();n++)
    {
        optimizePolygon(polys[n]);
        if (polys[n].size() < 3)
        {
            polys.remove(n);
            n--;
        }
    }
	
}

}//namespace Engine
