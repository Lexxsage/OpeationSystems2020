CC=gcc
FLAGS=-Wall

all: server client

server: bin server.c
	$(CC) $(FLAGS) server.c -o bin/server

client: bin client.c
	$(CC) $(FLAGS) client.c -o bin/client

bin:
	mkdir bin
