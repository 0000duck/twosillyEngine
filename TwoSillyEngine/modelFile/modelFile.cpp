/**
文件名：IntModelFile.cpp

时间：2016年7月5日 11:03:53
内容：model加载文件！用于加载3d模型的类
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/
#include <string.h>
#include "../tool/settings.h"
#include <stdio.h>

#include "ModelFile.h"
#include "../tool/logoutput.h"
#include "../tool/string.h"

FILE* binaryMeshBlob = nullptr; //二元网格块（文件指针）

//LogOutput *LOG = LogOutput::getInstance();

/*  
    自定义函数的支持Mac线两端的ASCII STL文件。当应用在Mac OpenSCAD产生
*/
/**
时间：2016年7月8日 16:51:12
内容：从文件中获取字符串，遇到 "换行" 或 "制表符" 停止并在后位补 "\0" （没有遇到就读取len长度的字符）
	参数：
		char* ptr --> 获取出的字符串
		size_t len -->获取的长度
		FILE* f --> 需要获取的目标文件
作者：TwoSilly
*/
void* fGetStr_(char* ptr, size_t len, FILE* f)
{
    while(len && fread(ptr, 1, 1, f) > 0)
    {
        if (*ptr == '\n' || *ptr == '\r')
        {
            *ptr = '\0';
            return ptr;
        }
        ptr++;
        len--;
    }
    return nullptr;
}
/**
时间：2016年7月8日 16:48:25
内容：加载STL“ascii”模型
	参数：
		IntModelFile *model --> 一个简单的模型是一个具有1个或更多的三维体积的三维模型
		const char* filename --> 文件名 即：需要加载的文件名
		FloatMatrix3x3& matrix --> 变换矩形
作者：TwoSilly

*/
IntModelFile* loadModelSTL_ascii(IntModelFile *model,const char* filename, FloatMatrix3x3& matrix)
{
    model->volumes.push_back(IntFaceVolume());
    
    IntFaceVolume* vol = &model->volumes[model->volumes.size()-1];
    FILE* f = nullptr;
	fopen_s(&f,filename, "rt"); // rt 只读打开一个文本文件，只允许读数据
    char buffer[1024];
    FloatPoint3 vertex;
    int n = 0;
    Point3 v0(0,0,0), v1(0,0,0), v2(0,0,0);
    while(fGetStr_(buffer, sizeof(buffer), f))
    {
		//sscanf() - 从一个字符串中读进与指定格式相符的数据。
		/*vs 上这里的函数改为 sscanf_s*/
        if (sscanf(buffer, " vertex %lf %lf %lf", &vertex.x, &vertex.y, &vertex.z) == 3)
        {
            n++;
            switch(n)
            {
            case 1:
                v0 = matrix.apply(vertex);
                break;
            case 2:
                v1 = matrix.apply(vertex);
                break;
            case 3:
                v2 = matrix.apply(vertex);
                vol->addFace(v0, v1, v2);
                n = 0;
                break;
            }
        }
    }
    fclose(f);
    return model;
}

/**
时间：2016年7月8日 16:48:25
内容：加载STL“binary”模型
	参数：
		IntModelFile *model --> 一个简单的模型是一个具有1个或更多的三维体积的三维模型
		const char* filename --> 文件名 即：需要加载的文件名
		FloatMatrix3x3& matrix --> 变换矩形
作者：TwoSilly

*/
IntModelFile* loadModelSTL_binary(IntModelFile *model,const char* filename, FloatMatrix3x3& matrix)
{
	//'rb'只读打开一个二进制文件，只允许读数据
    FILE* f = nullptr;
	fopen_s(&f,filename, "rb");
    char buffer[80]; //这是一个中间变量 不需要的数据取出来 如：模型标题，面的法矢量等
    uint32_t faceCount;//面的计数
    //忽略标题
    if (fread(buffer, 80, 1, f) != 1)
    {
        fclose(f);
        return nullptr;
    }
    //读取表面计数
    if (fread(&faceCount, sizeof(uint32_t), 1, f) != 1)
    {
        fclose(f);
        return nullptr;
    }
    //读取每个面：
    //float(x,y,z) = normal, float(X,Y,Z)*3 = vertexes, uint16_t = flags
    model->volumes.push_back(IntFaceVolume());//构建一个简单模型数据  里面是存面的 （里面只有一个面的链表）
    IntFaceVolume* vol = &model->volumes[model->volumes.size()-1];
	if(vol == nullptr)
	{
		fclose(f);
		return nullptr;
	}

    for(unsigned int i=0;i<faceCount;i++)
    {
        if (fread(buffer, sizeof(float) * 3, 1, f) != 1)//读取失败 执行下一句 关闭 f 打开的文件（model）//三角面片发矢量
        {
            fclose(f);
            return nullptr;
        }
        float v[9];
        if (fread(v, sizeof(float) * 9, 1, f) != 1)//三角面的坐标
        {
            fclose(f);
            return nullptr;
        }
        Point3 v0 = matrix.apply(FloatPoint3(v[0], v[1], v[2]));
        Point3 v1 = matrix.apply(FloatPoint3(v[3], v[4], v[5]));
        Point3 v2 = matrix.apply(FloatPoint3(v[6], v[7], v[8]));
        vol->addFace(v0, v1, v2);

        if (fread(buffer, sizeof(uint16_t), 1, f) != 1) //描述三角面片的属性信息。
        {
            fclose(f);
            return nullptr;
        }
    }
    fclose(f);
    return model;
}

