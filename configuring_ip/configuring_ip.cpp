// configuring_ip.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <WINSOCK2.H>
#include <iostream>
#include <sstream> 
#include "getopt.h"
#include "protocol.h"
#include <vector>

#pragma comment(lib,"WS2_32.lib")
using namespace std;

/**********************************************************************************************************/


const char* help_info =
"Examples:\n\
\n\
1.Get camera list:\n\
configuring_ip.exe --get-list \n\
\n\
2.Get address:\n\
configuring_ip.exe --get-address --mac xx:xx:xx:xx:xx:xx \n\
\n\
3.Get ip:\n\
configuring_ip.exe --get-ip --mac xx:xx:xx:xx:xx:xx \n\
\n\
4.Get netmask:\n\
configuring_ip.exe --get-netmask --mac xx:xx:xx:xx:xx:xx \n\
\n\
5.Get gateway:\n\
configuring_ip.exe --get-gateway --mac xx:xx:xx:xx:xx:xx \n\
\n\
6.Set ip:\n\
configuring_ip.exe --set-ip 192.168.1.100 --mac xx:xx:xx:xx:xx:xx \n\
\n\
7.Set netmask:\n\
configuring_ip.exe --set-netmask 255.255.255.0 --mac xx:xx:xx:xx:xx:xx \n\
\n\
8.Set gateway:\n\
configuring_ip.exe --set-gateway 192.168.1.1 --mac xx:xx:xx:xx:xx:xx \n\
\n\
9.Set auto:\n\
configuring_ip.exe --set-auto --mac xx:xx:xx:xx:xx:xx \n\
";


extern int optind, opterr, optopt;
extern char* optarg;

enum opt_set
{
	SET_AUTO,
	SET_IP,
	SET_NETMASK,
	SET_GATEWAY, 
	GET_LIST,
	GET_ADDRESS,
	GET_IP,
	GET_NETMASK,
	GET_GATEWAY, 
	GET_MAC,
	MAC,
	HELP
};

static struct option long_options[] =
{
	{"mac",required_argument,NULL,MAC},
	{"set-ip",required_argument,NULL,SET_IP},
	{"set-netmask",required_argument,NULL,SET_NETMASK},
	{"set-gateway",required_argument,NULL,SET_GATEWAY}, 
	{"set-auto",no_argument,NULL,SET_AUTO},
	{"get-address",no_argument,NULL,GET_ADDRESS},
	{"get-ip", no_argument, NULL, GET_IP},
	{"get-netmask",no_argument,NULL,GET_NETMASK},
	{"get-gateway",no_argument,NULL,GET_GATEWAY}, 
	{"get-list",no_argument,NULL,GET_MAC},
	{"help",no_argument,NULL,HELP},
};


const char* camera_id;
const char* path;
int command = HELP;


/**********************************************************************************************************/


const int MAX_BUF_LEN = 255;

// 创建socket
SOCKET connect_socket;
// 用来从网络上的广播地址接收数据
SOCKADDR_IN sin_from;

const char* message;
const char* ip;
const char* netmask;
const char* gateway;
const char* dns;
const char* mac;




int send_command(int command); 
int send_server_data(const char* buff);


int send_param(const char* buff); 

int bind_server_port(int nNetTimeout = 30* 1000);
int recv_server_data(char* buffer, int& buffer_size);
int recv_server_command(int& command);


