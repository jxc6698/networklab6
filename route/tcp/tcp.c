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







						if( tcp->syn && tcp->ack && ((_U32)htonl(tcp->ack_num)) == mylink.mynum  )
						{
printf("enter deeper\n");

							mylink.status = TCP_ESTABLISH ;
							mylink.rmtnum = htonl(tcp->sequence_num) ;
							mylink.rmtnum += 1 ;
							struct tcp_hdr *tcp1 =(struct tcp_hdr *)(buff+14+20) ;
							
							tcp1->src_port = tcp->dst_port ;
							tcp1->dst_port = tcp->src_port ;
							tcp1->sequence_num = htonl(mylink.mynum) ;

	buff[54] = 0x01 ;
	buff[55] = 0x01 ;
	buff[56] = 0x08 ;

	buff[57] = 0x0a ;
	buff[58] = 0x00 ;
	buff[59] = 0x02 ;
	buff[60] = 0xe3 ;
	buff[61] = 0x11 ;		
	buff[62] = 0x01 ;
	buff[63] = 0x00 ;				
	buff[64] = 0x8f ;
	buff[65] = 0xff ;

							tcp1->ack_num = htonl(mylink.rmtnum+1) ;

							tcp_pack((_U8 *)tcp1 ,32,32,mylink.sport , mylink.dport,mylink.mynum , mylink.rmtnum ,TCP_ACK , 512 , 0 ,mylink.sip ,mylink.dip ) ;
							ip_pack( buff+14,20,20+32, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;
							ip_sed_process( buff+14,20+32 , mylink.sip , mylink.dip ) ;


//  debug
printf("let us hello world\n");


							mylink.rmtnum = htonl(tcp->sequence_num) ;

							
							tcp1->src_port = tcp->dst_port ;
							tcp1->dst_port = tcp->src_port ;
							tcp1->sequence_num = htonl(mylink.mynum) ;

	buff[54] = 0x01 ;
	buff[55] = 0x01 ;
	buff[56] = 0x08 ;
	buff[57] = 0x0a ;
	buff[58] = 0x00 ;
	buff[59] = 0x06 ;
	buff[60] = 0xaf ;
	buff[61] = 0x6e ;		
	buff[62] = 0x01 ;
	buff[63] = 0x04 ;				
	buff[64] = 0x5c ;
	buff[65] = 0x4e ;
	int length = strlen( "hello world\n") ;
	strncpy( buff+66 , "hello world\n" , length ) ;

							tcp1->ack_num = htonl(mylink.rmtnum+1) ;

							tcp_pack((_U8 *)tcp1 ,32+12,32,mylink.sport , mylink.dport,mylink.mynum , mylink.rmtnum ,TCP_ACK , 512 , 0 ,mylink.sip ,mylink.dip ) ;
							ip_pack( buff+14,20,20+32+12, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;

							mylink.mynum += 12 ;

							ip_sed_process( buff+14,20+32+12 , mylink.sip , mylink.dip ) ;





printf("let fin \n");

						mylink.rmtnum = htonl(tcp->sequence_num) ;

							
							tcp1->src_port = tcp->dst_port ;
							tcp1->dst_port = tcp->src_port ;
							tcp1->sequence_num = htonl(mylink.mynum) ;

	buff[54] = 0x01 ;
	buff[55] = 0x01 ;
	buff[56] = 0x08 ;
	buff[57] = 0x0a ;
	buff[58] = 0x01 ;
	buff[59] = 0x27 ;
	buff[60] = 0x32 ;
	buff[61] = 0x6f ;		
	buff[62] = 0x00 ;
	buff[63] = 0x2b ;
	buff[64] = 0xe3 ;
	buff[65] = 0x5e ;

							tcp1->ack_num = htonl(mylink.rmtnum+1) ;

							tcp_pack((_U8 *)tcp1 ,32,32,mylink.sport , mylink.dport,mylink.mynum , mylink.rmtnum ,TCP_ACK | TCP_FIN , 512 , 0 ,mylink.sip ,mylink.dip ) ;
							ip_pack( buff+14,20,20+32, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;

							mylink.mynum += 1 ;

							ip_sed_process( buff+14,20+32 , mylink.sip , mylink.dip ) ;



						}
						break ;
	case TCP_SYN_RECV : 
						break ;
	case TCP_ESTABLISH :


printf(" in estabilsh : receive num: %x , mynum : %x\n", ((_U32)htonl(tcp->ack_num)) , mylink.mynum );
if( tcp->fin && tcp->ack && ((_U32)htonl(tcp->ack_num)) == mylink.mynum )
{

printf("request fin \n");
struct tcp_hdr *tcp1 =(struct tcp_hdr *)(buff+14+20) ;
							mylink.rmtnum = htonl(tcp->sequence_num) ;

							
							tcp1->src_port = tcp->dst_port ;
							tcp1->dst_port = tcp->src_port ;
							tcp1->sequence_num = htonl(mylink.mynum) ;

	buff[54] = 0x01 ;
	buff[55] = 0x01 ;
	buff[56] = 0x08 ;
	buff[57] = 0x0a ;
	buff[58] = 0x01 ;
	buff[59] = 0x27 ;
	buff[60] = 0x32 ;
	buff[61] = 0x6f ;		
	buff[62] = 0x00 ;
	buff[63] = 0x2b ;
	buff[64] = 0xe3 ;
	buff[65] = 0x5e ;

							tcp1->ack_num = htonl(mylink.rmtnum+1) ;

							tcp_pack((_U8 *)tcp1 ,32,32,mylink.sport , mylink.dport,mylink.mynum , mylink.rmtnum ,TCP_ACK , 512 , 0 ,mylink.sip ,mylink.dip ) ;
							ip_pack( buff+14,20,20+32, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;
							ip_sed_process( buff+14,20+32 , mylink.sip , mylink.dip ) ;
}

							
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

// send syn
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
	mylink.mynum += 1 ;
	ip_pack( buff+14,20,20+40, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;
	ip_sed_process( buff+14,20+40 , mylink.sip , mylink.dip ) ;

}

#define MESSAGE_FILE_NAME  "./formessage"
					
void mtsyscal()
{

	key_t key ;
	if( (key = ftok( MESSAGE_FILE_NAME , 'a')) < 0 )
	{
		perror("create key error:");
		return ;
	}
	if((msgid=msgget(key,0666|IPC_CREAT ) ) < 0)
	{
		perror("create message error:");
		return ;
	}	

	struct msgtype msg ;
	while(1)
	{

	
	if( msgrcv( msgid , &msg ,sizeof( struct msgtype)  , 1 , 0 ) < 0)
		perror("message recv error");


	int op = ((int *)msg.buffer)[0] ;
	printf("!!!!!!! op : %d\n" , op ) ;

	switch( op )
	{
		case 1 :        //  socket 
			constructsocket();
			break ;
		case 2 :        //  connect
	myconnect( 1 );    // this paramenter is not valid
			break ;      
		case 3 :        //  send   
//	mysendtcp( _U8 *buf , int num ) ;
			break ;
		case 4 :        //  recv     // suck
						//  use  pipe

			break ;   
		case 5 :        // close

			break ;
		default : ;
	}
	}
}



void mysendtcp( _U8 *buf , int num )
{
	_U8 buff[ 600 ] ;
	struct tcp_hdr *tcp1 =(struct tcp_hdr *)(buff+14+20) ;
							
	tcp1->src_port = htons(mylink.sport) ; //  tcp->dst_port ;
	tcp1->dst_port = htons(mylink.dport) ; // tcp->src_port ;
	tcp1->sequence_num = htonl(mylink.mynum) ;

	buff[54] = 0x01 ;
	buff[55] = 0x01 ;
	buff[56] = 0x08 ;
	buff[57] = 0x0a ;
	buff[58] = 0x00 ;
	buff[59] = 0x06 ;
	buff[60] = 0xaf ;
	buff[61] = 0x6e ;		
	buff[62] = 0x01 ;
	buff[63] = 0x04 ;				
	buff[64] = 0x5c ;
	buff[65] = 0x4e ;
	int length = strlen( "hello world\n") ;
	strncpy( buff+66 , "hello world\n" , length ) ;

	tcp1->ack_num = htonl(mylink.rmtnum+1) ;

	tcp_pack((_U8 *)tcp1 ,32+12,32,mylink.sport , mylink.dport,mylink.mynum , mylink.rmtnum ,TCP_ACK , 512 , 0 ,mylink.sip ,mylink.dip ) ;
	ip_pack( buff+14,20,20+32+12, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;

	mylink.mynum += 12 ;

	ip_sed_process( buff+14,20+32+12 , mylink.sip , mylink.dip ) ;


}

int constructsocket()
{
	
	tcp_initial() ;
	return -1 ;
}

int myconnect( int sock )
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
	mylink.mynum += 1 ;
	ip_pack( buff+14,20,20+40, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;
	ip_sed_process( buff+14,20+40 , mylink.sip , mylink.dip ) ;

	return 1 ;
}

int closesocket( int socket )
{
	_U8 buff[600] ;
	struct tcp_hdr *tcp1 =(struct tcp_hdr *)(buff+14+20) ;

							
							tcp1->src_port = htons(mylink.sport) ;
							tcp1->dst_port = htons(mylink.dport) ;
							tcp1->sequence_num = htonl(mylink.mynum) ;

	buff[54] = 0x01 ;
	buff[55] = 0x01 ;
	buff[56] = 0x08 ;
	buff[57] = 0x0a ;
	buff[58] = 0x01 ;
	buff[59] = 0x27 ;
	buff[60] = 0x32 ;
	buff[61] = 0x6f ;		
	buff[62] = 0x00 ;
	buff[63] = 0x2b ;
	buff[64] = 0xe3 ;
	buff[65] = 0x5e ;

							tcp1->ack_num = htonl(mylink.rmtnum+1) ;

							tcp_pack((_U8 *)tcp1 ,32,32,mylink.sport , mylink.dport,mylink.mynum , mylink.rmtnum ,TCP_ACK | TCP_FIN , 512 , 0 ,mylink.sip ,mylink.dip ) ;
							ip_pack( buff+14,20,20+32, 0 , 0 ,64,IP_PROTOTCP , mylink.sip , mylink.dip ) ;

							mylink.mynum += 1 ;

							ip_sed_process( buff+14,20+32 , mylink.sip , mylink.dip ) ;

	return 0 ;
}


