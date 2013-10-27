/**
 * @file   message.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Wed Oct 23 02:27:12 2013
 * 
 * @brief  
 * 
 * 
 */
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "message.h"

typedef struct private_message_t private_message_t;

struct private_message_t {
    message_t public;
    char version;
    message_type type;
    char *data;
    int fd;
};

/* TODO: this is ugly and error-prone */
char *message_type_array[] = {
    "INIT",
    "JOIN",
    "LOGIN",
    "LOGOUT",
    "JOIN_ROOM",
    "LEAVE_ROOM",
    "MESSAGE",
    "LIST_ROOMS",
    "LIST_USERS",
    "CREATE_ROOM"
    "DELETE ROOM"
    "JOIN_SUCCESS",    
    "JOIN_FAILED",
    "LOGIN_SUCCESS",    
    "LOGIN_FAILED",
    "LOGOUT_SUCCESS",
    "JOIN_ROOM_SUCCESS",
    "JOIN_ROOM_FAILED",
    "LIST_ROOMS_SUCCESS",
    "LIST_USERS_SUCCESS",
    "LIST_ROOMS_FAILED",
    "LIST_USERS_FAILED",
    "CREATE_ROOM_SUCCESS",
    "CREATE_ROOM_FAILED",    
    "DELETE_ROOM_SUCCESS",
    "DELETE_ROOM_FAILED",
    "ECHO",
    "UNKNOWN_TYPE"
};

static char get_version(private_message_t *this)
{
    return this->version;
}

static void set_version(private_message_t *this, char version)
{
    this->version = version;
}

static message_type get_type(private_message_t *this)
{
    return this->type;
}

static void set_type(private_message_t *this, message_type type)
{
    this->type = type;
}

static char * get_data(private_message_t *this)
{
    return this->data;
}

static void set_data(private_message_t *this, char *data)
{
    strncpy(this->data, data, DATA_SIZE);
}

static void get_username_password(private_message_t *this,
                                  char *username, char *password)
{
    if (this->type == JOIN || this->type == LOGIN){
        /* TODO: error prone */
        sscanf(this->data, "%s %s", username, password);
    }
}

static void set_username_password(private_message_t *this,
                                  char *username, char *password)
{
    if (this->type == JOIN || this->type == LOGIN){
        /* TODO: error prone */
        sprintf(this->data, "%s %s", username, password);
    }
}

static void get_room_desc(private_message_t *this,
                          char *room, char *desc)
{
    if (this->type == CREATE_ROOM){
        /* TODO: error prone */
        sscanf(this->data+(DATA_OFFSET), "%s %s", room, desc);
    }
}

static void set_room_desc(private_message_t *this,
                          char *room, char *desc)
{
    if (this->type == CREATE_ROOM){
        /* TODO: error prone */
        sprintf(this->data+(DATA_OFFSET), "%s %s", room, desc);
    }
}

static int get_fd(private_message_t *this)
{
    return this->fd;
}

static void set_fd(private_message_t *this, int fd)
{
    this->fd = fd;
}

static packet_t * get_packet(private_message_t *this)
{
    packet_t *packet = packet_create();

    packet->set_fd(packet, this->fd);

    char *payload = packet->get_payload(packet);

    /* set size */
    /* printf ("%d\n", HEADER_SIZE); */
    /* uint32_t size = htonl(HEADER_SIZE); */
    /* memcpy(payload, &size, SIZE_SIZE); */

    /* set version */
    memcpy(payload+VERSION_OFFSET, &this->version, VERSION_SIZE);

    /* set type */
    memcpy(payload+TYPE_OFFSET,    &this->type, TYPE_SIZE);

    /* set payload */    
    memcpy(payload+DATA_OFFSET,    this->data, DATA_SIZE);

    packet->set_size(packet, HEADER_SIZE+strlen(this->data));
    
    return packet;
}

static void destroy(private_message_t *this)
{
    free(this->data);
    free(this);
}

static void parse_packet(private_message_t *this, packet_t *packet)
{
    char *payload = packet->get_payload(packet);
    
    this->version = *(payload + VERSION_OFFSET);
    this->type    = *(payload + TYPE_OFFSET);
    memcpy(this->data, (payload + DATA_OFFSET), DATA_SIZE);
    this->fd = packet->get_fd(packet);
}

/*
 * initalize the share part of message_create_from_packet and message_create
 * */
static private_message_t * message_create_init()
{
    private_message_t *this = malloc(sizeof(private_message_t));
    if (this == NULL){
        log_ret("message_create_init: malloc failed");
        exit(EXIT_FAILURE);
    }
    
    this->public.get_version = (char (*)(message_t *message))get_version;
    this->public.set_version =
        (void (*)(message_t *message, char version))set_version;

    this->public.get_type = (message_type (*)(message_t *message))get_type;
    this->public.set_type =
        (void (*)(message_t *message, message_type type))set_type;

    this->public.get_data = (char * (*)(message_t *message))get_data;
    this->public.set_data = (void (*)(message_t *message, char *data))set_data;

    this->public.get_fd = (int (*)(message_t *message))get_fd;
    this->public.set_fd =
        (void (*)(message_t *message, int fd))set_fd;

    this->public.get_username_password =
        (void (*)(message_t *message, char *username, char *password))
        get_username_password;

    this->public.set_username_password =
        (void (*)(message_t *message, char *username, char *password))
        set_username_password;

    this->public.get_room_desc =
        (void (*)(message_t *message, char *room, char *desc))
        get_room_desc;

    this->public.set_room_desc =
        (void (*)(message_t *message, char *room, char *desc))
        set_room_desc;
    
    this->public.get_packet =
        (packet_t *(*)(message_t *this))get_packet;
        
    this->public.destroy = (void (*)(message_t *message))destroy;

    this->type = INIT;
    this->version = '1';
    
    this->data = malloc(DATA_SIZE * sizeof(char));
    if (this->data == NULL){
        log_ret("message_create_init: malloc failed");
        exit(EXIT_FAILURE);
    }
    memset(this->data, 0, DATA_SIZE);
    
    return this;
}

message_t *message_create_from_packet(packet_t *packet)
{
    if (packet->get_size(packet) < HEADER_SIZE){
        /* malformed packet */
        return NULL;
    }

    private_message_t *this = message_create_init();
    
    parse_packet(this, packet);

    return &this->public;
}

message_t *message_create()
{
    private_message_t *this = message_create_init();
    
    this->version = '1';
    this->type    = INIT;
    this->fd      = -1;
    
    return &this->public;
}
