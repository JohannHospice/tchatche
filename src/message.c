#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct segment {
	int size;
	char *body;
	struct segment *next;
};

struct message {
	char *type;
	struct segment *segment;
};

struct message *newMessage(char *type){
	struct message* message = malloc(sizeof(struct message));
	message->type = type;
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

// recupere et enleve dernier segment
struct segment *removeLastSegment(struct message *message){
	//first
	if(message->segment == NULL)
		return NULL;
	struct segment * segment = message->segment;
	//second
	if (message->segment->next == NULL){
		message->segment = NULL;
		return segment;
	}
	//others
	while(segment->next->next != NULL)
		segment = segment->next;
	struct segment *segment_tmp = segment->next;
	segment->next = NULL;
	return segment_tmp;
}

struct segment *removeFirstSegment(struct message *message){
	if(message->segment == NULL)
		return NULL;
	struct segment * segment_tmp = message->segment;
	message->segment = message->segment->next;
	return segment_tmp;
}

void printMessage(const struct message *message){
	printf("message:\n\ttype: %s\n", message->type);
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

char *splitStr(const char *str, const int from, const int to){
	char *value = malloc(sizeof(char) * (to - from));

	int indexValue = 0;
	int indexStr = from;
	for (indexStr = from; indexStr < to; ++indexStr){
		value[indexValue] = str[indexStr];
		indexValue++;
	}

	return value;
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
/*
//test
int main(int argv, const char** argc){

	struct message* message = parseMessage("0034XYZT0003Bon0019int main(int argv,");
	printMessage(message);

	struct segment* segment;
	while(segment != NULL){
		segment = removeLastSegment(message);
		if(segment != NULL){
			printf("\nremove:\n");
			printSegment(segment);
		}
	}

	printMessage(message);
	return 0;
}
*/