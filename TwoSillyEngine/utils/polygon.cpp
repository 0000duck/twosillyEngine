/***********************************************
*文件名：polygon.cpp

*时间：2016年6月27日 17:19:40

*内容：这是封装了clipper的矩形类我打算把他们重写！---》其实就是把他们的声明和实现分开而已！
		这样！让我的代码跑起来更直观一些！
*作者：TwoSilly
*E-MAIL：twosilly@foxmail.com
************************************************/


#include "polygon.h"

namespace Engine {

	
	/**
	时间：2016年6月27日 17:29:24
	内容：这是类 polygonRef 的实现
	作者：TwoSilly
	**/
//--------------------------------PolygonRef-------------------------------------------------------------
	/*PolygonRef:: PolygonRef()
					 : polygon(nullptr) {}
		*/			

	PolygonRef:: PolygonRef(ClipperLib::Path& polygon)
					: polygon(&polygon) {}
					
	//PolygonRef::PolygonRef(Polygon& polygon_)
	//	: polygon(&polygon_.GetPoly()) {}

	unsigned int PolygonRef::size() const
    {
        return polygon->size();
    }
	
	/*Point PolygonRef::operator[] (unsigned int index) const
    {
        POLY_ASSERT(index < size());
        return (*polygon)[index];
    }*/

    void* PolygonRef::data()
   	{
		return polygon->data();//这是vector的第一个元素的返回地址
	}

    void PolygonRef::add(const Point p)
    {
        polygon->push_back(p);
    }

    void PolygonRef::remove(unsigned int index)
    {
        POLY_ASSERT(index < size());
        polygon->erase(polygon->begin() + index);
    }



    void PolygonRef::clear()
    {
        polygon->clear();
    }

	//方向定位
    bool PolygonRef::orientation() const
    {
        return ClipperLib::Orientation(*polygon);
    }

    void PolygonRef::reverse()
    {
        ClipperLib::ReversePath(*polygon);
    }

    int64_t PolygonRef::polygonLength() const
    {
        int64_t length = 0;
        Point p0 = (*polygon)[polygon->size()-1];
        for(unsigned int n=0; n<polygon->size(); n++)
        {
            Point p1 = (*polygon)[n];
            length += vSize(p0 - p1);
            p0 = p1;
        }
        return length;
    }

    double PolygonRef::area() const
    {
        return ClipperLib::Area(*polygon);
    }


	/*
	时间：2016年6月28日 10:29:51
	内容：这是求一序列点所组成面的中点（中心点）
	作者：TwoSilly
	*/
    Point PolygonRef::centerOfMass() const
    {
        double x = 0, y = 0;
        Point p0 = (*polygon)[polygon->size()-1];
        for(unsigned int n=0; n<polygon->size(); n++)
        {
            Point p1 = (*polygon)[n];
            double second_factor = static_cast<double>((p0.X * p1.Y) - (p1.X * p0.Y));

            x += double(p0.X + p1.X) * second_factor;
            y += double(p0.Y + p1.Y) * second_factor;
            p0 = p1;
        }

        double area = Area(*polygon);
        x = x / 6 / area;
        y = y / 6 / area;

        if (x < 0)
        {
            x = -x;
            y = -y;
        }
		//return Pint(x,y);
        return Point(static_cast<ClipperLib::cInt>(x),
					 static_cast<ClipperLib::cInt>(y));
    }

	/*
	时间：2016年6月28日 10:49:37
	内容：polygon 上最接近点p的一个点，返回的是polygon上的一个点
	作者：TwoSilly
	*/
    Point PolygonRef::closestPointTo(Point p)
    {
        Point ret = p;
        float bestDist = FLT_MAX;
        for(unsigned int n=0; n<polygon->size(); n++)
        {
            float dist = vSize2f(p - (*polygon)[n]);
            if (dist < bestDist)
            {
                ret = (*polygon)[n];
                bestDist = dist;
            }
        }
        return ret;
    }
    
    //Check if we are inside the polygon. We do this by tracing from the point towards the negative X direction,
    //  every line we cross increments the crossings counter. If we have an even number of crossings then we are not inside the polygon.

