/**

文件名：gocdeExport.h

时间：2016年8月2日 14:47:59
内容：Gocde输出模块
注：算法啊！坑爹的算法没看
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef GCODEEXPORT_H
#define GCODEEXPORT_H

#include <stdio.h>

#include "../tool/settings.h"
#include "comb.h"
#include "../utils/intpoint.h"
#include "../utils/polygon.h"
#include "timeEstimate.h"

//函数模块
#include "../tool/function.h"
//#include "../tool/function.h"
namespace Engine {


//gcodeexport类写的实际显示。这是班里唯一知道如何显示的外观和感觉。

//在任何自定义gcodes风格这类做。

/**
时间：2016年8月2日 14:49:55
内容：Gocde输出
	数据成员：
		FILE* f;//文件指针
		double extrusionAmount;//挤出量
		double extrusionPerMM;//挤出设计
		double retractionAmount;//回退量
		double retractionAmountPrime;//收缩量基础
		int retractionZHop;//回退的z轴跳动
		double extruderSwitchRetraction;//挤出量回退的开关
		double minimalExtrusionBeforeRetraction;//最小挤压前回缩
		double extrusionAmountAtPreviousRetraction;//前回缩的挤出量
		Point3 currentPosition;//现在位置
		Point3 startPosition;//开始位置
		Point extruderOffset[MAX_EXTRUDERS];//挤出机偏移
		char extruderCharacter[MAX_EXTRUDERS];//挤出机特征
		int currentSpeed, retractionSpeed;//现在的速度  回退速度
		int zPos;//z轴位置
		bool isRetracted;//是否回退
		int extruderNr;//挤出机正常范围  Nr-->Normal Range 
		int currentFanSpeed;//目前的风扇转速
		int flavor;//风格
		std::string preSwitchExtruderCode;//预开关挤出机代码
		std::string postSwitchExtruderCode;//后开关挤出机代码

		double totalFilament[MAX_EXTRUDERS];//全部的丝
		double totalPrintTime;//全部的打印时间
		TimeEstimateCalculator estimateCalculator;//估计时间计算器  估计计算器
作者：TwoSilly
*/
class GCodeExport
{
private:
    FILE* _f;//文件指针
    double _extrusionAmount;//挤出量
    double _extrusionPerMM;//挤出每毫米（mm）
    double _retractionAmount;//回退量
    double _retractionAmountPrime;//收缩量基础
    int _retractionZHop;//回退的z轴跳动
    double _extruderSwitchRetraction;//挤出量回退的开关
    double _minimalExtrusionBeforeRetraction;//最小挤压前回缩
    double _extrusionAmountAtPreviousRetraction;//前回缩的挤出量
    Point3 _currentPosition;//现在位置
    Point3 _startPosition;//开始位置
    Point _extruderOffset[MAX_EXTRUDERS];//挤出机偏移
    char _extruderCharacter[MAX_EXTRUDERS];//挤出机特征
    int _currentSpeed; //当前速度
	int _retractionSpeed;//  回退速度
    int _zPos;//z轴位置
    bool _isRetracted;//是否回退
    int _extruderNr;//挤出机正常范围  Nr-->Normal Range 
    int _currentFanSpeed;//目前的风扇转速
    int _flavor;//风格
    std::string _preSwitchExtruderCode;//预开关挤出机代码
    std::string _postSwitchExtruderCode;//后开关挤出机代码
    
    double _totalFilament[MAX_EXTRUDERS];//全部的丝
    double _totalPrintTime;//全部的打印时间
    TimeEstimateCalculator _estimateCalculator;//估计时间计算器  估计计算器
public:

    /**
	时间：2016年8月2日 15:13:58
	内容：gocde输出的构造函数
	作者：TwoSilly
	*/
    GCodeExport();
    
    ~GCodeExport();


    /**
	时间：2016年8月2日 15:18:20
	内容：在开始替换标记
		参数：
			const char* tag --> 标签
			const char* replaceValue --> 替换的量
	作者：TwoSilly
	*/
    void replaceTagInStart(const char* tag, const char* replaceValue);
    
    /**
	时间：2016年8月2日 15:39:29
	内容：设置挤出器偏移
		参数：
			int id --> 
			Point p --> 
	作者：TwoSilly
	*/
    void setExtruderOffset(int id, Point p);

