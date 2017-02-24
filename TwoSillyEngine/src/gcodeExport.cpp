/**

文件名：gocdeExport.cpp

时间：2016年8月2日 14:47:59
内容：Gocde输出模块
注：算法啊！坑爹的算法没看
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#include <stdarg.h>
#include <stdio.h>
#include <math.h>

#include "gcodeExport.h"
#include "pathOrderOptimizer.h"
#include "timeEstimate.h"
#include "../tool/settings.h"
#include "../tool/logoutput.h"

namespace Engine {

	/**
	时间：2016年8月2日 15:13:58
	内容：gocde输出的构造函数
	作者：TwoSilly
	*/
GCodeExport::GCodeExport()//Export 输出
: _currentPosition(0,0,0), _startPosition(INT32_MIN,INT32_MIN,0)
{
    _extrusionAmount = 0;//挤出量
    _extrusionPerMM = 0;//挤出每毫米（mm）
    _retractionAmount = 4.5;//回退量
    _minimalExtrusionBeforeRetraction = 0.0;//最小的挤出回退量
    _extrusionAmountAtPreviousRetraction = -10000;//前回缩的挤出量
    _extruderSwitchRetraction = 14.5;//挤出机切换回缩
    _extruderNr = 0;//挤出机正常范围 Nr --> Normal Range 
    _currentFanSpeed = -1;//目前的风扇转速
    
    _totalPrintTime = 0.0;//总计打印时间
    for(unsigned int e=0; e<MAX_EXTRUDERS; e++)
        _totalFilament[e] = 0.0;
    
    _currentSpeed = 0;//现在的速度
    _retractionSpeed = 45;//回退速度
    _isRetracted = false;//是否回退
    setFlavor(GCODE_FLAVOR_REPRAP);//设置Gocde格式
    memset(_extruderOffset, 0, sizeof(_extruderOffset));//memset 清零 _extruderOffset 挤出机的偏移 
    _f = stdout;//标志输出文件
}

GCodeExport::~GCodeExport()
{
    if (_f && _f != stdout)
        fclose(_f);
}
/**
时间：2016年8月2日 15:18:20
内容：在开始替换标记
参数：
const char* tag --> 标签
const char* replaceValue --> 替换的量
作者：TwoSilly
*/
void GCodeExport::replaceTagInStart(const char* tag, const char* replaceValue)
{
    if (_f == stdout)
    {
        Engine::log("Replace:%s:%s\n", tag, replaceValue);
        return;
    }
    fpos_t oldPos;
	fgetpos(_f, &oldPos);//	fgetpos，取得当前文件的句柄。
    
    char buffer[1024];
	/**
	int fseek(FILE *stream, long offset, int fromwhere);
	函数设置文件指针stream的位置。如果执行成功，
	stream将指向以fromwhere为基准，偏移offset（指针偏移量）个字节的位置，
	函数返回0。如果执行失败(比如offset超过文件自身大小)，则不改变stream指向的位置
	，函数返回一个非0值。实验得出，超出文件末尾位置，还是返回0。往回偏移超出首位置，返回-1，
	且指向一个-1的位置，请小心使用。fseek函数和lseek函数类似，
	但lseek返回的是一个off_t数值，而fseek返回的是一个整型。
	*/
    fseek(_f, 0, SEEK_SET);
    fread(buffer, 1024, 1, _f);
    
    char* c = strstr(buffer, tag);
    memset(c, ' ', strlen(tag));
	/*
	memcpy的函数原型是void *memcpy(void *dest, const void *src, size_t n)，
	其功能是从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中。
	memcpy则是根据其第3个参数决定复制的长度。函数返回指向destin的指针.作用：
	将s中第13个字符开始的4个连续字符复制到d中。
	strcpy只能复制字符串，而memcpy可以复制任意内容，例如字符数组、整型、结构体、类等。
	strcpy不需要指定长度，它遇到被复制字符的串结束符"\0"才结束，所以容易溢出。
	*/
    if (c) memcpy(c, replaceValue, strlen(replaceValue));
    
    fseek(_f, 0, SEEK_SET);
    fwrite(buffer, 1024, 1, _f);
    
	//fsetpos 	fsetpos，计算机编程语言函数，其功能是定位流上的文件指针。
    fsetpos(_f, &oldPos);
}

    /**
	时间：2016年8月2日 15:39:29
	内容：设置挤出器偏移
		参数：
			int id --> 
			Point p --> 
	作者：TwoSilly
	*/
