#ifndef  __TCP_H_
#define  __TCP_H_


#include "essential_data.h"
#include "function_head.h"


     

int tcptest( _U8 *buf, int len , _U32 src , _U32 dst ) ;
void tcp_inital() ;
void tcp_first_step() ;

//extern struct tcp_link link ;


#endif
