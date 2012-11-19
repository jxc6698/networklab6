
CC = gcc 
CFLAGS = -lpthread  -I./include
LDFLAGS = -e main

objects = function/function_head.o route.o  essential/essential_data.o

headerfile = mylock.h essential_head.h essential_data.h function_head.h multithread.h typess.h header.h arp.h


target: $(objects) 
	$(CC)  $(objects) $(CFLAGS) -o test 

#function_head.o : function_head.c  
#	$(CC) -c  function_head.c  $(CFLAGS) -o function_head.o
#route.o : route.c 
#	$(CC) -c route.c $(CFLAGS) -o route.o
#essential_data.o : essential_data.c 
#	$(CC) -c essential_data.c $(CFLAGS) -o essential_data.o 

clean :
	find . -name "*.o" | xargs rm -f
#	rm -rf *.o
