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
	pes->protocol =               1  ;    //
	pes->len = len ;
	tcp->csum = in_cksum( (_U16*)buff , len+12 ) ;
}

void ip_pack( _U8 *buf , int len , int total_len , int id , int flag , int time , int protocao , int src , int dst )
{
	struct ip4hdr *ip = (struct ip4hdr *)( buf ) ;
	ip->head_len = len /4 ;
	ip->version =            1   ;    //
	ip->ser_type =            1   ;  //
	ip->total_len = total_len ;
	ip->id = id ;
	ip->frag_off = flag ;
	ip->src_addr = src ;
	ip->dst_addr = dst ;
	ip->csum = 0 ;
	ip->csum = in_cksum( (_U16*)buf , len ) ;
}


void repack_vpn( int addr , _U8 *buf , int len )
{
	_U8 buff[4096] ;
	
	memcpy( buff+40 , buf , len ) ;
	struct ip4hdr *ip = (struct ip4hdr *)( buff ) ;
	struct tcp_hdr *tcp = (struct tcp_hdr *)( buff + sizeof(struct ip4hdr) );
	
}
