/*******************************************************
*文件名：timeEstimate.h

*时间：2016年7月13日 10:24:57

*内容：时间估计模块
*作者：TwoSilly
*E-MAIL：twosilly@foxmail.com											
*******************************************************/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "timeEstimate.h"

#define MINIMUM_PLANNER_SPEED 0.05// (mm/sec)最小的计划者的速度

//最大进料率
static double max_feedrate[TimeEstimateCalculator::NUM_AXIS] = {600, 600, 40, 25};

//最小进料率
static double minimumfeedrate = 0.01;

//加速
static double acceleration = 3000;

//最大加速度
static double max_acceleration[TimeEstimateCalculator::NUM_AXIS] = {9000,9000,100,10000};

//xy的最大抽动
static double max_xy_jerk = 20.0;

//z的最大抽动
static double max_z_jerk = 0.4;

//e的最大抽动
static double max_e_jerk = 5.0;

//平方
template<typename T> const T 
	square(const T& a) { return a * a; }

	/**
	时间：2016年9月23日15:54:24
	内容：添加回抽的设置
		参数： Config& config
	作者：TwoSilly
	*/
void TimeEstimateCalculator::applyAccelerationSettings(Config& config)
{
    acceleration = float(config.acceleration) / 1000.0;
    for(unsigned int n=0; n<TimeEstimateCalculator::NUM_AXIS; n++)
        max_acceleration[n] = float(config.max_acceleration[n]) / 1000.0;
    max_xy_jerk = float(config.max_xy_jerk) / 1000.0;
    max_z_jerk = float(config.max_z_jerk) / 1000.0;
    max_e_jerk = float(config.max_e_jerk) / 1000.0;
}

	/**
	时间：2016年7月13日 11:01:27
	内容：设置位置，设置现在的位置（currentPosition）
		参数： Position newPos
	作者：TwoSilly
	*/
void TimeEstimateCalculator::setPosition(Position newPos)
{
    currentPosition = newPos;
}




	/**
	时间：2016年7月13日 11:01:27
	内容：重置清零，清空 std::vector<Block> blocks;//阻隔 链表	，	
	作者：TwoSilly
	*/
void TimeEstimateCalculator::reset()
{
    blocks.clear();
}

// 计算在这一点上的最大允许速度时，你必须能够达到目标速度使用
// 分配距离内的加速度.
static inline double max_allowable_speed(double acceleration, double target_velocity, double distance)
{
  return sqrt(target_velocity*target_velocity-2*acceleration*distance);
}

// 计算距离（时间）以加速从initial_rate到target_rate使用给定的加速度：
static inline double estimate_acceleration_distance(double initial_rate, double target_rate, double acceleration)
{
    if (acceleration == 0)
        return 0.0;
    return (square(target_rate)-square(initial_rate)) / (2.0*acceleration);
}

// 这个功能给你一点，你必须开始制动（以加速的速度），如果 
// 你开始在速度initial_rate加速直到此时，想结束在final_rate后
// 总距离旅行。这可以用来计算加速度和之间的交点
// 梯形没有高原的情况下减速（即从来没有达到最大速度）
static inline double intersection_distance(double initial_rate, double final_rate, double acceleration, double distance) 
{
    if (acceleration == 0.0)
        return 0.0;
    return (2.0*acceleration*distance-square(initial_rate)+square(final_rate)) / (4.0*acceleration);
}

//这个函数给出了它需要从最初的速度加快，以达到最后的距离。
static inline double acceleration_time_from_distance(double initial_feedrate, double distance, double acceleration)
{
    double discriminant = sqrt(square(initial_feedrate) - 2 * acceleration * -distance);
    return (-initial_feedrate + discriminant) / acceleration;
}

