#include "essential_data.h"
#include "function_head.h"


void *tcp_handle_thread(void *data)
{
	return 0;
}


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

void initial_pthread_pipe()
{
	if( pipe(ether_ip) < 0 )
	{
		printf("pipe create  ether_ip\n");
		exit(-1);
	}
	if( pipe(protopipe) < 0 )
	{
		printf("pipe create  ip_tcp\n");
		exit(-1);
	}
}

void tcp_pack( _U8 *buf , int len , int hlen , _U16 sport ,_U16 dport,int snum , int acknum,_U32 flag , _U16 size , _U16 point , _U32 src , _U32 dst )
{
	struct tcp_hdr *tcp = (struct tcp_hdr *)( buf );
	memset( buf, 0 , hlen ) ;
	static _U8 buff[4096] ;

	tcp->src_port = htons(sport) ;
	tcp->dst_port = htons(dport) ;
	tcp->sequence_num = htonl(snum) ;
	tcp->ack_num = htonl(acknum) ;
//  需要详细改 用 switch
//	tcp->unknown1 = (flag&0x0f00)>>8 ;
	tcp->head_len = hlen/4 ;
//	tcp->unknown2 = flag&0x00ff ;
	if( flag & TCP_FIN )
		tcp->fin = 1 ;
	if( flag & TCP_SYN )
		tcp->syn = 1 ;
	if( flag & TCP_RST )
		tcp->rst = 1 ;
	if( flag & TCP_PSH )
		tcp->psh = 1 ;
	if( flag & TCP_ACK )
		tcp->ack = 1 ;
	if( flag & TCP_URG )
		tcp->urg = 1 ;
	if( flag & TCP_ECN )
		tcp->ecn = 1 ;
	if( flag & TCP_CWR )
		tcp->cwr = 1 ;
	if( flag & TCP_NON )
		tcp->non = 1 ;

	tcp->windows_size = htons(size) ;
	tcp->emergency_point = point ;
	tcp->csum = 0 ;
	memcpy( buff+12 , buf , len ) ;
	struct pesudo_hdr *pes = (struct pesudo_hdr *)( buff ) ;
//  这里进行计算的时候我觉得应该是网络序（但是没有测试）
	pes->saddr = src ; 
	pes->daddr = dst ; 
	pes->unused = 0 ;
	pes->protocal = IPPROTO_TCP ;    //
	pes->len = ntohs(len) ;
	tcp->csum = in_cksum( (_U16*)buff , len+12 ) ;
}

void tcp_unpack( _U8 *buf , int len , int *hlen , _U16 *sport ,_U16 *dport,int *snum , int *acknum,_U32 *flag , _U16 *size , _U16 *point , _U32 src , _U32 dst )
{
	struct tcp_hdr *tcp = (struct tcp_hdr *)( buf );
	static _U8 buff[4096] ;
	int csum = tcp->csum ;
	
	memcpy( buff+12 , buf , len ) ;

	tcp->csum = 0 ;

	struct pesudo_hdr *pes = (struct pesudo_hdr *)( buff ) ;
//  这里进行计算的时候我觉得应该是网络序（但是没有测试）
	pes->saddr = src ; // htonl(src) ;
	pes->daddr = dst ; // htonl(dst) ;
	pes->unused = 0 ;
	pes->protocal = IPPROTO_TCP ;    //
	pes->len = ntohs(len) ;
	tcp->csum = in_cksum( (_U16*)buff , len+12 ) ;

	if( tcp->csum != csum )
	{
		printf("csum error in function tcp_unpack\n") ;
	} 
/*
	tcp->src_port = htons(sport) ;
	tcp->dst_port = htons(dport) ;
	tcp->sequence_num = snum ;
	tcp->ack_num = acknum ;
//  需要详细改 用 switch
	if( tcp->fin )
		flag |= TCP_FIN ;
	if( tcp->syn )
		flag |= TCP_SYN ;
	if( tcp->rst )
		flag |= TCP_RST ;
	if( tcp->psh )
		flag |= TCP_PSH ;
	if( tcp->ack )
		flag |= TCP_ACK ;
	if( tcp->urg )
		flag |= TCP_URG ;
	if( tcp->ecn )
		flag |= TCP_ECN ;
	if( tcp->cwr )
		flag |= TCP_CWR ;
	if( tcp->non )
		flag |= TCP_NON ;
	tcp->unknown1 = (flag&0x0f00)>>8 ;
	tcp->head_len = hlen/4 ;
	tcp->unknown2 = flag&0x00ff ;

	tcp->windows_size = size ;
	tcp->emergency_point = point ;
*/

}