	/**
	时间：2016年8月2日 15:39:29
	内容：设置开关挤出机代码
		参数：
			std::string preSwitchExtruderCode --> 预开关挤出机代码
			std::string postSwitchExtruderCode --> 后开关挤出机代码
	作者：TwoSilly
	*/
    void setSwitchExtruderCode(std::string preSwitchExtruderCode, std::string postSwitchExtruderCode);
    
	/**
	时间：2016年8月2日 15:39:29
	内容：设置风格（Gocde风格）
		参数：
			int flavor --> 风格
			
	作者：TwoSilly
	*/
    void setFlavor(int flavor);
	
	/**
	时间：2016年8月2日 15:39:29
	内容：得到当前（Gocde风格）		
			
	作者：TwoSilly
	*/
    int getFlavor();
    
	/**
	时间：2016年9月23日16:00:14
	内容：添加回抽设置（调用的是timeEstimate类里面的）
			
	作者：TwoSilly
	*/
	void applyAccelerationSettings(Config& config);
	/**
	时间：2016年8月2日 15:39:29
	内容：设置文件名		
		参数：
			const char* filename --> 文件名
	作者：TwoSilly
	*/
    void setFilename(const char* filename);
    
	/**
	时间：2016年8月2日 15:39:29
	内容：是否是打开的		
		参数：

	作者：TwoSilly
	*/
    bool isOpened();
    
	    
	/**
	时间：2016年8月2日 15:39:29
	内容：设置挤出机		
		参数：
		int layerThickness --> 层厚
		int filamentDiameter --> 长丝的直径（材料的直径）
		int flow -->流（流量）
	作者：TwoSilly
	*/
    void setExtrusion(int layerThickness, int filamentDiameter, int flow);
    
	/**
	时间：2016年8月2日 15:39:29
	内容：设置回缩设置		
		参数：
		int retractionAmount --> 回缩量
		int retractionSpeed --> 回缩速度
		int extruderSwitchRetraction --> 挤出量回退的开关
		int minimalExtrusionBeforeRetraction --> 最小挤压前回缩
		int zHop --> z轴的跳跃
		int retractionAmountPrime --> 收缩量基础
	作者：TwoSilly
	*/
    void setRetractionSettings(int retractionAmount, int retractionSpeed, int extruderSwitchRetraction, int minimalExtrusionBeforeRetraction, int zHop, int retractionAmountPrime);
    
    void setZ(int z);
    
	/**
	时间：2016年8月2日 16:11:17
	内容：得到当前位置点		
		
	作者：TwoSilly
	*/
    Point getPositionXY();
    
	/**
	时间：2016年8月2日 16:11:17
	内容：重置开始位置		
		注：用 INT32_MIN 重置
	作者：TwoSilly
	*/
    void resetStartPosition();

	/**
	时间：2016年8月2日 16:11:17
	内容：得到开始位置点		
		
	作者：TwoSilly
	*/
    Point getStartPositionXY();

    int getPositionZ();

    int getExtruderNr();
   
	/**
	时间：2016年8月2日 15:39:29
	内容：获得总使用的灯丝		
		参数：
		int e --> 
		
	作者：TwoSilly
	*/
    double getTotalFilamentUsed(int e);

	/**
	时间：2016年8月3日 10:21:13
	内容：获得全部的打印时间		
		参数：
		
	作者：TwoSilly
	*/
    double getTotalPrintTime();

	/**
	时间：2016年8月3日 10:21:20
	内容：更新全部的打印时间		
		参数：
		
	作者：TwoSilly
	*/
    void updateTotalPrintTime();
    
	/**
	时间：2016年8月2日 15:39:29
	内容：写入注释		
		参数：
		const char* comment --> 注释
		... --> 变参函数
	作者：TwoSilly
	*/
    void writeComment(const char* comment, ...);

	/**
	时间：2016年8月2日 15:39:29
	内容：写入行		
		参数：
		const char* line --> 
		... --> 变参函数
	作者：TwoSilly
	*/
    void writeLine(const char* line, ...);
    
	/**
	时间：2016年8月2日 15:39:29
	内容：重置挤出机价值		
		参数：
		
	作者：TwoSilly
	*/
    void resetExtrusionValue();
    
