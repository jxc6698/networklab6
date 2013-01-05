#include<sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <string.h>


#define MESSAGE_BUFFER     256
#define MESSAGE_FILE_NAME  "./formessage"
struct msgtype{
	long mtype ;
	char buffer[ MESSAGE_BUFFER ];
};

int  msgid ;


int main()
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

	char buf[50] ;

	struct msgtype msg ;
	msg.mtype = 1 ;
//	memcpy( msg.buffer , buf , len  ) ;
	*(int *)(msg.buffer) = 1 ;
	if( msgsnd( msgid , &msg ,sizeof( struct msgtype) , 0 ) < 0 )
		perror("message send error:");

	struct msgtype msg1 ;
	msg1.mtype = 1 ;
//	memcpy( msg.buffer , buf , len  ) ;
	*(int *)(msg1.buffer) = 2 ;
	if( msgsnd( msgid , &msg1 ,sizeof( struct msgtype) , 0 ) < 0 )
		perror("message send error:");

	printf("send success \n");
	scanf("%s" , buf ) ;
	if( msgctl( msgid , IPC_RMID , NULL ) )
	{
		perror( "delete message error:");
	}

return 0 ;
}

