/**

文件名：comb.cpp

时间：2016年8月2日 11:14:36
内容：梳子类，即每次挤出机移动的补缺！
注：算法呵呵！没看！
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include "comb.h"

namespace Engine {

		/**
	时间：2016年8月2日 11:56:47
	内容：前缀检测
	注：其实里面什么都没做直接调用了，collisionTest（）碰撞检测
		参数：
			Point startPoint --> 开始点
			Point endPoint --> 结束点
	作者：TwoSilly
	*/
	bool Comb::preTest(Point startPoint, Point endPoint)
	{
		return collisionTest(startPoint, endPoint);
	}

	/**
	时间：2016年8月2日 11:56:47
	内容：碰撞检测
		参数：
			Point startPoint --> 开始点
			Point endPoint --> 结束点
	作者：TwoSilly
	*/
	bool Comb::collisionTest(Point startPoint, Point endPoint)
	{
		Point diff = endPoint - startPoint;

		matrix = PointMatrix(diff);
		sp = matrix.apply(startPoint);
		ep = matrix.apply(endPoint);

		for(unsigned int n=0; n<boundery.size(); n++)
		{
			if (boundery[n].size() < 1)
				continue;
			Point p0 = matrix.apply(boundery[n][boundery[n].size()-1]);
			for(unsigned int i=0; i<boundery[n].size(); i++)
			{
				Point p1 = matrix.apply(boundery[n][i]);
				if ((p0.Y > sp.Y && p1.Y < sp.Y) || (p1.Y > sp.Y && p0.Y < sp.Y))
				{
					int64_t x = p0.X + (p1.X - p0.X) * (sp.Y - p0.Y) / (p1.Y - p0.Y);

					if (x > sp.X && x < ep.X)
						return true;
				}
				p0 = p1;
			}
		}
		return false;
	}

		/**
	时间：2016年8月2日 13:31:40
	内容：计算最小最大值
		
	作者：TwoSilly
	*/
	void Comb::calcMinMax()
	{
		for(unsigned int n=0; n<boundery.size(); n++)
		{
			minX[n] = INT64_MAX;
			maxX[n] = INT64_MIN;
			Point p0 = matrix.apply(boundery[n][boundery[n].size()-1]);
			for(unsigned int i=0; i<boundery[n].size(); i++)
			{
				Point p1 = matrix.apply(boundery[n][i]);
				if ((p0.Y > sp.Y && p1.Y < sp.Y) || (p1.Y > sp.Y && p0.Y < sp.Y))
				{
					int64_t x = p0.X + (p1.X - p0.X) * (sp.Y - p0.Y) / (p1.Y - p0.Y);

					if (x >= sp.X && x <= ep.X)
					{
						if (x < minX[n]) { minX[n] = x; minIdx[n] = i; }
						if (x > maxX[n]) { maxX[n] = x; maxIdx[n] = i; }
					}
				}
				p0 = p1;
			}
		}
	}

   	/**
	时间：2016年8月2日 13:32:42
	内容：得到多边形以上
		参数：
			int64_t x
	注：这里这个函数有啥用不大懂啊！
	作者：TwoSilly
	*/
	unsigned int Comb::getPolygonAbove(int64_t x)
	{
		int64_t min = POINT_MAX;
		unsigned int ret = NO_INDEX;
		for(unsigned int n=0; n<boundery.size(); n++)
		{
			if (minX[n] > x && minX[n] < min)
			{
				min = minX[n];
				ret = n;
			}
		}
		return ret;
	}

	/**
	时间：2016年8月2日 13:45:58
	内容：偏移得到边界点
		参数：
			unsigned int polygonNr --> 多边形的正常范围
			unsigned int idx --> 索引
	作者：TwoSilly
	*/
	Point Comb::getBounderyPointWithOffset(unsigned int polygonNr, unsigned int idx)
	{
		Point p0 = boundery[polygonNr][(idx > 0) ? (idx - 1) : (boundery[polygonNr].size() - 1)];
		Point p1 = boundery[polygonNr][idx];
		Point p2 = boundery[polygonNr][(idx < (boundery[polygonNr].size() - 1)) ? (idx + 1) : (0)];

		Point off0 = crossZ(normal(p1 - p0, MM2INT(1.0)));
		Point off1 = crossZ(normal(p2 - p1, MM2INT(1.0)));
		Point n = normal(off0 + off1, MM2INT(0.2));

		return p1 + n;
	}

	Comb::Comb(Polygons& _boundery)
		: boundery(_boundery)
	{
		minX = new int64_t[boundery.size()];
		maxX = new int64_t[boundery.size()];
		minIdx = new unsigned int[boundery.size()];
		maxIdx = new unsigned int[boundery.size()];
	}

	Comb::~Comb()
	{
		delete[] minX;
		delete[] maxX;
		delete[] minIdx;
		delete[] maxIdx;
	}

	/**
	时间：2016年8月2日 13:55:52
	内容：移动到内部，
		参数：
			Point* p --> 点
			int distance = 100 --> 距离缺省 100
			
	作者：TwoSilly
	*/
	bool Comb::moveInside(Point* p, int distance)
	{
		Point ret = *p;
		int64_t bestDist = MM2INT(2.0) * MM2INT(2.0);
		for(unsigned int n=0; n<boundery.size(); n++)
		{
			if (boundery[n].size() < 1)
				continue;
			Point p0 = boundery[n][boundery[n].size()-1];
			for(unsigned int i=0; i<boundery[n].size(); i++)
			{
				Point p1 = boundery[n][i];

				//Q = A + Normal( B - A ) * ((( B - A ) dot ( P - A )) / VSize( A - B ));
				Point pDiff = p1 - p0;
				int64_t lineLength = vSize(pDiff);
				int64_t distOnLine = dot(pDiff, *p - p0) / lineLength;
				if (distOnLine < 10)
					distOnLine = 10;
				if (distOnLine > lineLength - 10)
					distOnLine = lineLength - 10;
				Point q = p0 + pDiff * distOnLine / lineLength;

				int64_t dist = vSize2(q - *p);
				if (dist < bestDist)
				{
					bestDist = dist;
					ret = q + crossZ(normal(p1 - p0, distance));
				}

				p0 = p1;
			}
		}
		if (bestDist < MM2INT(2.0) * MM2INT(2.0))
		{
			*p = ret;
			return true;
		}
		return false;
	}

	bool Comb::calc(Point startPoint, Point endPoint, vector<Point>& combPoints)
	{
		if (shorterThen(endPoint - startPoint, MM2INT(1.5)))
			return true;

		bool addEndpoint = false;
		//检查如果我们是内部的梳状边界
		if (!boundery.inside(startPoint))
		{
			if (!moveInside(&startPoint))     //如果我们不能把点梳到边界内我们需要收回。
				return false;
			combPoints.push_back(startPoint);
		}
		if (!boundery.inside(endPoint))
		{
			if (!moveInside(&endPoint))    //如果我们不能把点梳到边界内我们需要收回.
				return false;
			addEndpoint = true;
		}

		 //看看我们跨越任何边界，和预计算一些值.
		if (!preTest(startPoint, endPoint))
		{
			//我们没有跨越任何界限。所以跳过梳子的一代。 
			if (!addEndpoint && combPoints.size() == 0) //只跳，如果我们没有移动的开始和结束点.
				return true;
		}

		// //计算我们穿过梳形边界的最小和最大位置
		calcMinMax();

		int64_t x = sp.X;
		vector<Point> pointList;
		//现在走过的十字路口，每一个边界，我们交叉，找到最初的交叉点和退出点。然后添加所有的点之间
		// 给点链表和继续下一个边界我们会交叉，直至不再有界限，交叉。
		// 这给出了一个路径，从开始到结束弯曲周围的洞，它遇到。
		while(true)
		{
			unsigned int n = getPolygonAbove(x);
			if (n == NO_INDEX) break;

			pointList.push_back(matrix.unapply(Point(minX[n] - MM2INT(0.2), sp.Y)));
			if ( (minIdx[n] - maxIdx[n] + boundery[n].size()) % boundery[n].size() > (maxIdx[n] - minIdx[n] + boundery[n].size()) % boundery[n].size())
			{
				for(unsigned int i=minIdx[n]; i != maxIdx[n]; i = (i < boundery[n].size() - 1) ? (i + 1) : (0))
				{
					pointList.push_back(getBounderyPointWithOffset(n, i));
				}
			}else{
				if (minIdx[n] == 0)
					minIdx[n] = boundery[n].size() - 1;
				else
					minIdx[n]--;
				if (maxIdx[n] == 0)
					maxIdx[n] = boundery[n].size() - 1;
				else
					maxIdx[n]--;

				for(unsigned int i=minIdx[n]; i != maxIdx[n]; i = (i > 0) ? (i - 1) : (boundery[n].size() - 1))
				{
					pointList.push_back(getBounderyPointWithOffset(n, i));
				}
			}
			pointList.push_back(matrix.unapply(Point(maxX[n] + MM2INT(0.2), sp.Y)));

			x = maxX[n];
		}
		pointList.push_back(endPoint);

		    //优化点链表，跳过每一点我们已经可以达到不越过边界。这消除了道路，使它跳过任何不需要的角落。
		Point p0 = startPoint;
		for(unsigned int n=1; n<pointList.size(); n++)
		{
			if (collisionTest(p0, pointList[n]))
			{
				if (collisionTest(p0, pointList[n-1]))
					return false;
				p0 = pointList[n-1];
				combPoints.push_back(p0);
			}
		}
		if (addEndpoint)
			combPoints.push_back(endPoint);
		return true;
	}

}//namespace Engine
