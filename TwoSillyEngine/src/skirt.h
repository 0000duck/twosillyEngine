/**

文件名：skirt.h

时间：2016年7月28日 15:32:42
内容：边缘，产生边缘
注：这是产生裙边，边缘的模块，集体算法我没细看，滞后
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef SKIRT_H
#define SKIRT_H

#include "sliceDataStorage.h"

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
void generateSkirt(SliceDataStorage& storage, int distance, int extrusionWidth, int count, int minLength, int initialLayerHeight);

}//namespace Engine

#endif//SKIRT_H
