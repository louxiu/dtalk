
/**
 * @file   message.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Wed Oct 23 02:26:50 2013
 * 
 * @brief  
 * 
 * 
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "packet.h"

/* TODO: time, crypt, auth, DH exchange */
/*
  message: [[header][data]]
  header : [[version][type]]
  data   : [data]

  type of data:
  join                   : [[username] [password]]
  login                  : [[username] [password]]
  logout                 : []
  join room              : [room name]
  leave room             : [room name]
  message                : [[room name] [data]]
  list rooms             : []
  list users             : [room name]
  create room            : [[room name][description]]
  delete room            : [room name]  
  list users             : [room name]  
  join success           : []
  join failed            : [data]
  login success          : []
  login failed           : [data]
  logout success         : []
  join room success      : [desc]
  join room failed       : [data]
  list rooms success     : [room name]
  list users success     : [user name]
  list rooms failed      : [data]
  list users failed      : [data]
  create room success    : []
  create room failed     : [data]
  delete room success    : []
  delete room failed     : [data]
  
*/
/* NOTE: join == register */

#define VERSION_SIZE 1
#define TYPE_SIZE    1
#define DATA_SIZE    (MAX_PACKET_SIZE  - VERSION_SIZE - TYPE_SIZE)
#define HEADER_SIZE  (VERSION_SIZE + TYPE_SIZE)

#define VERSION_OFFSET 0
#define TYPE_OFFSET    (VERSION_OFFSET + VERSION_SIZE)
#define DATA_OFFSET    (TYPE_OFFSET + TYPE_SIZE)

#define MAX_USERNAME_SIZE 10 
#define MAX_PASSWORD_SIZE 10
#define MAX_ROOMNAME_SIZE 10
#define MAX_DESC_SIZE     100

#define VERSION '1'

typedef struct message_t message_t;

typedef enum message_type{
    INIT,                       /* initalize  */
    JOIN,               //user regist   
    LOGIN,              //user login
    LOGOUT,             //user logout
    JOIN_ROOM,          //join room
    LEAVE_ROOM,         //leave room
    MESSAGE,            //send message
    LIST_ROOMS,         //list all rooms
    LIST_USERS,         //list a room's all users
    CREATE_ROOM,
    DELETE_ROOM,
    JOIN_SUCCESS,    
    JOIN_FAILED,
    LOGIN_SUCCESS,    
    LOGIN_FAILED,
    LOGOUT_SUCCESS,
    JOIN_ROOM_SUCCESS,
    JOIN_ROOM_FAILED,
    LEAVE_ROOM_SUCCESS,
    LEAVE_ROOM_FAILED,
    LIST_ROOMS_SUCCESS,
    LIST_USERS_SUCCESS,
    LIST_ROOMS_FAILED,
    LIST_USERS_FAILED,
    CREATE_ROOM_SUCCESS,
    CREATE_ROOM_FAILED,  
    DELETE_ROOM_SUCCESS,
    DELETE_ROOM_FAILED,
    ECHO,                       /* for test */
    UNKNOWN_TYPE
}message_type;

extern char *message_type_array[];

struct message_t {
    char (*get_version)(message_t *this);
    void (*set_version)(message_t *this, char version);

    message_type (*get_type)(message_t *this);
    void  (*set_type)(message_t *this, message_type type);
    
    char *(*get_data)(message_t *this);    
    void  (*set_data)(message_t *this, char *data);

    /* used only in login and join */
    void (*get_username_password)
    (message_t *this, char *username, char *password);

    void  (*set_username_password)
    (message_t *this, char *username, char *password);

    /* used only in create romm */
    void (*get_room_desc)
    (message_t *this, char *room, char *desc);

    void  (*set_room_desc)
    (message_t *this, char *room, char *desc);
    
    int (*get_fd)(message_t *this);
    void (*set_fd)(message_t *this, int fd);
    
    packet_t *(*get_packet)(message_t *this);
    
    /* destroys a message and all including objects. */
    void (*destroy)(message_t *this);
};

message_t *message_create_from_packet(packet_t *packet);
message_t *message_create();

#endif /* MESSAGE_H_ */
