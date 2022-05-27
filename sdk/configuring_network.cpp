// sdk.cpp : 定义静态库的函数。
//
#include "pch.h"
#include "framework.h"
#include "configuring_network.h"
#include "../firmware/easylogging++.h"

INITIALIZE_EASYLOGGINGPP

// TODO: 这是一个库函数示例
/*******************************************************************************************/

bool BIND_FLAG_ = false;
const int MAX_BUF_LEN = 255;
// 创建socket
SOCKET connect_socket;
// 用来从网络上的广播地址接收数据
SOCKADDR_IN sin_from;

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
/*
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
*/
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
		LOG(INFO) << "socket error: error code is " << err;
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
		LOG(INFO) << "sendto error: error code is " << err;
		return -1;
	}

	return 0;
}

int set_recv_timeout(int nNetTimeout)
{
	setsockopt(connect_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
	return 0;
}

int set_send_timeout(int nNetTimeout)
{
	setsockopt(connect_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(int));
	return 0;
}

int bind_server_port(int nNetTimeout)
{
	if (BIND_FLAG_)
	{
		set_recv_timeout(nNetTimeout);
		LOG(INFO) << "BIND_FLAG_: already true.";
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
		LOG(INFO) << "WSAStartup: error code is " << err;
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		LOG(INFO) << "wsaData.wVersion error: " << wsaData.wVersion;
		return -1;
	}

	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connect_socket)
	{
		err = WSAGetLastError();
		LOG(INFO) << "socket error: error code is " << err;
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
		LOG(INFO) << "bind error: error code is " << err;
		return -1;
	}

	BIND_FLAG_ = true;
	
	return 0;
}
/*
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
*/
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
//		LOG(INFO) << "recvfrom error: error code is " << err;
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

// -- Set ip address, sleep for network re-up, then get the new ip address.
int SetCameraIp(std::string mac, std::string& ip)
{
	LOG(INFO) << "SET_IP......";
	int ret = bind_server_port(0.3 * 1000);
	if (0 != ret) {
		LOG(INFO) << "SetCameraIp: bind error and continue";
	}

	// set ip address
	char command_c[MAX_BUF_LEN] = "";
	sprintf(command_c, "%8d", DF_CMD_SET_NETWORK_IP);
	std::string str_data = std::string(command_c) + ";" + std::string(mac) + ";" + std::string(ip);
	char* message_buff = const_cast<char*>(str_data.c_str());
	send_server_data(message_buff);
	LOG(INFO) << "Set static ip complete, wait for network to be accessed.";

	// wait 5sec at least for camera network re-up
	Sleep(5000);

	// get the new ip address
	memset(command_c, 0xff, MAX_BUF_LEN);
	sprintf(command_c, "%8d", DF_CMD_GET_NETWORK_IP);
	str_data = std::string(command_c) + ";" + std::string(mac) + ";" + std::string(ip);
	message_buff = const_cast<char*>(str_data.c_str());
	send_server_data(message_buff);

	char recv_buff[MAX_BUF_LEN] = "";
	int recv_buff_size = 0;
	ret = recv_server_data(recv_buff, recv_buff_size);
	if (0 != ret)
	{
		LOG(INFO) << "get new ip address error";
		return -1;
	}

	std::string rec_str = std::string(recv_buff);
	std::vector<std::string> str_list = vStringSplit(rec_str, ";");
	LOG(INFO) << "new ip address:" << str_list[0];

	if (str_list.size() > 0) {
		ip = str_list[0];
	} else {
		ip = "";
	}

	return 0;
}

