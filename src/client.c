#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tools.h"
#include "message.h"
#include "user.h"
#include "composer.h"
#include "communication.h"

#define RWX_ALL 0777
#define TMP_STR "tmp/"
#define PIPE_SERVER_PATH "tmp/serv"
#define TMP_SIZE 4

#define SEPARATOR_INPUT "> "
#define INPUT_LIST "list"
#define INPUT_SHUT "shut"
#define INPUT_BYEE "byee"
#define INPUT_PRIVATE "private:"

// verification de presence du server et connection
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
void clearInputBuffer(){
    int c;
    do c = getchar(); while (c != '\n' && c != EOF);
}

//saisie du pseudo
int login(char *pseudo_str, int pseudo_limit){
	printf("Pseudo> ");
	scanf("%s", pseudo_str);
	clearInputBuffer();
	//	fgets(pseudo_str, pseudo_limit, stdin);
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
		if(mkfifo(pipe_str, RWX_ALL) == -1)
			return -1;

	if(pipe_size > pipe_limit)
		return pipe_limit;
	return pipe_size;
}

struct user *generateIdentity(int pipe_w){
    struct user *user = NULL;

    char pseudo_str[20];
    int pseudo_size = login(pseudo_str, 20);
    //create read pipe
    char pipe_str[20];
    int pipe_size = createReadPipe(pipe_str, 20);

    // connection
    struct message *message = helo(pseudo_size, pseudo_str, pipe_size, pipe_str);
    send(message, pipe_w, -1);
    freeMessage(message);

    int pipe_r = open(pipe_str, O_RDONLY);
    if(pipe_r == -1) {
        return NULL;
    }
    //wait for answer
    if((message = receive(pipe_r, -1)) != NULL && strcmp(message->type, OKOK) == 0) {
        user = newUser(
                atoi(message->segment->body_str),
                message->segment->body_size,
                message->segment->body_str,
                pseudo_size,
                pseudo_str,
                pipe_size,
                pipe_str,
                pipe_r);
    }
    freeMessage(message);
    return user;
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
			if((message = receive(user->pipe, -1)) != NULL) {
				if(strcmp(message->type, OKOK) == 0){
					printf("%s", SEPARATOR_INPUT);
				}
				else  if(strcmp(message->type, BCST) == 0){
					printf("[%s] %s\n%s", message->segment->body_str, message->segment->next->body_str, SEPARATOR_INPUT);
				}
				else if(strcmp(message->type, PRVT) == 0){
					printf("[private:%s] %s\n%s", message->segment->body_str, message->segment->next->body_str, SEPARATOR_INPUT);
				}
				else if(strcmp(message->type, LIST) == 0){
					printf("[list] %s\n%s", message->segment->next->body_str, SEPARATOR_INPUT);
				} 
				else if(strcmp(message->type, BADD) == 0) {
					printf("[badd]\n%s", SEPARATOR_INPUT);
				}
				else if(strcmp(message->type, SHUT) == 0){
					printf("[shut]\n");
					run = 0;
				}
				else if(strcmp(message->type, BYEE) == 0){
					printf("[byee]\n");
					run = 0;
				}
				else {
					perror("receive");
					run = 0;
				}
				fflush(stdout);
				freeMessage(message);
			}
			else {
				perror("receive");
				run = 0;
			}
		} while(run);
		close(user->pipe);
		remove(user->pipe_str);
	}
	else {
		struct message *message;
		do {
			printf("%s", SEPARATOR_INPUT);
			char txt_str[255];
			if(fgets(txt_str, sizeof(txt_str), stdin) != NULL){
				int txt_size = str_size(txt_str, sizeof(txt_str)) - 1;
                txt_str[txt_size] = '\0';

				if(strcmp(txt_str, INPUT_BYEE) == 0){
					message = byee(user->id_size, user->id_str);
					run = 0;
				}
				else if(strcmp(txt_str, INPUT_SHUT) == 0) {
					message = shut_client(user->id_size, user->id_str);
					run = 0;
				}
				else if(strcmp(txt_str, INPUT_LIST) == 0){
					message = list_client(user->id_size, user->id_str);
				}
				else if(str_start_with(txt_size, txt_str, 8, INPUT_PRIVATE) == 0){
					char pseudo_str[20];
					int pseudo_size = slice_to_char(txt_size, txt_str, sizeof(pseudo_str), pseudo_str, 8, ' ');
					int from = pseudo_size + 9;
					
					int private_txt_size = txt_size - from;
					char *private_txt_str = malloc(sizeof(char) * private_txt_size);
				
					slice(txt_str, private_txt_str, from, txt_size);
				
					message = prvt_client(
						user->id_size,
						user->id_str, 
						pseudo_size,
						pseudo_str,
						private_txt_size,
						private_txt_str);
				}
				else
					message = bcst_client(user->id_size, user->id_str, txt_size, txt_str);
				send(message, pipe_w, -1);
				freeMessage(message);
			}
		} while(run);
		close(pipe_w);
		wait(NULL);
		printf("[exit]\n");
	}
	return 0;
}
