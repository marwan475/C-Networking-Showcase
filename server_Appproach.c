/*Marwan Mostafa mam024 11305332*/

#include <network.h>
#include <sys/select.h>

struct sockaddr_storage their_addr;
socklen_t addr_size;
LIST* connections; /*list holding all conections FD*/
int count; /*number of connections*/
uint16_t buf[64];
uint16_t *response;

void handle_connections(int socket){

        int c,n,rv,i,r,s,DBL;
        fd_set readfds;
        struct timeval tv;
	struct connection *cs;

        count = 0;

        fprintf(stdout,"server running using Approach based\n");

	/*creating list that holds all connection file discriptors*/
	connections = ListCreate();

	tv.tv_sec = 1;
        tv.tv_usec = 500000;

	while(1){
          /*checking if main socket has incomming connection*/
	  FD_ZERO(&readfds);
          FD_SET(socket, &readfds);
	  n = socket + 1;
	  rv = select(n, &readfds, NULL, NULL, &tv);
          
	  if (rv == -1) fprintf(stderr,"selcet error");
	  else if (FD_ISSET(socket, &readfds)){
            /*accepting new connection and storing it*/		  
	    c = accept(socket, (struct sockaddr *)&their_addr, &addr_size);
	    
	    fprintf(stdout,"server recieved connection\n"); 

	    /*checking return value of accept*/
	    if (c == -1){
              fprintf(stderr,"accept error");
              exit(1);
            }

            cs = (struct connection*)malloc(sizeof(struct connection));

	    cs->FD = c;
	    cs->DBL = count; 

	    /*adding new connection to list*/
	    ListAppend(connections,cs);

	    count++;
	  }
          
	  /*check for open connections*/
	  if (ListCount(connections) > 0){
	    ListFirst(connections);
	    for (i = 0;i<=ListCount(connections);i++){
	      cs = ListCurr(connections);
	      if (cs == NULL) break;
	      c = cs->FD;

	      FD_ZERO(&readfds);
              FD_SET(c, &readfds);
              n = c + 1;
              rv = select(n, &readfds, NULL, NULL, &tv);
	      if (rv == -1) fprintf(stderr,"selcet error");
	      else if (FD_ISSET(c, &readfds)){
	        r = recv(c,buf,sizeof(uint16_t)*64,0);
		fprintf(stdout,"server recieved request\n");
		if ( r > 0 ){
		  network_to_host(buf,64);
		  DBL = get_DBL(buf);
		  if (DBL == -1) DBL = cs->DBL;
		  response = operate(DBL,buf);
		  host_to_network(response,64);
		  s = send(c,response,sizeof(uint16_t)*64,0);
		  fprintf(stdout,"server sent response\n");
		  if (s != -1){
		    /*close connection*/	  
		    free(cs);
		    ListRemove(connections);
		    close(c);
	            free(response);	    
		  }
		}
	      }
	      ListNext(connections);
	    }
	  }

	}

}
