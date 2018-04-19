# smartpay01

TOC

1. Intro
2. Compile
3. Inastall
4. Usage

1. INTRO

This project was designed as a main requirement for Smartpay job test (started a 16.00 18-04-2018 NZT, will be handed in by 16.00 20-04-2018). 

At the moment the following features are complete:
 - Endpoint to create new terminals
 - Endpoint to list terminals
 - Endpoint to view terminal details
 - Tests for helper functions
 - Documentation (you are reading it)

Partially complete features:
 - Terminal creation is done only by empty POST request to /terminal endpoint
 - Terminals have only dummy transactions

Not implemented features:
 - JSON POST input to create terminals
 - JSON POST input to update transactions
 - comprehencive main program testing

2. COMPILE

There are two libraries used in this project:
 a. libmicrohttpd 
  - https://www.gnu.org/software/libmicrohttpd/
  - provides web server API
 b. CUnit
  - http://cunit.sourceforge.net/
  - provides framework for testing

To compile main project use the makefile and GNU make in the projects root directory
To compile tests use the makefile in the tests/ directory

3. INSTALL

After successful compilation the  binary "spserver" will be available in the project root directory. Copy it wherever seems suitable.

4. USAGE

After launching the server will be available on port 8888. For GET requests you can us a web browser:
 localhost:8888
     /terminals 			will list available terminals
     /terminals/{id} 		will display particular terminal info
For POST requests use curl or similar programm:
 curl -X POST localhost:8888/terminal 	will create new terminal


