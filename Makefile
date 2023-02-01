CC=gcc

all: server

server:
	${CC} server.c -o server

# client:
# 	${CC} client.c -o client

clean:
	rm -rf *.o server *.tar.gz
