#include "essential_data.h"


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


