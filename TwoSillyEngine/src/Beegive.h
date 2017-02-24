/*
文件名：Beegive.h
时间：2016年12月30日16:24:34
内容：这是蜂窝创建类，我自己写的，估计会有很多问题，等测试
作者：TwoSilly
*/
#ifndef BEEGIVE_H
#define BEEGIVE_H

#include <vector>
#include <iostream>
#include <math.h>
//#include "../utils/intpoint.h"
#include "../utils/polygon.h"



#define SIN60 sin(M_PI / 3) //sin(60)
#define SIN30 sin(M_PI / 6) //sin(30)
using namespace std;
namespace Engine {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


	//class Point;
	class PolygonRef;
	class Polygon;
	class Polygons;
	//class Point;
	/*
	时间：2016年12月30日11:00:45
	内容：产生正六边形 函数，通过传入点为 第一点生成一个正六边形 边长为第二个参数（默认为6）
	参数：
	Point _min --> 正六边形的起始点
	int side = 6 --> 正六边形的边长
	作者：TwoSilly
	*/
	Polygon CreateBeegive(Point pos, int side);

	/*
	时间：2017年1月3日11:10:29
	内容：产生正六边形 函数，通过传入点为 第一点生成一个正六边形 边长为第二个参数（默认为6）
	参数：
	Point _min --> 正六边形的起始点
	int side = 6 --> 正六边形的边长
	作者：TwoSilly
	*/
	Polygon CreateBeegiveTwo(Point pos, int side);
 
	/*
	时间：2016年12月30日11:00:45
	内容：产生正六边形 链表函数，通过传入 最小最大点生成 一个全 蜂窝填充的矩形
	参数：
	Point min_ --> 正六边形的起始点
	Point maX --> 正六边形的最大边界点
	int side = 6 --> 正六边形的边长
	作者：TwoSilly
	*/
 bool CreateBeegiveList(Polygons &List, Point min_, Point max_, int side);

 /*
 时间：2017年1月3日17:01:01
 内容：是的 ，我前面的算法有问题不完美，这个是改进版,右偏移
 参数：
 Point min_ --> 正六边形的起始点
 Point max_ --> 正六边形的结束点
 int side  --> 正六边形的边长
 int dir --> 向左还是向右偏移 0 向右 1向左
 作者：TwoSilly
 */
 Polygon CreateBeegiveLineRight(Point min_, Point max_, int side);

 /*
 时间：2017年1月3日17:01:01
 内容：是的 ，我前面的算法有问题不完美，这个是改进版,左偏移
 参数：
 Point min_ --> 正六边形的起始点
 Point max_ --> 正六边形的结束点
 int side  --> 正六边形的边长
 int dir --> 向左还是向右偏移 0 向右 1向左
 作者：TwoSilly
 */
 Polygon CreateBeegiveLineLeft(Point min_, Point max_,Point pos, int side);
 /*
 时间：2016年12月30日11:00:45
 内容：产生正六边形 链表函数，通过传入 最小最大点生成 一个全 蜂窝填充的矩形
 参数：
 Point min_ --> 正六边形的起始点
 Point maX --> 正六边形的最大边界点
 int side = 6 --> 正六边形的边长
 作者：TwoSilly
 */
 bool CreateBeegiveLineList(Polygons &List, Point min_, Point max_, int side);


}
#endif //BEEGIVE_H