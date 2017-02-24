/**

文件名：skin.h

时间：2016年8月1日 09:07:16
内容：皮肤，封底封顶，以及壁内加强部分，skin部分都是要实心打印的
注：这个里面的算法没太多时间看了！做个备注，以后再看
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef SKIN_H
#define SKIN_H

#include "sliceDataStorage.h"

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
void generateSkins(int layerNr, SliceVolumeStorage& storage, int extrusionWidth, int downSkinCount, int upSkinCount, int infillOverlap);


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
void generateSparse(int layerNr, SliceVolumeStorage& storage, int extrusionWidth, int downSkinCount, int upSkinCount);

}//namespace Engine

#endif//SKIN_H
