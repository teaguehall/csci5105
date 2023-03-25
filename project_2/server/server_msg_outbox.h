#ifndef SERVER_MSG_OUTBOX_H
#define SERVER_MSG_OUTBOX_H

int serverMsgOutbox_Insert(char* msg_to_insert);
int serverMsgOutbox_ReadAndRemove(char* out_msg);

#endif // SERVER_MSG_OUTBOX_H
