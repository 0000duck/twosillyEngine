/**

�ļ�����comb.cpp

ʱ�䣺2016��8��2�� 11:14:36
���ݣ������࣬��ÿ�μ������ƶ��Ĳ�ȱ��
ע���㷨�Ǻǣ�û����
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include "comb.h"

namespace Engine {

	/**
	ʱ�䣺2016��8��2�� 11:56:47
	���ݣ�ǰ׺���
	ע����ʵ����ʲô��û��ֱ�ӵ����ˣ�collisionTest������ײ���
		������
			Point startPoint --> ��ʼ��
			Point endPoint --> ������
	���ߣ�TwoSilly
	*/
bool Comb::_preTest(Point startPoint, Point endPoint)
{
    return _collisionTest(startPoint, endPoint);
}

	/**
	ʱ�䣺2016��8��2�� 11:56:47
	���ݣ���ײ���
		������
			Point startPoint --> ��ʼ��
			Point endPoint --> ������
	���ߣ�TwoSilly
	*/
bool Comb::_collisionTest(Point startPoint, Point endPoint)
{
    Point diff = endPoint - startPoint;

    _matrix = PointMatrix(diff);
    _startPoint = _matrix.apply(startPoint);
    _endPoint = _matrix.apply(endPoint);
    
    for(unsigned int n=0; n<_boundery.size(); n++)
    {
        if (_boundery[n].size() < 1)
            continue;
        Point p0 = _matrix.apply(_boundery[n][_boundery[n].size()-1]);
        for(unsigned int i=0; i<_boundery[n].size(); i++)
        {
            Point p1 = _matrix.apply(_boundery[n][i]);
            if ((p0.Y > _startPoint.Y && p1.Y < _startPoint.Y) || (p1.Y > _startPoint.Y && p0.Y < _startPoint.Y))
            {
                int64_t x = p0.X + (p1.X - p0.X) * (_startPoint.Y - p0.Y) / (p1.Y - p0.Y);
                
                if (x > _startPoint.X && x < _endPoint.X)
                    return true;
            }
            p0 = p1;
        }
    }
    return false;
}

	/**
	ʱ�䣺2016��8��2�� 13:31:40
	���ݣ�������С���ֵ
		
	���ߣ�TwoSilly
	*/
void Comb::_calcMinMax()
{
    for(unsigned int n=0; n<_boundery.size(); n++)
    {
        _minX[n] = INT64_MAX;
        _maxX[n] = INT64_MIN;
        Point p0 = _matrix.apply(_boundery[n][_boundery[n].size()-1]);
        for(unsigned int i=0; i<_boundery[n].size(); i++)
        {
            Point p1 = _matrix.apply(_boundery[n][i]);
            if ((p0.Y > _startPoint.Y && p1.Y < _startPoint.Y) || (p1.Y > _startPoint.Y && p0.Y < _startPoint.Y))
            {
                int64_t x = p0.X + (p1.X - p0.X) * (_startPoint.Y - p0.Y) / (p1.Y - p0.Y);
                
                if (x >= _startPoint.X && x <= _endPoint.X)
                {
                    if (x < _minX[n]) { _minX[n] = x; _minIdx[n] = i; }
                    if (x > _maxX[n]) { _maxX[n] = x; _maxIdx[n] = i; }
                }
            }
            p0 = p1;
        }
    }
}

   	/**
	ʱ�䣺2016��8��2�� 13:32:42
	���ݣ��õ����������
		������
			int64_t x
	ע���������������ɶ�ò��󶮰���
	���ߣ�TwoSilly
	*/
unsigned int Comb::_getPolygonAbove(int64_t x)
{
    int64_t min = POINT_MAX;
    unsigned int ret = NO_INDEX;
    for(unsigned int n=0; n<_boundery.size(); n++)
    {
        if (_minX[n] > x && _minX[n] < min)
        {
            min = _minX[n];
            ret = n;
        }
    }
    return ret;
}

	/**
	ʱ�䣺2016��8��2�� 13:45:58
	���ݣ�ƫ�Ƶõ��߽��
		������
			unsigned int polygonNr --> ����ε�������Χ
			unsigned int idx --> ����
	���ߣ�TwoSilly
	*/
Point Comb::_getBounderyPointWithOffset(unsigned int polygonNr, unsigned int idx)
{
    Point p0 = _boundery[polygonNr][(idx > 0) ? (idx - 1) : (_boundery[polygonNr].size() - 1)];
    Point p1 = _boundery[polygonNr][idx];
    Point p2 = _boundery[polygonNr][(idx < (_boundery[polygonNr].size() - 1)) ? (idx + 1) : (0)];
    
    Point off0 = crossZ(normal(p1 - p0, MM2INT(1.0)));
    Point off1 = crossZ(normal(p2 - p1, MM2INT(1.0)));
    Point n = normal(off0 + off1, MM2INT(0.2));
    
    return p1 + n;
}

Comb::Comb(Polygons& boundery)
: _boundery(boundery)
{
    _minX = new int64_t[_boundery.size()];
    _maxX = new int64_t[_boundery.size()];
    _minIdx = new unsigned int[_boundery.size()];
    _maxIdx = new unsigned int[_boundery.size()];
}

