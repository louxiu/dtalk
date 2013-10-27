/**
 * @file   online_rooms.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Fri Oct 25 15:20:02 2013
 * 
 * @brief  
 * 
 * 
 */
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "user.h"
#include "room.h"
#include "online_rooms.h"
#include "online_users.h"
#include "utils.h"
#include "message.h"

/* online users vector */
extern online_users_t *online_users;
extern database_t *db;

#define MAX_ONLINE_ROOMS_TABLE 1024

typedef struct private_online_rooms_t private_online_rooms_t;

struct private_online_rooms_t {
    online_rooms_t public;
    /* head node store nothing */
    online_room_lnode online_rooms[MAX_ONLINE_ROOMS_TABLE];
};

static int add_room(private_online_rooms_t *this, char *room_name,
                    int user_fd, char *desc)
{
    if (user_fd == 0){
        /* this is the load room operation, do not add to database */
    } else {
        /* this is the create room operation, add to database */
        /* TODO: add to database */
        room_t room;
        strncpy(room.roomname, room_name, MAX_ROOMNAME_SIZE-1);
        online_user_t *one_user = online_users->get_user(online_users, user_fd);
        strncpy(room.owner, one_user->a_user.username, MAX_USERNAME_SIZE-1);
        strncpy(room.desc, desc, MAX_DESC_SIZE-1);
        
        if(db->create_room(db, &room) == -1){
            /* create room failed */
            return -1;
        }
    }
    /* create a new room and initalize the value*/
    online_room_t *new_room = malloc(sizeof(online_room_t));
    if (new_room == NULL){
        log_ret("add room: malloc failed");
        exit(EXIT_FAILURE);
    }
    new_room->user_list_head = NULL;
    strncpy(new_room->a_room.roomname, room_name, MAX_ROOMNAME_SIZE - 1);
    strncpy(new_room->a_room.desc, room_name, MAX_DESC_SIZE - 1);    
    online_user_t *this_user = online_users->get_user(online_users, user_fd);
    strncpy(new_room->a_room.owner, this_user->a_user.username,
            MAX_USERNAME_SIZE-1);

    /* TODO: check malloc return value */
    online_room_lnode *new_room_p = malloc(sizeof(online_room_lnode));
    new_room_p->p = new_room;
    new_room_p->next = NULL;
    
    unsigned int id = BKDRHash(room_name, MAX_ONLINE_ROOMS_TABLE);

    /* the head node is empty */
    new_room_p->next = this->online_rooms[id].next;
    this->online_rooms[id].next = new_room_p;
    
    return 0;
}

/* TODO: load rooms from database */
static int load_rooms(private_online_rooms_t *this, database_t *db)
{
    int row = 0;
    room_t *default_room = db->list_rooms(db, &row);
    room_t *save_room = default_room; 
    log_ret("row = %d", row);
    int i = 0;
    for(i = 0; i < row; ++i){
        add_room(this, default_room->roomname, 0, default_room->desc);
        log_ret("load_rooms: %s", default_room->roomname);
        default_room++;
    }
    free(save_room);
    return 0;
}

/* TODO: delete room support */
static int delete_room(private_online_rooms_t *this, char *room_name,
                       int user_fd)
{
    /* TODO: match the owner of the room */
    /* TODO: delete room at table */
    /* TODO: delete room at database */
}
static online_room_lnode *find_room(private_online_rooms_t *this,
                                    char *room_name)
{
    unsigned int id = BKDRHash(room_name, MAX_ONLINE_ROOMS_TABLE);
    online_room_lnode *target = this->online_rooms[id].next;

    while(target != NULL){
        if(strncmp(target->p->a_room.roomname, room_name,
                   MAX_ROOMNAME_SIZE) == 0){
            return target;
        }
        target = target->next;
    }
    return NULL;
}
static int list_users_in_room(private_online_rooms_t *this, char *room_name)
{
    online_room_lnode *target = find_room(this, room_name);
    if (target != NULL){
        online_user_lnode *user_list_head = target->p->user_list_head;
        while (user_list_head != NULL){
            log_ret("list_users_in_room: %s",
                    user_list_head->p->a_user.username);
            user_list_head = user_list_head->next;
        }
    } else {
        log_ret("room does not exists");
        return -1;
    }
    
    return 0;
}

static int list_rooms(private_online_rooms_t *this, message_t *message)
{
    int i;
    for(i = 0; i < MAX_ONLINE_ROOMS_TABLE; ++i){
        /* the head node is empty */
        online_room_lnode *node = this->online_rooms[i].next;
        while (node != NULL){
            log_ret("list_rooms: %s", node->p->a_room.roomname);
            /* to seperate packets */
            usleep(1000);
            message->set_data(message, node->p->a_room.roomname);
            packet_t *packet = message->get_packet(message);
            packet->packet_send(packet);
            packet->destroy(packet);
            
            node = node->next;
        }
    }
    return 0;
}

