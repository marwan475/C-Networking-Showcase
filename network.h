/*Marwan Mostafa mam024 11305332*/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#define ERROR_EXIT -1
#define SUCCESS_EXIT 0

#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

struct connection {
  int FD;
  int DBL;
};

/*holds users events*/
struct user {
  char* username;
  LIST* events;
};

/*holds event data*/
struct event {
  char* name;
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t start_hour;
  uint16_t start_min;
  uint16_t end_hour;
  uint16_t end_min;
};


void host_to_network(uint16_t *buff,int size);
void network_to_host(uint16_t *buff,int size);
uint16_t* operate(int DBlocation,uint16_t *request);
void handle_connections(int socket);
int get_DBL(uint16_t *packet);

#endif