	/**
	时间：2016年8月2日 15:39:29
	内容：写入延迟		
		参数：
		double timeAmount --> 时间量
		
	作者：TwoSilly
	*/
    void writeDelay(double timeAmount);
    

	/**
	时间：2016年8月2日 15:39:29
	内容：写入移动		
		参数：
		Point p --> 
		int speed --> 
		int lineWidth --> 
	作者：TwoSilly
	*/
    void writeMove(Point p, int speed, int lineWidth);
    
	/**
	时间：2016年8月2日 15:39:29
	内容：写入回退		
		参数：
		bool force=false --> 强迫，默认为false
	作者：TwoSilly
	*/
    void writeRetraction(bool force=false);
    
	/**
	时间：2016年8月2日 15:39:29
	内容：挤出机开关		
		参数：
		int newExtruder --> 新的挤出机
	作者：TwoSilly
	*/
    void switchExtruder(int newExtruder);
    
    
	/**
	时间：2016年8月2日 15:39:29
	内容：写入代码		
		参数：
		const char* str --> 
	作者：TwoSilly
	*/
    void writeCode(const char* str);
    

	/**
	时间：2016年8月2日 15:39:29
	内容：写入风扇命令		
		参数：
		int speed --> 
	作者：TwoSilly
	*/
    void writeFanCommand(int speed);
    
	/**
	时间：2016年8月3日 10:01:14
	内容：完成	，在gocde生成结束时产生的代码	
		参数：
		int maxObjectHeight --> 最大对象的高
		int moveSpeed --> 移动的速度
		const char* endCode --> 结束代码
	注：这是模仿的函数！模仿
		finalize() 方法是在垃圾收集器删除对象之前对这个对象调用的。
		为解决这个问题，Java提供了一个名为finalize()的方法，
		它的工作原理应该是这样的：一旦垃圾收集器准备好释放对象占用的
		存储空间，它首先调用finalize()，而且只有在下一次垃圾收集过程中，
		才会真正回收对象的内存。
		垃圾回收过程中执行终结器的准确时间是不确定的。
		在这种情况下，运行时的第一个进程终止通知是 DLL_PROCESS_DETACH 通知。
		柄或数据库连接这类在回收使用托管对象时必须释放的非托管资源时，
		该类型必须实现 Finalize。 Finalize 可以采取任何操作，
		包括在 垃圾回收过程中清理了对象后使对象复活（即，使对象再次可访问）

	作者：TwoSilly
	*/
    void finalize(int maxObjectHeight, int moveSpeed, const char* endCode);
	
	/**
	时间：2016年8月3日 10:27:18
	内容：得到文件大小		
		参数：
	作者：TwoSilly
	*/
    int getFileSize();

	/**
	时间：2016年8月3日 10:27:18
	内容：显示，告诉文件大小（在控制台）		
		参数：
	作者：TwoSilly
	*/
    void tellFileSize();
};

//The GCodePathConfig is  configuration for moves/extrusion actions. This defines at which width the line is printed and at which speed.
//

	/**
	时间：2016年8月3日 10:27:18
	内容：Gocde路径配置		
		数据成员：
		int speed;//速度
		int lineWidth;//行宽
		const char* name;//名字
		bool spiralize;//螺旋切片机
	注：他是 Gocde路径配置 移动/挤压的作用。这定义了在该宽度的线被打印，并在其中速度。
	作者：TwoSilly
	*/
class GCodePathConfig
{
public:
    int speed;//速度
    int lineWidth;//行宽
    const char* name;//名字
	/*
	spiralize 是一个在Z方向帮助打印光滑的功能，
	他在整个打印过程中会稳固增加z，
	这个功能可以使得打印物体像有结实的单墙面一样。
	*/
    bool spiralize;//旋转升高z
    
    GCodePathConfig() : speed(0), lineWidth(0), name(nullptr), spiralize(false) {}

    GCodePathConfig(int speed, int lineWidth, const char* name) : speed(speed), lineWidth(lineWidth), name(name), spiralize(false) {}
    
