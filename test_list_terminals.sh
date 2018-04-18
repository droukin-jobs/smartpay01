#!/bin/bash

#create 3 terminals

for i in $(seq 1 3); do
	curl http://localhost:8888/terminal/$i
	sleep .5
done

#list terminals
curl http://localhost:8888/terminals
