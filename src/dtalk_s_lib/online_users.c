/**
 * @file   online_users.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Thu Oct 24 20:56:04 2013
 * 
 * @brief  
 * 
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "debug.h"
#include "user.h"
#include "room.h"
#include "online_users.h"
#include "online_rooms.h"

/* online rooms table */
extern online_rooms_t *online_rooms;

typedef struct private_online_users_t private_online_users_t;

#define MAX_ONLINE_USERS 1024

struct private_online_users_t {
    online_users_t public;
    online_user_t online_users[MAX_ONLINE_USERS];
};

/* check wether the user has joined the room or not */
static int login(private_online_users_t *this, int user_fd, char *user_name)
{
    if (user_fd >= MAX_ONLINE_USERS){
        /*  can not hanle more , I am tired */
        return -1;
    }
    
    this->online_users[user_fd].a_user.online = 1;
    strncpy(this->online_users[user_fd].a_user.username,
            user_name, MAX_USERNAME_SIZE - 1);
    this->online_users[user_fd].room_list_head = NULL;
}

/* check wether the user has joined the room or not */
static int logout(private_online_users_t *this, int user_fd)
{
    this->online_users[user_fd].a_user.online = 0;
    /* erase the user name */
    memset(this->online_users[user_fd].a_user.username, 0,
           MAX_USERNAME_SIZE - 1);
    struct online_room_lnode *p = this->online_users[user_fd].room_list_head;
    struct online_room_lnode *p_next = NULL;
    
    while(p != NULL){
        p_next = p->next;
        /* leave from every joined room */
        online_rooms->delete_user_from_room(online_rooms,
                                            p->p->a_room.roomname,
                                            user_fd);
        /* free this joined room node */
        free(p);
        p = p_next;
    }
    this->online_users[user_fd].room_list_head = NULL;
    return 0;
}
static online_user_t *get_user(private_online_users_t *this, int user_fd)
{
    return &this->online_users[user_fd];
}
/* check wether the user has joined the room or not */
static int check_joined_room(private_online_users_t *this,
                             int user_fd, char *room)
{
    assert(user_fd < MAX_ONLINE_USERS);
    struct online_room_lnode *p = this->online_users[user_fd].room_list_head;
    struct online_room_lnode *p_next = NULL;
    
    while(p != NULL){
        p_next = p->next;
        /* leave from every joined room */
        if (strcmp(p->p->a_room.roomname, room) == 0){
            return 0;
        }
        p = p_next;
    }

    return -1;
}

/* the user want to join the room  */
static int join_room(private_online_users_t *this, int user_fd, char *room)
{
    char *username = this->online_users[user_fd].a_user.username;
    online_room_lnode *joined_room =
        online_rooms->add_user_to_room(online_rooms, room, user_fd);
    
    if (joined_room == NULL){
        log_ret("join rooms failed: %s %s", username, room);
        return -1;
    }
    
    online_room_lnode *tmp = malloc(sizeof(online_room_lnode));
    if (tmp == NULL){
        log_ret("join room, malloc failed");
        exit(EXIT_FAILURE);
    }
    tmp->p = joined_room->p;

    /* add the new joined room to joined room list */
    tmp->next = this->online_users[user_fd].room_list_head;
    this->online_users[user_fd].room_list_head = tmp;

    return 0;
}

/* the user leave the room  */
static int leave_room(private_online_users_t *this, int user_fd, char *room)
{
    /* TODO: find the room node in user's room list */
    /* TODO: remove the user node from that room's user list */

    /* TODO: remove the room node from user's room list */
    struct online_room_lnode *p = this->online_users[user_fd].room_list_head;
    struct online_room_lnode *p_next = NULL;

    if (p == NULL){
        return -1;
    } else {
        p_next = p->next;
        if (strcmp(p->p->a_room.roomname, room) == 0){
            online_rooms->delete_user_from_room(online_rooms,
                                                p,
                                                user_fd);
            this->online_users[user_fd].room_list_head = p_next;
            free(p);
            return 0;
        }
        while(p_next != NULL){
            /* leave from every joined room */
            if (strcmp(p_next->p->a_room.roomname, room) == 0){
                online_rooms->delete_user_from_room(online_rooms,
                                                    p_next,
                                                    user_fd);
                p->next = p_next->next;
                /* free this joined room node */                
                free(p_next);
                return 0;
            }
            p = p_next;
            p_next = p_next->next;
        }
    }

    return -1;
}

static void destroy(private_online_users_t *this)
{
    /* TODO: release every room node */
    int i;
    for(i = 0; i < MAX_ONLINE_USERS; ++i){
        online_room_lnode *room_node = this->online_users[i].room_list_head;
        while(room_node != NULL){
            online_room_lnode *room_save = room_node->next;
            free(room_node);
            room_node = room_save;
        }
    }
    free(this);
}

online_users_t *online_users_create()
{
    private_online_users_t *this = malloc(sizeof(private_online_users_t));

    if (this == NULL){
        log_ret("online_users_create: malloc failed");
        exit(EXIT_FAILURE);
    }
    /* initalize the vector */
    int i = 0;
    for(i = 0; i < MAX_ONLINE_USERS; ++i){
        /* this->online_users[i].room_list = NULL; */
        this->online_users[i].a_user.online = 0;
        this->online_users[i].a_user.fd = i;
    }
    
    this->public.login =
        (int (*)(online_users_t *this, int user_fd, char *user_name))login;

    this->public.logout =
        (int (*)(online_users_t *this, int user_fd))logout;

    this->public.get_user =
        (online_user_t *(*)(online_users_t *this, int user_fd))get_user;
    
    this->public.check_joined_room =
        (int (*)(online_users_t *this, int user_fd, char *room))
        check_joined_room;

    this->public.join_room =
        (int (*)(online_users_t *this, int user_fd, char *room))join_room;

    this->public.leave_room =
        (int (*)(online_users_t *this, int user_fd, char *room))leave_room;
    
    this->public.destroy = (void (*)(online_users_t *this))destroy;

    return &this->public;
}
