#include "essential_data.h"
#include "function_head.h"
#include "header.h"
#include "multithread.h"


void route( char *buf , int len )
{
	struct myethhdr *phl = (struct myethhdr	*)buf ;
	struct ip4hdr *ip = (struct ip4hdr *)( buf + sizeof(struct myethhdr )) ;
	struct icmp_hdr *icmp = (struct icmp_hdr *)( buf + sizeof(struct myethhdr ) + sizeof(struct ip4hdr ) );
	int leng = htons(ip->total_len) + sizeof(struct myethhdr );

//  修改TTL
	ip->time -- ;
	ip->csum=0;
	ip->csum = in_cksum( (_U16 *)ip , ip->head_len*4 ) ;
	

//  查表获取mac
	int index = reroute( ip->dst_addr )	;  // 都是网络序

	stdshowip( ip->dst_addr ) ;

	printf(" index hhh : %d\n",index );
	if( index < 0 )
		return ;
//  chech error 
	printf("index : %d \n", index);
	int netcard = search_arp_cache( ip->dst_addr ) ;
	printf("netcard: %d \n", netcard);
	memcpy( buf , arp_table[netcard].mac_addr , 6 );
	memcpy( buf+6 , device[index].mac_addr , 6 ) ; 

//	showpacket( buf , len ) ;

	

//  发送
	sendto( socket_array[index] , buf , len , 0 , (struct sockaddr *)&send_addr[index] , sizeof( send_addr[index] ) ) ;
}



