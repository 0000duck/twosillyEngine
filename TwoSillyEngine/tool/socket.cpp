
/**

文件名：socket.h

时间：2016年7月4日 09:29:54
内容：网络编程套接字
作者：TwoSilly
E-Mail:twosilly@foxmail.com
*/



#include <stdio.h>
#include <string.h>


#ifdef __WIN32
#include <winsock2.h>
#pragma  comment(lib,"ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "socket.h"
#include "logoutput.h"


//#ifdef 如果宏被定义就进行编译
#ifdef __WIN32
 bool wsaStartupDone = false;
#endif


ClientSocket::ClientSocket()//客户端 网络协议
{
    sockfd = -1;
	//这是一个文件指针是为了我把，套接字信息接收出来！
	
    
	Send.open("Send.txt");
	Send_end.open("Send_end.txt");
	Recv.open("Recv.txt");
	Recv_end.open("Recv_end.txt");
#ifdef __WIN32
    if (!wsaStartupDone)
    {
        WSADATA wsaData;
        memset(&wsaData, 0, sizeof(WSADATA));
        //它需要调用Windows套接字可以使用之前。请求版本1.1，这是支持在窗口98和更高。
        WSAStartup(MAKEWORD(1, 1), &wsaData);
        wsaStartupDone = true;
    }
#endif


}


/**
	时间：2016年7月9日 16:54:10
	内容：链接到..
		参数：
			std::string host --> 主机
			int port --> 接口
	作者：TwoSilly
*/
void ClientSocket::connectTo(std::string host, int port)
{
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(host.c_str());

	/*
	这句是为了将端口和主机ip存入txt
	*/
	//Send<<"Port:"<<port<<","<<"host:"<<host<<"\n";

    
	//检查：C式铸铁被重新诠释的_铸造！！
    if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0)
    {
        printf("Connect to %s:%d failed\n", host.c_str(), port);
        close();
        return;
    }
}

ClientSocket::~ClientSocket()
{
	Recv.close();
	Send.close();
	Recv_end.close();
	Send_end.close();
    close();
}

void ClientSocket::sendNr(int nr)
{
	//Send<<"先的:"<<nr<<"\n";
    sendAll(&nr, sizeof(int));
}


	/**
	时间：2016年7月9日 17:01:32
	内容：发送全部
		参数：
			const void* data --> 数据	
			int length --> 发送的数据的长度	
	作者：TwoSilly
	*/
void ClientSocket::sendAll(const void* data, int length)
{
    if (sockfd == -1)
        return;
	const float * count = static_cast<const float *>(data);
    const char* ptr = static_cast<const char*>(data);
	/*
	检测发送了多少数据
	*/
	if(4 == length)
	{
	Send<<"Send: ";
	Send<<"data:"<<*count<<","<<"Length:"<<length<<"\n";
	}else{
	Send<<"Send: ";
	Send<<"data:"<<*ptr<<","<<"Length:"<<length<<"\n";
	}
    while(length > 0)
    {
        int n = send(sockfd, ptr, length, 0);
        if (n <= 0)
        {
            close();
            return;
        }

        ptr += length;
        length -= n;    
	}
		Send_end<<"Send: ";
		Send_end<<"data:"<<*ptr<<","<<"Length:"<<length<<"\n";
}

	/**
	时间：2016年7月9日 17:01:32
	内容：接收
	作者：TwoSilly
	*/
int ClientSocket::recvNr() //recv接收
{
    /*
    时间：2016年6月21日 11:24:31
    内容：这里的4 这个参数应该是sizeof(int)
    */
    int ret = 0;
    recvAll(&ret, 4);
    return ret;
}

	/**
	时间：2016年7月9日 17:01:32
	内容：接收全部
		参数：
			const void* data --> 数据	
			int length --> 发送的数据的长度	
	作者：TwoSilly
	*/
void ClientSocket::recvAll(void* data, int length)
{
    if (sockfd == -1)
        return;
	static int vertexCount = 1;
	float* Data_temp = static_cast<float*>(data);
    char* ptr = static_cast<char*>(data);
	//string str();这个if不用管，只是我把数据传入TXT
	Recv<<"Recv: ";
	if(length == 4)
	{
		Recv<<"Data:"<<*Data_temp<<","<<"Length:"<<length<<"\n";
	}else if(length == 12)
	{
		 Recv<<"Data:"<<vertexCount++<<"--> ("<<Data_temp[0]<<","<<Data_temp[1]<<","<<Data_temp[2]<<")"<<"\n";
	}else
	{
		Recv<<"Data:"<<*Data_temp<<","<<"Length:"<<length<<"\n";
	}
	while(length > 0)
    {
        int n = recv(sockfd, ptr, length, 0);
        if (n <= 0)
        {
            Engine::log("ClientSocket::recvAll error...");//这个打印出来了？没有

            close();
            return;
        }        
		ptr += n;
        length -= n;
	

    }

}

void ClientSocket::close()
{
	if (sockfd == -1)
		return;
	
#ifdef __WIN32
	closesocket(sockfd);
#else
	::close(sockfd);
#endif
	sockfd = -1;
}
