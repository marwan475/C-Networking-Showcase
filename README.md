# C-Networking-Showcase
Calendar server written in C on Linux, Makefile includes multiple CPU architectures, 3 different implementations, multithreaded, multiprocess, and single thread select-based approach.

## How to run

running make wil create 3 servers

./server_Approach port

./server_MultiThread port

./server_MultiProc port

run the server on one terminal/machine

run ./testing.bash hostname port

get version 2 and getall only return number of events 

check bash script for argument examples

## Client Server design

### Version 1.4

design for the client and 3 types of servers

### Connection requirments

Client:
        -server machine hostname
        -server machine port
        -client will have to get ipv4 adress from host name
        -client will have to create a socket to connect to server
        -client will send/recieve request/response using same connection
        -client will wait for response once a request is sent

Server:
        -port to listen on
        -server will create and bind socket to listen for connections
        -server will use connection to recieve and send responses

General:
        -All network communication will use the TCP protocol
        -All requests are sent by client
        -All responses are handled/sent by server

### Client Design

- CMD input format
	hostname:
	- name of machine server is running on
	port:
	- port to connect on
	myname:
	- client username
	- wqmax 12 chars
	msgtype:
	- type of msg you want to send
	date:
	- date of event
		format:
		monthday aka one 8 digt int
		ex:
		jan 5 2002
		20020105
	time:
	- time of event
		format:
		hourminute aka on 4 digit int
		ex:
		12:30
		1230
	Eventtitle:
	- name of event
	- max 9 chars

- Client will take command line arguments and check validity. 
- Argument format: hostname port myname msgtype [Attributes]
	- hostname is the name of the machine running the server
	- port is the port that the server is listining on
	- myname is the username of the client
	- msgtype is the type of msg you want to sent to the server
	- Attributes are any args the msg needs based on protocol
- Client will format the cmd args into a packet that follows the packet
format defined in the communication protocol
- Client will create a socket 
- Client will then use the socket to create a connection with the server
- Client will send the request to the server
- Client will wait for a response
- Client will print response output
- Client will close connection

### Server Design

- Server will take command line arguments and check validity
- Argument format: port
	- port is the port the server will bind on to listen
- Server will create a socket 
- Server will bind socket to port
- Server will wait for connections
- Since the server can recieve multiple connections, how these 
connections are handled will be version dependent and explained 
in each versions section
- Only one connection can access the database at a time 
this will be enforced per implmentation type
- once a conection gets its turn the server will preform the operation
requested
-main thread will be in a while loop waiting for a connection
-once a connection is recieved it will be sent to handle_con function
-each implmentation will handle this connection diferently 

### Operations:

- add
	- adds event to user list, if user struct doesnt exist
	then creates user struct, allocates list and adds to main list
	then sends success code response
- update
	- searches for user struct, searches for event, if event exists
	then update it, if it dose not then send error
- remove
	- searches for event in user struct, if exists remove it 
	if not then send error, also checks if user exists and sends
	error if not exist
- get
	- searches user list, if no time is given then retrive all events
	of the given date, if time is indicated search for requested 
	event, if nothing is found for ethier send an error
- getall
	- sends number of events in list, waits for individual requests
	from client
	

### Database:
-array of User structures
-Only one connection can access data base at time
-syncronization will be handled using semaphores
-when a users list is empty, it will be deleted
-array size is 1024,
-1024 is max amount of files a proc can open in linux
-max open connections is 1024

### Structs:
- User struct will contain user info
	- username
	- list of events
- Event struct will contain event details
	-event name
	-date
	-start time
	-end time

### Approach Based:

- single thread
- thread will be an infinite loop
- constantly checking using select the listning socket
- if there is a conenction waiting it will accept and add it to a 
list containing all connection sockets
-if nothing then it will loop through the list of connections 
calling select on each one
- if there is a request waiting it will procces the request
- if not it will repeat the loop
- connection socket will be removed once connection is lost
- since it is a single thread prog, sync is handled for us

### Multi-Threaded:

- main thread will accept connections
- every new connection socket will be added to list
- a new thread will be created for each connection
- each thread will wait for request and fufil them once they get turn 
with database
- threads will be destroyed when connection is lost
- threads will be synced using a mutex only one thread can access DB at
a time

### Multi-Process:

- main proccess will accept connections
- every new connection will fork a child proc
- child will comunicate with parent using pipes
- child will recieve request
- child will send request to parent 
- parent sends response back to child
- child sends response
- child will be killed after connection is killed
- parent proc will control access to DB ensuring only one proc accesses it
at a time

### Bonus features

- if there is a conflict in events the server will send a msg
to the client and not store the new event

- on every request to the server, the server will check if the end-time
is in the past, if it is it will clear the database