void GCodeExport::setExtruderOffset(int id, Point p)
{
    _extruderOffset[id] = p;
}

	/**
	时间：2016年8月2日 15:39:29
	内容：设置开关挤出机代码
		参数：
			std::string preSwitchExtruderCode --> 预开关挤出机代码
			std::string postSwitchExtruderCode --> 后开关挤出机代码
	作者：TwoSilly
	*/
void GCodeExport::setSwitchExtruderCode(std::string preSwitchExtruderCode, std::string postSwitchExtruderCode)
{
    this->_preSwitchExtruderCode = preSwitchExtruderCode;
    this->_postSwitchExtruderCode = postSwitchExtruderCode;
}

void GCodeExport::setFlavor(int flavor)
{
    this->_flavor = flavor;
    if (flavor == GCODE_FLAVOR_MACH3)
        for(int n=0; n<MAX_EXTRUDERS; n++)
            _extruderCharacter[n] = 'A' + n;
    else
        for(int n=0; n<MAX_EXTRUDERS; n++)
            _extruderCharacter[n] = 'E';
}

void GCodeExport::applyAccelerationSettings(Config& config)
{
    _estimateCalculator.applyAccelerationSettings(config);
}


	
	/**
	时间：2016年8月2日 15:39:29
	内容：得到当前（Gocde风格）		
			
	作者：TwoSilly
	*/
int GCodeExport::getFlavor()
{
    return this->_flavor;
}

	/**
	时间：2016年8月2日 15:39:29
	内容：设置文件名		
		参数：
			const char* filename --> 文件名
	作者：TwoSilly
	*/
void GCodeExport::setFilename(const char* filename)
{
    fopen_s(&_f,filename, "w+");
}

    
	/**
	时间：2016年8月2日 15:39:29
	内容：是否是打开的		
		参数：

	作者：TwoSilly
	*/
bool GCodeExport::isOpened()
{
    return _f != nullptr;
}


	/**
	时间：2016年8月2日 15:39:29
	内容：设置挤出		
		参数：
		int layerThickness --> 层厚
		int filamentDiameter --> 长丝的直径（材料的直径）
		int flow -->流（流量）
	作者：TwoSilly
	*/
void GCodeExport::setExtrusion(int layerThickness, int filamentDiameter, int flow)
{
    double filamentArea = M_PI * (INT2MM(filamentDiameter) / 2.0) * (INT2MM(filamentDiameter) / 2.0);
	//ultigcode采用体积挤压E值，从而在不需要filamentarea混合。
    if (_flavor == GCODE_FLAVOR_ULTIGCODE || _flavor == GCODE_FLAVOR_REPRAP_VOLUMATRIC)
        _extrusionPerMM = INT2MM(layerThickness);
    else
        _extrusionPerMM = INT2MM(layerThickness) / filamentArea * double(flow) / 100.0;
}


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
void GCodeExport::setRetractionSettings(int retractionAmount, int retractionSpeed, int extruderSwitchRetraction, int minimalExtrusionBeforeRetraction, int zHop, int retractionAmountPrime)
{
    this->_retractionAmount = INT2MM(retractionAmount);
    this->_retractionAmountPrime = INT2MM(retractionAmountPrime);
    this->_retractionSpeed = retractionSpeed;
    this->_extruderSwitchRetraction = INT2MM(extruderSwitchRetraction);
    this->_minimalExtrusionBeforeRetraction = INT2MM(minimalExtrusionBeforeRetraction);//最小挤压前回缩
    this->_retractionZHop = zHop;
}

void GCodeExport::setZ(int z)
{
    this->_zPos = z;
}

	/**
	时间：2016年8月2日 16:11:17
	内容：得到当前位置点		
		
	作者：TwoSilly
	*/
Point GCodeExport::getPositionXY()
{
    return Point(_currentPosition.x, _currentPosition.y);
}

	/**
	时间：2016年8月2日 16:11:17
	内容：重置开始位置		
		注：用 INT32_MIN 重置
	作者：TwoSilly
	*/
void GCodeExport::resetStartPosition()
{
    _startPosition.x = INT32_MIN;
    _startPosition.y = INT32_MIN;
}

Point GCodeExport::getStartPositionXY()
{
    return Point(_startPosition.x, _startPosition.y);
}

int GCodeExport::getPositionZ()
{
    return _currentPosition.z;
}

