#ifndef __HEADER_H_
#define __HEADER_H_

#include "function_head.h"
#include "arp.h"





void add_route( int des ,int gw , int netmask , char *interface )
{
	*(_U32 *)(route_info[route_item_index].destination) = des ;
	*(_U32 *)(route_info[route_item_index].gateway) = gw ;
	*(_U32 *)(route_info[route_item_index].netmask) = netmask ;
	strcpy( route_info[route_item_index].interface , interface  ) ;

	
	printf("des:  %d:%d:%d:%d\n",_getip(*((_U32 *)(route_info[route_item_index].destination)),1) ,_getip(*((_U32 *)(route_info[route_item_index].destination)),2) ,_getip(*((_U32 *)(route_info[route_item_index].destination)),3) ,_getip(*((_U32 *)(route_info[route_item_index].destination)),4) );
	printf("gw : %d:%d:%d:%d\n",_getip(*((_U32 *)(route_info[route_item_index].gateway)),1) ,_getip(*((_U32 *)(route_info[route_item_index].gateway)),2) ,_getip(*((_U32 *)(route_info[route_item_index].gateway)),3) ,_getip(*((_U32 *)(route_info[route_item_index].gateway)),4) );
	printf("netmask : %d:%d:%d:%d\n",_getip(*((_U32 *)(route_info[route_item_index].netmask)),1) ,_getip(*((_U32 *)(route_info[route_item_index].netmask)),2) ,_getip(*((_U32 *)(route_info[route_item_index].netmask)),3) ,_getip(*((_U32 *)(route_info[route_item_index].netmask)),4) );
	printf("netcard : %s \n",(route_info[route_item_index].interface));	
	route_item_index ++ ;
}

void initial_device_mac_getway()
{
	int i;
	int s = socket(AF_INET,SOCK_DGRAM,0);
	if( s<0)
	{
		fprintf(stderr,"create socket failed!errno=%d",errno);
		return ;
	}

	struct ifreq ifr;
	unsigned long nip , nnetmask,nbroadip;


	for(i=0;i<device_index;i++)
	{
		printf("netcard name : %s\n",device[i].interface );

		strcpy( ifr.ifr_name, device[i].interface );

		if( ioctl(s,SIOCGIFHWADDR,&ifr) < 0 )
		{
			perror("get mac name error\n" );
			return ;
		}
		memcpy( device[i].mac_addr ,ifr.ifr_hwaddr.sa_data, 6 );
		printf("\tMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",device[i].mac_addr[0],device[i].mac_addr[1],device[i].mac_addr[2],device[i].mac_addr[3],device[i].mac_addr[4],device[i].mac_addr[5]);

		strcpy( ifr.ifr_name,device[i].interface);
		if( ioctl(s,SIOCGIFADDR,&ifr) < 0 )
		{
			nip=0;
		}
		else
		{
			nip = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2] ;
		}
		device[i].ip = nip ;
		printf("\tip:%s\n",inet_ntoa(*(struct in_addr*)&nip));

		strcpy(ifr.ifr_name,device[i].interface);
		if( ioctl(s,SIOCGIFNETMASK,&ifr)<0)
		{
			nnetmask =0;
		}
		else
		{
			nnetmask=*(unsigned long*)&ifr.ifr_netmask.sa_data[2] ;
		}
		device[i].netmask = nnetmask ;
		printf("\tnetmask: %s\n",inet_ntoa(*(struct in_addr*)&nnetmask));

	}
}

void initial_device()
{
	FILE* f = fopen("/proc/net/dev","r");
	if(!f)
	{
		fprintf(stderr,"Open /proc/net/dev failed!errno:%d\n",errno);
		fprintf(stderr,"Open /proc/net/dev failed!errno:%d\n",errno);
		return ;
	}
	char szline[512];

	fgets(szline,sizeof(szline),f);
	fgets(szline,sizeof(szline),f);

	while(fgets(szline,sizeof(szline),f) )
	{
		char szname[128]={0};
		sscanf(szline,"%s",szname);
		int nlen = strlen(szname) ;
		if(nlen <= 0) continue ;
		if(szname[nlen-1] == ':') { szname[nlen-1]=0;}
		if(strcmp(szname,"lo") == 0 )continue ;
		if(strcmp(szname,"pan0") == 0 )continue ;
		strcpy( device[device_index].interface , szname );
		device[device_index].vpn = 0 ;
		device_index ++ ;
//		printf("name : %s\n" , szname );
	}
	fclose(f);
	initial_device_mac_getway();
	f=NULL;
	return ;
}

