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

extern int errno;
int geterror() { return errno; }

const int MAX_BUF_LEN = 255;

int send_command(int command);

int send_data_to_client(char *buffer, int buffer_size);

int bind_client_port();

int recv_client_data(char *buffer);

// 创建socket
int connect_socket;
// 用来从网络上的广播地址接收数据
struct sockaddr_in sin_from;

int main(int argc, char *argv[])
{

	// std::string set_address = "192.168.88.137";
	// std::string set_netmask = "";
	// std::string set_gateway = "";
	// std::string set_dns = "";

	// std::cout << "Hello IP" << std::endl;

	// int serial_number = 0;

	// bool ok = readSerialNumber(serial_number);

	// if (!ok)
	// {
	// 	std::cout << "read serial number error!" << std::endl;
	// }

	// std::cout << "read serial number: " << serial_number << std::endl;

	int ret = 0; 
	char mac_buff[512] = ""; 
	ret = getLocalMacAddr(mac_buff); 

	std::string mac_addr_(mac_buff); 
	std::cout<<"mac: "<<mac_addr_<<std::endl;

	/*********************************************************************************/
	//绑定客户端端口
	ret = bind_client_port();

	if (0 != ret)
	{
		std::cout << "bind error!" << std::endl;
		return -1;
	}

 
	char buff[MAX_BUF_LEN] = "";
	
	while (1)
	{
  
		ret = recv_client_data(buff); 
		// printf("data_str buff: %s \n", buff);

		if (ret != 0)
		{
			return -1;
		}

		/**************************************************************************/
		std::string data_str(buff);

		// std::cout << "data_str: " << data_str << std::endl;

		std::vector<std::string> param_list;
		param_list = vStringSplit(data_str, ";");

		/*************************************************************************/

		// std::cout << "param_list size: " << param_list.size() << std::endl;
 
		int command = 0; 
		std::stringstream ss;
		ss << param_list[0];
		ss >> command;

		// std::cout << "command: " << command << std::endl;

  
		//获取nano地址
		// char ip_buff[512] = "";
		// char mask_buff[512] = "";
		// char gateway_buff[512] = "";

		std::string now_address;
		std::string now_netmask;
		std::string now_gateway;
		std::string now_dns;

		switch (command)
		{
		case DF_CMD_GET_NETWORK_MAC:
		{

			readNetworkAddress(now_address, now_netmask, now_gateway, now_dns);

			// std::cout << "now_ip: " << now_address << std::endl;
			// std::cout << "now_netmask: " << now_netmask << std::endl;
			// std::cout << "now_gateway: " << now_gateway << std::endl;
			// std::cout << "now_dns: " << now_dns << std::endl;

			std::string message = "";
			
			message += std::string(mac_buff);
			message += ";";

			message += " ip: ";
			message += now_address;
			message += ";";


			char *message_buff = const_cast<char *>(message.c_str());
			send_data_to_client(message_buff, strlen(message_buff));
 
		}
		break;

		case DF_CMD_GET_NETWORK_ADDRESS:
		{

			if (mac_addr_ != param_list[1])
			{

				char err_buff[MAX_BUF_LEN] = "mac error";
				continue;
			}

			readNetworkAddress(now_address, now_netmask, now_gateway, now_dns);



			std::string message = "";
			message += now_address;
			message += ";";
			message += now_netmask;
			message += ";";
			message += now_gateway;
			message += ";";
			message += now_dns;
			message += ";";

			char *message_buff = const_cast<char *>(message.c_str());
			send_data_to_client(message_buff, strlen(message_buff));

		}
		break;
		case DF_CMD_GET_NETWORK_IP:
		{
  
			if (mac_addr_ != param_list[1])
			{

				char err_buff[MAX_BUF_LEN] = "mac error"; 
				continue;
			}

			readNetworkAddress(now_address, now_netmask, now_gateway, now_dns);

			std::string message = now_address;
			char *message_buff = const_cast<char *>(message.c_str());
			send_data_to_client(message_buff, strlen(message_buff));
		}
		break;
		case DF_CMD_GET_NETWORK_NETMASK:
		{
			if (mac_addr_ != param_list[1])
			{

				char err_buff[MAX_BUF_LEN] = "mac error"; 
				continue;
			}

			readNetworkAddress(now_address, now_netmask, now_gateway, now_dns);

			std::string message = now_netmask;
			char *message_buff = const_cast<char *>(message.c_str());
			send_data_to_client(message_buff, strlen(message_buff));
		}
		break;
		case DF_CMD_GET_NETWORK_GATEWAY:
		{
			if (mac_addr_ != param_list[1])
			{

				char err_buff[MAX_BUF_LEN] = "mac error"; 
				continue;
			}

			readNetworkAddress(now_address, now_netmask, now_gateway, now_dns);

			std::string message = now_gateway;
			char *message_buff = const_cast<char *>(message.c_str());
			send_data_to_client(message_buff, strlen(message_buff));
		}
		break;
		case DF_CMD_GET_NETWORK_DNS:
		{
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
		case DF_CMD_SET_NETWORK_IP:
		{ 

			if (mac_addr_ != param_list[1])
			{

				char err_buff[MAX_BUF_LEN] = "mac error"; 
				continue;
			}
  
			std::string set_address = param_list[2];
			std::string set_netmask = "";
			std::string set_gateway = "";
			std::string set_dns = "";

			int ok = setNetworkAddress(set_address, set_netmask, set_gateway, set_dns);

			if(!ok)
			{

				char err_buff[MAX_BUF_LEN] = "set ip error";
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
		case DF_CMD_SET_NETWORK_NETMASK:
		{ 
			if (mac_addr_ != param_list[1])
			{

				char err_buff[MAX_BUF_LEN] = "mac error"; 
				continue;
			}
 

			std::string set_address = "";
			std::string set_netmask = param_list[2];
			std::string set_gateway = "";
			std::string set_dns = "";

			bool ok = setNetworkAddress(set_address, set_netmask, set_gateway, set_dns);


			if(!ok)
			{

				char err_buff[MAX_BUF_LEN] = "set netmask error";
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
		case DF_CMD_SET_NETWORK_GATEWAY:
		{

			if (mac_addr_ != param_list[1])
			{

				char err_buff[MAX_BUF_LEN] = "mac error"; 
				continue;
			}
 

			std::string set_address = "";
			std::string set_netmask = "";
			std::string set_gateway = param_list[2];
			std::string set_dns = "";

			int ok = setNetworkAddress(set_address, set_netmask, set_gateway, set_dns);



			if(!ok)
			{

				char err_buff[MAX_BUF_LEN] = "set gateway error";
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
		case DF_CMD_SET_NETWORK_DNS:
		{
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
	// char buff[MAX_BUF_LEN] = "";

	std::cout << "wait data..." << std::endl;
	// 接收数据
	int nSendSize = recvfrom(connect_socket, buffer, MAX_BUF_LEN, 0, (struct sockaddr *)&sin_from, &nAddrLen);
	if (-1 == nSendSize)
	{
		err = geterror(); 
		std::cout << "recvfrom error: error code is " << err;
		return -1;
	}
	buffer[nSendSize] = '\0';
	// printf("recv: %s \n", buffer);
  
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

	// std::cout << "nSendSize: " << nSendSize << std::endl;
	// std::cout << "buffer_size: " << buffer_size << std::endl;

	if (-1 == nSendSize)
	{ 

		err = geterror(); 
		std::cout << "sendto error: error code is " << err << std::endl;
 
	}
	// printf("Send: %s \n", buffer);
 

	return 0;
}
