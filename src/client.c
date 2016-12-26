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

#define TMP "tmp/"
#define READDER "tmp/read"
#define RWX_ALL 0777

void init() {
	if(access(TMP, F_OK) != F_OK)
		mkdir(TMP, RWX_ALL);

	if(access(READDER, F_OK) != F_OK) 
		mkfifo(READDER, RWX_ALL);
}

void clean() {
	remove(READDER);
	rmdir(TMP);
}

int login(int pipe_w) {
	printf("Pseudo> ");

	char *pseudoStr;
	scanf("%s", pseudoStr);
	int pseudoSize = sizeof(pseudoStr);
	
	struct message *message = newMessage(4 + 4 + 4 + pseudoSize, "HELO");
	addSegment(message, pseudoSize, pseudoStr);
	
	char *messageStr = composeMessage(message);
//	write(pipe_w, &messageStr, message->length);
	return 1;
}

int main(int argv, const char **argc){
	init();
	int pipe_r = open(READDER, O_RDONLY), pipe_w;

	if(login(pipe_w) == 0) {
		perror("error: login");
		return -1;
	}

	int run = 0;
	do{
		char *messageStr;
		read(pipe_r, &messageStr, sizeof(messageStr));

		struct message *message = parseMessage(messageStr);
		//TODO
		/*
		if(strcmp(message->type, "OKOK") == 0){
		} 
		else if(strcmp(message->type, "BADD") == 0){
		} 
		else if(strcmp(message->type, "BYEE") == 0){
		} 
		else if(strcmp(message->type, "OKOK") == 0){
		} 
		else if(strcmp(message->type, "PRVT") == 0){
		} 
		else if(strcmp(message->type, "LIST") == 0){
		} 
		else if(strcmp(message->type, "SHUT") == 0){
		}
		*/
	} while(run);

	close(pipe_w);
	close(pipe_r);

	clean();
	return 0;
}