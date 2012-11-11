#include "essential_data.h"

int socket_array[MAX_DEVICE] ;
struct sockaddr_ll send_addr[MAX_DEVICE] ;
int route_item_index =0;
struct route_item route_info[MAX_ROUTE_INFO];

int arp_item_index = 0 ;
struct arp_table_item arp_table[MAX_ARP_SIZE];

int device_index = 0 ;
struct device_item device[MAX_DEVICE] ;


int vpn_route_index;
struct vpn_route_table vpn_route[MAX_ROUTE_INFO];


int ether_ip[2];
int ip_tcp[2];

