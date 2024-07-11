/*Marwan Mostafa mam023 11305332*/

#include <network.h>
/*max users, based of max amount of files a proc can open*/
#define MAXUSERS 1024

char *output1 = "added";
char *output2 = "removed";
char *output3 = "updated";
char *output4 = "conflict";
char *output5 = "error";
char *output6 = "clearedd";

struct user mainDB[MAXUSERS]; /*main database*/

int get_DBL(uint16_t *packet)
{
  char *username;
  int i;

  /*allocating space for username*/
  username = (char*)malloc(sizeof(char)*13);

  for (i = 0;i<13;i++){
    username[i] = (char) packet[i];
  }

  for ( i = 0;i<MAXUSERS;i++){
    if (mainDB[i].username != 0){	  
      if (strcmp(username,mainDB[i].username) == 0) return i;
    }
  }
  return -1;
}

/*free event struct*/
void free_event(struct event *e)
{
  free(e->name);
  free(e);
}

/*returns num events for user or num events for day if date is given*/
int num_events(uint16_t year,uint16_t month,uint16_t day,int DBL)
{
  int i;
  struct event *e;
  int count;
  int check;

  count = 0;

  if (mainDB[DBL].events == NULL) return 0;

  if(ListCount(mainDB[DBL].events)==0) return 0;

  if (year == 0 && month == 0 && day == 0){
    return ListCount(mainDB[DBL].events);
  }else{
    ListFirst(mainDB[DBL].events);
    for (i = 0;i<=ListCount(mainDB[DBL].events);i++){
      check = 0;
      
      e = ListCurr(mainDB[DBL].events);

      if (e == NULL) break;
      if (e->year == year) check += 1;
      if (e->month == month) check += 1;
      if (e->day == day) check += 1;  

      if (check == 3) count += 1;
      ListNext(mainDB[DBL].events);
    }        
  }
  return count;
}

/*return event pointer given data and set list cursor to item*/
struct event* search(uint16_t year,
		     uint16_t month,
		     uint16_t day,
		     uint16_t start_hour,
		     uint16_t start_min,
		     int DBlocation)
{
  struct event* e;
  int count;
  int i;
  LIST *events;
  int check;

  check = 0;

  e = NULL;

  events = mainDB[DBlocation].events;

  count = ListCount(events);

  ListFirst(events);

  for (i = 0;i <= count;i++){
    e = ListCurr(events);
    if (e == NULL) return NULL;
    if (e->year == year) check += 1;
    if (e->month == month) check += 1;
    if (e->day == day) check += 1;
    if (e->start_hour == start_hour) check += 1;
    if (e->start_min == start_min) check += 1;

    if (check == 5){
      return e;   
    }else{
      check = 0;
      ListNext(events);
    }
  }
   
  return e;
}

