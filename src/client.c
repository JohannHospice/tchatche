#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "tools.h"
#include "message.h"
#include "user.h"
#include "composer.h"
#include "communication.h"

#define RWX_ALL 0777
#define TMP_SIZE 4
#define TMP_STR "tmp/"
#define PIPE_SERVER_PATH "tmp/serv"

int connectToServer() {
	if(access(TMP_STR, F_OK) != F_OK || access(PIPE_SERVER_PATH, F_OK) != F_OK)
		return -1;
	int pipe = open(PIPE_SERVER_PATH, O_WRONLY);
	if(pipe == -1){
		perror("error: open server pipe");
		return -1;
	}
	return pipe;
}

int login(char *pseudo_str, int pseudo_limit){
	printf("Pseudo> ");
	scanf("%s", pseudo_str);
	int pseudo_size = 0;
	while(pseudo_str[pseudo_size]!='\0' && pseudo_size < pseudo_limit)
		pseudo_size++;
	return pseudo_size;
}

int createReadPipe(char *pipe_str, int pipe_limit){
	//verifie si nexiste pas deja
	char rand_str[11]; 
	int rand_size = itoa(rand_str, rand());
	int pipe_size = TMP_SIZE + rand_size;
		
	strcpy(pipe_str, TMP_STR);
	strcat(pipe_str, rand_str);

	if(access(pipe_str, F_OK) != F_OK) 
		if(mkfifo(pipe_str, RWX_ALL) == -1){
			perror("error: mkfifo read\n");
			return -1;
		}

	if(pipe_size > pipe_limit)
		return pipe_limit;
	return pipe_size;
}

struct user *generateIdentity(int pipe_w){
	char pseudo_buffer[20];
	int pseudo_size = login(pseudo_buffer, sizeof(pseudo_buffer));
	char *pseudo_str = pseudo_buffer;
	//create read pipe
	char pipe_buffer[20];
	int pipe_size = createReadPipe(pipe_buffer, sizeof(pipe_buffer));
	char *pipe_str = pipe_buffer;
	// connection
	struct message *message = helo(pseudo_size, pseudo_str, pipe_size, pipe_str);
	send(message, pipe_w);
	freeMessage(message);

	int pipe_r = open(pipe_str, O_RDONLY);
	if(pipe_r == -1)
		return NULL;
	//wait for answer
	if((message = receive(pipe_r)) != NULL && strcmp(message->type, "OKOK") == 0)
		return newUserForClient(
			message->segment->size,
			message->segment->body,
			pseudo_size,
			pseudo_str,
			pipe_size,
			pipe_str,
			pipe_r);
	freeMessage(message);
	return NULL;
}

int main(int argv, const char **argc){
	srand(time(NULL));

	int pipe_w;
	if((pipe_w = connectToServer()) == -1) {
		perror("connectToServer");
		return -1;
	}

	struct user *user;
	if((user = generateIdentity(pipe_w)) == NULL){
		perror("generateIdentity");
		return -1;
	}
	printf("Connected.\n");

	int run = 1;
	pid_t reader = fork();
	if(reader == -1)
		perror("fork");
	else if(reader == 0){
		do {
			struct message *message;
			if((message = receive(user->pipe)) != NULL) {
				if(strcmp(message->type, "OKOK") == 0){
				}
				else if(strcmp(message->type, "BYEE") == 0){
					run = 0;
				}
				else if(strcmp(message->type, "BCST") == 0){
					if(strcmp(message->segment->body, user->pseudo_str) != 0)
						printf("\n[%s] %s", message->segment->body, message->segment->next->body);
				}
				else if(strcmp(message->type, "PRVT") == 0){
					printf("[private -> %s] %s\n", message->segment->body, message->segment->next->body);
				} 
				else if(strcmp(message->type, "LIST") == 0){
					printf("[list -> %s] %s\n", message->segment->body, message->segment->next->body);
				} 
				else if(strcmp(message->type, "SHUT") == 0){
					printf("[shut]\n");
					run = 0;
				}
				else if(strcmp(message->type, "BADD") == 0) {
					printf("BADD\n");
				}
				else {
					perror("unknow type");
				}
				fflush(stdout);
			}
			else {
				run = 0;
				perror("receive");
			}
			freeMessage(message);
		} while(run);
		close(user->pipe);
		remove(user->pipe_str);
	}
	else {
		do {
			printf("> ");
			char txt_str[255];
			fgets(txt_str, sizeof(txt_str), stdin);
			int txt_size = str_size(txt_str, 255);

			struct message *message;
			if(strcmp(txt_str, "quit\n") == 0){
				run = 0;
				message = byee(user->id_size, user->id_str);
			}
			else if(strcmp(txt_str, "shut\n") == 0)
				message = shut_client(user->id_size, user->id_str);
			else
				message = bcst_client(user->id_size, user->id_str, txt_size, txt_str);
			send(message, pipe_w);
			freeMessage(message);
		} while(run);
		close(pipe_w);
	}
	printf("exit\n");
	return 0;
}