


#include "essential_head.h"
#include "typess.h"
#include "arp.h"

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

_U32 sockfd , sock_sd ;




unsigned short in_cksum(unsigned short *addr, int len) 
{ 
        int sum=0; 
        unsigned short res=0; 
        while( len > 1)  { 
                sum += *addr++; 
                len -=2; 
        } 
        if( len == 1) { 
                *((unsigned char *)(&res))=*((unsigned char *)addr); 
                sum += res; 
        } 
        sum = (sum >>16) + (sum & 0xffff); 
        sum += (sum >>16) ; 
        res = ~sum; 
        return res; 
}

struct pesudo_udphdr { 
 unsigned int saddr, daddr; 
 unsigned char unused; 
 unsigned char protocol; 
 unsigned short udplen; 
}__attribute__((packed));;


#define NETCARD1   "eth5"  //  用于send
#define NETCARD2   "eth4"  //  用于receive

#define _getip( x , i )  ( x>>((i-1)<<3) & (0xff) )



#define MAX_ARP_SIZE  512 
#define MAX_DEVICE    10        // how many netcard
#define MAX_ROUTE_INFO  20
//   用与存储所有的socket 
int socket_array[MAX_DEVICE] ;
struct sockaddr_ll send_addr[MAX_DEVICE] ;
int route_item_index =0;
struct route_item{
	char destination[16];
	char gateway[16];
	char netmask[16];
	char interface[ 20 ] ;      // netcard name
}route_info[MAX_ROUTE_INFO];

int arp_item_index = 0 ;
struct arp_table_item{
	char ip_addr[16];      // ip
	char mac_addr[18];     // mac
}arp_table[MAX_ARP_SIZE];

int device_index = 0 ;
struct device_item{
	char interface[14];    // netcard name
	_U8 mac_addr[18];     // mac
	_U32 ip ;
	_U32 netmask ;
}device[MAX_DEVICE] ;


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
	unsigned char mac[6];
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
		strcpy( device[device_index].interface , szname );
		device_index ++ ;
//		printf("name : %s\n" , szname );
	}
	fclose(f);
	initial_device_mac_getway();
	f=NULL;
	return ;
}


void initial_route()
{
	device_index = 0;
	arp_item_index = 0;
	route_item_index =0;
//  设置路由表项	


	initial_device();
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


	add_route( inet_addr( "192.168.1.0" ) , inet_addr( "192.168.1.2" ) , inet_addr("255.255.255.0") , "eth4" ) ;
	add_route( inet_addr( "192.168.2.0" ) , inet_addr( "192.168.2.2" ) , inet_addr("255.255.255.0") , "eth5" ) ;
//  设置arp缓存	
	arp_table[0].mac_addr[0] = 0x00 ;
	arp_table[0].mac_addr[1] = 0x0c ;
	arp_table[0].mac_addr[2] = 0x29 ;
	arp_table[0].mac_addr[3] = 0x75 ;
	arp_table[0].mac_addr[4] = 0x72 ;
	arp_table[0].mac_addr[5] = 0xea ;	
	*(_U32 *)arp_table[0].ip_addr = inet_addr( "192.168.2.2" ); 
	arp_item_index ++ ;
	arp_table[1].mac_addr[0] = 0x00 ;
	arp_table[1].mac_addr[1] = 0x0c ;
	arp_table[1].mac_addr[2] = 0x29 ;
	arp_table[1].mac_addr[3] = 0x61 ;
	arp_table[1].mac_addr[4] = 0x1a ;
	arp_table[1].mac_addr[5] = 0xda ;	
	*(_U32 *)arp_table[1].ip_addr = inet_addr( "192.168.1.2" ); 
	arp_item_index ++ ;


}

#define stdshowip( x )  	printf("des:  %d:%d:%d:%d\n",_getip(x,1) ,_getip(x,2) ,_getip(x,3) ,_getip(x,4) );
#define stdshowmac( x )          printf("sender MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",(_U8)x[0],(_U8)x[1],(_U8)x[2],(_U8)x[3],(_U8)x[4],(_U8)x[5] );    

// 用来检测这个数据包是收到的还是发出去的
int whether_inout( char *addr )    // mac
{
	printf( " in whether_inout\n");	
//	printf("device_index : %d\n", device_index ) ;
	int i=0;
	
	for( i=0;i<device_index ; i++ )
	{
		stdshowmac( addr );
		stdshowmac( device[i].mac_addr ) ;

		int j,mark=0;
		for( j=0;j<6;j++ )
			if( addr[j] != device[i].mac_addr[j] )
				mark = 1 ;
		if( !mark )
		{
			return 1 ;
		}

	}
	return 0 ;
}


