#!/bin/sh
gcc -Wall -g lib/*.c src/serverrpc/*.c -lnsl -lpthread -o bin/serverRPC/storage
valgrind --log-file="valgrindrep.txt" ./bin/serverRPC/storage 
sleep 20
#./bin/serverRPC/storage > output.txt 
# valgrind --log-file="valgrindrep.txt" ./bin/serverC/server -p 5001 > output.txt 