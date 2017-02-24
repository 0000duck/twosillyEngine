/**********************************************
文件名：floatpoint.h

时间：2016年7月1日 17:26:46
内容：这是一个float点类（三维点 x，y，z），还有一个3x3矩形类！
作者：TwoSilly
E-MAIL：twosilly@foxmail.com
**********************************************/
#ifndef FLOAT_POINT_H
#define FLOAT_POINT_H

/*
在模型加载作为三维向量的浮点三维点被使用。他们代表毫米在三维空间。
*/

#include "intpoint.h"

#include <stdint.h>
#include <math.h>

class FloatPoint3
{
public:
    double x,y,z;
    FloatPoint3() {}
    FloatPoint3(double _x, double _y, double _z): x(_x), y(_y), z(_z) {}
    
    FloatPoint3 operator+(const FloatPoint3& p) const { return FloatPoint3(x+p.x, y+p.y, z+p.z); }
    FloatPoint3 operator-(const FloatPoint3& p) const { return FloatPoint3(x-p.x, y-p.y, z-p.z); }
    FloatPoint3 operator*(const double f) const { return FloatPoint3(x*f, y*f, z*f); }
    FloatPoint3 operator/(const double f) const { return FloatPoint3(x/f, y/f, z/f); }
    
    FloatPoint3& operator += (const FloatPoint3& p) { x += p.x; y += p.y; z += p.z; return *this; }
    FloatPoint3& operator -= (const FloatPoint3& p) { x -= p.x; y -= p.y; z -= p.z; return *this; }
    
    bool operator==(FloatPoint3& p) const { return x==p.x&&y==p.y&&z==p.z; }
    bool operator!=(FloatPoint3& p) const { return x!=p.x||y!=p.y||z!=p.z; }
    
	/*
	时间：2016年7月4日 08:37:03
	内容：max得到3维点中的最大值！
	作者：TwoSilly
	*/
    double Max()
    {
        if (x > y && x > z) return x;
        if (y > z) return y;
        return z;
    }
    
	/*
	时间：2016年7月4日 08:38:07
	内容：判断this点到原点距离的平方是否 小于或等于传入值len
	作者：TwoSilly
	*/
    bool testLength(double len)
    {
        return vSize2() <= len*len;
    }
    
	/*
	时间：2016年7月4日 08:38:07
	内容：this点到原点距离的平方！
	作者：TwoSilly
	*/
    double vSize2()
    {
        return x*x+y*y+z*z;
    }

    /*
	时间：2016年7月4日 08:38:07
	内容：this点到原点的距离！（已开方）
	作者：TwoSilly
	*/
    double vSize()
    {
        return sqrt(vSize2());
    }
};



/************************************************************************/
/* 
时间：2016年7月4日 08:40:34
内容：3x3 矩形类！
作者：TwoSilly
*/
/************************************************************************/
class FloatMatrix3x3
{
public:
    double m[3][3];
    
    FloatMatrix3x3()
    {
        m[0][0] = 1.0;
        m[1][0] = 0.0;
        m[2][0] = 0.0;
        
        m[0][1] = 0.0;
        m[1][1] = 1.0;
        m[2][1] = 0.0;
        
        m[0][2] = 0.0;
        m[1][2] = 0.0;
        m[2][2] = 1.0;
    }
    

	/*
	时间：2016年7月4日 08:58:41
	内容：点p左乘矩阵！单位mm到int（毫米到微米）即：乘1000
	作者：TwoSilly
	*/
    Point3 apply(FloatPoint3 p)
    {
        return Point3(
            static_cast<int32_t>(MM2INT(p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0])),
                
            static_cast<int32_t>(MM2INT(p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1])),
                
            static_cast<int32_t>(MM2INT(p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2])));
    }
};

#endif//INT_POINT_H
