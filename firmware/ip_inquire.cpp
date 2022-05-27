#include "ip_inquire.h"
#include "string"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <fstream>
#include <string.h>
#include "stdlib.h"
#include "easylogging++.h"

volatile int g_SendFlag;
char g_SendBuff[1024];

char g_eth_name[16];
char g_macaddr[6];

int is_valid_ip(char* str)
{
    int ret = 0;
    struct in_addr inp;
    ret = inet_aton(str, &inp);
    if (0 == ret)
    {
		printf("is_valid_ip failed\n");
        return -1;
    }
    else
    {
        printf("inet_aton:ip=%lu\n",ntohl(inp.s_addr));
    }
 
    return 0;
}

int is_valid_netmask(char* netmask)
{
	if(is_valid_ip(netmask) >= 0)
    {
        unsigned int b = 0, i, n[4];
        sscanf(netmask, "%u.%u.%u.%u", &n[3], &n[2], &n[1], &n[0]);
        for(i = 0; i < 4; ++i) //将子网掩码存入32位无符号整形
        {
            b += n[i] << (i * 8);
            //  std::cout<<"n[i]: "<<n[i]<<std::endl;
        }
        b = ~b + 1;
        if((b & (b - 1)) == 0) //判断是否为2**n
            return 0;
    }
	return -1;
}
/*
	The function can printf the LOCAL MAC addr, copy the Local MAC addr to the parm str
*/
int getLocalMacAddr(char *str)
{
	int i;
    int fd;
    struct ifreq ifr;

	memset(str, 0, strlen(str));
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
        return -1;
    }

    strcpy(g_eth_name, "eth0");
    strcpy(ifr.ifr_name, g_eth_name);

    if ( ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
    {
        close( fd );
        return -1;
    }
    memcpy(g_macaddr, ifr.ifr_hwaddr.sa_data, sizeof(g_macaddr));

    printf("local mac:");
    for(i=0;i<sizeof(g_macaddr)-1;i++)
    {
        printf("%.2x:", g_macaddr[i]);
        char ch[2];
	    sprintf(ch, "%.2x", g_macaddr[i]); 
 
        strcat(str,ch);
        strcat(str,":");
 
    }
    char ch[2];
	sprintf(ch, "%.2x", g_macaddr[i]);
    printf("%s\n", ch); 
    strcat(str,ch);
 
	close(fd);
	return 0;
}
/*
	The function can printf the Broadcast addr, copy the Broadcast addr to the parm str
*/
int getBroadcastAddr(char* str)
{
	#if 1
    int fd;
    struct ifreq ifr;
    struct sockaddr_in sin;

	memset(str, 0, strlen(str));
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
		perror("socket error\n");
        return -1;
    }

    strcpy(g_eth_name, "eth0");
    strcpy(ifr.ifr_name, g_eth_name);

    //get the broadcast addr
    if ( ioctl(fd, SIOCGIFBRDADDR, &ifr) < 0)
    {
        close( fd );
        return -1;
    }
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	sprintf(str,"%s",inet_ntoa(sin.sin_addr));
    printf("broadcast is :%s\n", str);
	close(fd);
	#endif
	return 0;
}

/*
	The function can printf the ip addr, copy the ip addr to the parm str
*/
int getIpAddr(char *str, char *name)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in sin;
	
	memset(str, 0, strlen(str));
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == fd)
    {
        LOG(INFO)<<"getIpAddr, socket connect error";
        return -1;
    }

    strcpy(g_eth_name, name);
    strcpy(ifr.ifr_name, g_eth_name);
    //get the ip addr
    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
    {
        LOG(INFO)<<"getIpAddr, ioctl error";
        close( fd );
        return -1;
    }
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	sprintf(str,"%s",inet_ntoa(sin.sin_addr));
    LOG(INFO) << "getIpAddr, ip is :" << str;
	
	close(fd);
	return 0;
}
/*
	The function can printf the Gateway addr, copy the Gateway addr to the parm str
*/
int getGatewayAddr(char *str)
{ 
	FILE *fp;    
	char buf[256]; // 128 is enough for linux    
	char iface[16];    
	unsigned long dest_addr, gate_addr;    
	
    //*p = INADDR_NONE;    
	fp = fopen("/proc/net/route", "r");
	if (fp == NULL) {
        LOG(INFO) << "Open /proc/net/route fail!"; 
		return -1; 
    }
	
    /* Skip title line */    
	fgets(buf, sizeof(buf), fp);
    LOG(INFO) << "getGatewayAddr = " << buf;
	while (fgets(buf, sizeof(buf), fp) != NULL) {
        LOG(INFO) << "fgets = " << buf;

		if ((sscanf(buf, "%s\t%lX\t%lX", iface, &dest_addr, &gate_addr) != 3) || (dest_addr != 0))    
			continue;
		break;    
	}    
	
    sprintf(str, "%d.%d.%d.%d", gate_addr&0x000000ff, (gate_addr>>8)&0x000000ff, (gate_addr>>16)&0x000000ff, (gate_addr>>24)&0x000000ff);
    LOG(INFO) << "gateway is = " << str;
	
	fclose(fp);    
	return 0;
}
/*
	The function can printf the mask addr, copy the mask addr to the parm str
*/
int getMaskAddr(char *str, char *name)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in sin;
	
	memset(str, 0, strlen(str));
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == fd)
    {
		perror("socket error\n");
        return -1;
    }

    strcpy(g_eth_name, name);
    strcpy(ifr.ifr_name, g_eth_name);
	
    //get the ip addr
    if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0)
    {
		perror("ioctl error\n");
        close( fd );
        return -1;
    }
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	sprintf(str,"%s",inet_ntoa(sin.sin_addr));
    printf("Mask is :%s\n", str);
	close(fd);
	return 0;
}

