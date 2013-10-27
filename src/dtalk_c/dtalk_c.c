/**
 * @file   dtalk_c.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Mon Oct 21 20:09:20 2013
 * 
 * @brief  
 * 
 * 
 */

#include "dtalk_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "packet.h"
#include "message.h"
#include "deal_reply.h"
#include "deal_request.h"
#include "dtalk_c_room.h"
#include "color_print.h"
#include "utils.h"
#include "load_conf.h"

#define CONF_FILE CONF_DIR "/dtalk_c.conf"
#define MAX_CONTENT_SIZE     520
#define MAX_PACKAGE_SIZE     1024
#define SLEEP_TIME      1

msg_list_t *msgList = NULL;
room_handle_t room_handle;

//define in deal_request.h
extern char global_username[MAX_USERNAME_SIZE];
extern char global_password[MAX_PASSWORD_SIZE];

extern pthread_mutex_t msg_mutex;
sem_t msg_num;

void *msg_send_func(int *fd)
{
	int send_fd;

	send_fd = *fd;
    message_t *message = NULL;
	while (1){
		usleep(SLEEP_TIME);

		sem_wait(&msg_num);
		/* get message from msglist */
        if ((message = remove_msglist(&msgList)) == NULL){
            err_print("NULL\n");
        }
        packet_t *packet = message->get_packet(message);

		/* judge the type of the message */
		/* if the roomname is not join*/

        packet->packet_send(packet);

        packet->destroy(packet);
        message->destroy(message);
	}
}

void *msg_recv_func(int *fd)
{
	int recv_fd;
	int recv_num;
	char recv_buf[MAX_PACKAGE_SIZE];

	recv_fd = *fd;
	while (1){
		/* usleep(SLEEP_TIME * 1000000); */
		memset(recv_buf, 0, MAX_PACKAGE_SIZE);
		recv_num = recv(recv_fd, recv_buf, sizeof(recv_buf), 0);
        if (recv_num > 0){
            /* some data comes */
            process_recv_buf(&room_handle, recv_buf, recv_num);
        }
        else if (recv_num < 0
				 && errno != EINTR
				 && errno != EWOULDBLOCK
				 && errno != EAGAIN){
            /* connect error */
            /* inf_print("connect error\n"); */
            return;
        }
        else if (recv_num < 0 &&
				 (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)){
            /* inf_print("correct but no data "); */
            /* correct but no data */
        }else{
			/* inf_print("correct but no data "); */
		}
	}
}

void socket_init(int *fd, pthread_t *msg_send_thread,
				 pthread_t *msg_recv_thread)
{
	int *client_fd = fd;
	struct sockaddr_in addr_client;
	memset(&addr_client, 0, sizeof(addr_client));
	addr_client.sin_family = AF_INET;
	addr_client.sin_port = htons(conf_port);
	addr_client.sin_addr.s_addr = inet_addr(conf_ip);
	*client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(*client_fd, (struct sockaddr *)&addr_client,
			   sizeof(struct sockaddr)) < 0){
		err_print("connect failed..\n");
	}
    
    /* TODO: call make_socket_non_blocking */
    /* make_socket_non_blocking(*client_fd); */
	int ret;
	int optval;
	ret = setsockopt(*client_fd, IPPROTO_TCP, TCP_NODELAY,
			   (char *)&optval, sizeof(int));
    /*login*/
    //send_login(*client_fd, global_username, global_password);
    
    //create send & recv thread

	ret = pthread_create(msg_send_thread, NULL,
						 (void *)&msg_send_func, (void *)client_fd);
	if (ret != 0){
		err_print("create send thread failed!");
		exit(EXIT_FAILURE);
	} else {
		/* inf_print("create send thread"); */
	}

	ret = pthread_create(msg_recv_thread, NULL,
						 (void *)&msg_recv_func, (void *)client_fd);
	if (ret != 0){
		err_print("create recv thread failed!");
		exit(EXIT_FAILURE);
	} else {
		/* inf_print("create recv thread"); */
	}
}

void my_scanf(char *input)
{
    char character;
    int i = 0;
    while (i < MAX_CONTENT_SIZE && (character = getchar()) != '\n'){
        *(input+i) = character;
        i++;
    }
}

int get_input(char *input, int size, int client_fd)
{
    memset(input, 0, MAX_CONTENT_SIZE);
    my_scanf(input);
    /* inf_print("\ninput:%s\n", input); */
    if (input[0] != '/' && room_handle.cur_room == NULL){
        return -1;
	}
	message_t *message = construct_message(&room_handle, input,
										   strlen(input), client_fd);
	if (message == NULL){
		/* construct failed */
		return -1;
	}
	/* add to msg list */
	add_msglist(&msgList, message);
    return 0;
}

void client_start(int client_fd)
{
	inf_print("----------------------------");
 	//client start
	char input[MAX_CONTENT_SIZE];
	while(1){
		/* get input content */
		usleep(SLEEP_TIME);
		if (get_input(input, MAX_CONTENT_SIZE, client_fd) == 0)
            sem_post(&msg_num);//put input into msgList
	}
}

void init_username_password(int argc, char **argv)
{
    memset(global_username, 0, MAX_USERNAME_SIZE);
    memset(global_password, 0, MAX_PASSWORD_SIZE);
}

int main(int argc, char **argv)
{
	load_conf(CONF_FILE);
	/* printf ("%s\n", conf_ip); */
	/* printf ("%hd\n",conf_port); */
	init_username_password(argc, argv);
	
    room_handle.use_room_list = NULL;
    room_handle.all_room_list = NULL;
	room_handle.status = OFFLINE;
	
	pthread_mutex_init(&msg_mutex, NULL);

	sem_init(&msg_num, 0, 0);

	/* create client_fd and connect to server */
	int client_fd = 0;
	pthread_t msg_send_thread, msg_recv_thread;
	socket_init(&client_fd, &msg_send_thread, &msg_recv_thread);
	
    /* client start */
    client_start(client_fd);

	pthread_join(msg_send_thread, NULL);
	pthread_join(msg_recv_thread, NULL);

	return 0;
}
