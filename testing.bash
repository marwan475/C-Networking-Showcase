#!/bin/bash

# testing multiple users adding events concurrently
# test if server can handle multi request at same time 
# expected response: 1 added 2 added
./client $1 $2 marwan add 20020422 1200 1430 birthday

./client $1 $2 mar add 20030521 1100 1530 birth 

# testing bonus 1
# testing adding samething twice conncurently
# tests if server handles race condition and can detect conflict
# excpected repsonse: 1 added 2 confl
./client $1 $2 wan add 20050627 1300 1730 bir 

./client $1 $2 wan add 20050627 1300 1730 bir 

# testing remove feature, 
# testing if server can handle race condition of 2 removes of same thing
# excpected response: 1 removed 2 errored
./client $1 $2 marwan remove 20020422 1200 

./client $1 $2 marwan remove 20020422 1200 

# testing update and get version 1
# excpected output: 1 updated 2 happyday
./client $1 $2 mar update 20030521 1100 1600 happyday 

./client $1 $2 mar get 20030521 1100 

# testing get version 2
# excpected output 1 1 2 removed 3 0
./client $1 $2 mar get 20030521 

./client $1 $2 mar remove 20030521 1100

./client $1 $2 mar get 20030521

#testing bonus 2 and getall
#excpected output: 1 1 2 cleared 3 0  

./client $1 $2 wan getall

./client $1 $2 wan add 20050627 1300 1200 bir 

./client $1 $2 wan getall