/*
	The function can set the Broadcast addr, the parm str is want change's broadcast addr
*/
int setBroadcastAddr(char *str)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in sin;
	

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
		perror("socket error\n");
        return -1;
    }

    strcpy(g_eth_name, "eth0");
    strcpy(ifr.ifr_name, g_eth_name);
	
	//set the Mask addr
	struct sockaddr_in *sin_net_broadcast;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name )-1);
    sin_net_broadcast = (struct sockaddr_in *)&ifr.ifr_addr;
    sin_net_broadcast->sin_family = AF_INET;
    inet_pton(AF_INET, str, &sin_net_broadcast->sin_addr);

    if (ioctl(fd, SIOCGIFBRDADDR, &ifr ) < 0)
    {
        close( fd );
        printf("set net Broadcast error:%s\n", strerror(errno));
        return -1;
    }
	else 
		printf("set the Broadcast(%s) successfully\n", str);
	
	close(fd);
	return 0;
}

/*
	The function can set the ip addr, the parm str is want change's ip addr
*/
int setIpAddr(char *str)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in sin;
		

	
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == fd)
    {
		perror("socket error\n");
        return -1;
    }

    strcpy(g_eth_name, "eth0");
    strcpy(ifr.ifr_name, g_eth_name);
    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0)
    {
		perror(ifr.ifr_name);
        close( fd );
        return -1;
    }
	
	//set the Mash addr
	struct sockaddr_in *sin_net_ip;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name )-1);
    sin_net_ip = (struct sockaddr_in *)&ifr.ifr_addr;
    sin_net_ip->sin_family = AF_INET;
    inet_pton(AF_INET, str, &sin_net_ip->sin_addr);

    if (ioctl(fd, SIOCSIFADDR, &ifr ) < 0)
    {
        close( fd );
        printf("set net ip error:%s\n", strerror(errno));
        return -1;
    }
	else 
		printf("set the ip(%s) successfully\n", str);
	
	close(fd);
	
	return 0;
}
/*
	The function can set the gateway addr, the parm str is want change's gateway addr
*/
int setGatewayAddr(char *str)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in sin;
    struct rtentry rt;
		
	
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == fd)
    {
		perror("socket\n");
        return -1;
    }

    memset(&rt, 0, sizeof(struct rtentry));
    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_port = 0;
    if(inet_aton(str, &sin.sin_addr)<0)
    {
        perror("inet_aton error\n" );
        close(fd);
        return -1;
    }

    memcpy (&rt.rt_gateway, &sin, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
    rt.rt_flags = RTF_GATEWAY;
    if (ioctl(fd, SIOCADDRT, &rt)<0)
    {
        perror("ioctl(SIOCADDRT) error in set_default_route\n");
        close(fd);
        return -1;
    }
	else
		printf("set the gateway(%s) successfully\n", str);
	
	close(fd);
	return 0;
}
/*
	The function can set the mask addr, the parm str is want change's mask addr
*/
int setMaskAddr(char *str)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in sin;
	
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
        return -1;
    }

    strcpy(g_eth_name, "eth0");
    strcpy(ifr.ifr_name, g_eth_name);
	
	//set the Mash addr
	struct sockaddr_in *sin_net_mask;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name )-1);
    sin_net_mask = (struct sockaddr_in *)&ifr.ifr_addr;
    sin_net_mask->sin_family = AF_INET;
    inet_pton(AF_INET, str, &sin_net_mask->sin_addr);

    if (ioctl(fd, SIOCSIFNETMASK, &ifr ) < 0)
    {
        close( fd );
        printf("set net mask error:%s\n", strerror(errno));
        return -1;
    }
	else 
		printf("set the mask(%s) successfully\n", str);
	
	close(fd);
	return 0;
}

