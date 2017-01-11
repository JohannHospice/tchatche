int send(struct message *message, int pipe, int printMessage);

struct message *receive(int pipe, int printMessage);

void sendAll(struct message *message, struct user **users, int size, int printMessage);
