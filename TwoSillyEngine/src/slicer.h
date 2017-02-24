/**

文件名：slicer.h

时间：2016年7月13日 19:08:37
内容：切片模块
	1.makePolygons（）函数算法有点不明白，
		不过时间快没了,就先用着吧，以后再来研究！
	2.Slicer 类里的函数也只看了个大概！
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef SLICER_H
#define SLICER_H

#include "optimizedModel.h"
#include "../utils/polygon.h"
/*
      切片机从创建优化的3D模型的多边形层。的切片结果是一系列多边形无秩序或结构。
*/
namespace Engine {

	/**
	时间：2016年7月13日 19:09:47
	内容：切片部分
		数据成员：
		Point start, end; //开始结束点
		int faceIndex; //面的索引
		bool addedToPolygon; //添加多边形
		说明：这只是一个简单的数据结构没有实现，其实可以用结构替代他
	作者：TwoSilly
	*/
class SlicerSegment
{
public:
    Point start, end; //开始结束点
    int faceIndex; //面的索引
    bool addedToPolygon; //是否添加多边形
};

	/**
	时间：2016年7月13日 19:09:47
	内容：封闭多边形的结果
		数据成员：
		Point intersectionPoint; //交叉点
		int polygonIdx; //多边形索引
		unsigned int pointIdx; //点的索引
		说明：这只是一个简单的数据结构没有实现，其实可以用结构替代他
			这是他原来的注释：
			//试图在一个封闭的多边形线上找到一个点的结果。这给出了点指数，多边形指数，和连接点。
			//该线的点是pointidx-1奠定和pointidx之间
	作者：TwoSilly
	*/
class closePolygonResult
{
public:
    Point intersectionPoint; //交叉点
    int polygonIdx; //多边形索引
    unsigned int pointIdx; //点的索引
};

	/**
	时间：2016年7月13日 19:09:47
	内容：有缺口的封闭多边形的结果，结果差距拉近
		数据成员：
		int64_t len;  //长度
		int polygonIdx; //多边形的索引
		unsigned int pointIdxA; //点的索引A
		unsigned int pointIdxB; //点的索引B
		bool AtoB; //A向B
		说明：这只是一个简单的数据结构没有实现，其实可以用结构替代他

	作者：TwoSilly
	*/
class gapCloserResult
{
public:
    int64_t len;  //长度
    int polygonIdx; //多边形的索引
    unsigned int pointIdxA; //点的索引A
    unsigned int pointIdxB; //点的索引B
    bool AtoB; //A向B
};

/*
时间：2016年7月26日 17:52:10
内容：这是setget宏 我为了保持类的封装性改变了他的数据成员的级别维私有随意需要她
作者：TwoSilly
*/
#ifndef SETGET_
#define SETGET_(type ,name )\
private:\
	type m_##name;\
public:\
	inline void set##name(type name){\
	this->m_##name = name;\
}\
	inline type get##name(){\
	return m_##name;\
}\

#else
#undef  SETGET_
#define SETGET_(type ,name )\
        private:\
            type m_##name;\
        public:\
            inline void set##name(type name){\
            this->m_##name = name;\
        }\
            inline type get##name(){\
            return m_##name;\
        }\

#endif //SETGET_
/**
时间：2016年7月13日 19:26:10
内容：切片层
	数据成员：
	std::vector<SlicerSegment> segmentList;//分段链表
	std::map<int, int> faceToSegmentIndex;//面和分段 的编号
	int z;	//z
	Polygons polygonList;//多边形链表
	Polygons openPolygons;//打开的多边型
作者：TwoSilly
*/
class SlicerLayer
{
public:
	
    std::vector<SlicerSegment> m_segmentList;//切片的分割链表 vector<SlicerSegment>
    std::map<int, int> m_faceToSegmentIndex;//面和分段 的编号    map<int, int>
    int z_;
    Polygons  polygonList_;
    Polygons openPolygons_;
    /*SETGET_(int, z);	//z
    SETGET_(Polygons, polygonList);//多边形链表 Polygons
    SETGET_( Polygons, openPolygons);//打开的多边型 Polygons*/
    
	/**
	时间：2016年7月13日 19:30:57
	内容：制造多边形
		参数：
			OptimizedVolume* ov --> 优化量
			bool keepNoneClosed --> 保持闭合
			bool extensiveStitching --> 广阔的缝合
	作者：TwoSilly
	*/
    void makePolygons(OptimizedVolume* ov, bool keepNoneClosed, bool extensiveStitching);

private:
	/*
	时间：2016年7月27日 16:13:24
	内容：查找多边形间隙更近
		参数：
			Point ip0
			Point ip1
	作者：TwoSilly
	**/
    gapCloserResult findPolygonGapCloser(Point ip0, Point ip1)
    {
        gapCloserResult ret;
        closePolygonResult c1 = findPolygonPointClosestTo(ip0);
        closePolygonResult c2 = findPolygonPointClosestTo(ip1);
        if (c1.polygonIdx < 0 || c1.polygonIdx != c2.polygonIdx)
        {
            ret.len = -1;
            return ret;
        }
        ret.polygonIdx = c1.polygonIdx;
        ret.pointIdxA = c1.pointIdx;
        ret.pointIdxB = c2.pointIdx;
        ret.AtoB = true;
        
        if (ret.pointIdxA == ret.pointIdxB)
        {
            //连接点在同一线段上。
            ret.len = vSize(ip0 - ip1);
        }else{
            //如果我们有应该从A到B或其他方式.
            Point p0 = polygonList_[ret.polygonIdx][ret.pointIdxA];
            int64_t lenA = vSize(p0 - ip0);
            for(unsigned int i = ret.pointIdxA; i != ret.pointIdxB; i = (i + 1) % polygonList_[ret.polygonIdx].size())
            {
                Point p1 = polygonList_[ret.polygonIdx][i];
                lenA += vSize(p0 - p1);
                p0 = p1;
            }
            lenA += vSize(p0 - ip1);

            p0 = polygonList_[ret.polygonIdx][ret.pointIdxB];
            int64_t lenB = vSize(p0 - ip1);
            for(unsigned int i = ret.pointIdxB; i != ret.pointIdxA; i = (i + 1) % polygonList_[ret.polygonIdx].size())
            {
                Point p1 = polygonList_[ret.polygonIdx][i];
                lenB += vSize(p0 - p1);
                p0 = p1;
            }
            lenB += vSize(p0 - ip0);
            
            if (lenA < lenB)
            {
                ret.AtoB = true;
                ret.len = lenA;
            }else{
                ret.AtoB = false;
                ret.len = lenB;
            }
        }
        return ret;
    }

