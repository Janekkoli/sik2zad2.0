#!/bin/bash

# Start the server
.././kierki-serwer -p 2053 -f test3.txt -t 23 &

# Wait for the server to start
sleep 2

# Start the clients concurrently
.././kierki-klient -h localhost -p 2053 -N -4 -a &
.././kierki-klient -h localhost -p 2053 -E -4 -a &
.././kierki-klient -h localhost -p 2053 -S -4 -a &
.././kierki-klient -h localhost -p 2053 -W -4 -a &

# Wait for all background processes to finish
wait