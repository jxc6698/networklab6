#ifndef  __essential_header_ 
#define  __essential_header_

#include<sys/types.h>
#include<linux/if_ether.h>
//#include<linux/in.h>
#include<memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h> 
#include <netinet/udp.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/rtnetlink.h>

#include <netinet/if_ether.h>
#include <netinet/ether.h>

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <pthread.h>


#define stdshowip( x )  	printf("des:  %d:%d:%d:%d\n",_getip(x,1) ,_getip(x,2) ,_getip(x,3) ,_getip(x,4) );
#define stdshowmac( x )          printf("sender MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",(_U8)x[0],(_U8)x[1],(_U8)x[2],(_U8)x[3],(_U8)x[4],(_U8)x[5] );    



#endif
