/**

文件名：slicer.cpp

时间：2016年7月13日 19:08:37
内容：切片模块
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include <stdio.h>
#include <algorithm>/*我加的因为std：：max报错*/
#include "../tool/gettime.h"
#include "../tool/logoutput.h"

#include "slicer.h"
#include "polygonOptimizer.h"

namespace Engine {
	
	/**
	时间：2016年7月13日 19:30:57
	内容：制造多边形，
		他会把传入的层的多边形做一个优化整合！是之成为比较软滑的多边形路径
		参数：
			OptimizedVolume* ov --> 优化量
			bool keepNoneClosed --> 保持闭合
			bool extensiveStitching --> 广阔的缝合
	作者：TwoSilly
	*/
void SlicerLayer::makePolygons(OptimizedVolume* ov, bool keepNoneClosed, bool extensiveStitching)
{
	//构造一个打开的多边形链表
    Polygons openPolygonList;
    
	//遍历切片分段链表
    for(unsigned int startSegment=0; startSegment < this->m_segmentList.size(); startSegment++)
    {
		//判断切片的分段列表是否添加多边形
        if (m_segmentList[startSegment].addedToPolygon)
            continue;
        //构造一个多边形
        Polygon poly;
		//把当前切片分段的开始点压入多边形中
        poly.add(m_segmentList[startSegment].start);
        
		//记录当前分段的下标
        unsigned int segmentIndex = startSegment;
        //是否可以关闭
		bool canClose;
		
		
        while(true)
        {
			//不可以关闭
            canClose = false;
			
            m_segmentList[segmentIndex].addedToPolygon = true;
			//找出当前分段的结束点并压入多边形
            Point p0 = m_segmentList[segmentIndex].end;
            poly.add(p0);

            int nextIndex = -1;
			
			//当前面切片的分割链表所在的面
            OptimizedFace* face = &ov->faces[m_segmentList[segmentIndex].faceIndex];
			
			//找出多边形是否闭合，并拆分开
            for(unsigned int i=0;i<3;i++)
            {
                if (face->touching[i] > -1 && m_faceToSegmentIndex.find(face->touching[i]) != m_faceToSegmentIndex.end())
                {
                    Point p1 = m_segmentList[m_faceToSegmentIndex[face->touching[i]]].start;
                    Point diff = p0 - p1;
                    if (shorterThen(diff, MM2INT(0.01)))
                    {
                        if (m_faceToSegmentIndex[face->touching[i]] == static_cast<int>(startSegment))
                            canClose = true;
                        if (m_segmentList[m_faceToSegmentIndex[face->touching[i]]].addedToPolygon)
                            continue;
                        nextIndex = m_faceToSegmentIndex[face->touching[i]];
                    }
                }
            }
            if (nextIndex == -1)
                break;
            segmentIndex = nextIndex;
        }
        if (canClose)
            polygonList_.add(poly); //闭合的
        else
            openPolygonList.add(poly); //未闭合的
    }


    //清除段列表以保存内存，这一点之后就不再需要了。
    m_segmentList.clear();

  
    //连接的多边形还没有关闭，因为模型并不总是完美的流形，我们需要加入一些东西，以获得适当的多边形
    //首先连接多边形的两端，在2微米以内。
    for(unsigned int i=0;i<openPolygonList.size();i++)
    {
        if (openPolygonList[i].size() < 1) continue;
        for(unsigned int j=0;j<openPolygonList.size();j++)
        {
            if (openPolygonList[j].size() < 1) continue;
            
            Point diff = openPolygonList[i][openPolygonList[i].size()-1] - openPolygonList[j][0];
            int64_t distSquared = vSize2(diff);

            if (distSquared < MM2INT(0.02) * MM2INT(0.02))
            {
                if (i == j)
                {
                    polygonList_.add(openPolygonList[i]);
                    openPolygonList[i].clear();
                    break;
                }else{
                    for(unsigned int n=0; n<openPolygonList[j].size(); n++)
                        openPolygonList[i].add(openPolygonList[j][n]);

                    openPolygonList[j].clear();
                }
            }
        }
    }

   
   //下一个链接所有失踪的结束，关闭最小的差距第一。这是一个低效的实现，它可以运行在O（n * n）的时间。
    while(true)
    {
		//最好的得分
        int64_t bestScore = MM2INT(10.0) * MM2INT(10.0);
        unsigned int bestA = -1; //最好的a
        unsigned int bestB = -1; //最好的b
        bool reversed = false; //反转
		//遍历开口的多边形链表
        for(unsigned int i=0;i<openPolygonList.size();i++)
        {
            if (openPolygonList[i].size() < 1) continue;
            for(unsigned int j=0;j<openPolygonList.size();j++)
            {
                if (openPolygonList[j].size() < 1) continue;
                
                Point diff = openPolygonList[i][openPolygonList[i].size()-1] - openPolygonList[j][0];
                int64_t distSquared = vSize2(diff);
                if (distSquared < bestScore)
                {
                    bestScore = distSquared;
                    bestA = i;
                    bestB = j;
                    reversed = false;
                }

                if (i != j)
                {
                    Point diff = openPolygonList[i][openPolygonList[i].size()-1] - openPolygonList[j][openPolygonList[j].size()-1];
                    int64_t distSquared = vSize2(diff);
                    if (distSquared < bestScore)
                    {
                        bestScore = distSquared;
                        bestA = i;
                        bestB = j;
                        reversed = true;
                    }
                }
            }
        }
        
        if (bestScore >= MM2INT(10.0) * MM2INT(10.0))
            break;
        
        if (bestA == bestB)
        {
            polygonList_.add(openPolygonList[bestA]);
            openPolygonList[bestA].clear();
        }else{
            if (reversed)
            {
                if (openPolygonList[bestA].polygonLength() > openPolygonList[bestB].polygonLength())
                {
                    for(unsigned int n=openPolygonList[bestB].size()-1; int(n)>=0; n--)
                        openPolygonList[bestA].add(openPolygonList[bestB][n]);
                    openPolygonList[bestB].clear();
                }else{
                    for(unsigned int n=openPolygonList[bestA].size()-1; int(n)>=0; n--)
                        openPolygonList[bestB].add(openPolygonList[bestA][n]);
                    openPolygonList[bestA].clear();
                }
            }else{
                for(unsigned int n=0; n<openPolygonList[bestB].size(); n++)
                    openPolygonList[bestA].add(openPolygonList[bestB][n]);
                openPolygonList[bestB].clear();
            }
        }
    }

    if (extensiveStitching)
    {

        /*
        对于广泛的拼接发现2个开放的多边形，是触摸2个封闭的多边形。
        然后找到最简短的路径在这个多边形，可以用来连接开放的多边形，
        并生成一个路径，在这个最短的位连接起来的2个开放的多边形
        （如果这2个开放的多边形是相同的多边形，然后最终的结果是一个封闭的类别）
        */
        while(true)
        {
            unsigned int bestA = -1;
            unsigned int bestB = -1;
            gapCloserResult bestResult;
            bestResult.len = POINT_MAX;
            bestResult.polygonIdx = -1;
            bestResult.pointIdxA = -1;
            bestResult.pointIdxB = -1;
            
            for(unsigned int i=0; i<openPolygonList.size(); i++)
            {
                if (openPolygonList[i].size() < 1) continue;
                
                {
                    gapCloserResult res = findPolygonGapCloser(openPolygonList[i][0], openPolygonList[i][openPolygonList[i].size()-1]);
                    if (res.len > 0 && res.len < bestResult.len)
                    {
                        bestA = i;
                        bestB = i;
                        bestResult = res;
                    }
                }

                for(unsigned int j=0; j<openPolygonList.size(); j++)
                {
                    if (openPolygonList[j].size() < 1 || i == j) continue;
                    
                    gapCloserResult res = findPolygonGapCloser(openPolygonList[i][0], openPolygonList[j][openPolygonList[j].size()-1]);
                    if (res.len > 0 && res.len < bestResult.len)
                    {
                        bestA = i;
                        bestB = j;
                        bestResult = res;
                    }
                }
            }
            
            if (bestResult.len < POINT_MAX)
            {
                if (bestA == bestB)
                {
                    if (bestResult.pointIdxA == bestResult.pointIdxB)
                    {
                        polygonList_.add(openPolygonList[bestA]);
                        openPolygonList[bestA].clear();
                    }
                    else if (bestResult.AtoB)
                    {
                        PolygonRef poly = polygonList_.newPoly();
                        for(unsigned int j = bestResult.pointIdxA; j != bestResult.pointIdxB; j = (j + 1) % polygonList_[bestResult.polygonIdx].size())
                            poly.add(polygonList_[bestResult.polygonIdx][j]);
                        for(unsigned int j = openPolygonList[bestA].size() - 1; int(j) >= 0; j--)
                            poly.add(openPolygonList[bestA][j]);
                        openPolygonList[bestA].clear();
                    }
                    else
                    {
                        unsigned int n = polygonList_.size();
                        polygonList_.add(openPolygonList[bestA]);
                        for(unsigned int j = bestResult.pointIdxB; j != bestResult.pointIdxA; j = (j + 1) % polygonList_[bestResult.polygonIdx].size())
                            polygonList_[n].add(polygonList_[bestResult.polygonIdx][j]);
                        openPolygonList[bestA].clear();
                    }
                }
                else
                {
                    if (bestResult.pointIdxA == bestResult.pointIdxB)
                    {
                        for(unsigned int n=0; n<openPolygonList[bestA].size(); n++)
                            openPolygonList[bestB].add(openPolygonList[bestA][n]);
                        openPolygonList[bestA].clear();
                    }
                    else if (bestResult.AtoB)
                    {
                        Polygon poly;
                        for(unsigned int n = bestResult.pointIdxA; n != bestResult.pointIdxB; n = (n + 1) % polygonList_[bestResult.polygonIdx].size())
                            poly.add(polygonList_[bestResult.polygonIdx][n]);
                        for(unsigned int n=poly.size()-1;int(n) >= 0; n--)
                            openPolygonList[bestB].add(poly[n]);
                        for(unsigned int n=0; n<openPolygonList[bestA].size(); n++)
                            openPolygonList[bestB].add(openPolygonList[bestA][n]);
                        openPolygonList[bestA].clear();
                    }
                    else
                    {
                        for(unsigned int n = bestResult.pointIdxB; n != bestResult.pointIdxA; n = (n + 1) % polygonList_[bestResult.polygonIdx].size())
                            openPolygonList[bestB].add(polygonList_[bestResult.polygonIdx][n]);
                        for(unsigned int n = openPolygonList[bestA].size() - 1; int(n) >= 0; n--)
                            openPolygonList[bestB].add(openPolygonList[bestA][n]);
                        openPolygonList[bestA].clear();
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    if (keepNoneClosed)
    {
        for(unsigned int n=0; n<openPolygonList.size(); n++)
        {
            if (openPolygonList[n].size() > 0)
                polygonList_.add(openPolygonList[n]);
        }
    }
    for(unsigned int i=0;i<openPolygonList.size();i++)
    {
        if (openPolygonList[i].size() > 0)
            openPolygons_.newPoly() = openPolygonList[i];
    }

   
    //删除所有不关闭的小多边形或多边形。因为他们没有贡献的实际打印。
    int snapDistance = MM2INT(1.0);
    for(unsigned int i=0;i<polygonList_.size();i++)
    {
		/**
		时间：2016年7月13日 16:28:21
		内容：原：int length = 0;我为了改掉烦人的警告放大了他
		作者：TwoSilly
		*/
        int64_t length = 0;
        
        for(unsigned int n=1; n<polygonList_[i].size(); n++)
        {
            length += vSize(polygonList_[i][n] - polygonList_[i][n-1]);
            if (length > snapDistance)
                break;
        }
        if (length < snapDistance)
        {
            polygonList_.remove(i);
            i--;
        }
    }


    //最后优化所有的多边形。从长远来看，每一点都节省了时间。
    optimizePolygons(polygonList_);
}

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
Slicer::Slicer(OptimizedVolume* ov, int32_t initial, int32_t thickness, bool keepNoneClosed, bool extensiveStitching)
{
    modelSize = ov->model->modelSize_; //模型大小
    modelMin = ov->model->vMin_; //模型最小值
    
	//层计数
    int layerCount = (modelSize.z - initial) / thickness + 1;
    Engine::log("Layer count: %i\n", layerCount);
    layers.resize(layerCount);
    
	//计算模型的每层的z轴的大小
    for(int32_t layerNr = 0; layerNr < layerCount; layerNr++)
    {
		/*
		时间：2016年7月26日 17:32:53
		内容：这里我为了保持类的封装性把她原来的公有的数据成员换成了私有的！所以改值得调set函数
		作者：TwoSilly
		*/
        layers[layerNr].z_ =  initial + thickness * layerNr;
    }
    
	//遍历整个模型的面（用于切片）
    for(unsigned int i=0; i<ov->faces.size(); i++)
    {
        Point3 p0 = ov->points[ov->faces[i].index[0]].p;
        Point3 p1 = ov->points[ov->faces[i].index[1]].p;
        Point3 p2 = ov->points[ov->faces[i].index[2]].p;

        int32_t minZ = p0.z;
        int32_t maxZ = p0.z;
        if (p1.z < minZ) minZ = p1.z;
        if (p2.z < minZ) minZ = p2.z;
        if (p1.z > maxZ) maxZ = p1.z;
        if (p2.z > maxZ) maxZ = p2.z;
        
		//这是切片一个面跟分层相交那么增加一个切片部件
        for(int32_t layerNr = (minZ - initial) / thickness; layerNr <= (maxZ - initial) / thickness; layerNr++)
        {
            int32_t z = layerNr * thickness + initial;
            if (z < minZ) continue;
            if (layerNr < 0) continue;
            
			//切片的分段
            SlicerSegment Segment;

            if (p0.z < z && p1.z >= z && p2.z >= z)
                Segment = project2D(p0, p2, p1, z);
            else if (p0.z > z && p1.z < z && p2.z < z)
                Segment = project2D(p0, p1, p2, z);

            else if (p1.z < z && p0.z >= z && p2.z >= z)
                Segment = project2D(p1, p0, p2, z);
            else if (p1.z > z && p0.z < z && p2.z < z)
                Segment = project2D(p1, p2, p0, z);

            else if (p2.z < z && p1.z >= z && p0.z >= z)
                Segment = project2D(p2, p1, p0, z);
            else if (p2.z > z && p1.z < z && p0.z < z)
                Segment = project2D(p2, p0, p1, z);
            else
            {
				/**
				时间：2016年7月14日 11:47:19
				内容：当它当前层z轴的值等于这个三角面任意一个或多个z轴的值时，不用分层
						还有低于初始层的也不用分层
				作者：TwoSilly
				*/
                  //当它当前层z轴的值等于这个面最小点的z轴的值
                //并非所有的情况都是创建一个片段，因为一个面部的一个点可以创建一个点，和两个接触面
                //在片上会创建两个片段
                continue;
            }

            layers[layerNr].m_faceToSegmentIndex[i] = layers[layerNr].m_segmentList.size();
            Segment.faceIndex = i;
            Segment.addedToPolygon = false;
            layers[layerNr].m_segmentList.push_back(Segment);
        }
    }
    
    for(unsigned int layerNr=0; layerNr<layers.size(); layerNr++)
    {
        layers[layerNr].makePolygons(ov, keepNoneClosed, extensiveStitching);
    }
}

void Slicer::dumpSegmentsToHTML(const char* filename)
{
	/*
	时间：2016年7月13日 16:26:19
	内容：这里的 float scale = std::max(modelSize.x, modelSize.y) / 1500;
		被我改了！是为了改正vs的sb宏max
	作者：TwoSilly
	*/
    float scale = ( (std::max)(modelSize.x, modelSize.y) )/ 1500.0;
    FILE* f = nullptr;
	fopen_s(&f,filename, "w");
    fprintf(f, "<!DOCTYPE html><html><body>\n");
    for(unsigned int i=0; i<layers.size(); i++)
    {
        fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" style='width:%ipx;height:%ipx'>\n", int(modelSize.x / scale), int(modelSize.y / scale));
        fprintf(f, "<marker id='MidMarker' viewBox='0 0 10 10' refX='5' refY='5' markerUnits='strokeWidth' markerWidth='10' markerHeight='10' stroke='lightblue' stroke-width='2' fill='none' orient='auto'>");
        fprintf(f, "<path d='M 0 0 L 10 5 M 0 10 L 10 5'/>");
        fprintf(f, "</marker>");
        fprintf(f, "<g fill-rule='evenodd' style=\"fill: gray; stroke:black;stroke-width:1\">\n");
        fprintf(f, "<path marker-mid='url(#MidMarker)' d=\"");
        for(unsigned int j=0; j<layers[i].polygonList_.size(); j++)
        {
            PolygonRef p = layers[i].polygonList_[j];
            for(unsigned int n=0; n<p.size(); n++)
            {
                if (n == 0)
                    fprintf(f, "M");
                else
                    fprintf(f, "L");
                fprintf(f, "%f,%f ", float(p[n].X - modelMin.x)/scale, float(p[n].Y - modelMin.y)/scale);
            }
            fprintf(f, "Z\n");
        }
        fprintf(f, "\"/>");
        fprintf(f, "</g>\n");
        for(unsigned int j=0; j<layers[i].openPolygons_.size(); j++)
        {
            PolygonRef p = layers[i].openPolygons_[j];
            fprintf(f, "<polyline marker-mid='url(#MidMarker)' points=\"");
            for(unsigned int n=0; n<p.size(); n++)
            {
                fprintf(f, "%f,%f ", float(p[n].X - modelMin.x)/scale, float(p[n].Y - modelMin.y)/scale);
            }
            fprintf(f, "\" style=\"fill: none; stroke:red;stroke-width:1\" />\n");
        }
        fprintf(f, "</svg>\n");
    }
    fprintf(f, "</body></html>");
    fclose(f);
}

}//namespace Engine
