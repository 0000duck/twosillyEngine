/**

文件名：Config.cpp

时间：2016年7月4日 09:29:54
内容：配置文件类，他有一般配置的全部值，和读取配置文件的方法
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/

#include <cctype>
#include <fstream>
#include <stdio.h>

#include "../tool/LogOutput.h"
#include "../tool/string.h"
#include "Config.h"

//我要使用getline（）函数string版本的所以需要添加头文件<string>
#include <string>

//前导空白 (LTrim)、尾随空白 (RTrim) 

//删除前置空白（制表符）
#define LTRIM_STRING(s) do { while(((s).length() > 0) && isspace((s)[0])) { (s).erase(0, 1); } } while(0)

//删除尾随制表符（ \0 \n \r \f 等）
#define RTRIM_STRING(s) do { while(((s).length() > 0) && isspace((s)[(s).length() - 1])) { (s).erase((s).length() - 1); } } while(0)

//同时删除前导空白与尾随空白
#define TRIM_STRING(s) do { LTRIM_STRING(s); RTRIM_STRING(s); } while(0)

//字符串指定
#define STRINGIFY(_s) #_s
#define SETTING(name, default) do { _index.push_back(_ConfigSettingIndex(STRINGIFY(name), &name)); name = (default); } while(0)
#define SETTING2(name, altname, default) do { _index.push_back(_ConfigSettingIndex(STRINGIFY(name), &name)); _index.push_back(_ConfigSettingIndex(STRINGIFY(altname), &name)); name = (default); } while(0)

Config *Config::config = NULL;



Config::Config()
{
    config = this;
    SETTING(nozzleSize, 400);
    SETTING(layerThickness, 100);
    SETTING(initialLayerThickness, 300);
    SETTING(filamentDiameter, 2890);
    SETTING(filamentFlow, 100);
    SETTING(layer0extrusionWidth, 600);
    SETTING(extrusionWidth, 400);
    SETTING(insetCount, 2);
    SETTING(downSkinCount, 6);
    SETTING(upSkinCount, 6);
    SETTING(skirtDistance, 6000);
    SETTING(skirtLineCount, 1);
    SETTING(skirtMinLength, 0);

    SETTING(initialSpeedupLayers, 4);
    SETTING(initialLayerSpeed, 20);
    SETTING(printSpeed, 50);
    SETTING(inset0Speed, 50);
    SETTING(insetXSpeed, 50);
    SETTING(moveSpeed, 150);
    SETTING(fanFullOnLayerNr, 2);

    SETTING(sparseInfillLineDistance, 100 * extrusionWidth / 20);

    SETTING(infillOverlap, 15);
    SETTING(infillSpeed, 50);
	SETTING(infillPattern, INFILL_BEEGIVE);
    SETTING(skinSpeed, 50);
    SETTING(perimeterBeforeInfill, 0);

    SETTING(supportType, SUPPORT_TYPE_GRID);
    SETTING(supportAngle, 60);
    SETTING(supportEverywhere, 0);
    SETTING(supportLineDistance, sparseInfillLineDistance);
    SETTING(supportXYDistance, 700);
    SETTING(supportZDistance, 150);
    SETTING(supportExtruder, -1);

    SETTING(retractionAmount, 4500);
    SETTING(retractionAmountPrime, 0);
    SETTING(retractionSpeed, 45);
    SETTING(retractionAmountExtruderSwitch, 14500);
    SETTING(retractionMinimalDistance, 1500);
    SETTING(minimalExtrusionBeforeRetraction, 100);
    SETTING(retractionZHop, 0);

    SETTING(enableCombing, COMBING_ALL);
    SETTING(enableOozeShield, 0);
    SETTING(wipeTowerSize, 0);
    SETTING(multiVolumeOverlap, 0);
    SETTING2(objectPosition.X_, posx, 102500);
    SETTING2(objectPosition.Y_, posy, 102500);
    SETTING(objectSink, 0);
    SETTING(autoCenter, 1);

    SETTING(raftMargin, 5000);
    SETTING(raftLineSpacing, 1000);
    SETTING(raftBaseThickness, 0);
    SETTING(raftBaseLinewidth, 0);
    SETTING(raftInterfaceThickness, 0);
    SETTING(raftInterfaceLinewidth, 0);
    SETTING(raftInterfaceLineSpacing, 0);
    SETTING(raftAirGap, 0);
    SETTING(raftAirGapLayer0, 0);
    SETTING(raftBaseSpeed, 0);
    SETTING(raftFanSpeed, 0);
    SETTING(raftSurfaceThickness, 0);
    SETTING(raftSurfaceLinewidth, 0);
    SETTING(raftSurfaceLineSpacing, 0);
    SETTING(raftSurfaceLayers, 0);
    SETTING(raftSurfaceSpeed, 0);

    SETTING(minimalLayerTime, 5);
    SETTING(minimalFeedrate, 10);
    SETTING(coolHeadLift, 0);
    SETTING(fanSpeedMin, 100);
    SETTING(fanSpeedMax, 100);

	SETTING(fixHorrible, 0x10);
    SETTING(spiralizeMode, 0);
    SETTING(simpleMode, 0);
    SETTING(gcodeFlavor, GCODE_FLAVOR_REPRAP);

    memset(extruderOffset, 0, sizeof(extruderOffset));
    SETTING(extruderOffset[0].X_, 0); //没有人说，挤出机0不能有一个偏移量
    SETTING(extruderOffset[0].Y_, 0);
    SETTING(extruderOffset[1].X_, 0);
    SETTING(extruderOffset[1].Y_, 0);
    SETTING(extruderOffset[2].X_, 0);
    SETTING(extruderOffset[2].Y_, 0);
    SETTING(extruderOffset[3].X_, 0);
    SETTING(extruderOffset[3].Y_, 0);
    SETTING(extruderOffset[4].X_, 0);
    SETTING(extruderOffset[4].Y_, 0);
    SETTING(extruderOffset[5].X_, 0);
    SETTING(extruderOffset[5].Y_, 0);
    SETTING(extruderOffset[6].X_, 0);
    SETTING(extruderOffset[6].Y_, 0);
    SETTING(extruderOffset[7].X_, 0);
    SETTING(extruderOffset[7].Y_, 0);
    SETTING(extruderOffset[8].X_, 0);
    SETTING(extruderOffset[8].Y_, 0);
    SETTING(extruderOffset[9].X_, 0);
    SETTING(extruderOffset[9].Y_, 0);
    SETTING(extruderOffset[10].X_, 0);
    SETTING(extruderOffset[10].Y_, 0);
    SETTING(extruderOffset[11].X_, 0);
    SETTING(extruderOffset[11].Y_, 0);
    SETTING(extruderOffset[12].X_, 0);
    SETTING(extruderOffset[12].Y_, 0);
    SETTING(extruderOffset[13].X_, 0);
    SETTING(extruderOffset[13].Y_, 0);
    SETTING(extruderOffset[14].X_, 0);
    SETTING(extruderOffset[14].Y_, 0);
    SETTING(extruderOffset[15].X_, 0);
    SETTING(extruderOffset[15].Y_, 0);

	startCode =
		"Sliced at: {day} {date} {time}\n"
		"Basic settings: Layer height: {layer_height} Walls: {wall_thickness} Fill: {fill_density}\n"
		"Print time: {print_time}\n"
		"Filament used: {filament_amount}m {filament_weight}g\n"
		"Filament cost: {filament_cost}\n"
		"M190 S60 ;Uncomment to add your own bed temperature line\n"
		"M109 S205 ;Uncomment to add your own temperature line\n"
		"G21        ;metric values\n"
		"G90        ;absolute positioning\n"
		"M82        ;set extruder to absolute mode\n"
		"M107       ;start with the fan off\n"
		"G28 X0 Y0  ;move X/Y to min endstops\n"
		"G28 Z0     ;move Z to min endstops\n"
		"G1 Z15.0 F2500 ;move the platform down 15mm\n"
		"G92 E0                  ;zero the extruded length\n"
		"G1 F200 E3              ;extrude 3mm of feed stock\n"
		"G92 E0                  ;zero the extruded length again\n"
		"G1 F2500\n"
		";Put printing message on LCD screen\n"
		"M117 Printing...\n";
	/*
    startCode =
		"Sliced at: {day} {date} {time}\n"
		"Basic settings: Layer height: {layer_height} Walls: {wall_thickness} Fill: {fill_density}\n"
		"Print time: {print_time}\n"
		"Filament used: {filament_amount}m {filament_weight}g\n"
		"Filament cost: {filament_cost}\n"
		"M190 S{print_bed_temperature} ;Uncomment to add your own bed temperature line\n"
		"M109 S{print_temperature} ;Uncomment to add your own temperature line\n"
		"G21        ;metric values\n"
		"G90        ;absolute positioning\n"
		"M82        ;set extruder to absolute mode\n"
		"M107       ;start with the fan off\n"
		"G28 X0 Y0  ;move X/Y to min endstops\n"
		"G28 Z0     ;move Z to min endstops\n"
		"G1 Z15.0 F{travel_speed} ;move the platform down 15mm\n"
		"G92 E0                  ;zero the extruded length\n"
		"G1 F200 E3              ;extrude 3mm of feed stock\n"
		"G92 E0                  ;zero the extruded length again\n"
		"G1 F{travel_speed}\n"
		";Put printing message on LCD screen\n"
		"M117 Printing...\n";
		*/
    endCode =
        "M104 S0                     ;extruder heater off\n"
        "M140 S0                     ;heated bed heater off (if you have it)\n"
        "G91                            ;relative positioning\n"
        "G1 E-1 F300                    ;retract the filament a bit before lifting the nozzle, to release some of the pressure\n"
        "G1 Z+0.5 E-5 X-20 Y-20 F9000   ;move Z up a bit and retract filament even more\n"
        "G28 X0 Y0                      ;move X/Y to min endstops, so the head is out of the way\n"
        "M84                         ;steppers off\n"
        "G90                         ;absolute positioning\n";

	SETTING(acceleration, 3000 * 1000);
    SETTING(max_acceleration[0], 9000 * 1000);
    SETTING(max_acceleration[1], 9000 * 1000);
    SETTING(max_acceleration[2], 100 * 1000);
    SETTING(max_acceleration[3], 10000 * 1000);
    SETTING(max_xy_jerk, 20.0 * 1000);
    SETTING(max_z_jerk, 0.4 * 1000);
    SETTING(max_e_jerk, 5.0 * 1000);
}

