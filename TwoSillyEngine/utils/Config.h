/**

文件名：Config.h

时间：2016年7月4日 09:29:54
内容：配置文件类，他有一般配置的全部值，和读取配置文件的方法
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <vector>

#include "../utils/floatpoint.h"

#include "../tool/settings.h"

//配置文件设置的编号
class _ConfigSettingIndex
{
public:
    const char* key; //键值
    int* ptr;	//数值

    _ConfigSettingIndex(const char* key, int* ptr) : key(key), ptr(ptr) {}
};

/**
时间：2016年8月3日 20:52:44
内容：配置文件类，里面装有所有的配置文件所需要的参数
	数据成员：
		 static Config *config; // 允许访问来自世界各地的配置设置
                                  
    //基础
    int nozzleSize;  //喷嘴直径
    int layerThickness;//层厚
    int initialLayerThickness;//初始层厚
    int filamentDiameter;//细丝直径(我估计是材料的直径)
    int filamentFlow;//丝流（材料流量）
    int layer0extrusionWidth;//层0挤出宽度
    int extrusionWidth;//挤出宽度
    int insetCount;//插入数（？？）
    int downSkinCount;//下皮肤计数（？？）
    int upSkinCount;//上皮肤计数
    
    int skirtDistance;//边缘距离
    int skirtLineCount;//边缘行计数
    int skirtMinLength;//边缘的最小长度
    
    //Retraction settings
    //回退设置
    int retractionAmount;//回退量
    int retractionAmountPrime;//初期的回退
    int retractionAmountExtruderSwitch;//回退量挤出机开关
    int retractionSpeed;//回退速度
    int retractionMinimalDistance;//回退的最小距离
    int minimalExtrusionBeforeRetraction;//最小挤压前回退（？？）
    int retractionZHop;//回退的Z轴跳转

    //梳理 
    int enableCombing; //使梳理
    int enableOozeShield;//使溢出保护（加壳？？）
    int wipeTowerSize;//擦除高架大小
    int multiVolumeOverlap;//复选体积重叠

    //Speedup 速度
    int initialSpeedupLayers;//初始加速层
    int initialLayerSpeed;//初始层速度
    int printSpeed;//打印速度
    int inset0Speed;//插入0的速度
    int insetXSpeed;//插入X的速度
    int moveSpeed;//移动速度
    int fanFullOnLayerNr;//风扇完全的正常范围

    //Infill settings
    //填充设置
    int sparseInfillLineDistance;//稀少的填充线的距离
    int infillOverlap;//填充重叠
    int infillSpeed;//填充速度
    int infillPattern;//填充样式
   
    INFILL_AUTOMATIC = 0,   //自动填充    
    INFILL_GRID = 1,        //网格    
    INFILL_LINES = 2,       //线条    
    INFILL_CONCENTRIC = 3,  //同轴      
   /
    int skinSpeed;//外皮速度
    int perimeterBeforeInfill;//边界填充

    //Support material
    //支撑材料
    int supportType;//支撑的类型
    int supportAngle;//支撑的角度
    int supportEverywhere;//全部支撑(无处不在的)
    int supportLineDistance;//支撑线的距离！
    int supportXYDistance;//支撑的XY轴距离
    int supportZDistance;//支撑的Z轴距离
    int supportExtruder;//挤出机

    //Cool settings
    //冷却设置
    int minimalLayerTime;//最低层的时间
    int minimalFeedrate;//最小速度
    int coolHeadLift;//冷却打印头上升
    int fanSpeedMin;//风扇最小速度
    int fanSpeedMax;//风扇最大速度

    //Raft settings
    //木筏设置
    int raftMargin;//木筏边缘
    int raftLineSpacing;//木筏线条间隔
    int raftBaseThickness;//筏板基础厚度
    int raftBaseLinewidth;//筏基线宽度
    int raftBaseSpeed;//筏板基础速度
    int raftInterfaceThickness;//筏板间厚度
    int raftInterfaceLinewidth;//筏板间宽度
    int raftInterfaceLineSpacing;//筏板间距
    int raftFanSpeed;//筏风扇转速
    int raftSurfaceThickness;//筏面厚度
    int raftSurfaceLinewidth;//筏面线宽度
    int raftSurfaceLineSpacing;//筏面线间距
    int raftSurfaceLayers;//筏板的表面层
    int raftSurfaceSpeed;//筏板的表面速度
    int raftAirGap;//筏板空气间隙
    int raftAirGapLayer0;//筏板空气间隙层0

    FloatMatrix3x3 matrix;//矩阵
    IntPoint objectPosition;//对象位置
    int objectSink;//对象库

	时间：2016年7月13日 09:01:34
	内容：自动中心点
		值：
		 1 --> 偏移为模型最大点与最小点的和的一半。z轴为最小点
		 2 --> 不偏移
		 不为1且不等于2 的数 --> x，y轴不偏移 z轴为模型最小点的z值
	使用地点：优化模型时计算偏移量
	作者：TwoSilly
	///
    int autoCenter;//自动中心

    int fixHorrible;//修复可怕的
    int spiralizeMode;//螺旋化模型
    int simpleMode;//简单的模型
    int gcodeFlavor;//Gocde模型

    IntPoint extruderOffset[MAX_EXTRUDERS];//挤出机的偏移 MAX 16个
    std::string startCode;//开始Gocde
    std::string endCode;//结束Gocde
    std::string preSwitchExtruderCode;//预开关挤出机代码
    std::string postSwitchExtruderCode;//后开关挤出机代码
作者：TwoSilly
*/

class Config
{
private:
    std::vector<_ConfigSettingIndex> _index;
public:
    static Config *config; // 允许访问来自世界各地的配置设置
                                  
