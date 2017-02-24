/**

文件名：raft.h

时间：2016年8月1日 14:29:47
内容：伐，木筏，底阀
注：算法没看，这是产生底阀的模块
作者：TwoSilly

E-Mail:twosilly@foxmail.com
*/

#ifndef RAFT_H
#define RAFT_H

#include "sliceDataStorage.h"

namespace Engine {
/**
时间：2016年8月1日 09:09:33
内容：形成，产生底阀
	参数：
		SliceDataStorage& storage --> 切片数据储存
		int distance --> 距离
作者：TwoSilly
*/
void generateRaft(SliceDataStorage& storage, int distance);

}//namespace Engine

#endif//RAFT_H
