/*Marwan Mostafa mam024 11305332*/

#include <network.h>
#include <pthread.h>

struct sockaddr_storage their_addr;
socklen_t addr_size;
pthread_t threads[1024];
int count;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void *connection(void *dta)
{
  int r;
  int c;
  int *conp;
  int co;
  uint16_t buf[64];
  uint16_t *response;
  int DBL;
  int s;

  conp = (int*) dta;
  c = conp[0];
  co = conp[1];

  /*recv msg request from client*/
  r = recv(c,buf,sizeof(uint16_t)*64,0);
  if ( r > 0){
    /*wait for turn with DB*/	  
    pthread_mutex_lock( &mutex1 );
    fprintf(stdout,"server recieved request\n");
    network_to_host(buf,64);
    DBL = get_DBL(buf);
    if (DBL == -1) DBL = co;
    response = operate(DBL,buf);
    host_to_network(response,64);
    pthread_mutex_unlock( &mutex1 );
  }

  /*send response to client*/
  s = send(c,response,sizeof(uint16_t)*64,0);
  fprintf(stdout,"server sent response\n");
  if (s != -1){
    /*close connection*/
    close(c);
    free(response);
  }

  return NULL;
}

void handle_connections(int socket)
{
  int c;
  int *conp;  
  
  count = 0;

  fprintf(stdout,"server running using MultiThread\n");

  while (1){
    /*accepting new connection and storing it*/		  	
    c = accept(socket, (struct sockaddr *)&their_addr, &addr_size);

    fprintf(stdout,"server recieved connection\n");

    /*checking return value of accept*/
    if (c == -1){
      fprintf(stderr,"accept error");
      exit(1);
    }

    conp = (int*)malloc(sizeof(int)*2);

    conp[0] = c;
    conp[1] = count;

    pthread_create(&threads[count],NULL,connection,(void*)conp);

    count++;
  }
}