/*performs operation given the request and returns new packet for s*/
uint16_t* operate(int DBlocation,uint16_t *request){
  uint16_t *response; /*server reponse for request following protocol*/
  uint16_t msgtype;
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t start_hour;
  uint16_t start_min;
  uint16_t end_hour;
  uint16_t end_min;
  char *username;
  char *eventname;
  int i;
  struct event *e;
  int count;
  int j;

  /*allocating space for response*/
  response = (uint16_t*)malloc(sizeof(uint16_t)*64);

  /*allocating space for username*/
  username = (char*)malloc(sizeof(char)*13);

  /*allocating space for event name*/
  eventname = (char*)malloc(sizeof(char)*10);

  /*saving msgtype from request packet*/
  msgtype = request[13];

  if (mainDB[DBlocation].username != NULL){
    free(mainDB[DBlocation].username);
  }

  mainDB[DBlocation].username = username;

  if (mainDB[DBlocation].events == NULL){
    mainDB[DBlocation].events = ListCreate();
  }

  for (i = 0;i<13;i++){
    username[i] = (char) request[i];
    response[i] = request[i];
  }

   switch (msgtype){
    case 2:
	    /*add msgtype packet extraction*/
     
	    month = request[14];
	    day = request[15];
	    year = request[16];

	    start_hour = request[24];
	    start_min = request[25];

            end_hour = request[34];
            end_min = request[35];

	    for (i = 44;i<54;i++){
	      eventname[i-44] = (char) request[i];
	    }

	    e = search(year,month,day,start_hour,start_min,DBlocation);

	    /*bonus 1 conflict detection*/
	    if (e != NULL){
	      response[13] = 1;

              response[14] = (uint16_t) output4[0];
	      response[15] = (uint16_t) output4[1];
	      response[16] = (uint16_t) output4[2];
	      response[17] = (uint16_t) output4[3];
	      response[18] = (uint16_t) output4[4];
	      response[18] = (uint16_t) output4[5];
	      response[18] = (uint16_t) output4[6];
	      response[18] = (uint16_t) output4[7];
	    
	      break;
	    }

	    /*allocating space for new event*/
            e = (struct event*)malloc(sizeof(struct event));

            e->month = month;
	    e->day = day;
	    e->year = year;

	    e->start_hour = start_hour;
	    e->start_min = start_min;

	    e->end_hour = end_hour;
	    e->end_min = end_min;

	    e->name = eventname;

            /*adding formated event struct to users list of events*/
	    ListAppend(mainDB[DBlocation].events,e);

	    /*success*/
	    response[13] = 1;

            response[14] = (uint16_t) output1[0];
	    response[15] = (uint16_t) output1[1];
	    response[16] = (uint16_t) output1[2];
	    response[17] = (uint16_t) output1[3];
	    response[18] = (uint16_t) output1[4]; 

	    break;
    case 3:
	    /*remove msgtype packet extraction*/
           
            month = request[14];
            day = request[15];
            year = request[16];

            start_hour = request[24];
            start_min = request[25];

	    /*search for event and return pointer*/
	    e = search(year,month,day,start_hour,start_min,DBlocation);

	    /*remove event from list and free the allocated mem*/
	    if (e != NULL){
	      ListRemove(mainDB[DBlocation].events);
              free_event(e);

	      /*success*/
              response[13] = 1;

              response[14] = (uint16_t) output2[0];
              response[15] = (uint16_t) output2[1];
              response[16] = (uint16_t) output2[2];
              response[17] = (uint16_t) output2[3];
              response[18] = (uint16_t) output2[4];
              response[19] = (uint16_t) output2[5];
              response[20] = (uint16_t) output2[6];
	    }else{
	      /*error*/	    
	      response[13] = 8;

              response[14] = (uint16_t) output5[0];
              response[15] = (uint16_t) output5[1];
              response[16] = (uint16_t) output5[2];
              response[17] = (uint16_t) output5[3];
              response[18] = (uint16_t) output5[4];
	    }

	    break;
    case 4:
	    /*update msgtype packet extraction*/
            
            month = request[14];
            day = request[15];
            year = request[16];

            start_hour = request[24];
            start_min = request[25];

            end_hour = request[34];
            end_min = request[35];

            for (i = 44;i<54;i++){
              eventname[i-44] = (char) request[i];
            }

	    /*searches for event and returns pointer*/
	    e = search(year,month,day,start_hour,start_min,DBlocation);

	    /*updates event with changed info*/
	    if (e != NULL){
	      e->month = month;
	      e->day = day;
	      e->year = year;

	      e->start_hour = start_hour;
	      e->start_min = start_min;

	      e->end_hour = end_hour;
	      e->end_min = end_min;

	      e->name = eventname;

	      /*success*/
              response[13] = 1;

              response[14] = (uint16_t) output3[0];
              response[15] = (uint16_t) output3[1];
              response[16] = (uint16_t) output3[2];
              response[17] = (uint16_t) output3[3];
              response[18] = (uint16_t) output3[4];
              response[19] = (uint16_t) output3[5];
              response[20] = (uint16_t) output3[6];
	    }else{
              /*error*/
              response[13] = 8;

              response[14] = (uint16_t) output5[0];
              response[15] = (uint16_t) output5[1];
              response[16] = (uint16_t) output5[2];
              response[17] = (uint16_t) output5[3];
              response[18] = (uint16_t) output5[4];
            }

	    break;
    case 5:
	    /*get msgtype packet extraction*/
            
            month = request[14];
            day = request[15];
            year = request[16];

	    if (request[63] == 7){
              start_hour = request[24];
              start_min = request[25];

	      /*search for event*/
              e = search(year,month,day,start_hour,start_min,DBlocation);

	      /*success*/
              response[13] = 1;

	      for (i = 14;i<24;i++){
	        response[i] = (uint16_t) e->name[i-14];
              }
                  
	    }else{ 
	      /*get number of events on date*/
              count = num_events(year,month,day,DBlocation);

	      /*success*/
              response[13] = 1;

	      response[14] = (uint16_t) count;

	      response[15] = 0;
	    }

	    break;
    case 6:
	    /*get total number of events*/
	    count = num_events(0,0,0,DBlocation);

	    /*success*/
            response[13] = 1;

            response[14] = (uint16_t) count;

	    response[15] = 0;
	    break;
    default:
	    fprintf(stderr,"non reconized msgtype");
	    exit(1);
  }

  /*bonus 2*/  
  if (start_hour > end_hour || 
		  (start_hour == end_hour && start_min > end_min)){
    for (j = 0;j<1024;j++){
      if (mainDB[j].events == NULL) continue;	    
      for (i = 0;i<=ListCount(mainDB[j].events);i++){
        e = ListCurr(mainDB[j].events);

        if (e == NULL) break;
        ListRemove(mainDB[j].events);
        free_event(e);
	if (mainDB[j].events == NULL) break;
        ListNext(mainDB[j].events);
      }
    }
    /*all events are removed and freed*/

    /*telling user they just cleared the entire database*/
    response[13] = 1;

    response[14] = (uint16_t) output6[0];
    response[15] = (uint16_t) output6[1];
    response[16] = (uint16_t) output6[2];
    response[17] = (uint16_t) output6[3];
    response[18] = (uint16_t) output6[4]; 
    response[18] = (uint16_t) output6[5];
    response[18] = (uint16_t) output6[6];
    response[18] = (uint16_t) output6[7];

  }

  return response; 

}

