#pragma once
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>

struct rtc_time
 {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};


int is_valid_ip(char* str);


int is_valid_netmask(char* netmask);

/*
	The function can printf the LOCAL MAC addr, copy the Local MAC addr to the parm str
*/
int getLocalMacAddr(char *str);

/*
	The function can printf the Broadcast addr, copy the Broadcast addr to the parm str
*/
int getBroadcastAddr(char* str);

/*
	The function can printf the ip addr, copy the ip addr to the parm str
*/
int getIpAddr(char *str, char *name);

/*
	The function can printf the Gateway addr, copy the Gateway addr to the parm str
*/
int getGatewayAddr(char *str);

/*
	The function can printf the mask addr, copy the mask addr to the parm str
*/
int getMaskAddr(char *str, char *name);

/*
	The function can set the Broadcast addr, the parm str is want change's broadcast addr
*/
int setBroadcastAddr(char *str);

/*
	The function can set the ip addr, the parm str is want change's ip addr
*/
int setIpAddr(char *str);

/*
	The function can set the gateway addr, the parm str is want change's gateway addr
*/
int setGatewayAddr(char *str);

/*
	The function can set the mask addr, the parm str is want change's mask addr
*/
int setMaskAddr(char *str);


