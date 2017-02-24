/**

文件名：infill.h

时间：2016年8月1日 14:29:47
内容：填充
注：算法没看，这是填充的模块
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef INFILL_H
#define INFILL_H

#include "../utils/polygon.h"
#include "polygonOptimizer.h"

namespace Engine {
/**
时间：2016年8月1日 18:49:04
内容：形成，产生，同轴的，同中心的填充（实心的，全体填充，固体）
	参数：
		Polygons outline --> 多边形，大纲，轮廓
		Polygons& result -->   结果，
		int inset_value --> 插入的价值，
作者：TwoSilly
*/
void generateConcentricInfill(Polygons outline, Polygons& result, int inset_value);

/**
时间：2016年8月1日 18:49:04
内容：形成，产生，智能的填充（如果行间隔大于挤出机宽度的4倍自动选择网格填充，否则选择行填充）
	参数：
		const Polygons& in_outline --> 大纲
		Polygons& result --> 结果
		int extrusionWidth --> 挤出机宽度
		int lineSpacing --> 行间隔
		int infillOverlap --> 填充的重叠部分
		double rotation --> 旋转
作者：TwoSilly
*/
void generateAutomaticInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);

/**
时间：2016年8月1日 18:49:04
内容：形成，产生，网格填充（）
	参数：
		const Polygons& in_outline --> 大纲
		Polygons& result --> 结果
		int extrusionWidth --> 挤出机宽度
		int lineSpacing --> 行间隔
		int infillOverlap --> 填充的重叠部分
		double rotation --> 旋转
作者：TwoSilly
*/
void generateGridInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);

/**
时间：2016年8月1日 18:49:04
内容：形成，产生，行填充
	参数：
		const Polygons& in_outline --> 大纲
		Polygons& result --> 结果
		int extrusionWidth --> 挤出机宽度
		int lineSpacing --> 行间隔
		int infillOverlap --> 填充的重叠部分
		double rotation --> 旋转

注：这里会生成行填充，不过我又没看算法
作者：TwoSilly
*/
void generateLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);


/**
时间：2016年8月1日 18:49:04
内容：形成，产生，Z填充（折线填充）
参数：
const Polygons& in_outline --> 大纲
Polygons& result --> 结果
int extrusionWidth --> 挤出机宽度
int lineSpacing --> 行间隔
int infillOverlap --> 填充的重叠部分
double rotation --> 旋转

注：这里会生成行填充，不过我又没看算法
作者：TwoSilly
*/
void generateBrokenLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);

}//namespace Engine

#endif//INFILL_H
