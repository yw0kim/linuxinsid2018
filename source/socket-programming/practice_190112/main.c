#include "common.h"
#include "server.h"

struct peer server;
struct peer clients[MAX_CLIENTS];

int main(int argc, char **argv) {
	int i;
	int port;	/* server listen port */

	if (argc < 2) {
		printf("usage) %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	port = atoi(argv[1]);

	/* for SIGINT & SIGPIPE */
	if (setup_signals() != 0)
		exit(EXIT_FAILURE);

	/* bind & listen server socket */
	if (start_listen_socket(port, &server.socket) != 0)
		exit(EXIT_FAILURE);

	/* Set nonblock for stdin. */
	set_sock_nonblocking(STDIN_FILENO);

	/* prepare clients */
	for (i = 0; i < MAX_CLIENTS; ++i) {
		clients[i].socket = NO_SOCKET;
		create_peer(&clients[i]);
	}

	/* do socket processing */
	do_server();
}