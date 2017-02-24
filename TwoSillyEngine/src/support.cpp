/**

文件名：support.cpp

时间：2016年7月29日 11:44:56
内容：支撑模块
改动：在私有成员面前都加“_”private,表示私有（数据成员和成员函数都是如此）
注：该模块算法我还不是很理解，不过时间关系先暂时滞留！
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/



/*
时间：2016年12月16日11:09:13
算法：x + y * _storage.gridWidth
			二维下标图：
			(0,0)(1,0)(2,0) -\
			(0,1)(1,1)(2,1)	   - Hight				
			(0,2)(1,2)(2,2) -/
              |    |    |
              \    |    /
				Width
			一维下标图：
			0，1，2
			3，4，5
			6，7，8

			如：下标(1,1)
			1 + 1 * 3 = 4
注：其实是2维转1维算法
作者：TwoSilly

*/
#include "support.h"

namespace Engine {


	/**
	时间：2016年7月29日 11:58:29
	内容：交换，交换两个值，这是一个模版函数
	作者：TwoSilly
	*/
template<typename T> inline void swap(T& p0, T& p1)
{
    T tmp = p0;
    p0 = p1;
    p1 = tmp;
}


	/**
	时间：2016年7月29日 13:29:57
	内容：压缩支撑点，这是为了使用qsort（）函数来创建的函数
		
	作者：TwoSilly
	*/
int cmp_SupportPoint(const void* a, const void* b)
{
    return ((SupportPoint*)a)->z - ((SupportPoint*)b)->z;
}

