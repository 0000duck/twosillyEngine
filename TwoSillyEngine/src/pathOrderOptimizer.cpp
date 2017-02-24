/**

文件名：pathOrderOptimizer.cpp

时间：2016年8月1日 20:10:46
内容：路径，制度; 次序;优化
注：算法啊！坑爹的算法没看，这里面是一个“路径次序优化类”
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/


#include <map>
#include "pathOrderOptimizer.h"
//地址次序优化
namespace Engine {

	/**
	时间：2016年8月1日 20:27:26
	内容：哈希点，根据点生成单独的哈希值
	作者：TwoSilly
	*/
static uint32_t hashPoint(const Point& p)
{
    return (p.X / 20000) ^ (p.Y / 20000) << 8;
}

//void PathOrderOptimizer::optimizeTwo()
//{

//	//进来的垂直的正常范围
//    const float incommingPerpendicularNormalScale = 0.0001f;
    
//	//位置到多边形的map
//    std::map<uint32_t, std::vector<unsigned int>> location_to_polygon_map;

//	//精选的，摘下的
//    std::vector<bool> picked;
//	for (PolygonRef poly : polygons_) /// 找到最近点到初始起点在每个多边形初始化挑选
//	{
//		int best = -1;
//		float bestDist = std::numeric_limits<float>::infinity();

//		//在多边形中得到最接近点
//		for (unsigned int point_idx = 0; point_idx < poly.size(); point_idx++) 
//		{
//			float dist = vSize2f(poly[point_idx] - startPoint_);
//			if (dist < bestDist)
//			{
//				best = point_idx;
//				bestDist = dist;
//			}
//		}
//		polyStart_.push_back(best);
//		picked.push_back(false); 

//		assert(poly.size() != 2);
//	}



//    Point prev_point = startPoint_;
	
//	//实际路径优化顺序
//    for(unsigned int poly_order_idx=0; poly_order_idx<polygons_.size(); poly_order_idx++)
//    {
//        int best_poly_idx = -1;
//        float bestDist = std::numeric_limits<float>::infinity();
        

//		for (unsigned int poly_idx = 0; poly_idx < polygons_.size(); poly_idx++)
//		{
//			/// 跳过单点多边形
//			if (picked[poly_idx] || polygons_[poly_idx].size() < 1) 
//				continue;
			

//			assert (polygons_[poly_idx].size() != 2);

//			float dist = vSize2f(polygons_[poly_idx][polyStart_[poly_idx]] - prev_point);
//			if (dist < bestDist)
//			{
//				best_poly_idx = poly_idx;
//				bestDist = dist;
//			}

//		}



//        //应该始终是正确的，我们应该能够识别最好的下一个多边形 


//		if (best_poly_idx > -1) 
//		{
//			assert(polygons_[best_poly_idx].size() != 2);

//			prev_point = polygons_[best_poly_idx][polyStart_[best_poly_idx]];

//			picked[best_poly_idx] = true;
//			polyOrder_.push_back(best_poly_idx);
//		}
//		else
//		{
//			//logError("未能找到下一个最接近的多边形.\n");
//		}


//    }
    


//	// prev_point = startPoint_;

//	//决定每个多边形的最终起始点
//	for (unsigned int order_idx = 0; order_idx < polyOrder_.size(); order_idx++) /// decide final starting points in each polygon
//	{
//		int poly_idx = polyOrder_[order_idx];
//		int point_idx = getClosestPointInPolygon(prev_point, poly_idx);
//		polyStart_[poly_idx] = point_idx;
//		prev_point = polygons_[poly_idx][point_idx];

//	}


//}

//int PathOrderOptimizer::getClosestPointInPolygon(Point prev_point, int poly_idx)
//{
//	PolygonRef poly = polygons_[poly_idx];

//	int best_point_idx = -1;
//	float best_point_score = std::numeric_limits<float>::infinity();
//	Point p0 = poly.back();
//	for (unsigned int point_idx = 0; point_idx < poly.size(); point_idx++)
//	{
//		Point& p1 = poly[point_idx];
//		Point& p2 = poly[(point_idx + 1) % poly.size()];
//		int64_t dist = vSize2(p1 - prev_point);
//		float is_on_inside_corner_score = -getAngleLeft_(p0, p1, p2) / M_PI * 5000 * 5000; // 喜欢里面的角落
//		// 这个分数是在5毫米的顺序
//		if (dist + is_on_inside_corner_score < best_point_score)
//		{
//			best_point_idx = point_idx;
//			best_point_score = dist + is_on_inside_corner_score;
//		}
//		p0 = p1;
//	}
//	return best_point_idx;
//}


