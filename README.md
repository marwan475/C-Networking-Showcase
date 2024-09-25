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

## Calender server client communication protocol

### Version 2.1

how the calender server and client will communicate, packet format and message types.

### Packet Format

- size 64 bytes
### HEADER
- 14 bytes
- first 14 bytes are for header info
- first 13 bytes of header is for username of client
- byte 14 is for msgtype or status depending on sender
- each msgtype and status has its own corisponding int code
### DATA
- 50 bytes
- Atributes are 10 bytes each
- max 5 atributes
- server will read Attributes based on msgtype 

### Int codes

- empty/null = 0
- success = 1
- add = 2
- remove = 3
- update = 4
- get = 5
- getall = 6
- error = 7
- not found = 8

### Data Format

- each attribute has 10 bytes
- max 10 bytes per attribute for client packets
- server has 50 bytes to send response
- date, first byte is month, second byte is day, third byte is year
- time, first byte is hour, second byte is minutes (24 hour clock)
- Event title, 10 bytes, each byte rperesents a letter, max 10 char title
- Server has 50 bytes to send any raw data

### Messages

*server will read attributes in the order listed for msg
*client will read Data starting at first byte of DATA (byte 15)

*client will use DATA to send attributes
*server will use DATA to send requested data
*server will use DATA to comunicate errors
*server sends username of client in header, client uses this to ensure
it is the correct reciever of the packet

### Add-Event-Msg:

	Function: Add event to calender for user

	Attributes: 
		msgtype = add 
		date: date of the event
		start-time: start time of event
		end-time: end time of event
		Event-title: name of event
	Request:
		client sends request to server to add event to 
		personal calender
	Response:
		server sends a response if operation was successful
		or if there was a failure
 
### Remove-Event-Msg:

        Function: Remove event from calender for user 

        Attributes:
		msgtype = remove
		date: date of the event
                start-time: start time of event
        Request:
		client sends request to server to remove event from
		personal calender
        Response:
		server sends a response if event exist or not and 
		if operation was successful

### Update-Event-Msg:

        Function: Update an existing event from the calender of the user

        Attributes:
		msgtype = update
                date: date of the event
                start-time: start time of event
                end-time: end time of event
                Event-title: name of event
        Request:
		client sends request to server to update event from
		personal calender, ethier the end time or event type
		has to be diffrent
        Response:
		server sends a response if operation was successful
		or if there was a failure such as event not existing

### Get-Event1-Msg:

        Function: Gets the event type of the event requested

        Attributes:
		msgtype = get
		date: date of event
		time: start time of event
        Request:
		client sends request to get and event type given the
		date and start time of that event
        Response:
		server sends a response containing the event type if
		the event exist, if not it will send an error
### Get-Event2-Msg:

        Function: gets all event types of the day

        Attributes:
		msgtype = get
		date: date of event
        Request:
		client sends a request to get the event types of all
		events on the given day
        Response:
		server sends a response with all the number of
	        events on that day if any exist, if not then the
		server sends a 0

### GetAll-Events-Msg:

        Function: 
		first get the number of events for the user, if
		it is non zero the client requests each event 
		individually with 2 second delay 
        Attributes:
		msgtype = getall
        Request:
		client sends request for all events, then sends 
		individual requests for each event
        Response:
		server sends response containing number of events,
		then fufils each individual request, if no events then
		server sends 0
			
### Examples

	client wants to add event birthday
	april 22 2024, 16:00 - 18:30

        Client request packet
                HEADER
                - username (13 bytes)
                - 2 (1 byte)
                DATA
                -4(1byte),22(1byte),24(1byte)(7bytes empty)
		-16(1byte),0(1byte)(8bytes empty)
		-18(1byte),30(1byte)(8bytes empty)
		-birthday(8bytes)(2bytes empty)


	server responds with success

	Server response packet
		HEADER
		- username of client (13 bytes)
		- 1 (1 byte)
		DATA
		- 50bytes empty
