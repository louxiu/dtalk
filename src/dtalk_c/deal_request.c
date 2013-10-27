#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "packet.h"
#include "color_print.h"
#include "deal_request.h"

#define COMMAND_LEN 1024

pthread_mutex_t fd_mutex;
pthread_mutex_t msg_mutex;

char global_username[MAX_USERNAME_SIZE] = {'\0'};
char global_password[MAX_PASSWORD_SIZE] = {'\0'};

message_t *remove_msglist(msg_list_t **msglist)
{
	
	msg_list_t *buf;
	message_t *message;
	pthread_mutex_lock(&msg_mutex);
	if (*msglist == NULL){
		return NULL;
	}
	else {
		buf = (*msglist)->next;
		/* get content */
		message = (*msglist)->message;
		free(*msglist);
		*msglist = buf;
	}
	pthread_mutex_unlock(&msg_mutex);

	return message;
}


int add_msglist(msg_list_t **msglist,
                  message_t *message)
{
	/* malloc a new msg and 
     * add into the msglist */
	msg_list_t *msg = (msg_list_t *)malloc \
                        (sizeof(msg_list_t));
	if (msg == NULL){
		err_print("malloc failed");
		exit(1);
	}
	msg->next = NULL;
	msg->message = message;

	/* add && msg_mutex */
	pthread_mutex_lock(&msg_mutex);
	if (*msglist == NULL){
		*msglist = msg;
	}
	else {
		msg_list_t *buf;
		buf = (*msglist)->next;
		(*msglist)->next = msg;
		msg->next = buf;
	}
	pthread_mutex_unlock(&msg_mutex);

	return 0;
}

/************************************************/
/* 
 * send login|logout|list message to server
 * 
 * */
void send_login(int client_fd, char *username, 
                char *password)
{
    /* create a logout message */
    message_t *message = message_create();
    message_type msg_type = LOGIN;
    message->set_version(message, VERSION);
    message->set_type(message, msg_type);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);

    /* change it into a packet */
    packet_t *packet = message->get_packet(message);

    /* send a packet */
    packet->packet_send(packet);

    /* destory packet & message */
    packet->destroy(packet);
    message->destroy(message);
}

void send_logout(int client_fd, char *username, 
                    char *password)
{
    /* create a logout message */
    message_t *message = message_create();
    message_type msg_type = LOGOUT;
    message->set_version(message, VERSION);
    message->set_type(message, msg_type);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);

    /* change it into a packet */
    packet_t *packet = message->get_packet(message);

    /* send a packet */
    packet->packet_send(packet);

    /* destory packet & message */
    packet->destroy(packet);
    message->destroy(message);
}
void send_list_room(int client_fd, 
                    char *room_name)
{
    /* create a logout message */
    message_t *message = message_create();
    message_type msg_type = LIST_ROOMS;
    message->set_version(message, VERSION);
    message->set_type(message, msg_type);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);

    /* change it into a packet */
    packet_t *packet = message->get_packet(message);

    /* send a packet */
    packet->packet_send(packet);

    /* destory packet & message */
    packet->destroy(packet);
    message->destroy(message);

}
void send_list_user(int client_fd, char *room_name)
{
    /* create a logout message */
    message_t *message = message_create();
    message_type msg_type = LIST_USERS;
    message->set_version(message, VERSION);
    message->set_type(message, msg_type);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);
    message->set_data(message, room_name);

    /* change it into a packet */
    packet_t *packet = message->get_packet(message);

    /* send a packet */
    packet->packet_send(packet);

    /* destory packet & message */
    packet->destroy(packet);
    message->destroy(message);
}

/******************************************************/
void fill_join_room(message_t *message, int client_fd, 
                    char *room_name)
{
    message->set_version(message, VERSION);
    message->set_type(message, JOIN_ROOM);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);
    message->set_data(message, room_name);
}

void fill_register(message_t *message, int client_fd, 
                char *username, char *password)
{
    message->set_version(message, VERSION);
    message->set_type(message, JOIN);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    username, password);
    /* printf("in register, username:%s, password:%s\n", username, password); */
}


void fill_login(message_t *message, int client_fd, 
                char *username, char *password)
{
    message->set_version(message, VERSION);
    message->set_type(message, LOGIN);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    username, password);

    /* printf("in login, username:%s, password:%s\n", username, password); */
}
void fill_logout(message_t *message, int client_fd)
{
    message->set_type(message, LOGOUT);
    message->set_fd(message, client_fd);
}

void fill_leave_room(message_t *message, int client_fd, 
                    char *room_name)
{
    message->set_version(message, VERSION);
    message->set_type(message, LEAVE_ROOM);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);
    message->set_data(message, room_name);
}
void fill_listUser(message_t *message, int client_fd, 
                    char *room_name)
{
    message->set_version(message, VERSION);
    message->set_type(message, LIST_USERS);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);
    message->set_data(message, room_name);
}
void fill_listroom(message_t *message, int client_fd)
{
    message->set_version(message, VERSION);
    message->set_type(message, LIST_ROOMS);
    message->set_fd(message, client_fd);
    message->set_username_password(message, global_username, global_password);
}
void fill_createRoom(message_t *message, int client_fd, 
                    char *room_name)
{
    message->set_version(message, VERSION);
    message->set_type(message, CREATE_ROOM);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);
    message->set_data(message, room_name);
}
void fill_deleteRoom(message_t *message, int client_fd, 
                    char *room_name)
{
    message->set_version(message, VERSION);
    message->set_type(message, DELETE_ROOM);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);
    //room name
}
void fill_message(room_handle_t *room_handle, message_t *message,
                  int client_fd, char *content)
{
    message->set_version(message, VERSION);
    message->set_type(message, MESSAGE);
    message->set_fd(message, client_fd);
    message->set_username_password(message, 
                    global_username, global_password);

    /* set room name and content like: "room_name chat_content" */
    char data[DATA_SIZE];
    sprintf(data, "%s %s", (room_handle->cur_room)->room_name, content);
    message->set_data(message, data);
}


