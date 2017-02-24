/*
�ļ�����Beegive.h
ʱ�䣺2016��12��30��16:24:34
���ݣ����Ƿ��Ѵ����࣬���Լ�д�ģ����ƻ��кܶ����⣬�Ȳ���
���ߣ�TwoSilly
*/
#ifndef BEEGIVE_H
#define BEEGIVE_H

#include <vector>
#include <iostream>
#include <math.h>
//#include "../utils/intpoint.h"
#include "../utils/polygon.h"



#define SIN60 sin(M_PI / 3) //sin(60)
#define SIN30 sin(M_PI / 6) //sin(30)
using namespace std;
namespace Engine {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


	//class Point;
	class PolygonRef;
	class Polygon;
	class Polygons;
	//class Point;
	/*
	ʱ�䣺2016��12��30��11:00:45
	���ݣ������������� ������ͨ�������Ϊ ��һ������һ���������� �߳�Ϊ�ڶ���������Ĭ��Ϊ6��
	������
	Point _min --> �������ε���ʼ��
	int side = 6 --> �������εı߳�
	���ߣ�TwoSilly
	*/
	Polygon CreateBeegive(Point pos, int side);

	/*
	ʱ�䣺2017��1��3��11:10:29
	���ݣ������������� ������ͨ�������Ϊ ��һ������һ���������� �߳�Ϊ�ڶ���������Ĭ��Ϊ6��
	������
	Point _min --> �������ε���ʼ��
	int side = 6 --> �������εı߳�
	���ߣ�TwoSilly
	*/
	Polygon CreateBeegiveTwo(Point pos, int side);
 
	/*
	ʱ�䣺2016��12��30��11:00:45
	���ݣ������������� ��������ͨ������ ��С�������� һ��ȫ �������ľ���
	������
	Point min_ --> �������ε���ʼ��
	Point maX --> �������ε����߽��
	int side = 6 --> �������εı߳�
	���ߣ�TwoSilly
	*/
 bool CreateBeegiveList(Polygons &List, Point min_, Point max_, int side);

 /*
 ʱ�䣺2017��1��3��17:01:01
 ���ݣ��ǵ� ����ǰ����㷨�����ⲻ����������ǸĽ���,��ƫ��
 ������
 Point min_ --> �������ε���ʼ��
 Point max_ --> �������εĽ�����
 int side  --> �������εı߳�
 int dir --> ����������ƫ�� 0 ���� 1����
 ���ߣ�TwoSilly
 */
 Polygon CreateBeegiveLineRight(Point min_, Point max_, int side);

 /*
 ʱ�䣺2017��1��3��17:01:01
 ���ݣ��ǵ� ����ǰ����㷨�����ⲻ����������ǸĽ���,��ƫ��
 ������
 Point min_ --> �������ε���ʼ��
 Point max_ --> �������εĽ�����
 int side  --> �������εı߳�
 int dir --> ����������ƫ�� 0 ���� 1����
 ���ߣ�TwoSilly
 */
 Polygon CreateBeegiveLineLeft(Point min_, Point max_,Point pos, int side);
 /*
 ʱ�䣺2016��12��30��11:00:45
 ���ݣ������������� ��������ͨ������ ��С�������� һ��ȫ �������ľ���
 ������
 Point min_ --> �������ε���ʼ��
 Point maX --> �������ε����߽��
 int side = 6 --> �������εı߳�
 ���ߣ�TwoSilly
 */
 bool CreateBeegiveLineList(Polygons &List, Point min_, Point max_, int side);


}
#endif //BEEGIVE_H