// 计算梯形参数，使进入和退出速度是由所提供的因素补偿.
void TimeEstimateCalculator::calculate_trapezoid_for_block(Block *block, double entry_factor, double exit_factor)
{
    double initial_feedrate = block->nominal_feedrate*entry_factor;
    double final_feedrate = block->nominal_feedrate*exit_factor;

    double acceleration = block->acceleration;
    double accelerate_distance = estimate_acceleration_distance(initial_feedrate, block->nominal_feedrate, acceleration);
    double decelerate_distance = estimate_acceleration_distance(block->nominal_feedrate, final_feedrate, -acceleration);

    // 计算标称速率的高原的大小。
    double plateau_distance = block->distance-accelerate_distance - decelerate_distance;

    // 高原的标称率比什么都小吗？这意味着没有巡航, 
	//我们将使用intersection_distance()计算加速度、启动、制动时中止
    // 为了更好地在这一块的端到final_rate。
    if (plateau_distance < 0)
    {
        accelerate_distance = intersection_distance(initial_feedrate, final_feedrate, acceleration, block->distance);
        accelerate_distance = std::max(accelerate_distance, 0.0); // Check limits due to numerical round-off
        accelerate_distance = std::min(accelerate_distance, block->distance);//(We can cast here to unsigned, because the above line ensures that we are above zero)
        plateau_distance = 0;
    }

    block->accelerate_until = accelerate_distance;
    block->decelerate_after = accelerate_distance+plateau_distance;
    block->initial_feedrate = initial_feedrate;
    block->final_feedrate = final_feedrate;
}                    

	/**
	时间：2016年7月13日 11:01:27
	内容：计划，打算，设计;
		参数： 
		Position newPos --> 新的位置
		double feedRate --> 进料速度
	作者：TwoSilly
	*/
