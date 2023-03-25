#ifndef SERVER_REQUEST_HANDLER_H
#define SERVER_REQUEST_HANDLER_H

        case MSG_TYPE_POST_REQUEST :
            handlePostRequest(remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_READ_REQUEST :
            handleReadRequest(remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_CHOOSE_REQUEST :
            handleChooseRequest(remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_REPLY_REQUEST :
            handleReplyRequest(remote_socket, rcvd_msg);

#endif // SERVER_REQUEST_HANDLER_H
