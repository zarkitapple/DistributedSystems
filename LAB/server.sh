#!/bin/sh
gcc -Wall -g lib/lines.c lib/LinkedList.c src/serverc/server.c -lpthread -o bin/serverC/server
./bin/serverC/server -p 5000
