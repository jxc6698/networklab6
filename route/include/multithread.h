#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAC_PACKET_SIZE 8096
#define PACKET_CACHE 20 

struct{
	int i,j;
	unsigned char buffer[ PACKET_CACHE ][8096];  // buffer
	int len[PACKET_CACHE];       // length
	pthread_mutex_t lock;
	pthread_cond_t notempty;
	pthread_cond_t notfull;
}pack_cache;


void init_mutithread()
{
	pthread_mutex_init(&pack_cache.lock , NULL ) ;
	pthread_cond_init(&pack_cache.notempty , NULL );
	pthread_cond_init(&pack_cache.notfull , NULL );
	pack_cache.i=0;
	pack_cache.j=0;
}

void push_to_buf( char *buf , int n_read )
{
	pthread_mutex_lock( &pack_cache.lock );
	if( (pack_cache.i+1)%PACKET_CACHE == pack_cache.j )
	{
		pthread_mutex_unlock( &pack_cache.lock );
		pthread_cond_wait(&pack_cache.notfull,&pack_cache.lock); 
		printf("pack_cache buffer is full\n");
//		exit(-1);		
	}
	printf("push : %d\n", pack_cache.i ) ;
	memcpy( pack_cache.buffer[pack_cache.i] , buf , n_read );
	pack_cache.len[pack_cache.i] = n_read ;
	pack_cache.i = (pack_cache.i +1) % PACKET_CACHE ;

	pthread_cond_signal( &pack_cache.notempty);
	pthread_mutex_unlock( &pack_cache.lock );
}


void pull_from_buf( char *buf , int *n_read )
{
	
	pthread_mutex_lock( &pack_cache.lock );
	if( pack_cache.i == pack_cache.j )
	{
		pthread_mutex_unlock( &pack_cache.lock );
		pthread_cond_wait(&pack_cache.notempty,&pack_cache.lock);
//		pthread_mutex_lock( &pack_cache.lock );
		printf("pack_cache buffer is empty\n");
//		exit(-1);
	}
	printf( "poll : %d\n",pack_cache.j );
	memcpy( buf , pack_cache.buffer[pack_cache.j] , pack_cache.len[pack_cache.j] );
	*n_read = pack_cache.len[pack_cache.j];
	pack_cache.j = (pack_cache.j +1) % PACKET_CACHE ;
	pthread_cond_signal( &pack_cache.notfull );
	pthread_mutex_unlock( &pack_cache.lock );
}


