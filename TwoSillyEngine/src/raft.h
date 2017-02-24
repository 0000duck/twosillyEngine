/**

�ļ�����raft.h

ʱ�䣺2016��8��1�� 14:29:47
���ݣ�����ľ�����׷�
ע���㷨û�������ǲ����׷���ģ��
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef RAFT_H
#define RAFT_H

#include "sliceDataStorage.h"

namespace Engine {
/**
ʱ�䣺2016��8��1�� 09:09:33
���ݣ��γɣ������׷�
	������
		SliceDataStorage& storage --> ��Ƭ���ݴ���
		int distance --> ����
���ߣ�TwoSilly
*/
void generateRaft(SliceDataStorage& storage, int distance);

}//namespace Engine

#endif//RAFT_H
