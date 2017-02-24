/************************************************************************/
/* �ļ�����PointMatrix.h												*/
/* ʱ�䣺2016��7��1�� 09:50:46                                           */
/* ���ݣ���ֻ��PointMatrix�����д����ԭ���� intPoint.h ͷ�ļ��֮���Ұ�
		��������ˣ����������Ժ��ά����*/
/* ���ߣ�TwoSilly														*/
/*E-MAIL��twosilly@foxmail.com											*/
/*ע����������������ʱû�ҵ������Ժ������о���						*/
/************************************************************************/

#ifndef POINT_MATRIX_H
#define POINT_MATRIX_H

#include "intpoint.h"
/**
ʱ�䣺2016��8��1�� 19:17:35
���ݣ����Σ�������
	���ݳ�Ա��
		double matrix[4];
���ߣ�TwoSilly
*/
class PointMatrix
{
public:
    double matrix[4];
	/**
	ʱ�䣺2016��8��1�� 19:20:03
	���ݣ����Σ�������Ĺ��캯����������һ�����Σ�
		matrix[0] = 1;
		matrix[1] = 0;
		matrix[2] = 0;
		matrix[3] = 1;
	���ߣ�TwoSilly
	*/
    PointMatrix();

	/**
	ʱ�䣺2016��8��1�� 19:20:03
	���ݣ����Σ�������Ĵ��ι��캯����������һ�����Σ�
		������
			double rotation --> ��ת
	ע�������ת�е㲻��⣬������ת��ת���٣���
	���ߣ�TwoSilly
	*/
    PointMatrix(double rotation);

	/**
	ʱ�䣺2016��8��1�� 19:20:03
	���ݣ����Σ�������Ĵ��ι��캯����������һ�����Σ�
		������
			const Point p --> 
	ע�������ת�е㲻��⣬������ת��ת���٣���
	���ߣ�TwoSilly
	*/
    PointMatrix(const Point p); 

	/*
	ʱ�䣺2016��7��1�� 09:00:48
	���ݣ���p��˾���
	���ߣ�TwoSilly
	*/
    Point apply(const Point p) const;

	/*
	ʱ�䣺2016��7��1�� 09:00:48
	���ݣ���p�ҳ˾���
	���ߣ�TwoSilly
	*/
    Point unapply(const Point p) const;
  
	friend std::ostream& operator<< (std::ostream&, PointMatrix&); //�����غ�������Ϊ��Ԫ����

	

};



#endif //POINT_MATRIX_H