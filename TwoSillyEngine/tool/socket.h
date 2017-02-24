
/**

文件名：socket.h

时间：2016年7月4日 09:29:54
内容：网络编程套接字
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/

#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <fstream>


/**
时间：2016年7月9日 16:14:42
内容：客户端套接字类
	注;网络编程我不熟，或者说不懂，不过这里我感觉就没用到他，所以我打算暂时不管了
		不过等一切弄完后我该去看网编了！
作者：TwoSilly
*/


class ClientSocket
{
    int sockfd;
	std::ofstream Send;
	std::ofstream Send_end;
	std::ofstream Recv;
	std::ofstream Recv_end;
    //ocout.open("test.txt");
public:
   ClientSocket();
    ~ClientSocket();
    
	/**
	时间：2016年7月9日 16:54:10
	内容：链接到..
		参数：
			std::string host --> 主机
			int port --> 接口
	作者：TwoSilly
	*/
    void connectTo(std::string host, int port);
	/**
	时间：2016年7月9日 17:01:32
	内容：发送数据
		参数：
			int nr --> 发送的数据			
	作者：TwoSilly
	*/
    void sendNr(int nr);

	/**
	时间：2016年7月9日 17:01:32
	内容：发送全部
		参数：
			const void* data --> 数据	
			int length --> 发送的数据的长度	
	作者：TwoSilly
	*/
    void sendAll(const void* data, int length);

	/**
	时间：2016年7月9日 17:01:32
	内容：接收
	作者：TwoSilly
	*/
    int recvNr();

	
	/**
	时间：2016年7月9日 17:01:32
	内容：接收全部
		参数：
			const void* data --> 数据	
			int length --> 发送的数据的长度	
	作者：TwoSilly
	*/
    void recvAll(void* data, int length);
    
	//关闭链接
    void close();
};

#endif//SOCKET_H