Comb::~Comb()
{
    delete[] _minX;
    delete[] _maxX;
    delete[] _minIdx;
    delete[] _maxIdx;
}

	/**
	ʱ�䣺2016��8��2�� 13:55:52
	���ݣ��ƶ����ڲ���
		������
			Point* p --> ��
			int distance = 100 --> ����ȱʡ 100
			
	���ߣ�TwoSilly
	*/
bool Comb::moveInside(Point* p, int distance)
{
    Point ret = *p;
    int64_t bestDist = MM2INT(2.0) * MM2INT(2.0);
    for(unsigned int n=0; n<_boundery.size(); n++)
    {
        if (_boundery[n].size() < 1)
            continue;
        Point p0 = _boundery[n][_boundery[n].size()-1];
        for(unsigned int i=0; i<_boundery[n].size(); i++)
        {
            Point p1 = _boundery[n][i];
            
            //Q = A + Normal( B - A ) * ((( B - A ) dot ( P - A )) / VSize( A - B ));
            Point pDiff = p1 - p0;
            int64_t lineLength = vSize(pDiff);
            int64_t distOnLine = dot(pDiff, *p - p0) / lineLength;
            if (distOnLine < 10)
                distOnLine = 10;
            if (distOnLine > lineLength - 10)
                distOnLine = lineLength - 10;
            Point q = p0 + pDiff * distOnLine / lineLength;
            
            int64_t dist = vSize2(q - *p);
            if (dist < bestDist)
            {
                bestDist = dist;
                ret = q + crossZ(normal(p1 - p0, distance));
            }
            
            p0 = p1;
        }
    }
    if (bestDist < MM2INT(2.0) * MM2INT(2.0))
    {
        *p = ret;
        return true;
    }
    return false;
}

bool Comb::calc(Point startPoint, Point endPoint, vector<Point>& combPoints)
{
    if (shorterThen(endPoint - startPoint, MM2INT(1.5)))
        return true;
    
    bool addEndpoint = false;
    //�������������ڲ�����״�߽�
    if (!_boundery.inside(startPoint))
    {
        if (!moveInside(&startPoint))    //������ǲ��ܰѵ��ᵽ�߽���������Ҫ�ջء�
            return false;
        combPoints.push_back(startPoint);
    }
    if (!_boundery.inside(endPoint))
    {
        if (!moveInside(&endPoint))    //������ǲ��ܰѵ��ᵽ�߽���������Ҫ�ջ�.
            return false;
        addEndpoint = true;
    }
    
    //�������ǿ�Խ�κα߽磬��Ԥ����һЩֵ.
    if (!_preTest(startPoint, endPoint))
    {
        //����û�п�Խ�κν��ޡ������������ӵ�һ���� 
        if (!addEndpoint && combPoints.size() == 0) //ֻ�����������û���ƶ��Ŀ�ʼ�ͽ�����.
            return true;
    }
    
    //�������Ǵ������α߽����С�����λ��
    _calcMinMax();
    
    int64_t x = startPoint.X;
    vector<Point> pointList;
    //�����߹���ʮ��·�ڣ�ÿһ���߽磬���ǽ��棬�ҵ�����Ľ������˳��㡣Ȼ��������еĵ�֮��
    // ��������ͼ�����һ���߽����ǻύ�棬ֱ�������н��ޣ����档
    // �������һ��·�����ӿ�ʼ������������Χ�Ķ�����������
    while(true)
    {
        unsigned int n = _getPolygonAbove(x);
        if (n == NO_INDEX) break;
        
        pointList.push_back( _matrix.unapply(Point( _minX[n] - MM2INT(0.2), _startPoint.Y)));
        if ( ( _minIdx[n] -  _maxIdx[n] +  _boundery[n].size()) %  _boundery[n].size() > ( _maxIdx[n] -  _minIdx[n] +  _boundery[n].size()) %  _boundery[n].size())
        {
            for(unsigned int i=_minIdx[n]; i != _maxIdx[n]; i = (i < _boundery[n].size() - 1) ? (i + 1) : (0))
            {
                pointList.push_back(_getBounderyPointWithOffset(n, i));
            }
        }else{
            if (_minIdx[n] == 0)
                _minIdx[n] = _boundery[n].size() - 1;
            else
                _minIdx[n]--;
            if (_maxIdx[n] == 0)
                _maxIdx[n] = _boundery[n].size() - 1;
            else
                _maxIdx[n]--;
            
            for(unsigned int i=_minIdx[n]; i != _maxIdx[n]; i = (i > 0) ? (i - 1) : (_boundery[n].size() - 1))
            {
                pointList.push_back(_getBounderyPointWithOffset(n, i));
            }
        }
        pointList.push_back(_matrix.unapply(Point(_maxX[n] + MM2INT(0.2), startPoint.Y)));
        
        x = _maxX[n];
    }
    pointList.push_back(endPoint);
    
    //�Ż�����������ÿһ�������Ѿ����Դﵽ��Խ���߽硣�������˵�·��ʹ�������κβ���Ҫ�Ľ��䡣
    Point p0 = startPoint;
    for(unsigned int n=1; n<pointList.size(); n++)
    {
        if (_collisionTest(p0, pointList[n]))
        {
            if (_collisionTest(p0, pointList[n-1]))
                return false;
            p0 = pointList[n-1];
            combPoints.push_back(p0);
        }
    }
    if (addEndpoint)
        combPoints.push_back(endPoint);
    return true;
}

}//namespace Engine
