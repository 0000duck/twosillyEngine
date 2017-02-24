/**

文件名：layerPart.cpp

时间：2016年7月13日 16:40:16
内容：层的部件
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include <stdio.h>

#include "layerPart.h"
#include "../tool/settings.h"

/*

该层的一部分创建步骤是创建3D打印实际有用的数据的第一步。
它以切片步骤的结果，这是一个无序列表和多边形，多边形组，每一组称为“部分”，
它有时也被称为“孤岛”。这些部件表示在有可能孔的二维层中的孤立区域。

创建“零件”是一个重要的一步，因为在一个单一的部分的所有元素都应该打印到另一部分之前。
和所有的每一位一个部分里面可以打印没有喷嘴离开这部分的边界。
它也是存储在“数据存储”中的结果的第一步，所以所有其他的步骤都可以访问它。

*/

namespace Engine {


	/**
	时间：2016年7月13日 16:45:09
	内容：创建一个层和部件
		参数：
			SliceLayer& storageLayer --> 储存层
			SlicerLayer* layer --> 层
			int unionAllType --> 联盟的全部类型
	作者：TwoSilly
	*/
void createLayerWithParts(SliceLayer& storageLayer, SlicerLayer* layer, int unionAllType)
{
    storageLayer.openLines = layer->openPolygons_;//->getopenPolygons();

	//&按位与都为1才为1
	//传入的类型第二位为1就进入该判断
    if (unionAllType & FIX_HORRIBLE_UNION_ALL_TYPE_B)
    {
        for(unsigned int i=0; i<layer->polygonList_.size(); i++)
        {
            if (layer->polygonList_[i].orientation())
                layer->polygonList_[i].reverse();
        }
    }
    
    vector<Polygons> result;//结果

    if (unionAllType & FIX_HORRIBLE_UNION_ALL_TYPE_C)
        result = layer->polygonList_.offset(1000).splitIntoParts(unionAllType);
    else
        result = layer->polygonList_.splitIntoParts(unionAllType);
    for(unsigned int i=0; i<result.size(); i++)
    {
        storageLayer.parts.push_back(SliceLayerPart());
        if (unionAllType & FIX_HORRIBLE_UNION_ALL_TYPE_C)
        {
            storageLayer.parts[i].outline.add(result[i][0]);
            storageLayer.parts[i].outline = storageLayer.parts[i].outline.offset(-1000);
        }else
            storageLayer.parts[i].outline = result[i];
        storageLayer.parts[i].boundaryBox.calculate(storageLayer.parts[i].outline);
    }
}

	/**
	时间：2016年7月27日 16:23:19
	内容：创建层部件
		参数：
			SliceVolumeStorage& storage --> 切片量储存
			Slicer* slicer --> 切片
			int unionAllType --> 联盟的全部类型
	作者：TwoSilly
	*/
void createLayerParts(SliceVolumeStorage& storage, Slicer* slicer, int unionAllType)
{
    for(unsigned int layerNr = 0; layerNr < slicer->layers.size(); layerNr++)
    {
        storage.layers_.push_back(SliceLayer());
        storage.layers_[layerNr].sliceZ = slicer->layers[layerNr].z_;//.getz();
        storage.layers_[layerNr].printZ = slicer->layers[layerNr].z_;//.getz();
        createLayerWithParts(storage.layers_[layerNr], &slicer->layers[layerNr], unionAllType);
    }
}


	/**
	时间：2016年7月27日 17:07:42
	内容：堆层部分（dump linux dump功能是备份文件系统）
		参数：
			SliceVolumeStorage& storage --> 切片量储存
			Slicer* slicer --> 切片
			int unionAllType --> 联盟的全部类型
	作者：TwoSilly
	*/
void dumpLayerparts(SliceDataStorage& storage, const char* filename)
{
    FILE* out = nullptr;
		fopen_s(&out,filename, "w");
    fprintf(out, "<!DOCTYPE html><html><body>");
    Point3 modelSize = storage.modelSize;
    Point3 modelMin = storage.modelMin;
    
    for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
    {
        for(unsigned int layerNr=0;layerNr<storage.volumes[volumeIdx].layers_.size(); layerNr++)
        {
            fprintf(out, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" style=\"width: 500px; height:500px\">\n");
            SliceLayer* layer = &storage.volumes[volumeIdx].layers_[layerNr];
            for(unsigned int i=0;i<layer->parts.size();i++)
            {
                SliceLayerPart* part = &layer->parts[i];
                for(unsigned int j=0;j<part->outline.size();j++)
                {
                    fprintf(out, "<polygon points=\"");
                    for(unsigned int k=0;k<part->outline[j].size();k++)
                        fprintf(out, "%f,%f ", float(part->outline[j][k].X - modelMin.x)/modelSize.x*500, float(part->outline[j][k].Y - modelMin.y)/modelSize.y*500);
                    if (j == 0)
                        fprintf(out, "\" style=\"fill:gray; stroke:black;stroke-width:1\" />\n");
                    else
                        fprintf(out, "\" style=\"fill:red; stroke:black;stroke-width:1\" />\n");
                }
            }
            fprintf(out, "</svg>\n");
        }
    }
    fprintf(out, "</body></html>");
    fclose(out);
}

}//namespace Engine
