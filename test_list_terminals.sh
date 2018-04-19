#!/bin/bash
MAX_TERMINALS=100

is_running=1
pgrep sp_server || is_running=0

if [[ $is_running -eq 0 ]]; then
	echo "sp_server should be running"
	exit 0
fi
#create 100 terminals


for i in $(seq 1 $MAX_TERMINALS); do
	echo -n "$i "
	curl -X POST http://localhost:8888/terminal
	sleep .1
done

#list terminals
curl http://localhost:8888/terminals
