/**

文件名：skirt.cpp

时间：2016年7月28日 15:32:42
内容：边缘，产生边缘
注：这是产生裙边，边缘的模块，集体算法我没细看，滞后
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#include "skirt.h"
#include "support.h"

namespace Engine {

	/**
	时间：2016年7月28日 15:38:32
	内容：创建，产生，边缘
		参数：
			SliceDataStorage& storage --> 切片数据储存
			int distance --> 距离
			int extrusionWidth --> 挤出宽度
			int count --> 数量
			int minLength --> 最小的长度
			int initialLayerHeight --> 初始化层的高
	作者：TwoSilly
	*/
void generateSkirt(SliceDataStorage& storage, int distance, int extrusionWidth, int count, int minLength, int initialLayerHeight)
{
	//判断是否距离大于零
    bool externalOnly = (distance > 0);

    for(int skirtNr=0; skirtNr<count;skirtNr++)
    {
		//偏移距离
        int offsetDistance = distance + extrusionWidth * skirtNr + extrusionWidth / 2;
        
        SupportPolyGenerator supportGenerator(storage.support, initialLayerHeight);
        Polygons skirtPolygons(storage.wipeTower.offset(offsetDistance));
        for(unsigned int volumeIdx = 0; volumeIdx < storage.volumes.size(); volumeIdx++)
        {
            if (storage.volumes[volumeIdx].layers_.size() < 1) continue;
            SliceLayer* layer = &storage.volumes[volumeIdx].layers_[0];
            for(unsigned int i=0; i<layer->parts.size(); i++)
            {
                if (externalOnly)
                {
                    Polygons p;
                    p.add(layer->parts[i].outline[0]);
                    skirtPolygons = skirtPolygons.unionPolygons(p.offset(offsetDistance));
                }
                else
                    skirtPolygons = skirtPolygons.unionPolygons(layer->parts[i].outline.offset(offsetDistance));

                supportGenerator.polygons_ = supportGenerator.polygons_.difference(layer->parts[i].outline);
            }
        }
        
        //合同和扩大支援多边形小部分除去最后的多边形平滑一点。
        supportGenerator.polygons_ = supportGenerator.polygons_.offset(-extrusionWidth * 3);
        supportGenerator.polygons_ = supportGenerator.polygons_.offset(extrusionWidth * 3);
        skirtPolygons = skirtPolygons.unionPolygons(supportGenerator.polygons_.offset(offsetDistance));

		//拆下小的内裙孔。洞中有负值区，删除任何小于100x挤出“面积”
        for(unsigned int n=0; n<skirtPolygons.size(); n++)
        {
            double area = skirtPolygons[n].area();
            if (area < 0 && area > -extrusionWidth * extrusionWidth * 100)
                skirtPolygons.remove(n--);
        }

        storage.skirt.add(skirtPolygons);
        /**
		时间：2016年8月1日 08:42:05
		内容：这里我做一个简单的放大，为了屏蔽烦人的禁告
			原：int lenght = storage.skirt.polygonLength();
		作者：TwoSilly
		*/
        int64_t lenght = storage.skirt.polygonLength();
        if (skirtNr + 1 >= count && lenght > 0 && lenght < minLength)
            count++;
    }
}

}//namespace Engine
