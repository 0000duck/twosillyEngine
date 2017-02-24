
/*******************************************************
*文件名：timeEstimate.h

*时间：2016年7月13日 10:24:57

*内容：时间估计模块********我还没看懂这个文件不过实在没头绪先搁置一下*******
*作者：TwoSilly
*E-MAIL：twosilly@foxmail.com											
*******************************************************/

#ifndef TIME_ESTIMATE_H
#define TIME_ESTIMATE_H

#include <stdint.h>
#include <vector>
#include "../utils/Config.h"

/**
    T
    时间估计计算器类 生成一个计算在头脑中加速计算的打印时间的估计。
	这部分代码已经 添加 五 从Marlin的来源
*/

/**
时间：2016年7月13日 10:27:44
内容:时间估计计算器
----数据成员：
------------public：
					const static unsigned int NUM_AXIS = 4; //轴计数
					const static unsigned int X_AXIS = 0;	//x轴
					const static unsigned int Y_AXIS = 1;	//y轴
					const static unsigned int Z_AXIS = 2;	//z轴
					const static unsigned int E_AXIS = 3;	//E轴
------------private:
					Position previous_feedrate;//以前的速度
					double previous_nominal_feedrate;//以前的标称速度

					Position currentPosition;//现在的位置

					std::vector<Block> blocks;//阻隔
作者：TwoSilly
*/
class TimeEstimateCalculator
{
public:
    const static unsigned int NUM_AXIS = 4; //轴计数
    const static unsigned int X_AXIS = 0;	//x轴
    const static unsigned int Y_AXIS = 1;	//y轴
    const static unsigned int Z_AXIS = 2;	//z轴
    const static unsigned int E_AXIS = 3;	//E轴


	/**
	时间：2016年7月13日 10:27:44
	内容:位置类，TimeEstimateCalculator时间估计计算器的内置类，
		数据成员： 
				double axis[NUM_AXIS];
	作者：TwoSilly
	*/
    class Position
    {
    public:
		//位置类的构造函数 他将数据成员 axis赋零
        Position() {for(unsigned int n=0;n<NUM_AXIS;n++) axis[n] = 0;}
		//Position的有参构造函数
        Position(double x, double y, double z, double e) {axis[0] = x;axis[1] = y;axis[2] = z;axis[3] = e;}
        double axis[NUM_AXIS];
        
        double& operator[](const int n) { return axis[n]; }
    };

    class Block//块
    {
    public:
        bool recalculate_flag;//重新计算的 标志
        
        double accelerate_until;//加速到
        double decelerate_after;//后减速
        double initial_feedrate;//初始进料速率
        double final_feedrate;//最终的进料速率

        double entry_speed;//入口速度;
        double max_entry_speed;//最大输入速度
        bool nominal_length_flag;//名义上的长度标志
        
        double nominal_feedrate;//名义上的进料速率
        double maxTravel;//最大行程
        double distance;//距离
        double acceleration;//加速
        Position delta;//变量增量（位置偏移量）
        Position absDelta;//绝对值的变量增量（位置偏移量的绝对值）
    };

private:
    Position previous_feedrate;//以前的进料速率
    double previous_nominal_feedrate;//以前的标称速度

    Position currentPosition;//现在的位置

    std::vector<Block> blocks;//阻隔
public:
	/**
	时间：2016年9月23日15:54:24
	内容：添加回抽的设置
		参数： Config& config
	作者：TwoSilly
	*/
	    void applyAccelerationSettings(Config& config);

	/**
	时间：2016年7月13日 11:01:27
	内容：设置位置，设置现在的位置（currentPosition）
		参数： Position newPos
	作者：TwoSilly
	*/
    void setPosition(Position newPos);

	/**
	时间：2016年7月13日 11:01:27
	内容：计划，打算，设计;
		参数： 
		Position newPos --> 新的位置
		double feedRate --> 进料速度
	作者：TwoSilly
	*/
    void plan(Position newPos, double feedRate);


	/**
	时间：2016年7月13日 11:01:27
	内容：重置清零，清空 std::vector<Block> blocks;//阻隔 链表	，	
	作者：TwoSilly
	*/
    void reset();
    
	/**
	时间：2016年7月13日 11:01:27
	内容：计算估计	
	作者：TwoSilly
	*/
    double calculate();
private:
	//反转通过
    void reverse_pass();
	//发送通过
    void forward_pass();

	//重新计算梯形
    void recalculate_trapezoids();
    
	//块体梯形计算
    void calculate_trapezoid_for_block(Block *block, double entry_factor, double exit_factor);
    //规划反向通核
	void planner_reverse_pass_kernel(Block *previous, Block *current, Block *next);
   //规划正向通核
	void planner_forward_pass_kernel(Block *previous, Block *current, Block *next);
};

#endif//TIME_ESTIMATE_H