int main(int argc, char* argv[])
{
	int c = 0; 

	while (EOF != (c = getopt_long(argc, argv, "i:h", long_options, NULL)))
	{
		switch (c)
		{
		case SET_IP:
			ip = optarg;
			command = c;
			break;
		case SET_NETMASK:
			netmask = optarg;
			command = c;
			break;
		case SET_GATEWAY:
			gateway = optarg;
			command = c;
			break; 
		case MAC:
			mac = optarg; 
			break;
		case '?':
			printf("unknow option:%c\n", optopt);
			break;
		default:
			command = c;
			break;
		}
	}


	char recv_buff[MAX_BUF_LEN] = "";
	int recv_buff_size = 0;

	std::vector<std::string> mac_list_; 

	switch (command)
	{
	case HELP:
		printf(help_info);
		break;

	case GET_MAC:
	{
		std::cout << "GET_MAC......" << std::endl;


		int ret = bind_server_port(0.1*1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		}

  
		send_command(DF_CMD_GET_NETWORK_MAC);

		

		for (int i = 0; i < 20; i++)
		{

			char recv_buff_temp[MAX_BUF_LEN] = "";
			int recv_buff_size_temp = 0;
			recv_server_data(recv_buff_temp, recv_buff_size_temp);
			 

			if (recv_buff_size_temp > 0)
			{ 
				printf("mac: %s \n", recv_buff_temp);

				std::string mac_str(recv_buff_temp);

				mac_list_.push_back(mac_str);
			}
			 
		}
	}


		break;

	case GET_ADDRESS:
	{
		std::cout << "GET_ADDRESS......" << std::endl;

		int ret = bind_server_port(3 * 1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		} 

		char command_c[MAX_BUF_LEN] = "";
		sprintf(command_c, "%8d", DF_CMD_GET_NETWORK_ADDRESS);


		std::string str_data = std::string(command_c) + ";" + std::string(mac);
		char* message_buff = const_cast<char*>(str_data.c_str());

		 

		send_server_data(message_buff);
		 
		recv_server_data(recv_buff, recv_buff_size);


		printf("ADDRESS: %s \n", recv_buff);


	}

		break;
	case GET_IP:
	{		
		std::cout << "GET_IP......" << std::endl;
		int ret = bind_server_port(3 * 1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		}


		char command_c[MAX_BUF_LEN] = "";
		sprintf(command_c, "%8d", DF_CMD_GET_NETWORK_IP);


		std::string str_data = std::string(command_c) + ";" + std::string(mac);
		char* message_buff = const_cast<char*>(str_data.c_str());

		 
		send_server_data(message_buff);
		 
		recv_server_data(recv_buff, recv_buff_size);

		printf("IP: %s \n", recv_buff);

	}

		break;
	case GET_NETMASK:
	{
		std::cout << "GET_NETMASK......" << std::endl;
		int ret = bind_server_port(3 * 1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		}

		char command_c[MAX_BUF_LEN] = "";
		sprintf(command_c, "%8d", DF_CMD_GET_NETWORK_NETMASK);


		std::string str_data = std::string(command_c) + ";" + std::string(mac);
		char* message_buff = const_cast<char*>(str_data.c_str());
		 
		send_server_data(message_buff);
		 
		recv_server_data(recv_buff, recv_buff_size);


		printf("NETMASK: %s \n", recv_buff);
	}

		break;
	case GET_GATEWAY:
	{
		std::cout << "GET_GATEWAY......" << std::endl;
		int ret = bind_server_port(3 * 1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		}


		char command_c[MAX_BUF_LEN] = "";
		sprintf(command_c, "%8d", DF_CMD_GET_NETWORK_GATEWAY);


		std::string str_data = std::string(command_c) + ";" + std::string(mac);
		char* message_buff = const_cast<char*>(str_data.c_str());

		send_server_data(message_buff);
		 
		recv_server_data(recv_buff, recv_buff_size);

		printf("GATEWAY: %s \n", recv_buff);
	}

		break;
	case SET_AUTO:
	{
		std::cout << "SET_AUTO......" << std::endl;
		int ret = bind_server_port(60 * 1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		}


		char command_c[MAX_BUF_LEN] = "";
		sprintf(command_c, "%8d", DF_CMD_SET_NETWORK_DHCP);


		printf("mac: %s \n", mac);
		std::string str_data = std::string(command_c) + ";" + std::string(mac);
		char* message_buff = const_cast<char*>(str_data.c_str());


		send_server_data(message_buff); 

		recv_server_data(recv_buff, recv_buff_size);

		printf("ADDRESS: %s \n", recv_buff);

	}

	break;
	case SET_IP:
	{
		std::cout << "SET_IP......" << std::endl;
		int ret = bind_server_port(32 * 1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		}

		char command_c[MAX_BUF_LEN] = "";
		sprintf(command_c, "%8d", DF_CMD_SET_NETWORK_IP);


		std::string str_data = std::string(command_c) + ";" + std::string(mac) + ";" + std::string(ip);
		char* message_buff = const_cast<char*>(str_data.c_str());

		send_server_data(message_buff);


		recv_server_data(recv_buff, recv_buff_size);
		printf("ADDRESS: %s \n", recv_buff);
 

	}

	break;
	case SET_NETMASK:
	{
		std::cout << "SET_NETMASK......" << std::endl;
		int ret = bind_server_port(30 * 1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		}

		char command_c[MAX_BUF_LEN] = "";
		sprintf(command_c, "%8d", DF_CMD_SET_NETWORK_NETMASK);


		std::string str_data = std::string(command_c) + ";" + std::string(mac) + ";" + std::string(netmask);
		char* message_buff = const_cast<char*>(str_data.c_str());

		send_server_data(message_buff);
		recv_server_data(recv_buff, recv_buff_size);

		printf("ADDRESS: %s \n", recv_buff);
	}

	break;
	case SET_GATEWAY:
	{
		std::cout << "SET_GATEWAY......" << std::endl;
		int ret = bind_server_port(30 * 1000);
		if (0 != ret)
		{
			std::cout << "bind error" << std::endl;
		}

		char command_c[MAX_BUF_LEN] = "";
		sprintf(command_c, "%8d", DF_CMD_SET_NETWORK_GATEWAY);

		std::string str_data = std::string(command_c) + ";" + std::string(mac) + ";" + std::string(gateway);
		char* message_buff = const_cast<char*>(str_data.c_str());

		send_server_data(message_buff);
		recv_server_data(recv_buff, recv_buff_size);

		printf("ADDRESS: %s \n", recv_buff);
	}

	break;
	default:
		break;
	}

	 


	/*******************************************************************************/

}

 

