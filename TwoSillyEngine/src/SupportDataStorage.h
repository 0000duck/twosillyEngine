/**

文件名：SupportDataStorage.h

时间：2016年7月4日 09:29:54
内容：支撑的储存类，支撑的储存的数据结构
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef SUPPORT_DATA_STORAGE_H
#define SUPPORT_DATA_STORAGE_H

namespace Engine{

	/**
	时间：2016年7月11日 09:25:46
	内容：支撑点
	作者：TwoSilly
	*/
class SupportPoint
{
public:
    int32_t z;
    double cosAngle;
    
    SupportPoint(int32_t z, double cosAngle) : z(z), cosAngle(cosAngle) {}
};

	/**
	时间：2016年7月11日 09:25:46
	内容：支撑储存结构体
		数据成员：
		bool generated;//创建
		int angle;//角度
		bool everywhere;//到处
		int XYDistance;//XY距离
		int ZDistance;//Z距离

		Point gridOffset;//网格偏移
		int32_t gridScale;//格网比例尺;
		int32_t gridWidth, gridHeight; //网格宽 网格高
		vector<SupportPoint>* grid;//网格
	作者：TwoSilly
	*/
class SupportStorage
{
public:
    bool generated;//是否创建
    int angle;//角度
    bool everywhere;//是否到处
    int XYDistance;//XY距离
    int ZDistance;//Z距离
    
    Point gridOffset;//网格偏移
    int32_t gridScale;//格网比例尺;测量;
    int32_t gridWidth, gridHeight; //网格宽 网格高
    vector<SupportPoint>* grid;//网格

   	SupportStorage(){grid = nullptr;}
	  ~SupportStorage()
	  {
		  if(grid)
		  {
			  delete [] grid;
			  grid = nullptr;
		  }
	  }
};



};
#endif //SUPPORT_DATA_STORAGE_H