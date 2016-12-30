CC=gcc
CFLAGS=-Wall

run_client: client
	clear && ./client

run_server: server
	clear && ./server

client: message.o client.o
	$(CC) -o $@ $^

server: message.o server.o
	$(CC) -o $@ $^

client.o: client.c message.h
	$(CC) -o $@ -c $< $(CFLAGS)

server.o: server.c message.h
	$(CC) -o $@ -c $< $(CFLAGS)

message.o: message.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o && rm -rf tmp/