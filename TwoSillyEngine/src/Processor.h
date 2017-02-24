#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <algorithm>
#include <vector>
#include "../tool/socket.h"

#include "gcodeExport.h"
#include "../utils/Config.h"
#include "../tool/gettime.h"

#include "sliceDataStorage.h"

#include "../tool/logoutput.h"
#include "../modelFile/ModelFile.h"
#include "optimizedModel.h"
#include "slicer.h"
#include "skirt.h"
#include "inset.h"
#include "skin.h"
#include "raft.h"
#include "infill.h"
#include "inset.h"
#include "comb.h"
#include "bridge.h"
#include "polygonOptimizer.h"
#include "support.h"
//#include "multiVolumes.h"
#include "pathOrderOptimizer.h"
#include "timeEstimate.h"
#include "layerPart.h"

#define GUI_CMD_REQUEST_MESH 0x01   // GUI命令要求网格
#define GUI_CMD_SEND_POLYGONS 0x02  //GUI命令发送的多边形
#define GUI_CMD_FINISH_OBJECT 0x03  //GUI命令完成目标

namespace Engine {

//装有保险丝的长丝制造 中央处理器.

class Processor
{
private:
    int _maxObjectHeight;//最大的对象的高
    int _fileNr;//文件的正常计数
    GCodeExport _gcode;//gocde输出
    Config& _config;//配置
    TimeKeeper _timeKeeper;//时间监护人
    ClientSocket _guiSocket;//gui套接字(客户端)

    GCodePathConfig _skirtConfig;//边缘配置
    GCodePathConfig _inset0Config;//插入配置
    GCodePathConfig _insetXConfig;//插入X配置
    GCodePathConfig _infillConfig;//填充配置
    GCodePathConfig _skinConfig;//表皮配置
    GCodePathConfig _supportConfig;//支撑配置
public:
    Processor(Config& _config)
    : _config(_config)
    {
        _fileNr = 1;//文件正常范围: Normal Range  
        _maxObjectHeight = 0;//最大目标高度
    }


	/**
	时间：2016年8月4日 11:34:45
	内容：gui的链接，与gui即服务器建立链接
			注：这是网络编程的，我打算摒弃它（好吧我承认我小看网络编程的，
				好强大的样子，所以前面所说的放弃他是我的错！）
		参数：
			int portNr --> 端口号

	作者：TwoSilly
	*/
    void guiConnect(int portNr);


	/**
	时间：2016年8月4日 11:34:45
	内容：发送多边形到GUI，
			注：这是网络编程的，我打算摒弃它
		参数：
			const char* name -->
			int layerNr -->
			int32_t z -->
			Polygons& polygons --> 

	作者：TwoSilly
	*/
    void sendPolygonsToGui(const char* name, int layerNr, int32_t z, Polygons& polygons);

	/**
	时间：2016年8月4日 11:34:45
	内容：设置目标文件
		参数：
			const char* filename -->

	作者：TwoSilly
	*/
    bool setTargetFile(const char* filename);

	/**
	时间：2016年8月4日 11:34:45
	内容：过程文件
		参数：
			const std::vector<std::string> &files -->

	作者：TwoSilly
	*/
    bool processFile(const std::vector<std::string> &files);

	/**
	时间：2016年8月4日 11:34:45
	内容：结束文件，跟析构函数类似的最后才调用的函数
		参数：

	作者：TwoSilly
	*/
    void finalize();

private:

	/**
	时间：2016年8月4日 11:34:45
	内容：预先的计划
		参数：

	作者：TwoSilly
	*/
    void preSetup();

	/**
	时间：2016年8月4日 11:34:45
	内容：准备模型
		参数：
		SliceDataStorage& storage -->
		const std::vector<std::string> &files -->
		他有两个模式。与gui链接模式和自运行模式！
		与gui链接就会通过gui套接字传入的数据更改模型的设置
		未与gui练级就会按默认模式准备
	作者：TwoSilly
	*/
    bool prepareModel(SliceDataStorage& storage, const std::vector<std::string> &files);

	/**
	时间：2016年8月4日 11:34:45
	内容：处理切片数据
		参数：
		SliceDataStorage& storage -->
	作者：TwoSilly
	*/
    void processSliceData(SliceDataStorage& storage);

	/**
	时间：2016年8月4日 11:34:45
	内容：写入GOcde
		参数：
		SliceDataStorage& storage -->
	作者：TwoSilly
	*/
    void writeGCode(SliceDataStorage& storage);

   

	/**
	时间：2016年8月4日 11:34:45
	内容：添加层的体积到Gocde，从一个单一的网格体积的Gocde添加一个层
		参数：
		SliceDataStorage& storage -->
		GCodePlanner& gcodeLayer -->
		int volumeIdx -->
		int layerNr -->
	作者：TwoSilly
	*/
    void addVolumeLayerToGCode(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int volumeIdx, int layerNr);
    
    

	/**
	时间：2016年8月4日 11:34:45
	内容：添加填充的gocde
		参数：
		SliceLayerPart* part -->
		GCodePlanner& gcodeLayer -->
		int layerNr -->
		int extrusionWidth -->
		int fillAngle -->
	作者：TwoSilly
	*/
    void addInfillToGCode(SliceLayerPart* part, GCodePlanner& gcodeLayer, int layerNr, int extrusionWidth, int fillAngle);

	/**
	时间：2016年8月4日 11:34:45
	内容：添加插入的gocde
		参数：
		SliceLayerPart* part -->
		GCodePlanner& gcodeLayer -->
		int layerNr -->
		int extrusionWidth -->
		int fillAngle -->
	作者：TwoSilly
	*/
    void addInsetToGCode(SliceLayerPart* part, GCodePlanner& gcodeLayer, int layerNr);

	/**
	时间：2016年8月4日 11:34:45
	内容：添加支撑的gocde
		参数：
		SliceDataStorage& storage -->		
		GCodePlanner& gcodeLayer -->
		int layerNr -->
	作者：TwoSilly
	*/
    void addSupportToGCode(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int layerNr);

	/**
	时间：2016年8月4日 11:34:45
	内容：添加擦除塔
		参数：
		SliceDataStorage& storage -->		
		GCodePlanner& gcodeLayer -->
		int layerNr -->
		int prevExtruder -->
	作者：TwoSilly
	*/
    void addWipeTower(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int layerNr, int prevExtruder);
};

}//namespace Engine



#endif // PROCESSOR_H