void initial_vpn_device()
{
	strcpy( device[device_index].interface , "vpn_eth0" ) ;
	device[device_index].netmask = inet_addr( "255.255.255.0" ) ;
	device[device_index].ip = inet_addr( "192.168.10.1" );
	device[device_index].vpn = 1 ;
//	printf("vpn ether %d \n", device_index );
//	stdshowip( device[device_index].netmask );
//	stdshowip( device[device_index].ip );
	device_index ++ ;
}



void initial_route()
{
//  初始化，琴空数据域
	device_index = 0;
	arp_item_index = 0;
	route_item_index =0;
	vpn_route_index=0;
	memset( socket_array, 0 , sizeof(socket_array)) ;
	memset( send_addr,0,sizeof(send_addr));
	memset( route_info , 0, sizeof(route_info));
	memset( arp_table,0,sizeof(arp_table));
	memset(device,0,sizeof(device));
	memset( vpn_route,0,sizeof(vpn_route));
//  设置路由表项	


	initial_device();
	initial_vpn_device() ;
/*
//  设置设备与mac
	device[0].mac_addr[0] = 0x00 ;
	device[0].mac_addr[1] = 0x0c ;
	device[0].mac_addr[2] = 0x29 ;
	device[0].mac_addr[3] = 0x61 ;
	device[0].mac_addr[4] = 0x1a ;
	device[0].mac_addr[5] = 0xda ;
	strcpy( device[0].interface , "eth5" ) ;
	device_index ++ ;	

	device[1].mac_addr[0] = 0x00 ;
	device[1].mac_addr[1] = 0x0c ;
	device[1].mac_addr[2] = 0x29 ;
	device[1].mac_addr[3] = 0x61 ;
	device[1].mac_addr[4] = 0x1a ;
	device[1].mac_addr[5] = 0xd0 ;
	strcpy( device[1].interface , "eth4" ) ;
	device_index ++ ;
//
*/

	add_vpn_route( inet_addr( "192.168.2.10" ),inet_addr( "192.168.3.0" ),inet_addr( "255.255.255.0" ) ) ;


	add_route( inet_addr( "192.168.1.0" ) , inet_addr( "192.168.1.1" ) , inet_addr("255.255.255.0") , "eth4" ) ;
	add_route( inet_addr( "192.168.2.0" ) , inet_addr( "192.168.2.10" ) , inet_addr("255.255.255.0") , "eth5" ) ;
	add_route( inet_addr( "192.168.3.0" ) , inet_addr( "192.168.3.2" ) , inet_addr("255.255.255.0") , "vpn_eth0" ) ;
//  "192.168.10.2" not use


//  设置arp缓存	
	arp_table[0].mac_addr[0] = 0x00 ;
	arp_table[0].mac_addr[1] = 0x0c ;
	arp_table[0].mac_addr[2] = 0x29 ;
	arp_table[0].mac_addr[3] = 0x6c ;
	arp_table[0].mac_addr[4] = 0x35 ;
	arp_table[0].mac_addr[5] = 0xc5 ;	
	arp_table[0].ip_addr = inet_addr( "192.168.2.10" ); 
	arp_item_index ++ ;
	arp_table[1].mac_addr[0] = 0x00 ;
	arp_table[1].mac_addr[1] = 0x0c ;
	arp_table[1].mac_addr[2] = 0x29 ;
	arp_table[1].mac_addr[3] = 0xde ;
	arp_table[1].mac_addr[4] = 0xdb ;
	arp_table[1].mac_addr[5] = 0xeb ;	
	arp_table[1].ip_addr = inet_addr( "192.168.1.1" ); 
	arp_item_index ++ ;


}



