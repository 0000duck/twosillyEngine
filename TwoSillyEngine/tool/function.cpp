/**

�ļ�����function.cpp

ʱ�䣺2017��1��3��11:33:40
���ݣ����Ǻ���ģ�飡 ��Ҫ����ϡliunxתwindows��������һЩ�Լ�ʵ�ֵ�С����
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/

#include "function.h"

/*
ʱ�䣺2017��1��3��18:22:30
���ݣ��Ƕ�ת����
���ߣ�TwoSilly
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
ʱ�䣺2017��1��3��18:22:30
���ݣ�����ת�Ƕ�
���ߣ�TwoSilly
*/

double RadianToAngle(double Radian_)
{
	return  (Radian_ / M_PI) * 180.0;

}