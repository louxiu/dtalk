#ifndef _deal_request_h_
#define _deal_request_h_

#include "message.h"
#include "dtalk_c_room.h"

typedef struct msg_list_t {
	message_t *message;
	struct msg_list_t *next;
}msg_list_t;

extern pthread_mutex_t fd_mutex;
extern pthread_mutex_t msg_mutex;

/* construct a message based on input */
message_t *construct_message(room_handle_t *room_handle, char *input,
							 int size, int client_fd);

/* add message to msglist */
int add_msglist(msg_list_t **msgList, message_t *message);
/* get message from msglist */
message_t *remove_msglist(msg_list_t **msgList);

/* send login | logout | list | message to server*/
void send_login(int client_fd, char *username, char *password);
void send_logout(int client_fd, char *username, char *password);
void send_list_room(int client_fd, char *room_name);
void send_list_user(int client_fd, char *room_name);

/* construct certain kinds of message */
void fill_login(message_t *message, int client_fd, char *username,
				char *password);
void fill_logout(message_t *message, int client_fd);

void fill_join_room(message_t *message, int client_fd, char *room_name);
void fill_leave_room(message_t *message, int client_fd, char *room_name);
void fill_listUser(message_t *message, int client_fd, char *room_name);
void fill_listRoom(message_t *message, int client_fd);
void fill_createRoom(message_t *message, int client_fd, char *room_name);
void fill_deleteRoom(message_t *message, int client_fd, char *room_name);
//void fill_message(message_t *message, int client_fd, char *content);

#endif
