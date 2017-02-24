/**

�ļ�����comb.h

ʱ�䣺2016��8��2�� 11:14:36
���ݣ������࣬��ÿ�μ������ƶ��Ĳ�ȱ��
ע���㷨�Ǻǣ�û����
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/
#ifndef COMB_H
#define COMB_H

#include "../utils/polygon.h"

namespace Engine {

/**
ʱ�䣺2016��8��2�� 11:15:53
���ݣ�����
	���ݳ�Ա��
		Polygons& boundery;//��

		int64_t* minX;//��С��x
		int64_t* maxX;//����x
		unsigned int* minIdx;//��С��idx
		unsigned int* maxIdx;//����idx

		PointMatrix matrix;//����
		Point startPoint;//��ʼ��
		Point endPoint;//������

���ߣ�TwoSilly
*/
	class Comb
	{
	private:
		Polygons& boundery;//��

		int64_t* minX;//��С��X
		int64_t* maxX;//����X
		unsigned int* minIdx;//��Сֵ���±�
		unsigned int* maxIdx;//���ֵ���±�

		PointMatrix matrix;//����
		Point sp;//��ʼ��ӡ��
		Point ep;//������ӡ��

		/**
		ʱ�䣺2016��8��2�� 11:56:47
		���ݣ�ǰ׺���
		ע����ʵ����ʲô��û��ֱ�ӵ����ˣ�collisionTest������ײ���
			������
				Point startPoint --> ��ʼ��
				Point endPoint --> ������
		���ߣ�TwoSilly
		*/
		bool preTest(Point startPoint, Point endPoint); 

		/**
		ʱ�䣺2016��8��2�� 11:56:47
		���ݣ���ײ���
			������
				Point startPoint --> ��ʼ��
				Point endPoint --> ������
		���ߣ�TwoSilly
		*/
		bool collisionTest(Point startPoint, Point endPoint);

		/**
		ʱ�䣺2016��8��2�� 13:31:40
		���ݣ�������С���ֵ
		
		���ߣ�TwoSilly
		*/
		void calcMinMax();
   		/**
		ʱ�䣺2016��8��2�� 13:32:42
		���ݣ��õ����������
			������
				int64_t x
		ע���������������ɶ�ò��󶮰���
		���ߣ�TwoSilly
		*/
		unsigned int getPolygonAbove(int64_t x);

	  /**
		ʱ�䣺2016��8��2�� 13:45:58
		���ݣ�ƫ�Ƶõ��߽��
			������
				unsigned int polygonNr --> ����ε�������Χ
				unsigned int idx --> ����
		���ߣ�TwoSilly
		*/
		Point getBounderyPointWithOffset(unsigned int polygonNr, unsigned int idx);

	public:
		/**
		ʱ�䣺2016��8��2�� 13:53:29
		���ݣ�comb���ι��캯��
			������
				Polygons& _boundery --> ����ε�������Χ
			
		���ߣ�TwoSilly
		*/
		Comb(Polygons& _boundery);
		~Comb();

		/**
		ʱ�䣺2016��8��2�� 13:55:52
		���ݣ��ڲ��ģ����Ƿ��ڱ߽��ϣ���߽���
			������
				const Point p --> ��
			
		���ߣ�TwoSilly
		*/
		bool inside(const Point p) { return boundery.inside(p); }

		/**
		ʱ�䣺2016��8��2�� 13:55:52
		���ݣ��ƶ����ڲ���
			������
				Point* p --> ��
				int distance = 100 --> ����ȱʡ 100
			
		���ߣ�TwoSilly
		*/
		bool moveInside(Point* p, int distance = 100);
		/**
		ʱ�䣺2016��8��2�� 14:09:49
		���ݣ����㣬
			������
				Point startPoint --> ��ʼ��
				Point startPoint --> ������
				vector<Point>& combPoints --> ���������			
		���ߣ�TwoSilly
		*/
		bool calc(Point startPoint, Point endPoint, vector<Point>& combPoints);
	};

}//namespace Engine

#endif//COMB_H
