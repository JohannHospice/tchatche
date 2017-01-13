#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "tools.h"
#include "message.h"
#include "user.h"
#include "composer.h"
#include "communication.h"

#define RWX_ALL 0777
#define TMP_PATH "tmp/"
#define PIPE_SERVER_PATH "tmp/serv"
#define LIMIT_CLIENT 20

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

int main(int argv, const char **argc) {

    if (init() == -1) {
        perror("error: initialisation\n");
        return -1;
    }

    printf("Waiting for clients\n");

    int run = 1,
            users_size = 0,
            pipe_r = open(PIPE_SERVER_PATH, O_RDONLY);

    struct user *users[LIMIT_CLIENT];
    struct message *message_send = NULL;
    struct message *message_receive = NULL;
    for (int i = 0; i < LIMIT_CLIENT; i++)
        users[i] = NULL;

    do {
        // attente et reception d'un éventuelle message
        if ((message_receive = receive(pipe_r, 0)) == NULL)
            continue;

        if (strcmp(message_receive->type, HELO) == 0) {

            printf("[new client]\n");
            int slot = 0;
            char id_str[11];
            int id_size = itoa(id_str, users_size);
            int n_tmp = 1;
            int test;
            int pseudo_size = message_receive->segment->body_size;
            char pseudo[pseudo_size + 3];
            str_cpy(pseudo, message_receive->segment->body_str, pseudo_size);
            do {
                test = 0;
                for (int i = 0; i < LIMIT_CLIENT; i++) {
                    if (users[i] != NULL) {
                        if (strcmp(pseudo, users[i]->pseudo_str) == 0) {
                            printf("%s %s\n", pseudo, users[i]->pseudo_str);
                            if (pseudo_size == message_receive->segment->body_size)
                                pseudo_size = pseudo_size + 2;
                            pseudo[pseudo_size - 2] = '_';
                            pseudo[pseudo_size - 1] = n_tmp + '0';
                            n_tmp++;
                            test = 1;
                        }
                    }
                }
            } while (test == 1);


            while (users[slot] != NULL)
                slot++;

            users[slot] = newUser(
                    slot,
                    id_size,
                    id_str,
                    pseudo_size,
                    pseudo,
                    message_receive->segment->next->body_size,
                    message_receive->segment->next->body_str,
                    open(message_receive->segment->next->body_str, O_WRONLY));
            users_size++;
            printUser(users[slot]);

            message_send = okok(users[slot]->id_size, users[slot]->id_str);
            send(message_send, users[slot]->pipe, 0);
            freeMessage(&message_send);

            message_send = helo_server(pseudo_size, pseudo);
            sendAll(message_send, users, users_size, 0);
        } else if (strcmp(message_receive->type, BYEE) == 0) {
            int id = atoi(message_receive->segment->body_str);
            message_send = byee(users[id]->id_size, users[id]->id_str);
            send(message_send, users[id]->pipe, 0);
            close(users[id]->pipe);
            users_size--;
            freeUser(&users[id]);
        } else if (strcmp(message_receive->type, BCST) == 0) {
            int id = atoi(message_receive->segment->body_str);
            message_send = bcst_server(
                    users[id]->pseudo_size,
                    users[id]->pseudo_str,
                    message_receive->segment->next->body_size,
                    message_receive->segment->next->body_str);
            sendAll(message_send, users, users_size, 0);
        } else if (strcmp(message_receive->type, PRVT) == 0) {
            int id = atoi(message_receive->segment->body_str);
            int ok, i = 0;
            // si pseudo dans message existe dans liste utilisateur
            while (i < users_size
                   && (ok = strcmp(message_receive->segment->next->body_str, users[i]->pseudo_str)) != 0) {
                i++;
            }
            if (ok == 0) {
                message_send = prvt_server(
                        users[id]->pseudo_size,
                        users[id]->pseudo_str,
                        message_receive->segment->next->next->body_size,
                        message_receive->segment->next->next->body_str);
                send(message_send, users[i]->pipe, 0);
            } else {
                message_send = badd();
                send(message_send, users[id]->pipe, 0);
            }
        } else if (strcmp(message_receive->type, LIST) == 0) {
            int id = atoi(message_receive->segment->body_str);
            char nb_client_str[11];
            int nb_client_size = itoa(nb_client_str, users_size);
            for (int i = 0; i < users_size; ++i) {
                message_send = list_server(
                        nb_client_size,
                        nb_client_str,
                        users[i]->pseudo_size,
                        users[i]->pseudo_str);
                send(message_send, users[id]->pipe, 0);
            }
        } else if (strcmp(message_receive->type, SHUT) == 0) {
            int id = atoi(message_receive->segment->body_str);
            run = 0;
            message_send = shut_server(
                    users[id]->pseudo_size,
                    users[id]->pseudo_str);
            sendAll(message_send, users, users_size, 0);
        } else if (message_receive->segment != NULL) {
            int id = atoi(message_receive->segment->body_str);
            message_send = badd();
            send(message_send, users[id]->pipe, 0);
        }
        freeMessage(&message_receive);
        freeMessage(&message_send);
    } while (run);

    for (int i = 0; i < users_size; ++i) {
        close(users[i]->pipe);
        freeUser(&users[i]);
    }
    close(pipe_r);
    remove(PIPE_SERVER_PATH);
    printf("[exit]\n");
    return 0;
}