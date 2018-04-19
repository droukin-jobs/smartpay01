# smartpay01

TOC

### 1 Intro
### 2 Compile
### 3 Inastall
### 4 Usage
### 5 Testing
---
## 1 INTRO

This project was designed as a main requirement for Smartpay job test (started a 16.00 18-04-2018 NZT, will be handed in by 16.00 20-04-2018). 

At the moment the following features are complete:
 - Endpoint to create new terminals
 - Endpoint to list terminals
 - Endpoint to view terminal details
 - Each connection runs in it's own thread
 - Tests for helper functions
 - Documentation (you are reading it)

Partially complete features:
 - Terminal creation is done only by empty POST request to /terminal endpoint
 - Terminals have only dummy transactions

Not implemented features:
 - JSON POST input to create terminals
 - JSON POST input to update transactions
 - comprehensive main program testing

In order to fullfil the requirements i will need to provide a way of parsing JSON data and make sure the POST data processing is safe and reliable. Right now the POST processing is being worked upon but is still quite unstable. Also will need to develop a 

---
## 2 COMPILE

There are two libraries used in this project:
 
 a. libmicrohttpd 
  - https://www.gnu.org/software/libmicrohttpd/
  - provides web server API
 
 b. CUnit
  - http://cunit.sourceforge.net/
  - provides framework for testing

To compile main project use the makefile and GNU make in the projects root directory

To compile tests use the makefile in the tests/ directory

---
## 3 INSTALL

After successful compilation the  binary "sp\_server" will be available in the project root directory. Copy it wherever seems suitable.

---
## 4 USAGE

After launching the server will be available on port 8888. For GET requests you can us a web browser:
 ```
 localhost:8888
     /terminals 			will list available terminals
     /terminals/{id} 		will display particular terminal info
 ```
For POST requests use curl or similar programm:
```
 curl -X POST localhost:8888/terminal 	will create new terminal
```
---
## 5 TESTING

All runtime tests are in /\*.sh
- Multithreading performance can be tested with test\_list\_terminals.sh script by forking it several times
- There are makefile rules for runtime testing

All function tests are in file tests/test.c
- At the moment there are only helper function tests checking for validity of input and some boundary conditions.
- Use tests/makefile to compile the tests
- Use tests/test binary to run tests

Note: json\_float\_test fails due to floating point format not being carefully specified, but this function is not used in the project and there fore will not be fixed in the nearest future.

---
### PS. Thank you

I am thankful to Smartpay for allowing me to take this test. This was very challenging and intense experience. I plan on resolving the issues even after the test's due date - at least I will have something interesting to work on.

Kind regards,

Dmitri
