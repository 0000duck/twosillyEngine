/**

文件名：function.h

时间：2017年1月3日11:33:40
内容：这是函数模块！ 主要放依稀liunx转windows函数！和一些自己实现的小函数
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef FUNCTION_H
#define  FUNCTION_H
#include <stdio.h>
#include <math.h>

//由于c++11里面没有了宏M_PI的定义，所以自定义一个
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
时间：2016年8月2日 14:44:44
内容：四舍五入函数，linux的函数我做了重写
作者：TwoSilly
*/
float roundf(float x);



/*
时间：2017年1月3日18:22:30
内容：角度转弧度
作者：TwoSilly
*/

double AngleToRadian(double angle_);

/*
时间：2017年1月3日18:22:30
内容：角度转弧度
作者：TwoSilly
*/

double RadianToAngle(double Radian_);


#endif //FUNCTION_H