int GCodeExport::getExtruderNr()
{
    return _extruderNr;
}

double GCodeExport::getTotalFilamentUsed(int e)
{
    if (e == _extruderNr)
        return _totalFilament[e] + _extrusionAmount;
    return _totalFilament[e];
}

double GCodeExport::getTotalPrintTime()
{
    return _totalPrintTime;
}

void GCodeExport::updateTotalPrintTime()
{
    _totalPrintTime += _estimateCalculator.calculate();
    _estimateCalculator.reset();
}

	/**
	时间：2016年8月2日 15:39:29
	内容：写入注释		
		参数：
		const char* comment --> 
		... --> 变参函数
	作者：TwoSilly
	*/
void GCodeExport::writeComment(const char* comment, ...)
{
    va_list args;
    va_start(args, comment);
    fprintf(_f, ";");
    vfprintf(_f, comment, args);
    if (_flavor == GCODE_FLAVOR_BFB)
        fprintf(_f, "\r\n");
    else
        fprintf(_f, "\n");
    va_end(args);
}

	/**
	时间：2016年8月2日 15:39:29
	内容：写入行		
		参数：
		const char* line --> 
		... --> 变参函数
	作者：TwoSilly
	*/
void GCodeExport::writeLine(const char* line, ...)
{
    va_list args;
    va_start(args, line);
    vfprintf(_f, line, args);
    if (_flavor == GCODE_FLAVOR_BFB)
        fprintf(_f, "\r\n");
    else
        fprintf(_f, "\n");
    va_end(args);
}


	/**
	时间：2016年8月2日 15:39:29
	内容：重置挤出机价值		
		参数：
		
	作者：TwoSilly
	*/
void GCodeExport::resetExtrusionValue()
{
    if (_extrusionAmount != 0.0 && _flavor != GCODE_FLAVOR_MAKERBOT && _flavor != GCODE_FLAVOR_BFB)
    {
        fprintf(_f, "G92 %c0\n", _extruderCharacter[_extruderNr]);
        _totalFilament[_extruderNr] += _extrusionAmount;
        _extrusionAmountAtPreviousRetraction -= _extrusionAmount;
        _extrusionAmount = 0.0;
    }
}

	/**
	时间：2016年8月2日 15:39:29
	内容：写入延迟		
		参数：
		double timeAmount --> 时间量
		
	作者：TwoSilly
	*/
void GCodeExport::writeDelay(double timeAmount)
{
    fprintf(_f, "G4 P%d\n", int(timeAmount * 1000));
    _totalPrintTime += timeAmount;
}

	/**
	时间：2016年8月2日 15:39:29
	内容：写入移动		
		参数：
		Point p --> 
		int speed --> 
		int lineWidth --> 
	作者：TwoSilly
	*/
