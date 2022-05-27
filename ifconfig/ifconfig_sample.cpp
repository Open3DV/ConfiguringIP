#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <features.h>
#include <string.h>

/*****************************************************************************/
/* Copyright: (c) 2011, Vasudeva Kamath <kamathvasudev@gmail.com>            */
/* License: ISC <https://secure.wikimedia.org/wikipedia/en/wiki/ISC_License> */
/*****************************************************************************/

int main(int argc, char *argv[])
{
    int sfd,i;
    struct ifreq ifreqs[20];
    struct ifconf ic;
    char interface[10];
    char ip[20];

    ic.ifc_len = sizeof ifreqs;
    ic.ifc_req = ifreqs;

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket");
        abort();
    }

    if(ioctl(sfd,SIOCGIFCONF,&ic) < 0){
        perror("SIOCGIFCONF");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < ic.ifc_len/sizeof(struct ifreq); ++i){
        struct sockaddr_in *ia = (struct sockaddr_in*)&ifreqs[i].ifr_addr;
        printf("%s: %s\n", ifreqs[i].ifr_name,
                inet_ntoa(ia->sin_addr));
    }

    printf("Do you want to change IP address of interface? ");
    char s = getchar();
    if(s == 'y'){
        printf("Please enter the interface name: ");
        scanf("%s",interface);

        printf ("Enter IP address in dotted decimal format: ");
        scanf("%s",ip);

        struct in_addr ipaddr;
        if(inet_aton(ip,&ipaddr) < 0){
            printf ("Guess you have entered invalid ip address\n");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in addr;
        memset(&addr,0,sizeof(struct sockaddr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = ipaddr.s_addr;

        struct ifreq ifr;
        strcpy(ifr.ifr_name,interface);
        memcpy(&ifr.ifr_addr,&addr,sizeof(struct sockaddr));
        if(ioctl(sfd,SIOCSIFADDR,&ifr) < 0){
            perror("SIOCSIFADDR");
            abort();
        }
        
        printf("Successfully changed ip for %s\n",interface);
        
        
    }
    
    return 0;
}
