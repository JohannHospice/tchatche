#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "user.h"
#include "message.h"
#include "communication.h"

/**
 * permet d'envoyer un message sur un pipe
 * si printMessage == 0 affiche le message envoyé
 * @param message
 * @param pipe
 * @param printMessage
 * @return
 */
int send(struct message *message, int pipe, int printMessage){
	char *message_str = composeMessage(message);
	int send = write(pipe, message_str, message->size);
	if(printMessage == 0)
		printf("[send to %d] %s\n", pipe, message_str);
	free(message_str);
	return send;
}
/**
 * permet d'envoyer un message meme à plusieurs utilisateurs
 * si printMessage == 0 affiche le message envoyé
 * @param message
 * @param users
 * @param size
 * @param printMessage
 */
void sendAll(struct message *message, struct user **users, int size, int printMessage){
	char *message_str = composeMessage(message);
	int i;
	for (i = 0; i < size; ++i)if(users[i] != NULL)
		write(users[i]->pipe, message_str, message->size);
	if(printMessage == 0)
		printf("[send to all] %s\n", message_str);
	free(message_str);
}

/**
 * permet d'envoyer un message meme à plusieurs utilisateurs
 * si printMessage == 0 affiche le message envoyé
 * @param pipe
 * @param printMessage
 * @return
 */
struct message *receive(int pipe, int printMessage){
	char length_str[4];
	int lus = read(pipe, &length_str, sizeof(length_str));
    length_str[4] = '\0';
	if(lus > 0){
		int length = atoi(length_str) - sizeof(length_str);
		char buffer[length];
		lus = read(pipe, &buffer, length);
        buffer[length] = '\0';
		if(lus > 0){
			if(printMessage == 0)
				printf("[receive] %s%s\n", length_str, buffer);
			return parseMessageWithLength(buffer, length);
		}
	}
	return NULL;
}