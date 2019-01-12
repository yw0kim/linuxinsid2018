#include "stdio.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <stdlib.h>
#include "../common.h"

#define BUF_LEN 2048

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



void main(int argc, char *argv[])
{
        int s, n;
        char *haddr;
        struct sockaddr_in server_addr;
        //struct sockaoddr_in server_addr : 서버의 소켓주소 구조체
        unsigned char rx_buf[BUF_LEN+1];
	unsigned char tx_buf[BUF_LEN+1];
 
        if(argc != 3)
        {
                printf("usage : %s ip_Address port\n", argv[0]);
                exit(0);
        }
        haddr = argv[1];
	
	message_t tx_msg;
	message_t rx_msg;
 
	tx_msg.magic_number = 0x12345678;

        if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {//소켓 생성과 동시에 소켓 생성 유효검사
                printf("can't create socket\n");
                exit(0);
        }
 
        bzero((char *)&server_addr, sizeof(server_addr));
        //서버의 소켓주소 구조체 server_addr을 NULL로 초기화
 
        server_addr.sin_family = AF_INET;
        //주소 체계를 AF_INET 로 선택
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
        //32비트의 IP주소로 변환
        server_addr.sin_port = htons(atoi(argv[2]));
 
        if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("can't connect.\n");
		exit(0);
	}

	while(1) {
		int data1, data2;
		int seq_id, tran_id;
		data_type_1_t struct_data;
		
		printf("data1 data2 seq_id tran_id > ");
		scanf("%d %d %d %d", &data1, &data2, &seq_id, &tran_id);

		tx_msg.sequence_id = seq_id;
		tx_msg.transaction_id = tran_id;
		tx_msg.data_type = 1;
		tx_msg.data_len = 8;

		struct_data.a = data1;
		struct_data.b = data2;

		memcpy(tx_msg.data, &struct_data, sizeof(data_type_1_t));

		memcpy(tx_buf, &tx_msg, sizeof(message_t));
		write(s, tx_buf, sizeof(tx_buf));
		while(1)
		{//서버가 보내오는 daytime 데이터의 수신
			memset(rx_buf, 0x00, BUF_LEN);
			int recv_len = recv(s, rx_buf, BUF_LEN, 0);
			memcpy(&rx_msg, rx_buf, sizeof(message_t));
			
			if(rx_msg.magic_number == 0x12345678)
				break;	
		}

		printf("RECV MSG\n");
		print_message(&rx_msg);
		printf("--------------------------\n");
	}

	close(s);
	//사용이 완료된 소켓을 닫기
}


