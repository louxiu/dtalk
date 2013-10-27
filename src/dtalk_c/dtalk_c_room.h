#ifndef DTALK_C_ROOM_H_
#define DTALK_C_ROOM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "color_print.h"

/* user list */
typedef struct user_t{
	char user_name[MAX_USERNAME_SIZE];
	struct user_t *next;
}user_t;

/* joined room list*/
typedef struct use_room_t {
	char room_name[MAX_ROOMNAME_SIZE];
	struct user_t *user_list;
	struct use_room_t *next;
}use_room_t;

/* all room list*/
typedef struct all_room_t {
	char room_name[MAX_ROOMNAME_SIZE];
	struct all_room_t *next;
}all_room_t;

typedef enum status_t{
    OFFLINE,
	ONLINE
}status_t;

typedef struct room_handle_t {
    struct use_room_t *use_room_list;
    struct all_room_t *all_room_list;
    struct use_room_t *cur_room;
	/* 0: offine 1:online */
	status_t status;
}room_handle_t;

/* call when login */
void room_init(room_handle_t *room_handle);

/* call when logout */
void room_destory(room_handle_t *room_handle);

void room_join(room_handle_t *room_handle, char *room_name);

void room_logout(room_handle_t *room_handle, char *room_name);

void room_update_add(room_handle_t *room_handle, char *room_name,
					 char *user_name);

void room_update_del(room_handle_t *room_handle, char *room_name,
					 char *user_name);

void room_list_add(room_handle_t *room_handle, char *room_name);

void room_list_del(room_handle_t *room_handle, char *room_name);

#endif /* DTALK_C_ROOM_H_ */
