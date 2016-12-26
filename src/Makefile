CC=gcc
CFLAGS=-Wall
EXEC=main

all: $(EXEC)

#run: $(EXEC)
#	./main

client: message.o client.o
	$(CC) -o $@ $^

client.o: client.c message.h
	$(CC) -o $@ -c $< $(CFLAGS)

client.o: client.c message.h
	$(CC) -o $@ -c $< $(CFLAGS)

message.o: message.c
	$(CC) -o $@ -c $< $(CFLAGS)


clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)ls
