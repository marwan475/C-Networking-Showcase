/*Marwan Mostafa mam024 11305332*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <network.h>

/*Command line argument vars*/
char* hostname;
char* port;
char myname[13];
char* msgtype;
int date;
int starttime;
int endtime;
char Eventtitle[10];

/*packet to send*/
uint16_t packet[64];

/*packet recieved*/
uint16_t output[64];

/*This function formats the CMD args 
 * into a packet following the protocol*/
void format_packet(int msgtype, int arc)
{
  int i;
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t hour;
  uint16_t min;  

  /*formating header*/

  /*adding name to header in int*/
  for (i = 0;i<13;i++){
    packet[i] = (uint16_t) myname[i];
  }

  /*adding msgtype to header*/
  packet[13] = msgtype;

  /*formating data*/

  switch (msgtype){
    case 2:
	    /*add msgtype packet formatting*/
            day = date % 100;
	    month = (date - day) % 10000;
	    year = (date - month - day) / 10000;

	    packet[14] = month;
	    packet[15] = day;
	    packet[16] = year;
	    
	    min = starttime % 100;
	    hour = (starttime - min ) / 100;

	    packet[24] = hour;
	    packet[25] = min;

	    min = endtime % 100;
            hour = (endtime - min ) / 100;

            packet[34] = hour;
            packet[35] = min;

	    for (i = 44;i<54;i++){
	      packet[i] = (uint16_t) Eventtitle[i-44];
	    } 

	    break;
    case 3:
	    /*remove msgtype packet formatting*/
            day = date % 100;
            month = (date - day) % 10000;
            year = (date - month - day) / 10000;

            packet[14] = month;
            packet[15] = day;
	    packet[16] = year;

            min = starttime % 100;
            hour = (starttime - min ) / 100;

            packet[24] = hour;
            packet[25] = min;

	    break;
    case 4:
	    /*update msgtype packet formatting*/
            day = date % 100;
            month = (date - day) % 10000;
            year = (date - month - day) / 10000;

            packet[14] = month;
            packet[15] = day;
	    packet[16] = year;

            min = starttime % 100;
            hour = (starttime - min ) / 100;

            packet[24] = hour;
            packet[25] = min;

            min = endtime % 100;
            hour = (endtime - min ) / 100;

            packet[34] = hour;
            packet[35] = min;

            for (i = 44;i<54;i++){
              packet[i] = (uint16_t) Eventtitle[i-44];
            }

	    break;
    case 5: 
	    /*get msgtype packet formatting*/
            day = date % 100;
            month = (date - day) % 10000;
            year = (date - month - day) / 10000;

            packet[14] = month;
            packet[15] = day;
	    packet[16] = year;

	    if (arc == 7){
              min = starttime % 100;
              hour = (starttime - min ) / 100;

              packet[24] = hour;
              packet[25] = min;

	      packet[63] = 7;

	    }
           
	    break;
    case 6:
	    packet[14] = 0;
	    break;	    
    default:	    
	    fprintf(stderr,"non reconized msgtype");
  }
}

/*this function prints the servers response to stdout*/
void display_output()
{
  int i;	
  char msg[50];

  network_to_host(output,64);
  
  if (output[15] != 0){ 
    for (i = 0;i<50;i++){
      msg[i] = (char) output[i+14];
    }

    fprintf(stdout,"\n%s\n",msg);
  }else{
    fprintf(stdout,"\n%d\n",output[14]);
  }
}

