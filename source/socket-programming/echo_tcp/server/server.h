#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdint.h>

struct message {
	uint
};

struct peer {
	int socket;
	struct sockaddr_in addr;

};

#endif // _SERVER_H_
