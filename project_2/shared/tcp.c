#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"
#include "string.h"
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

// returns true if provided IP address is valid
int tcp_IpAddrIsValid(const char* addr)
{
    struct in_addr dummy;

    return (inet_pton(AF_INET, addr, &dummy));
}

// returns true if provided port is valid
int tcp_PortIsValid(int port_no)
{   
    return (port_no > 1 && port_no < 65535);
}

// connects to remote host and returns socket if successful
int tcp_Connect(const char* remote_addr, int remote_port, int* out_socket)
{
    struct sockaddr_in remote;
    int result;

    // force disconnect (just in case)
    *out_socket = netDisconnect(*out_socket);

    // validate IP address
    result = inet_pton(AF_INET, remote_addr, &remote.sin_addr);
    if(result == 0)
    {
        printf("ERROR: \"%s\" is an invalid address.\n", remote_addr);
        return -1; // failure
    }
    else if(result < 0)
    {
        printf("ERROR: %s. Please try again.\n", strerror(errno));
        return -1; // failure
    }

    // build socket address
    remote.sin_family = AF_INET;
    remote.sin_port = htons(remote_port);
    memset(&(remote.sin_zero), 0, sizeof(remote.sin_zero));

    // create socket
    *out_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(*out_socket == -1)
    {
        printf("ERROR: Failed to create socket, %s\n", strerror(errno));
        return -1;
    } 
    
    // connect to remote machine
    printf("Connecting to remote machine at %s:%u...\n", remote_addr, remote_port);

    result = connect(*out_socket, (struct sockaddr*)&remote, sizeof(remote));
    if(result == -1)
    {
        printf("ERROR: Failed to connect. %s\n", strerror(errno));
        close(*out_socket);
        return -1; // failure
    }

    // success
    printf("Successfully connected to \"%s\".\n", remote_addr);
    return 0; 
}

// create listener socket
int tcp_CreateListener(const char* local_addr, int local_port, int* out_socket)
{
    int enable = 1;
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(local_addr);
    addr.sin_port = htons(local_port);
    
    // generate new listening socket
    *out_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (*out_socket == -1) 
    {
        printf("ERROR: Failed to create socket, %s\n", strerror(errno));
        return -1;
    }

    // set socket as reusable (to avoid "address already in use" error)
    if(setsockopt(*out_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        printf("ERROR: Failed to configure socket as address reusable. %s\n", strerror(errno));
        return -1;
    }

    // bind listening socket to specified address / port
    if (bind(*out_socket, (struct sockaddr*) &addr, sizeof(addr)) == -1) 
    {
        printf("ERROR: Socket bind error, %s\n", strerror(errno));
        close(*out_socket);
        return -1;
    }

    // begin listening
    if (listen(*out_socket, 1/*length of connections queue*/) == -1) {
        printf("ERROR: Listening error, %s\n", strerror(errno));
        close(*out_socket);
        return -1;
    }

    // success
    return 0;
}

// accepts new connections (blocks indefinitely)
int tcp_Accept(int listener_socket, char* remote_addr, int* remote_port, int* out_socket)
{
        struct sockaddr_in remote;
        socklen_t socklen;
        
        *out_socket = accept(listener_socket, (struct sockaddr*)&remote, &socklen);
        if(*out_socket == -1) 
        {
            printf("ERROR: Error occurred during connection accept, %s\n", strerror(errno));
            close(*out_socket);
            return -1;
        }

        // copy remote info  to outputs
        sprintf(remote_addr, "%s", inet_ntoa(remote.sin_addr));
        *remote_port = ntohs(remote.sin_port);

        // success
        return 0;
}

// closes connections and returns -1
int tcp_Disconnect(int socket)
{
    if(socket != -1)
    {
        close(socket);
    }
    
    return -1;
}

// return 1 if socket is connected to remote host
int tcp_IsConnected(int socket)
{
    char dummy;

    int result = recv(socket, &dummy, 1, MSG_PEEK | MSG_DONTWAIT);

    if(result == 0)
    {
        return 0;
    }
    else if(result < 0)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return 1;
        }

        return 0;
    }
    else
    {
        return 1;
    }
}

// sends data
int tcp_Send(int socket, const char* msg_send, size_t msg_size, uint32_t timeout_sec)
{
    // configure timeout
    struct timeval timeout;      
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;

    if(setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        printf("ERROR: Failed to configure socket timeout. %s\n", strerror(errno));
        return -1;
    }

    // send message
    ssize_t bytes_sent = send(socket, msg_send, msg_size, 0);
    
    if(bytes_sent == -1)
    {
        printf("ERROR: Error occured while sending bytes. %s\n", strerror(errno));
        return -1;
    }
    else if(bytes_sent < msg_size)
    {
        printf("ERROR: Timed-out while sending bytes. %ld/%ld bytes sent.\n", bytes_sent, msg_size);
        return -1;
    }

    // success
    return 0;
}

// receives data from socket
int tcp_Recv(int socket, char* msg_recv, size_t msg_size, uint32_t timeout_sec)
{
    // configure timeout
    struct timeval timeout;      
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;

    if(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        printf("ERROR: Failed to configure socket timeout. %s\n", strerror(errno));
        return -1;
    }

    // read bytes from socket
    ssize_t bytes_rcvd = recv(socket, msg_recv, msg_size, 0);

    if(bytes_rcvd == -1)
    {
        printf("ERROR: Error occured while receiving bytes. %s\n", strerror(errno));
        return -1;
    }
    else if(bytes_rcvd < msg_size)
    {
        printf("ERROR: Timed-out while receiving bytes.\n");
        return -1;
    }

    // success
    return 0;
}
