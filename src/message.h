struct segment {
	int size;
	char *body;
	struct segment *next;
};
struct message {
	int length;
	char *type;
	struct segment *segment;
};

struct message *newMessage(char *type);
struct segment *newSegment(int size, char *body);

void addSegment(struct message *message, int size, char *body);

void printMessage(const struct message *message);
void printSegment(const struct segment *segment);

struct message *parseMessage(const char *str);
char *composeMessage(const struct message *message);

void freeSegment(struct segment* segment);
void freeMessage(struct message *message);
