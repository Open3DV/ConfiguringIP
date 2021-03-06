#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "ip_inquire.h"
#include "protocol.h"
#include "ip_config.h"
#include "easylogging++.h"

// init easylogging
INITIALIZE_EASYLOGGINGPP

extern int errno;
int geterror() { return errno; }

const int MAX_BUF_LEN = 255;

int send_command(int command);

int send_data_to_client(char *buffer, int buffer_size);

int bind_client_port();

int recv_client_data(char *buffer);

int restart_networking();

// 创建socket
int connect_socket;
// 用来从网络上的广播地址接收数据
struct sockaddr_in sin_from;

int main(int argc, char *argv[])
{
	int ret = 0; 
	char mac_buff[512] = ""; 
	ret = getLocalMacAddr(mac_buff); 

	std::string mac_addr_(mac_buff); 
    LOG(INFO)<<"Configuring IP start, MAC: "<<mac_addr_;

	//绑定客户端端口
	ret = bind_client_port();

	if (0 != ret)
	{
    	LOG(INFO)<<"bind_client_port error!";
		return -1;
	}

	char buff[MAX_BUF_LEN] = "";
	
	while (1)
	{  
		ret = recv_client_data(buff); 

		if (ret != 0) {
			return -1;
		}

		std::string data_str(buff);
		std::vector<std::string> param_list;
		param_list = vStringSplit(data_str, ";");
 
		int command = 0; 
		std::stringstream ss;
		ss << param_list[0];
		ss >> command;

		std::string now_address;
		std::string now_netmask;
		std::string now_gateway;
		std::string now_dns;

		switch (command)
		{
			case DF_CMD_GET_NETWORK_MAC:
			{
				LOG(INFO) << "DF_CMD_GET_NETWORK_MAC";

				std::string address;
				std::string netmask;
				std::string gateway;

				bool ret = inquireNetworkAddress(address, netmask, gateway);
				std::string message = "";

				message += std::string(mac_buff);
				message += ";";
				message += address;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
			}
			break;

			case DF_CMD_GET_NETWORK_ADDRESS:
			{
				LOG(INFO) << "DF_CMD_GET_NETWORK_ADDRESS";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error";
					continue;
				}

				std::string address;
				std::string netmask;
				std::string gateway;
				bool ret = inquireNetworkAddress(address, netmask, gateway);

				std::string message = "";
				message += address;
				message += ";";
				message += netmask;
				message += ";";
				message += gateway;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
			}
			break;

			case DF_CMD_GET_NETWORK_IP:
			{
				LOG(INFO) << "DF_CMD_GET_NETWORK_IP";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}

				std::string address;
				std::string netmask;
				std::string gateway;
				bool ret = inquireNetworkAddress(address, netmask, gateway);

				std::string message = "";
				message += address;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
			}
			break;

			case DF_CMD_GET_NETWORK_NETMASK:
			{
				LOG(INFO) << "DF_CMD_GET_NETWORK_NETMASK";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}

				std::string address;
				std::string netmask;
				std::string gateway;
				bool ret = inquireNetworkAddress(address, netmask, gateway);

				std::string message = "";
				message += netmask;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
			}
			break;

			case DF_CMD_GET_NETWORK_GATEWAY:
			{
				LOG(INFO) << "DF_CMD_GET_NETWORK_GATEWAY";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}

				std::string address;
				std::string netmask;
				std::string gateway;
				bool ret = inquireNetworkAddress(address, netmask, gateway);

				std::string message = "";
				message += gateway;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
			}
			break;

			case DF_CMD_GET_NETWORK_DNS:
			{
				LOG(INFO) << "DF_CMD_GET_NETWORK_DNS";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}
		
				readNetworkAddress(now_address, now_netmask, now_gateway, now_dns);

				std::string message = now_dns;
				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
			}
			break;

			case DF_CMD_SET_NETWORK_DHCP:
			{
				LOG(INFO) << "DF_CMD_SET_NETWORK_DHCP";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}

				bool flag_ret = setNetworkDhcp();
				if (!flag_ret)
				{
					std::cout << "dhcp failure!" << std::endl;
					char err_buff[MAX_BUF_LEN] = "set auto error";
//					send_data_to_client(err_buff, strlen(err_buff));
					continue;
				}