	/*
	时间：2016年12月16日11:56:35
	内容：设置数值，其实就是给GCodePathConfig类的数据成员 
			int speed;//速度	
			int lineWidth;//行宽 
			const char* name;//名字 
			 赋值！
	参数：
		int speed;//速度	int lineWidth;//行宽 const char* name;//名字
	作者：TwoSilly
	*/
    void setData(int speed, int lineWidth, const char* name)
    {
        this->speed = speed;
        this->lineWidth = lineWidth;
        this->name = name;
    }
};

	/**
	时间：2016年8月3日 10:27:18
	内容：Gocde路径类，这是一个类似结构体没有函数		
		数据成员：
		int speed;//速度
		int lineWidth;//行宽
		const char* name;//名字
		bool spiralize;//螺旋切片机
	注：他是 Gocde路径配置 移动/挤压的作用。这定义了在该宽度的线被打印，并在其中速度。
	作者：TwoSilly
	*/
class GCodePath
{
public:
    GCodePathConfig* config;//Gocde路径配置
    bool retract;//缩进
    int extruder;//挤压机
    vector<Point> points;//点链表
    bool done;//   合乎规矩的 
                //路径完成后，没有更多的动作，应该说，和一个新的路径应该是开始而不是添加了这一。
};

/*
gcode计划类存储多个行动计划。
它有利于梳理，保持头部内的打印。
它还跟踪该规划的打印时间估计，所以速度调整，可以为最小的层时间。
*/

/**
时间：2016年8月3日 15:31:59
内容：Gocde计划者，规划者
	数据成员：
		GCodeExport& _gcode; //gocde输出

		Point _lastPosition;//上一次，最近一次; 最后;的位置
		vector<GCodePath> _paths;//Gocde路径链表
		Comb* _comb;//梳里    
		GCodePathConfig _travelConfig;//Gocde路径配置 
		int _extrudeSpeedFactor;//挤压机速度因素，管家
		int _travelSpeedFactor;//移动，旅行速度管家
		int _currentExtruder;//现在的挤出机
		int _retractionMinimalDistance;//收缩的最小距离
		bool _forceRetraction;//强迫回缩
		bool _alwaysRetract;//总是，常常，回缩！
		double _extraTime;//额外的时间
		double _totalPrintTime;//全部的打印时间
作者：TwoSilly
*/
class GCodePlanner
{
private:
    GCodeExport& _gcode; //gocde输出
    
    Point _lastPosition;//上一次，最近一次; 最后;的位置

    vector<GCodePath> _paths;//Gocde路径链表
    Comb* _comb;//梳里    
    GCodePathConfig _travelConfig;//Gocde路径配置 
    int _extrudeSpeedFactor;//挤压机速度因素，管家
    int _travelSpeedFactor;//移动，旅行速度管家
    int _currentExtruder;//现在的挤出机
    int _retractionMinimalDistance;//收缩的最小距离
    bool _forceRetraction;//强迫回缩
    bool _alwaysRetract;//总是，常常，回缩！
    double _extraTime;//额外的时间
    double _totalPrintTime;//全部的打印时间
private:

	/**
	时间：2016年8月3日 15:31:59
	内容：得到最新的路径与配置
		参数：
			GCodePathConfig* config --> Gocde路径配置
	作者：TwoSilly
	*/
    GCodePath* getLatestPathWithConfig_(GCodePathConfig* config);

	/**
	时间：2016年8月3日 15:31:59
	内容：强迫新的路径开始
		参数：
	作者：TwoSilly
	*/
    void forceNewPathStart_();

	//void isExtrudeAmount();
public:

	/**
	时间：2016年8月3日 15:31:59
	内容：强迫新的路径开始
		参数：
		GCodeExport& gcode --> gocde输出
		int travelSpeed --> 旅行移动速度
		int retractionMinimalDistance --> 回抽的最小距离
	作者：TwoSilly
	*/
    GCodePlanner(GCodeExport& gcode, int travelSpeed, int retractionMinimalDistance);


	/**
	时间：2016年8月3日 15:31:59
	内容：析构，这只释放了comb梳理
	作者：TwoSilly
	*/
	~GCodePlanner();
    
	/**
	时间：2016年8月3日 15:31:59
	内容：设置挤出机
		参数：
		int extruder --> 挤出机
	作者：TwoSilly
	*/
    bool setExtruder(int extruder)
    {
        if (extruder == _currentExtruder)
            return false;
        _currentExtruder = extruder;
        return true;
    }
   
