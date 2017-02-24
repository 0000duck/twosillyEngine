/**

文件名：support.h

时间：2016年7月29日 11:44:56
内容：支撑模块
改动：在私有成员面前都加“_”private,表示私有（数据成员和成员函数都是如此）
注：该模块算法我还不是很理解，不过时间关系先暂时滞留！
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#ifndef SUPPORT_H
#define SUPPORT_H

#include "sliceDataStorage.h"
#include "SupportDataStorage.h"
#include "optimizedModel.h"

namespace Engine {
	/**
	时间：2016年7月29日 11:47:55
	内容：形成支撑格子
		参数：
			SupportStorage& storage --> 支撑的数据储存
			OptimizedModel* om --> 优化后的模型
			int supportAngle --> 支撑角度
			bool supportEverywhere --> 是否所有都支撑
			int supportXYDistance --> 支撑xy的距离
			int supportZDistance --> 支撑z的距离
	作者：TwoSilly
	*/
void generateSupportGrid(SupportStorage& storage, OptimizedModel* om, int supportAngle, bool supportEverywhere, int supportXYDistance, int supportZDistance);


	/**
	时间：2016年7月29日 14:53:06
	内容：支撑多边形创造者
		数据成员：
			public:
				Polygons polygons;//多边形
			private:
				SupportStorage& _storage;//支撑储存
				double			_cosAngle; //余玄角度
				int32_t			_z; //z
				int				_supportZDistance;//支撑z轴的距离
				bool			_everywhere;//是否到处都是
				int*			_done;//已完成的; 煮熟的; 合乎礼仪的; 合乎规矩的
	作者：TwoSilly
	*/
class SupportPolyGenerator
{
public:
    Polygons polygons_;

private:
    SupportStorage& _storage;//支撑储存
    double _cosAngle; //余玄角度
    int32_t _z; //z
    int _supportZDistance;//支撑z轴的距离
    bool _everywhere;//是否到处都是
    int* _done;//已完成的; 煮熟的; 合乎礼仪的; 合乎规矩的

	/**
	时间：2016年7月29日 15:05:47
	内容：需要支撑在某点
	参数：
	Point p
	作者：TwoSilly
	*/
    bool _needSupportAt(Point p);
   

	/**
	时间：2016年7月29日 15:09:28
	内容：懒惰的填充
	参数：
		Point startPoint
	作者：TwoSilly
	*/
	void _lazyFill(Point startPoint);
    
public:
	
	/**
	时间：2016年7月29日 17:45:35
	内容：产生支撑多边形的构造函数
	参数：
		SupportStorage& storage //支撑的储存
		int32_t z //z
	作者：TwoSilly
	*/
    SupportPolyGenerator(SupportStorage& storage, int32_t z);
};

}//namespace Engine

#endif//SUPPORT_H
