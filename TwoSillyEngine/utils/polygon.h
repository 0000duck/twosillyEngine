
/***********************************************

*文件名：polygon.h

*时间：2016年6月27日 17:19:40

*内容：这是封装了clipper的矩形类我打算把他们重写！---》其实就是把他们的声明和实现分开而已！
		这样！让我的代码跑起来更直观一些！
*作者：TwoSilly
*E-MAIL：twosilly@foxmail.com
************************************************/


#ifndef UTILS_POLYGON_H
#define UTILS_POLYGON_H

#include <iostream>
#include <vector>
#include <assert.h>
#include <float.h>
using std::vector;
#include "../libs/clipper/clipper.hpp"

#include "intpoint.h"
#include "PointMatrix.h"

//我加的蜂窝填充
#include "../src/Beegive.h"

//检查多边形的断言
//#define CHECK_POLY_ACCESS
#ifdef CHECK_POLY_ACCESS
#define POLY_ASSERT(e) assert(e)
#else
#define POLY_ASSERT(e) do {} while(0)
#endif

/*
时间：2017年1月3日18:19:53
内容;这是我加的sin 30度 和sin60度 宏，注意如果需要其他值（角度）需要更改
作者：TwoSilly
*/

#ifdef SIN60
#define SIN60 sin(M_PI / 3) //sin(60)
#endif
#ifdef SIN30
#define SIN30 sin(M_PI / 6) //sin(30)
#endif


namespace Engine {

//#include "../libs/clipper/clipper.hpp"

const static int clipper_init = (0);

//返回系统允许的unsigned int的最大值
#define NO_INDEX (std::numeric_limits<unsigned int>::max())

/**
时间：2016年7月13日 09:42:08
内容：多边形类的基类,他有一切对多边形的操作，封装于clipper
-----数据成员：
-------------ClipperLib::Path* polygon; 即：typedef std::vector< IntPoint > Path;
作者：TwoSilly
*/
class Polygon;
class PolygonRef
{
    ClipperLib::Path* polygon;

    PolygonRef()
		 : polygon(nullptr) {};//多边形基础类的无参构造为polygon赋一个空值nullptr
 public:
	  ClipperLib::Path* getPolygon()
	  {
		return polygon;
	  }
	 //多边形基础类的一个构造函数！参数是 Clipper::Path& 即-->vector< IntPoint >&
    PolygonRef(ClipperLib::Path& polygon);
	//PolygonRef(Polygon& polygon_);
	//得到的是 polugon链表的长度 
    unsigned int size() const;

	
    Point operator[] (unsigned int index) const
	{
		POLY_ASSERT(index < size());
		return (*polygon)[index];
	}

	//这里返回的是 vector的data（）即-->vector 的第一个元素的返回地址！
	 void* data();//{	 return polygon->data();	 }


	//添加即polygon的push_back（）
    void add(const Point p);

	//即vector erase（）删除
    void remove(unsigned int index);

	//即vector clear 
    void clear();

	//方向定位！（是否存在面积里面会进行点序列的面积计算然后判断他是否大于0）
    bool orientation() const;
  

	//反转即函数std::reverse()反转vector链表p.begin(), p.end()
    void reverse(); 
	
	//矩形周长
	int64_t polygonLength() const;

	//计算点序列的面积
    double area() const;



	ClipperLib::Path::reference back() const
	{
		return polygon->back();
	}
	/*
	时间：2016年6月28日 10:29:51
	内容：这是求一序列点所组成面的中点（中心点）
	作者：TwoSilly
	*/
    Point centerOfMass() const;
   	/*
	时间：2016年6月28日 10:49:37
	内容：polygon 上最接近点p的一个点，返回的是polygon上的一个点
	作者：TwoSilly
	*/
    Point closestPointTo(Point p);

  
	/*
	检查我们是否在多边形内。我们这样做是通过从负X方向上的点，
	我们的每一行交叉增量交叉点计数器。如果我们有一个偶数的交叉点，那么我们不在多边形内。
	*/
    bool inside(Point p);
  
    ClipperLib::Path::iterator begin();
 
    ClipperLib::Path::iterator end();
  

    ClipperLib::Path::const_iterator begin() const;
  