int mysend1(  char *buff , int len )
{

    int sock;
    int n_write;        
    int n_res;
    
    struct sockaddr_ll sll;
    struct ifreq ifstruct;

    char MAC_BUFFER[12]= {0x00,0x0c,0x29,0x75,0x72,0xea,0x00,0x0c,0x29,0x61,0x1a,0xda };
    char TYPE_BUFFER[2] = {0x88,0x66};

    printf ("Date:%s || Time:%s \n", __DATE__, __TIME__);

    if ((sock = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0)
    {
        fprintf (stdout, "create socket error\n");
        return -1;
    }

    n_res = 0;
    n_write = 0;

    memset (&sll, 0, sizeof (sll));
    sll.sll_family = PF_PACKET;
    sll.sll_protocol = htons (ETH_P_ALL);

    strcpy (ifstruct.ifr_name, NETCARD1 );
    ioctl (sock, SIOCGIFINDEX, &ifstruct);
    sll.sll_ifindex = ifstruct.ifr_ifindex;

	printf("index : %d\n" , ifstruct.ifr_ifindex);
    strcpy (ifstruct.ifr_name, NETCARD1 );
    ioctl (sock, SIOCGIFHWADDR, &ifstruct);
    memcpy (sll.sll_addr, ifstruct.ifr_ifru.ifru_hwaddr.sa_data, ETH_ALEN);
    sll.sll_halen = ETH_ALEN;
   
 
    if (bind (sock, (struct sockaddr *) &sll, sizeof (sll)) == -1)
    {
        printf ("bind:   ERROR\n");
        return -1;
    }

    memset(&ifstruct, 0, sizeof(ifstruct));
    strcpy (ifstruct.ifr_name, NETCARD1 );
    if (ioctl (sock, SIOCGIFFLAGS, &ifstruct) == -1)
    {
        perror ("iotcl()\n");
        printf ("Fun:%s Line:%d\n", __func__, __LINE__);
        return -1;
    }

    ifstruct.ifr_flags |= IFF_PROMISC;

    if(ioctl(sock, SIOCSIFFLAGS, &ifstruct) == -1)
    {
       perror("iotcl()\n");
       printf ("Fun:%s Line:%d\n", __func__, __LINE__);
       return -1;
    } 

//    在传入数据中已经设定好了 ethernent 包头
      memcpy(buff,MAC_BUFFER,12);
//    memcpy(buffer+6,sll.sll_addr,ETH_ALEN);
//    memcpy(buffer+12,TYPE_BUFFER,2);

	printf("len : %d\n",len);
	showpacket( buff , len);


       n_res = sendto ( sock, buff, len , 0, 
                         (struct sockaddr *) &sll, sizeof (sll));
        printf( "ret : %d\n",n_res);
		sleep(1 );
        if (n_res < 0)
        {
            perror("sendto() error\n");
            return -1;
        }

        n_write += n_res;
        if (n_write >= 2048 * 2560)
        {
			printf("%d\n",n_write);
        }
}




void *listen_packet(void *data)
{
	int sock_fd;
	int proto;
	int n_read;
	static _U8 buf[ 8096 ] ;

	struct sockaddr_ll sll;
	struct ifreq ifstruct;
	if( (sock_fd = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0)
	{
		printf("error create raw socket\n");
		return NULL;
	}


	while(1)
	{
		n_read = recvfrom(sock_fd,buf,2048,0,NULL,NULL);

		if(n_read < 42 ) 
		{
			printf("error when recv msg\n");
			return NULL;
		}
		push_to_buf( buf , n_read ) ;
		
		printf("---------------------\n");
	}
	return NULL;

}



//  初始化所有的socket 
int initial()
{
// 树吃化静态路由   arp表项
	initial_route() ;	
	init_mutithread();     // mutitherad support
	

	int i,sock;
	struct sockaddr_ll sll;
	struct ifreq ifstruct;
	for(i=0;i<device_index;i++ )
	{
		if((sock = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0)
		{
		    fprintf (stdout, "create socket error\n");
		    return -1;
		}

		memset (&sll, 0, sizeof (sll));
		sll.sll_family = PF_PACKET;
		sll.sll_protocol = htons (ETH_P_ALL);

		strcpy (ifstruct.ifr_name, device[i].interface );
		ioctl (sock, SIOCGIFINDEX, &ifstruct);
		sll.sll_ifindex = ifstruct.ifr_ifindex;

		printf("index : %d\n" , ifstruct.ifr_ifindex);
		strcpy (ifstruct.ifr_name, device[i].interface );
		ioctl (sock, SIOCGIFHWADDR, &ifstruct);
		memcpy (sll.sll_addr, ifstruct.ifr_ifru.ifru_hwaddr.sa_data, ETH_ALEN);
		sll.sll_halen = ETH_ALEN;
	   
	 
		if (bind (sock, (struct sockaddr *) &sll, sizeof (sll)) == -1)
		{
		    printf ("bind:   ERROR\n");
		    return -1;
		}

		memset(&ifstruct, 0, sizeof(ifstruct));
		strcpy (ifstruct.ifr_name, device[i].interface );
		if (ioctl (sock, SIOCGIFFLAGS, &ifstruct) == -1)
		{
		    perror ("iotcl()\n");
		    printf ("Fun:%s Line:%d\n", __func__, __LINE__);
		    return -1;
		}

		ifstruct.ifr_flags |= IFF_PROMISC;

		if(ioctl(sock, SIOCSIFFLAGS, &ifstruct) == -1)
		{
		   perror("iotcl()\n");
		   printf ("Fun:%s Line:%d\n", __func__, __LINE__);
		   return -1;
		} 	
		socket_array[i] = sock ;
		send_addr[i] = sll ;
	}
}

void *handle_packet(void *data)
{
	char *eth_head;
	char *ip_head;
	char *tcp_head;
	char *udp_head;
	char *icmp_head;
	_U8 *p;
	char buf[8096] ;
	int n_read = 0 ;
	_U32 proto ;
	while(1)
	{
		pull_from_buf( buf , &n_read );

// 只显示目标是自己的包
		if( !whether_inout( buf ) )
		{
			continue ;
//			route( buf , n_read ) ; 
		}


		eth_head = buf ;
		p = eth_head ;
		printf("%04x",*(_U16 *)(eth_head+12));
		switch( ntohs( *(_U16 *)(eth_head+12) ) )
		{
			case eth_IP:
				ip_head = eth_head+14;
				p = ip_head+12;
				printf("IP:%d,%d.%d.%d ==> %d.%d.%d.%d\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
				proto = (ip_head+9)[0] ;
				printf("protocal:");
				switch(proto){
					case IPPROTO_ICMP: printf("icmp\n"); /*showicmp(buf , n_read )*/ ;
						break;
					case IPPROTO_IGMP: printf("igmp\n");
						break;					
					case IPPROTO_IPIP: printf("ipip\n");	
						break;
					case IPPROTO_TCP:  printf("tcp:\n");
						write( ether_ip[1],  (char *)(&n_read)  , 4 );
						write( ether_ip[1],buf , n_read );
						break;
					case IPPROTO_UDP:  printf("udp:\n");
						break;
					default:printf("pls query yourself\n");
					}

 				break;
			case eth_ARP:
				printf("arp\n");
			//	showarp( buf , n_read );
			//	arp_reply( buf , n_read, device[1].mac_addr , inet_addr("192.168.1.1"));
				break;
			default:
				printf(" not ip or arp protocal, maybe wrong?\n");
				break; 
		}
	}
	return NULL ;
}



int main()
{	
	pthread_t th1,th2;
	void *retval;
	char buf[8096];
	int len=0;
	initial();

	pthread_create( &th1 ,NULL , listen_packet , 0 );
	pthread_create( &th2 ,NULL , handle_packet , 0 );

	pthread_join( th1, &retval );
	pthread_join( th2, &retval );


	return 0;
}








void dispnetinfo( )
{
	char szdevname[20] = "wlan0";
	int s = socket(AF_INET,SOCK_DGRAM,0);
	if( s<0)
	{
		fprintf(stderr,"create socket failed!errno=%d",errno);
		return ;
	}

	struct ifreq ifr;
	unsigned char mac[6];
	unsigned long nip , nnetmask,nbroadip;

	printf("%s:\n",szdevname);

	strcpy( ifr.ifr_name,szdevname );

	if( ioctl(s,SIOCGIFHWADDR,&ifr) < 0 )
	{
		return ;
	}
	memcpy(mac,ifr.ifr_hwaddr.sa_data,sizeof(mac) );
	printf("\tMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

	strcpy( ifr.ifr_name,szdevname);
	if( ioctl(s,SIOCGIFADDR,&ifr) < 0 )
	{
		nip=0;
	}
	else
	{
		nip = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2] ;
	}
	printf("\tip:%s\n",inet_ntoa(*(struct in_addr*)&nip));
	
	strcpy(ifr.ifr_name,szdevname);
	if( ioctl(s,SIOCGIFBRDADDR,&ifr)<0)
	{
		nbroadip=0;
	}
	else
	{
		nbroadip =*(unsigned long*)&ifr.ifr_broadaddr.sa_data[2] ;
	}
	printf("\tbroadip:%s\n",inet_ntoa(*(struct in_addr*)&nbroadip));

	strcpy(ifr.ifr_name,szdevname);
	if( ioctl(s,SIOCGIFNETMASK,&ifr)<0)
	{
		nnetmask =0;
	}
	else
	{
		nnetmask=*(unsigned long*)&ifr.ifr_netmask.sa_data[2] ;
	}
	printf("\tnetmask: %s\n",inet_ntoa(*(struct in_addr*)&nnetmask));
	close(s);

}
