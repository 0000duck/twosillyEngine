/**
文件名：IntModelFile.h

时间：2016年7月4日 09:29:54
内容：model加载文件！用于加载3d模型的类
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/
#ifndef MODELFILE_H
#define MODELFILE_H
/**

IntModelFile包含切片模型装载机。该模型装载机将任何格式，它可以读取到一个列表的三角形与3个X / Y / Z点。
返回的格式是一个模型类，一个数组的面，其中有一个分辨率为1微米的整数点。给出一个4米的最大对象大小。
**/

#include <vector>
using std::vector;
#include "../utils/intpoint.h"
#include "../utils/floatpoint.h"

extern FILE* binaryMeshBlob; //二元网格块（文件指针）

//把第一个参数设置为两个中最小的一个
#define SET_MIN(n, m) do { if ((m) < (n)) n = m; } while(0)

//把第一个参数设置成两个中最大的一个
#define SET_MAX(n, m) do { if ((m) > (n)) n = m; } while(0)

/**
时间：2016年7月4日 13:42:29
内容:这是一个简单面类！里面有3个点！传进来后数值都被准换为int型
作者：TwoSilly
*/
class IntFace
{
public:
    Point3 v[3];

    IntFace(Point3& v0, Point3& v1, Point3& v2) { v[0] = v0; v[1] = v1; v[2] = v2; }
};

/* 
时间：2016年7月4日 14:09:15
内容： 一个简单的体积是一个三维模型最基本的表达。
		它包含所有的面作为简单的三角形，没有什么花哨的东西。 
		即：这个类有一个数据成员vector<IntFace> faces;这个链表里存储
			的是简单面的链表
原：SimpleVolume
作者：TwoSilly
   
*/
class IntFaceVolume 
{
public:
    vector<IntFace> faces;

	//添加一个面
    void addFace(Point3& v0, Point3& v1, Point3& v2)
    {
        faces.push_back(IntFace(v0, v1, v2));
    }


	//找出这个volume链表中最小的那个x，y，z，把他们返回出来
    Point3 Min()
    {
        if (faces.size() < 1)
            return Point3(0, 0, 0);
        Point3 ret = faces[0].v[0];
        for(unsigned int i=0; i<faces.size(); i++)
        {
            SET_MIN(ret.x, faces[i].v[0].x);
            SET_MIN(ret.y, faces[i].v[0].y);
            SET_MIN(ret.z, faces[i].v[0].z);
            
            SET_MIN(ret.x, faces[i].v[1].x);
            SET_MIN(ret.y, faces[i].v[1].y);
            SET_MIN(ret.z, faces[i].v[1].z);
            
            SET_MIN(ret.x, faces[i].v[2].x);
            SET_MIN(ret.y, faces[i].v[2].y);
            SET_MIN(ret.z, faces[i].v[2].z);
        }
        return ret;
    }

	//找出这个volume链表中最大的那个x，y，z，把他们返回出来
    Point3 Max()
    {
        if (faces.size() < 1)
            return Point3(0, 0, 0);
        Point3 ret = faces[0].v[0];
        for(unsigned int i=0; i<faces.size(); i++)
        {
            SET_MAX(ret.x, faces[i].v[0].x);
            SET_MAX(ret.y, faces[i].v[0].y);
            SET_MAX(ret.z, faces[i].v[0].z);
            
            SET_MAX(ret.x, faces[i].v[1].x);
            SET_MAX(ret.y, faces[i].v[1].y);
            SET_MAX(ret.z, faces[i].v[1].z);
            
            SET_MAX(ret.x, faces[i].v[2].x);
            SET_MAX(ret.y, faces[i].v[2].y);
            SET_MAX(ret.z, faces[i].v[2].z);
        }
        return ret;
    }
};


//一个简单的模型是一个具有1个或更多的三维体积的三维模型
//原：SimpleModel
class IntModelFile
{
public:
    vector<IntFaceVolume> volumes;

	//找出这个简单模型中的最 低 点（x，y，z）
    Point3 Min()
    {
        if (volumes.size() < 1)
            return Point3(0, 0, 0);
        Point3 ret = volumes[0].Min();
        for(unsigned int i=0; i<volumes.size(); i++)
        {
            Point3 v = volumes[i].Min();
            SET_MIN(ret.x, v.x);
            SET_MIN(ret.y, v.y);
            SET_MIN(ret.z, v.z);
        }
        return ret;
    }

	//找出这个简单模型中的最 高 点（x，y，z）
    Point3 Max()
    {
        if (volumes.size() < 1)
            return Point3(0, 0, 0);
        Point3 ret = volumes[0].Max();
        for(unsigned int i=0; i<volumes.size(); i++)
        {
            Point3 v = volumes[i].Max();
            SET_MAX(ret.x, v.x);
            SET_MAX(ret.y, v.y);
            SET_MAX(ret.z, v.z);
        }
        return ret;
    }
};

/*
时间：2016年7月5日 09:32:01
内容：加载模型文件
	参数：
	IntModelFile* m --> 一个模型指针用于储存加载后的模型文件
	const char* filename --> 文件名，需要加载的模型的文件名（他是根据名字加载的）
	FloatMatrix3x3& matrix --> 3x3的变幻矩形
作者：TwoSilly
*/
IntModelFile* loadModelFromFile(IntModelFile*m,const char* filename, FloatMatrix3x3& matrix);

#endif//MODELFILE_H
