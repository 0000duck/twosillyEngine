/**

�ļ�����skirt.h

ʱ�䣺2016��7��28�� 15:32:42
���ݣ���Ե��������Ե
ע�����ǲ���ȹ�ߣ���Ե��ģ�飬�����㷨��ûϸ�����ͺ�
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef SKIRT_H
#define SKIRT_H

#include "sliceDataStorage.h"

namespace Engine {
	/**
	ʱ�䣺2016��7��28�� 15:38:32
	���ݣ���������������Ե
		������
			SliceDataStorage& storage --> ��Ƭ���ݴ���
			int distance --> ����
			int extrusionWidth --> �������
			int count --> ����
			int minLength --> ��С�ĳ���
			int initialLayerHeight --> ��ʼ����ĸ�
	���ߣ�TwoSilly
	*/
void generateSkirt(SliceDataStorage& storage, int distance, int extrusionWidth, int count, int minLength, int initialLayerHeight);

}//namespace Engine

#endif//SKIRT_H
