#!/bin/sh
# gcc -Wall -g -D RPC lib/*.c src/serverc/*.c -lnsl -lpthread -o bin/serverC/server
gcc -Wall -g lib/*.c src/serverc/*.c -lnsl -lpthread -o bin/serverC/server
./bin/serverC/server -p 5001
# valgrind --log-file="valgrindrep.txt" ./bin/serverC/server -p 5001
sleep 20