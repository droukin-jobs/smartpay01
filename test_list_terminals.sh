#!/bin/bash

is_running=1
pgrep sp_server || is_running=0

if [[ $is_running -eq 0 ]]; then
	echo "sp_server should be running"
	exit 0
fi
#create 3 terminals


for i in $(seq 1 3); do
	curl -X POST http://localhost:8888/terminal
	sleep .5
done

#list terminals
curl http://localhost:8888/terminals