// 用来检测这个数据包是收到的还是发出去的
// 检测目的地址 是否 为自己 , 识别了广播包(ff:ff:ff:ff:ff:ff)
// 返回 1 目的地址是自己  0 不是自己
int whether_inout( _U8 *addr )    // mac
{
	printf( " in whether_inout\n");	
//	printf("device_index : %d\n", device_index ) ;
	int i=0,j,mark;
	
	for( i=0;i<device_index ; i++ )
	{
//		stdshowmac( addr );
//		stdshowmac( device[i].mac_addr ) ;

		mark=0;
		for( j=0;j<6;j++ )
			if( addr[j] != device[i].mac_addr[j] )
				mark = 1 ;
		if( !mark )
		{
			return 1 ;
		}
	}
	mark =0;
	for( j = 0 ; j<6;j++)
		if( addr[j] != 0xff )
			mark = 1 ;
	if( !mark )
	{
		return 1 ;
	}
	return 0 ;
}


int cmp_which_net( _U32 addr , int x )
{
/*	printf(" addr  netmask  dest\n");
	stdshowip( addr ) ;
	stdshowip( *(_U32 *)route_info[x].netmask ) ;
	stdshowip( *(_U32 *)route_info[x].destination ) ;
	printf( "result : %d\n",(addr & *(_U32 *)route_info[x].netmask) == *(_U32 *)route_info[x].destination ) ;
*/ 
	return (addr & *(_U32 *)route_info[x].netmask) == *(_U32 *)route_info[x].destination ; 
}


// return 第几个设备   在device  和 socket_array中的数组下标
int reroute( _U32 addr , _U32 *dst  )  // ip
{
	int i=0 , j;
	for( i=0;i<route_item_index;i++ )
	{
		if( cmp_which_net( addr , i ) )
		{
			*dst = *(_U32 *)( route_info[i].gateway ) ;
			printf(" net card: %s\n",route_info[i].interface );
			for(j=0;j< device_index ; j++ )
				if( strcmp( route_info[i].interface , device[j].interface ) == 0 )
			return j ;
		}
	}
	return -1;
}


int search_arp_cache( _U32 addr )
{
	int i=0;
	for( i=0;i< arp_item_index ;i ++ )
	{
		if( arp_table[i].ip_addr == addr )
			return i ;
	}
	return -1;
}



void showudp( char *buf )
{
	struct myethhdr *phl = (struct myethhdr	*)buf ;
	struct ip4hdr *ip = (struct ip4hdr *)( buf + sizeof(struct myethhdr )) ;
	struct udp_hdr *udp = (struct udp_hdr *)( buf + sizeof(struct myethhdr ) + sizeof(struct ip4hdr) );
	printf("\n\nshowudp function\n\n");
	printf("sender MAC:%02x:%02x:%02x:%02x:%02x:%02x ==> target MAC:%02x:%02x:%02x:%02x:%02x:%02x \n",phl->h_source[0],phl->h_source[1],phl->h_source[2],phl->h_source[3],phl->h_source[4],phl->h_source[5],phl->h_dest[0],phl->h_dest[1],phl->h_dest[2],phl->h_dest[3],phl->h_dest[4],phl->h_dest[5] );   
	printf("IP:%d,%d.%d.%d ==> %d.%d.%d.%d\n",_getip(ip->src_addr,1),_getip(ip->src_addr,2),_getip(ip->src_addr,3),_getip(ip->src_addr,4),_getip(ip->dst_addr,1),_getip(ip->dst_addr,2),_getip(ip->src_addr,3),_getip(ip->src_addr,4) );
	printf("  version : %d     head length : %d \n" , ip->version , htons(ip->head_len) );
	printf("  ser_type: %d     total length: %d \n" , ip->ser_type , htons(ip->total_len) ) ;
	printf("  id      : %d     frag_off    : %d \n" , ip->id , ip->frag_off ) ;
	printf("  time    : %d     protocal    : %d \n" , ip->time ,ip->protocal );
	printf("  csum    : %d ( %04x )   \n" , ip->csum,ip->csum); 
	printf("src port: %04x(%d) ==> dst port: %04x(%d)\n",ntohs(udp->src_port),ntohs(udp->src_port),ntohs(udp->dst_port),ntohs(udp->dst_port)); 
	printf( " udp length : %d        udp csum : %d \n" , htons(udp->len) , htons(udp->csum) ) ; 	
	printf("showudp function over \n\n");

	_U8 *b = (_U8 *)buf;
	int i ;
	int leng = htons(ip->total_len) + sizeof(struct myethhdr );
	for( i = 0 ;i<leng ;i ++ , b++ )
	{		 
		if( i %8 == 0 )
			printf("\n");
		printf( "%.2x ",  *b );
	}
}


