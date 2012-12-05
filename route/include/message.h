#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include "essential_head.h"

#define MESSAGE_BUFFER     256
#define MESSAGE_FILE_NAME  "./formessage"


struct msgtype{
	long mtype ;
	char buffer[ MESSAGE_BUFFER ];
};

extern int  msgid ;


extern int msg_init() ;
extern int msg_end() ;
extern int msg_send( char buf[] , int len  ) ;
extern int msg_recv( char buf[] , int *len )  ;


#endif