void GCodeExport::writeMove(Point p, int speed, int lineWidth)
{
    if (_currentPosition.x == p.X && _currentPosition.y == p.Y && _currentPosition.z == _zPos)
        return;

    if (_flavor == GCODE_FLAVOR_BFB)
    {
        //从字节的机器上的位，我们需要处理这个完全不同的。由于他们不使用E值，但转速值。
        float fspeed = speed * 60;
        float rpm = (_extrusionPerMM * double(lineWidth) / 1000.0) * speed * 60;
        const float mm_per_rpm = 4.0; //所有的病机有4mm每转挤压。
        rpm /= mm_per_rpm;
        if (rpm > 0)
        {
            if (_isRetracted)
            {
                if (_currentSpeed != int(rpm * 10))
                {
                    //fprintf(_f, "; %f e-per-mm %d mm-width %d mm/s\n", _extrusionPerMM, lineWidth, speed);
                    fprintf(_f, "M108 S%0.1f\r\n", rpm);
                    _currentSpeed = int(rpm * 10);
                }
                fprintf(_f, "M%d01\r\n", _extruderNr + 1);
                _isRetracted = false;
            }
            //通过实际转速我们要求修理的速度，因为我们不能把所有的舍入误差转速值，但我们在进给速度值有更多的分辨率.
            // (Trick copied from KISSlicer, thanks Jonathan)
            fspeed *= (rpm / (roundf(rpm * 100) / 100));

            //增加挤出量计算所用长丝的量。
            Point diff = p - getPositionXY();
            _extrusionAmount += _extrusionPerMM * INT2MM(lineWidth) * vSizeMM(diff);
        }else{
            //如果我们没有挤出，检查，如果我们仍然需要禁用挤出机。这导致了由于自动回缩的回缩。
            if (!_isRetracted)
            {
                fprintf(_f, "M103\r\n");
                _isRetracted = true;
            }
        }
        fprintf(_f, "G1 X%0.3f Y%0.3f Z%0.3f F%0.1f\r\n", INT2MM(p.X - _extruderOffset[_extruderNr].X), INT2MM(p.Y - _extruderOffset[_extruderNr].Y), INT2MM(_zPos), fspeed);
    }else{
        
        //正常e处理。
        if (lineWidth != 0)
        {
            Point diff = p - getPositionXY();
            if (_isRetracted)
            {
                if (_retractionZHop > 0)
                    fprintf(_f, "G1 Z%0.3f\n", float(_currentPosition.z)/1000);
                if (_flavor == GCODE_FLAVOR_ULTIGCODE || _flavor == GCODE_FLAVOR_REPRAP_VOLUMATRIC)
                {
                    fprintf(_f, "G11\n");
                }else{
					_extrusionAmount += _retractionAmountPrime;
					_extrusionAmount += _retractionAmountPrime;
                    fprintf(_f, "G1 F%i %c%0.5f\n", _retractionSpeed * 60, _extruderCharacter[_extruderNr], _extrusionAmount);
                    _currentSpeed = _retractionSpeed;
                    _estimateCalculator.plan(TimeEstimateCalculator::Position(INT2MM(_currentPosition.x), INT2MM(_currentPosition.y), INT2MM(_currentPosition.z), _extrusionAmount), _currentSpeed);
                }
                if (_extrusionAmount > 10000.0) //根据 有超过21m挤压会导致不准确的. 所以重置每10m, 只是要确定.
                    resetExtrusionValue();
                _isRetracted = false;
            }
            _extrusionAmount += _extrusionPerMM * INT2MM(lineWidth) * vSizeMM(diff);
            fprintf(_f, "G1");
        }else{
            fprintf(_f, "G0");
        }

        if (_currentSpeed != speed)
        {
            fprintf(_f, " F%i", speed * 60);
            _currentSpeed = speed;
        }

        fprintf(_f, " X%0.3f Y%0.3f", INT2MM(p.X - _extruderOffset[_extruderNr].X), INT2MM(p.Y - _extruderOffset[_extruderNr].Y));
        if (_zPos != _currentPosition.z)
            fprintf(_f, " Z%0.3f", INT2MM(_zPos));
        if (lineWidth != 0)
            fprintf(_f, " %c%0.5f", _extruderCharacter[_extruderNr], _extrusionAmount);
        fprintf(_f, "\n");
    }
    
    _currentPosition = Point3(p.X, p.Y, _zPos);
    _startPosition = _currentPosition;
    _estimateCalculator.plan(TimeEstimateCalculator::Position(INT2MM(_currentPosition.x), INT2MM(_currentPosition.y), INT2MM(_currentPosition.z), _extrusionAmount), speed);
}

	/**
	时间：2016年8月2日 15:39:29
	内容：写入回退		
		参数：
		bool force=false --> 强迫
	作者：TwoSilly
	*/
void GCodeExport::writeRetraction(bool force)
{
    if (_flavor == GCODE_FLAVOR_BFB)//bits from bytes并自动回缩。
        return;
    
    if (_retractionAmount > 0 && !_isRetracted && (_extrusionAmountAtPreviousRetraction + _minimalExtrusionBeforeRetraction < _extrusionAmount || force))
    {
        if (_flavor == GCODE_FLAVOR_ULTIGCODE || _flavor == GCODE_FLAVOR_REPRAP_VOLUMATRIC)
        {
            fprintf(_f, "G10\n");
        }else{
            fprintf(_f, "G1 F%i %c%0.5f\n", _retractionSpeed * 60, _extruderCharacter[_extruderNr], _extrusionAmount - _retractionAmount);
            _currentSpeed = _retractionSpeed;
            _estimateCalculator.plan(TimeEstimateCalculator::Position(INT2MM(_currentPosition.x), INT2MM(_currentPosition.y), INT2MM(_currentPosition.z), _extrusionAmount - _retractionAmount), _currentSpeed);
        }
        if (_retractionZHop > 0)
            fprintf(_f, "G1 Z%0.3f\n", INT2MM(_currentPosition.z + _retractionZHop));
        _extrusionAmountAtPreviousRetraction = _extrusionAmount;
        _isRetracted = true;
    }
}


	/**
	时间：2016年8月2日 15:39:29
	内容：挤出机开关		
		参数：
		int newExtruder --> 新的挤出机
	作者：TwoSilly
	*/
