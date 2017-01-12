#include <stdlib.h>
#include <stdio.h>
#include "tools.h"
#include "message.h"

/**
 * constructeur de message
 * @param type
 * @return
 */
struct message *newMessage(char *type){
	struct message* message = malloc(sizeof(struct message));
	message->size = 8;
	message->segment = NULL;
	message->type = malloc(sizeof(char) * 4);
	str_cpy(message->type, type, 4);
	return message;
}

/**
 * constructeur de segment
 * @param body_size
 * @param body_str
 * @return
 */
struct segment *newSegment(int body_size, char *body_str){
	struct segment *segment = malloc(sizeof(struct segment));
	segment->next = NULL;
	segment->body_size = body_size;
	segment->body_str = malloc(sizeof(char) * body_size);
	str_cpy(segment->body_str, body_str, body_size);
	return segment;
}

/**
 * recuperer le segment d'un message d'une certaine profondeur
 * @param message
 * @param level
 * @return
 */
struct segment *getSegment(struct message *message, int level){
	struct segment *segment = message->segment;
	for (int i = 0; i < level; ++i){
		if(segment == NULL)
			return NULL;
		segment = segment->next;
	}
	return segment;
}

/**
 * ajoute un segment à un message
 * @param message
 * @param size
 * @param body
 */
void addSegment(struct message *message, int size, char *body){
	message->size += size + 4;
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

/**
 * affiche un message
 * @param message
 */
void printMessage(const struct message *message){
	printf("message:\n\tlength: %d\n\ttype: %s\n", message->size, message->type);
	struct segment *segment = message->segment;
	if(segment != NULL){
		printf("\tsegment:\n\t\tsize: %d\n\t\tbody: %s\n", segment->body_size, segment->body_str);
		while(segment->next != NULL){
			printf("\tsegment:\n\t\tsize: %d\n\t\tbody: %s\n", segment->next->body_size, segment->next->body_str);
			segment = segment->next;
		}
	}
}

/**
 * affiche un segment
 * @param segment
 */
void printSegment(const struct segment *segment){
	printf("segment:\n\tsize: %d\n\tbody: %s\n", segment->body_size, segment->body_str);
}

/**
 * verifie qu'un message soit correct
 * @param message
 * @return
 */
//TODO: advance checker
int verify(const struct message *message) {
	int length = 8;
	
	struct segment *segment = message->segment;
	while(segment != NULL) {
		length += segment->body_size + 4;
		segment = segment->next;
	}

	if(length == message->size)
		return 1;

	return 0;
}

/**
 * lit un message
 * retourne la transformation d'une chaine de caractere en une structure message
 * @param str
 * @param length
 * @return
 */
struct message *parseMessage(const char *str){
	//get message body_size
	char length_str[4];
	slice(str, length_str, 0, 4);
	int length = atoi(length_str);

	char re[length - 4];
	slice(str, re, 4, length);
	return parseMessageWithLength(re, length - 4);
}

/**
 * lit un message
 * retourne la transformation d'une chaine de caractere en une structure message en connaissant la taille de la chaine de caractere
 * @param str
 * @param length
 * @return
 */
struct message *parseMessageWithLength(char *str, const int length){
	//get type
	char type[4];
	slice(str, type, 0, 4);
	struct message *message = newMessage(type);
	//get segments 
	int index = 4;
	while(index < length){
		//get segment body_size
		char body_size[4];
		slice(str, body_size, index, index + 4);
		index += 4;
		int size = atoi(body_size);

		//get segment body_str
		char body_str[size];
		slice(str, body_str, index, index + size);

		index += size;
		addSegment(message, size, body_str);
	}
	return message;
}
/**
 * ecrit un message
 * retourne la transformation une structure message en une chaine de caractere
 * @param message
 * @return
 */
char *composeMessage(const struct message *message) {
	if(verify(message) == 0)
		return NULL;

	char *messageStr = malloc(sizeof(char) * message->size);
	
	char length[4];
	itoa_formated(length, message->size, 4, '0');

	for (int i = 0; i < 4; ++i)
		messageStr[i] = length[i];

	int indexMessage = 4;
	for (int i = 0; i < 4; ++i)
		messageStr[indexMessage++] = message->type[i];

	struct segment *segment = message->segment;
	while(segment != NULL) {
		itoa_formated(length, segment->body_size, 4, '0');

		for (int i = 0; i < 4; ++i)
			messageStr[indexMessage++] = length[i];
		
		for (int i = 0; i < segment->body_size; ++i)
			messageStr[indexMessage++] = segment->body_str[i];
		
		segment = segment->next;
	}
	messageStr[message->size] = '\0';
	return messageStr;
}
/**
 * libere la memoire d'une structure segment
 * @param segment
 */
void freeSegment(struct segment **segment){
    if(*segment != NULL){
        if((*segment)->next != NULL)
            freeSegment(&(*segment)->next);
        free((*segment)->body_str);
        free(*segment);
        *segment = NULL;
    }
}

/**
 * libere la memoire d'une structure message
 * @param message
 */
void freeMessage(struct message **message){
    if(*message != NULL){
        if((*message)->segment != NULL)
            freeSegment(&(*message)->segment);
        free(*message);
        free((*message)->type);
        *message = NULL;
    }
}