/*
				std::string address;
				std::string netmask;
				std::string gateway;
				bool ret = inquireNetworkAddress(address, netmask, gateway);

				std::string message = "";
				message += address;
				message += ";";
				message += netmask;
				message += ";";
				message += gateway;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
*/
			}
			break;

			case DF_CMD_SET_NETWORK_IP:
			{
				LOG(INFO) << "DF_CMD_SET_NETWORK_IP";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}
	
				std::string set_address = param_list[2];
				std::string set_netmask = "";
				std::string set_gateway = "";
				std::string set_dns = "";
				bool ok = setNetworkStatic(set_address, set_netmask, set_gateway);

				if(!ok)
				{
					char err_buff[MAX_BUF_LEN] = "set ip error";
//					send_data_to_client(err_buff, strlen(err_buff));
					continue;
				}

				std::string address;
				std::string netmask;
				std::string gateway;
				bool ret = inquireNetworkAddress(address, netmask, gateway);
/*
				std::string message = "";
				message += address;
				message += ";";
				message += netmask;
				message += ";";
				message += gateway;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				LOG(INFO) << "inquireNetwork: " << message_buff;
				send_data_to_client(message_buff, strlen(message_buff));
*/
			}
			break;

			case DF_CMD_SET_NETWORK_NETMASK:
			{
				LOG(INFO) << "DF_CMD_SET_NETWORK_NETMASK";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}
	
				std::string set_address = "";
				std::string set_netmask = param_list[2];
				std::string set_gateway = "";
				std::string set_dns = "";
				bool ok = setNetworkStatic(set_address, set_netmask, set_gateway);

				if(!ok)
				{
					char err_buff[MAX_BUF_LEN] = "set netmask error";
	//				send_data_to_client(err_buff, strlen(err_buff)); 
					continue;
				}
/*
				std::string message = ""; 
				message += set_address;
				message += ";";
				message += set_netmask;
				message += ";";
				message += set_gateway;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
*/
			}
			break;

			case DF_CMD_SET_NETWORK_GATEWAY:
			{
				LOG(INFO) << "DF_CMD_SET_NETWORK_GATEWAY";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}
	
				std::string set_address = "";
				std::string set_netmask = "";
				std::string set_gateway = param_list[2];
				std::string set_dns = "";
				bool ok = setNetworkStatic(set_address, set_netmask, set_gateway);

				if(!ok)
				{
					char err_buff[MAX_BUF_LEN] = "set gateway error";
	//				send_data_to_client(err_buff, strlen(err_buff));
					continue;
				}
/*
				std::string message = "";
				message += set_address;
				message += ";";
				message += set_netmask;
				message += ";";
				message += set_gateway;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
*/
			}
			break;

			case DF_CMD_SET_NETWORK_DNS:
			{
				LOG(INFO) << "DF_CMD_SET_NETWORK_DNS";

				if (mac_addr_ != param_list[1])
				{
					char err_buff[MAX_BUF_LEN] = "mac error"; 
					continue;
				}
	
				std::string set_address = "";
				std::string set_netmask = "";
				std::string set_gateway = "";
				std::string set_dns = param_list[2];
				bool ok = setNetworkAddress(set_address, set_netmask, set_gateway, set_dns);

				if(!ok)
				{
					char err_buff[MAX_BUF_LEN] = "set dns error";
					send_data_to_client(err_buff, strlen(err_buff));
					continue;
				
				}

				std::string message = "";
				message += set_address;
				message += ";";
				message += set_netmask;
				message += ";";
				message += set_gateway;
				message += ";";
				message += set_dns;
				message += ";";

				char *message_buff = const_cast<char *>(message.c_str());
				send_data_to_client(message_buff, strlen(message_buff));
			}
			break;

			default:
				break;
		}
 
	}

	return 0;
}