void ip_pack( _U8 *buf , int len , int total_len , int id , int flag , int time , int protocal , int src , int dst )
{
	struct ip4hdr *ip = (struct ip4hdr *)( buf ) ;
	ip->head_len = len /4 ;
	ip->version = 4 ;
	ip->ser_type = 0 ;  //
	ip->total_len = ntohs(total_len) ;
	ip->id = ntohs(id) ;
	ip->frag_off = flag ;
	ip->protocal = protocal ;
	ip->time = 64 ;
	ip->src_addr = src ;
	ip->dst_addr = dst ;
	ip->csum = 0 ;
	ip->csum = in_cksum( (_U16*)buf , len ) ;
}


//   addr has changed in function repack_vpn
int repack_vpn( _U32 *addr_src , _U32 *addr , _U8 *buf , int len )
{
	_U32 src , dst ;
	src = *addr_src ;
	dst = *addr ;
	struct ip4hdr *ip = (struct ip4hdr *)( buf ) ;
//	struct tcp_hdr *tcp =(struct tcp_hdr *)( buff + sizeof(struct ip4hdr) );


	if( check_vpn_route( *addr , &dst ) < 0 )
	{
		// no route item
//		stdshowip( dst ) ;
		printf("vpn route not match in repack_vpn\n");
		return -1 ;
	}
	int index ;
	getnetcard( dst , &index ) ;
//	printf("net index : %d   in repack_vpn\n",index);
	src = device[index].ip ;
	ip_pack( buf , 20 , len+20 , 1 , 0 , 60 ,  IP_PROTOPPTP , src , dst ) ;
	*addr_src = src ;
	*addr = dst ;

	return 1 ;
}


//  这里面应该有 vpn路由 的功能 ， 并且这部分也是第二层协议
//  返回 2 表示 是发给我的数据
//  返回 1 表示 需要继续路由
//  吓一跳地址放在了 dst 中返回
int unpack_vpn( _U8 *buf , _U32 *len , _U32 *src , _U32 *dst )
{
	struct ip4hdr* ip = (struct ip4hdr*)buf ;
	*src = ip->src_addr ;
	*dst = ip->dst_addr ;
	if( whether_ip_me( ip->dst_addr ) > 0 )
	{
		memcpy( buf , buf + ip->head_len*4 , *len -ip->head_len*4 ) ;
		*len = *len -ip->head_len*4 ; 
		return 2 ;
	}
	return 1 ;
}





int check_vpn_route( _U32 addr , _U32 *des )
{
	int i;
	for( i=0;i<vpn_route_index ;i++ )	
	{
		if( (addr&vpn_route[i].netmask) == (vpn_route[i].vpn_des & vpn_route[i].netmask) )
		{
			*des = vpn_route[i].des ;
			return 1 ;
		}
	}
	return -1 ;
}

int add_vpn_route( _U32 des , _U32 vpn_des , _U32 netmask)
{
	vpn_route[vpn_route_index].des = des ;
	vpn_route[vpn_route_index].vpn_des = vpn_des ;
	vpn_route[vpn_route_index].netmask = netmask ;
	vpn_route_index ++ ;
}




