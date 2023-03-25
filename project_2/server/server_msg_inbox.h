#ifndef SERVER_MSG_OUTBOX_H
#define SERVER_MSG_OUTBOX_H

int serverMsgInbox_Insert(char* msg_to_insert);
int serverMsgInbox_WaitFor(int timeout_sec, int msg_id, char* out_msg);

#endif // SERVER_MSG_OUTBOX_H
