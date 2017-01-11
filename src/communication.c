#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "user.h"
#include "message.h"
#include "communication.h"


int send(struct message *message, int pipe, int printMessage){
	char *message_str = composeMessage(message);
	int send = write(pipe, message_str, message->length);
	if(printMessage == 0)
		printf("[send to %d] %s\n", pipe, message_str);
	free(message_str);
	return send;
}

struct message *receive(int pipe, int printMessage){
	char buffer[255];
	int lus = read(pipe, &buffer, sizeof(buffer));
	if(lus > 0){
		if(printMessage == 0)
			printf("[receive] %s\n", buffer);
		return parseMessage(buffer);
	}
	return NULL;
}

void sendAll(struct message *message, struct user **users, int size, int printMessage){
	char *message_str = composeMessage(message);
	for (int i = 0; i < size; ++i)
		write(users[i]->pipe, message_str, message->length);
	if(printMessage == 0)
		printf("[send to all] %s\n", message_str);
	free(message_str);
}