void ip_rec_process( _U8 *buf , int len)
{

	int i, tome=0 , offset=0 ;
	int mark;
	_U32 src , dst ;
	_U32 vpn_src , vpn_dst ; 
	struct ip4hdr *ip1 = ( struct ip4hdr *)( buf ) ;
	
//	printf("in ip_rec_process\n\n\n");
//	stdshowip( ip1->dst_addr );

//  先要判断 ip 是不是发给自己的
//  不是直接转发
	tome = whether_ip_me( ip1->dst_addr ) ;
	// 不是发给我的 ，判断是否需要vpn转发
	if( tome < 0 )
	{
//     不是vpn直接转发
		int index = reroute( ip1->dst_addr , &dst );
		printf("index : %d\n\n",index );
		if( device[index].vpn == 0 )
		{
			ip_sed_process(buf , len , 0 , dst ) ;
		}
		else // 是vpn ,要先包 zhuang 再转发
		{
//			check_vpn_route( ip1->dst_addr , &dst ) ;
//			stdshowip(dst );

			ip_sed_process(buf , len , 0 , ip1->dst_addr ) ;
		}		
		return ;
	}
//  是的话继续判断 是否有vpn


//  判断vpn是不是发给自己的
//  
//  解包 找出目的地址

//  在解包中应当包含路由 , 且在vpn_dst 中应当包含下一跳地址
	offset += (ip1->head_len*4) ;
	if( ip1->protocal == IP_PROTOPPTP )
	{		
		len -= offset ;
		mark = unpack_vpn( buf+offset , &len , &vpn_src , &vpn_dst ) ;
printf("mark %d \n", mark) ;
		
		if( mark < 0 )
		{
			printf(" error 1 : in function ip_rec_process \n");
			return ;
		}
		else if( mark == 1 )
		{
//   填 0 是因为这里应该放自己的 ip
			
			ip_sed_process( buf+offset , len , 0 , vpn_dst ) ;
			return ;
		}
		else if( mark == 2 )
		{
			getmessage( buf+offset , len , src ) ;
			return ;
		}
	}
	else    // 是发给我的，并且没有vpn服务
	{
		getmessage( buf+offset , len- offset  , src ) ;
		return ;
	}
//  路由

}


