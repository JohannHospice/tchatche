struct segment;
struct message;

struct message *parseMessage(const char *str);
char *composeMessage(struct message *message);

struct message *newMessage(int length, char *type);
struct segment *newSegment(int size, char *body);

struct segment *removeLastSegment(struct message *message);
struct segment *removeFirstSegment(struct message *message);

void addSegment(struct message *message, int size, char *body);

void printMessage(const struct message *message);
void printSegment(const struct segment *segment);
