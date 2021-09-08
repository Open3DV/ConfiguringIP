#include "ip_setting.h"
#include <iostream>
#include <string>

std::string& StringTrim(std::string &str)
{
    if (str.empty()){
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

std::vector<std::string> vStringSplit(const std::string &s, const std::string &delim)
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

  bool readSettingsFromFile(std::string path,IpSetting &data)
  {

    std::ifstream ifile;
    ifile.open(path, std::ios::in);
    if (!ifile)
    {
        std::cerr << "Open Settings File Fail." << std::endl;
        return false;
    }

    std::vector<std::string> str_list;
    std::string line;

    // std::string set_address = "192.168.88.138";
    // std::string set_netmask = "";
    // std::string set_gateway = "";
    // std::string set_dns = "";

    std::string old_address = "";
    std::string old_netmask = "";
    std::string old_gateway = "";
    std::string old_dns = "";

    while (getline(ifile, line)) // line中不包括每行的换行符
    {
        std::cout << line << std::endl;
        str_list.push_back(line);
    }
 

    ifile.close();

    for (int i = 0; i < str_list.size(); i++)
    {
        // std::cout << str_list[i] << std::endl;

        std::vector<std::string> param_list = vStringSplit(str_list[i], ":");

        if (2 == param_list.size())
        { 

            param_list[1] = StringTrim(param_list[1]);

            if ("ip" == param_list[0])
            {
              data.ip = param_list[1];
            }
            else if ("netmask" == param_list[0])
            { 
              data.netmask = param_list[1];
            }
            else if ("gateway" == param_list[0])
            { 
              data.gateway = param_list[1];
            }
            else if ("dns" == param_list[0])
            { 
              data.dns = param_list[1];
            }
        }
    }


    return true;

  }


  bool saveSettingsToFile(std::string path,IpSetting &data)
  {

    std::ofstream ofile;
    ofile.open(path, std::ios::out);
    if (!ofile)
    {
        std::cerr << "Open File Fail." << std::endl;
        return false;
    }
  
    ofile << "ip: " << data.ip << std::endl;
    ofile << "netmask: " << data.netmask << std::endl;
    ofile << "gateway: " << data.gateway << std::endl;
    ofile << "dns: " << data.dns << std::endl;
    ofile << "flag: " << data.flag << std::endl;
  
    ofile.close();

    return true;
  }