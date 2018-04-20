#!/bin/bash

#test  some preset scenarios with known outcome
# start server just before running the test

#check server
is_running=1
pgrep sp_server || is_running=0

if [[ $is_running -eq 0 ]]; then
	echo "sp_server should be running"
	exit 0
fi

#set some vars

server=http://localhost:8888
MAX_TERMINALS=100

# Scenario 0
# query /terminals - should get an error
# query /terminals/0 - should get an error

#begin
scenario=0 
t=0
echo "Scenario $scenario ========"
echo -n "   Test $t ... "
curl -X GET -ss "$server/terminals" |grep -q "error" && echo OK || echo FAIL
sleep .5
t=$(($t + 1))
echo -n "   Test $t ... "
curl -X GET -ss "$server/terminals/0" |grep -q "error" && echo OK || echo FAIL
sleep .5
#end

# Scenario 1
# test 0: create 1 terminal by POSTing to /terminal
# test 1: query /terminals - there should be one terminal with id 0
# test 2: query /terminals/0 - should be no error

#begin
scenario=$(($scenarioi + 1))
t=0
echo "Scenario $scenario ========"
echo -n "   Test $t ... "
curl -X POST -ss $server/terminal |grep "terminalID" -q && echo OK || echo FAIL
sleep .1
t=$(($t + 1))
echo -n "   Test $t ... "
curl -X GET -ss "$server/terminals" |grep "TerminalID"|grep -q "0" && echo OK || echo FAIL
sleep .1
t=$(($t + 1))
echo -n "   Test $t ... "
curl -X GET -ss "$server/terminals/0" |grep "error" -q && echo FAIL || echo OK
sleep .1
# end

# Scenario 2
# create 10 more terminals by POSTing to /terminal 10 times
# test 0: query /terminals - there should be 11 terminals
# test 1: query /terminals/10 - should be no error
# test 2: query /terminals/11 - should be error

#begin
scenario=$(($scenario + 1))
t=0
echo "Scenario $scenario ========"
echo -n "   Test $t ... "
for i in $(seq 1 10); do
	curl -X POST -ss $server/terminal > /dev/null
	sleep .1
done
lines=$(curl -X GET -ss $server/terminals|grep "TerminalID"|wc -l)
if [[ $lines -eq 11 ]]; then
	echo OK
else
	echo FAIL
fi
t=$(($t + 1))
echo -n "   Test $t ... "
curl -X GET -ss "$server/terminals" |grep "TerminalID"|grep -q "10" && echo OK || echo FAIL
sleep .1
t=$(($t + 1))
echo -n "   Test $t ... "
curl -X GET -ss "$server/terminals" |grep "TerminalID"|grep -q "11" && echo FAIL || echo OK
sleep .1
#end

# Scenario 3
# attepmt to create MAX_TERMINALS more terminals
# test 0: make sure there are only MAX_TERMINALS
# test 1: query for terminal id MAX_TERMINALS - should FAIL
# test 2: query for terminal id (MAX_TERMINALS - 1) - should be OK

#begin
scenario=$(($scenario + 1))
t=0
echo "Scenario $scenario ========"
echo -n "   Test $t ... "
for i in $(seq 1 $MAX_TERMINALS); do
	curl -X POST -ss $server/terminal > /dev/null
	sleep .1
done
lines=$(curl -X GET -ss $server/terminals|grep "TerminalID"|wc -l)
if [[ $lines -eq $MAX_TERMINALS ]]; then
	echo OK
else
	echo FAIL
fi
t=$(($t + 1))
echo -n "   Test $t ... "
curl -X GET -ss "$server/terminals" |grep "TerminalID"|grep -q "$MAX_TERMINALS" && echo FAIL || echo OK
sleep .1
t=$(($t + 1))
echo -n "   Test $t ... "
curl -X GET -ss "$server/terminals" |grep "TerminalID"|grep -q "$(($MAX_TERMINALS - 1))" && echo OK || echo FAIL
sleep .1
#end