int cmp_which_net( int addr , int x )
{
	printf(" addr  netmask  dest\n");
	stdshowip( addr ) ;
	stdshowip( *(_U32 *)route_info[x].netmask ) ;
	stdshowip( *(_U32 *)route_info[x].destination ) ;
	printf( "result : %d\n",(addr & *(_U32 *)route_info[x].netmask) == *(_U32 *)route_info[x].destination ) ; 
	return (addr & *(_U32 *)route_info[x].netmask) == *(_U32 *)route_info[x].destination ; 
}


// return  the 第几个设备   在device  和 socket_array中的数组下标
int reroute( int addr )  // ip
{
	int i=0 , j;
	for( i=0;i<route_item_index;i++ )
	{
		if( cmp_which_net( addr , i ) )
		{
			printf(" net card: %s\n",route_info[i].interface );
			for(j=0;j< device_index ; j++ )
				if( strcmp( route_info[i].interface , device[j].interface ) == 0 )
			return j ;
		}
	}
	return -1;
}


int search_arp_cache( int addr )
{
	int i=0;
	for( i=0;i< arp_item_index ;i ++ )
	{
		if( *(_U32 *)arp_table[i].ip_addr == addr )
			return i ;
	}
	return -1;
}






void showpacket( char *buf , int len)
{
	_U8 *b = (_U8 *)buf;
	int i,j ;
	for( i = 0 ;i<len ;i ++ , b++ )
	{		 
		if( i %8 == 0 )
			printf("\n");
		printf( "%.2x ",  *b );
	}
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
	int i,j ;
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

	printf("hw_type : %d   p_type:  %d\n",ntohs(arp->hw_type),ntohs(arp->protocol_type) );
	printf("hw_size : %d   protocol_size: %d\n",arp->hw_size,arp->protocol_size);
	printf("op : %s\n", (arp->op==1)?("arpsender"):("arprequest"));
	printf("src_mac:%02x:%02x:%02x:%02x:%02x:%02x  src_ip:%d,%d.%d.%d \n",arp->src_mac[0],arp->src_mac[1],arp->src_mac[2],arp->src_mac[3],arp->src_mac[4],arp->src_mac[5], _getip(arp->src_ip,1),_getip(arp->src_ip,2),_getip(arp->src_ip,3),_getip(arp->src_ip,4) );
	printf("dst_mac:%02x:%02x:%02x:%02x:%02x:%02x  src_ip:%d,%d.%d.%d \n",arp->dst_mac[0],arp->dst_mac[1],arp->dst_mac[2],arp->dst_mac[3],arp->dst_mac[4],arp->dst_mac[5], _getip(arp->dst_ip,1),_getip(arp->dst_ip,2),_getip(arp->dst_ip,3),_getip(arp->dst_ip,4) );


}



void arp_request( int addr , char *MAC , int IP  )   //  
{
	char buf[ 61 ];     // 一般搜不用整10 防止使用了strcpy什么的覆盖越界
	struct myethhdr *phl = (struct myethhdr	*)buf ;
	struct arp_msg *arp = (struct arp_msg *)( buf + sizeof( struct myethhdr ) ) ;
	memset( phl->h_dest , 0xff , 6);
	memcpy( phl->h_source , MAC  , 6 );

	phl->h_proto = htons( eth_ARP);

	arp->hw_type = htons( 1 ) ;    // 表示以太网
	arp->protocol_type = htons( eth_ARP ) ;

	arp->hw_size = 6 ;
	arp->protocol_size = 4 ;
	arp->op= htons( ARP_REQUEST );
	memcpy( arp->src_mac , MAC , 6 ) ;
	arp->src_ip = IP ;
	memset( arp->dst_mac , 0 , arp->protocol_size );
	arp->dst_ip = addr ;


}

void arp_reply( char *buff , int len , char *MAC , int IP )
{
	struct myethhdr *phl = (struct myethhdr	*)buff ;
	struct arp_msg *arp = (struct arp_msg *)( buff + sizeof( struct myethhdr ) ) ;
	
	char send_buf[61] ;
	struct myethhdr *phl1 = (struct myethhdr	*)send_buf ;
	struct arp_msg *arp1 = (struct arp_msg *)( send_buf + sizeof( struct myethhdr ) ) ;
	memcpy( phl1->h_source , MAC , 6 ) ;
	memcpy(phl1->h_dest , phl->h_source , 6 );

	phl1->h_proto = phl->h_proto;

	arp1->hw_type = htons( 1 );
	arp1->protocol_type = htons( eth_ARP ) ;
	arp1->hw_size = 6 ;
	arp1->protocol_size = 4 ;
	arp1->op = htons( ARP_REPLY ) ;
	memcpy( arp1->src_mac , MAC , 6 )  ;
	arp1->src_ip =  IP ;
	memcpy( arp1->dst_mac , arp->src_mac , 6) ;
	arp1->dst_ip = arp->src_ip ;
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






