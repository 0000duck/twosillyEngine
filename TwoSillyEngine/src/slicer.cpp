/**

�ļ�����slicer.cpp

ʱ�䣺2016��7��13�� 19:08:37
���ݣ���Ƭģ��
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include <stdio.h>
#include <algorithm>/*�Ҽӵ���Ϊstd����max����*/
#include "../tool/gettime.h"
#include "../tool/logoutput.h"

#include "slicer.h"
#include "polygonOptimizer.h"

namespace Engine {
	
	/**
	ʱ�䣺2016��7��13�� 19:30:57
	���ݣ��������Σ�
		����Ѵ���Ĳ�Ķ������һ���Ż����ϣ���֮��Ϊ�Ƚ����Ķ����·��
		������
			OptimizedVolume* ov --> �Ż���
			bool keepNoneClosed --> ���ֱպ�
			bool extensiveStitching --> �����ķ��
	���ߣ�TwoSilly
	*/
void SlicerLayer::makePolygons(OptimizedVolume* ov, bool keepNoneClosed, bool extensiveStitching)
{
	//����һ���򿪵Ķ��������
    Polygons openPolygonList;
    
	//������Ƭ�ֶ�����
    for(unsigned int startSegment=0; startSegment < this->m_segmentList.size(); startSegment++)
    {
		//�ж���Ƭ�ķֶ��б��Ƿ���Ӷ����
        if (m_segmentList[startSegment].addedToPolygon)
            continue;
        //����һ�������
        Polygon poly;
		//�ѵ�ǰ��Ƭ�ֶεĿ�ʼ��ѹ��������
        poly.add(m_segmentList[startSegment].start);
        
		//��¼��ǰ�ֶε��±�
        unsigned int segmentIndex = startSegment;
        //�Ƿ���Թر�
		bool canClose;
		
		
        while(true)
        {
			//�����Թر�
            canClose = false;
			
            m_segmentList[segmentIndex].addedToPolygon = true;
			//�ҳ���ǰ�ֶεĽ����㲢ѹ������
            Point p0 = m_segmentList[segmentIndex].end;
            poly.add(p0);

            int nextIndex = -1;
			
			//��ǰ����Ƭ�ķָ��������ڵ���
            OptimizedFace* face = &ov->faces[m_segmentList[segmentIndex].faceIndex];
			
			//�ҳ�������Ƿ�պϣ�����ֿ�
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
            polygonList_.add(poly); //�պϵ�
        else
            openPolygonList.add(poly); //δ�պϵ�
    }


    //������б��Ա����ڴ棬��һ��֮��Ͳ�����Ҫ�ˡ�
    m_segmentList.clear();

  
    //���ӵĶ���λ�û�йرգ���Ϊģ�Ͳ����������������Σ�������Ҫ����һЩ�������Ի���ʵ��Ķ����
    //�������Ӷ���ε����ˣ���2΢�����ڡ�
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

   
   //��һ����������ʧ�ٵĽ������ر���С�Ĳ���һ������һ����Ч��ʵ�֣�������������O��n * n����ʱ�䡣
    while(true)
    {
		//��õĵ÷�
        int64_t bestScore = MM2INT(10.0) * MM2INT(10.0);
        unsigned int bestA = -1; //��õ�a
        unsigned int bestB = -1; //��õ�b
        bool reversed = false; //��ת
		//�������ڵĶ��������
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
        ���ڹ㷺��ƴ�ӷ���2�����ŵĶ���Σ��Ǵ���2����յĶ���Ρ�
        Ȼ���ҵ����̵�·�����������Σ������������ӿ��ŵĶ���Σ�
        ������һ��·�����������̵�λ����������2�����ŵĶ����
        �������2�����ŵĶ��������ͬ�Ķ���Σ�Ȼ�����յĽ����һ����յ����
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

   
    //ɾ�����в��رյ�С����λ����Ρ���Ϊ����û�й��׵�ʵ�ʴ�ӡ��
    int snapDistance = MM2INT(1.0);
    for(unsigned int i=0;i<polygonList_.size();i++)
    {
		/**
		ʱ�䣺2016��7��13�� 16:28:21
		���ݣ�ԭ��int length = 0;��Ϊ�˸ĵ����˵ľ���Ŵ�����
		���ߣ�TwoSilly
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


    //����Ż����еĶ���Ρ��ӳ�Զ������ÿһ�㶼��ʡ��ʱ�䡣
    optimizePolygons(polygonList_);
}

	/**
	ʱ�䣺2016��7��13�� 20:44:42
	���ݣ���Ƭ��Ĺ��캯���������ģ�Ͱ��ֺõĲ��зֲ��������꣡����3dģ��ת��λÿ���в�ߵ�2d����
		������
			OptimizedVolume* ov --> �Ż���(ģ��)
			int32_t initial --> �����
			int32_t thickness --> ���
			bool keepNoneClosed --> һֱû�йر�
			bool extensiveStitching --> �㷺��ƴ��
	���ߣ�TwoSilly
	*/
Slicer::Slicer(OptimizedVolume* ov, int32_t initial, int32_t thickness, bool keepNoneClosed, bool extensiveStitching)
{
    modelSize = ov->model->modelSize_; //ģ�ʹ�С
    modelMin = ov->model->vMin_; //ģ����Сֵ
    
	//�����
    int layerCount = (modelSize.z - initial) / thickness + 1;
    Engine::log("Layer count: %i\n", layerCount);
    layers.resize(layerCount);
    
	//����ģ�͵�ÿ���z��Ĵ�С
    for(int32_t layerNr = 0; layerNr < layerCount; layerNr++)
    {
		/*
		ʱ�䣺2016��7��26�� 17:32:53
		���ݣ�������Ϊ�˱�����ķ�װ�԰���ԭ���Ĺ��е����ݳ�Ա������˽�еģ����Ը�ֵ�õ�set����
		���ߣ�TwoSilly
		*/
        layers[layerNr].z_ =  initial + thickness * layerNr;
    }
    
	//��������ģ�͵��棨������Ƭ��
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
        
		//������Ƭһ������ֲ��ཻ��ô����һ����Ƭ����
        for(int32_t layerNr = (minZ - initial) / thickness; layerNr <= (maxZ - initial) / thickness; layerNr++)
        {
            int32_t z = layerNr * thickness + initial;
            if (z < minZ) continue;
            if (layerNr < 0) continue;
            
			//��Ƭ�ķֶ�
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
				ʱ�䣺2016��7��14�� 11:47:19
				���ݣ�������ǰ��z���ֵ�����������������һ������z���ֵʱ�����÷ֲ�
						���е��ڳ�ʼ���Ҳ���÷ֲ�
				���ߣ�TwoSilly
				*/
                  //������ǰ��z���ֵ�����������С���z���ֵ
                //�������е�������Ǵ���һ��Ƭ�Σ���Ϊһ���沿��һ������Դ���һ���㣬�������Ӵ���
                //��Ƭ�ϻᴴ������Ƭ��
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
	ʱ�䣺2016��7��13�� 16:26:19
	���ݣ������ float scale = std::max(modelSize.x, modelSize.y) / 1500;
		���Ҹ��ˣ���Ϊ�˸���vs��sb��max
	���ߣ�TwoSilly
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