	/*
	检查我们是否在多边形内。我们这样做是通过从负X方向上的点，
	我们的每一行交叉增量交叉点计数器。如果我们有一个偶数的交叉点，那么我们不在多边形内。
	*/
    bool PolygonRef::inside(Point p)
    {
        if (polygon->size() < 1)
            return false;
        
        int crossings = 0;
        Point p0 = (*polygon)[polygon->size()-1];
        for(unsigned int n=0; n<polygon->size(); n++)
        {
            Point p1 = (*polygon)[n];
            
            if ((p0.Y >= p.Y && p1.Y < p.Y) || (p1.Y > p.Y && p0.Y <= p.Y))
            {
                int64_t x = p0.X + (p1.X - p0.X) * (p.Y - p0.Y) / (p1.Y - p0.Y);
                if (x >= p.X)
                    crossings ++;
            }
            p0 = p1;
        }
        return (crossings % 2) == 1;
    }

    ClipperLib::Path::iterator PolygonRef::begin()
    {
        return polygon->begin();
    }

    ClipperLib::Path::iterator PolygonRef::end()
    {
        return polygon->end();
    }

    ClipperLib::Path::const_iterator PolygonRef::begin() const
    {
        return polygon->begin();
    }

    ClipperLib::Path::const_iterator PolygonRef::end() const
    {
        return polygon->end();
    }

//--------------------------------PolygonRef-------------------------------------------------------------

//++++++++++++++++++++++++++++++++Polygons+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*
	时间：2016年6月29日 15:39:26
	内容：这是我用于测试时重写的输出！没啥大用！只是会输出polygons里面的点的列表
	作者：TwoSilly
	*/
	std::ostream& operator<< (std::ostream &p, Polygons &poly) //定义重载函数。
	{
		
		for(unsigned int j = 0; j < poly.size(); j++)
		{
			//std::cout<<"poly[0].size():"<<poly[j].size()<<std::endl;
			for (unsigned int i = 0; i < poly.size(); i++)
			{
				p<<poly[j].getPolygon()[i]<<" ";
			}
		}
		/*	p<<"商品是："<<shop.name<<endl;
		p<<"价格是："<<shop.price<<endl;*/
		return p;
	}

	bool Polygons::CreateBeegiveList(Polygons &List, Point min_, Point max_, int side /*= 6*/)
	{
		if (Engine::CreateBeegiveLineList(List, min_, max_, side))
		{
			return true;
		}
		return false;
		
	}

	unsigned int Polygons::size() const
	{
		return polygons.size();
	}

	//PolygonRef operator[] (unsigned int index);

	void Polygons::remove(unsigned int index)
	{
		POLY_ASSERT(index < size());
		polygons.erase(polygons.begin() + index);
	}

	void Polygons::clear()
	{
		polygons.clear();
	}

	void Polygons::add(const PolygonRef& poly)
	{
		polygons.push_back(*poly.polygon);
	}


	void Polygons::add(const Polygons& other)
	{
		for(unsigned int n=0; n<other.polygons.size(); n++)
			polygons.push_back(other.polygons[n]);
	}

	PolygonRef Polygons::newPoly()
	{
		polygons.push_back(ClipperLib::Path());
		return PolygonRef(polygons[polygons.size()-1]);
	}

	/***********************************************
	时间：2016年6月28日 13:47:21
	内容：返回一个other 多边形;这个多边形是polygons裁剪other所剩下的部分
			difference -->特色，差别
	作者：TwoSilly
	************************************************/
 Polygons Polygons::difference(const Polygons& other) const
    {
        Polygons ret;
        ClipperLib::Clipper clipper(clipper_init);
        clipper.AddPaths(polygons, ClipperLib::ptSubject, true);
        clipper.AddPaths(other.polygons, ClipperLib::ptClip, true);
        clipper.Execute(ClipperLib::ctDifference, ret.polygons);
        return ret;
    }

 /********************************************
 时间：2016年6月28日 14:59:54
 内容:传入矩形与this的polygon的并集
 作者：TwoSilly
 *********************************************/


