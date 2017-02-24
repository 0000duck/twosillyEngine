/**
*文件名：string.h
*时间：2016年7月4日 09:11:05
*内容：这是自定义的字符串比较函数，该类没有.cpp文件
*作者：TwoSilly
*E-MAIL：twosilly@foxmail.com	
*/
#ifndef STRING_H
#define STRING_H

//c++11 no longer supplies a strcasecmp, so define our own version.
//C 11不再供应strcasecmp，所以定义自己的版本。
/*
小于零		第一个子字符串在排序顺序中位于第二个子字符串之前。
零			子字符串出现在同一位置的排序顺序或 length 为零。（两个字符串相同或者为空）
大于零		第一个子字符串在排序顺序中位于第二个子字符串之后。
*/
static inline int stringcasecompare(const char* a, const char* b)
{
    while(*a && *b)
    {
        if (tolower(*a) != tolower(*b))
            return tolower(*a) - tolower(*b);
        a++;
        b++;
    }
    return *a - *b;
}

#endif//STRING_H
