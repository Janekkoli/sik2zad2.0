#!/bin/bash

# Start the server
.././kierki-serwer -p 2022 -f rozgrywka.txt -t 23 &

# Wait for the server to start
sleep 2

# Start the clients concurrently
.././kierki-klient -h localhost -p 2022 -N -4 -a &
.././kierki-klient -h localhost -p 2022 -E -4 -a &
.././kierki-klient -h localhost -p 2022 -S -4 -a &
.././kierki-klient -h localhost -p 2022 -W -4 -a &

# Wait for all background processes to finish
wait