	/**
	时间：2016年8月3日 15:31:59
	内容：得到当前挤出机
		参数：
	作者：TwoSilly
	*/
    int getExtruder()
    {
        return _currentExtruder;
    }

   
	/**
	时间：2016年8月3日 15:31:59
	内容：设置梳子的边界线，范围
		参数：
			Polygons* polygons --> 
	作者：TwoSilly
	*/
    void setCombBoundary(Polygons* polygons)
    {
        if (_comb)
		{
            delete _comb;
			_comb=nullptr;
		}
        if (polygons)
            _comb = new Comb(*polygons);
        else
            _comb = nullptr;
    }
   
	/**
	时间：2016年8月3日 15:31:59
	内容：设置永远回抽
		参数：
			Polygons* polygons --> 
	作者：TwoSilly
	*/
    void setAlwaysRetract(bool alwaysRetract)
    {
        this->_alwaysRetract = alwaysRetract;
    }
    
	/**
	时间：2016年8月3日 15:31:59
	内容：强制回抽
		参数：
	作者：TwoSilly
	*/
	void forceRetract()
    {
       _forceRetraction = true;
    }
    

	/**
	时间：2016年8月3日 15:31:59
	内容：设置挤出机的速度代理
		参数：
			int speedFactor -->　速度代理
	作者：TwoSilly
	*/
    void setExtrudeSpeedFactor(int speedFactor)
    {
        if (speedFactor < 1) speedFactor = 1;
        this->_extrudeSpeedFactor = speedFactor;
    }

	/**
	时间：2016年8月3日 15:31:59
	内容：得到挤出机的速度代理
		参数：
	作者：TwoSilly
	*/
    int getExtrudeSpeedFactor()
    {
        return this->_extrudeSpeedFactor;
    }

	/**
	时间：2016年8月3日 15:31:59
	内容：设置移动，旅行的速度代理
		参数：
			int speedFactor -->　速度代理
	作者：TwoSilly
	*/
    void setTravelSpeedFactor(int speedFactor)
    {
        if (speedFactor < 1) speedFactor = 1;
        this->_travelSpeedFactor = speedFactor;
    }

	/**
	时间：2016年8月3日 15:31:59
	内容：得到移动，旅行的速度代理
		参数：
	作者：TwoSilly
	*/
    int getTravelSpeedFactor()
    {
        return this->_travelSpeedFactor;
    }
    
	/**
	时间：2016年8月3日 15:31:59
	内容：添加旅行，移动
		参数：
			Point p --> 
	作者：TwoSilly
	*/
    void addTravel(Point p);
    
	/**
	时间：2016年8月3日 15:31:59
	内容：添加挤出机移动
		参数：
			Point p --> 
			 GCodePathConfig* config --> 
	作者：TwoSilly
	*/
    void addExtrusionMove(Point p, GCodePathConfig* config);
   
	/**
	时间：2016年8月3日 15:31:59
	内容：移动里面的梳理边
		参数：
			int distance --> 
	作者：TwoSilly
	*/
    void moveInsideCombBoundary(int distance);

	/**
	时间：2016年8月3日 15:31:59
	内容：添加多边形
		参数：
			PolygonRef polygon --> 
			int startIdx --> 
			GCodePathConfig* config --> 
	作者：TwoSilly
	*/
    void addPolygon(PolygonRef polygon, int startIdx, GCodePathConfig* config);


	/**
	时间：2016年8月3日 15:31:59
	内容：通过优化添加多边形
		参数：
			Polygons polygons --> 
			GCodePathConfig* config --> 
	作者：TwoSilly
	*/
    void addPolygonsByOptimizer(Polygons& polygons, GCodePathConfig* config);
    
	/**
	时间：2016年8月3日 15:31:59
	内容：强制的最小层时间
		参数：
			double minTime --> 
			int minimalSpeed --> 
	作者：TwoSilly
	*/
    void forceMinimalLayerTime(double minTime, int minimalSpeed);
    
	/**
	时间：2016年8月3日 15:31:59
	内容：写入Gocde
		参数：
			bool liftHeadIfNeeded --> 抬起头，如果需要的话
			 int layerThickness --> 层厚度;
	作者：TwoSilly
	*/
    void writeGCode(bool liftHeadIfNeeded, int layerThickness);
};

}//namespace Engine

#endif//GCODEEXPORT_H
