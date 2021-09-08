#pragma once
#include <iostream>
#include "istream"
#include <fstream>
#include "string"
#include <vector>
#include <sstream>

#define IP_CONFIG_STATIC 1
#define IP_CONFIG_DHCP 2


struct IpSetting {
  std::string ip;
  std::string netmask;
  std::string gateway;
  std::string dns;
  int   flag; 
 
  };

  std::string &StringTrim(std::string &str);

  std::vector<std::string> vStringSplit(const std::string &s, const std::string &delim);

  bool readSettingsFromFile(std::string path, IpSetting &data);

  bool saveSettingsToFile(std::string path, IpSetting &data);
  