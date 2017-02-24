/***********************************************
*�ļ�����polygon.cpp

*ʱ�䣺2016��6��27�� 17:19:40

*���ݣ����Ƿ�װ��clipper�ľ������Ҵ����������д��---����ʵ���ǰ����ǵ�������ʵ�ַֿ����ѣ�
		���������ҵĴ�����������ֱ��һЩ��
*���ߣ�TwoSilly
*E-MAIL��twosilly@foxmail.com
************************************************/


#include "polygon.h"

namespace Engine {

	
	/**
	ʱ�䣺2016��6��27�� 17:29:24
	���ݣ������� polygonRef ��ʵ��
	���ߣ�TwoSilly
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
		return polygon->data();//����vector�ĵ�һ��Ԫ�صķ��ص�ַ
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

	//����λ
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
	ʱ�䣺2016��6��28�� 10:29:51
	���ݣ�������һ���е����������е㣨���ĵ㣩
	���ߣ�TwoSilly
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
	ʱ�䣺2016��6��28�� 10:49:37
	���ݣ�polygon ����ӽ���p��һ���㣬���ص���polygon�ϵ�һ����
	���ߣ�TwoSilly
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
	��������Ƿ��ڶ�����ڡ�������������ͨ���Ӹ�X�����ϵĵ㣬
	���ǵ�ÿһ�н����������������������������һ��ż���Ľ���㣬��ô���ǲ��ڶ�����ڡ�
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
	ʱ�䣺2016��6��29�� 15:39:26
	���ݣ����������ڲ���ʱ��д�������ûɶ���ã�ֻ�ǻ����polygons����ĵ���б�
	���ߣ�TwoSilly
	*/
	std::ostream& operator<< (std::ostream &p, Polygons &poly) //�������غ�����
	{
		
		for(unsigned int j = 0; j < poly.size(); j++)
		{
			//std::cout<<"poly[0].size():"<<poly[j].size()<<std::endl;
			for (unsigned int i = 0; i < poly.size(); i++)
			{
				p<<poly[j].getPolygon()[i]<<" ";
			}
		}
		/*	p<<"��Ʒ�ǣ�"<<shop.name<<endl;
		p<<"�۸��ǣ�"<<shop.price<<endl;*/
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
	ʱ�䣺2016��6��28�� 13:47:21
	���ݣ�����һ��other �����;����������polygons�ü�other��ʣ�µĲ���
			difference -->��ɫ�����
	���ߣ�TwoSilly
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
 ʱ�䣺2016��6��28�� 14:59:54
 ����:���������this��polygon�Ĳ���
 ���ߣ�TwoSilly
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
	ʱ�䣺2016��6��28�� 16:11:57
	���ݣ���������εĽ�������μ���ĵ�ȥ����������
	���ߣ�TwoSilly
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
	ʱ�䣺2016��6��28�� 16:13:51
	���ݣ�����ƫ��
	���ߣ�TwoSilly
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

	ʱ�䣺2016��6��29�� 16:24:43
	���ݣ�������㼸����һ��ֱ�ߣ����м���Ƴ�
	���ߣ�TwoSilly
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

	//������ܳ�
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
	ʱ�䣺2016��6��29�� 16:45:57
	���ݣ�Ӧ�þ��Σ��Ѵ���ľ��������δ���polygons��
	���ߣ�TwoSilly
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
	ʱ�䣺2016��6��29�� 16:45:57
	���ݣ�Ӧ�þ��Σ��Ѵ���ľ��������δ���polygons��
	���ߣ�TwoSilly
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