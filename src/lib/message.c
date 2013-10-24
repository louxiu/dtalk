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
    "JOIN_SUCCESS",    
    "JOIN_FAILED",
    "LOGIN_SUCCESS",    
    "LOGIN_FAILED",
    "LOGOUT_SUCCESS",
    "JOIN_ROOM_SUCCESS",
    "JOIN_ROOM_FAILED",
    "LIST_ROOMS_CON_SUCCESS",
    "LIST_ROOMS_END_SUCCESS",
    "LIST_USERS_CON_SUCCESS",
    "LIST_USERS_END_SUCCESS",
    "LIST_ROOMS_FAILED",
    "LIST_USERS_FAILED" 
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

static void get_username(private_message_t *this, char *username)
{
    memcpy(username, this->data+(DATA_OFFSET), MAX_USERNAME_SIZE);
}

static void set_username(private_message_t *this, char *username)
{
    memcpy(this->data+(DATA_OFFSET), username, MAX_USERNAME_SIZE);
}

static void get_password(private_message_t *this, char *password)
{
    memcpy(password, this->data+(DATA_OFFSET+MAX_USERNAME_SIZE),
                  MAX_PASSWORD_SIZE);    
}

static void set_password(private_message_t *this, char *password)
{
    memcpy(this->data+(DATA_OFFSET+MAX_USERNAME_SIZE), password,
           MAX_PASSWORD_SIZE);
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

    this->public.get_username = (void (*)(message_t *message, char *username))get_username;
    this->public.set_username =
        (void (*)(message_t *message, char *username))set_username;

    this->public.get_password = (void (*)(message_t *message, char *password))get_password;
    this->public.set_password = (void (*)(message_t *message, char *password))set_password;
    
    this->public.get_packet =
        (packet_t *(*)(message_t *this))get_packet;
        
    this->public.destroy = (void (*)(message_t *message))destroy;

    this->data = malloc(DATA_SIZE * sizeof(char));
    
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
    
    this->version = '0';
    this->type    = INIT;
    this->fd      = -1;
    
    return &this->public;
}
