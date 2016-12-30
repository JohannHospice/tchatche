#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <dirent.h>
#include <strings.h>
#include <time.h>
#include <stdlib.h>
#include "message.h"

#define TMP_PATH "tmp/"
#define PIPE_SERVER_PATH "tmp/serv"
#define RWX_ALL 0777

int init(){
	if(access(TMP_PATH, F_OK) != F_OK)
		if(mkdir(TMP_PATH, RWX_ALL) == -1)
			return -1;
	if(access(PIPE_SERVER_PATH, F_OK) != F_OK)
		if(mkfifo(PIPE_SERVER_PATH, RWX_ALL) == -1)
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
	int pipe_r, 
		pipe_w_size = 0;
	int *pipe_w = malloc(sizeof(int) * 0); 
	char buffer[BUFSIZ + 1];

	if(init() == -1) {
		perror("error: initialisation\n");
		return -1;
	}

	pipe_r = open(PIPE_SERVER_PATH, O_RDONLY);
	log_int(pipe_r);

	char *pseudo
	int run = 1;
	do {
		read(pipe_r, &buffer, BUFSIZ);
		
		struct message *message;

		if((message = parseMessage(buffer)) == NULL)
			continue;

		if(strcmp(message->type, "HELO") == 0){
			//get pseudo: message->segment->body);
			//get pipe path: message->segment->next->body);
			//open new pipe w

			char *id = itoa(pipe_w_size);
			
			message = newMessage("OKOK");
			addSegment(message, sizeof(id), id);

			buffer = composeMessage(message);
			write(pipe_w[id], buffer, BUFSIZ);

			pipe_w_size++;
		} 
		else if(strcmp(message->type, "BYEE") == 0){
			int id = atoi(message->segment->body);
			write(pipe_w[id], buffer, BUFSIZ);	
			close(pipe_w[id]);
			pipe_w[id] = NULL;
			pseudo[id] = NULL;
		} 
		else if(strcmp(message->type, "BCST") == 0){
		} 
		else if(strcmp(message->type, "PRVT") == 0){
		} 
		else if(strcmp(message->type, "LIST") == 0){
		} 
		else if(strcmp(message->type, "SHUT") == 0){
		}
		if(run == 0) {
			//prevenir tous clients
		}
		log_str("turn");
	} while(run);

	return 0;
}