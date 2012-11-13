#include "essential_data.h"
#include "function_head.h"

void *ip_handle_thread( void *data)
{
	
	return 0;
}


void *vpn_ip_handle_thread( void *data)
{
	return 0;
}


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
	if( pipe(ip_tcp) < 0 )
	{
		printf("pipe create  ip_tcp\n");
		exit(-1);
	}
}

void tcp_pack( _U8 *buf , int len , int hlen , _U16 sport ,_U16 dport,int snum , int acknum,int flag1,int flag2 , _U16 size , _U16 point , _U32 src , _U32 dst )
{
	struct tcp_hdr *tcp = (struct tcp_hdr *)( buf );
	static _U8 buff[4096] ;

	tcp->src_port = htons(sport) ;
	tcp->dst_port = htons(dport) ;
	tcp->sequence_num = snum ;
	tcp->ack_num = acknum ;
//  需要详细改 用 switch
	tcp->unknown1 = flag1 ;
	tcp->head_len = hlen/4 ;
	tcp->unknown2 = flag2 ;

	tcp->windows_size = size ;
	tcp->emergency_point = point ;
	tcp->csum = 0 ;
	memcpy( buff+12 , buf , len ) ;
	struct pesudo_hdr *pes = (struct pesudo_hdr *)( buff ) ;
//  这里进行计算的时候我觉得应该是网络序（但是没有测试）
	pes->saddr = htonl(src) ;
	pes->daddr = htonl(dst) ;
	pes->unused = 0 ;
	pes->protocal = IPPROTO_TCP ;    //
	pes->len = ntohs(len) ;
	tcp->csum = in_cksum( (_U16*)buff , len+12 ) ;
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
	ip->src_addr = src ;
	ip->dst_addr = dst ;
	ip->csum = 0 ;
	ip->csum = in_cksum( (_U16*)buf , len ) ;
}


//   addr has changed in function repack_vpn
int repack_vpn( _U32 *addr_src , _U32 *addr , _U8 *buf , int len )
{
	_U8 buff[4096] ;
	_U32 src , dst ;
	src = *addr_src ;
	dst = *addr ;
	memcpy( buff+20 , buf , len ) ;
	struct ip4hdr *ip = (struct ip4hdr *)( buff ) ;
//	struct tcp_hdr *tcp =(struct tcp_hdr *)( buff + sizeof(struct ip4hdr) );
	if( check_vpn_route( *addr , &dst ) )
	{
		// no route item
		printf("vpn route not match in repackvpn\n");
		return -1 ;
	}
		
	ip_pack( buff , 20 , len+20 , 1 , 0 , 60 ,  IPPROTO_RAW , src , dst ) ;

	memcpy( buf , buff ,len+20 );

	return 1 ;
}

int unpack_vpn( _U8 *buf , _U32 len , _U32 *src , _U32 *dst )
{
	struct ip4hdr* ip = (struct ip4hdr*)buf ;
	*src = ip->src_addr ;
	*dst = ip->dst_addr ;
	if( whether_ip_me( ip->dst_addr ) > 0 )
	{
		getmessage( buf+ip->head_len*4 , len-ip->head_len*4 , ip->src_addr  ) ;
		return 2 ;
	}
	memcpy( buf, buf+ip->head_len*4 , len -ip->head_len*4 );
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

//  先要判断 ip 是不是发给自己的
//  不是直接转发
	tome = whether_ip_me( ip1->dst_addr ) ;
	// 不是发给我的 ，判断是否需要vpn转发
	if( tome < 0 )
	{
//     不是vpn直接转发
		if( device[tome].vpn == 0 )
		{
			dst = ip1->dst_addr ;
			src = ip1->src_addr ;
			offset = ip1->head_len *4 ;
			route( buf , len ) ;
		}
		else // 是vpn ,要先包换再转发
		{
			;
		}
		return ;
	}
//  是的话继续判断 是否有vpn


//  判断vpn是不是发给自己的
//  
//  解包 找出目的地址

//  在解包中应当包含路由 , 且在vpn_dst 中应当包含下一跳地址
	if( ip1->protocal == IP_PROTOPPTP )
	{		
		mark = unpack_vpn( buf+offset , len- offset , &vpn_src , &vpn_dst ) ;
		if( mark < 0 )
		{
			printf(" error 1 : in function ip_rec_process \n");
			return ;
		}
		else if( mark == 1 )
		{
//   填 0 是因为这里应该放自己的 ip
			
			ip_sed_process( buf+offset , len-offset , 0 , vpn_dst ) ;
			return ;
		}
		else if( mark == 2 )
		{
			getmessage( buf , len , src ) ;
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
void ip_sed_process( _U8 *buf , int len , _U32 src , _U32 dst )
{
	static _U8 buff[4096] ;
	int up_vpn = 0 , dst1 = dst ;
	memcpy( buff + sizeof(struct myethhdr) + sizeof(struct ip4hdr) , buf , len );
	
//  如果要用到vpn，dst 返回不确定的值 否则返回吓一跳ip地址
	int index = reroute( dst1 , &dst );
	
	printf("index netcard : %d  in ip_sed_process\n" , index );
	if( index < 0 )
		return ;
	src = device[index].ip ;
//  如果要使用vpn ，那么需要重包装一下
	if( device[index].vpn > 0 )
	{
		
		repack_vpn( &src , &dst1 , buff + sizeof(struct myethhdr) + sizeof(struct ip4hdr) , len ) ;
		up_vpn = 1;
//  因为在repack函数中 dst1已经被改变
		index = reroute( dst1 , &dst );
		len += 20 ;
	}

//  判断一下是什么协议
	if( up_vpn == 1 )
		ip_pack( buff+14, 20 , len+20 , 0 ,0,60,IP_PROTOPPTP, src ,dst );	
	else
		ip_pack( buff+14, 20 , len+20 , 0 ,0,60,IP_PROTORAW, src ,dst );

	struct myethhdr* eth = ( struct myethhdr* )( buff ) ;
	eth->h_proto = htons( 0x0800 ) ;

	printf("index : %d \n", index);
	int netcard = search_arp_cache( dst ) ;
	printf("netcard: %d \n", netcard);
	memcpy( buf , arp_table[netcard].mac_addr , 6 );
	memcpy( buf+6 , device[index].mac_addr , 6 ) ;
 
	len += 14 ;

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
	for(i=0; i<vpn_route_index ; i++ ) 
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
	printf("from  ");
	stdshowip( src );
	printf("length : %d \n%s\n" ,len , buf );
}

