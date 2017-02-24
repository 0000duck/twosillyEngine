
/**

文件名：multiVolumes.h

时间：2016年8月1日 14:29:47
内容：多，许多，卷
注：算法没看！
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef MULTIVOLUMES_H
#define MULTIVOLUMES_H
#include "sliceDataStorage.h"
/* 此文件包含代码，以帮助固定和更改从多个卷生成的层. */
namespace Engine {

	/**
	时间：2016年8月1日 19:46:29
	内容：切片，切开，多卷，去槽所有的卷，
			并删除以前的卷从我们自己的轮廓，所以我们从来没有重叠的领域。
		参数：
			vector<SliceVolumeStorage> &volumes --> 切片数据储存的链表

	作者：TwoSilly
	*/
void carveMultipleVolumes(vector<SliceVolumeStorage> &volumes)
{
    
    for(unsigned int idx=0; idx < volumes.size(); idx++)
    {
        for(unsigned int idx2=0; idx2<idx; idx2++)
        {
            for(unsigned int layerNr=0; layerNr < volumes[idx].layers_.size(); layerNr++)
            {
                SliceLayer* layer1 = &volumes[idx].layers_[layerNr];
                SliceLayer* layer2 = &volumes[idx2].layers_[layerNr];
                for(unsigned int p1 = 0; p1 < layer1->parts.size(); p1++)
                {
                    for(unsigned int p2 = 0; p2 < layer2->parts.size(); p2++)
                    {
                        layer1->parts[p1].outline = layer1->parts[p1].outline.difference(layer2->parts[p2].outline);
                    }
                }
            }
        }
    }
}

	/**
	时间：2016年8月1日 19:46:29
	内容：生成多个卷重叠，扩大每一层有一点，然后保持额外的重叠部分重叠与其他卷。
			这产生了一些重叠的双挤压，更好的粘接在接触部分。
		参数：
			vector<SliceVolumeStorage> &volumes --> 切片数据储存的链表
			int overlap --> 重叠部分
	作者：TwoSilly
	*/
void generateMultipleVolumesOverlap(vector<SliceVolumeStorage> &volumes, int overlap)
{
    if (volumes.size() < 2 || overlap <= 0) return;
    
    for(unsigned int layerNr=0; layerNr < volumes[0].layers_.size(); layerNr++)
    {
        Polygons fullLayer;
        for(unsigned int volIdx = 0; volIdx < volumes.size(); volIdx++)
        {
            SliceLayer* layer1 = &volumes[volIdx].layers_[layerNr];
            for(unsigned int p1 = 0; p1 < layer1->parts.size(); p1++)
            {
                fullLayer = fullLayer.unionPolygons(layer1->parts[p1].outline.offset(20));
            }
        }
        fullLayer = fullLayer.offset(-20);
        
        for(unsigned int volIdx = 0; volIdx < volumes.size(); volIdx++)
        {
            SliceLayer* layer1 = &volumes[volIdx].layers_[layerNr];
            for(unsigned int p1 = 0; p1 < layer1->parts.size(); p1++)
            {
                layer1->parts[p1].outline = fullLayer.intersection(layer1->parts[p1].outline.offset(overlap / 2));
            }
        }
    }
}

}//namespace Engine

#endif//MULTIVOLUMES_H
