/**

文件名：bridge.cpp

时间：2016年7月12日 11:34:38
内容：优化模型
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/
#include <stdio.h>

#include "../tool/gettime.h"
#include "../tool/logoutput.h"
#include "optimizedModel.h"

/**
时间：2016年7月12日 13:43:26
内容：这里的0.03是一个约定值，若模型中点的距离在0.03毫米内
作者：TwoSilly
*/
#define MELD_DIST MM2INT(0.03)


	/*
	时间：2016年7月12日 13:38:16
	内容：优化的体积的构造函数
		参数：
			IntFaceVolume* volume --> 一个简单面的储存量
			OptimizedModel* model --> 优化后的模型
	作者：TwoSilly
	*/
OptimizedVolume::OptimizedVolume(IntFaceVolume* volume, OptimizedModel* model)
: model(model)
{
    //reserve stl 指定链表的容量
    //点的数量是面的3倍
    points.reserve(volume->faces.size() * 3);
    
	//面的数量不会小于传入volume的面的量
    faces.reserve(volume->faces.size());

	//索引的map
    std::map<uint32_t, std::vector<uint32_t> > indexMap;
    
    double t = getTime();//得到当前时间


	//遍历传入模型的每个面
    for(uint32_t i=0; i<volume->faces.size(); i++)
    {
        OptimizedFace f; //存取一个简单的优化面
		//其实这跟优化没有关系只是为了调试每隔1000次打印一次输出面
        if((i%1000==0) && (getTime()-t)>2.0)
            Engine::logProgress("optimized", i + 1, volume->faces.size());
        
		//遍历每个面上的三个点
        for(uint32_t j=0; j<3; j++)
        {
			//存取面上的3个点（每次一个）
            Point3 p = volume->faces[i].v[j];
            /*
            时间：2016年7月12日 13:40:38
            内容：这里是为了找到，3维点的一个特有的哈希值（如果点的差距在0.03内哈希值相同）
			作者：TwoSilly
            */            
            //MELD_DIST 合并分配
            int hash = ((p.x + MELD_DIST/2) / MELD_DIST) ^ (((p.y + MELD_DIST/2) / MELD_DIST) << 10) ^ (((p.z + MELD_DIST/2) / MELD_DIST) << 20);
            
            uint32_t idx; //一个索引（无符号int型）
            bool add = true; //是否添加的开关
           
			//加入当前点（哈希值）已近存在遍历它
            for(unsigned int n = 0; n < indexMap[hash].size(); n++)
            {
                //判断当前哈希值中的点与当前点的 差值 是否在0.03mm内
				//如果是那么 索引 idx 用当前哈希值的点的索引赋值 并且 关闭添加开关
                if ((points[indexMap[hash][n]].p - p).testLength(MELD_DIST))
                {
                    /*
                        时间：2016年6月23日 11:09:32
                        内容：我发现下面这个赋值是没有意义的所以我把他注释掉了！
                        作者：TwoSilly
                        我的错！这个是必须的！当他的值变化小于0.03时就会需要他！我的错不能注掉他！
                    */
                    idx = indexMap[hash][n]; 
                    add = false;
                    break;
                }
            }

			//若添加开关为ture， 
			//索引 idx 用当前优化的3维点的链表 points的大小即（链表的最后一个元素的下一个下标）赋值
			//	并且把该 idx 压入 当前哈希值的map中 
			//把当前点压入 优化的3维点的链表
            if (add)
            {
               /*
                时间：2016年6月21日 14:05:42
                内容：这里的把点的大小添加到indexmap中 可以做一个简单优化
                内容：TwoSilly
                原：indexMap[hash].push_back(points.size());
                    idx = points.size();
                */
                idx = points.size();
                indexMap[hash].push_back(idx);
                
                
                points.push_back(p);
            }
			//当前点在优化的3维点的链表中的 索引
            f.index[j] = idx;
        }
		//判断这个面中的三个点相互不相同（即为一个三角面而不是线之类的）
        if (f.index[0] != f.index[1] && f.index[0] != f.index[2] && f.index[1] != f.index[2])
        {
			/*
			将每个点（一个面的）分别加入当前所在 优化的3维点的链表 的 面的索引链表
			*/

            points[f.index[0]].faceIndexList.push_back(faces.size());
            points[f.index[1]].faceIndexList.push_back(faces.size());
            points[f.index[2]].faceIndexList.push_back(faces.size());
			//将该三角面压入 优化面中
            faces.push_back(f);
  
        }
    }
   

    int openFacesCount = 0;
    for(unsigned int i=0;i<faces.size();i++)
    {
        OptimizedFace* f = &faces[i];
        f->touching[0] = getFaceIdxWithPoints(f->index[0], f->index[1], i);
        f->touching[1] = getFaceIdxWithPoints(f->index[1], f->index[2], i);
        f->touching[2] = getFaceIdxWithPoints(f->index[2], f->index[0], i);
        if (f->touching[0] == -1)
            openFacesCount++;
        if (f->touching[1] == -1)
            openFacesCount++;
        if (f->touching[2] == -1)
            openFacesCount++;
    }
	Engine::logError( "  Number of open faces: %i\n", openFacesCount);
    //fprintf(stdout, "  Number of open faces: %i\n", openFacesCount);
}


	/**
	时间：2016年7月13日 09:21:19
	内容：节省调试STL
		参数;
			const char* filename --> 文件名
	作者：TwoSilly
	*/
void OptimizedModel::saveDebugSTL(const char* filename)
{
    char buffer[80] = "TwoSilly_Engine_STL_export";
    uint32_t n;
    uint16_t s;
    double  flt;
    OptimizedVolume* vol = &volumes_[0];
    FILE* f = nullptr;
		fopen_s(&f,filename, "wb"); //只写打开或新建一个二进制文件；只允许写数据
    fwrite(buffer, 80, 1, f);
    n = vol->faces.size();
    fwrite(&n, sizeof(n), 1, f);
    for(unsigned int i=0;i<vol->faces.size();i++)
    {
        flt = 0;
        s = 0;
        fwrite(&flt, sizeof(flt), 1, f);
        fwrite(&flt, sizeof(flt), 1, f);
        fwrite(&flt, sizeof(flt), 1, f);

        flt = INT2MM(vol->points[vol->faces[i].index[0]].p.x); fwrite(&flt, sizeof(flt), 1, f);
        flt = INT2MM(vol->points[vol->faces[i].index[0]].p.y); fwrite(&flt, sizeof(flt), 1, f);
        flt = INT2MM(vol->points[vol->faces[i].index[0]].p.z); fwrite(&flt, sizeof(flt), 1, f);
        flt = INT2MM(vol->points[vol->faces[i].index[1]].p.x); fwrite(&flt, sizeof(flt), 1, f);
        flt = INT2MM(vol->points[vol->faces[i].index[1]].p.y); fwrite(&flt, sizeof(flt), 1, f);
        flt = INT2MM(vol->points[vol->faces[i].index[1]].p.z); fwrite(&flt, sizeof(flt), 1, f);
        flt = INT2MM(vol->points[vol->faces[i].index[2]].p.x); fwrite(&flt, sizeof(flt), 1, f);
        flt = INT2MM(vol->points[vol->faces[i].index[2]].p.y); fwrite(&flt, sizeof(flt), 1, f);
        flt = INT2MM(vol->points[vol->faces[i].index[2]].p.z); fwrite(&flt, sizeof(flt), 1, f);

        fwrite(&s, sizeof(s), 1, f);
    }
    fclose(f);
}
