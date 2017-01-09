#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tools.h"
#include "message.h"

struct message *newMessage(char *type){
	struct message* message = malloc(sizeof(struct message));
	message->type = type;
	message->length = 8;
	message->segment = NULL;
	return message;
}

struct segment *newSegment(int size, char *body){
	struct segment *segment = malloc(sizeof(struct segment));
	segment->size = size;
	segment->body = body;
	segment->next = NULL;
	return segment;
}

// ajoute segment
void addSegment(struct message *message, int size, char *body){
	message->length += size + 4;
	struct segment *segment_tmp = newSegment(size, body);

	if(message->segment == NULL)
		message->segment = segment_tmp;
	else {
		struct segment *segment = message->segment;
		while(segment->next != NULL)
			segment = segment->next;
		segment->next = segment_tmp;
	}
}

void printMessage(const struct message *message){
	printf("message:\n\tlength: %d\n\ttype: %s\n", message->length, message->type);
	struct segment *segment = message->segment;
	if(segment != NULL){
		printf("\tsegment:\n\t\tsize: %d\n\t\tbody: %s\n", segment->size, segment->body);
		while(segment->next != NULL){
			printf("\tsegment:\n\t\tsize: %d\n\t\tbody: %s\n", segment->next->size, segment->next->body);
			segment = segment->next;
		}
	}
}

void printSegment(const struct segment *segment){
	printf("segment:\n\tsize: %d\n\tbody: %s\n", segment->size, segment->body);
}

//TODO: advance checker
int verify(const struct message *message) {
	int length = 8;
	
	struct segment *segment = message->segment;
	while(segment != NULL) {
		length += segment->size + 4;
		segment = segment->next;
	}

	if(length == message->length)
		return 1;

	return 0;
}

struct message *parseMessage(const char *str){
	
	//get message length
	int length = atoi(splitStr(str, 0, 4));
	
	//get type
	struct message *message = newMessage(splitStr(str, 4, 8));

	//get segments 
	int index = 8;
	while(index < length){
		//get segment size
		int size = atoi(splitStr(str, index, index + 4));
		index += 4;

		//get segment body
		char *body = splitStr(str, index, index + size);
		index += size;

		addSegment(message, size, body);
	}
	
	return message;
}

char *composeMessage(const struct message *message) {
	if(verify(message) == 0){
		printf("error: message composition unvalid\n");
		return NULL;
	}

	char *messageStr = malloc(sizeof(char) * message->length);
	
	char length[4];
	itoa_formated(length, message->length, 4, '0');

	for (int i = 0; i < 4; ++i)
		messageStr[i] = length[i];

	int indexMessage = 4;
	for (int i = 0; i < 4; ++i)
		messageStr[indexMessage++] = message->type[i];

	struct segment *segment = message->segment;
	while(segment != NULL) {
		itoa_formated(length, segment->size, 4, '0');

		for (int i = 0; i < 4; ++i)
			messageStr[indexMessage++] = length[i];
		
		for (int i = 0; i < segment->size; ++i)
			messageStr[indexMessage++] = segment->body[i];
		
		segment = segment->next;
	}

	return messageStr;
}

void freeSegment(struct segment *segment){
	if(segment->next != NULL)
		freeSegment(segment->next);
	free(segment);
}

void freeMessage(struct message *message){
	if(message != NULL){
		if(message->segment != NULL)
			freeSegment(message->segment);
		free(message);
	}
}