#undef STRINGIFY
#undef SETTING

bool Config::setSetting(const char* key, const char* value)
{
    for(unsigned int n=0; n < _index.size(); n++)
    {
        if (stringcasecompare(key, _index[n].key) == 0)
        {
            *_index[n].ptr = atoi(value);
            return true;
        }
    }
    if (stringcasecompare(key, "startCode") == 0)
    {
        this->startCode = value;
        return true;
    }
    if (stringcasecompare(key, "endCode") == 0)
    {
        this->endCode = value;
        return true;
    }
    if (stringcasecompare(key, "preSwitchExtruderCode") == 0)
    {
        this->preSwitchExtruderCode = value;
        return true;
    }
    if (stringcasecompare(key, "postSwitchExtruderCode") == 0)
    {
        this->postSwitchExtruderCode = value;
        return true;
    }
    return false;
}

bool Config::readSettings(void) {
    return readSettings(DEFAULT_CONFIG_PATH);
}

bool Config::readSettings(const char* path) {
    std::ifstream config(path);
    std::string line;
    size_t line_number = 0;

    if(!config.good())
        return false;

    while(config.good()) {
        bool multilineContent = false; //多线路内容
        size_t pos = std::string::npos;
		/*
		时间：2016年6月30日 10:19:27
		内容：std::getline(config, line);从配置文件中读取一行这里显示不是std的内部函数
		作者：TwoSilly
		*/
        std::getline(config, line);
        line_number += 1;

        // De-comment and trim, skipping anything that shows up empty
        //去评论和修剪，跳过任何显示空的
        pos = line.find_first_of('#');
        if(pos != std::string::npos) line.erase(pos);
        TRIM_STRING(line);
        if(line.length() == 0) continue;

         //分裂成键=值
        std::string key(""), val("");
        pos = line.find_first_of('=');
        if(pos != std::string::npos && line.length() > (pos + 1)) {
            key = line.substr(0, pos);
            val = line.substr(pos + 1);
            TRIM_STRING(key);
            TRIM_STRING(val);
        }

        //我们读到的多行字符串？
        if(val == CONFIG_MULTILINE_SEPARATOR) {
            val = "";
            multilineContent = true;
            bool done_multiline = false;

            while(config.good() && !done_multiline) {
                std::getline(config, line);
                line_number += 1;

                 /*
                    我们RTrim线的原因有两个：
                    1）确保一个直接的= = =比较“”“”的作品不必担心尾随空格。
                    2）没有人喜欢尾随空格无论如何
                */
                RTRIM_STRING(line);

                // Either accumuliate or terminate
                //累积或终止
                if(line == CONFIG_MULTILINE_SEPARATOR) {
                            //配置多路分离器
                    done_multiline = true;
                    //让我们不要加换行 解析的值
                   
                    RTRIM_STRING(val);
                }
                else {
                    line += "\n";
                    val += line;
                }
            }

            //如果我们辍学，但没有完成阅读，一些失败了
            if(!done_multiline) {
                Engine::logError("Config(%s):L%zd: 读取多行字符串时失败.\n", path, line_number);
                return false;
            }

        }


        //失败如果我们没有得到 key和Val
        if(key.length() == 0 || (val.length() == 0 && !multilineContent)) {
            Engine::logError("Config(%s): 行 %zd: 没有找到键值对\n", path, line_number);
            return false;
        }

        // Set a config setting for the current K=V
        //为当前K = V设置一个配置设置
        if(!setSetting(key.c_str(), val.c_str())) {
            Engine::logError("Config(%s):L%zd: 未能设置 '%s' to '%s'\n", path, line_number, key.c_str(), val.c_str());
            return false;
        }
    }

    return true;
}


