/**

�ļ�����support.h

ʱ�䣺2016��7��29�� 11:44:56
���ݣ�֧��ģ��
�Ķ�����˽�г�Ա��ǰ���ӡ�_��private,��ʾ˽�У����ݳ�Ա�ͳ�Ա����������ˣ�
ע����ģ���㷨�һ����Ǻ���⣬����ʱ���ϵ����ʱ������
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/
#ifndef SUPPORT_H
#define SUPPORT_H

#include "sliceDataStorage.h"
#include "SupportDataStorage.h"
#include "optimizedModel.h"

namespace Engine {
	/**
	ʱ�䣺2016��7��29�� 11:47:55
	���ݣ��γ�֧�Ÿ���
		������
			SupportStorage& storage --> ֧�ŵ����ݴ���
			OptimizedModel* om --> �Ż����ģ��
			int supportAngle --> ֧�ŽǶ�
			bool supportEverywhere --> �Ƿ����ж�֧��
			int supportXYDistance --> ֧��xy�ľ���
			int supportZDistance --> ֧��z�ľ���
	���ߣ�TwoSilly
	*/
void generateSupportGrid(SupportStorage& storage, OptimizedModel* om, int supportAngle, bool supportEverywhere, int supportXYDistance, int supportZDistance);


	/**
	ʱ�䣺2016��7��29�� 14:53:06
	���ݣ�֧�Ŷ���δ�����
		���ݳ�Ա��
			public:
				Polygons polygons;//�����
			private:
				SupportStorage& _storage;//֧�Ŵ���
				double			_cosAngle; //�����Ƕ�
				int32_t			_z; //z
				int				_supportZDistance;//֧��z��ľ���
				bool			_everywhere;//�Ƿ񵽴�����
				int*			_done;//����ɵ�; �����; �Ϻ����ǵ�; �Ϻ���ص�
	���ߣ�TwoSilly
	*/
class SupportPolyGenerator
{
public:
    Polygons polygons_;

private:
    SupportStorage& _storage;//֧�Ŵ���
    double _cosAngle; //�����Ƕ�
    int32_t _z; //z
    int _supportZDistance;//֧��z��ľ���
    bool _everywhere;//�Ƿ񵽴�����
    int* _done;//����ɵ�; �����; �Ϻ����ǵ�; �Ϻ���ص�

	/**
	ʱ�䣺2016��7��29�� 15:05:47
	���ݣ���Ҫ֧����ĳ��
	������
	Point p
	���ߣ�TwoSilly
	*/
    bool _needSupportAt(Point p);
   

	/**
	ʱ�䣺2016��7��29�� 15:09:28
	���ݣ���������
	������
		Point startPoint
	���ߣ�TwoSilly
	*/
	void _lazyFill(Point startPoint);
    
public:
	
	/**
	ʱ�䣺2016��7��29�� 17:45:35
	���ݣ�����֧�Ŷ���εĹ��캯��
	������
		SupportStorage& storage //֧�ŵĴ���
		int32_t z //z
	���ߣ�TwoSilly
	*/
    SupportPolyGenerator(SupportStorage& storage, int32_t z);
};

}//namespace Engine

#endif//SUPPORT_H
