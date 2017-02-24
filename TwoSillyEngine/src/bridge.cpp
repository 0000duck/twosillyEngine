/**

文件名：bridge.cpp

时间：2016年7月4日 09:29:54
内容：bridge 桥 起链接作用的东西
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#include "bridge.h"

namespace Engine {

	/**
	时间：2016年7月9日 17:40:47
	内容：桥的角度
		参数：
			Polygons outline --> 多边型 大纲
			SliceLayer* prevLayer --> 切片层 prev 预防层
	作者：TwoSilly
	*/
int bridgeAngle(Polygons outline, SliceLayer* prevLayer)
{
    AxisAlignedBoundaryBox boundaryBox(outline); //边界盒子
    
    /*
        若要检测是否有一个桥，首先计算当前层与上一层的交集
        这给我们的层建立的岛屿
    */
    Polygons islands;//一个孤立的岛
    for(auto prevLayerPart : prevLayer->parts)
    {
        if (!boundaryBox.hit(prevLayerPart.boundaryBox))
            continue;
        
        islands.add(outline.intersection(prevLayerPart.outline));
    }
    if (islands.size() > 5 || islands.size() < 1) //为什么要大于5就不行？
        return -1;    

    //下一个找到2个最大的岛屿，我们休息。
    double area1 = 0;
    double area2 = 0;
    int idx1 = -1;
    int idx2 = -1;
    for(unsigned int n=0; n<islands.size(); n++)
    {
        // 跳过内部的孔
        if (!islands[n].orientation())
            continue;
        double area = fabs(islands[n].area()); //fabs 求绝对值
        if (area > area1)
        {
            if (area1 > area2)
            {
                area2 = area1;
                idx2 = idx1;
            }
            area1 = area;
            idx1 = n;
        }else if (area > area2)
        {
            area2 = area;
            idx2 = n;
        }
    }
    
    if (idx1 < 0 || idx2 < 0)
        return -1;
    
    Point center1 = islands[idx1].centerOfMass();
    Point center2 = islands[idx2].centerOfMass();

    return angle(center2 - center1);
}



float getAngleLeft_(const Point& a, const Point& b, const Point& c)
{
	Point ba = a - b;
	Point bc = c - b;
	int64_t dott = dot(ba, bc); // dot product
	int64_t det = ba.X * bc.Y - ba.Y * bc.X; // determinant
	float angle = -atan2(det, dott); // from -pi to pi
	if (angle >= 0 )
	{
		return angle;
	}
	else 
	{
		return M_PI * 2 + angle;
	}
}

};//namespace Engine

