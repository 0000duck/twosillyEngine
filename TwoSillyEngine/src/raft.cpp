/**

文件名：raft.cpp

时间：2016年8月1日 14:29:47
内容：伐，木筏，底阀
注：算法没看，这是产生底阀的模块
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include "raft.h"
#include "support.h"

namespace Engine {


/**
时间：2016年8月1日 09:09:33
内容：形成，产生底阀
	参数：
		SliceDataStorage& storage --> 切片数据储存
		int distance --> 距离
作者：TwoSilly
*/
void generateRaft(SliceDataStorage& storage, int distance)
{
	/*遍历切片数据储存类中的 切片量*/
    for(unsigned int volumeIdx = 0; volumeIdx < storage.volumes.size(); volumeIdx++)
    {
		//假如当前切片层的的数量小于1 就做下一个判断
		//即：判空
        if (storage.volumes[volumeIdx].layers_.size() < 1) continue;
		
		//切片层数据中的第一层
        SliceLayer* layer = &storage.volumes[volumeIdx].layers_[0];

		//遍历切片层中的第一层中的 部件数量
        for(unsigned int i=0; i<layer->parts.size(); i++)
        {
			//求出第一层轮廓的最大值，  distance 值是当前轮廓放大多少倍！
            storage.raftOutline = storage.raftOutline.unionPolygons(layer->parts[i].outline.offset(distance));
        }
    }


    SupportPolyGenerator supportGenerator(storage.support, 0);
    storage.raftOutline = storage.raftOutline.unionPolygons(supportGenerator.polygons_.offset(distance));
    storage.raftOutline = storage.raftOutline.unionPolygons(storage.wipeTower.offset(distance));
}

}//namespace Engine
