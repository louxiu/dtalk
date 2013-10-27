/**
 * @file   online_users.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Thu Oct 24 20:36:59 2013
 * 
 * @brief  the online users array
 * 
 * 
 */

#ifndef ONLINE_USERS_H_
#define ONLINE_USERS_H_

#include "user.h"

typedef struct online_user_t{
    user_t a_user;
    /* NOTE: solve the circle reference problem */
    struct online_room_lnode *room_list_head;
}online_user_t; 

/* TODO: use kernel list.h instead of normal linked list */
typedef struct online_user_lnode{
    online_user_t *p;
    struct online_user_lnode *next;
}online_user_lnode;

typedef struct online_users_t online_users_t;

struct online_users_t {

    /* NOTE: COMMAND may depends on other function */
    
    /* get all the rooms the user joined */
    /* char *(*get_joined_rooms)(online_users_t *online_users, int user_fd); */
    /* used by login COMMAND */
    int (*login)(online_users_t *this, int user_fd, char *username);

    /* used by logout COMMAND */
    int (*logout)(online_users_t *this, int user_fd);

    online_user_t *(*get_user)(online_users_t *this, int user_fd);
    
    /* check wether the user has joined the room or not */
    /* use by speak_at_room FUNCTION */
    int (*check_joined_room)(online_users_t *this, int user_fd, char *room);

    /* used by join room COMMAND */
    int  (*join_room)(online_users_t *this, int user_fd, char *room);

    /* used by leave room COMMAND */
    int  (*leave_room)(online_users_t *this, int user_fd, char *room);

    /* TODO: used by delete_room FUNCTION.*/
    /* int  (*delete_room_from_user_joined_rooms) */
    /* (online_users_t *this, int user_fd, char *room); */

    void  (*destroy) (online_users_t *this);              

};

online_users_t *online_users_create();

#endif /* ONLINE_USERS_H_ */
