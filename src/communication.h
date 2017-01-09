void send(struct message *message, int pipe);

struct message *receive(int pipe);

void sendAll(struct message *message, struct user **users, int size);
