#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "tools.h"
#include "message.h"
#include "user.h"
#include "composer.h"
#include "communication.h"

#define RWX_ALL 0777
#define TMP_PATH "tmp/"
#define PIPE_SERVER_PATH "tmp/serv"
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

int main(int argv, const char **argc){

	if(init() == -1) {
		perror("error: initialisation\n");
		return -1;
	}

	printf("waiting for client\n");
	
	int run = 1,
		nb_client = 0,
		pipe_r = open(PIPE_SERVER_PATH, O_RDONLY);

	struct user *clients[LIMIT_CLIENT];
	do {
		struct message *message = receive(pipe_r);
		if(message == NULL){
			continue;
		}
		if(strcmp(message->type, "HELO") == 0) {
			printf("[new client]\n");			
				
			clients[nb_client] = newUser(
				nb_client, 
				message->segment->size, 
				message->segment->body, 
				message->segment->next->size, 
				message->segment->next->body,
				open(message->segment->next->body, O_WRONLY));

			printUser(clients[nb_client]);
			
			
			message = okok(clients[nb_client]->id_size, clients[nb_client]->id_str);
			send(message, clients[nb_client]->pipe);
			nb_client++;
		} 
		else{
			int id = atoi(message->segment->body);
			if(strcmp(message->type, "BYEE") == 0){
				message = byee(clients[id]->id_size, clients[id]->id_str);
				send(message, clients[id]->pipe);
				close(clients[id]->pipe);
				free(clients[id]);
			} 
			else if(strcmp(message->type, "BCST") == 0){
				message = bcst_server(
					clients[id]->pseudo_size, 
					clients[id]->pseudo_str, 
					message->segment->next->size, 
					message->segment->next->body);
				sendAll(message, clients, nb_client);
			} 
			else if(strcmp(message->type, "PRVT") == 0){
				message = prvt_server(
					clients[id]->pseudo_size, 
					clients[id]->pseudo_str, 
					message->segment->next->next->size,
					message->segment->next->next->body);
				
				int i = 0;
				while(strcmp(message->segment->next->body, clients[i]->pseudo_str) && i < nb_client) i++;
				send(message, clients[i]->pipe);
			} 
			else if(strcmp(message->type, "LIST") == 0){
				char n_str[11];
				int n_size = itoa(n_str, sizeof(n_str));

				for (int i = 0; i < nb_client; ++i){
					message = list_server(
						n_size,
						n_str,
						clients[i]->pseudo_size, 
						clients[i]->pseudo_str);
					send(message, clients[id]->pipe);
				}
			} 
			else if(strcmp(message->type, "SHUT") == 0){
				run = 0;
				message = shut_server(
					clients[id]->pseudo_size, 
					clients[id]->pseudo_str);
				sendAll(message, clients, nb_client);
			}
		}
		freeMessage(message);
	} while(run);
	for (int i = 0; i < nb_client; ++i){
		close(clients[i]->pipe);
	}
	remove(PIPE_SERVER_PATH);
	printf("exit\n");
	return 0;
}