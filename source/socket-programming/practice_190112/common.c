#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"

void handle_signal_action(int sig_number)
{
	if (sig_number == SIGINT) {
		printf("SIGINT was catched!\n");
		shutdown_properly(EXIT_SUCCESS);
	}
	else if (sig_number == SIGPIPE)
		printf("SIGPIPE was catched!\n");
}

int setup_signals()
{
	struct sigaction sa;
	sa.sa_handler = handle_signal_action;
	if (sigaction(SIGINT, &sa, 0) != 0) {
		perror("sigaction()");
		return -1;
	}
	if (sigaction(SIGPIPE, &sa, 0) != 0) {
		perror("sigaction()");
		return -1;
	}

	return 0;
}

int delete_peer(peer_t *peer)
{
	close(peer->socket);
}

int create_peer(peer_t *peer)
{
	peer->tx_bytes = -1;
	peer->rx_bytes = 0;

	return 0;
}

char *peer_get_addres_str(peer_t *peer)
{
	static char ret[INET_ADDRSTRLEN + 10];
	char peer_ipv4_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &peer->addres.sin_addr, peer_ipv4_str, INET_ADDRSTRLEN);
	sprintf(ret, "%s(%d)", peer_ipv4_str, ntohs(peer->addres.sin_port));

	return ret;
}

int clear_rx_retry_cnt(peer_t *peer)
{
	peer->rx_retry_cnt = 0;
}

static void print_msg(message_t msg) {
    printf("Magic Number : %d\n", msg.magic_number);
    pritnf("Sequence ID : %d\n", msg.sequence_id);
    pritnf("Transaction ID : %d\n", msg.transaction_id);
    pritnf("Data Type : %d\n", msg.data_type);
    pritnf("Data Length : %d\n", msg.data_len);

    data_type_1_t data;
    memcpy(&data, msg.data, sizeof(data_type_1_t));
    printf("Data : %d, %d\n", data.a, data.b); 
}
static int message_handler(peer_t *peer) {
    message_t *rx_msg = &peer->rx_buff;
    message_t *tx_msg = &peer->tx_buff;

    printf("Peer addr : %s]\n", peer_get_addres_str(peer));
    print_msg(*rx_msg);

    if(rx_msg->magic_number != 0x12345678) { // magic_number err
        printf("Magic Number Error\n");
        return -1;
    }
    else if(peer->transaction_id != rx_msg->transaction_id) { // new transaction
        if(rx_msg->sequence_id != 0) { // sequence id err
            printf("Sequence ID Error. New transaction should be started at 0(sequence ID) \n");
            return -1;
        }
        printf("New Transaction Started\n");
        return -1;
    }
    else if(peer->sequence_id+1 != rx_buff->sequence_id) { // seq id가 전거랑 안이어짐
        printf("Sequence ID Error. Current seq id is worng.\n");
        return -1;
    }

   	if (!strncmp(rx_msg->data, "quit", 4))
		return -1;
	if (!strncmp(rx_msg->data, "exit", 4))
		return -1; 

    peer->sequence_id = rx_buff->sequence_id;
    peer->transaction_id = rx_buff->transaction_id;


};

/* 
 * Receive message from peer and handle it with message_handler(). 
 * 
 * return:	-1:  failed (need to disconnect)
 *		 0~: continue (try again)
 *		 1~: successed (received bytes)
 */
int receive_from_peer(peer_t *peer)
{
	int len;	/* length to receive */
	int n;		/* receive bytes */
	int rc;

	/* Count bytes to send. */
	len = sizeof(peer->rx_buff) - peer->rx_bytes;
	if (len > MAX_SEND_SIZE)
		len = MAX_SEND_SIZE;

	if (peer->rx_bytes == 0)
		memset(&peer->rx_buff, 0, sizeof(peer->rx_buff));

	/* try to receive bytes */
	n = recv(peer->socket, (char *) &peer->rx_buff + peer->rx_bytes, 
			len, MSG_DONTWAIT);
	if (n < 0) {
		/* peer busy? or server busy? then try again later */
		if (errno == EAGAIN || errno == EINTR)
			return 0;	/* continue */
		else if (errno == EPIPE || errno == ETIMEDOUT ) {
			printf("recv() failed & try to close socket. err=%d(%s)\n",
					errno, strerror(errno));
			return -1;	/* close client socket */
		} else {
			peer->rx_retry_cnt++;
			if (peer->rx_retry_cnt > 3) {
				printf("recv() failed(max-retry) & " \
						"try to close socket. err=%d(%s)\n",
						errno, strerror(errno));
				return -1;	/* close client socket */
			}

			printf("recv() failed. retry=%d, err=%d(%s)\n",
					peer->rx_retry_cnt, errno, strerror(errno));
			return 0;	/* continue */
		}
	} else if (n == 0) {
		/* If recv() returns 0, it means that peer gracefully shutdown. 
		 * Shutdown client. */
		printf("recv() 0 bytes. Peer gracefully shutdown.\n");
		return -1;	/* close client socket */
	}

	/* received n bytes */
	clear_rx_retry_cnt(peer);
	peer->rx_bytes += n;

#ifdef _USE_MSG
	/* Is completely received? */
	if (peer->rx_bytes >= sizeof(peer->rx_buff)) 
#endif
	{
		peer->rx_bytes = 0;
		rc = message_handler(peer);
		if (rc < 0)
			return -1;	/* quit command from client */
	}

	return peer->rx_bytes;
}

/* 
 * Send message to peer. 
 * 
 * 
 * return:	-1:  failed (need to disconnect)
 *		 0~: continue (try again)
 *		 1~: successed (sent bytes)
 */
int send_to_peer(peer_t *peer, message_t msg)
{
	int len;	/* lenhth to send */
	int n;	/* send bytes */
	int tot_len;
	int tot_n;

/*
    // new message then dequeue for tx 
    if (peer->tx_bytes < 0 || peer->tx_bytes >= sizeof(peer->tx_buff)) {
        if (dequeue(&peer->fifo, &peer->tx_buff) != 0) {
            peer->tx_bytes = -1;
            return 0;
        }
        peer->tx_bytes = 0;
    }
*/

    peer->tx_bytes = 0;

#ifdef USE_MSG
	tot_len = sizeof(peer->tx_buff);
#else
	tot_len = strlen(peer->tx_buff.data);
#endif

	/* Count bytes to send. */
	len = tot_len - peer->tx_bytes;
	if (len <= 0) {
		peer->tx_bytes = -1;
		return 0;
	} else if (len > MAX_SEND_SIZE)
		len = MAX_SEND_SIZE;

	n = send(peer->socket, (char *) &peer->tx_buff + peer->tx_bytes, len, 0);
	if (n < 0) {
		/* peer is not ready right now or server is interrupted, try again later. */
		if (errno == EAGAIN || errno == EINTR)
			return 0;
		else if (errno == EPIPE || errno == ETIMEDOUT) {
			printf("send() failed & try to close socket. err=%d(%s)\n",
					errno, strerror(errno));
			return -1;
		} else {
			printf("send() failed & try to close socket. err=%d(%s)\n",
					errno, strerror(errno));
			return -1;
		}
	} else if (n == 0)
		/* send 0 bytes. 
                 * It seems that peer can't accept data right now. Try again later. */
		return 0;

	/* send n bytes */
	tot_n = peer->tx_bytes;
	peer->tx_bytes = -1;

	return tot_n;
}