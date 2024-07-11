# Marwan Mostafa mam024 11305332

CC=gcc
CFLAGS=-g
CPPFLAGS=-std=gnu90 -Wall -Wextra -pedantic
CCARM=arm-linux-gnueabihf-
CCPPC=powerpc-linux-gnu-

all: client server_Approach server_MultiThread server_MultiProc

arm: client_arm server_A_arm server_MT_arm server_MP_arm

ppc: client_ppc server_A_ppc server_MT_ppc server_MP_ppc

client: client.o network.o
	$(CC) -o client client.o network.o

client.o: client.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c client.c -I. 

server_Approach: server.o liblist.a network.o server_Approach.o
	$(CC) -o server_Approach server.o network.o server_Approach.o \
		-L. -llist
server_Approach.o: server_Appproach.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o server_Approach.o \
		server_Appproach.c -I. 

server_MultiThread: server.o liblist.a network.o server_MultiThread.o
	$(CC) -o server_MultiThread \
		server.o network.o server_MultiThread.o -L. -llist -lpthread

server_MultiThread.o: server_MultiThread.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o server_MultiThread.o \
                server_MultiThread.c -I. 

server_MultiProc: server.o liblist.a network.o server_MultiProc.o
	$(CC) -o server_MultiProc \
                server.o network.o server_MultiProc.o -L. -llist 

server_MultiProc.o: server_MultiProc.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o server_MultiProc.o \
                server_MultiProc.c -I.

server: server.o liblist.a network.o
	$(CC) -o server server.o network.o -L. -llist

server.o: server.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c server.c -I. 

network.o: network_functions.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c network_functions.c -o network.o -I.

liblist.a: list_adders.o list_movers.o list_removers.o
	ar -rc liblist.a list_adders.o list_movers.o list_removers.o

list_adders.o: list_adders.c list.h
	$(CC) -I./ -o list_adders.o -c $(CFLAGS) $(CPPFLAGS) list_adders.c

list_movers.o: list_movers.c list.h
	$(CC) -I./ -o list_movers.o -c $(CFLAGS) $(CPPFLAGS) list_movers.c

list_removers.o: list_removers.c list.h
	$(CC) -I./ -o list_removers.o -c $(CFLAGS) $(CPPFLAGS) \
	list_removers.c

client_arm: client_arm.o network_arm.o
	$(CC) -o client_arm client_arm.o network_arm.o

client_arm.o: client.c
	$(CCARM) $(CFLAGS) $(CPPFLAGS) -c client.c -I. -o client_arm.o

server_A_arm: server_arm.o liblistarm.a network_arm.o server_A_arm.o
	$(CCARM) -o server_A_arm server_arm.o network_arm.o server_A_arm.o \
                -L. -llistarm
server_A_arm.o: server_Appproach.c
	$(CCARM) $(CFLAGS) $(CPPFLAGS) -c -o server_A_arm.o \
                server_Appproach.c -I.

server_MT_arm: server_arm.o liblistarm.a network_arm.o server_MT_arm.o
	$(CCARM) -o server_MT_arm server_arm.o \
		network_arm.o server_MT_arm.o -L. -llistarm -lpthread

server_MT_arm.o: server_MultiThread.c
	$(CCARM) $(CFLAGS) $(CPPFLAGS) -c -o server_MT_arm.o \
                server_MultiThread.c -I.

server_MP_arm: server_arm.o liblistarm.a network_arm.o server_MP_arm.o
	$(CCARM) -o server_MP_arm \
                server_arm.o network_arm.o server_MP_arm.o -L. -llistarm

server_MP_arm.o: server_MultiProc.c
	$(CCARM) $(CFLAGS) $(CPPFLAGS) -c -o server_MP_arm.o \
                server_MultiProc.c -I.

server_arm.o: server.c
	$(CCARM) $(CFLAGS) $(CPPFLAGS) -c server.c -I. -o server_arm.o

network-arm.o: network_functions.c
	$(CCARM) $(CPPFLAGS) -c network_functions.c -o network_arm.o -I.

client_ppc: client_ppc.o network_ppc.o
	$(CCPPC) -o client_ppc client_ppc.o network_ppc.o

client_ppc.o: client.c
	$(CCPPC) $(CFLAGS) $(CPPFLAGS) -c client.c -I. -o client_ppc.o

server_A_ppc: server_ppc.o liblistppc.a network_ppc.o server_A_ppc.o
	$(CCPPC) -o server_A_ppc server_ppc.o network_ppc.o \
		server_A_ppc.o -L. -llistarm
server_A_ppc.o: server_Appproach.c
	$(CCPPC) $(CFLAGS) $(CPPFLAGS) -c -o server_A_ppc.o \
                server_Appproach.c -I.

server_MT_ppc: server_ppc.o liblistppc.a network_ppc.o server_MT_ppc.o
	$(CCPPC) -o server_MT_ppc server_ppc.o \
                network_ppc.o server_MT_ppc.o -L. -llistppc -lpthread

server_MT_ppc.o: server_MultiThread.c
	$(CCPPC) $(CFLAGS) $(CPPFLAGS) -c -o server_MT_ppc.o \
                server_MultiThread.c -I.

server_MP_arm: server_ppc.o liblistppc.a network_ppc.o server_MP_ppc.o
	$(CCPPC) -o server_MP_ppc \
                server_ppc.o network_ppc.o server_MP_ppc.o -L. -llistppc

server_MP_ppc.o: server_MultiProc.c
	$(CCPPC) $(CFLAGS) $(CPPFLAGS) -c -o server_MP_ppc.o \
                server_MultiProc.c -I.

server_ppc.o: server.c
	$(CCPPC) $(CFLAGS) $(CPPFLAGS) -c server.c -I. -o server_ppc.o

network-ppc.o: network_functions.c
	$(CCPPC) $(CPPFLAGS) -c network_functions.c -o network_ppc.o -I.  

list_adders_arm.o: list_adders.c list.h
	$(CCARM)$(CC)-10 -I./ -o list_adders_arm.o \
	       	-c $(CPPFLAGS) list_adders.c 

list_movers_arm.o: list_movers.c list.h
	$(CCARM)$(CC)-10 -I./ -o list_movers_arm.o \
	       	-c $(CPPFLAGS) list_movers.c 

list_removers_arm.o: list_removers.c list.h
	$(CCARM)$(CC)-10 -I./ -o list_removers_arm.o -c $(CPPFLAGS) \
	list_removers.c

liblistarm.a: list_adders_arm.o list_movers_arm.o list_removers_arm.o
	ar -rc liblistarm.a list_adders_arm.o list_movers_arm.o \
		list_removers_arm.o

list_adders_ppc.o: list_adders.c list.h
	$(CCPPC)$(CC)-10 -I./ -o list_adders_ppc.o \
		-c $(CPPFLAGS) list_adders.c 

list_movers_ppc.o: list_movers.c list.h
	$(CCPPC)$(CC)-10 -I./ -o list_movers_ppc.o \
	       	-c $(CPPFLAGS) list_movers.c 

list_removers_ppc.o: list_removers.c list.h
	$(CCPPC)$(CC)-10 -I./ -o list_removers_ppc.o -c $(CPPFLAGS) \
	list_removers.c

clean:
	rm -f client
	rm -f server
	rm -f server_Approach
	rm -f server_MultiThread
	rm -f server_MultiProc
	rm -f *.o
	rm -f liblist.a