void showtcp( char *buf )
{
	struct myethhdr *phl = (struct myethhdr	*)buf ;
	struct ip4hdr *ip = (struct ip4hdr *)( buf + sizeof(struct myethhdr )) ;
	struct tcp_hdr *tcp = (struct tcp_hdr *)( buf + sizeof(struct myethhdr ) + sizeof(struct ip4hdr) );
	printf("\n\nshowtcp function\n\n");
	printf("sender MAC:%02x:%02x:%02x:%02x:%02x:%02x ==> target MAC:%02x:%02x:%02x:%02x:%02x:%02x \n",phl->h_source[0],phl->h_source[1],phl->h_source[2],phl->h_source[3],phl->h_source[4],phl->h_source[5],phl->h_dest[0],phl->h_dest[1],phl->h_dest[2],phl->h_dest[3],phl->h_dest[4],phl->h_dest[5] );   
	printf("IP:%d,%d.%d.%d ==> %d.%d.%d.%d\n",_getip(ip->src_addr,1),_getip(ip->src_addr,2),_getip(ip->src_addr,3),_getip(ip->src_addr,4),_getip(ip->dst_addr,1),_getip(ip->dst_addr,2),_getip(ip->src_addr,3),_getip(ip->src_addr,4) );
	printf("  version : %d     head length : %d \n" , ip->version , ip->head_len*4 );
	printf("  ser_type: %d     total length: %d \n" , ip->ser_type , htons(ip->total_len) ) ;
	printf("  id      : %d     frag_off    : %d \n" , ip->id , ip->frag_off ) ;
	printf("  time    : %d     protocal    : %d \n" , ip->time ,ip->protocal );
	printf("  csum    : %d ( %04x )   \n" , ip->csum,ip->csum); 
	

	printf("  src port : %d (%04x)	dst port : %d (%04x)\n",htons(tcp->src_port),htons(tcp->src_port),htons(tcp->dst_port),htons(tcp->dst_port) ) ;
	printf(" sequence_num   : %d	ack_num : %d \n" ,tcp->sequence_num ,tcp->ack_num ); 
	printf(" head_len       : %d	unknown1: %d \n" , tcp->head_len , tcp->unknown1 );
	printf(" windows size   : %d	\n" , tcp->windows_size );
	printf(" csum           : %d ( %x) \n" ,tcp->csum ,(tcp->csum) );
	printf(" emergency point: %d \n" , tcp->emergency_point ) ;

	printf(" the data       :\n" ) ;

	printf("showtcp function over \n\n");

	_U8 *b = (_U8 *)buf;
	int i,j ;
	int leng = htons(ip->total_len) + sizeof(struct myethhdr );
	for( i = 0 ;i<leng ;i ++ , b++ )
	{		 
		if( i %8 == 0 )
			printf("\n");
		printf( "%.2x ",  *b );
	}
}


