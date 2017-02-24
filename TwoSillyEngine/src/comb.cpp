/**

�ļ�����comb.cpp

ʱ�䣺2016��8��2�� 11:14:36
���ݣ������࣬��ÿ�μ������ƶ��Ĳ�ȱ��
ע���㷨�Ǻǣ�û����
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include "comb.h"

namespace Engine {

		/**
	ʱ�䣺2016��8��2�� 11:56:47
	���ݣ�ǰ׺���
	ע����ʵ����ʲô��û��ֱ�ӵ����ˣ�collisionTest������ײ���
		������
			Point startPoint --> ��ʼ��
			Point endPoint --> ������
	���ߣ�TwoSilly
	*/
	bool Comb::preTest(Point startPoint, Point endPoint)
	{
		return collisionTest(startPoint, endPoint);
	}

	/**
	ʱ�䣺2016��8��2�� 11:56:47
	���ݣ���ײ���
		������
			Point startPoint --> ��ʼ��
			Point endPoint --> ������
	���ߣ�TwoSilly
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
	ʱ�䣺2016��8��2�� 13:31:40
	���ݣ�������С���ֵ
		
	���ߣ�TwoSilly
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
	ʱ�䣺2016��8��2�� 13:32:42
	���ݣ��õ����������
		������
			int64_t x
	ע���������������ɶ�ò��󶮰���
	���ߣ�TwoSilly
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
	ʱ�䣺2016��8��2�� 13:45:58
	���ݣ�ƫ�Ƶõ��߽��
		������
			unsigned int polygonNr --> ����ε�������Χ
			unsigned int idx --> ����
	���ߣ�TwoSilly
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
	ʱ�䣺2016��8��2�� 13:55:52
	���ݣ��ƶ����ڲ���
		������
			Point* p --> ��
			int distance = 100 --> ����ȱʡ 100
			
	���ߣ�TwoSilly
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
		//�������������ڲ�����״�߽�
		if (!boundery.inside(startPoint))
		{
			if (!moveInside(&startPoint))     //������ǲ��ܰѵ��ᵽ�߽���������Ҫ�ջء�
				return false;
			combPoints.push_back(startPoint);
		}
		if (!boundery.inside(endPoint))
		{
			if (!moveInside(&endPoint))    //������ǲ��ܰѵ��ᵽ�߽���������Ҫ�ջ�.
				return false;
			addEndpoint = true;
		}

		 //�������ǿ�Խ�κα߽磬��Ԥ����һЩֵ.
		if (!preTest(startPoint, endPoint))
		{
			//����û�п�Խ�κν��ޡ������������ӵ�һ���� 
			if (!addEndpoint && combPoints.size() == 0) //ֻ�����������û���ƶ��Ŀ�ʼ�ͽ�����.
				return true;
		}

		// //�������Ǵ������α߽����С�����λ��
		calcMinMax();

		int64_t x = sp.X;
		vector<Point> pointList;
		//�����߹���ʮ��·�ڣ�ÿһ���߽磬���ǽ��棬�ҵ�����Ľ������˳��㡣Ȼ��������еĵ�֮��
		// ��������ͼ�����һ���߽����ǻύ�棬ֱ�������н��ޣ����档
		// �������һ��·�����ӿ�ʼ������������Χ�Ķ�����������
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

		    //�Ż�����������ÿһ�������Ѿ����Դﵽ��Խ���߽硣�������˵�·��ʹ�������κβ���Ҫ�Ľ��䡣
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