/* the user join the room  */
static int speak_at_room(private_online_rooms_t *this, char *room,
                         int user_fd)
{
    /* TODO: make sure the user is at that room */
    online_room_lnode *target = find_room(this, room);
    
    /* forward the message to all users in that room */
    if (target != NULL){
        online_user_lnode *user_list_head = target->p->user_list_head;
        while (user_list_head != NULL){
            log_ret("speak at room: send message to %d",
                    user_list_head->p->a_user.fd);
            user_list_head = user_list_head->next;
        }
    } else {
        log_ret("room does not exists");
        return -1;
    }
    return 0;
}

/* used by join_room FUNCTION */
static online_room_lnode *add_user_to_room(private_online_rooms_t *this,
                                           char *room, int user_fd)
{
    /* NOTE: this is not user join command, just part of it */
    online_room_lnode *target = find_room(this, room);
    
    /* forward the message to all users in that room */
    if (target != NULL){
        online_user_lnode *new_node = malloc(sizeof(online_user_lnode));
        if (new_node == NULL){
            log_ret("add_user_to_room: malloc failed");
            exit(EXIT_FAILURE);
        }
        new_node->p = online_users->get_user(online_users, user_fd);
        new_node->next = NULL;
        
        
        if (target->p->user_list_head != NULL){
            /* insert to first */
            new_node->next = target->p->user_list_head->next;
            target->p->user_list_head = new_node;
        } else {
            /* the list pointer is empty, just point to the new one */
            target->p->user_list_head = new_node;
        }
    } else {
        log_ret("room does not exists");
        return NULL;
    }
    return target;
}

/* used by leave_room and logout FUNCTION */
static int delete_user_from_room(private_online_rooms_t *this,
                                 online_room_lnode *target, int user_fd)
{
    /* online_room_lnode *target = find_room(this, room); */
    online_user_lnode *user_list_head = target->p->user_list_head;

    /* handle the head */
    if (user_list_head != NULL){
        if (user_list_head->p->a_user.fd == user_fd){
            free(user_list_head);
            target->p->user_list_head = NULL;
            return 0;
        }
    } else {
        return -1;
    }

    while (user_list_head->next != NULL){
        if ((user_list_head->next)->p->a_user.fd == user_fd){
            online_user_lnode *save = user_list_head->next->next;
            free(user_list_head->next);
            user_list_head->next = save;
            return 0;
        }
        user_list_head = user_list_head->next;
    }
    
    return -1;
}

static void destroy(private_online_rooms_t *this)
{
    /* release every room node and user pointer node*/
    int i;
    for(i = 0; i < MAX_ONLINE_ROOMS_TABLE; ++i){
        /* the head node is empty */
        online_room_lnode *room_node = this->online_rooms[i].next;
        while (room_node != NULL){
            online_room_lnode *room_save = room_node->next;
            online_user_lnode *user_node = room_node->p->user_list_head;
            /* release every user pointer node */
            while(user_node != NULL){
                online_user_lnode *user_save = user_node->next;
                free(user_node);
                user_node = user_save;
            }
            free(room_node);
            room_node = room_save;
        }
    }

    free(this);
}

online_rooms_t *online_rooms_create()
{
    /* TODO; handle malloc failed */
    private_online_rooms_t *this = malloc(sizeof(private_online_rooms_t));
    if (this == NULL){
        log_ret("online rooms create: malloc failed");
        exit(EXIT_FAILURE);
    }

    /* initalize the online rooms hash table */
    int i = 0;
    for(i = 0; i < MAX_ONLINE_ROOMS_TABLE; ++i){
        this->online_rooms[i].next = NULL;
        this->online_rooms[i].p = NULL;
    }

    this->public.add_room =
        (int (*)(online_rooms_t *this, char *room_name,
                 int user_fd, char *desc))add_room;
    this->public.load_rooms =
        (int (*)(online_rooms_t *this, database_t *db))load_rooms;
    /* this->public.delete_room = */
    /*     (int (*)(online_rooms_t *this, char *room_name, int user_fd)) */
    /*     delete_room; */

    this->public.list_users_in_room =
        (int (*)(online_rooms_t *this, char *room))
        list_users_in_room;

    this->public.list_rooms =
        (int (*)(online_rooms_t *this, message_t *message))
        list_rooms;

    this->public.speak_at_room =
        (int (*)(online_rooms_t *this, char *room, int user_fd))
        speak_at_room;

    this->public.add_user_to_room =
        (online_room_lnode *(*)(online_rooms_t *this, char *room, int user_fd))
        add_user_to_room;

    this->public.delete_user_from_room =
        (int (*)(online_rooms_t *this, char *room, int user_fd))
        delete_user_from_room;
    
    this->public.destroy = (void (*)(online_rooms_t *this))destroy;

    return &this->public;
}
