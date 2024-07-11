/*Marwan Mostafa mam024 11305332*/
/*Contains common functions used by both client and servers*/

#include <network.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>


/*convert array of integers from host to network*/
void host_to_network(uint16_t *buff,int size)
{
  int i;

  for (i = 0;i<size;i++){
    buff[i] = htons(buff[i]);
  }
}

/*convert array of integers from network to host*/
void network_to_host(uint16_t *buff,int size)
{
  int i;

  for (i = 0;i<size;i++){
    buff[i] = ntohs(buff[i]);
  }
}
