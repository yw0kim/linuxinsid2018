#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>

#define MAX_CLIENTS 10
#define	MAX_MESSAGE_SIZE 4096

typedef struct {
	uint32_t magic_number;
	uint32_t sequnece_id;
	uint32_t transaction_id; 
	uint32_t data_type;
	uint32_t data_len;

	unsigned char data[MAX_MESSAGE_SIZE];
} message_t;

typedef struct {
	int a;
	int b;
} data_type_1_t;

typedef struct {
	int socket;
	struct sockaddr_in addr;
	uint32_t sequence_id;
	uint32_t transaction_id;

	struct message tx_buff;
	uint64_t tx_bytes;
	int tx_retry_cnt;

	struct message rx_buff;
	uint64_t rx_bytes;
	int rx_retry_cnt;
} peer_t;

extern void handle_signal_action(int sig_number);
extern int setup_signals();

extern int delete_peer(peer_t *peer);
extern int create_peer(peer_t *peer);
extern char *peer_get_addres_str(peer_t *peer);
extern int receive_from_peer(peer_t *peer);
extern int send_to_peer(peer_t *peer, message_t message);

#endif