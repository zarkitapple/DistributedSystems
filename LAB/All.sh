#!/bin/sh
gnome-terminal -- bash -c "./touppercase.sh"
gnome-terminal -- bash -c "./rpcservice.sh"
sleep 1
gnome-terminal -- bash -c "./server.sh"
gnome-terminal -- bash -c "./client.sh"
gnome-terminal -- bash -c "./client.sh"