void PathOrderOptimizer::optimize()
{
	//进来的垂直的正常范围
    const double incommingPerpendicularNormalScale = 0.0001;
    
	//位置到多边形的map
    std::map<uint32_t, std::vector<unsigned int>> location_to_polygon_map;

	//精选的，摘下的
    std::vector<bool> picked;

	/*
	时间：2016年12月19日14:06:02
	内容：这里我打算改一下，其实也没什么只是试试c++11新特性怎么玩，洗完会快一些
	作者：TwoSilly
	*/

	//**  找到最近点到初始起点在每个多边形初始化挑选
    for(unsigned int i=0;i<polygons_.size(); i++)
    {
        int best = -1;
		//原 float
        float bestDist = std::numeric_limits<float>::infinity();
        PolygonRef poly = polygons_[i];
		//找出每个多边形中与开始点最接近那个点
        for(unsigned int j=0; j<poly.size(); j++)
        {
			//距离
            float dist = vSize2f(poly[j] - startPoint_);
            if (dist < bestDist)
            {
                best = j;
                bestDist = dist;
            }
        }
        polyStart_.push_back(best);
        picked.push_back(false);
        
        if (poly.size() == 2)
        {
            location_to_polygon_map[hashPoint(poly[0])].push_back(i);
            location_to_polygon_map[hashPoint(poly[1])].push_back(i);
        }
    }
	
	


	//进来的垂直的正常，常态; 标准; [数学]法线;
    Point incommingPerpendicularNormal(0, 0);
    Point prev_point = startPoint_;
	//实际路径优化顺序
    for(unsigned int n=0; n<polygons_.size(); n++)
    {
        int best = -1;
        float bestDist = std::numeric_limits<float>::infinity();
        
       for(unsigned int i : location_to_polygon_map[hashPoint(prev_point)])
        {
            if (picked[i] || polygons_[i].size() < 1)
                continue;

			//每个多边形的第一个点与开始点的距离的差值
            float dist = vSize2f(polygons_[i][0] - prev_point);
            dist += abs(dot(incommingPerpendicularNormal, normal(polygons_[i][1] - polygons_[i][0], 1000))) * incommingPerpendicularNormalScale;
            if (dist < bestDist)
            {
                best = i;
                bestDist = dist;
                polyStart_[i] = 0;
            }
            dist = vSize2f(polygons_[i][1] - prev_point);
            dist += abs(dot(incommingPerpendicularNormal, normal(polygons_[i][0] - polygons_[i][1], 1000))) * incommingPerpendicularNormalScale;
            if (dist < bestDist)
            {
                best = i;
                bestDist = dist;
                polyStart_[i] = 1;
            }
        }
        
        if (best == -1)
        {
            for(unsigned int i=0;i<polygons_.size(); i++)
            {
                if (picked[i] || polygons_[i].size() < 1)
                    continue;
                if (polygons_[i].size() == 2)
                {
                    float dist = vSize2f(polygons_[i][0] - prev_point);
                    dist += abs(dot(incommingPerpendicularNormal, normal(polygons_[i][1] - polygons_[i][0], 1000))) * incommingPerpendicularNormalScale;
                    if (dist < bestDist)
                    {
                        best = i;
                        bestDist = dist;
                        polyStart_[i] = 0;
                    }
                    dist = vSize2f(polygons_[i][1] - prev_point);
                    dist += abs(dot(incommingPerpendicularNormal, normal(polygons_[i][0] - polygons_[i][1], 1000))) * incommingPerpendicularNormalScale;
                    if (dist < bestDist)
                    {
                        best = i;
                        bestDist = dist;
                        polyStart_[i] = 1;
                    }
                }else{
                    float dist = vSize2f(polygons_[i][polyStart_[i]] - prev_point);
                    if (dist < bestDist)
                    {
                        best = i;
                        bestDist = dist;
                    }
                }
            }
        }
        
        if (best > -1)
        {
            if (polygons_[best].size() == 2)
            {
                int endIdx = (polyStart_[best] + 1) % 2;
                prev_point = polygons_[best][endIdx];
                incommingPerpendicularNormal = crossZ(normal(polygons_[best][endIdx] - polygons_[best][polyStart_[best]], 1000));
            }else{
                prev_point = polygons_[best][polyStart_[best]];
                incommingPerpendicularNormal = Point(0, 0);
            }
            picked[best] = true;
            polyOrder_.push_back(best);
        }
    }
    
    //prev_point = startPoint_;
    for(int nr : polyOrder_)
    {
        PolygonRef poly = polygons_[nr];
        if (poly.size() > 2)
        {
            int best = -1;
            float bestDist = std::numeric_limits<float>::infinity();
            bool orientation = poly.orientation();
            for(unsigned int i=0;i<poly.size(); i++)
            {
                const int64_t dot_score_scale = 2000;
                float dist = vSize2f(polygons_[nr][i] - prev_point);
                Point n0 = normal(poly[(i+poly.size()-1)%poly.size()] - poly[i], dot_score_scale);
                Point n1 = normal(poly[i] - poly[(i + 1) % poly.size()], dot_score_scale);
                float dot_score = dot(n0, n1) - dot(crossZ(n0), n1);
                if (orientation)
                    dot_score = -dot_score;
                dist += dot_score;
                if (dist < bestDist)
                {
                    best = i;
                    bestDist = dist;
                }
            }
            polyStart_[nr] = best;
        }
        if (poly.size() <= 2)
        {
            prev_point = poly[(polyStart_[nr] + 1) % 2];
        }else{
            prev_point = poly[polyStart_[nr]];
        }
    }

	
}

}//namespace Engine
