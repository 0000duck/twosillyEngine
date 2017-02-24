/*******************************************************
*时间：2016年6月27日 14:58:00
*内容：这是我重写CuraEngine的开始！main函数的存在文件
		我打算吧他们所有的分为各个模块以便解析和维护
		1.首先我会把他的<clipper>(裁剪模块)分出来！这不是他的slicer模块！只是他用来

*作者：TwoSilly
*E-MAIL：twosilly@foxmail.com
*******************************************************/
#ifndef ENGINE_CPP
#define ENGINE_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include <time.h>
#include <signal.h>
#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#include <execinfo.h>
#include <sys/resource.h>
#endif
#include <stddef.h>
#include <vector>
#include "./Processor.h"

//#include<fstream>

void print_usage()
{
	Engine::logError("usage: Engine [-h] [-v] [-m 3x3matrix] [-c <config file>] [-s <settingkey>=<value>] -o <output.gcode> <model.stl>\n");
}

//Signal handler for a "floating point exception", which can also be integer division by zero errors.
//“浮点异常”的信号处理程序，它也可以由零错误进行整数划分
void signal_FPE(int n)
{
	(void)n;
	Engine::logError("Arithmetic exception.\n");
	exit(1);
}

using namespace Engine;

int main(int argc, char **argv)
{
	//    std::ofstream SaveFile("cpp-home.txt");
	//    SaveFile<< argv;//"Hello World, from www.cpp-home.com and Loobian!";
	//    SaveFile.close();

#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
	//Lower the process priority on linux and mac. On windows this is done on process creation from the GUI.
	//降低在基于和苹果的进程优先级。在窗口上，这是从图形用户界面上进行的过程创建的。
	setpriority(PRIO_PROCESS, 0, 10);
#endif

	//Register the exception handling for arithmic exceptions, this prevents the "something went wrong" dialog on windows to pop up on a division by zero.
	//除登记为算术例外处理，这可以防止“出了Windows错误”对话框弹出一个除数为零。
	signal(SIGFPE, signal_FPE);

	Config config;
	Processor processor(config);
	std::vector<std::string> files;

	Engine::logError("Engine version %s\n", VERSION);
	Engine::logError("版权所有：HueWay有限公司\n");
	Engine::logError("\n");

	Engine::logError("作者：TwoSilly.\n");
	Engine::logError("E-MAIL：twosilly@foxmail.com	.\n");

	if(!config.readSettings()) {
		Engine::logError("Default config '%s' not used\n", DEFAULT_CONFIG_PATH);
	}
	/*
	时间：2016年9月23日17:36:07
	作用：我打算用一个文件把cura界面传入的数据全接下来！试试
	作者：TwoSilly
	*/
	ofstream ocout;
    ocout.open("test.txt");
    
	for(int argn = 1; argn < argc; argn++)
	{
		Engine::log("Arg: %s\n", argv[argn]);
		//Engine::log("argv %d:%s\n",argn, argv[argn]);
		ocout<<argn<<":"<<argv[argn]<<"\n";
    
	}
	ocout.close();

	for(int argn = 1; argn < argc; argn++)
	{
		char* str = argv[argn];
		if (str[0] == '-')
		{
			for(str++; *str; str++)
			{
				switch(*str)
				{
				case 'h':
					print_usage();
					exit(1);
				case 'v':
					Engine::onLogSwitch();
					break;
				case 'p':
					Engine::onProgressLoggingSwitch();
					break;
				case 'g':
					argn++;
					//Connect the GUI socket to the given port number.
					//将接口套接字连接到给定的端口号
					processor.guiConnect(atoi(argv[argn]));
					break;
				case 'b':
					argn++;
					//The binaryMeshBlob is depricated and will be removed in the future.二元网格块
					//是的二元网格块 depricated和在未来将被删除

					 fopen_s(&binaryMeshBlob,argv[argn], "rb");
					break;
				case 'o':
					argn++;
					if (!processor.setTargetFile(argv[argn]))
					{
						Engine::logError("Failed to open %s for output.\n", argv[argn]);
						exit(1);
					}
					break;
				case 'c':
					{
						// Read a config file from the given path
						//从给定路径读取配置文件
						argn++;
						if(!config.readSettings(argv[argn])) {
							Engine::logError("Failed to read config '%s'\n", argv[argn]);
						}
					}
					break;
				case 's':
					{
						//Parse the given setting and store it.
						//解析给定的设置并存储它
						argn++;
						char* valuePtr = strchr(argv[argn], '=');
						if (valuePtr)
						{
							//我打算加一段查看指针长度的代码！
							//int count = strlen(valuePtr);
							//这里不用太在意这个"\0"，没有他也可以正常赋值的@！
							*valuePtr++ = '\0';

							if (!config.setSetting(argv[argn], valuePtr))
								Engine::logError("Setting not found: %s %s\n", argv[argn], valuePtr);
						}
					}
					break;
				case 'm':
					//Read the given rotation/scale matrix
					//读取给定的旋转/缩放矩阵
					argn++;
					sscanf(argv[argn], "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",   
						&config.matrix.m[0][0], &config.matrix.m[0][1], &config.matrix.m[0][2],
						&config.matrix.m[1][0], &config.matrix.m[1][1], &config.matrix.m[1][2],
						&config.matrix.m[2][0], &config.matrix.m[2][1], &config.matrix.m[2][2]);
					break;
				case '-':
					try {
						//Catch all exceptions, this prevents the "something went wrong" dialog on windows to pop up on a thrown exception.
						// Only ClipperLib currently throws exceptions. And only in case that it makes an internal error.
						//捕获所有异常，这样可以防止在弹出窗口上的“出错”对话框弹出一个抛出异常。
						//只有clipperlib目前抛出异常。只有在它的情况下，它使一个内部错误
						if (files.size() > 0)
							processor.processFile(files);
						files.clear();
					}catch(...){
						Engine::logError("未知异常\n");
						exit(1);
					}
					break;
				default:
					Engine::logError("未知的选项: %c\n", *str);
					break;
				}
			}
		}else{
			if (argv[argn][0] == '$')
			{
				try {
					//Catch all exceptions, this prevents the "something went wrong" dialog on windows to pop up on a thrown exception.
					// Only ClipperLib currently throws exceptions. And only in case that it makes an internal error.
					//捕获所有异常，这样可以防止在弹出窗口上的“出错”对话框弹出一个抛出异常
					//只有clipperlib目前抛出异常。只有在它的情况下，它使一个内部错误。
					std::vector<std::string> tmp;
					tmp.push_back(argv[argn]);
					processor.processFile(tmp);
				}catch(...){
					Engine::logError("Unknown exception\n");
					exit(1);
				}
			}else{
				files.push_back(argv[argn]);
			}
		}
	}
	try {
		//Catch all exceptions, this prevents the "something went wrong" dialog on windows to pop up on a thrown exception.
		// Only ClipperLib currently throws exceptions. And only in case that it makes an internal error.
		//捕获所有异常，这样可以防止在弹出窗口上的“出错”对话框弹出一个抛出异常
		//只有clipperlib目前抛出异常。只有在它的情况下，它使一个内部错误。
		if (files.size() > 0)
			processor.processFile(files);
	}catch(...){
		Engine::logError("Unknown exception\n");
		exit(1);
	}
	//Finalize the processor, this adds the end.gcode. And reports statistics.
	//完成处理器，这增加了end.gcode。和统计报表
	processor.finalize();
	return 0;
}

#endif//ENGINE_CPP
