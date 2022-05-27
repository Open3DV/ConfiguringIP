#pragma once
#include <WINSOCK2.H>
#include <iostream>
#include <sstream> 
#include "../configuring_ip/getopt.h" 
#include "../configuring_ip/protocol.h"
#include <vector>
#pragma comment(lib,"ws2_32.lib")


int send_command(int command);
int send_server_data(const char* buff);
//int send_param(const char* buff);
int bind_server_port(int nNetTimeout = 1 * 1000);
int set_recv_timeout(int nNetTimeout = 1 * 1000);
int set_send_timeout(int nNetTimeout = 1 * 1000);
int recv_server_data(char* buffer, int& buffer_size);
//int recv_server_command(int& command);

std::vector<std::string> vStringSplit(const std::string& s, const std::string& delim);

int GetCameraList(std::vector<std::string>& mac_list, std::vector<std::string>& ip_list);

int SetCameraIp(std::string mac, std::string& ip);

int SetCameraIpAuto(std::string mac, std::string& ip);

int SetCameraNetwork(long cmd, std::string mac, std::string& cfg, long sleep);