/**

�ļ�����function.h

ʱ�䣺2017��1��3��11:33:40
���ݣ����Ǻ���ģ�飡 ��Ҫ����ϡliunxתwindows��������һЩ�Լ�ʵ�ֵ�С����
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef FUNCTION_H
#define  FUNCTION_H
#include <stdio.h>
#include <math.h>

//����c++11����û���˺�M_PI�Ķ��壬�����Զ���һ��
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
ʱ�䣺2016��8��2�� 14:44:44
���ݣ��������뺯����linux�ĺ�����������д
���ߣ�TwoSilly
*/
float roundf(float x);



/*
ʱ�䣺2017��1��3��18:22:30
���ݣ��Ƕ�ת����
���ߣ�TwoSilly
*/

double AngleToRadian(double angle_);

/*
ʱ�䣺2017��1��3��18:22:30
���ݣ��Ƕ�ת����
���ߣ�TwoSilly
*/

double RadianToAngle(double Radian_);


#endif //FUNCTION_H