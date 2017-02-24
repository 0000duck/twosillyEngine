/**

文件名：sliceDataStorage.h

时间：2016年7月4日 09:29:54
内容：切片数据储存的数据结构
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/

#ifndef SLICE_DATA_STORAGE_H
#define SLICE_DATA_STORAGE_H

#include "../utils/intpoint.h"
#include "../utils/polygon.h"

#include "SupportDataStorage.h"

/*
SliceData
+ Layers[]
  + LayerParts[]
    + OutlinePolygons[]
    + Insets[]
      + Polygons[]
    + SkinPolygons[]
*/
namespace Engine {

	/**
	时间：2016年7月11日 09:25:46
	内容：切片层的部件
		数据成员：
			AxisAlignedBoundaryBox boundaryBox;//轴对齐边界框
			Polygons outline;		//轮廓
			Polygons combBoundery; //comb 梳子  boundery 边 
			vector<Polygons> insets;	//插入
			Polygons skinOutline;	//皮肤轮廓
			Polygons sparseOutline; //稀少的轮廓
	作者：TwoSilly
	*/
class SliceLayerPart
{
public:
    AxisAlignedBoundaryBox boundaryBox;//轴对齐边界框
    Polygons outline;		//轮廓
    Polygons combBoundery; //comb 梳子  boundery 边 
    vector<Polygons> insets;	//插入
    Polygons skinOutline;	//皮肤轮廓
	Polygons sparseOutline; //稀少的轮廓
};



	/**
	时间：2016年7月11日 09:25:46
	内容：切片层
		数据成员：
		int sliceZ; //切片Z
		int printZ; //打印Z
		vector<SliceLayerPart> parts; //部件
		Polygons openLines; //打印行
	作者：TwoSilly
	*/
class SliceLayer
{
public:
    int sliceZ; //切片Z
    int printZ; //打印Z
    vector<SliceLayerPart> parts; //部件
    Polygons openLines; //打印行
};


	/**
	时间：2016年7月11日 09:25:46
	内容：切片量储存
		数据结构;
			vector<SliceLayer> layers_;
	作者：TwoSilly
	*/
class SliceVolumeStorage
{
public:
    vector<SliceLayer> layers_;
};

	/**
	时间：2016年7月11日 09:25:46
	内容：切片数据储存
	作者：TwoSilly
	*/
class SliceDataStorage//切片数据储存
{
public:
    Point3 modelSize, modelMin, modelMax;//模型大小  模型最小 模型最大（x y z）
    Polygons skirt; //裙子边缘
    Polygons raftOutline;               //筏轮廓的存储。将充满线当Gcode生成。
    vector<Polygons> oozeShield;        //oozeShield per layer软泥盾构每层
    vector<SliceVolumeStorage> volumes;//量
    
    SupportStorage support;//支撑
    Polygons wipeTower;//擦拭塔
    Point wipePoint; //擦拭打印
};

}//namespace Engine

#endif//SLICE_DATA_STORAGE_H
