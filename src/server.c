#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <dirent.h>
#include <string.h>
#include <time.h>

#include "message.h"
#include "tools.h"
#include "composer.h"

#define TMP_PATH "tmp/"
#define PIPE_SERVER_PATH "tmp/serv"
#define RWX_ALL 0777
#define LIMIT_CLIENT 20

int init(){
	if(access(TMP_PATH, F_OK) != F_OK)
		if(mkdir(TMP_PATH, RWX_ALL) == -1)
			return -1;
	if(access(PIPE_SERVER_PATH, F_OK) != F_OK)
		if(mkfifo(PIPE_SERVER_PATH, RWX_ALL) == -1)
			return -1;
	return 0;
}

struct client {
	char *pseudo_str;
	int pseudo_size;
	int pipe;
};

int main(int argv, const char **argc){

	if(init() == -1) {
		perror("error: initialisation\n");
		return -1;
	}

	printf("waiting for client\n");
	
	int run = 1,
		nb_client = 0,
		pipe_r = open(PIPE_SERVER_PATH, O_RDONLY);

	char buffer[BUFSIZ + 1];
	struct client *clients = malloc(sizeof(struct client) * LIMIT_CLIENT);
	do {
		read(pipe_r, &buffer, BUFSIZ);
		printf("[receive] %s\n", buffer);

		struct message *message = parseMessage(buffer);
		if(message == NULL)
			continue;

		if(strcmp(message->type, "HELO") == 0) {
			clients[nb_client].pseudo_str = message->segment->body;
			clients[nb_client].pseudo_size = message->segment->size;
			clients[nb_client].pipe = open(message->segment->next->body, O_WRONLY);
			
			printf("add new client: \n\tid: %d\n\tpseudo_str: %s\n\tpseudo_size: %d\n\tpipe_str: %s\n\tpipe: %d\n", 
				nb_client,
				clients[nb_client].pseudo_str, 
				clients[nb_client].pseudo_size, 
				message->segment->next->body,
				clients[nb_client].pipe);
			
			char id_str[11];
			int id_size = itoa(id_str, nb_client);
			
			message = okok(id_size, id_str);
			char *message_str = composeMessage(message);
			printf("send: %s\n", message_str);
			write(clients[nb_client].pipe, message_str, message->length);
			free(message_str);
			nb_client++;
		} 
		else if(strcmp(message->type, "BYEE") == 0){
			int id = atoi(message->segment->body);

			write(clients[id].pipe, buffer, BUFSIZ);
			close(clients[id].pipe);
			free(&clients[id]);
		} 
		else if(strcmp(message->type, "BCST") == 0){
			int id = atoi(message->segment->body);

			message = bcst_server(
				clients[id].pseudo_size, 
				clients[id].pseudo_str, 
				message->segment->next->size, 
				message->segment->next->body);
			char *message_str = composeMessage(message);
			printf("[send foreach] %s\n", message_str);
			for (int i = 0; i < nb_client; ++i)
				write(clients[i].pipe, message_str, message->length);
			free(message_str);
		} 
		else if(strcmp(message->type, "PRVT") == 0){
			int id = atoi(message->segment->body);

			message = prvt_server(
				clients[id].pseudo_size, 
				clients[id].pseudo_str, 
				message->segment->next->next->size,
				message->segment->next->next->body);
			
			char *message_str = composeMessage(message);
			int i =0;
			while(strcmp(message->segment->next->body, clients[i].pseudo_str) && i < nb_client) i++;

			printf("[send] %s\n", message_str);
			write(clients[i].pipe, message_str, message->length);
			free(message_str);
		} 
		else if(strcmp(message->type, "LIST") == 0){
		} 
		else if(strcmp(message->type, "SHUT") == 0){
		}
		freeMessage(message);
	} while(run);
	for (int i = 0; i < nb_client; ++i){
		//wait each
	}
	remove(PIPE_SERVER_PATH);
	remove(TMP_PATH);
	free(clients);
	printf("exit\n");
	return 0;
}