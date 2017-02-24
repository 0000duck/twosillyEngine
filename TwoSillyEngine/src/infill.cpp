/**

文件名：infill.cpp

时间：2016年8月1日 14:29:47
内容：填充
注：算法没看，这是填充的模块
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#include "infill.h"

namespace Engine {



/**
时间：2016年8月1日 18:49:04
内容：形成，产生，同轴的，同中心的填充（实心的，全体填充，固体）
	参数：
		Polygons outline --> 多边形，大纲，轮廓
		Polygons& result -->   结果，
		int inset_value --> 插入的价值，
作者：TwoSilly
*/
void generateConcentricInfill(Polygons outline, Polygons& result, int inset_value)
{
    while(outline.size() > 0)
    {
        for (unsigned int polyNr = 0; polyNr < outline.size(); polyNr++)
        {
            PolygonRef r = outline[polyNr];
            result.add(r);
        }
        outline = outline.offset(-inset_value);
    }
}

/**
时间：2016年8月1日 18:49:04
内容：形成，产生，智能的填充（如果行间隔大于挤出机宽度的4倍自动选择网格填充，否则选择行填充）
	参数：
		const Polygons& in_outline --> 大纲
		Polygons& result --> 结果
		int extrusionWidth --> 挤出机宽度
		int lineSpacing --> 行间隔
		int infillOverlap --> 填充的重叠部分
		double rotation --> 旋转
作者：TwoSilly
*/
void generateAutomaticInfill(const Polygons& in_outline, Polygons& result,
                             int extrusionWidth, int lineSpacing,
                             int infillOverlap, double rotation)
{
    if (lineSpacing > extrusionWidth * 4)
    {
        generateGridInfill(in_outline, result, extrusionWidth, lineSpacing,
                           infillOverlap, rotation);
    }
    else
    {
        generateLineInfill(in_outline, result, extrusionWidth, lineSpacing,
                           infillOverlap, rotation);
    }
}

/**
时间：2016年8月1日 18:49:04
内容：形成，产生，网格填充（）
	参数：
		const Polygons& in_outline --> 大纲
		Polygons& result --> 结果
		int extrusionWidth --> 挤出机宽度
		int lineSpacing --> 行间隔
		int infillOverlap --> 填充的重叠部分
		double rotation --> 旋转
作者：TwoSilly
*/
void generateGridInfill(const Polygons& in_outline, Polygons& result,
                        int extrusionWidth, int lineSpacing, int infillOverlap,
                        double rotation)
{
	generateBrokenLineInfill(in_outline, result, extrusionWidth, lineSpacing * 2,
		infillOverlap, rotation);

	generateLineInfill(in_outline, result, extrusionWidth, lineSpacing * 2,
		infillOverlap, rotation + 90);
	/*generateLineInfill(in_outline, result, extrusionWidth, lineSpacing * 2,
		infillOverlap, 150);*/
}

/**
时间：2016年8月1日 19:13:42
内容：比较int64_t --> 
					参数a大于b返回1
					a == b return 0 
					a < b  return -1
	参数：
		const void* a --> 
		const void* b --> 

作者：TwoSilly
*/
int compare_int64_t(const void* a, const void* b)
{
    int64_t n = (*(int64_t*)a) - (*(int64_t*)b);
    if (n < 0) return -1;
    if (n > 0) return 1;
    return 0;
}

