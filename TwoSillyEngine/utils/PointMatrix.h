/************************************************************************/
/* 文件名：PointMatrix.h												*/
/* 时间：2016年7月1日 09:50:46                                           */
/* 内容：这只是PointMatrix类的重写！他原来在 intPoint.h 头文件里！之里我把
		他提出来了！这样便于以后的维护！*/
/* 作者：TwoSilly														*/
/*E-MAIL：twosilly@foxmail.com											*/
/*注：这个类的作用我暂时没找到，得以后再做研究！						*/
/************************************************************************/

#ifndef POINT_MATRIX_H
#define POINT_MATRIX_H

#include "intpoint.h"
/**
时间：2016年8月1日 19:17:35
内容：矩形，矩阵类
	数据成员：
		double matrix[4];
作者：TwoSilly
*/
class PointMatrix
{
public:
    double matrix[4];
	/**
	时间：2016年8月1日 19:20:03
	内容：矩形，矩阵类的构造函数，他构造一个矩形！
		matrix[0] = 1;
		matrix[1] = 0;
		matrix[2] = 0;
		matrix[3] = 1;
	作者：TwoSilly
	*/
    PointMatrix();

	/**
	时间：2016年8月1日 19:20:03
	内容：矩形，矩阵类的代参构造函数，他构造一个矩形！
		参数：
			double rotation --> 旋转
	注：这个旋转有点不理解，向那旋转，转多少？？
	作者：TwoSilly
	*/
    PointMatrix(double rotation);

	/**
	时间：2016年8月1日 19:20:03
	内容：矩形，矩阵类的代参构造函数，他构造一个矩形！
		参数：
			const Point p --> 
	注：这个旋转有点不理解，向那旋转，转多少？？
	作者：TwoSilly
	*/
    PointMatrix(const Point p); 

	/*
	时间：2016年7月1日 09:00:48
	内容：点p左乘矩阵
	作者：TwoSilly
	*/
    Point apply(const Point p) const;

	/*
	时间：2016年7月1日 09:00:48
	内容：点p右乘矩阵
	作者：TwoSilly
	*/
    Point unapply(const Point p) const;
  
	friend std::ostream& operator<< (std::ostream&, PointMatrix&); //将重载函数声明为友元函数

	

};



#endif //POINT_MATRIX_H