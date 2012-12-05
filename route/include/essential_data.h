#ifndef __essential_data_
#define __essential_data_


#include "essential_head.h"
#include "mylock.h"
#include "message.h"

extern int socket_array[MAX_DEVICE] ;
extern struct sockaddr_ll send_addr[MAX_DEVICE] ;
extern int route_item_index ;
extern struct route_item route_info[MAX_ROUTE_INFO];

extern int arp_item_index ;
extern struct arp_table_item arp_table[MAX_ARP_SIZE];

extern int device_index ;
extern struct device_item device[MAX_DEVICE] ;

extern int vpn_route_index;
extern struct vpn_route_table vpn_route[MAX_ROUTE_INFO];


extern int ether_ip[2];
extern int protopipe[2];

extern _U8 arp_wait_buf[10][4096] ;
extern _U32 arp_wait_ip[10] ; 
extern int arp_wait_len[10] ;
extern int arp_valid[10] ;


extern struct mylock iolock[10] ;





#endif