/**
时间：2016年8月1日 18:49:04
内容：形成，产生，行填充
	参数：
		const Polygons& in_outline --> 输入的轮廓
		Polygons& result --> 结果
		int extrusionWidth --> 挤出机宽度
		int lineSpacing --> 行间隔
		int infillOverlap --> 填充的重叠部分
		double rotation --> 旋转

注：这里会生成行填充，不过我又没看算法
作者：TwoSilly
*/
void generateLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation)
{
	//轮廓线需要先往外括 一个层厚的值
	Polygons outline = in_outline.offset(extrusionWidth * infillOverlap / 100);
	PointMatrix matrix(rotation);

	outline.applyMatrix(matrix);
	//boundary:边界线
	AxisAlignedBoundaryBox boundary(outline);
	/**
	时间：2016年8月1日 18:02:10
	内容：这里我为了保持封装性，做了更改
	原：boundary.min.X = ((boundary.min.X / lineSpacing) - 1) * lineSpacing;
	作者：TwoSilly
	*/

	boundary.setminX(((boundary.getMinX() / lineSpacing) - 1) * lineSpacing);

	//Engine::logError("Min-X:%f\n",temp);
	/**
   时间：2016年8月7日 10:00:20
   内容：这句出了问题，同样的条件下qt计算出的是2，是因为min.x有问题
   作者：TwoSilly
   */
	//这是需要填充线的数量
	int lineCount = (boundary.getMaxX() - boundary.getMinX() + (lineSpacing - 1)) / lineSpacing;


	vector<vector<int64_t> > cutList;
	for (int n = 0; n < lineCount; n++)
		cutList.push_back(vector<int64_t>());

	for (unsigned int polyNr = 0; polyNr < outline.size(); polyNr++)
	{
		//轮廓线中的最后一个点
		Point p1 = outline[polyNr][outline[polyNr].size() - 1];
		for (unsigned int i = 0; i < outline[polyNr].size(); i++)
		{
			Point p0 = outline[polyNr][i];
			int idx0 = (p0.X - boundary.getmin().X) / lineSpacing;
			int idx1 = (p1.X - boundary.getmin().X) / lineSpacing;
			int64_t xMin = p0.X, xMax = p1.X;
			if (p0.X > p1.X) { xMin = p1.X; xMax = p0.X; }
			if (idx0 > idx1) { int tmp = idx0; idx0 = idx1; idx1 = tmp; }
			for (int idx = idx0; idx <= idx1; idx++)
			{
				int x = (idx * lineSpacing) + boundary.getmin().X + lineSpacing / 2;
				if (x < xMin) continue;
				if (x >= xMax) continue;

				//这是直线方程通过x算取y的值
				/*
						原理：
						过点P,Q的直线的方向向量就是向量PQ,所以设P(x1,y1,z1),Q(x2,y2,z2),直线的方程就是
						(x－x1)/(x2－x1)＝(y－y1)/(y2－y1)＝(z－z1)/(z2－z1)
						用直线方程算一下即可
						*/
				int y = p0.Y + (p1.Y - p0.Y) * (x - p0.X) / (p1.X - p0.X);
				cutList[idx].push_back(y);
			}
			p1 = p0;
		}
	}

	int idx = 0;
	for (int64_t x = boundary.getMinX() + lineSpacing / 2; x < boundary.getMaxX(); x += lineSpacing)
	{
		qsort(cutList[idx].data(), cutList[idx].size(), sizeof(int64_t), compare_int64_t);
		for (unsigned int i = 0; i + 1 < cutList[idx].size(); i += 2)
		{
			if (cutList[idx][i + 1] - cutList[idx][i] < extrusionWidth / 5)
				continue;
			PolygonRef p = result.newPoly();
			p.add(matrix.unapply(Point(x, cutList[idx][i])));
			p.add(matrix.unapply(Point(x, cutList[idx][i + 1])));
		}
		idx += 1;
	}


}
	
/**
时间：2016年8月1日 18:49:04
内容：形成，产生，Z填充(折线填充)
参数：
const Polygons& in_outline --> 输入的轮廓
Polygons& result --> 结果
int extrusionWidth --> 挤出机宽度
int lineSpacing --> 行间隔
int infillOverlap --> 填充的重叠部分
double rotation --> 旋转 注：在蜂窝里 这个参数是没有用的

注：这里会生成行填充，不过我又没看算法
作者：TwoSilly
*/
void generateBrokenLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation)
{
	

	//轮廓线需要先往外括 一个层厚的值
	Polygons outline = in_outline.offset(extrusionWidth * infillOverlap / 100);
	//当传入的边界为0时不需要操作
	if (outline.size() <= 0){ return; }
	//PointMatrix matrix(90);
	//outline.applyMatrix(matrix);
	//int64_t Length = outline.polygonLength();
	//	int temp = outline.polygonLength() / lineSpacing;
	//boundary:边界线
	AxisAlignedBoundaryBox boundary(outline);
	/**
	时间：2016年8月1日 18:02:10
	内容：这里我为了保持封装性，做了更改
	原：boundary.min.X = ((boundary.min.X / lineSpacing) - 1) * lineSpacing;
	作者：TwoSilly
	*/

	boundary.setminX(((boundary.getMinX() / lineSpacing) - 1) * lineSpacing);
	
	 
	CreateBeegiveLineList(result, boundary.getmin(), boundary.getmax(), lineSpacing);

	//optimizePolygons(result);
	result = result.intersection(outline);
	
	//优化一下 他已经有了 没必要再加
	//optimizePolygons(result);
	return ;

	

}


}//namespace Engine