	/**
	时间：2016年7月29日 11:47:55
	内容：形成支撑格子
		参数：
			SupportStorage& storage --> 支撑的数据储存
			OptimizedModel* om --> 优化后的模型
			int supportAngle --> 支撑角度
			bool supportEverywhere --> 是否所有都支撑
			int supportXYDistance --> 支撑xy的距离
			int supportZDistance --> 支撑z的距离
	作者：TwoSilly
	*/
void generateSupportGrid(SupportStorage& storage, OptimizedModel* om, int supportAngle, bool supportEverywhere, int supportXYDistance, int _supportZDistance)
{
    storage.generated = false;
    if (supportAngle < 0)
        return;
    storage.generated = true;
    
    storage.gridOffset.X = om->vMin_.x;
    storage.gridOffset.Y = om->vMin_.y;
    storage.gridScale = 200;
    storage.gridWidth = (om->modelSize_.x / storage.gridScale) + 1;
    storage.gridHeight = (om->modelSize_.y / storage.gridScale) + 1;
    
    /*
    时间：2016年6月23日 15:38:58
    内容：这里的new 内存太多 导致这里会卡慢 看看可不可以优化
		,其实发现还好
    作者：TwoSilly
    */
    storage.grid = new vector<SupportPoint>[storage.gridWidth * storage.gridHeight];
    storage.angle = supportAngle;
    storage.everywhere = supportEverywhere;//是否到处都支撑
    storage.XYDistance = supportXYDistance;
    storage.ZDistance = _supportZDistance;

    for(unsigned int volumeIdx = 0; volumeIdx < om->volumes_.size(); volumeIdx++)
    {
        OptimizedVolume* vol = &om->volumes_[volumeIdx];
        for(unsigned int faceIdx = 0; faceIdx < vol->faces.size(); faceIdx++)
        {
            OptimizedFace* face = &vol->faces[faceIdx];
            Point3 v0 = vol->points[face->index[0]].p;
            Point3 v1 = vol->points[face->index[1]].p;
            Point3 v2 = vol->points[face->index[2]].p;
            
			//法向量：这个是计算出 改三角面的法向量
            Point3 normal = (v1 - v0).cross(v2 - v0);

			//空间坐标点与原点的距离
            int32_t normalSize = normal.vSize();
            
			/*    余弦角 */    /*  fabs（）函数 求绝对值*/
            double _cosAngle = fabs(double(normal.z) / double(normalSize));
            
			/*
			时间：2016年12月17日16:38:13
			内容：当前点与模型最小点的差值 在缩小 一定比例尺
			作者：TwoSilly
			*/
            v0.x = (v0.x - storage.gridOffset.X) / storage.gridScale;
            v0.y = (v0.y - storage.gridOffset.Y) / storage.gridScale;
            v1.x = (v1.x - storage.gridOffset.X) / storage.gridScale;
            v1.y = (v1.y - storage.gridOffset.Y) / storage.gridScale;
            v2.x = (v2.x - storage.gridOffset.X) / storage.gridScale;
            v2.y = (v2.y - storage.gridOffset.Y) / storage.gridScale;

			/*
			时间：2016年12月17日17:48:29
			内容：将三个点排序，按从小到大排序
					即：最后结果：v0.x < v1.x < v2.x
			作者：TwoSilly
			*/			
            if (v0.x > v1.x) swap(v0, v1);
            if (v1.x > v2.x) swap(v1, v2);
            if (v0.x > v1.x) swap(v0, v1);

		    /**/
            for(int64_t x=v0.x; x<v1.x; x++)
            {
                int64_t y0 = v0.y + (v1.y - v0.y) * (x - v0.x) / (v1.x - v0.x);
                int64_t y1 = v0.y + (v2.y - v0.y) * (x - v0.x) / (v2.x - v0.x);
                int64_t z0 = v0.z + (v1.z - v0.z) * (x - v0.x) / (v1.x - v0.x);
                int64_t z1 = v0.z + (v2.z - v0.z) * (x - v0.x) / (v2.x - v0.x);

                if (y0 > y1) { swap(y0, y1); swap(z0, z1); }
                for(int64_t y=y0; y<y1; y++)
                    storage.grid[x+y*storage.gridWidth].push_back(SupportPoint(z0 + (z1 - z0) * (y-y0) / (y1-y0), _cosAngle));
            }
            for(int64_t x=v1.x; x<v2.x; x++)
            {
                int64_t y0 = v1.y + (v2.y - v1.y) * (x - v1.x) / (v2.x - v1.x);
                int64_t y1 = v0.y + (v2.y - v0.y) * (x - v0.x) / (v2.x - v0.x);
                int64_t z0 = v1.z + (v2.z - v1.z) * (x - v1.x) / (v2.x - v1.x);
                int64_t z1 = v0.z + (v2.z - v0.z) * (x - v0.x) / (v2.x - v0.x);

                if (y0 > y1) { swap(y0, y1); swap(z0, z1); }
                for(int64_t y=y0; y<y1; y++)
                    storage.grid[x+y*storage.gridWidth].push_back(SupportPoint(z0 + (z1 - z0) * (y-y0) / (y1-y0), _cosAngle));
            }
        }
    }
    
    for(int32_t x=0; x<storage.gridWidth; x++)
    {
        for(int32_t y=0; y<storage.gridHeight; y++)
        {
            unsigned int n = x+y*storage.gridWidth;
            qsort(storage.grid[n].data(), storage.grid[n].size(), sizeof(SupportPoint), cmp_SupportPoint);
        }
    }
    storage.gridOffset.X += storage.gridScale / 2;
    storage.gridOffset.Y += storage.gridScale / 2;
}

	/**
	时间：2016年7月29日 15:05:47
	内容：是否需要支撑在某点
	参数：
		Point p
	作者：TwoSilly
	*/
bool SupportPolyGenerator::_needSupportAt(Point p)
{
    if (p.X < 1) return false;
    if (p.Y < 1) return false;
    if (p.X >= _storage.gridWidth - 1) return false;
    if (p.Y >= _storage.gridHeight - 1) return false;
	/**
	时间：2016年7月29日 16:40:57
	内容：这里我做了更改，他原来是做完判断在赋值，我先赋值在做判断，这样可以节约一点点计算
	原来：    if (_done[p.X + p.Y * _storage.gridWidth]) return false;

				unsigned int n = p.X+p.Y*_storage.gridWidth;
	作者：TwoSilly
	*/
	//
	 unsigned int n = p.X+p.Y*_storage.gridWidth;
    if (_done[n]) return false;
    
   
    
    if (_everywhere)
    {
        bool ok = false;
        for(unsigned int i=0; i<_storage.grid[n].size(); i+=2)
        {
            if (_storage.grid[n][i].cosAngle >= _cosAngle && _storage.grid[n][i].z - _supportZDistance >= _z && (i == 0 || _storage.grid[n][i-1].z + _supportZDistance < _z))
            {
                ok = true;
                break;
            }
        }
        if (!ok) return false;
    }else{
        if (_storage.grid[n].size() < 1) return false;
        if (_storage.grid[n][0].cosAngle < _cosAngle) return false;
        if (_storage.grid[n][0].z - _supportZDistance < _z) return false;
    }
    return true;
}