    //基础
    int nozzleSize;  //喷嘴直径
    int layerThickness;//层厚
    int initialLayerThickness;//初始层厚
    int filamentDiameter;//细丝直径(我估计是材料的直径)
    int filamentFlow;//丝流（材料流量）
    int layer0extrusionWidth;//层0挤出宽度
    int extrusionWidth;//挤出宽度
    int insetCount;//插入数（？？）
    int downSkinCount;//下皮肤计数（？？）
    int upSkinCount;//上皮肤计数
    
    int skirtDistance;//边缘距离
    int skirtLineCount;//边缘行计数
    int skirtMinLength;//边缘的最小长度
    
    //Retraction settings
    //回退设置
    int retractionAmount;//回退量
    int retractionAmountPrime;//初期的回退
    int retractionAmountExtruderSwitch;//回退量挤出机开关
    int retractionSpeed;//回退速度
    int retractionMinimalDistance;//回退的最小距离
    int minimalExtrusionBeforeRetraction;//最小挤压前回退（？？）
    int retractionZHop;//回退的Z轴跳转

    //梳理 
    int enableCombing; //使梳理
    int enableOozeShield;//使溢出保护（加壳？？）
    int wipeTowerSize;//擦除高架大小
    int multiVolumeOverlap;//复选体积重叠

    //Speedup 速度
    int initialSpeedupLayers;//初始加速层
    int initialLayerSpeed;//初始层速度
    int printSpeed;//打印速度
    int inset0Speed;//插入0的速度
    int insetXSpeed;//插入X的速度
    int moveSpeed;//移动速度
    int fanFullOnLayerNr;//风扇完全的正常范围

    //Infill settings
    //填充设置
    int sparseInfillLineDistance;//稀少的填充线的距离
    int infillOverlap;//填充重叠
    int infillSpeed;//填充速度
    int infillPattern;//填充样式
    /*
    INFILL_AUTOMATIC = 0,   //自动填充    
    INFILL_GRID = 1,        //网格    
    INFILL_LINES = 2,       //线条    
    INFILL_CONCENTRIC = 3,  //同轴      
    */
    int skinSpeed;//外皮速度
    int perimeterBeforeInfill;//边界填充

    //Support material
    //支撑材料
    int supportType;//支撑的类型
    int supportAngle;//支撑的角度
    int supportEverywhere;//全部支撑(无处不在的)
    int supportLineDistance;//支撑线的距离！
    int supportXYDistance;//支撑的XY轴距离
    int supportZDistance;//支撑的Z轴距离
    int supportExtruder;//支撑挤出机

    //Cool settings
    //冷却设置
    int minimalLayerTime;//最低层的时间
    int minimalFeedrate;//最小速度
    int coolHeadLift;//冷却打印头上升
    int fanSpeedMin;//风扇最小速度
    int fanSpeedMax;//风扇最大速度

    //Raft settings
    //木筏设置
    int raftMargin;//木筏边缘
    int raftLineSpacing;//木筏线条间隔
    int raftBaseThickness;//筏板基础厚度
    int raftBaseLinewidth;//筏基线宽度
    int raftBaseSpeed;//筏板基础速度
    int raftInterfaceThickness;//筏板间厚度
    int raftInterfaceLinewidth;//筏板间宽度
    int raftInterfaceLineSpacing;//筏板间距
    int raftFanSpeed;//筏风扇转速
    int raftSurfaceThickness;//筏面厚度
    int raftSurfaceLinewidth;//筏面线宽度
    int raftSurfaceLineSpacing;//筏面线间距
    int raftSurfaceLayers;//筏板的表面层
    int raftSurfaceSpeed;//筏板的表面速度
    int raftAirGap;//筏板空气间隙
    int raftAirGapLayer0;//筏板空气间隙层0

    FloatMatrix3x3 matrix;//矩阵
    IntPoint objectPosition;//对象位置
    int objectSink;//对象库
	/**
	时间：2016年7月13日 09:01:34
	内容：自动中心点
		值：
		 1 --> 偏移为模型最大点与最小点的和的一半。z轴为最小点
		 2 --> 不偏移
		 不为1且不等于2 的数 --> x，y轴不偏移 z轴为模型最小点的z值
	使用地点：优化模型时计算偏移量
	作者：TwoSilly
	*/
    int autoCenter;//自动中心

    int fixHorrible;//修复可怕的
    int spiralizeMode;//螺旋化模型
    int simpleMode;//简单的模型
    int gcodeFlavor;//Gocde模型

    IntPoint extruderOffset[MAX_EXTRUDERS];//挤出机的偏移 MAX 16个
    std::string startCode;//开始Gocde
    std::string endCode;//结束Gocde
    std::string preSwitchExtruderCode;//预开关挤出机代码
    std::string postSwitchExtruderCode;//后开关挤出机代码

	//设置时间估计
    int acceleration; //加速度
    int max_acceleration[4]; //最大加速度
    int max_xy_jerk; //最大xy回抽
    int max_z_jerk; //最大z回抽
    int max_e_jerk; //最大e回抽

    Config();

	/**
	时间：2016年7月9日 14:41:54
	内容：根据键值来设置配置文件
		参数：
		const char* key --> 键值 （配置文件数据成员的名字）
		const char* value --> 数值 （配置文件数据成员的数值）
	作者：TwoSilly
	*/
    bool setSetting(const char* key, const char* value);

    //读取配置文件的配置.cfg格式的（内置配置文件）
    bool readSettings(void);
    
	/**
	时间：2016年7月9日 14:41:54
	内容：读取并设置配置文件
		参数：
		const char* path --> 配置文件的名字
	作者：TwoSilly
	*/
    bool readSettings(const char* path);
    bool readSettings_ini(const char* path);
};
#endif //CONFIG_H