int send_server_data(const char* buff)
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// 启动socket api
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}

	// 创建socket
	SOCKET connect_socket;
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connect_socket)
	{
		err = WSAGetLastError();
		//printf("/"socket / " error! error code is %d/n", err);
		std::cout << "socket error: error code is " << err;
		return -1;
	}

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);
	sin.sin_addr.s_addr = INADDR_BROADCAST;

	bool bOpt = true;
	//设置该套接字为广播类型
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	int nAddrLen = sizeof(SOCKADDR);

 

	// 发送数据
	int nSendSize = sendto(connect_socket, buff, strlen(buff), 0, (SOCKADDR*)&sin, nAddrLen);
	if (SOCKET_ERROR == nSendSize)
	{
		err = WSAGetLastError();
		//printf("/"sendto / " error!, error code is %d/n", err);
		std::cout << "sendto error: error code is " << err;
		return -1;
	}
 

	return 0;
}

int send_param(const char* buff)
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// 启动socket api
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}

	// 创建socket
	SOCKET connect_socket;
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connect_socket)
	{
		err = WSAGetLastError(); 
		std::cout << "socket error: error code is " << err;
		return -1;
	}

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);
	sin.sin_addr.s_addr = INADDR_BROADCAST;

	bool bOpt = true;
	//设置该套接字为广播类型
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	int nAddrLen = sizeof(SOCKADDR);
 


	/**********************************/
	int ret = bind_server_port();

	if (0 != ret)
	{
		std::cout << "bind error" << std::endl;
	}
	 

	// 发送数据
	int nSendSize = sendto(connect_socket, buff, strlen(buff), 0, (SOCKADDR*)&sin, nAddrLen);
	if (SOCKET_ERROR == nSendSize)
	{
		err = WSAGetLastError();
		//printf("/"sendto / " error!, error code is %d/n", err);
		std::cout << "sendto error: error code is " << err;
		return -1;
	}
	printf("Send: %s \n", buff);
	 
	return 0;
}

int send_command(int command)
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// 启动socket api
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}

	// 创建socket
	SOCKET connect_socket;
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connect_socket)
	{
		err = WSAGetLastError(); 
		std::cout << "socket error: error code is " << err;
		return -1;
	}

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);
	sin.sin_addr.s_addr = INADDR_BROADCAST;

	bool bOpt = true;
	//设置该套接字为广播类型
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	int nAddrLen = sizeof(SOCKADDR);

	char buff[MAX_BUF_LEN] = "";
 
	sprintf(buff, "%8d", command);

	// 发送数据
	int nSendSize = sendto(connect_socket, buff, strlen(buff), 0, (SOCKADDR*)&sin, nAddrLen);
	if (SOCKET_ERROR == nSendSize)
	{
		err = WSAGetLastError();
		//printf("/"sendto / " error!, error code is %d/n", err);
		std::cout << "sendto error: error code is " << err;
		return -1;
	}
 

	return 0;
}

int bind_server_port(int nNetTimeout)
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// 启动socket api
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}

  
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connect_socket)
	{
		err = WSAGetLastError();
		//printf("/"socket / " error! error code is %d/n", err);
		std::cout << "socket error: error code is " << err;
		return -1;
	}

	// 用来绑定套接字
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(CLIENT_PORT);
	sin.sin_addr.s_addr = 0;


	sin_from.sin_family = AF_INET;
	sin_from.sin_port = htons(CLIENT_PORT);
	sin_from.sin_addr.s_addr = INADDR_BROADCAST;

	//设置该套接字为广播类型，
	bool bOpt = true;
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	//int nNetTimeout = 0.1 * 1000;//1秒，
	setsockopt(connect_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));

	// 绑定套接字
	err = bind(connect_socket, (SOCKADDR*)&sin, sizeof(SOCKADDR));
	if (SOCKET_ERROR == err)
	{
		err = WSAGetLastError(); 

		std::cout << "bind error: error code is " << err;

		return -1;
	}

	return 0;
}


int recv_server_command(int& command)
{

	int err;

	int nAddrLen = sizeof(SOCKADDR);
	char buff[MAX_BUF_LEN] = "";
	int nLoop = 0;
 
	int nSendSize = recvfrom(connect_socket, buff, MAX_BUF_LEN, 0, (SOCKADDR*)&sin_from, &nAddrLen);
	if (SOCKET_ERROR == nSendSize)
	{
		err = WSAGetLastError();
 
		return -1;
	}
	buff[nSendSize] = '\0';
	printf("Recv: %s \n", buff);

	std::cout << "nSendSize: " << nSendSize << std::endl;
 

	std::string command_str(buff);

	std::stringstream ss;
	ss << command_str;
	ss >> command;


	return 0;;
}


int recv_server_data(char* buffer, int& buffer_size)
{


	int err;

	int nAddrLen = sizeof(SOCKADDR); 
	int nLoop = 0;
 
		// 接收数据
	int nSendSize = recvfrom(connect_socket, buffer, MAX_BUF_LEN, 0, (SOCKADDR*)&sin_from, &nAddrLen);
	if (SOCKET_ERROR == nSendSize)
	{
		err = WSAGetLastError(); 
		return -1;
	}
	buffer[nSendSize] = '\0';
  
	buffer_size = nSendSize;

	return 0;
}






