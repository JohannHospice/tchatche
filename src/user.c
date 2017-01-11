#include <stdlib.h>
#include <stdio.h>
#include "tools.h"
#include "user.h"

struct user *newUser(int id, int id_size, char *id_str, int pseudo_size, char *pseudo_str, int pipe_size, char *pipe_str, int pipe){
	struct user *user = malloc(sizeof(struct user));
	user->id = id;
	user->id_str = id_str;
	user->id_size = id_size;
	user->pseudo_size = pseudo_size;
	user->pseudo_str = pseudo_str;
	user->pipe_size = pipe_size;
	user->pipe_str = pipe_str;
	user->pipe = pipe;
	return user;
}
void printUser(struct user* user){
	printf("\n\tid: %d\n\tpseudo_str: %s\n\tpseudo_size: %d\n\tpipe_str: %s\n\tpipe_size: %d\n\tpipe: %d\n", 
		user->id,
		user->pseudo_str, 
		user->pseudo_size, 
		user->pipe_str,
		user->pipe_size,
		user->pipe);
}