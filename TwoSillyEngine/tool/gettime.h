/** 
文件名：gettime.h

时间：2016年7月9日 15:52:41
内容：得到时间模块
作者：TwoSilly
E-MAIL：TwoSilly@foxmail.com
*/
#ifndef GETTIME_H
#define GETTIME_H

#ifdef __WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <stddef.h>
#endif

/**
时间：2016年7月9日 16:05:48
内容：得到当前系统时间（获取毫秒级数目）
作者：TwoSilly
*/

static inline double getTime()
{
#ifdef __WIN32
    return double(GetTickCount()) / 1000.0;
#else
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return double(tv.tv_sec) + double(tv.tv_usec) / 1000000.0;
#endif
}


//时间管理者类
class TimeKeeper
{
private:
    double startTime;
public:
    TimeKeeper();
    
    double restart();
};

#endif//GETTIME_H
