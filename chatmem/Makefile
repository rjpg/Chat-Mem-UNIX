.PHONY: all
all: server client

server: server.c chat.h
	gcc -o server server.c -lreadline -ltermcap

client: client.c chat.h
	gcc -o client client.c -lreadline -ltermcap