void GCodeExport::switchExtruder(int newExtruder)
{
    if (_extruderNr == newExtruder)
        return;
    if (_flavor == GCODE_FLAVOR_BFB)
    {
        if (!_isRetracted)
            fprintf(_f, "M103\r\n");
        _isRetracted = true;
        return;
    }
    
    resetExtrusionValue();
    if (_flavor == GCODE_FLAVOR_ULTIGCODE || _flavor == GCODE_FLAVOR_REPRAP_VOLUMATRIC)
    {
        fprintf(_f, "G10 S1\n");
    }else{
        fprintf(_f, "G1 F%i %c%0.5f\n", _retractionSpeed * 60, _extruderCharacter[_extruderNr], _extrusionAmount - _extruderSwitchRetraction);
        _currentSpeed = _retractionSpeed;
    }
    if (_retractionZHop > 0)
        fprintf(_f, "G1 Z%0.3f\n", INT2MM(_currentPosition.z + _retractionZHop));
    _extruderNr = newExtruder;
    if (_flavor == GCODE_FLAVOR_MACH3)
        resetExtrusionValue();
    _isRetracted = true;
    writeCode(_preSwitchExtruderCode.c_str());
    if (_flavor == GCODE_FLAVOR_MAKERBOT)
        fprintf(_f, "M135 T%i\n", _extruderNr);
    else
        fprintf(_f, "T%i\n", _extruderNr);
    writeCode(_postSwitchExtruderCode.c_str());
}

	/**
	时间：2016年8月2日 15:39:29
	内容：写入代码		
		参数：
		const char* str --> 
	作者：TwoSilly
	*/
void GCodeExport::writeCode(const char* str)
{
    fprintf(_f, "%s", str);
    if (_flavor == GCODE_FLAVOR_BFB)
        fprintf(_f, "\r\n");
    else
        fprintf(_f, "\n");
}

	/**
	时间：2016年8月2日 15:39:29
	内容：写入风扇命令		
		参数：
		int speed --> 
	作者：TwoSilly
	*/
void GCodeExport::writeFanCommand(int speed)
{
    if (_currentFanSpeed == speed)
        return;
    if (speed > 0)
    {
        if (_flavor == GCODE_FLAVOR_MAKERBOT)
            fprintf(_f, "M126 T0 ; value = %d\n", speed * 255 / 100);
        else if (_flavor == GCODE_FLAVOR_MACH3)
            fprintf(_f, "M106 P%d\n", speed * 255 / 100);
        else
            fprintf(_f, "M106 S%d\n", speed * 255 / 100);
    }
    else
    {
        if (_flavor == GCODE_FLAVOR_MAKERBOT)
            fprintf(_f, "M127 T0\n");
        else if (_flavor == GCODE_FLAVOR_MACH3)
            fprintf(_f, "M106 P%d\n", 0);
        else
            fprintf(_f, "M107\n");
    }
    _currentFanSpeed = speed;
}

	/**
	时间：2016年8月3日 10:27:18
	内容：得到文件大小		
		参数：
	作者：TwoSilly
	*/
int GCodeExport::getFileSize(){
    return ftell(_f);
}

	/**
	时间：2016年8月3日 10:27:18
	内容：显示，告诉文件大小（在控制台）		
		参数：
	作者：TwoSilly
	*/
void GCodeExport::tellFileSize() {
    float fsize = ftell(_f);
    if(fsize > 1024*1024) {
        fsize /= 1024.0*1024.0;
        Engine::log("Wrote %5.1f MB.\n",fsize);
    }
    if(fsize > 1024) {
        fsize /= 1024.0;
        Engine::log("Wrote %5.1f kilobytes.\n",fsize);
    }
}

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
void GCodeExport::finalize(int maxObjectHeight, int moveSpeed, const char* endCode)
{
    writeFanCommand(0);
    writeRetraction();
    setZ(maxObjectHeight + 5000);
    writeMove(getPositionXY(), moveSpeed, 0);
    writeCode(endCode);
    Engine::log("Print time: %d\n", int(getTotalPrintTime()));
    Engine::log("Filament: %d\n", int(getTotalFilamentUsed(0)));
    Engine::log("Filament2: %d\n", int(getTotalFilamentUsed(1)));
    
    if (getFlavor() == GCODE_FLAVOR_ULTIGCODE)
    {
        char numberString[16];
        sprintf(numberString, "%d", int(getTotalPrintTime()));
        replaceTagInStart("<__TIME__>", numberString);
        sprintf(numberString, "%d", int(getTotalFilamentUsed(0)));
        replaceTagInStart("<FILAMENT>", numberString);
        sprintf(numberString, "%d", int(getTotalFilamentUsed(1)));
        replaceTagInStart("<FILAMEN2>", numberString);
    }
}

