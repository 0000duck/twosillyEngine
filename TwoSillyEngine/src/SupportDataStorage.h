/**

�ļ�����SupportDataStorage.h

ʱ�䣺2016��7��4�� 09:29:54
���ݣ�֧�ŵĴ����֧࣬�ŵĴ�������ݽṹ
���ߣ�TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef SUPPORT_DATA_STORAGE_H
#define SUPPORT_DATA_STORAGE_H

namespace Engine{

	/**
	ʱ�䣺2016��7��11�� 09:25:46
	���ݣ�֧�ŵ�
	���ߣ�TwoSilly
	*/
class SupportPoint
{
public:
    int32_t z;
    double cosAngle;
    
    SupportPoint(int32_t z, double cosAngle) : z(z), cosAngle(cosAngle) {}
};

	/**
	ʱ�䣺2016��7��11�� 09:25:46
	���ݣ�֧�Ŵ���ṹ��
		���ݳ�Ա��
		bool generated;//����
		int angle;//�Ƕ�
		bool everywhere;//����
		int XYDistance;//XY����
		int ZDistance;//Z����

		Point gridOffset;//����ƫ��
		int32_t gridScale;//����������;
		int32_t gridWidth, gridHeight; //����� �����
		vector<SupportPoint>* grid;//����
	���ߣ�TwoSilly
	*/
class SupportStorage
{
public:
    bool generated;//�Ƿ񴴽�
    int angle;//�Ƕ�
    bool everywhere;//�Ƿ񵽴�
    int XYDistance;//XY����
    int ZDistance;//Z����
    
    Point gridOffset;//����ƫ��
    int32_t gridScale;//����������;����;
    int32_t gridWidth, gridHeight; //����� �����
    vector<SupportPoint>* grid;//����

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