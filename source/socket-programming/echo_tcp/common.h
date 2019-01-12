#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>

#define MAX_CLIENTS 10
#define	MAX_MESSAGE_SIZE 4096

#define REQ_UP		0
#define REQ_DOWN	1
#define DATA_UP		2
#define DATA_DOWN	3

struct message {
	uint32_t magic_number;
	uint32_t sequnece_id;
	uint32_t transaction_id; 
	uint32_t to_do;

    unsigned char src[32];
    unsigned char dst[32];
	unsigned char data[MAX_MESSAGE_SIZE];
	unsigned char sender[MAX_SENDER_SIZE];
};

struct message_queue {
	int size;
	struct message *msg_q;
	int cuurent;
};

struct peer {
	int socket;
	struct sockaddr_in addr;

	struct message_queue send_msg_q;

	struct message tx_buff;
	uint64_t tx_bytes;
	int tx_retry_cnt;

	struct message rx_buff;
	uint64_t rx_bytes;
	int rx_retry_cnt;
};

#endif