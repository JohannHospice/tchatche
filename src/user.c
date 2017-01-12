#include <stdlib.h>
#include <stdio.h>
#include "user.h"
#include "tools.h"

struct user *newUser(int id, int id_size, char *id_str, int pseudo_size, char *pseudo_str, int pipe_size, char *pipe_str, int pipe){
	struct user *user = malloc(sizeof(struct user));
	user->id = id;
	user->id_size = id_size;
    user->pseudo_size = pseudo_size;
    user->pipe_size = pipe_size;
	user->pipe = pipe;

    user->id_str = malloc(sizeof(char) * id_size);
    user->pseudo_str = malloc(sizeof(char) *  pseudo_size);
    user->pipe_str = malloc(sizeof(char) * pipe_size);
    str_cpy(user->id_str, id_str, id_size);
    str_cpy(user->pipe_str, pipe_str, pipe_size);
    str_cpy(user->pseudo_str, pseudo_str, pseudo_size);
    return user;
}

void printUser(struct user* user){
	if(user != NULL)
		printf("user:\n\tid: %d\n\tpseudo_str: %s\n\tpseudo_size: %d\n\tpipe_str: %s\n\tpipe_size: %d\n\tpipe: %d\n",
			user->id,
			user->pseudo_str,
			user->pseudo_size,
			user->pipe_str,
			user->pipe_size,
			user->pipe);
}