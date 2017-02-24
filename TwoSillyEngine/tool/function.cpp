/**

文件名：function.cpp

时间：2017年1月3日11:33:40
内容：这是函数模块！ 主要放依稀liunx转windows函数！和一些自己实现的小函数
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#include "function.h"

/*
时间：2017年1月3日18:22:30
内容：角度转弧度
作者：TwoSilly
*/

double AngleToRadian(double angle_)
{
	return  (angle_ / 180.0) * M_PI;

}
float roundf(float x)
{
	return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
/*
时间：2017年1月3日18:22:30
内容：弧度转角度
作者：TwoSilly
*/

double RadianToAngle(double Radian_)
{
	return  (Radian_ / M_PI) * 180.0;

}