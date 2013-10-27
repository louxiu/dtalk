#ifndef DEAL_REPLY_H_
#define DEAL_REPLY_H_

#include "dtalk_c_room.h"

void process_recv_buf(room_handle_t *room_handle, char *recv_buf, int bufsize);
void process_message(room_handle_t *room_handle, message_t *message);

/* the aciton client do */
void join_success(room_handle_t *room_handle, message_t *message);

void join_failed(room_handle_t *room_handle, message_t *message);

void login_success(room_handle_t *room_handle, message_t *message);

void login_failed(room_handle_t *room_handle, message_t *message);

void logout_success(room_handle_t *room_handle, message_t *message);

void join_room_success(room_handle_t *room_handle, message_t *message);

void join_room_failed(room_handle_t *room_handle, message_t *message);

void list_rooms_con_success(room_handle_t *room_handle, message_t *message);

void list_rooms_end_success(room_handle_t *room_handle, message_t *message);

void list_users_con_success(room_handle_t *room_handle, message_t *message);

void list_users_end_success(room_handle_t *room_handle, message_t *message);

void list_rooms_failed(room_handle_t *room_handle, message_t *message);

void list_users_failed(room_handle_t *room_handle, message_t *message);

void unknow_type(room_handle_t *room_handle, message_t *message);

#endif /* DEAL_REPLY_H_ */
