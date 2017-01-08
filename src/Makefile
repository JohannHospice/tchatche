CC=gcc
CFLAGS=-Wall

CLIENT=client
SERVER=server
TEST=test
all: $(CLIENT) $(SERVER)

client: tools.o message.o composer.o client.o
	$(CC) -o $@ $^
server: tools.o message.o composer.o server.o
	$(CC) -o $@ $^
composer: tools.o message.o composer.o 
	$(CC) -o $@ $^
test: tools.o message.o composer.o test.o
	$(CC) -o $@ $^

# Object
client.o: client.c message.h
	$(CC) -o $@ -c $< $(CFLAGS)
server.o: server.c message.h
	$(CC) -o $@ -c $< $(CFLAGS)

tools.o: tools.c
	$(CC) -o $@ -c $< $(CFLAGS)
message.o: message.c
	$(CC) -o $@ -c $< $(CFLAGS)
composer.o: composer.c tools.h message.h
	$(CC) -o $@ -c $< $(CFLAGS)

test.o: test.c tools.h message.h composer.h
	$(CC) -o $@ -c $< $(CFLAGS)

run_client: client
	clear && ./client
run_server: server
	clear && ./server

clean:
	rm -rf *.o tmp/ 

mrproper: clean
	rm -rf $(CLIENT) $(SERVER) $(TEST)