	/*
	时间：2016年7月27日 16:15:09
	内容：查找最接近的多边形点
	作者：TwoSilly
	*/
    closePolygonResult findPolygonPointClosestTo(Point input)
    {
        closePolygonResult ret;
        for(unsigned int n=0; n<polygonList_.size(); n++)
        {
            Point p0 = polygonList_[n][polygonList_[n].size()-1];
            for(unsigned int i=0; i<polygonList_[n].size(); i++)
            {
                Point p1 = polygonList_[n][i];
                
                //Q = A + Normal( B - A ) * ((( B - A ) dot ( P - A )) / VSize( A - B ));
                Point pDiff = p1 - p0;
                int64_t lineLength = vSize(pDiff);
                if (lineLength > 1)
                {
                    int64_t distOnLine = dot(pDiff, input - p0) / lineLength;
                    if (distOnLine >= 0 && distOnLine <= lineLength)
                    {
                        Point q = p0 + pDiff * distOnLine / lineLength;
                        if (shorterThen(q - input, 100))
                        {
                            ret.intersectionPoint = q;
                            ret.polygonIdx = n;
                            ret.pointIdx = i;
                            return ret;
                        }
                    }
                }
                p0 = p1;
            }
        }
        ret.polygonIdx = -1;
        return ret;
    }
};

/**
时间;2016年7月13日 20:20:54
内容：切片类
	数据成员:
	std::vector<SlicerLayer> layers; --> 切片层链表
	Point3 modelSize, modelMin; --> 模型大小，模型最小点
作者：TwoSilly
*/
class Slicer
{
public:
    std::vector<SlicerLayer> layers; //切片层链表 vector<SlicerLayer>
    Point3 modelSize, modelMin; //模型大小，模型最小点 Point3
    
	/**
	时间：2016年7月13日 20:44:42
	内容：切片类的构造函数，他会吧模型按分好的层切分并留下坐标！集把3d模型转换位每层有层高的2d坐标
		参数：
			OptimizedVolume* ov --> 优化量(模型)
			int32_t initial --> 最初的
			int32_t thickness --> 层厚
			bool keepNoneClosed --> 一直没有关闭
			bool extensiveStitching --> 广泛的拼接
	作者：TwoSilly
	*/
    Slicer(OptimizedVolume* ov, int32_t initial, int32_t thickness, bool keepNoneClosed, bool extensiveStitching);
    
	/**
	时间：2016年7月14日 10:07:13
	内容：2D项目,切片的核心，他的作用是为一个三角面中有一个z轴切片值
		参数：
			Point3& p0 --> 
			Point3& p1 --> 
			Point3& p2 -->
			int32_t z --> 	
		原理：
		过点P,Q的直线的方向向量就是向量PQ,所以设P(x1,y1,z1),Q(x2,y2,z2),直线的方程就是
		(x－x1)/(x2－x1)＝(y－y1)/(y2－y1)＝(z－z1)/(z2－z1)
		 用直线方程算一下即可
	作者：TwoSilly
	*/
    SlicerSegment project2D(Point3& p0, Point3& p1, Point3& p2, int32_t z) const
    {
        SlicerSegment seg;
		//开始的x == 第一个点的x + 第二个点的x - 第一个点的x * z减第一个点的z /第二个点-第一个点
        seg.start.X = p0.x + int64_t(p1.x - p0.x) * int64_t(z - p0.z) / int64_t(p1.z - p0.z);
        
        seg.start.Y = p0.y + int64_t(p1.y - p0.y) * int64_t(z - p0.z) / int64_t(p1.z - p0.z);

        seg.end.X = p0.x + int64_t(p2.x - p0.x) * int64_t(z - p0.z) / int64_t(p2.z - p0.z);
        seg.end.Y = p0.y + int64_t(p2.y - p0.y) * int64_t(z - p0.z) / int64_t(p2.z - p0.z);
        return seg;
    }
    
	/**
	时间：2016年7月27日 16:19:18
	内容：卸下分段到网页
	作者：TwoSilly
	*/
    void dumpSegmentsToHTML(const char* filename);
};

}//namespace Engine

#endif//SLICER_H
