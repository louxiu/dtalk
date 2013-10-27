#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "packet.h"
#include "deal_reply.h"
#include "message.h"
#include "color_print.h"

void delete_room_success(room_handle_t *room_handle, message_t *message)
{
    /* do nothing */
}

void delete_room_failed(room_handle_t *room_handle, message_t *message)
{
    /* do nothing */
}

void create_room_failed(room_handle_t *room_handle, message_t *message)
{
    /* do nothing */
}

void create_room_success(room_handle_t *room_handle, message_t *message)
{
    /* do nothing */
}

void join_success(room_handle_t *room_handle, message_t *message)
{
    return;
    /* get room name */
    char room_name[MAX_ROOMNAME_SIZE];
    memset(room_name, 0, MAX_ROOMNAME_SIZE);
    //may be wrong，do set '\0' at last char
    message->get_room_desc(message, room_name, NULL);
    if (strlen(room_name) <= 0)
        err_print("room_name is null");

    room_join(room_handle, room_name);
}

void join_failed(room_handle_t *room_handle, message_t *message)
{
    //do nothing
}
void login_success(room_handle_t *room_handle, message_t *message)
{
    inf_print("login success");
    room_handle->status = ONLINE;
}

void login_failed(room_handle_t *room_handle, message_t *message)
{
    err_print("login failed");
}
void logout_success(room_handle_t *room_handle, message_t *message)
{
    err_print("logout success");
    //free room_handle->(useroom & allroom & userlist)
    room_handle->status = OFFLINE;
    room_destory(room_handle);
}
void join_room_success(room_handle_t *room_handle, message_t *message)
{
    inf_print(message->get_data(message));
    if (strlen(message->get_data(message)) <= 0){
        err_print("room_name is null");
    }

    room_join(room_handle, message->get_data(message));    
}
void join_room_failed(room_handle_t *room_handle, message_t *message)
{

}

void leave_room_success(room_handle_t *room_handle, message_t *message)
{
    inf_print(message->get_data(message));
    if (strlen(message->get_data(message)) <= 0){
        err_print("room_name is null");
    }

    room_logout(room_handle, message->get_data(message));
}
void leave_room_failed(room_handle_t *room_handle, message_t *message)
{

}
void list_rooms_success(room_handle_t *room_handle, message_t *message)
{
    /* get room name */
    /* char room_name[MAX_ROOMNAME_SIZE]; */
    /* memset(room_name, 0, MAX_ROOMNAME_SIZE); */
    //may be wrong，do set '\0' at last char
    
}
void list_users_success(room_handle_t *room_handle, message_t *message)
{
    /* get room name */
    char room_name[MAX_ROOMNAME_SIZE];
    memset(room_name, 0, MAX_ROOMNAME_SIZE);
    /* get user name */
    char user_name[MAX_USERNAME_SIZE];
    memset(user_name, 0, MAX_USERNAME_SIZE);
    //may be wrong，do set '\0' at last char
    message->get_room_desc(message, room_name, user_name);
    if (strlen(room_name) <= 0 || strlen(user_name) <= 0)
        err_print("room_name is null\n");

    room_update_add(room_handle, room_name, user_name);
   
}

void list_rooms_failed(room_handle_t *room_handle, message_t *message)
{
    //log
}
void list_users_failed(room_handle_t *room_handle, message_t *message)
{
    //log
}
void unknow_type(room_handle_t *room_handle, message_t *message)
{
    //write into log
}


void process_message(room_handle_t *room_handle, message_t *message)
{
    /* dirty work start */
    switch(message->get_type(message)) 
    {
        case JOIN_SUCCESS:
        {
            join_success(room_handle, message);
            break;
        }
        case JOIN_FAILED:
        {
            join_failed(room_handle, message);
            break;
        }
        case LOGIN_SUCCESS:
        {
            login_success(room_handle, message);
            break;
        }
        case LOGIN_FAILED:
        {
            login_failed(room_handle, message);            
            break;
        }
        case LOGOUT_SUCCESS:
        {
            logout_success(room_handle, message);
            break;
        }
        case JOIN_ROOM_SUCCESS:
        {
            join_room_success(room_handle, message);
            break;
        }
        case JOIN_ROOM_FAILED:
        {
            join_room_failed(room_handle, message);
            break;
        }
        case LIST_ROOMS_SUCCESS:
        {
            list_rooms_success(room_handle, message);
            break;
        }
        case LIST_USERS_SUCCESS:
        {
            list_users_success(room_handle, message);
            break;
        }
        case LIST_ROOMS_FAILED:
        {
            list_rooms_failed(room_handle, message);
            break;
        }
        case LIST_USERS_FAILED:
        {
            list_users_failed(room_handle, message);
            break;
        }
        case CREATE_ROOM_SUCCESS:
        {
            create_room_success(room_handle, message);
        }
        case CREATE_ROOM_FAILED:
        {
            create_room_failed(room_handle, message);
        }
        case DELETE_ROOM_SUCCESS:
        {
            delete_room_success(room_handle, message);
        }
        case DELETE_ROOM_FAILED:
        {
            delete_room_failed(room_handle, message);
        }
        default:
            unknow_type(room_handle, message);
    }
}

void process_recv_buf(room_handle_t *room_handle, char *recv_buf, int bufsize)
{
    //printf()
    /* process recv data */
    packet_t * packet = packet_create();
    packet->set_size(packet, bufsize);
    packet->set_payload(packet, recv_buf);

    message_t *message = message_create_from_packet(packet);
    packet->destroy(packet);

    /* check the type of message */
    process_message(room_handle, message);
    char version = message->get_version(message);
    message_type msg_type = message->get_type(message);
    char *data = message->get_data(message);

    message->destroy(message);
}
