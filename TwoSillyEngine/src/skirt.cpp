/**

�ļ�����skirt.cpp

ʱ�䣺2016��7��28�� 15:32:42
���ݣ���Ե��������Ե
ע�����ǲ���ȹ�ߣ���Ե��ģ�飬�����㷨��ûϸ�����ͺ�
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/

#include "skirt.h"
#include "support.h"

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
void generateSkirt(SliceDataStorage& storage, int distance, int extrusionWidth, int count, int minLength, int initialLayerHeight)
{
	//�ж��Ƿ���������
    bool externalOnly = (distance > 0);

    for(int skirtNr=0; skirtNr<count;skirtNr++)
    {
		//ƫ�ƾ���
        int offsetDistance = distance + extrusionWidth * skirtNr + extrusionWidth / 2;
        
        SupportPolyGenerator supportGenerator(storage.support, initialLayerHeight);
        Polygons skirtPolygons(storage.wipeTower.offset(offsetDistance));
        for(unsigned int volumeIdx = 0; volumeIdx < storage.volumes.size(); volumeIdx++)
        {
            if (storage.volumes[volumeIdx].layers_.size() < 1) continue;
            SliceLayer* layer = &storage.volumes[volumeIdx].layers_[0];
            for(unsigned int i=0; i<layer->parts.size(); i++)
            {
                if (externalOnly)
                {
                    Polygons p;
                    p.add(layer->parts[i].outline[0]);
                    skirtPolygons = skirtPolygons.unionPolygons(p.offset(offsetDistance));
                }
                else
                    skirtPolygons = skirtPolygons.unionPolygons(layer->parts[i].outline.offset(offsetDistance));

                supportGenerator.polygons_ = supportGenerator.polygons_.difference(layer->parts[i].outline);
            }
        }
        
        //��ͬ������֧Ԯ�����С���ֳ�ȥ���Ķ����ƽ��һ�㡣
        supportGenerator.polygons_ = supportGenerator.polygons_.offset(-extrusionWidth * 3);
        supportGenerator.polygons_ = supportGenerator.polygons_.offset(extrusionWidth * 3);
        skirtPolygons = skirtPolygons.unionPolygons(supportGenerator.polygons_.offset(offsetDistance));

		//����С����ȹ�ס������и�ֵ����ɾ���κ�С��100x�����������
        for(unsigned int n=0; n<skirtPolygons.size(); n++)
        {
            double area = skirtPolygons[n].area();
            if (area < 0 && area > -extrusionWidth * extrusionWidth * 100)
                skirtPolygons.remove(n--);
        }

        storage.skirt.add(skirtPolygons);
        /**
		ʱ�䣺2016��8��1�� 08:42:05
		���ݣ���������һ���򵥵ķŴ�Ϊ�����η��˵Ľ���
			ԭ��int lenght = storage.skirt.polygonLength();
		���ߣ�TwoSilly
		*/
        int64_t lenght = storage.skirt.polygonLength();
        if (skirtNr + 1 >= count && lenght > 0 && lenght < minLength)
            count++;
    }
}

}//namespace Engine
