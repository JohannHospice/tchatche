#define DEBUG 1
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
	struct message *message_send = NULL;
	struct message *message_receive = NULL;
	do {
		if((message_receive = receive(pipe_r, 0)) == NULL)
			continue;
		if(strcmp(message_receive->type, HELO) == 0) {
			printf("[new client]\n");			
				
			char id_str[11];
			int id_size = itoa(id_str, nb_client);
			clients[nb_client] = newUser(
				nb_client, 
				id_size,
				id_str,
				message_receive->segment->body_size,
				message_receive->segment->body_str,
				message_receive->segment->next->body_size,
				message_receive->segment->next->body_str,
				open(message_receive->segment->next->body_str, O_WRONLY));
            printUser(clients[nb_client]);
			message_send = okok(clients[nb_client]->id_size, clients[nb_client]->id_str);
			send(message_send, clients[nb_client]->pipe, 0);
			nb_client++;
		} 
		else{
			int id = atoi(message_receive->segment->body_str);
			if(strcmp(message_receive->type, BYEE) == 0){
				message_send = byee(clients[id]->id_size, clients[id]->id_str);
				send(message_send, clients[id]->pipe, 0);
				close(clients[id]->pipe);
				free(clients[id]);
			} 
			else if(strcmp(message_receive->type, BCST) == 0){
				message_send = bcst_server(
					clients[id]->pseudo_size, 
					clients[id]->pseudo_str, 
					message_receive->segment->next->body_size,
					message_receive->segment->next->body_str);
				sendAll(message_send, clients, nb_client, 0);
			} 
			else if(strcmp(message_receive->type,PRVT)== 0){
				int ok, i = 0;

                while(i < nb_client
					&& (ok = strcmp(message_receive->segment->next->body_str, clients[i]->pseudo_str)) != 0){
                    printf("%s / %s", message_receive->segment->next->body_str, clients[i]->pseudo_str);
                    i++;
                }
				if(ok == 0){
					message_send = prvt_server(
						clients[id]->pseudo_size, 
						clients[id]->pseudo_str, 
						message_receive->segment->next->next->body_size,
						message_receive->segment->next->next->body_str);
					send(message_send, clients[i]->pipe, 0);
				}
				else{
					message_send = badd();
					send(message_send, clients[id]->pipe, 0);
				}
				freeMessage(message_send);
			}
			else if(strcmp(message_receive->type, LIST) == 0){
				char nb_client_str[11];
				int nb_client_size = itoa(nb_client_str, nb_client);
				for (int i = 0; i < nb_client; ++i){
					message_send = list_server(
						nb_client_size,
						nb_client_str,
						clients[i]->pseudo_size, 
						clients[i]->pseudo_str);
					send(message_send, clients[id]->pipe, 0);
				}
			} 
			else if(strcmp(message_receive->type, SHUT) == 0){
				run = 0;
				message_send = shut_server(
					clients[id]->pseudo_size, 
					clients[id]->pseudo_str);
				sendAll(message_send, clients, nb_client, 0);
			}
			else {
				message_send = badd();
				send(message_send, clients[id]->pipe, 0);
			}
		}
		freeMessage(message_receive);
		freeMessage(message_send);
	} while(run);
	for (int i = 0; i < nb_client; ++i){
		close(clients[i]->pipe);
	}
	remove(PIPE_SERVER_PATH);
	printf("[exit]\n");
	return 0;
}