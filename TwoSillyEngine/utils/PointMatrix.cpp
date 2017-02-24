/************************************************************************/
/* �ļ�����PointMatrix.cpp												*/
/* ʱ�䣺2016��7��1�� 09:50:46                                          */
/* ���ݣ���ֻ��PointMatrix�����д����ԭ���� intPoint.h ͷ�ļ��֮���Ұ�
		��������ˣ����������Ժ��ά����								*/
/* ���ߣ�TwoSilly														*/
/* E-MAIL��twosilly@foxmail.com											*/
/************************************************************************/

#include "PointMatrix.h"
	/**
	ʱ�䣺2016��8��1�� 19:20:03
	���ݣ����Σ�������Ĺ��캯����������һ�����Σ�
		matrix[0] = 1;
		matrix[1] = 0;
		matrix[2] = 0;
		matrix[3] = 1;
	���ߣ�TwoSilly
	*/
PointMatrix::PointMatrix()
{
	matrix[0] = 1;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 1;
}

   PointMatrix::PointMatrix(double rotation)
    {
        rotation = rotation / 180 * M_PI;
        matrix[0] = cos(rotation);
        matrix[1] = -sin(rotation);
        matrix[2] = -matrix[1];
        matrix[3] = matrix[0];
    }

    PointMatrix::PointMatrix(const Point p)
    {
		
        matrix[0] = p.X;
        matrix[1] = p.Y;
        double f = sqrt((matrix[0] * matrix[0]) + (matrix[1] * matrix[1]));
        matrix[0] /= f;
        matrix[1] /= f;
        matrix[2] = -matrix[1];
        matrix[3] = matrix[0];
    }

	/*
	ʱ�䣺2016��7��1�� 09:00:48
	���ݣ���p��˾���
	���ߣ�TwoSilly
	*/
    Point PointMatrix::apply(const Point p) const
    {
        return Point((p.X * matrix[0] + p.Y * matrix[1]),
					 (p.X * matrix[2] + p.Y * matrix[3]) );
    }


	/*
	ʱ�䣺2016��7��1�� 09:00:48
	���ݣ���p�ҳ˾���
	���ߣ�TwoSilly
	*/
    Point PointMatrix::unapply(const Point p) const
    {
        return Point((p.X * matrix[0] + p.Y * matrix[2]),
					 (p.X * matrix[1] + p.Y * matrix[3]));
    }
std::ostream& operator<< (std::ostream &p, PointMatrix &Matrix) //�������غ�����
{

	p<<"["<<Matrix.matrix[0]<<","<<Matrix.matrix[1]<<"]\n"
		<<"["<<Matrix.matrix[2]<<","<<Matrix.matrix[3]<<"]\n";

	return p;
}
