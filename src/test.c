#include <stdlib.h>
#include <stdio.h>

#include "message.h"
#include "tools.h"
#include "composer.h"

int main(int argc, char const *argv[]) {
/*
	struct message* message;
	//parse message
	message = parseMessage("0020XYZT0003Bon00018");
	
	printMessage(message);
	printf("%s\n", composeMessage(message));

	//compose message
	message = newMessage("XYZT");
	addSegment(message, 3, "Bon");
	addSegment(message, 1, "8");
	
	printMessage(message);
	printf("%s\n", composeMessage(message));
*/
	/*
	struct message *message = helo("jo", 2, "tmp/jo", 6);
	char *compose = composeMessage(message);
	*/

	char id_str[11];
	int id_size = itoa(id_str, 297878);

	struct message *message = bcst_client(id_size, id_str, 3, "bon");
	char *compose = composeMessage(message);
	printMessage(message);
	return 0;
}