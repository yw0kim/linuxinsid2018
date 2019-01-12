#ifndef _SERVER_H_
#define _SERVER_H_

#include "../common.h"

extern struct peer server;
extern struct clients[MAX_CLIENTS];

int set_reuseaddr_opt(int fd);
int set_sock_nonblocking(int fd);
int set_sock_timeout(int fd, int sec);
void set_sock_keepallive(int fd, int idle, int interval, int cnt);

extern int start_listen_socket(int port, int *listen_sock);
extern int handle_new_connection();
extern int close_client_connection(peer_t *client);
extern int get_client_name(int argc, char **argv, char *client_name);
extern int build_fd_sets(fd_set *read_fds, fd_set *write_fds, fd_set *except_fds);
extern void shutdown_properly(int code);
extern int do_server();

#endif // _SERVER_H_
