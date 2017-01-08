#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/types.h>

#include <fcntl.h>
#include <dirent.h>
#include <strings.h>

#include "message.h"
#include "composer.h"
#include "tools.h"

#define RWX_ALL 0777

#define TMP_STR "tmp/"
#define TMP_SIZE 4

#define PIPE_SERVER_PATH "tmp/serv"

struct user {
	int id;

	char *id_str;
	int id_size;

	char pseudo_str[20];
	int pseudo_size;
};

int init() {
	if(access(TMP_STR, F_OK) != F_OK || access(PIPE_SERVER_PATH, F_OK) != F_OK)
		return -1;
	return 0;
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

int main(int argv, const char **argc){
	srand(time(NULL));
	int pipe_w, pipe_r;

	struct user *user = malloc(sizeof(user));

	if(init() == -1) {
		perror("error: initialisation\n");
		return -1; 
	}

	if((pipe_w = open(PIPE_SERVER_PATH, O_WRONLY)) == -1){
		perror("error: open server pipe");
		return -1;
	}

	user->pseudo_size = login(user->pseudo_str, sizeof(user->pseudo_str));

	//create read pipe
	char pipe_r_str[20];
	int pipe_r_size = createReadPipe(pipe_r_str, sizeof(pipe_r_str));

	// connection
	struct message *message = helo(user->pseudo_size, user->pseudo_str, pipe_r_size, pipe_r_str);
	char *composeMsg = composeMessage(message);
	write(pipe_w, composeMsg, BUFSIZ);

	if((pipe_r = open(pipe_r_str, O_RDONLY)) == -1){
		perror("error: open pipe read");
		return -1;
	}
	//wait for answer
	int run = 1;
	char buffer[BUFSIZ + 1];

	pid_t reader = fork();
	if(reader == -1)
		perror("fork");
	else if(reader == 0){
		fclose(stdin);
		int lus;
		do {
			if((lus = read(pipe_r, &buffer, BUFSIZ)) > 0) {
				printf("%s\n", buffer);
				struct message *message = parseMessage(buffer);

				if(strcmp(message->type, "BYEE") == 0){
					run = 0;
				}
				else if(strcmp(message->type, "BCST") == 0){
					printf("[%s] %s\n", message->segment->body, message->segment->next->body);
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
				else if(strcmp(message->type, "OKOK") == 0){
					printf("Connected.\n");
					user->id_str = message->segment->body;
					user->id_size = message->segment->size;
				}
				else if(strcmp(message->type, "BADD") == 0) {
					printf("error: connection -> BADD\n");
					run = 0;
				} 
				fflush(stdout);
			}
			else {
				run = 0;
				printf("blavla error end\n");
			}
			freeMessage(message);
		} while(run);
		close(pipe_r);
		remove(pipe_r_str);
	}
	else {
		do {
			char txt_str[255];
			printf("> ");
			scanf("%s", txt_str);

			int txt_size = 0;
			while(txt_str[txt_size]!='\0' && txt_size < 255)
				txt_size++;

			struct message *message;
			if(strcmp(txt_str, "quit") == 0)
				message = byee(user->id_size, user->id_str);
			else if(strcmp(txt_str, "shut") == 0)
				message = shut_client(user->id_size, user->id_str);
			else
				message = bcst_client(user->id_size, user->id_str, txt_size, txt_str);
			char *message_str = composeMessage(message);
			
			printf("%s\n", message_str);
			write(pipe_w, &message_str, message->length);

			freeMessage(message);
		} while(run);
		close(pipe_w);
	}

	printf("out\n");
	return 0;
}