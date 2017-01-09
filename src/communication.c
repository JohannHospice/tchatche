#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "user.h"
#include "message.h"
#include "communication.h"

#define DEBUG 1

void send(struct message *message, int pipe){
	char *message_str = composeMessage(message);
	write(pipe, message_str, message->length);
	if(DEBUG)
		printf("[send to %d] %s", pipe, message_str);
	free(message_str);
}

struct message *receive(int pipe){
	char buffer[255];
	int lus = read(pipe, &buffer, sizeof(buffer));
	if(lus > 0){
		if(DEBUG)
			printf("[receive] %s", buffer);
		return parseMessage(buffer);
	}
	return NULL;
}

void sendAll(struct message *message, struct user **users, int size){
	char *message_str = composeMessage(message);
	for (int i = 0; i < size; ++i)
		write(users[i]->pipe, message_str, message->length);
	if(DEBUG)
		printf("[send to all] %s", message_str);
	free(message_str);
}