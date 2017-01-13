#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tools.h"
#include "message.h"
#include "user.h"
#include "composer.h"
#include "communication.h"

#define RWX_ALL 0777
#define TMP_PATH "tmp/"
#define PIPE_SERVER_PATH "tmp/serv"
#define LIMIT_USERS 20

/**
 * initialisation: création du repertoire temporaire et du pipe server
 * @return
 */
int init() {
    if (access(TMP_PATH, F_OK) != F_OK)
        if (mkdir(TMP_PATH, RWX_ALL) == -1)
            return -1;
    if (access(PIPE_SERVER_PATH, F_OK) != F_OK)
        if (mkfifo(PIPE_SERVER_PATH, RWX_ALL) == -1)
            return -1;
    return 0;
}

/**
 * récupere un emplacement vide du tableau d'utilisateur, retourne -1 si non trouvé 
 * @param users
 * @param size
 * @return
 */
int getEmptySlot(struct user **users, int size){
    int i = 0;
    int slot = -1;
    while (i < size && slot == -1) {
        if(users[i] == NULL)
            slot = i; 
        i++;
    }
    return slot;
}

/**
 * recupere l'id d'un utilisateur selon le pseudo passé en parametre, renvoi -1 si non trouvé
 * @param pseudo_str
 * @param users
 * @return
 */
int getIdUserByPseudo(char *pseudo_str, struct user **users){
    int i = 0;
    int id = -1;
    while (i < LIMIT_USERS && id == -1) {
        if(users[i] != NULL){
            if(strcmp(pseudo_str, users[i]->pseudo_str) == 0)
              id = i;
        } 
        i++;
    } 
    return id;
}
/**
 * envoi une requete BADD
 * @param pipe
 */
void sendBadd(int pipe){
    struct message *message = badd();
    send(message, pipe, 0);
    freeMessage(&message);
}

int suffix(char *pseudo_str, char *pseudorequest_str, int pseudorequest_size, struct user **users){
    int pseudo_size = pseudorequest_size;
    str_cpy(pseudo_str, pseudorequest_str, pseudorequest_size);
    int n_tmp = 1, test, i;
    do {
        test = 0;
        for (i = 0; i < LIMIT_USERS; i++) if (users[i] != NULL) {
            if (strcmp(pseudo_str, users[i]->pseudo_str) == 0) {
                if (pseudo_size == pseudorequest_size)
                    pseudo_size = pseudo_size + 2;
                pseudo_str[pseudo_size - 2] = '_';
                pseudo_str[pseudo_size - 1] = n_tmp + '0';
                n_tmp++;
                test = 1;
            }
        }
    } while (test == 1);
    return pseudo_size;
}

