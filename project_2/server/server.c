#include <stdio.h>
#include <stdlib.h>

#include "../shared/tcp.h"
#include "../shared/msg.h"
#include "../shared/article.h"

int main(int argc, char * argv[])
{
    int listener_socket = -1;
    char local_addr[] = "127.0.0.1";
    int local_port = 5555;
    
    int remote_socket = -1;
    char remote_addr[64];
    int remote_port;

    char recv_msg[4096];
    char send_msg[4096];

    uint32_t msg_recv_type;
    uint32_t msg_recv_size;
    
    // create listener socket
    if(tcp_CreateListener(local_addr, local_port, &listener_socket))
    {
        printf("Failed to create listener socket\n");
        return -1;
    }
    
    // accept connections
    while(1)
    {        
        // accept connection
        if(tcp_Accept(listener_socket, remote_addr, &remote_port, &remote_socket))
        {
            printf("ERROR occurred while accepting connection\n");
            return -1;
        }

        // wait for header
        if(tcp_Recv(remote_socket, recv_msg, MSG_HEADER_OFFSET, 5))
        {
            printf("ERROR occurred while receiving message header\n");
            return -1;
        }

        // extract message header info
        if(msg_Parse_Header(recv_msg, &msg_recv_type, &msg_recv_size))
        {
            printf("Server received invalid header from client\n");
            return -1;
        }

        // read rest of message
        if(tcp_Recv(remote_socket, recv_msg + MSG_HEADER_OFFSET, msg_recv_size, 5))
        {
            printf("ERROR occurred while receiving message body\n");
            return -1;
        }

        // extract post message
        char author[4096];
        char title[4096];
        char contents[4096];

        if(msg_Parse_PostRequest(recv_msg, author, title, contents))
        {
            printf("ERROR while parsing Post request message");
            return -1;
        }

        // print debug message
        printf("Server received post: author = %s, title = %s, contents = %s\n", author, title, contents);

        // build response
        msg_Build_PostResponse(send_msg);

        // send response
        if(tcp_Send(remote_socket, send_msg, MSG_HEADER_OFFSET, 5))
        {
            printf("ERROR occurred while accepting connection\n");
            return -1;
        }

        // disconnect from client
        tcp_Disconnect(remote_socket);
    }

    return EXIT_SUCCESS;
}