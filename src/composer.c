/**
 * permet la formation different type de message
 */
#include "message.h"
#include "composer.h"

struct message *helo_server(int pseudo_size, char *pseudo_str){
    struct message *message = newMessage(HELO);
    addSegment(message, pseudo_size, pseudo_str);
    return message;
}

struct message *helo_client(int pseudo_size, char *pseudo_str, int tube_size, char *tube){
    struct message *message = newMessage(HELO);
    addSegment(message, pseudo_size, pseudo_str);
    addSegment(message, tube_size, tube);
    return message;
}

struct message *okok(int id_size, char *id_str){
	struct message *message = newMessage(OKOK);
	addSegment(message, id_size, id_str);
	return message;
}

struct message *badd(){
	struct message *message = newMessage(BADD);
	return message;
}

struct message *byee(int id_size, char *id_str){
	struct message *message = newMessage(BYEE);
	addSegment(message, id_size, id_str);
	return message;
}

struct message *bcst_client(int id_size, char *id_str, int txt_size, char *txt_str){
	struct message *message = newMessage(BCST);
	addSegment(message, id_size, id_str);
	addSegment(message, txt_size, txt_str);
	return message;
}

struct message *bcst_server(int pseudo_size, char *pseudo_str, int txt_size, char *txt_str){
	struct message *message = newMessage(BCST);
	addSegment(message, pseudo_size, pseudo_str);
	addSegment(message, txt_size, txt_str);
	return message;
}

struct message *prvt_client(int id_size, char *id_str, int pseudo_size, char *pseudo_str, int txt_size, char *txt_str){
	struct message *message = newMessage(PRVT);
	addSegment(message, id_size, id_str);
	addSegment(message, pseudo_size, pseudo_str);
	addSegment(message, txt_size, txt_str);
	return message;
}

struct message *prvt_server(int pseudo_size, char *pseudo_str, int txt_size, char *txt_str){
	struct message *message = newMessage(PRVT);
	addSegment(message, pseudo_size, pseudo_str);
	addSegment(message, txt_size, txt_str);
	return message;
}

struct message *list_client(int id_size, char *id_str){
	struct message *message = newMessage(LIST);
	addSegment(message, id_size, id_str);
	return message;
}

struct message *list_server(int n_size, char *n_str, int pseudo_size, char *pseudo_str){
	struct message *message = newMessage(LIST);
	addSegment(message, n_size, n_str);
	addSegment(message, pseudo_size, pseudo_str);
	return message;
}

struct message *shut_client(int id_size, char *id_str){
	struct message *message = newMessage(SHUT);
	addSegment(message, id_size, id_str);
	return message;
}

struct message *shut_server(int pseudo_size, char *pseudo_str){
	struct message *message = newMessage(SHUT);
	addSegment(message, pseudo_size, pseudo_str);
	return message;
}

struct message *debg(){
    struct message *message = newMessage(DEBG);
    return message;
}

struct message *file_permission(int serie_size, char *serie_str, int id_size, char *id_str, int pseudo_size, char *pseudo_str, int filelength_size, char *filelength_str, int filename_size, char *filename_str){
    struct message *message = newMessage(FILE);
    addSegment(message, serie_size, serie_str);
    addSegment(message, id_size, id_str);
    addSegment(message, pseudo_size, pseudo_str);
    addSegment(message, filelength_size, filelength_str);
    addSegment(message, filename_size, filename_str);
    return message;
}

struct message *file_inforation(int serie_size, char *serie_str, int idtransfert_size, char *idtransfert_str, int filelength_size, char *filelength_str, int filename_size, char *filename_str){
    struct message *message = newMessage(FILE);
    addSegment(message, serie_size, serie_str);
    addSegment(message, idtransfert_size, idtransfert_str);
    addSegment(message, filelength_size, filelength_str);
    addSegment(message, filename_size, filename_str);
    return message;
}

struct message *file_data(int serie_size, char *serie_str, int idtransfert_size, char *idtransfert_str, int data_size, char *data_str){
    struct message *message = newMessage(FILE);
    addSegment(message, serie_size, serie_str);
    addSegment(message, idtransfert_size, idtransfert_str);
    addSegment(message, data_size, data_str);
    return message;
}

