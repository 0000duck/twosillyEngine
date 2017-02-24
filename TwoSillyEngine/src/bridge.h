
/**

�ļ�����bridge.h

ʱ�䣺2016��7��4�� 09:29:54
���ݣ�bridge �� ���������õĶ���
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef BRIDGE_H
#define BRIDGE_H

#include "sliceDataStorage.h"

namespace Engine {

	/**
	ʱ�䣺2016��7��9�� 17:40:47
	���ݣ��ŵĽǶ�
		������
			Polygons outline --> ����� ���
			SliceLayer* prevLayer --> ��Ƭ�� prev Ԥ����
	���ߣ�TwoSilly
	*/
int bridgeAngle(Polygons outline, SliceLayer* prevLayer);
  /*!
     * �������������߶�֮��ļнǡ�
     * 
     * �Ǽ����B�����ʱ����A��
     * 
     *   c
     *    \                     .
     *     \ b
     * angle|
     *      |
     *      a
     * 
     * \param a ��һ�߶εĿ�ʼ
     * \param b ��һ�ν����͵ڶ��߶ο�ʼ
     * \param c �ڶ��߶εĽ���
     * \return ��0��2��P B PI֮��Ļ��Ƚǽ�
     */
     float getAngleLeft_(const Point& a, const Point& b, const Point& c);
}//namespace Engine

#endif//BRIDGE_H
