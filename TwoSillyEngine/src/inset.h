/**

文件名：inset.h

时间：2016年8月1日 14:34:51
内容：插入模块
注：依旧，没看算法
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#ifndef INSET_H
#define INSET_H

#include "sliceDataStorage.h"

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
void generateInsets(SliceLayerPart* part, int offset, int insetCount);

/**
时间：2016年8月1日 14:35:27
内容：形成，产生插入物，（切片层）
	参数：
		SliceLayer* layer --> 切片层
		int offset --> 补偿
		int insetCount --> 插入数量
作者：TwoSilly
*/
void generateInsets(SliceLayer* layer, int offset, int insetCount);

}//namespace Engine

#endif//INSET_H
