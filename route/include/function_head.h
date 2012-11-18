#ifndef __function_extern_
#define __function_extern_

#include "essential_data.h"

extern void *ip_handle_thread( void *data) ;
extern void *vpn_ip_handle_thread( void *data) ;
unsigned short in_cksum(unsigned short *addr, int len) ;
void tcp_pack( _U8 *buf , int len , int hlen , _U16 sport ,_U16 dport,int snum , int acknum,_U32 flag , _U16 size , _U16 point , _U32 src , _U32 dst ) ;

void tcp_unpack( _U8 *buf , int len , int *hlen , _U16 *sport ,_U16 *dport,int *snum , int *acknum,_U32 *flag , _U16 *size , _U16 *point , _U32 src , _U32 dst ) ;


void ip_pack( _U8 *buf , int len , int total_len , int id , int flag , int time , int protocal , int src , int dst );

int repack_vpn( _U32 *addr_src , _U32 *addr , _U8 *buf , int len ) ;
int uppack_vpn(_U8 *buf , _U32 *len , _U32 *src, _U32 *dst );



void ip_sed_process( _U8 *buf , int len , _U32 src ,_U32 dst ) ;
void ip_rec_process( _U8 *buf , int len) ;



void getmessage( _U8 *buf , int len , _U32 src );

// 判断在 此ip是不是自己的
int whether_ip_me( _U32 addr );
int getnetcard( _U32 addr , _U32 *num ) ; 

//  注意在内部有 我的pn包的时要设置protocal_type  




//  ip route table
int cmp_which_net( _U32 addr , int x ) ;


//  vpn route table
int check_vpn_route( _U32 addr , _U32 *des ) ;
int add_vpn_route( _U32 des , _U32 vpn_des , _U32 netmask) ;
//



//  show 
void showpacket( char *buf , int len ) ;


// 获得 addr  对应的地址的网卡 
// 错误返回 -1 
int selnetcard( _U32 addr ) ;

void initial_pthread_pipe() ;


// arp缓存

int check_arp_wait_buffer( _U8 *buf , int *len , _U32 ip ) ;
int add_to_arp_wait_buffer( _U8 *buf ,int len ,_U32 addr ) ;

int handle_arp( _U8 *buf, int len ) ;
void arp_reply( char *buff , int len , char *MAC , int IP );
void arp_request( int addr , char *MAC , int IP  );
void add_arp_item( _U32 addr , _U8 mac[] ) ;


#endif