int main(int argv, const char **argc) {

    if (init() == -1) {
        perror("error: initialisation\n");
        return -1;
    }

    printf("Waiting for clients\n");

    int i,
        run = 1,
        users_size = 0,
        pipe_r = open(PIPE_SERVER_PATH, O_RDONLY);

    struct user *users[LIMIT_USERS];
    struct message *message_send = NULL;
    struct message *message_receive = NULL;
    for (i = 0; i < LIMIT_USERS; i++)
        users[i] = NULL;

    do {
        // attente et reception d'un éventuelle message
        if ((message_receive = receive(pipe_r, 0)) == NULL)
            continue;
        int id_sender;
        if (strcmp(message_receive->type, HELO) == 0) {
            struct segment *pseudo_segment = getSegment(message_receive, 0);
            struct segment *pipe_segment = getSegment(message_receive, 1);

            if(pseudo_segment != NULL && pipe_segment != NULL){
                int user_pipe = open(pipe_segment->body_str, O_WRONLY);
                if(user_pipe != -1) {
                    int slot = getEmptySlot(users, LIMIT_USERS);
                    if(slot != -1){
                        char pseudo_str[30];
                        int pseudo_size = suffix(
                            pseudo_str, 
                            pseudo_segment->body_str, 
                            pseudo_segment->body_size, 
                            users);

                        char id_str[11];
                        int id_size = itoa(id_str, slot);

                        users[slot] = newUser(    
                            slot,
                            id_size,
                            id_str,
                            pseudo_size,
                            pseudo_str,
                            pipe_segment->body_size,
                            pipe_segment->body_str,
                            user_pipe);
                        users_size++;
                        printf("[new client]\n");
                        printUser(users[slot]);

                        message_send = okok(users[slot]->id_size, users[slot]->id_str);
                        send(message_send, users[slot]->pipe, 0);
                        freeMessage(&message_send);

                        message_send = helo_server(pseudo_size, pseudo_str);
                        sendAll(message_send, users, LIMIT_USERS, 0);
                    }
                    else sendBadd(user_pipe);
                }
            }
            continue;
        } else {
        	struct segment *id_segment = getSegment(message_receive, 0);
        	if (id_segment != NULL){
	            id_sender = atoi(id_segment->body_str);
	            if(id_sender < -1 || LIMIT_USERS < id_sender)
	            	continue;
        	}
        	else continue;
        }

        if (strcmp(message_receive->type, BYEE) == 0) {
            message_send = byee(users[id_sender]->id_size, users[id_sender]->id_str);
            send(message_send, users[id_sender]->pipe, 0);
            close(users[id_sender]->pipe);
            freeUser(&users[id_sender]);
            users_size--;
        } 
        else if (strcmp(message_receive->type, BCST) == 0) {
            struct segment *txt_segment = getSegment(message_receive, 1);
            if(txt_segment != NULL){
                message_send = bcst_server(
                        users[id_sender]->pseudo_size,
                        users[id_sender]->pseudo_str,
                        txt_segment->body_size,
                        txt_segment->body_str);
                sendAll(message_send, users, LIMIT_USERS, 0);
            }
        } 
        else if (strcmp(message_receive->type, PRVT) == 0) {
            struct segment *pseudo_segment = getSegment(message_receive, 1);
            struct segment *txt_segment = getSegment(message_receive, 2);
            if(pseudo_segment != NULL && txt_segment != NULL){
                int id_receiver = getIdUserByPseudo(pseudo_segment->body_str, users);
                if(id_receiver != -1){
                    message_send = prvt_server(
                            users[id_sender]->pseudo_size,
                            users[id_sender]->pseudo_str,
                            txt_segment->body_size,
                            txt_segment->body_str);
                    send(message_send, users[id_receiver]->pipe, 0);
                } else sendBadd(users[id_sender]->pipe);
            } else sendBadd(users[id_sender]->pipe);
        } 
        else if (strcmp(message_receive->type, LIST) == 0) {
            char nb_client_str[11];
            int nb_client_size = itoa(nb_client_str, users_size);
            for (i = 0; i < LIMIT_USERS; ++i) if(users[i] != NULL) {
                message_send = list_server(
                        nb_client_size,
                        nb_client_str,
                        users[i]->pseudo_size,
                        users[i]->pseudo_str);
                send(message_send, users[id_sender]->pipe, 0);
            }
        } 
        else if (strcmp(message_receive->type, SHUT) == 0) {
            run = 0;
            message_send = shut_server(
                    users[id_sender]->pseudo_size,
                    users[id_sender]->pseudo_str);
            sendAll(message_send, users, LIMIT_USERS, 0);
        } 
        else 
            sendBadd(users[id_sender]->pipe);
        freeMessage(&message_receive);
        freeMessage(&message_send);
    } while (run);

    for (i = 0; i < LIMIT_USERS; ++i) if(users[i] != NULL) {
        close(users[i]->pipe);
        freeUser(&users[i]);
    }
    close(pipe_r);
    remove(PIPE_SERVER_PATH);
    printf("[exit]\n");
    return 0;
}