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

	tcp->src_port = sport ;
	tcp->dst_port = dport ;
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
	pes->saddr = src ;
	pes->daddr = dst ;
	pes->unused = 0 ;
	pes->protocal =               1  ;    //
	pes->len = len ;
	tcp->csum = in_cksum( (_U16*)buff , len+12 ) ;
}

void ip_pack( _U8 *buf , int len , int total_len , int id , int flag , int time , int protocal , int src , int dst )
{
	struct ip4hdr *ip = (struct ip4hdr *)( buf ) ;
	ip->head_len = len /4 ;
	ip->version = 4 ;
	ip->ser_type = 0 ;  //
	ip->total_len = total_len ;
	ip->id = id ;
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
		
	ip_pack( buff , 20 , len+20 , 1 , 0 , 60 ,  IP_PROTOPPTP , src , dst ) ;

	memcpy( buf , buff ,len+20 );

	return 1 ;
}

int unpack_vpn( _U8 *buf , _U32 len , _U32 *src , _U32 *dst )
{
	struct ip4hdr* ip = (struct ip4hdr*)buf ;
	*src = ip->src_addr ;
	*dst = ip->dst_addr ;
	memcpy( buf , buf+ ip->head_len*4 , len - ip->head_len*4 ) ;
	return 1 ;
}





int check_vpn_route( _U32 addr , _U32 *des )
{

struct vpn_route_table{
	int des;
	int vpn_des ;
	int netmask ;	
};
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




void ip_process( _U8 *buf , int len)
{
	struct ip4hdr *ip1 = ( struct ip4hdr *)( buf ) ;
	if( ip1->protocal == IP_PROTOPPTP )
	{
		;
	}
}

