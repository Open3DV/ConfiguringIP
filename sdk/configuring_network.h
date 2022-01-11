#pragma once
#include <WINSOCK2.H>
#include <iostream>
#include <sstream> 
#include "../configuring_ip/getopt.h" 
#include "../configuring_ip/protocol.h"
#include <vector>
#pragma comment(lib,"ws2_32.lib")

int GetCameraList(std::vector<std::string>& mac_list, std::vector<std::string>& ip_list);

int SetCameraIp(std::string mac, std::string& ip);

int SetCameraIpAuto(std::string mac, std::string& ip);