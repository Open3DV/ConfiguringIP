#include "ip_config.h"
#include "ip_inquire.h"

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


bool readSerialNumber(int& number)
{

    std::ifstream ifile;
    ifile.open("/sys/firmware/devicetree/base/serial-number", std::ios::in);
    if (!ifile)
    {
        std::cerr << "Open File Fail." << std::endl;
        return -1;
    }

    std::vector<std::string> str_list;
    std::string line;

    while (getline(ifile, line)) // line中不包括每行的换行符
    {
        // std::cout << line << std::endl;
        str_list.push_back(line);
    }

    if(1 != str_list.size())
    {
        return false;
    }

	std::stringstream ss;
	ss << str_list[0];
	ss >> number;

    // std::string str;
    // ifile>>str;

    ifile.close();

    return true;
}

bool readNetworkAddress(std::string &address, std::string &netmask, std::string &gateway, std::string &dns)
{

    std::ifstream ifile;
    ifile.open("/etc/network/interfaces", std::ios::in);
    if (!ifile)
    {
        std::cerr << "Open File Fail." << std::endl;
        return -1;
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
        // std::cout << line << std::endl;
        str_list.push_back(line);
    }

    // std::string str;
    // ifile>>str;

    ifile.close();

    for (int i = 0; i < str_list.size(); i++)
    {
        // std::cout << str_list[i] << std::endl;

        std::vector<std::string> param_list = vStringSplit(str_list[i], " ");

        if (2 == param_list.size())
        {

            if ("address" == param_list[0])
            {
                old_address = param_list[1];
            }
            else if ("netmask" == param_list[0])
            {
                old_netmask = param_list[1];
            }
            else if ("gateway" == param_list[0])
            {
                old_gateway = param_list[1];
            }
            else if ("dns-nameservers" == param_list[0])
            {
                old_dns = param_list[1];
            }
        }
    }

    if(old_address.empty() && old_netmask.empty()&& old_gateway.empty()&& old_dns.empty())
    {
        std::cout<<"IP is null"<<std::endl;

		//获取nano地址
        char ip_buff[512] = "";
        char mask_buff[512] = "";
        char gateway_buff[512] = "";
        getGatewayAddr(gateway_buff);
        getMaskAddr(mask_buff);
        getIpAddr(ip_buff);

        old_address = std::string(ip_buff);
        old_gateway = std::string(gateway_buff);
        old_netmask = std::string(mask_buff);
        old_dns ="114.114.114.114";
    }


    address = old_address;
    netmask = old_netmask;
    gateway = old_gateway;
    dns = old_dns;

    return true;
}

bool setNetworkAddress(std::string &set_address, std::string &set_netmask, std::string &set_gateway, std::string &set_dns)
{

    std::ifstream ifile;
    ifile.open("/etc/network/interfaces", std::ios::in);
    if (!ifile)
    {
        std::cerr << "Open File Fail." << std::endl;
        return false;
    }

    std::vector<std::string> str_list;
    std::string line;

    std::string old_address = "";
    std::string old_netmask = "";
    std::string old_gateway = "";
    std::string old_dns = "";

    
    readNetworkAddress(old_address,old_netmask,old_gateway,old_dns);

    /****************************************************************************************************/

    // while (getline(ifile, line)) // line中不包括每行的换行符
    // {
    //     // std::cout << line << std::endl;
    //     str_list.push_back(line);
    // }

    // // std::string str;
    // // ifile>>str;

    // ifile.close();

    // for (int i = 0; i < str_list.size(); i++)
    // {
    //     // std::cout << str_list[i] << std::endl;

    //     std::vector<std::string> param_list = vStringSplit(str_list[i], " ");

    //     if (2 == param_list.size())
    //     {

    //         if ("address" == param_list[0])
    //         {
    //             old_address = param_list[1];
    //         }
    //         else if ("netmask" == param_list[0])
    //         {
    //             old_netmask = param_list[1];
    //         }
    //         else if ("gateway" == param_list[0])
    //         {
    //             old_gateway = param_list[1];
    //         }
    //         else if ("dns-nameservers" == param_list[0])
    //         {
    //             old_dns = param_list[1];
    //         }
    //     }
    // }

    /*****************************************************************************************************/

    std::cout << "set_address: " << set_address << std::endl;
    std::cout << "set_netmask: " << set_netmask << std::endl;
    std::cout << "set_gateway: " << set_gateway << std::endl;
    std::cout << "set_dns: " << set_dns << std::endl;

    std::cout << "old_address: " << old_address << std::endl;
    std::cout << "old_netmask: " << old_netmask << std::endl;
    std::cout << "old_gateway: " << old_gateway << std::endl;
    std::cout << "old_dns: " << old_dns << std::endl;

    if (set_address.empty())
    {
        set_address = old_address;
    }

    if (set_netmask.empty())
    {
        set_netmask = old_netmask;
    }

    if (set_gateway.empty())
    {
        set_gateway = old_gateway;
    }

    if (set_dns.empty())
    {
        set_dns = old_dns;
    }

    /************************************************************************************/

    int valid_ret = is_valid_ip(const_cast<char *>(set_address.c_str()));

    if(-1 == valid_ret)
    {
        std::cout<<"invalid ip!"<<std::endl;
        return false;
    }

    valid_ret = is_valid_ip(const_cast<char *>(set_gateway.c_str()));

    if(-1 == valid_ret)
    {
        std::cout<<"invalid gateway!"<<std::endl;
        return false;
    }

    valid_ret = is_valid_netmask(const_cast<char *>(set_netmask.c_str()));

    if(-1 == valid_ret)
    {
        std::cout<<"invalid netmask: "<<std::endl;
        return false;
    }


    valid_ret = is_valid_ip(const_cast<char *>(set_dns.c_str()));

    if(-1 == valid_ret)
    {
        std::cout<<"invalid dns: "<<valid_ret<<std::endl;
        return false;
    }
 
    /***************************************************************************************/

    std::ofstream ofile;
    ofile.open("/etc/network/interfaces", std::ios::out);
    if (!ofile)
    {
        std::cerr << "Open File Fail." << std::endl;
        return false;
    }

    ofile << "# interfaces(5) file used by ifup(8) and ifdown(8)" << std::endl;
    ofile << "# Include files from /etc/network/interfaces.d:" << std::endl;
    ofile << "source-directory /etc/network/interfaces.d" << std::endl;

    ofile << std::endl;
    ofile << std::endl;

    ofile << "auto eth0" << std::endl;
    ofile << "iface eth0 inet static" << std::endl;

    ofile << "address " << set_address << std::endl;
    ofile << "netmask " << set_netmask << std::endl;
    ofile << "gateway " << set_gateway << std::endl;
    ofile << "dns-nameservers " << set_dns << std::endl;

    ofile.close();

    int ret = system("/etc/init.d/networking restart");

    if (0 == ret)
    {
        std::cout << "Set Network Success!" << std::endl;
    }
    else
    {
        std::cout << "Set Network Failure!" << std::endl;

        set_address = old_address;
        set_netmask = old_netmask;
        set_gateway = old_gateway;
        set_dns = old_dns;
    }

    return true;
}