    ClipperLib::Path::const_iterator end() const;


    friend class Polygons;
    friend class Polygon;
};
/*
时间：2016年6月27日 17:36:49
内容：多边形类，继承于 PolygonRef，并且他是PolygonRef的友元类
作者：TwoSilly
*/
class Polygon : public PolygonRef
{
    ClipperLib::Path poly;
public:
    Polygon()
    : PolygonRef(poly)
    {
    }

    Polygon(const PolygonRef& other)
    : PolygonRef(poly)
    {
        poly = *other.polygon;
    }

	////拷贝构造
	//Polygon(Polygon & polygon_)
	//{
	//	ClipperLib::Path poly_;
	//	poly_ = polygon_.poly;
	//	poly = poly_;
	//}
	unsigned int Polygon::size() const
	{
		return poly.size();
	}

	Point operator[] (unsigned int index) const
	{
		POLY_ASSERT(index < size());
		return (poly)[index];
	}
	ClipperLib::Path GetPoly()
	{
		return poly;
	}
	//Polygon& operator=(const Polygon& other) { poly = other.poly; return *this; }
};

/*
时间：2016年6月27日 17:36:49
内容：多边形类，他是PolygonRef的友元类
----数据成员：
------------ClipperLib::Paths polygons;
作者：TwoSilly
*/

class Polygons
{
private:
    ClipperLib::Paths polygons;
public:

	bool CreateBeegiveList(Polygons &List, Point min_, Point max_, int side);
	
	ClipperLib::Paths getPolyhons()
	{
		
	   return polygons;
	
	}
	Polygons() {}
	Polygons(const Polygons& other) { polygons = other.polygons; }

	//得到多边形的大小（拥有多少个点）
    unsigned int size() const;
 



    PolygonRef operator[] (unsigned int index)
	{
		POLY_ASSERT(index < size());
		return PolygonRef(polygons[index]);
	}
	PolygonRef back()
	{
		return PolygonRef(polygons.back());
	}

	//删除指定位置的点
    void remove(unsigned int index);

	//清空多边形链表
    void clear();

	//添加一个多边形
    void add(const PolygonRef& poly);
	
	//同时添加多个多边形
    void add(const Polygons& other);

	//构建一个新的空的std::vector< IntPoint > Path;链表并把它push到this中再把他的值返回出来
    PolygonRef newPoly();
 
	friend std::ostream& operator<< (std::ostream&, Polygons&); //将重载函数声明为友元函数
	

    Polygons& operator=(const Polygons& other) { polygons = other.polygons; return *this; }


	/***********************************************
	时间：2016年6月28日 13:47:21
	内容：返回一个other 多边形;这个多边形是polygons裁剪other所剩下的部分
			difference -->特色
	作者：TwoSilly
	************************************************/
    Polygons difference(const Polygons& other) const;


	 /********************************************
	 时间：2016年6月28日 14:59:54
	 内容:传入矩形与this的polygon的并集
	 作者：TwoSilly
	 *********************************************/

    Polygons unionPolygons(const Polygons& other) const;


	/************************************************************************/
	/* 
	时间：2016年6月28日 16:11:57
	内容：两个多边形的交集（如何计算的得去看看啊！）
	作者：TwoSilly
	*/
	/************************************************************************/
    Polygons intersection(const Polygons& other) const;
 
	/****
	函数名：offset()
	时间：2016年6月29日 10:57:49
	内容：把整个多边形偏移distance个距离！正值把矩形放大！负值把矩形同比缩小
	作者：TwoSilly
	
	****/
    Polygons offset(int distance) const;

	/*

	时间：2016年6月29日 16:24:43
	内容：如果三点几乎呈一条直线，将中间点移除
	作者：TwoSilly
	*/
	vector<Polygons> splitIntoParts(bool unionAll = false) const;

private:
	//过程聚合树节点
    void _processPolyTreeNode(ClipperLib::PolyNode* node, vector<Polygons>& ret) const;

public:
	//奇偶数过程
    Polygons processEvenOdd() const;
  

	//多边形周长
    int64_t polygonLength() const;
  
	//点 p 是否在区域内。返回是否、或者 border_result（即在边界上）
    bool inside(Point p);

