/**
 * @file   online_rooms.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Fri Oct 25 09:49:40 2013
 * 
 * @brief  
 * 
 * 
 */

#ifndef ONLINE_ROOMS_H_
#define ONLINE_ROOMS_H_

#include "room.h"
#include "database.h"

typedef struct online_room_t{
    room_t a_room;
    /* NOTE: solve the circle reference problem */    
    struct online_user_lnode *user_list_head;
}online_room_t;

/* TODO: use kernel list.h instead of normal linked list */
typedef struct online_room_lnode{
    online_room_t *p;
    struct online_room_lnode *next;
}online_room_lnode;

typedef struct online_rooms_t online_rooms_t;

struct online_rooms_t {

    /* NOTE: COMMAND may depends on other function */
    
    /* used by create room COMMAND */
    int (*add_room)(online_rooms_t *this, char *room_name, int user_fd,
                    char *desc);

    int (*load_rooms)(online_rooms_t *this, database_t *db);
    /* TODO: used by delete room COMMAND */
    /* int (*delete_room)(online_rooms_t *this, */
    /*                    char *room_name, int user_fd); */

    /* used by list room COMMAND */
    int (*list_users_in_room)(online_rooms_t *this, char *room_name);
    
    /* used by list rooms COMMAND */
    int (*list_rooms)(online_rooms_t *this, message_t *message);

    /* used by message COMMAND */
    int (*speak_at_room)(online_rooms_t *this, char *room,
                                       int user_fd);
    
    /* used by join_room FUNCTION */
    online_room_lnode *(*add_user_to_room)(online_rooms_t *this, char *room,
                                           int user_fd);

    /* used by leave_room and logout FUNCTION */
    int (*delete_user_from_room)(online_rooms_t *this, char *room,
                                  int user_fd);

    void (*destroy) (online_rooms_t *this);              

};

online_rooms_t *online_rooms_create();

#endif /* ONLINE_ROOMS_H_ */
