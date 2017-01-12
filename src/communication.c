#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "user.h"
#include "message.h"
#include "communication.h"


int send(struct message *message, int pipe, int printMessage){
	char *message_str = composeMessage(message);
	int send = write(pipe, message_str, message->size);
	if(printMessage == 0)
		printf("[send to %d] %s\n", pipe, message_str);
	free(message_str);
	return send;
}

void sendAll(struct message *message, struct user **users, int size, int printMessage){
	char *message_str = composeMessage(message);
	for (int i = 0; i < size; ++i)
		write(users[i]->pipe, message_str, message->size);
	if(printMessage == 0)
		printf("[send to all] %s\n", message_str);
	free(message_str);
}

struct message *receive(int pipe, int printMessage){
	char length_str[4];
	int lus = read(pipe, &length_str, sizeof(length_str));

	if(lus > 0){
		int length = atoi(length_str) - sizeof(length_str);
		char buffer[length];
		lus = read(pipe, &buffer, length);
		if(lus > 0){
			if(printMessage == 0)
				printf("[receive] %s%s\n", length_str, buffer);
			return parseMessageWithLength(buffer, length);
		}
	}
	return NULL;
}