	/**
	时间：2016年6月29日 16:45:57
	内容：应用矩形！把传入的矩形类依次传入polygons中
	作者：TwoSilly
	**/
	void applyMatrix(const PointMatrix& matrix);

};


/**
时间：2016年8月7日 10:27:03
内容：这是为了使设置setget的准确性，我引入了clipper模块头文件！只为了用cInt
作者：TwoSilly
*/
//#include "../libs/clipper/clipper.hpp"
#ifdef use_int32
typedef int cInt;
typedef unsigned int cUInt;
#else
typedef signed long long cInt;
typedef unsigned long long cUInt;
#endif

#define SETGET_(type, name)\
private:\
	type m_##name;\
public:\
	void set##name(type name){\
			this->m_##name = name;\
		}\
	 type get##name(){\
			return m_##name;\
		}\
	void set##name##X(cInt _x){\
		this->m_##name.X = _x;\
	}\
	void set##name##Y(cInt _y){\
		this->m_##name.Y = _y;\
	}\

/* Axis aligned boundary box 轴对齐的边界框 */

/*
时间：2016年6月27日 16:44:20
内容：这是轴对齐的边界框类
作者：TwoSilly
*/
class AxisAlignedBoundaryBox 
{ //axis 轴 Aligned 对齐 Boundar 边界
public:
   // Point m_min, m_max;

	SETGET_(Point, min);
	SETGET_(Point, max);
	//void SetMinX()

	/*
	时间：2016年12月14日10:06:05
	内容：类型的强制转换 ，
		原：return this->m_max.X;
		一下同理
	原因：去掉烦人的类型转换警告警告
	报错信息： warning C4244: “return”: 从“ClipperLib::cInt”转换到“double”，可能丢失数据
		
	*/
	double getMaxX()
	{
		return static_cast<double>(this->m_max.X);
	}
	double getMaxY()
	{
		return static_cast<double>(this->m_max.Y);
	}
	double getMinX()
	{
		return static_cast<double>(this->m_min.X);
	}
	double getMinY()
	{
		return static_cast<double>(this->m_min.Y);
	}
    AxisAlignedBoundaryBox()
    : m_min(POINT_MIN, POINT_MIN), m_max(POINT_MIN, POINT_MIN)
    {
    }

	/**
	时间：2016年7月9日 17:51:45
	内容：轴对称多边形的代参构造函数，他在里面计算了 传入矩形的 最小 最大 值
		参数：
			Polygons polys --> 多边形
	作者：TwoSilly
	*/
    AxisAlignedBoundaryBox(Polygons polys)
    : m_min(POINT_MIN, POINT_MIN), m_max(POINT_MIN, POINT_MIN)
    {
        calculate(polys);
    }

	/**
	时间：2016年7月9日 17:51:45
	内容：计算找出传入多边形的 最大 和 最小 点
		参数：
			Polygons polys --> 多边形类
	作者：TwoSilly
	*/
    void calculate(Polygons polys)
    {
        m_min = Point(POINT_MAX, POINT_MAX);
        m_max = Point(POINT_MIN, POINT_MIN);
        for(unsigned int i=0; i<polys.size(); i++)
        {
            for(unsigned int j=0; j<polys[i].size(); j++)
            {
                if (m_min.X > polys[i][j].X) m_min.X = polys[i][j].X;
                if (m_min.Y > polys[i][j].Y) m_min.Y = polys[i][j].Y;
                if (m_max.X < polys[i][j].X) m_max.X = polys[i][j].X;
                if (m_max.Y < polys[i][j].Y) m_max.Y = polys[i][j].Y;
            }
        }
    }

	/**
	时间：2016年7月9日 17:51:45
	内容：判断两个轴对称盒子是否碰撞
		参数：
			const AxisAlignedBoundaryBox& other --> 另一个轴对称盒子
	作者：TwoSilly
	*/
    bool hit(const AxisAlignedBoundaryBox& other) const
    {
        if (m_max.X < other.m_min.X) return false;
        if (m_min.X > other.m_max.X) return false;

        if (m_max.Y < other.m_min.Y) return false;
        if (m_min.Y > other.m_max.Y) return false;
        return true;
    }
};

};//namespace Engine

#endif//UTILS_POLYGON_H