int main(int argc,char *argv[])
{
  char* port; /*port for server to listen on*/
  int s; /*main socket*/
  int b; /*return value of bind*/
  int l; /*return value of listen*/

  /*vars for getaddrinfo*/
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  /*checking arg count*/
  if (argc != 2){
    fprintf(stderr,"inccorect amount of args: only port required");
    exit(1);
  }

  /*saving port from cmd arg*/
  port = argv[1];

  /*setting up structs for getaddrinfo*/
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;     
  hints.ai_socktype = SOCK_STREAM; 
  hints.ai_flags = AI_PASSIVE;

  /*calling getaddrinfo*/
  status = getaddrinfo(NULL,port,&hints, &servinfo);

  /*checking return of getaddrinfo*/
  if (status != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  /*seting up main socket*/
  s = socket(servinfo->ai_family,
		  servinfo->ai_socktype,
		  servinfo->ai_protocol);

  /*checking return of socket*/
  if (s == -1){
    fprintf(stderr,"socket error:");
    exit(1);
  }

  /*bind socket to port*/
  b = bind(s,servinfo->ai_addr,servinfo->ai_addrlen);

  /*checking bind return value*/
  if (b == -1){
    fprintf(stderr,"bind error");
    exit(1);
  }

  /*calling listen on main socket*/
  l = listen(s,1);

  /*checking return value of listen*/
  if (l == -1){
    fprintf(stderr,"listen error");
    exit(1);
  }

  /*handling conenctions base on implmentation type*/
  handle_connections(s);

  /*freeing alocated structs*/
  freeaddrinfo(servinfo);

  return 0;
}
