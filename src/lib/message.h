
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
  message                : [data]
  list rooms             : []
  list users             : [room name]
  join success           : []
  join failed            : [data]
  login success          : []
  login failed           : [data]
  logout success         : []
  join room success      : []
  join room failed       : [data]
  list rooms success     : [[room name] [room name] ...]
  list users success     : [[user name] [room name] ...]
  list rooms failed      : [data]
  list users failed      : [data]
*/

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

typedef struct message_t message_t;

typedef enum message_type{
    INIT,                       /* initalize  */
    JOIN,    
    LOGIN,
    LOGOUT,
    JOIN_ROOM,
    LEAVE_ROOM,
    MESSAGE,
    LIST_ROOMS,
    LIST_USERS,
    JOIN_SUCCESS,    
    JOIN_FAILED,
    LOGIN_SUCCESS,    
    LOGIN_FAILED,
    LOGOUT_SUCCESS,
    JOIN_ROOM_SUCCESS,
    JOIN_ROOM_FAILED,
    LIST_ROOMS_SUCCESS,
    LIST_USERS_SUCCESS,
    LIST_ROOMS_FAILED,
    LIST_USERS_FAILED,
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

    /* used in login and join*/
    void (*get_username)(message_t *this, char *username);
    void  (*set_username)(message_t *this, char *username);

    /* used in login and join*/
    void (*get_password)(message_t *this, char *password);
    void  (*set_password)(message_t *this, char *password);

    int (*get_fd)(message_t *this);
    void (*set_fd)(message_t *this, int fd);
    
    packet_t *(*get_packet)(message_t *this);
    
    /* destroys a message and all including objects. */
    void (*destroy)(message_t *this);
};

message_t *message_create_from_packet(packet_t *packet);
message_t *message_create();

#endif /* MESSAGE_H_ */
