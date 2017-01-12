struct segment {
	int body_size;
	char *body_str;
	struct segment *next;
};
struct message {
	int size;
	char *type;
	struct segment *segment;
};

struct message *newMessage(char *type);
struct segment *newSegment(int size, char *body_str);

void addSegment(struct message *message, int size, char *body);

void printMessage(const struct message *message);
void printSegment(const struct segment *segment);

struct message *parseMessage(const char *str);
struct message *parseMessageWithLength(char *str, const int length);
char *composeMessage(const struct message *message);

void freeSegment(struct segment* segment);
void freeMessage(struct message *message);