// -- Set the network to be dhcp mode, sleep for network re-up, then get the new ip address.
int SetCameraIpAuto(std::string mac, std::string& ip)
{
	LOG(INFO) << "SET_AUTO......";
	int ret = bind_server_port(0.3 * 1000);
	if (0 != ret) {
		LOG(INFO) << "SetCameraIpAuto: bind error and continue";
	}

	// set ip address be dhcp mode
	char command_c[MAX_BUF_LEN] = "";
	sprintf(command_c, "%8d", DF_CMD_SET_NETWORK_DHCP);
	printf("mac: %s \n", mac.c_str());
	std::string str_data = std::string(command_c) + ";" + std::string(mac);
	char* message_buff = const_cast<char*>(str_data.c_str());
	send_server_data(message_buff);
	LOG(INFO) << "Set dhcp complete, wait for DHCP discover";

	// wait 20sec at least for camera network re-up
	Sleep(20000);

	// get the new ip address
	memset(command_c, 0xff, MAX_BUF_LEN);
	sprintf(command_c, "%8d", DF_CMD_GET_NETWORK_IP);
	str_data = std::string(command_c) + ";" + std::string(mac) + ";" + std::string(ip);
	message_buff = const_cast<char*>(str_data.c_str());
	send_server_data(message_buff);

	char recv_buff[MAX_BUF_LEN] = "";
	int recv_buff_size = 0;
	ret = recv_server_data(recv_buff, recv_buff_size);
	if (0 != ret) 
	{
		LOG(INFO) << "get new ip address error";
		return -1;
	}

	std::string rec_str = std::string(recv_buff);
	std::vector<std::string> str_list = vStringSplit(rec_str, ";");
	LOG(INFO) << "new ip address:" << str_list[0];

	if (str_list.size() > 0) {
		ip = str_list[0];
	} else {
		ip = "";
	}

	return 0;
}

// -- Set ip address/netmask/gateway/DHCP, sleep for network re-up, then get the new setting.
int SetCameraNetwork(long cmd, std::string mac, std::string &cfg, long sleep)
{
	LOG(INFO) << "Camera Network Setting";
	int ret = bind_server_port(0.3 * 1000);
	if (0 != ret) {
		LOG(INFO) << "bind error and continue";
	}

	// set command
	char command_c[MAX_BUF_LEN] = "";
	sprintf(command_c, "%8d", cmd);
	std::string str_data = std::string(command_c) + ";" + std::string(mac) + ";" + std::string(cfg);
	char* message_buff = const_cast<char*>(str_data.c_str());
	send_server_data(message_buff);

	// instruction list
	long getCmd = DF_CMD_GET_NETWORK_IP;
	switch (cmd)
	{
		case DF_CMD_SET_NETWORK_IP:
			getCmd = DF_CMD_GET_NETWORK_IP;
			LOG(INFO) << "Set static ip complete, wait for network to be accessed.";
			break;

		case DF_CMD_SET_NETWORK_NETMASK:
			getCmd = DF_CMD_GET_NETWORK_NETMASK;
			LOG(INFO) << "Set netmask complete, wait for network to be accessed.";
			break;

		case DF_CMD_SET_NETWORK_GATEWAY:
			getCmd = DF_CMD_GET_NETWORK_GATEWAY;
			LOG(INFO) << "Set gateway complete, wait for network to be accessed.";
			break;

		case DF_CMD_SET_NETWORK_DHCP:
			getCmd = DF_CMD_GET_NETWORK_IP;
			LOG(INFO) << "Set DHCP complete, wait for DHCP discover";
			break;

		default:
			break;
	}

	// wait Xsec at least for camera network re-up
	Sleep(sleep);

	// get the new setting
	memset(command_c, 0xff, MAX_BUF_LEN);
	sprintf(command_c, "%8d", getCmd);
	str_data = std::string(command_c) + ";" + std::string(mac) + ";" + std::string(cfg);
	message_buff = const_cast<char*>(str_data.c_str());
	send_server_data(message_buff);

	char recv_buff[MAX_BUF_LEN] = "";
	int recv_buff_size = 0;
	ret = recv_server_data(recv_buff, recv_buff_size);
	if (0 != ret)
	{
		LOG(INFO) << "get new setting error";
		return -1;
	}

	// unpack the setting
	std::string rec_str = std::string(recv_buff);
	std::vector<std::string> str_list = vStringSplit(rec_str, ";");
	LOG(INFO) << "new setting:" << str_list[0];

	if (str_list.size() > 0) {
		cfg = str_list[0];
	}
	else {
		cfg = "";
	}

	return 0;
}

int GetCameraList(std::vector<std::string>& mac_list, std::vector<std::string>& ip_list)
{
	std::cout << "GET_MAC......" << std::endl;

	int ret = bind_server_port(0.1 * 1000);
	if (0 != ret)
	{
		LOG(INFO) << "bind error";
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
			LOG(INFO) << "mac: " << recv_buff_temp;

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