void showicmp( _U8 *buf , int len )
{
	struct myethhdr *phl = (struct myethhdr	*)buf ;
	struct ip4hdr *ip = (struct ip4hdr *)( buf + sizeof(struct myethhdr )) ;
	struct icmp_hdr *icmp = (struct icmp_hdr *)( buf + sizeof(struct myethhdr ) + sizeof(struct ip4hdr ) );
	printf("\n\nshowicmp function\n\n");
	printf("sender MAC:%02x:%02x:%02x:%02x:%02x:%02x ==> target MAC:%02x:%02x:%02x:%02x:%02x:%02x \n",phl->h_source[0],phl->h_source[1],phl->h_source[2],phl->h_source[3],phl->h_source[4],phl->h_source[5],phl->h_dest[0],phl->h_dest[1],phl->h_dest[2],phl->h_dest[3],phl->h_dest[4],phl->h_dest[5] );   
	printf("IP:%d,%d.%d.%d ==> %d.%d.%d.%d\n",_getip(ip->src_addr,1),_getip(ip->src_addr,2),_getip(ip->src_addr,3),_getip(ip->src_addr,4),_getip(ip->dst_addr,1),_getip(ip->dst_addr,2),_getip(ip->dst_addr,3),_getip(ip->dst_addr,4) );
	printf("  version : %d     head length : %d \n" , ip->version , ip->head_len*4 );
	printf("  ser_type: %d     total length: %d \n" , ip->ser_type , htons(ip->total_len) ) ;
	printf("  id      : %d     frag_off    : %d \n" , ip->id , ip->frag_off ) ;
	printf("  time    : %d     protocal    : %d \n" , ip->time ,ip->protocal );
	printf("  csum    : %d ( %04x )   \n" , ip->csum,ip->csum); 
	printf("\nicmp head\n");
	printf("Don't know how to analysis\n");
	


	
//  在这里修改一下，ip包中的time
	int leng = htons(ip->total_len) + sizeof(struct myethhdr );
//	showpacket( buf , leng ) ;

	ip->time -- ;
	ip->csum=0;
	ip->csum = in_cksum( (_U16 *)ip , ip->head_len*4 ) ;

	mysend1( buf , len ) ;
	
//	showpacket( buf , leng );

}


void showarp( char *buf , int len )
{
	struct myethhdr *phl = (struct myethhdr	*)buf ;
	struct arp_msg *arp = (struct arp_msg *)( buf + sizeof( struct myethhdr ) ) ;
	printf("sender MAC:%02x:%02x:%02x:%02x:%02x:%02x ==> target MAC:%02x:%02x:%02x:%02x:%02x:%02x \n",phl->h_source[0],phl->h_source[1],phl->h_source[2],phl->h_source[3],phl->h_source[4],phl->h_source[5],phl->h_dest[0],phl->h_dest[1],phl->h_dest[2],phl->h_dest[3],phl->h_dest[4],phl->h_dest[5] );  

	switch( ntohs(phl->h_proto) )
	{
		case eth_IP :
		printf("proto_type : IP \n"); break;
		case eth_ARP :
		printf("prote_type : ARP \n"); break;
		default : printf("other type\n");
	}

	printf("hw_type : %d   p_type:  %d\n",ntohs(arp->hw_type),ntohs(arp->protocal_type) );
	printf("hw_size : %d   protocal_size: %d\n",arp->hw_size,arp->protocal_size);
	printf("op : %s\n", (arp->op==1)?("arpsender"):("arprequest"));
	printf("src_mac:%02x:%02x:%02x:%02x:%02x:%02x  src_ip:%d,%d.%d.%d \n",arp->src_mac[0],arp->src_mac[1],arp->src_mac[2],arp->src_mac[3],arp->src_mac[4],arp->src_mac[5], _getip(arp->src_ip,1),_getip(arp->src_ip,2),_getip(arp->src_ip,3),_getip(arp->src_ip,4) );
	printf("dst_mac:%02x:%02x:%02x:%02x:%02x:%02x  src_ip:%d,%d.%d.%d \n",arp->dst_mac[0],arp->dst_mac[1],arp->dst_mac[2],arp->dst_mac[3],arp->dst_mac[4],arp->dst_mac[5], _getip(arp->dst_ip,1),_getip(arp->dst_ip,2),_getip(arp->dst_ip,3),_getip(arp->dst_ip,4) );


}




void get_proto_type( short x)
{
	switch( ntohs(x) )
	{
		printf("proto_type : IP \n"); break;
		case eth_ARP :
		printf("prote_type : ARP \n"); break;
		default : printf("other type(not ip , arp )\n");
	}
}





#endif
