
/**

文件名：bridge.h

时间：2016年7月4日 09:29:54
内容：bridge 桥 起链接作用的东西
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef BRIDGE_H
#define BRIDGE_H

#include "sliceDataStorage.h"

namespace Engine {

	/**
	时间：2016年7月9日 17:40:47
	内容：桥的角度
		参数：
			Polygons outline --> 多边型 大纲
			SliceLayer* prevLayer --> 切片层 prev 预防层
	作者：TwoSilly
	*/
int bridgeAngle(Polygons outline, SliceLayer* prevLayer);
  /*!
     * 计算两个连续线段之间的夹角。
     * 
     * 角计算从B的左侧时，从A。
     * 
     *   c
     *    \                     .
     *     \ b
     * angle|
     *      |
     *      a
     * 
     * \param a 第一线段的开始
     * \param b 第一段结束和第二线段开始
     * \param c 第二线段的结束
     * \return 在0和2×P B PI之间的弧度角角
     */
     float getAngleLeft_(const Point& a, const Point& b, const Point& c);
}//namespace Engine

#endif//BRIDGE_H
