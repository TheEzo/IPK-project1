SERVER=server.c
CLIENT=client.c
FLAGS=-lpthread
CC=-std=c11
default:
	gcc ${CC} ${SERVER} -o ipk-server
	gcc ${CC} ${CLIENT} -o ipk-client
move:
	scp ${SERVER} ${CLIENT} Makefile merlin:~/IPK
