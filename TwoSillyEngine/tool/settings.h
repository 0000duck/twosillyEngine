/**
文件名：settings.h

时间：2016年7月9日 10:58:29
内容：设置模块这个模块了存有 
		版本号 -->VERSION (以后记得重写它，用读取版本文件的方式)
		挤出机的最大数量 --> MAX_EXTRUDERS "16"
		修复的类型 --> FIX_HORRIBLE_UNION_ALL_TYPE_A 等
		支撑的类型 --> Support_Pattern
		默认配置文件的路径 --> DEFAULT_CONFIG_PATH
		Gocde风格 --> GCode_Flavor
		填充图案类型 --> Infill_Pattern
		梳理特征 --> Combing_Feature 

作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/

#ifndef SETTINGS_H
#define SETTINGS_H
#include <string.h>
using namespace std;

//FILE* getFile(string str); 

#ifndef VERSION
#define VERSION "TWO_1.1"
#endif

#define FIX_HORRIBLE_UNION_ALL_TYPE_A    0x01   //修复可怕的联盟所有A型  0x01 0001
#define FIX_HORRIBLE_UNION_ALL_TYPE_B    0x02   //修复可怕的联盟所有B型  0x02 0010
#define FIX_HORRIBLE_EXTENSIVE_STITCHING 0x04   //修复可怕的广泛的缝合   0x04 0100
#define FIX_HORRIBLE_UNION_ALL_TYPE_C    0x08   //修复可怕的联盟所有C型  0x08 1000
#define FIX_HORRIBLE_KEEP_NONE_CLOSED    0x10   //修复可怕的保持没有关闭 0x10 1010

#define MAX_EXTRUDERS 16

/**
 * 支撑材料类型
 * 网格是一个X / Y网格的轮廓，这是非常强大的，提供了良好的支持。但在某些情况下是很难去除。
 * 线给一排线，一次一个，使他们更容易删除，但他们不支持和网格支持一样好。
 */
enum Support_Pattern
{
    SUPPORT_TYPE_GRID = 0, //格子
    SUPPORT_TYPE_LINES = 1 //线路
};

#ifndef DEFAULT_CONFIG_PATH
#define DEFAULT_CONFIG_PATH "default.cfg"
#endif

#define CONFIG_MULTILINE_SEPARATOR "\"\"\"" //配置多路分离器 MULTILINE（multiline） 多线路 SEPARATOR（separator） 分离器

enum GCode_Flavor
{
/**
 * RepRap风格gcode是Marlin/Sprinter/ repetier 型Gocde
 *  这是最常用的Gocde设置。
 *  G0对于移动 G1对于挤出
 *  E值给毫米的长丝挤压
 *  回缩在E值与G1做。开始/结束代码添加
 *  M106 Sxxx和M107是用来把风扇打开/关闭
 **/
    
    GCODE_FLAVOR_REPRAP = 0,
/**
 *  ultigcode风格是Marlin的Gocde
 *  ultigcode使用较少的设置在切片机和把更多的设置在固件。这使得更多的硬件/材料独立Gocde
 *  G0对于移动 G1对于挤出
 *  E值给MM 3的长丝挤出。忽略灯丝直径设置
 *  收缩完成G10和G11.不被忽略的回缩设置.G10 S1用于多机切换回缩。
 *  不添加开始/结束代码.
 *  M106 Sxxx和M107是用来把风扇打开/关闭。
 **/
    GCODE_FLAVOR_ULTIGCODE = 1,
    
/**
 * Makerbot风格的Gocde
 * 看起来很像RepRap显示有一些变化，要求制造商软件转换为x3g文件
 * 加热需要做M104 Sxxx T0
 * 没有G21 或 G90
 * 风扇是m126 T0（无风扇的强度控制？）
 * 风扇是M127 T0
 * 自动引导是 G162 x F2000
 **/
    GCODE_FLAVOR_MAKERBOT = 2,

/**
 * BFB Gocde
 * BFB使用转速而不是E，这是耦合的F而不是独立， (M108 S[十分之一 RPM])
 * 每一行都需要X，Y，Z，F。
 * 需要开启/关闭挤出机（101、103（M227），具有 自动-翻新收缩 (M227 S[2560*mm] P[2560*mm])
 **/
    GCODE_FLAVOR_BFB = 3,

/**
 * MACH3 GCode
 * Mach3 CNC控制软件是，预计A、B、C、D为挤出机，而不是E
 **/
    GCODE_FLAVOR_MACH3 = 4,
/**
 * RepRap容积味Gcode，Marlin的GCode。
 * 容积使用较少的设置提出了更高的切片机和设置在固件.他对更多的硬件/材料进行独立Gocde
 * G0对于移动 G1对于挤出
 * E值给MM 3的长丝挤出。忽略灯丝直径设置。
 * 收缩完成G10和G11.不被忽略的回缩设置. G10 S1用于多机切换回缩.
 * M106 Sxxx和M107是用来把风扇打开/关闭。
 **/
    GCODE_FLAVOR_REPRAP_VOLUMATRIC = 5,
};



/**
 * 填充图案类型。
 */
enum Infill_Pattern
{
    INFILL_AUTOMATIC = 0,
    INFILL_GRID = 1,
    INFILL_LINES = 2,
    INFILL_CONCENTRIC = 3,
	INFILL_BEEGIVE = 4

    /*
    INFILL_AUTOMATIC = 0,   //自动填充
    
    INFILL_GRID = 1,        //网格
    
    INFILL_LINES = 2,       //线条
    
    INFILL_CONCENTRIC = 3,  //同轴
    INFILL_BEEGIVE = 4      //蜂窝
    */
};

/**
 * 在哪里使用梳理功能
 */
enum Combing_Feature //梳理特征
{
    COMBING_OFF = 0,
    COMBING_ALL = 1,
    COMBING_NOSKIN = 2,
    /*
    COMBING_OFF = 0,   //梳理取消
    COMBING_ALL = 1,   //梳理全部
    COMBING_NOSKIN = 2,//梳理没有皮的
    */
};


#endif//SETTINGS_H
