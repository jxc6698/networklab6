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

#include "typess.h"

#ifndef ETHALEN
#define ETH_ALEN 6
#endif



struct myethhdr{
	unsigned char h_dest[ETH_ALEN];
	unsigned char h_source[ETH_ALEN];
	unsigned short h_proto;
}__attribute__((packed));
#define eth_IP  0x0800
#define eth_X25 0x8035
#define eth_IPX 0x8137
#define eth_PPP 0x0863 // 0x0864
#define eth_ARP 0x0806   //




// ip   20 bytes
struct ip4hdr
{	
	_U32   head_len    : 4 ;
	_U32   version : 4 ;
	_U8    ser_type ;            // type 0f service
	_U16   total_len    ;
	_U16   id ;               //   identification
	_U16   frag_off ;         // 3 + 13  
	_U8    time ;
	_U8    protocal ;
	_U16   csum ;
//  
	_U32 src_addr ;
	_U32 dst_addr ;
}__attribute__((packed));


//udp
struct udp_hdr{
	unsigned short src_port ;
	unsigned short dst_port ;
	unsigned short len ;
	unsigned short csum ;
}__attribute__((packed));

// tcp
struct tcp_hdr{
	unsigned short src_port ;
	unsigned short dst_port ;
	unsigned  sequence_num ;
	unsigned  ack_num ;
	unsigned  unknown1 : 4 ;
	unsigned  head_len : 4 ;   // tcp_head 的大小 in 32bits  5-15
	unsigned  unknown2 : 8 ;
	unsigned short windows_size ;
	unsigned short csum ;
	unsigned short emergency_point;
//	unsigned char optino[12] ;
}__attribute__((packed));

// arp 
// op  0x0001 arp  0x0002 arp reply
struct arp_msg{
	unsigned short hw_type ;         //  网络序
	unsigned short protocol_type ;   //  网络序
	unsigned char hw_size ;      //   ETH_ALEN = 6
	unsigned char protocol_size ;   // ipv4  is 4
	unsigned short op ;
	unsigned char  src_mac[6];
	unsigned  src_ip ;
	unsigned char  dst_mac[6];
	unsigned  dst_ip ;
	unsigned char  filled[18] ;   // 
}__attribute__((packed));
#define ARP_ETHERNET  1
 
#define PROTO_ARP 0x0800
#define ARP_REQUEST   1
#define ARP_REPLY  2
// SHA  sender hardware address 
// SPA  sender protocol address


// icmp
struct icmp_hdr
{
	_U8 type;
	_U8 code;
	_U16 checksum;
	union
	{
	struct
	{
		_U16 id;
		_U16 sequence;
	} echo;
	_U32 gateway;
	struct
	{
			_U16 unused;
			_U16 mtu;
		} frag;
	} un;
};


//igmp
struct igmp_hdr
{
	_U8 type ;
	_U8 code ;
	_U16 csum ;
	_U32 group ;
};

struct pesudo_hdr { 
 unsigned int saddr, daddr; 
 unsigned char unused; 
 unsigned char protocol; 
 unsigned short len; 
}__attribute__((packed));;




#define MAX_ARP_SIZE  512 
#define MAX_DEVICE    10        // how many netcard
#define MAX_ROUTE_INFO  20
struct route_item{
	char destination[16];
	char gateway[16];
	char netmask[16];
	char interface[ 20 ] ;      // netcard name
	int  vpn ;                  // 1 to vpn server
};

struct arp_table_item{
	char ip_addr[16];      // ip
	char mac_addr[18];     // mac
};

struct device_item{
	char interface[14];    // netcard name
	_U8 mac_addr[18];     // mac
	_U32 ip ;
	_U32 netmask ;
} ;

struct vpn_route_table{
	int des;
	int vpn_des ;
	int netmask ;	
};





//  与使用相关的宏


#define NETCARD1   "eth5"  //  用于send
#define NETCARD2   "eth4"  //  用于receive

#define stdshowip( x )  	printf("des:  %d:%d:%d:%d\n",_getip(x,1) ,_getip(x,2) ,_getip(x,3) ,_getip(x,4) );
#define stdshowmac( x )          printf("sender MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",(_U8)x[0],(_U8)x[1],(_U8)x[2],(_U8)x[3],(_U8)x[4],(_U8)x[5] );    

#define stdshowip( x )  	printf("des:  %d:%d:%d:%d\n",_getip(x,1) ,_getip(x,2) ,_getip(x,3) ,_getip(x,4) );
#define stdshowmac( x )          printf("sender MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",(_U8)x[0],(_U8)x[1],(_U8)x[2],(_U8)x[3],(_U8)x[4],(_U8)x[5] );    
#define _getip( x , i )  ( x>>((i-1)<<3) & (0xff) )


#endif
