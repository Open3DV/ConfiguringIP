#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <features.h>
#include <string.h>
#include "ip_config.h"
#include "ip_inquire.h"
#include "easylogging++.h"


// std::vector<std::string> vStringSplit(const std::string &s, const std::string &delim)
// {
//     std::vector<std::string> elems;
//     size_t pos = 0;
//     size_t len = s.length();
//     size_t delim_len = delim.length();
//     if (delim_len == 0)
//         return elems;
//     while (pos < len)
//     {
//         int find_pos = s.find(delim, pos);
//         if (find_pos < 0)
//         {
//             elems.push_back(s.substr(pos, len - pos));
//             break;
//         }
//         elems.push_back(s.substr(pos, find_pos - pos));
//         pos = find_pos + delim_len;
//     }
//     return elems;
// }


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


bool inquireNetworkAddress(std::string &address,std::string &netmask,std::string &gateway)
{
/*
    std::string dns= "";
    bool ret = readNetworkAddress(address,netmask,gateway,dns);

    if(!ret)
    {
    	//获取nano地址
        char ip_buff[512] = "";
        char mask_buff[512] = "";
        char gateway_buff[512] = "";
        if(-1 == getGatewayAddr(gateway_buff))
        {
            return false;
        }
        if(-1 == getMaskAddr(mask_buff))
        {
            return false;
        }
       if(-1 == getIpAddr(ip_buff))
        {
            return false;
        }

        address = std::string(ip_buff);
        netmask = std::string(mask_buff);
        gateway = std::string(gateway_buff);
        
    }
*/

    char ip_buff[512] = "";
    char mask_buff[512] = "";
    char gateway_buff[512] = "";
    if(-1 == getGatewayAddr(gateway_buff))
    {
        return false;
    }

    if(-1 == getMaskAddr(mask_buff, "eth0")) 
    {
        if (-1 == getMaskAddr(mask_buff, "eth0:avahi")) {
            return false;
        }
    }


    if(-1 == getIpAddr(ip_buff, "eth0"))
    {
        if (-1 == getIpAddr(ip_buff, "eth0:avahi")) {
            return false;
        }
    }

    address = std::string(ip_buff);
    netmask = std::string(mask_buff);
    gateway = std::string(gateway_buff);
    
    return true;
}

