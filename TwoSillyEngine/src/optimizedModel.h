/**

文件名：bridge.h

时间：2016年7月12日 11:34:38
内容：优化模型
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef OPTIMIZED_MODEL_H
#define OPTIMIZED_MODEL_H

#include <map>
#include "../modelFile/modelFile.h"
#include "../tool/settings.h"
#include "../utils/Config.h"

#include "../tool/logoutput.h"
/**
时间：2016年7月12日 11:45:58
内容：优化面的类（结构体）
	成员：
	int index[3];		//编号
	int touching[3];	//关于，至于
作者：TwoSilly
*/
struct OptimizedFace
{

    int index[3];
    int touching[3];
};

/**
时间：2016年7月12日 11:45:58
内容：优化的三维点
	数据成员：
		Point3 p;
		vector<uint32_t> faceIndexList; //面的编号（索引）链表
	
作者：TwoSilly
*/
class OptimizedPoint3
{ 
public:
    Point3 p;
    vector<uint32_t> faceIndexList; //面的编号（索引）链表

    OptimizedPoint3(Point3 p): p(p) {}
};

//优化模型 
//这里只有声明，类似头文件与.cpp的先声明后使用
//可用于交叉引用
class OptimizedModel;


/**
时间：2016年7月12日 12:02:28
内容：优化量
	数据成员：
	OptimizedModel* model; //优化模型
	vector<OptimizedPoint3> points; //优化的3维点的链表
	vector<OptimizedFace> faces; //优化的面
	
作者：TwoSilly
*/
class OptimizedVolume
{
public:
    OptimizedModel* model; //优化模型
    vector<OptimizedPoint3> points; //优化的3维点的链表
    vector<OptimizedFace> faces; //优化的面

	/*
	时间：2016年7月12日 13:38:16
	内容：优化的体积的构造函数
		参数：
			IntFaceVolume* volume --> 一个简单面的储存量
			OptimizedModel* model --> 优化后的模型
	作者：TwoSilly
	*/
    OptimizedVolume(IntFaceVolume* volume, OptimizedModel* model);

	/*
	时间：2016年7月12日 13:38:16
	内容：得到面的索引和点
		即：找到第一个索引和第二个索引相同且不等于 第三个点的数！
		参数：
			int idx0 -->
			int idx1 --> 
			int notFaceIdx --> 没有面的索引
	作者：TwoSilly
	*/
    int getFaceIdxWithPoints(int idx0, int idx1, int notFaceIdx)
    {
		//遍历传入索引 idx0 所在优化3维点中的 面的索引链表
        for(unsigned int i=0;i<points[idx0].faceIndexList.size();i++)
        {
            int f0 = points[idx0].faceIndexList[i];
            if (f0 == notFaceIdx) continue;
            for(unsigned int j=0;j<points[idx1].faceIndexList.size();j++)
            {
                int f1 = points[idx1].faceIndexList[j];
                if (f1 == notFaceIdx) continue;
                if (f0 == f1) return f0;
            }
        }
        return -1;
    }
};

	/*
	时间：2016年7月13日 08:37:35
	内容：优化模型
		数据成员：
		vector<OptimizedVolume> volumes_; --> 优化的体积
		Point3 modelSize_; -->	模型的大小
		Point3 vMin_, vMax_; --> 最小最大点
	作者：TwoSilly
	*/
class OptimizedModel
{
public:
    vector<OptimizedVolume> volumes_; //优化模型的量 vector
    Point3 modelSize_; //模型大小
    Point3 vMin_, vMax_; //模型最小与最大

	/*
	时间：2016年7月12日 13:38:16
	内容：优化模型的构造函数
		参数：
			IntModelFile* model --> 简单的模型文件
			Point3 center --> 中心点
			
	作者：TwoSilly
	*/
    OptimizedModel(IntModelFile* model, Point3 center)
    {
		//将传入模型优化后 压入优化体积链表中
        for(unsigned int i=0; i<model->volumes.size(); i++)
            volumes_.push_back(OptimizedVolume(&model->volumes[i], this));

        vMin_ = model->Min();
        vMax_ = model->Max();
		modelSize_ = vMax_ - vMin_; //得到模型大小

		//偏移量的设置（它 == 模型的最小点加最大点的一半，z坐标不变）其实现在得到的是模型实际的中心点
        Point3 vOffset((vMin_.x + vMax_.x) / 2, (vMin_.y + vMax_.y) / 2, vMin_.z);

        if(Config::config->autoCenter != 1)
        {
            vOffset.x = 0;
            vOffset.y = 0;
            if(Config::config->autoCenter == 2)
                vOffset.z = 0;
        }

		//用偏移量（模型实际的中心点）减去 传入的中心点计算出实际的偏移量
        vOffset -= center;

		/**
		时间：2016年7月13日 09:14:44
		内容：其实这里应该可以做一个优化 即：当偏移为0时就是计算出的 中心点等于传入的中心点时
				下面这些都是可以不操作的！
				所以：我在这里加一个if判断 已试图优化当偏移为0时的操作
		作者：TwoSilly
		*/
		
		Engine::logError("这里可以优化，不过为了保证不是我改错了，暂时不改它！\n位置：OptimizedModel(IntModelFile* model, Point3 center) ");
		
			//对当前模型做偏移
			for(unsigned int i=0; i<volumes_.size(); i++)
				for(unsigned int n=0; n<volumes_[i].points.size(); n++)
					volumes_[i].points[n].p -= vOffset;


			vMin_ -= vOffset; //对模型最小值做一个偏移
			vMax_ -= vOffset; //对模型最小值做一个偏移
		

    }

	/**
	时间：2016年7月13日 09:21:19
	内容：节省调试STL
		参数;
			const char* filename --> 文件名
		问题：这个好像是用来调试时输出 传入的模型文件的！不过我感觉有bug，
				他只输出了volumes_[0] 的模型，应该优化一下
	作者：TwoSilly
	*/
    void saveDebugSTL(const char* filename);
};

#endif//OPTIMIZED_MODEL_H
