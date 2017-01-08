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
	int id;
};

int main(int argv, const char **argc){
	struct client *client = malloc(sizeof(struct client) * LIMIT_CLIENT);

	int pipe_r,
		nb_client = 0;

	char buffer[BUFSIZ + 1];

	if(init() == -1) {
		perror("error: initialisation\n");
		return -1;
	}

	printf("waiting for client\n");
	pipe_r = open(PIPE_SERVER_PATH, O_RDONLY);

	int run = 1;
	struct message *message;
	do {
		read(pipe_r, &buffer, BUFSIZ);
		printf("%s\n", buffer);

		if((message = parseMessage(buffer)) == NULL)
			continue;

		if(strcmp(message->type, "HELO") == 0) {
			client[nb_client].pseudo_str = message->segment->body;
			client[nb_client].pseudo_size = message->segment->size;
			client[nb_client].pipe = open(message->segment->next->body, O_WRONLY);

			char id_str[11];
			int id_size = itoa(id_str, nb_client);
			
			message = okok(id_size, id_str);
			char *message_str = composeMessage(message);

			write(client[nb_client].pipe, &message_str, message->length);

			nb_client++;
		} 
		else if(strcmp(message->type, "BYEE") == 0){
			int id = atoi(message->segment->body);

			write(client[id].pipe, &buffer, BUFSIZ);
			close(client[id].pipe);
			free(&client[id]);
		} 
		else if(strcmp(message->type, "BCST") == 0){
			int id = atoi(message->segment->body);

			message = bcst_server(
				client[id].pseudo_size, 
				client[id].pseudo_str, 
				message->segment->next->size, 
				message->segment->next->body);
			char *message_str = composeMessage(message);
			for (int i = 0; i < nb_client; ++i)
				write(client[i].pipe, &message_str, message->length);
		} 
		else if(strcmp(message->type, "PRVT") == 0){
			int id = atoi(message->segment->body);

			message = prvt_server(
				client[id].pseudo_size, 
				client[id].pseudo_str, 
				message->segment->next->next->size,
				message->segment->next->next->body);
			
			char *message_str = composeMessage(message);
			
			while(strcmp(message->segment->next->body, client[i].pseudo_str) && i < nb_client) i++;

			write(client[i].pipe, &message_str, message->length);
		} 
		else if(strcmp(message->type, "LIST") == 0){
		} 
		else if(strcmp(message->type, "SHUT") == 0){
		}
	} while(run);

	//prevenir tous clients
	return 0;
}