//  本来这里还应该支持传如协议
//  在这里只需要传递上一层的目标地址,是否要添加vpn在ip_sed_process里面决定
void ip_sed_process( _U8 *buf , int len , _U32 src , _U32 dst )
{

	static _U8 buff[4096] ;
	int up_vpn = 0 , dst1 = dst ;
	
	
//  如果要用到vpn，dst 返回不确定的值 否则返回吓一跳ip地址
	int index = reroute( dst1 , &dst );
	
	printf("index netcard : %d  in ip_sed_process\n" , index );
	if( index < 0 )
		return ;
//	src = device[index].ip ;
	
//  如果要使用vpn ，那么需要重包装一下
	if( device[index].vpn > 0 )
	{
		printf("starr repack in ip_sed_process\n");
//		check_vpn_route( dst1 , &dst ) ;
//		stdshowip( dst1 );
		memcpy( buff + sizeof(struct myethhdr) + sizeof(struct ip4hdr) , buf , len );
showpacket( buff , len +20+14 );
printf("\n");
		repack_vpn( &src , &dst1 , buff + sizeof(struct myethhdr) , len ) ;
showpacket( buff , len +20+14 );
printf("\n");
		up_vpn = 1;
//  因为在repack函数中 dst1已经被改变
		len += 20 ;
printf("len : %d \n" , len );
	}
	else
	{
		memcpy( buff + sizeof(struct myethhdr) , buf , len );
	}

	struct myethhdr* eth = ( struct myethhdr* )( buff ) ;
	eth->h_proto = htons( 0x0800 ) ;

	getnetcard( dst1 , &index ) ;
	printf("index : %d \n", index);
	int netcard = search_arp_cache( dst1 ) ;
	
	if( netcard < 0 )
	{
		printf("not arp \n");
		add_to_arp_wait_buffer( buff , len+14 , dst1 ) ;
		return ;
	}

	printf("netcard: %d \n", netcard);
	memcpy( buff , arp_table[netcard].mac_addr , 6 );
	memcpy( buff+6 , device[index].mac_addr , 6 ) ;
 
	len += 14 ;
	showpacket( buff , len ) ;

	sendto( socket_array[index] , buff , len , 0 , (struct sockaddr *)&send_addr[index] , sizeof( send_addr[index] ) );

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

int whether_ip_me( _U32 addr )
{
	int i;
	for(i=0; i<device_index ; i++ ) 
	{

		if( cmp_ip_net_mask(addr , device[i].ip , device[i].netmask  ) )
		{
			return 1 ;
		}
	}
	return -1 ;
}



void getmessage( _U8 *buf , int len , _U32 src )
{
	buf[ len ] = 0 ;
	printf("in the getmessage functino\n");
//	printf("from  ");
//	stdshowip( src );
//	printf("length : %d \n%s\n" ,len , buf );
}


int selnetcard( _U32 addr )
{
	int i;
	for(i=0; i< device_index  ; i++ )
	{
		if( cmp_ip_net_mask( addr , device[i].ip , device[i].netmask ) )
			return i ;
	}
	return -1 ;
}


int getnetcard( _U32 addr , _U32 *num )
{
	int i;
	for(i=0; i< device_index  ; i++ )
	{
		if( cmp_ip_net_mask( addr , device[i].ip , device[i].netmask ) )
		{
			*num = i ;
			return 1 ;
		}
	}
	return -1 ;
}


int add_to_arp_wait_buffer( _U8 *buf , int len , _U32 ip  )
{
	int i ;
	for(i=0;i<10 ;i++ )
	{
		if( arp_valid[i] )
			continue ;
		memcpy( arp_wait_buf[i] , buf , len );
		arp_wait_len[i] = len ;
		arp_wait_ip[i] = ip ;
		arp_valid[i] = 1 ;
		return 1 ;
	}
	return -1 ;
}

int check_arp_wait_buffer( _U8 *buf , int *len , _U32 addr )
{
	int i,j;
	for( i=0;i<10 ; i++ )
	{
		if( !arp_valid[i] )
			continue ;
		i = ( i+1 )% 10 ;
		if( arp_wait_ip[i] == addr )
		{
			memcpy( buf , arp_wait_buf[i] , arp_wait_len[i] ) ;
			*len = arp_wait_len[i] ;
			arp_valid[i] = 0 ;
			return 1 ;
		}
	}
	return -1 ;
}

int handle_arp( _U8 *buf , int len )
{
	struct arp_msg *arp = ( struct arp_msg *)( buf + 14  ) ;
	_U8 buff[4096] ;
	int length ;
	int index ;
	if( htons(arp->op) == ARP_REQUEST )
	{
		;//send
	}
	else if( htons(arp->op ) == ARP_REPLY )
	{
		struct myethhdr *phl = (struct myethhdr	*)buf ;
		struct arp_msg *arp = (struct arp_msg *)( buf + sizeof( struct myethhdr ) ) ;
		add_arp_item( arp->src_ip , arp->src_mac ) ;
		while( check_arp_wait_buffer( buff , &length , arp->src_ip ) > 0 )
		{
			if( getnetcard(  arp->src_ip , &index )  > 0 )
			{
				memcpy( buff+6  , device[index].mac_addr , 6 ) ;
				memcpy( buff  , arp->src_mac , 6 ) ;
				sendto( socket_array[index] , buff , len , 0 , (struct sockaddr *)&send_addr[index] , sizeof( send_addr[index] ) );
			}
		}
	}
}

void add_arp_item( _U32 addr , _U8 mac[] )
{
	arp_table[ arp_item_index ].ip_addr = addr ;
	memcpy( arp_table[arp_item_index].mac_addr , mac , 6 ) ;
	arp_item_index ++ ;
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
	arp->protocal_type = htons( eth_ARP ) ;

	arp->hw_size = 6 ;
	arp->protocal_size = 4 ;
	arp->op= htons( ARP_REQUEST );
	memcpy( arp->src_mac , MAC , 6 ) ;
	arp->src_ip = IP ;
	memset( arp->dst_mac , 0 , arp->protocal_size );
	arp->dst_ip = addr ;
	
	int index ;
	getnetcard( addr , &index ) ;
	
	sendto( socket_array[index] , buf , 60 , 0 , (struct sockaddr *)&send_addr[index] , sizeof( send_addr[index] ) ) ;

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
	arp1->protocal_type = htons( eth_ARP ) ;
	arp1->hw_size = 6 ;
	arp1->protocal_size = 4 ;
	arp1->op = htons( ARP_REPLY ) ;
	memcpy( arp1->src_mac , MAC , 6 )  ;
	arp1->src_ip =  IP ;
	memcpy( arp1->dst_mac , arp->src_mac , 6) ;
	arp1->dst_ip = arp->src_ip ;
}



void readpipe( _U32 *op , void *argv , _U8 *buf , int x )  // x = protopipe[0]
{
	lock( x ) ; 
	int thepipe ;
	read( x , (char *)(&op) , 4 ) ;
	unlock( x ) ;
}

void writepipe( _U32 op , void *argv , _U8 *buf , int x )  // x = protopipe[1]
{
	lock( x ) ;
	int thepipe ;
	write( thepipe , (char *)(&op) , 4 ) ;

	lock( x ) ;
}


int lock( int x )
{
	return -1 ;
}


int unlock( int x )
{
	return 0;
}
