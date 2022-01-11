// sdk.cpp : 定义静态库的函数。
//

#include "pch.h"
#include "framework.h"
#include "configuring_network.h"

// TODO: 这是一个库函数示例
/*******************************************************************************************/

bool BIND_FLAG_ = false;
const int MAX_BUF_LEN = 255;
// 创建socket
SOCKET connect_socket;
// 用来从网络上的广播地址接收数据
SOCKADDR_IN sin_from;

int send_command(int command);
int send_server_data(const char* buff);  
int send_param(const char* buff); 
int bind_server_port(int nNetTimeout = 30 * 1000);
int set_timeout(int nNetTimeout = 30 * 1000);
int recv_server_data(char* buffer, int& buffer_size);
int recv_server_command(int& command);

std::vector<std::string> vStringSplit(const std::string& s, const std::string& delim);

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


int set_timeout(int nNetTimeout)
{

	setsockopt(connect_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));

	return 0;
}

int bind_server_port(int nNetTimeout)
{

	if (BIND_FLAG_)
	{
		set_timeout(nNetTimeout);
		return -1;
	}
	 

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

		BIND_FLAG_ = true;
	

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

std::vector<std::string> vStringSplit(const std::string& s, const std::string& delim)
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0)
		return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

/*******************************************************************************************/

int SetCameraIp(std::string mac, std::string& ip)
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


	char recv_buff[MAX_BUF_LEN] = "";
	int recv_buff_size = 0;
	recv_server_data(recv_buff, recv_buff_size);
	printf("ADDRESS: %s \n", recv_buff);


	std::string rec_str = std::string(recv_buff);

	std::vector<std::string> str_list = vStringSplit(rec_str, ";");

	if (str_list.size() > 0)
	{

		ip = str_list[0];
	}
	else
	{
		ip = "";
	}

	return 0;
}

int SetCameraIpAuto(std::string mac, std::string& ip)
{
	std::cout << "SET_AUTO......" << std::endl;
	int ret = bind_server_port(60 * 1000);
	if (0 != ret)
	{
		std::cout << "bind error" << std::endl; 
	}


	char command_c[MAX_BUF_LEN] = "";
	sprintf(command_c, "%8d", DF_CMD_SET_NETWORK_DHCP);


	printf("mac: %s \n", mac.c_str());
	std::string str_data = std::string(command_c) + ";" + mac;
	char* message_buff = const_cast<char*>(str_data.c_str());


	send_server_data(message_buff);


	char recv_buff[MAX_BUF_LEN] = "";
	int recv_buff_size = 0;
	recv_server_data(recv_buff, recv_buff_size);

	printf("ADDRESS: %s \n", recv_buff);
	 
	std::string rec_str = std::string(recv_buff);

	std::vector<std::string> str_list = vStringSplit(rec_str, ";");

	if (str_list.size() > 0)
	{

		ip = str_list[0];
	}
	else
	{
		ip = "";
	}


	

	return 0;
}

int GetCameraList(std::vector<std::string>& mac_list, std::vector<std::string>& ip_list)
{

	std::cout << "GET_MAC......" << std::endl;


	int ret = bind_server_port(0.1 * 1000);
	if (0 != ret)
	{
		std::cout << "bind error" << std::endl;
	}


	send_command(DF_CMD_GET_NETWORK_MAC);

	std::vector<std::string> str_list;

	for (int i = 0; i < 20; i++)
	{

		char recv_buff_temp[MAX_BUF_LEN] = "";
		int recv_buff_size_temp = 0;
		recv_server_data(recv_buff_temp, recv_buff_size_temp);


		if (recv_buff_size_temp > 0)
		{
			//printf("mac: %s \n", recv_buff_temp);

			std::string mac_str(recv_buff_temp);

			str_list.push_back(mac_str);
		}

	}
	 
	mac_list.clear();
	ip_list.clear();

	for (int n = 0; n < str_list.size(); n++)
	{
		std::vector<std::string> str = vStringSplit(str_list[n], ";");
		
		if (str.size() >= 2)
		{
			mac_list.push_back(str[0]);
			ip_list.push_back(str[1]);
		}
		  
	}

	return 0;
}