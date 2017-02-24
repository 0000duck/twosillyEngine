/**
�ļ�����polygonOptimizer.cpp

ʱ�䣺2016��7��13�� 09:38:09
���ݣ��Ż������
���ߣ�TwoSilly
E-Mail:twosilly@foxmail.com
*/
#include "polygonOptimizer.h"

namespace Engine {


	/**
	ʱ�䣺2016��7��13�� 09:40:01
	���ݣ��Ż������
	�ж϶���������ڵ�������Ĳ�ֵ�Ƿ��� 10΢�� ��
	����Ǿͽ���ǰ��ɾ��������Σ� 
	-----������
	--------PolygonRef poly --> �������Ļ��������װ�˶Զ���ε����л�������
	���ߣ�TwoSilly
	*/
void optimizePolygon(PolygonRef poly)
{
	//��ǰ������е����һ����
    Point p0 = poly[poly.size()-1];

	//������ǰ����ε�ÿ����
    for(unsigned int i=0;i<poly.size();i++)
    {
		//�õ���ǰ������е�һ����
        Point p1 = poly[i];

		//�жϵ�ǰ������Ĳ�ֵ�Ƿ��� 10΢�� ��
		//����Ǿͽ���ǰ��ɾ��������Σ� ���±��һ
        if (shorterThen(p0 - p1, MICRON2INT(10)))
        {
            poly.remove(i);
            i --;
			//�жϵ�ǰ������Ĳ�ֵ�Ƿ�  С�ڻ���� 500΢��
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
	ʱ�䣺2016��7��13�� 10:18:02
	���ݣ��Ż������
	-----������
	--------Polygons& polys --> ������࣬����PolygonRef����Ԫ��
	���ߣ�TwoSilly
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