void print_help()
{
    inf_print("-------------------------------------------------------------");
    inf_print("-/help                                                      -");
    inf_print("-/create roomname desc                                      -");
    inf_print("-/delete roomname                                           -");
    inf_print("-/register username password                                -");
    inf_print("-/login    username password                                -");
    inf_print("-/logout                                                    -");
    inf_print("-/listroom                                                  -");
    inf_print("-/list roomname                                             -");
    inf_print("-/join roomname                                             -");
    inf_print("-/leave roomname                                            -");
    inf_print("-------------------------------------------------------------");
}

message_t *construct_message(room_handle_t *room_handle, char *input,
                             int size, int client_fd)
{
	message_t *message = message_create();

    char command[COMMAND_LEN] = {'\0'};
    char other[COMMAND_LEN] = {'\0'};
    
    if (input[0] == '/'){
        char *pos = strstr(input+1, " ");
        if (pos != NULL){
            strncpy(other, pos+1, strlen(pos));
            strncpy(command, input+1, 
                    strlen(input) - strlen(pos) - 1);
        } else {
            strncpy(command, input+1, strlen(input)-1);
        }
        
        /* judge the kind of the command */
        if (strcmp(command, "help") == 0){
            /* NOTE: test confimed */
            print_help();
            goto err_handle;
        }
        else if (strcmp(command, "register") == 0){
            /* NOTE: test confimed */
            if (strlen(other) > 0){
                char username[MAX_USERNAME_SIZE] = {'\0'};
                char password[MAX_PASSWORD_SIZE] = {'\0'};
                /* printf("other:%s\n", other); */
                sscanf(other, "%s %s", username, password);
                /* printf("username:%s, password:%s\n", username, password); */
                /* inf_print(username); */
                /* inf_print(password); */
                if (strlen(username) <= 0 || strlen(password) <= 0){
                    err_print("command format error");
                    goto err_handle;
                }
                /* save the username and password */
                sprintf(global_username, "%s", username);
                sprintf(global_password, "%s", password);
                fill_register(message, client_fd, global_username,
                              global_password);
            }else{
                err_print("command format error");
                goto err_handle;
            }
        }
        else if (strcmp(command, "login") == 0){
            /* NOTE: test confimed */
            if (strlen(other) > 0){
                char username[MAX_USERNAME_SIZE] = {};
                char password[MAX_PASSWORD_SIZE] = {};
                /* printf("other:%s\n", other); */
                sscanf(other, "%s %s", username, password);
                /* printf("username:%s, password:%s\n", username, password); */
                /* inf_print(username); */
                /* inf_print(password); */
                if (strlen(username) <= 0 || strlen(password) <= 0){
                    goto err_handle;
                }
                sprintf(global_username, "%s", username);
                sprintf(global_password, "%s", password);
                fill_login(message, client_fd, username, password);
                /* printf("out fill, type:%d, content:%s\n", */
                /*        message->get_type(message), */
                /*        message->get_data(message)); */
            }else{
                err_print("command format error");
                goto err_handle;
            }
        }
        else if (strcmp(command, "exit") == 0){
            /* TODO: clean all up */
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(command, "logout") == 0){
            fill_logout(message, client_fd);
        }
        else if (strcmp(command, "join") == 0){
            //join room
            inf_print("join");
            inf_print(other);
            fill_join_room(message, client_fd, other);
        }
        else if (strcmp(command, "leave") == 0){
            //join room
            inf_print("leave room:");
            inf_print(other);
            fill_leave_room(message, client_fd, other);
        }
        else if (strcmp(command, "listroom") == 0){
            //
            inf_print("listroom");
            fill_listroom(message, client_fd);
        }
        else if (strcmp(command, "listuser") == 0){
            inf_print("list user");
            fill_listUser(message, client_fd, other);
        }
        else if (strcmp(command, "createroom") == 0){
            inf_print("create room");
            fill_createRoom(message, client_fd, other);
        }
        else if (strcmp(command, "deleteroom") == 0){
            inf_print("delete room");
            fill_deleteRoom(message, client_fd, other);
        }
        else {
            //do nothing
        }
    } else {
        if (room_handle->status == 0){
            /* offline now */
            goto err_handle;
        }
        /* inf_print("send message"); */
        if (room_handle->cur_room == NULL) {
            printf("please join one room first!\n");
            goto err_handle;
        }
        fill_message(room_handle, message, client_fd, input);
        /* printf("input:%s\n", input); */
        /* printf("input:%s\n", message->get_data(message)); */
    }
   
	return message;

  err_handle:
    message->destroy(message);
    return NULL;
}
