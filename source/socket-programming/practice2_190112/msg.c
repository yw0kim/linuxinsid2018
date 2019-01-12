#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"

int prepare_message(unsigned char *data, message_t *tx_msg)
{
	message_t rx_msg;

	memcpy(&rx_msg, data, sizeof(message_t));

	tx_msg->magic_number = 0x12345678;
	tx_msg->sequence_id = rx_msg.sequence_id;
	tx_msg->transaction_id = rx_msg.transaction_id;
	tx_msg->data_type = 1;
	tx_msg->data_len = 8;

	data_type_1_t send_data;
	send_data.a = 23232;
	send_data.b = 34343;
	
	memcpy(tx_msg->data, &send_data, sizeof(data_type_1_t));
	
	printf("TX: %d %d\n", send_data.a, send_data.b);

	return 0;
}

int print_message(message_t *msg)
{
	printf("Magic Number : %d\n", msg->magic_number);
	printf("Sequence ID : %d\n", msg->sequence_id);
	printf("Transaction ID : %d\n", msg->transaction_id);
	printf("Data Type : %d\n", msg->data_type);
	printf("Data Length : %d\n", msg->data_len);

	data_type_1_t data;
	memcpy(&data, msg->data, sizeof(data_type_1_t));
	printf("Data : %d, %d\n", data.a, data.b); 
	return 0;
}

int handle_received_message(peer_t *peer)
{
	message_t *rx_msg = &peer->rx_buff;
	message_t *tx_msg = &peer->tx_buff;

	print_message(rx_msg);

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
		return 0;
	}
	else if(peer->sequence_id != -1 && peer->sequence_id+1 != rx_msg->sequence_id) { // seq id가 전거랑 안이어짐
		printf("Sequence ID Error. Current seq id is worng.\n");
		return -1;
	}

	peer->sequence_id = rx_msg->sequence_id;
	peer->transaction_id = rx_msg->transaction_id;
	
	unsigned char rx_msg_char[4096];
	memcpy(rx_msg_char, rx_msg, sizeof(message_t));

	prepare_message(rx_msg_char, tx_msg);
	if(peer_add_to_send(peer, tx_msg) < 0) {
		printf("enqueue failed\n");
	}

	

	return 0;
}
