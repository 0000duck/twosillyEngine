/**

�ļ�����pathOrderOptimizer.h

ʱ�䣺2016��8��1�� 20:10:46
���ݣ�·�����ƶ�; ����;�Ż�����������һ����·�������Ż��ࡱ
ע���㷨�����ӵ����㷨û��
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef PATHOPTIMIZER_H
#define PATHOPTIMIZER_H

#include <stdint.h>
#include "../utils/polygon.h"
#include "bridge.h"
namespace Engine {

	/**
	ʱ�䣺2016��8��1�� 19:46:29
	���ݣ�·�������Ż�
		���ݳ�Ա��
			Point startPoint; --> ��ʼ��
			vector<PolygonRef> polygons; --> ���������
			vector<int> polyStart; -->  ����ο�ʼ��
			vector<int> polyOrder; --> ����� ����; ����; �����ƶ�; ����
	���ߣ�TwoSilly
	*/
class PathOrderOptimizer
{
public:
    Point startPoint_;//��ʼ��
    vector<PolygonRef> polygons_;//���������
    vector<int> polyStart_;//����ο�ʼ��
    vector<int> polyOrder_;//����� ����; ����; �����ƶ�; ����

	/**
	ʱ�䣺2016��8��2�� 10:52:10
	���ݣ�·�������Ż���Ĺ��캯��
		������
			Point startPoint --> ��ʼ��
	ע�����Ĺ��캯��ֻ��ֵ�ˣ�startPoint�㣡
	���ߣ�TwoSilly
	*/
    PathOrderOptimizer(Point startPoint)
    {
        this->startPoint_ = startPoint;
    }

	/**
	ʱ�䣺2016��8��2�� 10:52:10
	���ݣ���Ӷ���Σ�
		������
			PolygonRef polygon --> 
	ע��ֱ�ӰѶ����ѹ����������
	���ߣ�TwoSilly
	*/
    void addPolygon(PolygonRef polygon)
    {
        this->polygons_.push_back(polygon);
    }

    /**
	ʱ�䣺2016��8��2�� 10:52:10
	���ݣ���Ӷ���Σ�
		������
			Polygons polygons --> 
	ע���Ѷ���θ�����ֳ�polygon�򵥶������ȫ��ѹ��this�Ķ��������
	���ߣ�TwoSilly
	*/
    void addPolygons(Polygons& polygons)
    {
        for(unsigned int i=0;i<polygons.size(); i++)
            this->polygons_.push_back(polygons[i]);
    }
    
    /**
	ʱ�䣺2016��8��2�� 10:52:10
	���ݣ��Ż���
		������
	ע��
	���ߣ�TwoSilly
	*/
    void optimize();
	//void optimizeTwo();
	//int getClosestPointInPolygon(Point prev_point, int poly_idx);
};

}//namespace Engine

#endif//PATHOPTIMIZER_H
