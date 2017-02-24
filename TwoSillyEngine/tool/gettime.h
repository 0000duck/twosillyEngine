/** 
�ļ�����gettime.h

ʱ�䣺2016��7��9�� 15:52:41
���ݣ��õ�ʱ��ģ��
���ߣ�TwoSilly
E-MAIL��TwoSilly@foxmail.com
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
ʱ�䣺2016��7��9�� 16:05:48
���ݣ��õ���ǰϵͳʱ�䣨��ȡ���뼶��Ŀ��
���ߣ�TwoSilly
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


//ʱ���������
class TimeKeeper
{
private:
    double startTime;
public:
    TimeKeeper();
    
    double restart();
};

#endif//GETTIME_H
