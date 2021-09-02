#include "iostream"
#include "istream"
#include <fstream>
#include "string"
#include <vector>
#include <sstream>

bool readSerialNumber(int& number);

bool readNetworkAddress(std::string &address,std::string &netmask,std::string &gateway,std::string &dns);

bool setNetworkAddress(std::string &set_address,std::string &set_netmask,std::string &set_gateway,std::string &set_dns);

std::vector<std::string> vStringSplit(const std::string &s, const std::string &delim = " ");

 