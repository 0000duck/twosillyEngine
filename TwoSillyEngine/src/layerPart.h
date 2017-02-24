/**

文件名：layerPart.h

时间：2016年7月13日 16:40:16
内容：层的部件
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef LAYERPART_H
#define LAYERPART_H

#include "sliceDataStorage.h"
#include "slicer.h"

/*
该层的一部分创建步骤是创建3D打印实际有用的数据的第一步。
它以切片步骤的结果，这是一个无序列表和多边形，多边形组，
每一组称为“部分”，它有时也被称为“孤岛”。这些部件表示在有可能孔的二维层中的孤立区域。

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
void createLayerWithParts(SliceLayer& storageLayer, SlicerLayer* layer, int unionAllType);


	/**
	时间：2016年7月27日 16:23:19
	内容：创建层部件
		参数：
			SliceVolumeStorage& storage --> 切片量储存
			Slicer* slicer --> 切片
			int unionAllType --> 联盟的全部类型
	作者：TwoSilly
	*/
void createLayerParts(SliceVolumeStorage& storage, Slicer* slicer, int unionAllType);

	/**
	时间：2016年7月27日 17:07:42
	内容：堆层部分（dump linux dump功能是备份文件系统）
		参数：
			SliceVolumeStorage& storage --> 切片量储存
			Slicer* slicer --> 切片
			int unionAllType --> 联盟的全部类型
	作者：TwoSilly
	*/
void dumpLayerparts(SliceDataStorage& storage, const char* filename);

}//namespace Engine

#endif//LAYERPART_H
