/**

文件名：inset.cpp

时间：2016年8月1日 14:34:51
内容：插入模块
注：依旧，没看算法
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include "inset.h"
#include "polygonOptimizer.h"

namespace Engine {

/**
时间：2016年8月1日 14:35:27
内容：形成，产生插入物，（切片层的部件）
	参数：
		SliceLayerPart* part --> 切片数据储存
		int offset --> 补偿
		int insetCount --> 插入数量
作者：TwoSilly
*/
void generateInsets(SliceLayerPart* part, int offset, int insetCount)
{
    part->combBoundery = part->outline.offset(-offset);
    if (insetCount == 0)
    {
        part->insets.push_back(part->outline);
        return;
    }
    
    for(int i=0; i<insetCount; i++)
    {
        part->insets.push_back(Polygons());
        part->insets[i] = part->outline.offset(-offset * i - offset/2);
        optimizePolygons(part->insets[i]);
        if (part->insets[i].size() < 1)
        {
            part->insets.pop_back();
            break;
        }
    }
}


/**
时间：2016年8月1日 14:35:27
内容：形成，产生插入物，（切片层）
	参数：
		SliceLayer* layer --> 切片层
		int offset --> 补偿
		int insetCount --> 插入数量
作者：TwoSilly
*/
void generateInsets(SliceLayer* layer, int offset, int insetCount)
{
    for(unsigned int partNr = 0; partNr < layer->parts.size(); partNr++)
    {
        generateInsets(&layer->parts[partNr], offset, insetCount);
    }    
    //删除不产生插入部分。由于这些零件太小而不能印刷，
    //和以后的代码现在可以认为总有最少1嵌线。
    for(unsigned int partNr = 0; partNr < layer->parts.size(); partNr++)
    {
        if (layer->parts[partNr].insets.size() < 1)
        {
            layer->parts.erase(layer->parts.begin() + partNr);
            partNr -= 1;
        }
    }
}

}//namespace Engine