/**
时间：2016年8月3日 15:31:59
内容：得到最新的路径与配置
	参数：
		GCodePathConfig* config --> Gocde路径配置
作者：TwoSilly
*/
GCodePath* GCodePlanner::getLatestPathWithConfig_(GCodePathConfig* config)
{
    if (_paths.size() > 0 && _paths[_paths.size()-1].config == config && !_paths[_paths.size()-1].done)
        return &_paths[_paths.size()-1];
    _paths.push_back(GCodePath());
    GCodePath* ret = &_paths[_paths.size()-1];
    ret->retract = false;
    ret->config = config;
    ret->extruder = _currentExtruder;
    ret->done = false;
    return ret;
}

/**
时间：2016年8月3日 15:31:59
内容：强迫新的路径开始
	参数：
作者：TwoSilly
*/
void GCodePlanner::forceNewPathStart_()
{
    if (_paths.size() > 0)
        _paths[_paths.size()-1].done = true;
}
/**
时间：2016年8月3日 15:31:59
内容：强迫新的路径开始
	参数：
	GCodeExport& gcode --> gocde输出
	int travelSpeed --> 旅行移动速度
	int retractionMinimalDistance --> 回抽的最小距离
作者：TwoSilly
*/
GCodePlanner::GCodePlanner(GCodeExport& _gcode, int travelSpeed, int retractionMinimalDistance)
: _gcode(_gcode), _travelConfig(travelSpeed, 0, "travel")
{
    _lastPosition = _gcode.getPositionXY();
    _comb = nullptr;
    _extrudeSpeedFactor = 100;
    _travelSpeedFactor = 100;
    _extraTime = 0.0;
    _totalPrintTime = 0.0;
    _forceRetraction = false;
    _alwaysRetract = false;
    _currentExtruder = _gcode.getExtruderNr();
    this->_retractionMinimalDistance = retractionMinimalDistance;
}
GCodePlanner::~GCodePlanner()
{
	if (_comb)
	{
		delete _comb;
		_comb=nullptr;
	}
}

	/**
	时间：2016年8月3日 15:31:59
	内容：添加旅行，移动
		参数：
			Point p --> 
	作者：TwoSilly
	*/
void GCodePlanner::addTravel(Point p)
{
    GCodePath* path = getLatestPathWithConfig_(&_travelConfig);
    if (_forceRetraction)
    {
        if (!shorterThen(_lastPosition - p, _retractionMinimalDistance))
        {
            path->retract = true;
        }
        _forceRetraction = false;
    }else if (_comb != nullptr)
    {
        vector<Point> pointList;
        if (_comb->calc(_lastPosition, p, pointList))
        {
            for(unsigned int n=0; n<pointList.size(); n++)
            {
                path->points.push_back(pointList[n]);
            }
        }else{
            if (!shorterThen(_lastPosition - p, _retractionMinimalDistance))
                path->retract = true;
        }
    }else if (_alwaysRetract)
    {
        if (!shorterThen(_lastPosition - p, _retractionMinimalDistance))
            path->retract = true;
    }
	//if(path->points.size() > 1)
	//path->points.push_back(path->points[path->points.size() -1]);
    path->points.push_back(p);
    _lastPosition = p;
}


	/**
	时间：2016年8月3日 15:31:59
	内容：添加挤出机移动
		参数：
			Point p --> 
			 GCodePathConfig* config --> 
	作者：TwoSilly
	*/
void GCodePlanner::addExtrusionMove(Point p, GCodePathConfig* config)
{
    getLatestPathWithConfig_(config)->points.push_back(p);
    _lastPosition = p;
}

	/**
	时间：2016年8月3日 15:31:59
	内容：移动里面的梳理边
		参数：
			int distance --> 
	作者：TwoSilly
	*/