bool readNetworkAddress(std::string &address, std::string &netmask, std::string &gateway, std::string &dns)
{

    std::ifstream ifile;
    ifile.open("/etc/network/interfaces", std::ios::in);
    if (!ifile)
    {
    	LOG(INFO) << "readNetworkAddress: Open /etc/network/interfaces fail.";
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
        // std::cout << line << std::endl;
        str_list.push_back(line);
    }

    // std::string str;
    // ifile>>str;

    ifile.close();

    for (int i = 0; i < str_list.size(); i++)
    {
        // std::cout << str_list[i] << std::endl;

        std::vector<std::string> param_list_raw = vStringSplit(str_list[i], " ");

        std::vector<std::string> param_list;
        for(int r_i= 0;r_i< param_list_raw.size();r_i++)
        {
            if(!param_list_raw[r_i].empty())
            {
                param_list.push_back(param_list_raw[r_i]);
            }
        }

        

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

    if(old_address.empty() || old_netmask.empty() || old_gateway.empty())
    {
        std::cout<<"IP is error"<<std::endl;

        return false;

		//获取nano地址
        // char ip_buff[512] = "";
        // char mask_buff[512] = "";
        // char gateway_buff[512] = "";
        // getGatewayAddr(gateway_buff);
        // getMaskAddr(mask_buff);
        // getIpAddr(ip_buff);

        // old_address = std::string(ip_buff);
        // old_gateway = std::string(gateway_buff);
        // old_netmask = std::string(mask_buff);
        // old_dns ="114.114.114.114";
    }

    address = old_address;
    netmask = old_netmask;
    gateway = old_gateway;
    dns = old_dns;

    return true;
}


bool createDefaultNetworkSetting(std::string path,IpSetting &data)
{
    IpSetting setting_data;
    setting_data.ip = "192.168.1.100";
    setting_data.netmask = "255.255.255.0";
    setting_data.gateway = "192.168.1.1";
    setting_data.flag = IP_CONFIG_STATIC;

    data = setting_data;

    bool save_ret = saveSettingsToFile("config_ip.txt", setting_data);
    if(!save_ret)
    {
        LOG(INFO) << "Create Default Settings Error!";
        return false;    
    }

    return true;
}

bool readNetworkFromSetting(std::string &address,std::string &netmask,std::string &gateway)
{

    IpSetting setting_data;
    bool read_ret = readSettingsFromFile("config_ip.txt", setting_data);

    if(!read_ret)
    {
        bool ok = createDefaultNetworkSetting("config_ip.txt", setting_data);

        if(ok) {
            LOG(INFO) << "Create Default Settings!";
        }
    }

    LOG(INFO) << "setting_data.ip: "  << setting_data.ip;
    LOG(INFO) << "setting_data.netmask: "  << setting_data.netmask;
    LOG(INFO) << "setting_data.gateway: "  << setting_data.gateway;

    address = setting_data.ip; 
    netmask = setting_data.netmask; 
    gateway = setting_data.gateway; 

    return true;
}

/*
// -- config ip with effect
int ioctl_cfg_ip(char *interface, char *ip)
{
    int sfd, i;
    struct ifreq ifreqs[20];
    struct ifconf ic;

    ic.ifc_len = sizeof ifreqs;
    ic.ifc_req = ifreqs;

    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG(INFO) << "Socket open failed!";
        return -1;
    }

    if(ioctl(sfd, SIOCGIFCONF, &ic) < 0) {
        LOG(INFO) << "SIOCGIFCONF: get interface list failed!";
        return -1;
    }

    for(i = 0; i < ic.ifc_len/sizeof(struct ifreq); ++i) 
    {
        struct sockaddr_in *ia = (struct sockaddr_in*)&ifreqs[i].ifr_addr;
        printf("%s: %s\n", ifreqs[i].ifr_name, inet_ntoa(ia->sin_addr));
    }

    struct in_addr ipaddr;
    if(inet_aton(ip, &ipaddr) < 0) {
        LOG(INFO) << "Invalid ip address!";
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ipaddr.s_addr;

    struct ifreq ifr;
    strcpy(ifr.ifr_name, interface);
    memcpy(&ifr.ifr_addr, &addr, sizeof(struct sockaddr));

    if(ioctl(sfd, SIOCSIFADDR, &ifr) < 0) {
        LOG(INFO) << "SIOCSIFADDR: set interface address error!";
        return -1;
    }

    LOG(INFO) << "Successfully changed ip for " << interface;

    return 0;
}
*/

// -- set static network
bool setNetworkStatic(std::string &set_address,std::string &set_netmask,std::string &set_gateway)
{
    std::string old_address = "";
    std::string old_netmask = "";
    std::string old_gateway = "";
    std::string old_dns = "";

    readNetworkFromSetting(old_address,old_netmask,old_gateway);

    LOG(INFO) << "set_address: " << set_address.c_str();
    LOG(INFO) << "set_netmask: " << set_netmask.c_str();
    LOG(INFO) << "set_gateway: " << set_gateway.c_str();

    LOG(INFO) << "old_address: " << old_address.c_str();
    LOG(INFO) << "old_netmask: " << old_netmask.c_str();
    LOG(INFO) << "old_gateway: " << old_gateway.c_str();

    if (set_address.empty()) {
        set_address = old_address;
    }

    if (set_netmask.empty()) {
        set_netmask = old_netmask;
    }

    if (set_gateway.empty()) {
        set_gateway = old_gateway;
    }
 
    int valid_ret = is_valid_ip(const_cast<char *>(set_address.c_str()));
    if(-1 == valid_ret)
    {
        LOG(INFO) << "invalid ip!";
        return false;
    }

    valid_ret = is_valid_ip(const_cast<char *>(set_gateway.c_str()));
    if(-1 == valid_ret)
    {
        LOG(INFO) << "invalid gateway!";
        return false;
    }

    valid_ret = is_valid_netmask(const_cast<char *>(set_netmask.c_str()));
    if(-1 == valid_ret)
    {
        LOG(INFO) << "invalid netmask: ";
        return false;
    }
 
    std::ofstream ofile;
    ofile.open("/etc/network/interfaces", std::ios::out);
    if (!ofile)
    {
        LOG(INFO) << "setNetworkStatic: Open /etc/network/interfaces fail.";
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
    ofile.close();
/*
    // set running ip and make it effective.
    int ret = ioctl_cfg_ip("eth0", const_cast<char *>(set_address.c_str()));
    if (0 != ret) {
        LOG(INFO) << "ioctl_cfg_ip failed!";
        return false;
    }
*/
    system("ip addr flush dev eth0");
    system("ifdown eth0");
    system("ifup eth0");

    IpSetting setting_data;
    setting_data.ip = set_address;
    setting_data.netmask = set_netmask;
    setting_data.gateway = set_gateway;
    setting_data.flag = IP_CONFIG_STATIC; 
    bool save_ret = saveSettingsToFile("config_ip.txt", setting_data);

    return true;
}

bool setNetworkDhcp()
{
    std::ofstream ofile;
    ofile.open("/etc/network/interfaces", std::ios::out);
    if (!ofile)
    {
    	LOG(INFO) << "setNetworkDhcp: Open /etc/network/interfaces fail.";
        return false;
    }

    ofile << "# interfaces(5) file used by ifup(8) and ifdown(8)" << std::endl;
    ofile << "# Include files from /etc/network/interfaces.d:" << std::endl;
    ofile << "source-directory /etc/network/interfaces.d" << std::endl;
    ofile << std::endl;
    ofile << std::endl;
    ofile << "auto eth0" << std::endl;
    ofile << "iface eth0 inet dhcp" << std::endl;        
    ofile.close();

    system("ip addr flush dev eth0");
    system("ifdown eth0");
    system("ifup eth0");
/*
    std::string inquire_address;
    std::string inquire_netmask;
    std::string inquire_gateway;
    bool ret = inquireNetworkAddress(inquire_address, inquire_netmask, inquire_gateway);
    if(!ret)
    {
    	LOG(INFO) << "setNetworkDhcp: inquireNetworkAddress error.";
        return false;
    }

    LOG(INFO) << "inquire ip:" << inquire_address;
    LOG(INFO) << "inquire netmask:" << inquire_netmask;
    LOG(INFO) << "inquire gateway:" << inquire_gateway;

    int valid_ret = is_valid_ip(const_cast<char *>(inquire_address.c_str()));
    if(-1 == valid_ret) 
    {
        LOG(INFO) << "invalid ip!";
        return false;
    }

    valid_ret = is_valid_netmask(const_cast<char *>(inquire_netmask.c_str()));
    if(-1 == valid_ret)
    {
        LOG(INFO) << "invalid netmask!";
        return false;
    }

    valid_ret = is_valid_ip(const_cast<char *>(inquire_gateway.c_str()));
    if(-1 == valid_ret)
    {
        LOG(INFO) << "invalid gateway!";
        return false;
    }
*/
    return true;
}

bool setNetworkAddress(std::string &set_address, std::string &set_netmask, std::string &set_gateway, std::string &set_dns)
{

    std::ifstream ifile;
    ifile.open("/etc/network/interfaces", std::ios::in);
    if (!ifile)
    {
    	LOG(INFO) << "setNetworkAddress: Open /etc/network/interfaces fail.";
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


    // valid_ret = is_valid_ip(const_cast<char *>(set_dns.c_str()));

    // if(-1 == valid_ret)
    // {
    //     std::cout<<"invalid dns: "<<valid_ret<<std::endl;
    //     return false;
    // }
 
    /***************************************************************************************/

    std::ofstream ofile;
    ofile.open("/etc/network/interfaces", std::ios::out);
    if (!ofile)
    {
    	LOG(INFO) << "setNetworkAddress 2: Open /etc/network/interfaces fail.";
        return false;
    }

    ofile << "# interfaces(5) file used by ifup(8) and ifdown(8)" << std::endl;
    ofile << "# Include files from /etc/network/interfaces.d:" << std::endl;
    ofile << "source-directory /etc/network/interfaces.d" << std::endl;



    ofile << std::endl;
    ofile << std::endl;



    ofile << "auto eth0" << std::endl;
    ofile << "iface eth0 inet dhcp" << std::endl;        
    
    ofile << std::endl;
    ofile << std::endl; 

    ofile << "auto eth0" << std::endl;
    ofile << "iface eth0 inet static" << std::endl;

    ofile << "    address " << set_address << std::endl;
    ofile << "    netmask " << set_netmask << std::endl;
    ofile << "    gateway " << set_gateway << std::endl;
    ofile << "    dns-nameservers " << set_dns << std::endl;




    ofile.close();

    
    int ret = system("dhclient -r eth0");

    if (0 != ret)
    {
        std::cout << "Release Network Error!" << std::endl;
    }

    ret = system("/etc/init.d/networking restart");

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