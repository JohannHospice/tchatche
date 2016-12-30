#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <strings.h>
#include <time.h>
#include <stdlib.h>

#include "message.h"

#define RWX_ALL 0777
#define TMP "tmp/"
#define PIPE_SERVER_PATH "tmp/serv"
struct segment {
	int size;
	char *body;
	struct segment *next;
};

struct message {
	int length;
	char *type;
	struct segment *segment;
};

int init() {
	if(access(TMP, F_OK) != F_OK)
		if(mkdir(TMP, RWX_ALL)== -1)
			return -1; 
	return 0;
}

void log_str(char *str){
	printf("// %s\n", str);
}

void log_int(int i){
	printf("// %d\n", i);
}

int main(int argv, const char **argc){
	int pipe_w, pipe_r, id;
	char buffer[BUFSIZ + 1];
	if(init() == -1){
		perror("error: initialisation\n");
		return -1; 
	}

	// login
	printf("Pseudo> ");

	char pseudo[20] = "";
	scanf("%s", pseudo);
	int pseudo_size = sizeof(pseudo);
	
	log_str(pseudo);
	log_int(pseudo_size);

	int pipe_size = 6 + pseudo_size;

	//open write sever pipe 
	if(access(PIPE_SERVER_PATH, F_OK) != F_OK) {
		perror("error: no server found\n");
		return -1;
	}
	log_str("success: pipe write exist");

	if((pipe_w = open(PIPE_SERVER_PATH, O_WRONLY)) == -1){
		perror("error: open pipe write");
		return -1;
	}

	//create read pipe
	char *pipe_r_path = malloc(sizeof(char) * pipe_size);
	strcpy(pipe_r_path, TMP);
	strcat(pipe_r_path, pseudo);
	log_str(pipe_r_path);

	if(access(pipe_r_path, F_OK) != F_OK) 
		if(mkfifo(pipe_r_path, RWX_ALL) == -1){
			perror("error: mkfifo read\n");
			return -1;
		}
	log_str("success: pipe read exist");

	if((pipe_r = open(pipe_r_path, O_RDONLY)) == -1){
		perror("error: open pipe read");
		return -1;
	}

	// connection
	//compose message
	struct message *message = newMessage("HELO");
	addSegment(message, pseudo_size, pseudo);
	addSegment(message, pipe_size, pipe_r_path);
	char *composeMsg = composeMessage(message);
	
	log_str(composeMsg);
	//send
	write(pipe_w, &composeMsg, BUFSIZ);

	//wait for answer
	read(pipe_r, &buffer, BUFSIZ);
	log_str(buffer);
	message = parseMessage(buffer);
	
	if(strcmp(message->type, "OKOK") == 0){
		printf("Connected.\n");
		id = atoi(message->segment->body);
	}
	else if(strcmp(message->type, "BADD") == 0){
		printf("error: connection -> BADD\n");
		return -1;
	}
	else {
		printf("error: connection\n");
		return -1;
	}

	int run = 1;
	do{
		read(pipe_r, &buffer, BUFSIZ);

		struct message *message = parseMessage(buffer);
		//TODO
		if(strcmp(message->type, "BYEE") == 0){
			run = 0;
		}
		else if(strcmp(message->type, "BCST") == 0){
		}
		else if(strcmp(message->type, "PRVT") == 0){
		} 
		else if(strcmp(message->type, "LIST") == 0){
		} 
		else if(strcmp(message->type, "SHUT") == 0){
		}
	} while(run);

	close(pipe_w);
	close(pipe_r);
	remove(PIPE_SERVER_PATH);

	return 0;
}