	/**
	时间：2016年7月29日 15:09:28
	内容：懒惰的填充
	参数：
		Point startPoint
	作者：TwoSilly
	*/
void SupportPolyGenerator::_lazyFill(Point startPoint)
{
    static int nr = 0; //静态成员变量 用来计数
    nr++;
    PolygonRef poly = polygons_.newPoly();
    Polygon tmpPoly;

    while(true)
    {
        Point p = startPoint;
        _done[p.X + p.Y * _storage.gridWidth] = nr;
        while(_needSupportAt(p + Point(1, 0)))
        {
            p.X ++;
            _done[p.X + p.Y * _storage.gridWidth] = nr;
        }
        tmpPoly.add(startPoint * _storage.gridScale + _storage.gridOffset - Point(_storage.gridScale/2, 0));
        poly.add(p * _storage.gridScale + _storage.gridOffset);
        startPoint.Y++;
        while(!_needSupportAt(startPoint) && startPoint.X <= p.X)
            startPoint.X ++;

        if (startPoint.X > p.X)
        {
            for(unsigned int n=0;n<tmpPoly.size();n++)
            {
                poly.add(tmpPoly[tmpPoly.size()-n-1]);
            }
            polygons_.add(poly);
            return;
        }
        while(_needSupportAt(startPoint - Point(1, 0)) && startPoint.X > 1)
            startPoint.X --;
    }
}
    

	/**
	时间：2016年7月29日 17:45:35
	内容：产生支撑多边形的构造函数
	参数：
		SupportStorage& storage //支撑的储存
		int32_t z //z
	作者：TwoSilly
	*/
SupportPolyGenerator::SupportPolyGenerator(SupportStorage& storage, int32_t z)
: _storage(storage), _z(z), _everywhere(storage.everywhere)
{
    if (!_storage.generated)
        return;
    
    _cosAngle = cos(double(90 - _storage.angle) / 180.0 * M_PI) - 0.01;

    this->_supportZDistance = _storage.ZDistance;

    _done = new int[_storage.gridWidth*_storage.gridHeight];

	/**
	void *memset(void *s, int ch, size_t n);
	函数解释：将 s 中后 n 个字节 （typedef unsigned int size_t）用 ch 替换并返回 s 。
	*/
	//memset，是计算机语言中的函数。起功能是将s所指向的某一块内存中的每个字节的内容全部设置为ch指定的ASCII值
    memset(_done, 0, sizeof(int) * _storage.gridWidth*_storage.gridHeight);
    
    for(int32_t y=1; y<_storage.gridHeight; y++)
    {
        for(int32_t x=1; x<_storage.gridWidth; x++)
        {
			/*
			时间：2016年12月16日09:48:48
			内容：下面这句的意思是，判断当前点是否需要 添加支撑， 不需要进入下一个
				||
			内容：下面这句的_done[x + y * _storage.gridWidth] 里面的东西的 

				算法：x + y * _storage.gridWidth
					二维下标图：
					(0,0)(1,0)(2,0) -\
					(0,1)(1,1)(2,1)	   - Hight				
					(0,2)(1,2)(2,2) -/
					  |    |    |
				      \    |    /
						 Width
					一维下标图：
					0，1，2
					3，4，5
					6，7，8
					如：下标(1,1)
						1 + 1 * 3 = 4
				注：其实是2维转1维算法
			作者：TwoSilly
			*/

            if (!_needSupportAt(Point(x, y)) || _done[x + y * _storage.gridWidth]) continue;
            
            _lazyFill(Point(x, y));
        }
    }

    delete[] _done;
    
    polygons_ = polygons_.offset(_storage.XYDistance);
}

}//namespace Engine