/**
时间：2016年7月8日 16:31:34
内容：加载STL模型在里面会做一个判断是“ascii”还是“binary”并分别调取他们的加载函数
	参数：
		IntModelFile *model --> 一个简单的模型是一个具有1个或更多的三维体积的三维模型
		const char* filename --> 文件名 即：需要加载的文件名
		FloatMatrix3x3& matrix --> 变换矩形
作者：TwoSilly

*/
IntModelFile* loadModelSTL(IntModelFile *model,const char* filename, FloatMatrix3x3& matrix)
{
    FILE* f = nullptr;
	fopen_s(&f,filename, "r");
    char buffer[6];
    if (f == nullptr)
        return nullptr;

    if (fread(buffer, 5, 1, f) != 1) //读取stl模型文件名，判断她是否存在
    {
        fclose(f);
        return nullptr;
    }
    fclose(f);

    buffer[5] = '\0';
    if (stringcasecompare(buffer, "solid") == 0) //判断我需要加载的STL模型是不是 ascii
    {
        IntModelFile* asciiModel = loadModelSTL_ascii(model, filename, matrix);
        if (!asciiModel)
            return nullptr;

          //这个逻辑是用来处理文件开始的情况下
         //“固体”，但是一个二进制文件。
        if (model->volumes[model->volumes.size()-1].faces.size() < 1)
        {
            model->volumes.erase(model->volumes.end() - 1);
            return loadModelSTL_binary(model, filename, matrix);
        }
        return asciiModel;
    }
    return loadModelSTL_binary(model, filename, matrix);
}


/*
时间：2016年7月5日 09:32:01
内容：从文件加载模型
	参数：
	IntModelFile* model --> 一个模型指针用于储存加载后的模型文件
	const char* filename --> 文件名，需要加载的模型的文件名（他是根据名字加载的）
	FloatMatrix3x3& matrix --> 3x3的变幻矩形
作者：TwoSilly
*/
IntModelFile* loadModelFromFile(IntModelFile *model,const char* filename, FloatMatrix3x3& matrix)
{
	//strrchr() 函数查找字符在指定字符串中从正面开始的最后一次出现的位置，
	//如果成功，则返回指定字符最后一次出现位置的地址，如果失败，则返回 false 
    const char* ext = strrchr(filename, '.');//得到文件后缀

    if (ext && stringcasecompare(ext, ".stl") == 0) //判断是否是stl模型
    {
        return loadModelSTL(model,filename, matrix);
    }
    if (filename[0] == '#' && binaryMeshBlob != nullptr)
    {
        while(*filename == '#')
        {
            filename++;

            model->volumes.push_back(IntFaceVolume());
            IntFaceVolume* vol = &model->volumes[model->volumes.size()-1];
            int32_t n, pNr = 0;
            if (fread(&n, 1, sizeof(int32_t), binaryMeshBlob) < 1)
                return nullptr;
            Engine::log("Reading mesh from binary blob with %i vertexes\n", n);
            Point3 v[3];
            while(n)
            {
                float f[3];
                if (fread(f, 3, sizeof(float), binaryMeshBlob) < 1)
                    return nullptr;
                FloatPoint3 fp(f[0], f[1], f[2]);
                v[pNr++] = matrix.apply(fp);
                if (pNr == 3)
                {
                    vol->addFace(v[0], v[1], v[2]);
                    pNr = 0;
                }
                n--;
            }
        }
        return model;
    }
    return nullptr;
}
