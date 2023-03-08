#ifndef SHARED_TCP_H
#define SHARED_TCP_H

#include <stddef.h>

int tcp_IpAddrIsValid(const char* addr);
int tcp_PortIsValid(int port_no);
int tcp_Connect(const char* remote_addr, int remote_port, int* out_socket);
int tcp_Disconnect(int socket);
int tcp_IsConnected(int socket);
int tcp_CreateListener(const char* local_addr, int local_port, int* out_socket);
int tcp_Accept(int listener_socket, char* remote_addr, int* remote_port, int* out_socket);
int tcp_Send(int socket, const char* msg_send, size_t msg_size, int timeout_sec);
int tcp_Recv(int socket, char* msg_recv, size_t msg_size, int timeout_sec);

#endif // SHARED_TCP_H
