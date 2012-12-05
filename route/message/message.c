#include "message.h"

int msgid ;


int msg_init()
{
	key_t key ;
	if( (key = ftok( MESSAGE_FILE_NAME , 'a')) < 0 )
	{
		perror("create key error:");
		return -1 ;
	}
	if((msgid=msgget(key,0666|IPC_CREAT ) ) < 0)
	{
		perror("create message error:");
		return -1 ;
	}	
	
}

int msg_end() 
{
	if( msgctl( msgid , IPC_RMID , NULL ) )
	{
		perror( "delete message error:");
	}


}


int msg_send( char buf[] , int len )
{
	
	struct msgtype msg ;
	msg.mtype = 1 ;
	memcpy( msg.buffer , buf , len  ) ;
	if( msgsnd( msgid , &msg ,sizeof( struct msgtype) , 0 ) < 0 )
		perror("message send error:");
	return 0 ;
}

int msg_recv( char buf[] , int *len )
{
	struct msgtype msg ;
	if( msgrcv( msgid , &msg ,sizeof( struct msgtype)  , 1 , 0 ) < 0)
		perror("message recv error");
	else
		memcpy( buf , msg.buffer , sizeof( msg.buffer) ) ;
	return 0 ;
}
