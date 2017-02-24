/*******************************************************
*文件名：intpoint.h

*时间：2016年6月27日 16:46:46

*内容：这是一个int型点类主要继承与clipper其实是对他的一个封装
		她只有声明没有实现！因为不需要！这只是对点的一些操作！+ - * /等当然也有3维点
*作者：TwoSilly
*E-MAIL：twosilly@foxmail.com											
*******************************************************/
#ifndef INT_POINT_H
#define INT_POINT_H

/*
在二维或三维空间中，整数点类被使用，并代表微米。
整数点用来避免浮点舍入误差，因为clipperlib使用它们。
*/
#define INLINE static inline

//Include Clipper to get the ClipperLib::IntPoint definition, which we reuse as Point definition.
//包括剪去clipperlib：：intpoint定义，我们用点定义。
#include "../libs/clipper/clipper.hpp"

#include <limits>
#include <stdint.h>
#include <cmath>
#include "../tool/function.h"
//#include "../src/Beegive.h"

//缩小1000.0倍：  微米到毫米
#define INT2MM(n) (double(n) / 1000.0)

//将值放大1000倍：int(毫米)到微米
#define MM2INT(n) (int64_t((n) * 1000))

#define INT2MICRON(n) ((n) / 1)
#define MICRON2INT(n) ((n) * 1)

//由于c++11里面没有了宏M_PI的定义，所以自定义一个
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
时间：2016年8月1日 08:43:47
内容：3维点类
	数据成员：
		int32_t x,y,z;
作者“TwoSilly
*/
class Point3
{
public:
    int32_t x,y,z;
    Point3() {}
    Point3(const int32_t _x, const int32_t _y, const int32_t _z): x(_x), y(_y), z(_z) {}

    Point3 operator+(const Point3& p) const { return Point3(x+p.x, y+p.y, z+p.z); }
    Point3 operator-(const Point3& p) const { return Point3(x-p.x, y-p.y, z-p.z); }
    Point3 operator/(const int32_t i) const { return Point3(x/i, y/i, z/i); }

    Point3& operator += (const Point3& p) { x += p.x; y += p.y; z += p.z; return *this; }
    Point3& operator -= (const Point3& p) { x -= p.x; y -= p.y; z -= p.z; return *this; }

    bool operator==(const Point3& p) const { return x==p.x&&y==p.y&&z==p.z; }
    bool operator!=(const Point3& p) const { return x!=p.x||y!=p.y||z!=p.z; }

    int32_t Max()
    {
        if (x > y && x > z) return x;
        if (y > z) return y;
        return z;
    }
  	/*
	时间：2016年6月30日 09:48:57
	内容：测试传入的长度是否大于或等于 坐标点到原点的距离！
		
	作者：TwoSilly
	
	*/
    bool testLength(int32_t len)
    {
        if (x > len || x < -len)
            return false;
        if (y > len || y < -len)
            return false;
        if (z > len || z < -len)
            return false;
        return vSize2() <= len*len;
    }
	
	/*
	时间：2016年6月30日 09:48:57
	内容：空间坐标点与原点距离 的平方！返回值int64_t（long long）
	作者：TwoSilly
	
	*/
    int64_t vSize2()
    {
        return int64_t(x)*int64_t(x)+int64_t(y)*int64_t(y)+int64_t(z)*int64_t(z);
    }

	/*
	时间：2016年6月30日 09:48:57
	内容：空间坐标点与原点距离
	作者：TwoSilly
	
	*/
    int32_t vSize()
    {
        return static_cast<int32_t>(sqrt(vSize2()));
    }
	/*
	时间：2016年6月30日 10:03:53
	内容：这是向量外积！垂直于两个向量，|a ×b| = |a|·|b|·sin<a,b>.
	外积求夹角
	内积求长度
	作者：TwoSilly
	*/
    Point3 cross(const Point3& p)
    {
        return Point3(
            y*p.z-z*p.y,
            z*p.x-x*p.z,
            x*p.y-y*p.x);
    }
};


/* 64点大多采用的是代码，这些都是从clipperlib二维点 */
typedef ClipperLib::IntPoint Point;
class IntPoint {
public:
	
    int X_, Y_;
    Point p() { return Point(X_, Y_); }

	IntPoint():X_(0),Y_(0){};

	IntPoint(int x,int y)
		:X_(x),Y_(y){/*X=x;Y=y;*/}

	bool operator==(const Point& p0) { return p0.X == this->X_&&p0.Y == this->Y_; }
};
//返回系统允许的signed long long 的最小值
#define POINT_MIN (std::numeric_limits<ClipperLib::cInt>::min)()

//返回系统允许的signed long long 的最大值
#define POINT_MAX (std::numeric_limits<ClipperLib::cInt>::max)()

/* 
	额外的操作者容易与64点对象做数学
*/
INLINE Point operator+(const Point& p0, const Point& p1) { return Point(p0.X+p1.X, p0.Y+p1.Y); }
INLINE Point operator-(const Point& p0, const Point& p1) { return Point(p0.X-p1.X, p0.Y-p1.Y); }
INLINE Point operator*(const Point& p0, const int64_t i) { return Point(p0.X*i, p0.Y*i); }
INLINE Point operator/(const Point& p0, const int64_t i) { return Point(p0.X/i, p0.Y/i); }

//Point& operator += (const Point& p) { x += p.x; y += p.y; return *this; }
//Point& operator -= (const Point& p) { x -= p.x; y -= p.y; return *this; }

//INLINE bool operator==(const Point& p0, const Point& p1) { return p0.X==p1.X&&p0.Y==p1.Y; }
INLINE bool operator!=(const Point& p0, const Point& p1) { return p0.X!=p1.X||p0.Y!=p1.Y; }

