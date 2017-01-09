struct user {
	int id;
	char *id_str;
	int id_size;

	int pipe;
	char *pipe_str;
	int pipe_size;

	char *pseudo_str;
	int pseudo_size;
};

struct user *newUser(int id, int pseudo_size, char *pseudo_str, int pipe_size, char *pipe_str, int pipe);
struct user *newUserForClient(int id_size, char *id_str, int pseudo_size, char *pseudo_str, int pipe_size, char *pipe_str, int pipe);
void printUser(struct user* user);
