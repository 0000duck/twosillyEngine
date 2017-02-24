/** 
文件名：logoutput.h

时间：2016年7月8日 17:56:14
内容：日志输出文件，用于对程序的调试跟踪是一个很方便的内容
作者：TwoSilly
E-MAIL：TwoSilly@foxmail.com
*/
#ifndef LOGOUTPUT_H
#define LOGOUTPUT_H


namespace Engine
{

	/**
	时间：2016年7月9日 08:48:04
	内容：打开日志开关，他主要用于控制台测试时的显示
	作者：TwoSilly
	*/
	void onLogSwitch();
	
	/**
	时间：2016年7月9日 08:48:04
	内容：打开进程日志开关，他主要用于打开控制台测试时的显示
	作者：TwoSilly
	*/
	void onProgressLoggingSwitch();

	/**
	时间：2016年7月9日 08:48:04
	内容：输出字符串到stderr，这是一个变参函数，不过暂时只能读取一行！
	作者：TwoSilly
	*/
	 void logError(const char* fmt, ...);


	/**
	时间：2016年7月9日 08:48:04
	内容：输出字符串到stderr，这需要打开日志开关，即：LogSwitch必须大于1，否则将不做操作
			这是一个变参函数，不过暂时只能读取一行！（它与logError实现基本相同就是多了一个判断log开关）
	作者：TwoSilly
	*/
	 void log(const char* fmt, ...);


	/**
	时间：2016年7月9日 08:48:04
	内容：报告引擎的进展接口，她需要打开ProgressLoggingSwitch
		参数：
		const char* type --> 进程的类型，例：输出Export 皮skin
		int values --> 进行到第几步这是一个数量
		int maxValue --> 需要进行的总步数
	作者：TwoSilly
	*/
	 void logProgress(const char* type, int values, int maxValue);






};
#endif//LOGOUTPUT_H
