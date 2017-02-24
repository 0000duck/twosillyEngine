/**

文件名：comb.h

时间：2016年8月2日 11:14:36
内容：梳子类，即每次挤出机移动的补缺！
注：算法呵呵！没看！
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#ifndef COMB_H
#define COMB_H

#include "../utils/polygon.h"

namespace Engine {

/**
时间：2016年8月2日 11:15:53
内容：梳子
	数据成员：
		Polygons& boundery;//边

		int64_t* minX;//最小的x
		int64_t* maxX;//最大的x
		unsigned int* minIdx;//最小的idx
		unsigned int* maxIdx;//最大的idx

		PointMatrix matrix;//矩形
		Point startPoint;//开始点
		Point endPoint;//结束点

作者：TwoSilly
*/
	class Comb
	{
	private:
		Polygons& boundery;//边

		int64_t* minX;//最小的X
		int64_t* maxX;//最大的X
		unsigned int* minIdx;//最小值的下标
		unsigned int* maxIdx;//最大值的下标

		PointMatrix matrix;//矩阵
		Point sp;//开始打印点
		Point ep;//结束打印点

		/**
		时间：2016年8月2日 11:56:47
		内容：前缀检测
		注：其实里面什么都没做直接调用了，collisionTest（）碰撞检测
			参数：
				Point startPoint --> 开始点
				Point endPoint --> 结束点
		作者：TwoSilly
		*/
		bool preTest(Point startPoint, Point endPoint); 

		/**
		时间：2016年8月2日 11:56:47
		内容：碰撞检测
			参数：
				Point startPoint --> 开始点
				Point endPoint --> 结束点
		作者：TwoSilly
		*/
		bool collisionTest(Point startPoint, Point endPoint);

		/**
		时间：2016年8月2日 13:31:40
		内容：计算最小最大值
		
		作者：TwoSilly
		*/
		void calcMinMax();
   		/**
		时间：2016年8月2日 13:32:42
		内容：得到多边形以上
			参数：
				int64_t x
		注：这里这个函数有啥用不大懂啊！
		作者：TwoSilly
		*/
		unsigned int getPolygonAbove(int64_t x);

	  /**
		时间：2016年8月2日 13:45:58
		内容：偏移得到边界点
			参数：
				unsigned int polygonNr --> 多边形的正常范围
				unsigned int idx --> 索引
		作者：TwoSilly
		*/
		Point getBounderyPointWithOffset(unsigned int polygonNr, unsigned int idx);

	public:
		/**
		时间：2016年8月2日 13:53:29
		内容：comb代参构造函数
			参数：
				Polygons& _boundery --> 多边形的正常范围
			
		作者：TwoSilly
		*/
		Comb(Polygons& _boundery);
		~Comb();

		/**
		时间：2016年8月2日 13:55:52
		内容：内部的，点是否在边界上，或边界内
			参数：
				const Point p --> 点
			
		作者：TwoSilly
		*/
		bool inside(const Point p) { return boundery.inside(p); }

		/**
		时间：2016年8月2日 13:55:52
		内容：移动到内部，
			参数：
				Point* p --> 点
				int distance = 100 --> 距离缺省 100
			
		作者：TwoSilly
		*/
		bool moveInside(Point* p, int distance = 100);
		/**
		时间：2016年8月2日 14:09:49
		内容：计算，
			参数：
				Point startPoint --> 开始点
				Point startPoint --> 结束点
				vector<Point>& combPoints --> 梳理点链表			
		作者：TwoSilly
		*/
		bool calc(Point startPoint, Point endPoint, vector<Point>& combPoints);
	};

}//namespace Engine

#endif//COMB_H
