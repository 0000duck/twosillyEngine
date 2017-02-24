/** 
文件名：gettime.cpp

时间：2016年7月9日 15:52:41
内容：得到时间模块
作者：TwoSilly
E-MAIL：TwoSilly@foxmail.com
*/
#include "gettime.h"



TimeKeeper::TimeKeeper()
{
    restart();//重新开始
}
/*
时间从新开始，并返回记录时间点
*/
double TimeKeeper::restart()
{
    double ret = getTime() - startTime;
    startTime = getTime();
    return ret;
}