void TimeEstimateCalculator::plan(Position newPos, double feedrate)
{
	//定义一个块（阻碍）的对象
	Block block;
	//为他赋零值
	memset(&block, 0, sizeof(block));
	
	//把块的最大行程赋零
	block.maxTravel = 0;

	//遍历每个轴
	for(unsigned int n=0; n<NUM_AXIS; n++)
	{
		//为每个轴的变量赋值 == 新位置 - 现在的位置
		block.delta[n] = newPos[n] - currentPosition[n];

		//用该轴的变量的绝度值给  绝对值变量赋值
		block.absDelta[n] = fabs(block.delta[n]);

		//计算出变量与绝对值那个大
		block.maxTravel = std::max(block.maxTravel, block.absDelta[n]);
	}
	//如果最大行程小于或等于0，即：位置偏移量为零，那么直接return
	if (block.maxTravel <= 0)
		return;

	//进料速度不能小于最小值，小了就用最小值给他赋值
	if (feedrate < minimumfeedrate)
		feedrate = minimumfeedrate;

	//当前点与原点的距离！
	block.distance = sqrtf(static_cast<float>(square(block.absDelta[0]) + square(block.absDelta[1]) + square(block.absDelta[2])));
	
	//如果距离为零 就用位置偏移量的绝对值（e轴）
	if (block.distance == 0.0)
		block.distance = block.absDelta[3];
	//名义上的进料速度用 实参进料速度赋值
	block.nominal_feedrate = feedrate;

	Position current_feedrate;	//现在的进料速度
	Position current_abs_feedrate; //现在的进料速度的绝度值
	double feedrate_factor = 1.0;  //进料速度的管家（最大速率与当前速率的比）

	//遍历所有轴

	for(unsigned int n=0; n<NUM_AXIS; n++)
	{
		//当前轴的当前进料速度 == 当前轴的位置偏移量 * 实参的进料速率 / 当前点与原点的距离
		current_feedrate[n] = block.delta[n] * feedrate / block.distance;
		
		current_abs_feedrate[n] = fabs(current_feedrate[n]);

		//如果当前轴的现在的进料速度的绝度值 大于 当前轴的最大进料速度
		if (current_abs_feedrate[n] > max_feedrate[n])
			//进料速度的管家 == 当前轴的最大进料速度 / 当前轴的当前进料速率的绝对值
			feedrate_factor = std::min(feedrate_factor, max_feedrate[n] / current_abs_feedrate[n]);
	}
	//TODO: XY_FREQUENCY_LIMIT xy频率限制

	//
	if(feedrate_factor < 1.0)
	{
		for(unsigned int n=0; n<NUM_AXIS; n++)
		{
			current_feedrate[n] *= feedrate_factor;
			current_abs_feedrate[n] *= feedrate_factor;
		}
		block.nominal_feedrate *= feedrate_factor;
	}

	block.acceleration = acceleration;
	for(unsigned int n=0; n<NUM_AXIS; n++)
	{
		if (block.acceleration * (block.absDelta[n] / block.distance) > max_acceleration[n])
			block.acceleration = max_acceleration[n];
	}

	double vmax_junction = max_xy_jerk/2; 
	double vmax_junction_factor = 1.0; 
	if(current_abs_feedrate[Z_AXIS] > max_z_jerk/2)
		vmax_junction = std::min(vmax_junction, max_z_jerk/2);
	if(current_abs_feedrate[E_AXIS] > max_e_jerk/2)
		vmax_junction = std::min(vmax_junction, max_e_jerk/2);
	vmax_junction = std::min(vmax_junction, block.nominal_feedrate);
	double safe_speed = vmax_junction;

	if ((blocks.size() > 0) && (previous_nominal_feedrate > 0.0001))
	{
		double xy_jerk = sqrt(square(current_feedrate[X_AXIS]-previous_feedrate[X_AXIS])+square(current_feedrate[Y_AXIS]-previous_feedrate[Y_AXIS]));
		vmax_junction = block.nominal_feedrate;
		if (xy_jerk > max_xy_jerk) {
			vmax_junction_factor = (max_xy_jerk/xy_jerk);
		} 
		if(fabs(current_feedrate[Z_AXIS] - previous_feedrate[Z_AXIS]) > max_z_jerk) {
			vmax_junction_factor = std::min(vmax_junction_factor, (max_z_jerk/fabs(current_feedrate[Z_AXIS] - previous_feedrate[Z_AXIS])));
		} 
		if(fabs(current_feedrate[E_AXIS] - previous_feedrate[E_AXIS]) > max_e_jerk) {
			vmax_junction_factor = std::min(vmax_junction_factor, (max_e_jerk/fabs(current_feedrate[E_AXIS] - previous_feedrate[E_AXIS])));
		} 
		vmax_junction = std::min(previous_nominal_feedrate, vmax_junction * vmax_junction_factor); // Limit speed to max previous speed
	}

	block.max_entry_speed = vmax_junction;

	double v_allowable = max_allowable_speed(-block.acceleration, MINIMUM_PLANNER_SPEED, block.distance);
	block.entry_speed = std::min(vmax_junction, v_allowable);
	block.nominal_length_flag = block.nominal_feedrate <= v_allowable;
	block.recalculate_flag = true; // 总是计算新块的梯形

	previous_feedrate = current_feedrate;
	previous_nominal_feedrate = block.nominal_feedrate;

	currentPosition = newPos;

	calculate_trapezoid_for_block(&block, block.entry_speed/block.nominal_feedrate, safe_speed/block.nominal_feedrate);

	blocks.push_back(block);
}

double TimeEstimateCalculator::calculate()
{
    reverse_pass();
    forward_pass();
    recalculate_trapezoids();
    
    double totalTime = 0;
    for(unsigned int n=0; n<blocks.size(); n++)
    {
        double plateau_distance = blocks[n].decelerate_after - blocks[n].accelerate_until;
        
        totalTime += acceleration_time_from_distance(blocks[n].initial_feedrate, blocks[n].accelerate_until, blocks[n].acceleration);
        totalTime += plateau_distance / blocks[n].nominal_feedrate;
        totalTime += acceleration_time_from_distance(blocks[n].final_feedrate, (blocks[n].distance - blocks[n].decelerate_after), blocks[n].acceleration);
    }
    return totalTime;
}