void GCodePlanner::moveInsideCombBoundary(int distance)
{

    if (!_comb || _comb->inside(_lastPosition)) return;
    Point p = _lastPosition;
    if (_comb->moveInside(&p, distance))
    {
        //往里走一遍，所以我们走出紧张的90度角
        _comb->moveInside(&p, distance);
        if (_comb->inside(p))
        {
            addTravel(p);
            //确保任何回缩发生在这一举动之前，而不是在它之前，通过启动一个新的移动路径。
            forceNewPathStart_();
        }else
		{

		}
    }
}

	/**
	时间：2016年8月3日 15:31:59
	内容：添加多边形
		参数：
			PolygonRef polygon --> 
			int startIdx --> 
			GCodePathConfig* config --> 
	作者：TwoSilly
	*/
void GCodePlanner::addPolygon(PolygonRef polygon, int startIdx, GCodePathConfig* config)
{
    Point p0 = polygon[startIdx];
    addTravel(p0);
    for(unsigned int i=1; i<polygon.size(); i++)
    {
        Point p1 = polygon[(startIdx + i) % polygon.size()];
        addExtrusionMove(p1, config);
        p0 = p1;
    }
    if (polygon.size() > 2)
        addExtrusionMove(polygon[startIdx], config);
}

	/**
	时间：2016年8月3日 15:31:59
	内容：通过优化添加多边形
		参数：
			Polygons polygons --> 
			GCodePathConfig* config --> 
	作者：TwoSilly
	*/
void GCodePlanner::addPolygonsByOptimizer(Polygons& polygons, GCodePathConfig* config)
{
    PathOrderOptimizer orderOptimizer(_lastPosition);
    for(unsigned int i=0;i<polygons.size();i++)
        orderOptimizer.addPolygon(polygons[i]);
    orderOptimizer.optimize();
    for(unsigned int i=0;i<orderOptimizer.polyOrder_.size();i++)
    {
        int nr = orderOptimizer.polyOrder_[i];
        addPolygon(polygons[nr], orderOptimizer.polyStart_[nr], config);
    }
}


	/**
	时间：2016年8月3日 15:31:59
	内容：强制的最小层时间
		参数：
			double minTime --> 
			int minimalSpeed --> 
	作者：TwoSilly
	*/
void GCodePlanner::forceMinimalLayerTime(double minTime, int minimalSpeed)
{
    Point p0 = _gcode.getPositionXY();
    double travelTime = 0.0;
    double extrudeTime = 0.0;
    for(unsigned int n=0; n<_paths.size(); n++)
    {
        GCodePath* path = &_paths[n];
        for(unsigned int i=0; i<path->points.size(); i++)
        {
            double thisTime = vSizeMM(p0 - path->points[i]) / double(path->config->speed);
            if (path->config->lineWidth != 0)
                extrudeTime += thisTime;
            else
                travelTime += thisTime;
            p0 = path->points[i];
        }
    }
    double totalTime = extrudeTime + travelTime;
    if (totalTime < minTime && extrudeTime > 0.0)
    {
        double minExtrudeTime = minTime - travelTime;
        if (minExtrudeTime < 1)
            minExtrudeTime = 1;
        double factor = extrudeTime / minExtrudeTime;
        for(unsigned int n=0; n<_paths.size(); n++)
        {
            GCodePath* path = &_paths[n];
            if (path->config->lineWidth == 0)
                continue;
            int speed = path->config->speed * factor;
            if (speed < minimalSpeed)
                factor = double(minimalSpeed) / double(path->config->speed);
        }
        
        //只有在最短的时间，如果这将是慢下来，那么一个因素已经设置。一层增速放缓还设置速度因素。
        if (factor * 100 < getExtrudeSpeedFactor())
            setExtrudeSpeedFactor(factor * 100);
        else
            factor = getExtrudeSpeedFactor() / 100.0;
        
        if (minTime - (extrudeTime / factor) - travelTime > 0.1)
        {
            //TODO: 使用这个额外的时间（圆圈周围的打印？）
            this->_extraTime = minTime - (extrudeTime / factor) - travelTime;
        }
        this->_totalPrintTime = (extrudeTime / factor) + travelTime;
    }else{
        this->_totalPrintTime = totalTime;
    }
}

	/**
	时间：2016年8月3日 15:31:59
	内容：写入Gocde
		参数：
			bool liftHeadIfNeeded --> 抬起头，如果需要的话
			 int layerThickness --> 层厚度;
	作者：TwoSilly
	*/
