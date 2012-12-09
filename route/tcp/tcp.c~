#include "function_head.h"
#include "tcp.h"


#define TCP_LISTEN     1
#define TCP_SYN_SEND   2
#define TCP_SYN_RECV   3
#define TCP_ESTABLISH  4
#define TCP_FIN_WAIT1  5
#define TCP_FIN_WAIT2  6
#define TCP_CLOSing    7
#define TCP_CLOSE_WAIT 8
#define TCP_LAST_ACK   9
#define TCP_TIME_WAIT  10
#define TCP_CLOSED     11



#define TCP_SEQUENCE_MAX (1<<31)

struct tcp_link{
	int status ;
	_U32 mynum ;
	_U32 mynumd ;
	_U32 rmtnum ;
	_U32 rmtnumd ;
	int sport ;
	int dport ;
	_U32 sip ; 
	_U32 dip ;
	int w_size ;
} ;

struct tcp_link mylink ;

// temporary src and dst not used 
// since the function is called in listen function`
int tcptest( _U8 *buf , int len , _U32 src , _U32 dst )
{
	struct ip4hdr *ip = (struct ip4hdr *)(buf+14) ;
	char buff[1024] ;
	if( ip->protocal != IP_PROTOTCP )
	{
		return -1 ;
	}
	struct tcp_hdr *tcp = (struct tcp_hdr *)( buf + 14 + ip->head_len*4 ) ;

	switch( mylink.status )
	{
	case TCP_CLOSED :    return 0 ;
	case TCP_SYN_SEND :
						printf("TCP_SYN_SEND\n") ;

						if( tcp->syn && tcp->ack && ((_U32)htonl(tcp->ack_num)) == mylink.mynum+1  )
						{
printf("enter deeper\n");
							mylink.mynum += 1 ;
							mylink.status = TCP_ESTABLISH ;
							mylink.rmtnum = htonl(tcp->ack_num) ;
							struct tcp_hdr *tcp1 =(struct tcp_hdr *)(buff+14+20) ;
							
							tcp1->src_port = tcp->dst_port ;
							tcp1->dst_port = tcp->src_port ;
							tcp1->sequence_num = htonl(mylink.mynum) ;
							tcp1->ack_num = htonl(mylink.rmtnum+1) ;

							tcp_pack((_U8 *)tcp1 ,20,20,mylink.sport , mylink.dport,mylink.mynum , mylink.rmtnum ,TCP_ACK , 512 , 0 ,mylink.sip ,mylink.dip ) ;
							ip_pack( buff+14,20,20+20, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;
							ip_sed_process( buff+14,20+20 , mylink.sip , mylink.dip ) ;
						}
						break ;
	case TCP_SYN_RECV : 
						break ;
	case TCP_ESTABLISH :
							
							getmessage( ((_U8 *)(tcp))+tcp->head_len*4 ,len - ip->head_len*4 - tcp->head_len*4 , mylink.sip ) ;

						break ;
	case TCP_FIN :
						break ;
	case TCP_FIN_WAIT1 :
						break ;
	case TCP_FIN_WAIT2 :
						break ;
	case TCP_TIME_WAIT :
						break ;
	}

	return 0 ;
}


void tcp_initial()
{
	mylink.status = TCP_CLOSED ;
	mylink.mynum = htonl(0x80571cf0) ; // 0 ;
	mylink.rmtnum = 0 ; // 0 ; 
	mylink.mynumd = 1024 ;
	mylink.w_size = 5840 ;
	mylink.rmtnumd = 1024 ;
	mylink.sip = inet_addr("192.168.1.1") ;
	mylink.dip = inet_addr("192.168.1.2") ;
	mylink.sport = 49101  ;
	mylink.dport = 30000  ;
}


void tcp_first_step()
{
	_U8 buff[1024] ;
	struct tcp_hdr *tcp1 =(struct tcp_hdr *)(buff+14+20) ;
	buff[54] = 0x02 ;
	buff[55] = 0x04 ;
	buff[56] = 0x05 ;
	buff[57] = 0xb4 ;
	buff[58] = 0x04 ;
	buff[59] = 0x02 ;
	buff[60] = 0x08 ;
	buff[61] = 0x0a ;		
	buff[62] = 0x00 ;
	buff[63] = 0x73 ;				
	buff[64] = 0x3e ;
	buff[65] = 0x9d ;
	buff[70] = 0x01 ;
	buff[71] = 0x03 ;
	buff[72] = 0x03 ;
	buff[73] = 0x05 ;

	tcp_pack((_U8 *)tcp1 ,40,40,mylink.sport , mylink.dport,mylink.mynum , 0 ,TCP_SYN , mylink.w_size , 0 ,mylink.sip ,mylink.dip ) ;

;
	mylink.status = TCP_SYN_SEND ;
	ip_pack( buff+14,20,20+40, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;
	ip_sed_process( buff+14,20+40 , mylink.sip , mylink.dip ) ;

}

							