int recv_client_data(char *buffer)
{
	int err = 0;
	socklen_t nAddrLen = sizeof(struct sockaddr); 

    LOG(INFO)<<"wait data...";

	// 接收数据
	int nSendSize = recvfrom(connect_socket, buffer, MAX_BUF_LEN, 0, (struct sockaddr *)&sin_from, &nAddrLen);
	if (-1 == nSendSize)
	{
		err = geterror(); 
   		LOG(INFO) << "recvfrom error: error code is " << err;
		return -1;
	}
	buffer[nSendSize] = '\0';
	printf("recv: %s \n", buffer);
  
	return 0;
}

int bind_client_port()
{

	int err;
	// 创建socket
	// int connect_socket;
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == connect_socket)
	{
		err = geterror();
		std::cout << "socket error: error code is " << err;
		return -1;
	}

	// 用来绑定套接字
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);
	sin.sin_addr.s_addr = 0;

	// 用来从网络上的广播地址接收数据
	// struct sockaddr_in sin_from;
	sin_from.sin_family = AF_INET;
	sin_from.sin_port = htons(SERVER_PORT);
	sin_from.sin_addr.s_addr = INADDR_BROADCAST;

	//设置该套接字为广播类型，
	bool bOpt = true;
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char *)&bOpt, sizeof(bOpt));

	// 绑定套接字
	int ret = bind(connect_socket, (struct sockaddr *)&sin, sizeof(sin));
	if (-1 == ret)
	{
		err = geterror();
		std::cout << "bind error: error code is " << err;

		return -1;
	}

	return 0;
}

int recv_param(char *buff)
{

	return 0;
}

int send_command(int command)
{

	int err;

	// 创建socket
	int connect_socket;
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == connect_socket)
	{

		err = geterror(); 
		std::cout << "socket error: error code is " << err;
		return -1;
	}

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(CLIENT_PORT);
	sin.sin_addr.s_addr = INADDR_BROADCAST;

	// bool bOpt = true;
	// //设置该套接字为广播类型
	// setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	int bOpt = 1;
	//设置该套接字为广播类型
	int n = setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char *)&bOpt, sizeof(bOpt));

	std::cout << "setsockopt: " << n << std::endl;

	int nAddrLen = sizeof(struct sockaddr);

	char buff[MAX_BUF_LEN] = "";
	int nLoop = 0;
 

	sprintf(buff, "%8d", command);

	// 发送数据
	int nSendSize = sendto(connect_socket, buff, strlen(buff), 0, (struct sockaddr *)&sin, nAddrLen);

	std::cout << "nSendSize: " << nSendSize << std::endl;

	if (-1 == nSendSize)
	{ 

		err = geterror(); 
		std::cout << "sendto error: error code is " << err << std::endl;
		// return -1;
	}
	printf("Send: %s \n", buff);
	//	Sleep(500);
	//}

	return 0;
}

int send_data_to_client(char *buffer, int buffer_size)
{
	int err;

	// 创建socket
	int connect_socket;
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == connect_socket)
	{
		err = geterror();
		std::cout << "socket error: error code is " << err;
		return -1;
	}

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(CLIENT_PORT);
	sin.sin_addr.s_addr = INADDR_BROADCAST;

	// bool bOpt = true;
	// //设置该套接字为广播类型
	// setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	int bOpt = 1;
	//设置该套接字为广播类型
	int n = setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char *)&bOpt, sizeof(bOpt));
 
	int nAddrLen = sizeof(struct sockaddr);

	char buff[MAX_BUF_LEN] = "";
	int nLoop = 0;
 
	// 发送数据
	int nSendSize = sendto(connect_socket, buffer, buffer_size, 0, (struct sockaddr *)&sin, nAddrLen);

	std::cout << "nSendSize: " << nSendSize << std::endl;
	std::cout << "buffer_size: " << buffer_size << std::endl;

	if (-1 == nSendSize)
	{ 
		err = geterror(); 
		std::cout << "sendto error: error code is " << err << std::endl;
 	}
	printf("Send: %s \n", buffer);
 
	return 0;
}

int restart_networking()
{
    int ret = system("sudo systemctl restart networking");
    if (0 == ret) {
        LOG(INFO) << "Restart Network Success!";
		return 0;
    } else {
        LOG(INFO) << "Restart Network Failure!";
        return -1;
    }
}