int main(int argc, char * argv[])
{
  
  int s; /*Main socket*/
  int c; /*connect return value*/
  int bs; /*number of bytes sent*/
  int br; /*bytes recieved*/

  /*Vars for socket setup*/
  int errno, rc;
  struct hostent hbuf, *hp;
  char tmp[8192];
  struct in_addr **addr_list;
  char *had; /*Host address*/

  /*Vars for getaddrinfo*/
  int status;
  struct addrinfo hints;
  struct addrinfo *res;

  /*User must enter at least 4 args, up to msgtype*/
  if (argc < 5) {
    fprintf(stderr,"need minimum 4 args");
    exit(1);
  }

  /*check for myname length*/
  if (strlen(argv[3]) > 12){
    fprintf(stderr,"max 12 chars for 3rd arg");
    exit(1);
  }

  memset(myname,0,13);
  memset(Eventtitle,0,10);

  /*Saving cmd args into vars*/
  hostname = argv[1];
  port = argv[2];
  strcpy(myname,argv[3]); /*risk of buffer overflow if len not checked*/
  msgtype = argv[4];

  /*getting server ip from hostname*/
  rc = gethostbyname_r(hostname,
                            &hbuf,
                              tmp,
                             8192,
                              &hp,
                          &errno);

  /*checking gethostbyname_r return value*/
  if (rc != 0) {
     printf("gethostbyname_r error: %s\n", hstrerror(errno));
     exit(0);
  }

  /*Extracting ip*/
  addr_list = (struct in_addr **)hp->h_addr_list;
  had = inet_ntoa(*addr_list[0]);

  /*setting up structs for getaddrinfo*/
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  /*calling getaddrinfo*/
  status = getaddrinfo(had,port,&hints,&res);

  /*checking return of getaddrinfo*/
  if (status != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  /*setting up socket*/
  s = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

  /*checking return of socket*/
  if (s == -1){
    fprintf(stderr,"socket error:");
    exit(1);
  }

  /*connecting to host*/
  c = connect(s,res->ai_addr,res->ai_addrlen);

  /*checking connect return value*/
  if (c == -1){
    fprintf(stderr,"connection failed");
    exit(1);    
  }

  /*checking for msg type add*/
  if (strcmp(msgtype,"add") == 0){
    date = atoi(argv[5]);
    starttime = atoi(argv[6]);
    endtime = atoi(argv[7]);

    /*check for eventname length*/
    if (strlen(argv[8]) > 9){
      fprintf(stderr,"max 9 chars for 8th arg");
      exit(1);
    }

    strcpy(Eventtitle,argv[8]); /*buffer overflow risk*/

    format_packet(2,argc); /*formating data into packet for sending*/
    host_to_network(packet,64); /*converting packet to network order*/
  }

  /*checking for msg type remove*/
  if (strcmp(msgtype,"remove") == 0){
    date = atoi(argv[5]);
    starttime = atoi(argv[6]);

    format_packet(3,argc); /*formating data into packet for sending*/
    host_to_network(packet,64); /*converting packet to network order*/
  }

  /*checking for msg type update*/
  if (strcmp(msgtype,"update") == 0){
    date = atoi(argv[5]);
    starttime = atoi(argv[6]);
    endtime = atoi(argv[7]);

    /*check for eventname length*/
    if (strlen(argv[8]) > 9){
      fprintf(stderr,"max 9 chars for 8th arg");
      exit(1);
    }

    strcpy(Eventtitle,argv[8]); /*buffer overflow risk*/

    format_packet(4,argc); /*formating data into packet for sending*/
    host_to_network(packet,64); /*converting packet to network order*/
  }

  /*checking for msg type get*/
  if (strcmp(msgtype,"get") == 0){
    date = atoi(argv[5]);
    if (argc == 7){
      starttime = atoi(argv[6]);
    }

    format_packet(5,argc); /*formating data into packet for sending*/
    host_to_network(packet,64); /*converting packet to network order*/
  }

  /*checking for msg type getall*/
  if (strcmp(msgtype,"getall") == 0){
    
    format_packet(6,argc); /*formating data into packet for sending*/
    host_to_network(packet,64); /*converting packet to network order*/	  
  }

  bs = send(s,packet,sizeof(uint16_t)*64,0); /*sending packet to server*/

  /*checking if all bytes were sent or if an error occured*/
  if (bs < 64 || bs == -1){
    fprintf(stderr,"send failed");
  }  

  /*waiting for response from server*/
  br = recv(s,output,sizeof(uint16_t)*64,0);

  /*checking if all bytes were recieved or if an error occured*/
  if (br < 64 || br == -1){
    fprintf(stderr,"recv failed");
  }

  /*checking if connection got closed*/
  if (br == 0){
    fprintf(stderr,"connection closed");
    exit(1);
  }

  /*display server response*/
  display_output();

  freeaddrinfo(res);

  return 0;
}
