/************************************************************************/
/* 文件名：PointMatrix.cpp												*/
/* 时间：2016年7月1日 09:50:46                                          */
/* 内容：这只是PointMatrix类的重写！他原来在 intPoint.h 头文件里！之里我把
		他提出来了！这样便于以后的维护！								*/
/* 作者：TwoSilly														*/
/* E-MAIL：twosilly@foxmail.com											*/
/************************************************************************/

#include "PointMatrix.h"
	/**
	时间：2016年8月1日 19:20:03
	内容：矩形，矩阵类的构造函数，他构造一个矩形！
		matrix[0] = 1;
		matrix[1] = 0;
		matrix[2] = 0;
		matrix[3] = 1;
	作者：TwoSilly
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
	时间：2016年7月1日 09:00:48
	内容：点p左乘矩阵
	作者：TwoSilly
	*/
    Point PointMatrix::apply(const Point p) const
    {
        return Point((p.X * matrix[0] + p.Y * matrix[1]),
					 (p.X * matrix[2] + p.Y * matrix[3]) );
    }


	/*
	时间：2016年7月1日 09:00:48
	内容：点p右乘矩阵
	作者：TwoSilly
	*/
    Point PointMatrix::unapply(const Point p) const
    {
        return Point((p.X * matrix[0] + p.Y * matrix[2]),
					 (p.X * matrix[1] + p.Y * matrix[3]));
    }
std::ostream& operator<< (std::ostream &p, PointMatrix &Matrix) //定义重载函数。
{

	p<<"["<<Matrix.matrix[0]<<","<<Matrix.matrix[1]<<"]\n"
		<<"["<<Matrix.matrix[2]<<","<<Matrix.matrix[3]<<"]\n";

	return p;
}