    Polygons Polygons::unionPolygons(const Polygons& other) const
    {
        Polygons ret;
        ClipperLib::Clipper clipper(clipper_init);
        clipper.AddPaths(polygons, ClipperLib::ptSubject, true);
        clipper.AddPaths(other.polygons, ClipperLib::ptSubject, true);
        clipper.Execute(ClipperLib::ctUnion, ret.polygons, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
        return ret;
    }

	/************************************************************************/
	/* 
	时间：2016年6月28日 16:11:57
	内容：两个多边形的交集（如何计算的得去看看啊！）
	作者：TwoSilly
	*/
	/************************************************************************/
    Polygons Polygons::intersection(const Polygons& other) const
    {
        Polygons ret;
        ClipperLib::Clipper clipper(clipper_init);
        clipper.AddPaths(polygons, ClipperLib::ptSubject, true);
        clipper.AddPaths(other.polygons, ClipperLib::ptClip, true);
        clipper.Execute(ClipperLib::ctIntersection, ret.polygons);
        return ret;
    }

	/************************************************************************/
	/* 
	时间：2016年6月28日 16:13:51
	内容：这是偏移
	作者：TwoSilly
	*/
	/************************************************************************/
    Polygons Polygons::offset(int distance) const
    {
        Polygons ret;
        ClipperLib::ClipperOffset clipper;
        clipper.AddPaths(polygons, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
        clipper.MiterLimit = 2.0;
        clipper.Execute(ret.polygons, distance);
        return ret;
    }

	/*

	时间：2016年6月29日 16:24:43
	内容：如果三点几乎呈一条直线，将中间点移除
	作者：TwoSilly
	*/
    vector<Polygons> Polygons::splitIntoParts(bool unionAll) const
    {
        vector<Polygons> ret;
        ClipperLib::Clipper clipper(clipper_init);
        ClipperLib::PolyTree resultPolyTree;
        clipper.AddPaths(polygons, ClipperLib::ptSubject, true);
        if (unionAll)
            clipper.Execute(ClipperLib::ctUnion, resultPolyTree, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
        else
            clipper.Execute(ClipperLib::ctUnion, resultPolyTree);

        _processPolyTreeNode(&resultPolyTree, ret);
        return ret;
    }

    void Polygons::_processPolyTreeNode(ClipperLib::PolyNode* node, vector<Polygons>& ret) const
    {
        for(int n=0; n<node->ChildCount(); n++)
        {
            ClipperLib::PolyNode* child = node->Childs[n];
            Polygons polygons;
            polygons.add(child->Contour);
            for(int i=0; i<child->ChildCount(); i++)
            {
                polygons.add(child->Childs[i]->Contour);
                _processPolyTreeNode(child->Childs[i], ret);
            }
            ret.push_back(polygons);
        }
    }


    Polygons Polygons::processEvenOdd() const
    {
        Polygons ret;
        ClipperLib::Clipper clipper(clipper_init);
        clipper.AddPaths(polygons, ClipperLib::ptSubject, true);
        clipper.Execute(ClipperLib::ctUnion, ret.polygons);
        return ret;
    }

	//多边形周长
    int64_t Polygons::polygonLength() const
    {
        int64_t length = 0;
        for(unsigned int i=0; i<polygons.size(); i++)
        {
            Point p0 = polygons[i][polygons[i].size()-1];
            for(unsigned int n=0; n<polygons[i].size(); n++)
            {
                Point p1 = polygons[i][n];
                length += vSize(p0 - p1);
                p0 = p1;
            }
        }
        return length;
    }

	/**
	时间：2016年6月29日 16:45:57
	内容：应用矩形！把传入的矩形类依次传入polygons中
	作者：TwoSilly
	**/
    bool Polygons::inside(Point p)
    {
        if (size() < 1)
            return false;
        if (!(*this)[0].inside(p))
            return false;
        for(unsigned int n=1; n<polygons.size(); n++)
        {
            if ((*this)[n].inside(p))
                return false;
        }
        return true;
    }

	/**
	时间：2016年6月29日 16:45:57
	内容：应用矩形！把传入的矩形类依次传入polygons中
	作者：TwoSilly
	**/
    void Polygons::applyMatrix(const PointMatrix& matrix)
    {
        for(unsigned int i=0; i<polygons.size(); i++)
        {
            for(unsigned int j=0; j<polygons[i].size(); j++)
            {
                polygons[i][j] = matrix.apply(polygons[i][j]);
            }
        }
    }

	};
//++++++++++++++++++++++++++++++++Polygons+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++