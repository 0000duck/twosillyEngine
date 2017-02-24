/**
�ļ�����polygonOptimizer.h

ʱ�䣺2016��7��13�� 09:38:09
���ݣ��Ż������
���ߣ�TwoSilly
E-Mail:twosilly@foxmail.com
*/
#ifndef POLYGON_OPTIMIZER_H
#define POLYGON_OPTIMIZER_H

#include "../utils/polygon.h"

namespace Engine {

	/**
	ʱ�䣺2016��7��13�� 09:40:01
	���ݣ��Ż������
	-----������
	--------PolygonRef poly --> �������Ļ��������װ�˶Զ���ε����л�������
	���ߣ�TwoSilly
	*/
void optimizePolygon(PolygonRef poly);


	/**
	ʱ�䣺2016��7��13�� 10:18:02
	���ݣ��Ż������
	-----������
	--------Polygons& polys --> ������࣬����PolygonRef����Ԫ��
	���ߣ�TwoSilly
	*/
void optimizePolygons(Polygons& polys);

}//namespace Engine

#endif//POLYGON_OPTIMIZER_H
