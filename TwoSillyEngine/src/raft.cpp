/**

�ļ�����raft.cpp

ʱ�䣺2016��8��1�� 14:29:47
���ݣ�����ľ�����׷�
ע���㷨û�������ǲ����׷���ģ��
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include "raft.h"
#include "support.h"

namespace Engine {


/**
ʱ�䣺2016��8��1�� 09:09:33
���ݣ��γɣ������׷�
	������
		SliceDataStorage& storage --> ��Ƭ���ݴ���
		int distance --> ����
���ߣ�TwoSilly
*/
void generateRaft(SliceDataStorage& storage, int distance)
{
	/*������Ƭ���ݴ������е� ��Ƭ��*/
    for(unsigned int volumeIdx = 0; volumeIdx < storage.volumes.size(); volumeIdx++)
    {
		//���統ǰ��Ƭ��ĵ�����С��1 ������һ���ж�
		//�����п�
        if (storage.volumes[volumeIdx].layers_.size() < 1) continue;
		
		//��Ƭ�������еĵ�һ��
        SliceLayer* layer = &storage.volumes[volumeIdx].layers_[0];

		//������Ƭ���еĵ�һ���е� ��������
        for(unsigned int i=0; i<layer->parts.size(); i++)
        {
			//�����һ�����������ֵ��  distance ֵ�ǵ�ǰ�����Ŵ���ٱ���
            storage.raftOutline = storage.raftOutline.unionPolygons(layer->parts[i].outline.offset(distance));
        }
    }


    SupportPolyGenerator supportGenerator(storage.support, 0);
    storage.raftOutline = storage.raftOutline.unionPolygons(supportGenerator.polygons_.offset(distance));
    storage.raftOutline = storage.raftOutline.unionPolygons(storage.wipeTower.offset(distance));
}

}//namespace Engine
