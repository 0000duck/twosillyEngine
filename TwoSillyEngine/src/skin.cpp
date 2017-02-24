/**

文件名：skin.cpp

时间：2016年8月1日 09:07:16
内容：皮肤，封底封顶，以及壁内加强部分，skin部分都是要实心打印的
注：
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#include "skin.h"

namespace Engine {

/**
时间：2016年8月1日 09:09:33
内容：形成，产生皮肤
	参数：
		int layerNr --> 层的数量范围
		SliceVolumeStorage& storage --> 切片数据储存
		int extrusionWidth --> 挤出宽度
		int downSkinCount --> 下皮肤的数量
		int upSkinCount --> 上皮肤的数量
		int infillOverlap --> 填充重叠
作者：TwoSilly
*/
void generateSkins(int layerNr, SliceVolumeStorage& storage, int extrusionWidth, int downSkinCount, int upSkinCount, int infillOverlap)
{
    SliceLayer* layer = &storage.layers_[layerNr];

    for(unsigned int partNr=0; partNr<layer->parts.size(); partNr++)
    {
        SliceLayerPart* part = &layer->parts[partNr];
        
        Polygons upskin = part->insets[part->insets.size() - 1].offset(-extrusionWidth/2);
        Polygons downskin = upskin;
        
        if (part->insets.size() > 1)
        {
			/**
			时间：2016年8月1日 11:42:05
			内容：薄壁，添加薄壁以之间的间隔区域填充。
				注：这里的填充重叠的意思是啥？？意义？？不懂
			作者：TwoSilly
			*/
            
            Polygons thinWalls = part->insets[0].offset(-extrusionWidth / 2 - extrusionWidth * infillOverlap / 100).difference(part->insets[1].offset(extrusionWidth * 6 / 10));
            upskin.add(thinWalls);
            downskin.add(thinWalls);
        }
			/**
			时间：2016年8月1日 11:49:19
			内容：//这里的int-int为啥要强转位int？？？不懂
			作者：TwoSilly
			*/		
        if (static_cast<int>(layerNr - downSkinCount) >= 0)
        {
            SliceLayer* layer2 = &storage.layers_[layerNr - downSkinCount];
            for(unsigned int partNr2=0; partNr2<layer2->parts.size(); partNr2++)
            {
                if (part->boundaryBox.hit(layer2->parts[partNr2].boundaryBox))
                    downskin = downskin.difference(layer2->parts[partNr2].insets[layer2->parts[partNr2].insets.size() - 1]);
            }
        }

        if (static_cast<int>(layerNr + upSkinCount) < static_cast<int>(storage.layers_.size()))
        {
            SliceLayer* layer2 = &storage.layers_[layerNr + upSkinCount];
            for(unsigned int partNr2=0; partNr2<layer2->parts.size(); partNr2++)
            {
                if (part->boundaryBox.hit(layer2->parts[partNr2].boundaryBox))
                    upskin = upskin.difference(layer2->parts[partNr2].insets[layer2->parts[partNr2].insets.size() - 1]);
            }
        }
        
        part->skinOutline = upskin.unionPolygons(downskin);

        double minAreaSize = (2 * M_PI * INT2MM(extrusionWidth) * INT2MM(extrusionWidth)) * 0.3;
        for(unsigned int i=0; i<part->skinOutline.size(); i++)
        {
            double area = INT2MM(INT2MM(fabs(part->skinOutline[i].area())));
            if (area < minAreaSize) 
            {                       //只有创建一个向上/向下的皮肤，如果该地区是足够大。所以你不创造微小的“填补”的斑点
                part->skinOutline.remove(i);
                i -= 1;
            }
        }
    }
}

/**
时间：2016年8月1日 09:09:33
内容：形成，产生稀少的
	参数：
		int layerNr --> 层的数量范围
		SliceVolumeStorage& storage --> 切片数据储存
		int extrusionWidth --> 挤出宽度
		int downSkinCount --> 下皮肤的数量
		int upSkinCount --> 上皮肤的数量
作者：TwoSilly
*/
void generateSparse(int layerNr, SliceVolumeStorage& storage, int extrusionWidth, int downSkinCount, int upSkinCount)
{
    SliceLayer* layer = &storage.layers_[layerNr];

    for(unsigned int partNr=0; partNr<layer->parts.size(); partNr++)
    {
        SliceLayerPart* part = &layer->parts[partNr];

        Polygons sparse = part->insets[part->insets.size() - 1].offset(-extrusionWidth/2);
        Polygons downskin = sparse;
        Polygons upskin = sparse;
        
        if (static_cast<int>(layerNr - downSkinCount) >= 0)
        {
            SliceLayer* layer2 = &storage.layers_[layerNr - downSkinCount];
            for(unsigned int partNr2=0; partNr2<layer2->parts.size(); partNr2++)
            {
                if (part->boundaryBox.hit(layer2->parts[partNr2].boundaryBox))
                {
                    if (layer2->parts[partNr2].insets.size() > 1)
                    {
                        downskin = downskin.difference(layer2->parts[partNr2].insets[layer2->parts[partNr2].insets.size() - 2]);
                    }else{
                        downskin = downskin.difference(layer2->parts[partNr2].insets[layer2->parts[partNr2].insets.size() - 1]);
                    }
                }
            }
        }
        if (static_cast<int>(layerNr + upSkinCount) < static_cast<int>(storage.layers_.size()))
        {
            SliceLayer* layer2 = &storage.layers_[layerNr + upSkinCount];
            for(unsigned int partNr2=0; partNr2<layer2->parts.size(); partNr2++)
            {
                if (part->boundaryBox.hit(layer2->parts[partNr2].boundaryBox))
                {
                    if (layer2->parts[partNr2].insets.size() > 1)
                    {
                        upskin = upskin.difference(layer2->parts[partNr2].insets[layer2->parts[partNr2].insets.size() - 2]);
                    }else{
                        upskin = upskin.difference(layer2->parts[partNr2].insets[layer2->parts[partNr2].insets.size() - 1]);
                    }
                }
            }
        }
        
        Polygons result = upskin.unionPolygons(downskin);

        double minAreaSize = 3.0;//(2 * M_PI * INT2MM(_config.extrusionWidth) * INT2MM(_config.extrusionWidth)) * 3;
        for(unsigned int i=0; i<result.size(); i++)
        {
            double area = INT2MM(INT2MM(fabs(result[i].area())));
            if (area < minAreaSize) /* 只有创建一个向上/向下的皮肤，如果该地区是足够大。所以你不创造微小的“填补”的斑点 */
            {
                result.remove(i);
                i -= 1;
            }
        }
        
        part->sparseOutline = sparse.difference(result);
    }
}

}//namespace Engine