// 内核被accelerationplanner：：calculate()当扫描计划从去年第一次进入。
void TimeEstimateCalculator::planner_reverse_pass_kernel(Block *previous, Block *current, Block *next)
{
    (void)previous;
    if(!current || !next)
        return;

    //如果输入的速度已达到最大值的录入速度，不需要重新检查。块巡航。
    // 如果不是，块在加速或减速状态。复位输入速度最大和
    // 检查最大允许速度减少，以确保最大可能的计划速度。
    if (current->entry_speed != current->max_entry_speed)
    {
        // 如果标称长度为真，最大连接速度保证达到。只计算
        // 最大允许速度如果块减速和公称长度是假的。
        if ((!current->nominal_length_flag) && (current->max_entry_speed > next->entry_speed))
        {
            current->entry_speed = std::min(current->max_entry_speed, max_allowable_speed(-current->acceleration, next->entry_speed, current->distance));
        } else {
            current->entry_speed = current->max_entry_speed;
        }
        current->recalculate_flag = true;
    }
}

void TimeEstimateCalculator::reverse_pass()
{
    Block* block[3] = {nullptr, nullptr, nullptr};
    for(unsigned int n=blocks.size()-1; int(n)>=0; n--)
    {
        block[2]= block[1];
        block[1]= block[0];
        block[0] = &blocks[n];
        planner_reverse_pass_kernel(block[0], block[1], block[2]);
    }
}

// 内核被加速计划：：calculate()当扫描计划从第一个到最后一个条目.
void TimeEstimateCalculator::planner_forward_pass_kernel(Block *previous, Block *current, Block *next)
{
    (void)next;
    if(!previous)
        return;

    // 如果前面的块是一个加速块，但它是不够长，以完成
    // 在块内的全速度变化，我们需要相应地调整入口速度。进入
    // 速度已经被重置，最大化，并通过逆向规划的逆向计划。
    //如果名义长度是真的，最大的结速度保证达到。无需复核.
    if (!previous->nominal_length_flag)
    {
        if (previous->entry_speed < current->entry_speed)
        {
            double entry_speed = std::min(current->entry_speed, max_allowable_speed(-previous->acceleration,previous->entry_speed,previous->distance) );

            // 检查结速变化
            if (current->entry_speed != entry_speed)
            {
                current->entry_speed = entry_speed;
                current->recalculate_flag = true;
            }
        }
    }
}

void TimeEstimateCalculator::forward_pass()
{
    Block* block[3] = {nullptr, nullptr, nullptr};
    for(unsigned int n=0; n<blocks.size(); n++)
    {
        block[0]= block[1];
        block[1]= block[2];
        block[2] = &blocks[n];
        planner_forward_pass_kernel(block[0], block[1], block[2]);
    }
    planner_forward_pass_kernel(block[1], block[2], nullptr);
}

//重新计算梯形速度曲线为计划中的所有块的根据
// 每个路口entry_factor。必须在planner_recalculate()后
//更新块。
void TimeEstimateCalculator::recalculate_trapezoids()
{
    Block *current;
    Block *next = nullptr;

    for(unsigned int n=0; n<blocks.size(); n--)
    {
        current = next;
        next = &blocks[n];
        if (current)
        {
            // 如果当前块重新进入或退出结速了。
            if (current->recalculate_flag || next->recalculate_flag)
            {
                // 注：所有以前的逻辑操作的输入和退出因素总是> 0。
                calculate_trapezoid_for_block(current, current->entry_speed/current->nominal_feedrate, next->entry_speed/current->nominal_feedrate);
                current->recalculate_flag = false; // 复位电流只保证下一个梯形的计算
            }
        }
    }
    // 最后/最新的缓冲区。出口速度设置minimum_planner_speed。总是重新计算。
    if(next != nullptr)
    {
        calculate_trapezoid_for_block(next, next->entry_speed/next->nominal_feedrate, MINIMUM_PLANNER_SPEED/next->nominal_feedrate);
        next->recalculate_flag = false;
    }
}
