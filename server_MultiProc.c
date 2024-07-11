/*Marwan Mostafa mam024 11305332*/

#include <network.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

struct sockaddr_storage their_addr;
socklen_t addr_size;
int count;
/*array of usable pipes for IPC*/
int fd[1024][2];

void connection(int c)
{
  int r;
  int s;
  uint16_t buf[64];
  uint16_t response[64];

  /*recv msg request from client*/
  r = recv(c,buf,sizeof(uint16_t)*64,0);

  if (r > 0){
    network_to_host(buf,64);

    /*send request to main proc*/
    write(fd[count][1],buf,sizeof(uint16_t)*64);

    sleep(1);

    /*get request from main proc*/
    read(fd[count][0],response,sizeof(uint16_t)*64);
  
    host_to_network(response,64);

    /*send response to client*/
    s = send(c,response,sizeof(uint16_t)*64,0);
    if (s != -1){
      /*close connection*/
      close(c);
    }

  }
  close(fd[count][0]);
  close(fd[count][1]);
  exit(0);
}

void handle_connections(int socket)
{  
  pid_t p;
  int c;
  int i;
  fd_set readfds;
  struct timeval tv;
  int rv;
  int n;
  uint16_t request[64];
  uint16_t *response;
  int DBL;
  int ndb;

  tv.tv_sec = 1;
  tv.tv_usec = 500000;

  fprintf(stdout,"server running using MultiProc\n");

  count = 0;
  ndb = 0;

  while (1){
    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);
    n = socket + 1;
    rv = select(n, &readfds, NULL, NULL, &tv);

    if (rv == -1) fprintf(stderr,"selcet error");
    else if (FD_ISSET(socket, &readfds)){    
      /*accept connection*/
      c = accept(socket, (struct sockaddr *)&their_addr, &addr_size);

      fprintf(stdout,"server recieved connection\n");

      /*checking return value of accept*/
      if (c == -1){
        fprintf(stderr,"accept error");
        exit(1);
      }
    
      /*create pipe for comunication*/
      pipe(fd[count]);

      /*forking child proc to handle conections requests*/
      p = fork();

      if (p<0){
        fprintf(stderr,"fork error");
        exit(1);
      }else if (p == 0){ 
        connection(c);
      }
      count++;
      ndb++;
    }

    /*checking if child procs have a request waiting to be handled*/
    for (i = 0;i<count;i++){
      FD_ZERO(&readfds);
      FD_SET(fd[i][0], &readfds);
      n = fd[i][0] + 1;
      rv = select(n, &readfds, NULL, NULL, &tv);
      if (rv == -1) fprintf(stderr,"selcet error");
      else if (FD_ISSET(fd[i][0], &readfds)){
        memset(request,0,sizeof(uint16_t)*64);
	read(fd[i][0],request,sizeof(uint16_t)*64);
	fprintf(stdout,"server recieved request\n");
        DBL = get_DBL(request);
        if (DBL == -1) DBL = ndb;
        response = operate(DBL,request);
        write(fd[i][1],response,sizeof(uint16_t)*64);
	sleep(1);
	fprintf(stdout,"server sent response\n");
        free(response);
	count--;
        close(fd[i][0]);
        close(fd[i][1]);	
      }
    }

  }
  
}
