#include "iostream"
#pragma once
#include "istream"
#include <fstream>
#include "string"
#include <vector>
#include <sstream>
#include "ip_setting.h"


bool readSerialNumber(int& number);

bool readNetworkAddress(std::string &address,std::string &netmask,std::string &gateway,std::string &dns);

bool setNetworkAddress(std::string &set_address,std::string &set_netmask,std::string &set_gateway,std::string &set_dns);

bool setNetworkDhcp();

bool setNetworkStatic(std::string &set_address,std::string &set_netmask,std::string &set_gateway);

bool readNetworkFromSetting(std::string &address,std::string &netmask,std::string &gateway);

bool createDefaultNetworkSetting(std::string path,IpSetting &data);

bool inquireNetworkAddress(std::string &address,std::string &netmask,std::string &gateway);
// std::vector<std::string> vStringSplit(const std::string &s, const std::string &delim = " ");

 