void GCodePlanner::writeGCode(bool liftHeadIfNeeded, int layerThickness)
{
    GCodePathConfig* lastConfig = nullptr;
    int extruder = _gcode.getExtruderNr();

    for(unsigned int n=0; n<_paths.size(); n++)
    {
        GCodePath* path = &_paths[n];
        if (extruder != path->extruder)
        {
            extruder = path->extruder;
            _gcode.switchExtruder(extruder);
        }else if (path->retract)
        {
            _gcode.writeRetraction();
        }
        if (path->config != &_travelConfig && lastConfig != path->config)
        {
            _gcode.writeComment("TYPE:%s", path->config->name);
            lastConfig = path->config;
        }
        int speed = path->config->speed;
        
        if (path->config->lineWidth != 0)// 只适用于挤出速度因素的挤压动作
            speed = speed * _extrudeSpeedFactor / 100;
        else
            speed = speed * _travelSpeedFactor / 100;
        
        if (path->points.size() == 1 && path->config != &_travelConfig && shorterThen(_gcode.getPositionXY() - path->points[0], path->config->lineWidth * 2))
        {
            //检查大量的小动作，并将它们组合成一个大的线
            Point p0 = path->points[0];
            unsigned int i = n + 1;
            while(i < _paths.size() && _paths[i].points.size() == 1 && shorterThen(p0 - _paths[i].points[0], path->config->lineWidth * 2))
            {
                p0 = _paths[i].points[0];
                i ++;
            }
            if (_paths[i-1].config == &_travelConfig)
                i --;
            if (i > n + 2)
            {
                p0 = _gcode.getPositionXY();
                for(unsigned int x=n; x<i-1; x+=2)
                {
                    int64_t oldLen = vSize(p0 - _paths[x].points[0]);
                    Point newPoint = (_paths[x].points[0] + _paths[x+1].points[0]) / 2;
                    int64_t newLen = vSize(_gcode.getPositionXY() - newPoint);
                    if (newLen > 0)
                        _gcode.writeMove(newPoint, speed, path->config->lineWidth * oldLen / newLen);
                    
                    p0 = _paths[x+1].points[0];
                }
                _gcode.writeMove(_paths[i-1].points[0], speed, path->config->lineWidth);
                n = i - 1;
                continue;
            }
        }
        
        bool spiralize = path->config->spiralize;
        if ( spiralize)
        {
            //查看我们是否在列表中，最后spiralize路径如果不是，不要spiralize。
            for(unsigned int m=n+1; m<_paths.size(); m++)
            {
                if (_paths[m].config->spiralize)
                    spiralize = false;
            }
        }
        if (spiralize)
        {
            //如果我们需要spiralize然后抬起头慢慢由1层作为这条道路的进展.
            float totalLength = 0.0;
            int z = _gcode.getPositionZ();
            Point p0 = _gcode.getPositionXY();
            for(unsigned int i=0; i<path->points.size(); i++)
            {
                Point p1 = path->points[i];
                totalLength += vSizeMM(p0 - p1);
                p0 = p1;
            }
            
            float length = 0.0;
            p0 = _gcode.getPositionXY();
            for(unsigned int i=0; i<path->points.size(); i++)
            {
                Point p1 = path->points[i];
                length += vSizeMM(p0 - p1);
                p0 = p1;
                _gcode.setZ(z + layerThickness * length / totalLength);
                _gcode.writeMove(path->points[i], speed, path->config->lineWidth);
            }
        }else{
            for(unsigned int i=0; i<path->points.size(); i++)
            {
                _gcode.writeMove(path->points[i], speed, path->config->lineWidth);
            }
        }
    }
    
    _gcode.updateTotalPrintTime();
    if (liftHeadIfNeeded && _extraTime > 0.0)
    {
        _gcode.writeComment("Small layer, adding delay of %f", _extraTime);
        _gcode.writeRetraction(true);
        _gcode.setZ(_gcode.getPositionZ() + MM2INT(3.0));
        _gcode.writeMove(_gcode.getPositionXY(), _travelConfig.speed, 0);
        _gcode.writeMove(_gcode.getPositionXY() - Point(-MM2INT(20.0), 0), _travelConfig.speed, 0);
        _gcode.writeDelay(_extraTime);
    }
}

}//namespace Engine