/*
时间：2016年6月28日 10:51:52
内容：点po与原点的距离（0,0）的平方
作者：TwoSilly
*/
INLINE int64_t vSize2(const Point& p0)
{
    return p0.X*p0.X+p0.Y*p0.Y;
}

/*
时间：2016年6月28日 10:51:52
内容：点po与原点的距离（0,0）的平方
作者：TwoSilly
*/
INLINE float vSize2f(const Point& p0)
{
    return float(p0.X)*float(p0.X)+float(p0.Y)*float(p0.Y);
}


/*
时间：2016年6月30日 11:29:54
内容：传进来的点到原点的距离是否 小于或等于 len
作者：TwoSilly
*/
INLINE bool shorterThen(const Point& p0, int32_t len)
{
    if (p0.X > len || p0.X < -len)
        return false;
    if (p0.Y > len || p0.Y < -len)
        return false;
    return vSize2(p0) <= len*len;
}
//点与原点 的距离（已开平方）
INLINE int64_t vSize(const Point& p0)
{
    return static_cast<int64_t>(sqrt(vSize2(p0)));
}

/*
时间：2016年6月30日 11:34:17
内容：传入点与原点的距离！单位 mm毫米
作者：TwoSilly
*/
INLINE double vSizeMM(const Point& p0)
{
    double fx = INT2MM(p0.X);
    double fy = INT2MM(p0.Y);
    return sqrt(fx*fx+fy*fy);
}

/*
时间：2016年6月30日 16:21:39
内容：将点同比缩小到len
		列：点（30,40） 我们都知道这个点到原点（0,0）的距离为50
			这个函数就是把这个50缩为len的值! X Y 的比例相同
			假设我输入15那将得到点（9,12）VSzie(点（9,12）) == 15
作者：TwoSilly
*/
INLINE Point normal(const Point& p0, int64_t len)
{
    int64_t _len = vSize(p0);
    if (_len < 1)
        return Point(len, 0);
	//Point pp = p0 * (len / _len);
	return p0 * len / _len;

	//后面这个是不报禁告版！不过我觉得还是有警告好一些！
    //return p0 * static_cast<const int32_t>(len) / static_cast<const int32_t>(_len);
}

/*
时间：2017年1月3日18:01:38
内容：创建点 根据传入的    1，开始点  2，距离  3，角度
	产生下一个点！
	角度分为4个区间（0-360）  90一个区间

作者：TwoSilly
*/
INLINE Point CreatePoint(const Point& p0, int64_t len, int64_t angle)
{
	if (len <= 0){return Point(0,0);}
	if (angle <= 0 || angle > 360){return Point(0, 0);}

	//int y = abs(sin(AngleToRadian(angle))) * len; //y
	//int x = abs(sin(AngleToRadian(90 - angle))) * len; //x

	/*
	时间：2017年1月4日08:41:09
	内容：在0 -90 度范围内的就算是直角三角形
	作者：TwoSilly
	*/
	if (0 == angle || 360 == angle )
	{
		return Point(p0.X + len, p0.Y);
	}
	else if (0 < angle && angle < 90)
	{
		return Point(p0.X + (abs(sin(AngleToRadian(90 - angle))) * len),
					p0.Y + (abs(sin(AngleToRadian(90 - angle))) * len));
	} 
	else if (angle == 90)
	{
		return Point(p0.X,(p0.Y +  len));
	}
	else if ( 90< angle && angle <180 )
	{
		return Point(p0.X - (abs(sin(AngleToRadian(90 - angle))) * len),
						p0.Y + (abs(sin(AngleToRadian(90 - angle))) * len));
	}else if (180 == angle)
	{
		return Point(p0.X - len, p0.Y);
	}else if (180 < angle && angle < 270)
	{
		return Point(p0.X - (abs(sin(AngleToRadian(90 - angle))) * len),
						p0.Y - (abs(sin(AngleToRadian(90 - angle))) * len));
	}
	else if (270 == angle)
	{
		return Point(p0.X, p0.Y - len);
	}else if (270 < angle && angle < 360)
	{
		return Point(p0.X + (abs(sin(AngleToRadian(90 - angle))) * len),
					p0.Y - (abs(sin(AngleToRadian(90 - angle))) * len));
	}
	return p0;

}

/*
时间：2016年6月30日 17:03:19
内容：绕原点逆时针旋转90度
作者：TwoSilly
*/
INLINE Point crossZ(const Point& p0)
{
    return Point(-p0.Y, p0.X);
}


/*
时间：2016年6月30日 17:19:37
内容：向量内积-->a*b == |a|*|b|cos c(a,b为向量，  c为两个向量的夹角)
外积求夹角
内积求长度
	注：点积如果为负，则u,v形成的角大于90度；如果为零，那么u,v垂直；
	如果为正，那么u,v形成的角为锐角。
	两个单位向量的点积得到两个向量的夹角的cos值，
	通过它可以知道两个向量的相似性，利用点积可判断一个多边形是否面向摄像机还是背向摄像机。
作者：TwoSilly
*/
INLINE int64_t dot(const Point& p0, const Point& p1)
{
    return p0.X * p1.X + p0.Y * p1.Y;
}


/*
时间：2016年7月1日 08:50:28
内容：返回传入点与x正半轴夹角，角度制
作者：TwoSilly
*/
INLINE double angle(const Point& p)
{
	//atan2(y,x)所表达的意思是坐标原点为起点，
	//指向(x,y)的射线在坐标平面上与x轴正方向之间的角的角度。
    double angles = std::atan2(p.X, p.Y) / M_PI * 180.0;
    if (angles < 0.0) angles += 360.0;
	
    return angles;
}




#endif//INT_POINT_H