/*
时间：2016年6月24日 18:42:22
内容：下面这个重写的读取配置文件函数有点bug！不过暂时可以用！米时间就算了！不过得改！最好改成！专门的配置文件类！
作者：TwoSilly

*/
/*
时间：2016年6月23日 20:48:26
内容：我要重写一个读取配置文件并赋值的函数
作者：TwoSilly
*/
bool Config::readSettings_ini(const char* path) {
    //    FILE *fp;//文件指针
    //        fp = fopen(path,"r");
            //判断如果没有打开文件return false
    //        if(fp)
    //            return false;
        
        //ofstream 以输出方式打开
        std::ifstream config(path);
        //判断是否打开文件
        if(!config.is_open())
            return false;
            
            //申请一段内存用来储存每一行的数据
            std::string line;
            size_t line_Number = 0;
            char * note;
    //        size_t pos_ = std::string::npos;
            size_t pos = std::string::npos;
          while( std::getline(config, line))//读取一行
          {
              line_Number += 1;//line Number +1
              
              
              /*
                时间：2016年6月24日 15:05:32
                内容：这里为了找寻配置文件我添加了几项就是机型名字和大小等！这在引擎中是不需要的！
                    不过我打算把他加到界面中！所以这里读到这些配置我先会 跳过他们！
                 作者：TwoSilly
    
                */
               note = (char*)(strstr(line.c_str(),"[")); 
               if(note)
               {
                   if(!stringcasecompare(line.c_str(),"[alterations]"))
                   {
                       
                     // std::cout<<line.c_str()<<std::endl;
                       std::getline(config, line);
                      
                           char* valuePtr = (char*)strchr(line.c_str(), '=');               
                         // std::cout<<"test:"<<line.c_str()<<std::endl;     
                           
                           if (valuePtr)
                           {
                               *valuePtr++ = '\0';
                               std::string temp_str = line.c_str();
                               std::string temp_vel = valuePtr;
                               
                               //我需要在第一个数据之前加一个换行
                               temp_vel.append("\n");
                            
                               while(!config.eof())
                               {
                                   std::getline(config, line);
                                   if(!strstr(line.c_str(),"="))                               
                                        temp_vel.append(line+"\n");
                                   else
                                   {
                                       //std::cout<<"str-->"<<temp_str<<std::endl;
                                       // std::cout<<"vel-->"<<temp_vel<<std::endl;
                                       if (!setSetting(line.c_str(), valuePtr))
                                         Engine::logError("Setting not found: %s %s\n", line.c_str(), valuePtr);
                                         
                                        valuePtr = (char*)strchr(line.c_str(), '=');
                                        if (valuePtr)
                                        {
                                            *valuePtr++ = '\0';
                                        }
                                        temp_str = line.c_str();
                                       
                                        temp_vel = valuePtr;
                                        
                                        
                                   }
                                   //判断是否到文件最后一行
                               }
                               if (!setSetting(line.c_str(), valuePtr))
                                 Engine::logError("Setting not found: %s %s\n",line.c_str(), valuePtr);
                                 
                              // std::cout<<"str-->"<<temp_str<<std::endl;
                               // std::cout<<"vel-->"<<temp_vel<<std::endl;
         
                          }
                               
                       
                   };
                   //continue;
               }
              //找到; 注释 并跳过它
              note = (char*)strstr(line.c_str(),";"); 
              if(note)              
              {             //如果是屏蔽注释或评价
                  //std::cout<<"note:"<<note<<std::endl;
                  //找到；注释掉的位置--因为;为行注释所以只需要检测一行             
                  pos = line.find_first_of(";");
                  line.erase(pos); //剔除注释              
                  
              }
      
              //找到键值对
              note = (char*) strstr(line.c_str(),"=");
              if(note)
              {
                   //pos = line.find_first_of("=");
                   char* valuePtr = (char*)strchr(line.c_str(), '=');               
                 // std::cout<<"test:"<<line.c_str()<<std::endl;     
                   
                   if (valuePtr)
                   {
                       *valuePtr++ = '\0';
                        //std::cout<<"line:"<<line.c_str()<<std::endl;  
                        //std::cout<<"valuePtr:"<<valuePtr<<std::endl;
                        /*
                            时间：2016年6月24日 15:01:21
                            内容：这里就可以添加键值对了！根据读出来的配置文件，配置
                            作者：TwoSilly
                            if (!config.setSetting(line.c_str(), valuePtr))
                              cura::logError("Setting not found: %s %s\n", argv[argn], valuePtr);
                        */
                        if (!setSetting(line.c_str(), valuePtr))
                          Engine::logError("Setting not found: %s %s\n", line.c_str(), valuePtr);
                    
                   }
              }
    
           
          }
          config.close();
         //在文件最后一定要关闭他
    
        return true;
    }


#undef STRINGIFY
#undef SETTING