/**
 * @file   daemon.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Tue Oct 22 14:59:06 2013
 * 
 * @brief  
 * 
 * 
 */

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/tcp.h>
#include "utils.h"
#include "debug.h"
#include "packet.h"
#include "message.h"
#include "daemon.h"
#include "database.h"
#include "user.h"
#include "room.h"
#include "load_conf.h"

#define CONF_FILE CONF_DIR "/dtalk_s.conf"
#define DB_FILE   DB_DIR "/chat.db"

/* TODO: check pointer before use it */
/* TODO: undeclared (first use in this function) */
#define MAXEVENTS  64
#define NI_MAXHOST 1024
#define NI_MAXSERV 32

/* online users vector */
online_users_t *online_users = NULL;

/* online rooms table */
online_rooms_t *online_rooms = NULL;

/* database object */
database_t *db = NULL;

static void handle_join(message_t *message)
{
    user_t user;
    
    message->get_username_password(message, user.username, user.password);
    /* log_ret("username = %s, password = %s", user.username, user.password); */
    /* TODO: check username format */
    /* TODO: check password format */

    message_t *join_m = message_create();

    /* add to database */
    if (db->create_user(db, &user) == 0){
        join_m->set_type(join_m, JOIN_SUCCESS);
    } else {
        join_m->set_type(join_m, JOIN_FAILED);
    }
    
    /* this seems tedious now,
     * but it will be more scalable in the future */
    
    packet_t *join_p = join_m->get_packet(join_m);
    join_p->packet_send(join_p);
    
    join_p->destroy(join_p);
    join_m->destroy(join_m);    
}

static void handle_login(message_t *message)
{
    user_t user;

    message->get_username_password(message, user.username, user.password);
    /* log_ret("username = %s, password = %s", user.username, user.password); */
    /* TODO: check username format */
    /* TODO: check password format */

    message_t *login_m = message_create();
    int fd = message->get_fd(message);

    if (db->check_user(db, &user) == 0){
        /* add it to online_users */
        online_users->login(online_users, fd, user.password);
        login_m->set_type(login_m, LOGIN_SUCCESS);
    } else {
        login_m->set_type(login_m, LOGIN_FAILED);
    }
    login_m->set_fd(login_m, fd);
    /* log_ret("login fd: %d", fd); */
    
    packet_t *login_p = login_m->get_packet(login_m);
    login_p->packet_send(login_p);
    
    login_p->destroy(login_p);
    login_m->destroy(login_m);    
}

static void handle_logout_closefd(int fd)
{
    online_users->logout(online_users, fd);
}

static void handle_logout(message_t *message)
{
    int fd = message->get_fd(message);
    /* delete it from online_users */
    online_users->logout(online_users, fd);

    message_t *logout_m = message_create();
    logout_m->set_type(logout_m, LOGOUT_SUCCESS);
    logout_m->set_fd(logout_m, fd);
    log_ret("logout: fd = %d", fd);
    packet_t *logout_p = logout_m->get_packet(logout_m);
    logout_p->packet_send(logout_p);

    logout_p->destroy(logout_p);
    logout_m->destroy(logout_m);    
}

static int handle_join_room(message_t *message)
{
    log_ret("handle_join_room");
    int user_fd = message->get_fd(message);
    char *room = message->get_data(message);
    /* fetch info from message */
    int ret = online_users->join_room(online_users, user_fd, room);

    message_t *join_room_m = message_create();
    join_room_m->set_fd(join_room_m, user_fd);
    if (ret == 0){
        join_room_m->set_data(join_room_m, room);
        join_room_m->set_type(join_room_m, JOIN_ROOM_SUCCESS);
    }else{
        join_room_m->set_type(join_room_m, JOIN_ROOM_FAILED);        
    }

    packet_t *join_room_p = join_room_m->get_packet(join_room_m);
    join_room_p->packet_send(join_room_p);

    join_room_p->destroy(join_room_p);
    join_room_m->destroy(join_room_m);    
    
    return 0;
}

static void handle_leave_room(message_t *message)
{
    log_ret("handle_leave_room");
    int user_fd = message->get_fd(message);
    char *room = message->get_data(message);
    /* fetch info from message */
    int ret = online_users->leave_room(online_users, user_fd, room);
    log_ret("ret = %d", ret);
    message_t *leave_room_m = message_create();
    leave_room_m->set_fd(leave_room_m, user_fd);
    if (ret == 0){
        leave_room_m->set_data(leave_room_m, room);
        leave_room_m->set_type(leave_room_m, LEAVE_ROOM_SUCCESS);
    }else{
        leave_room_m->set_type(leave_room_m, LEAVE_ROOM_FAILED);        
    }

    packet_t *leave_room_p = leave_room_m->get_packet(leave_room_m);
    leave_room_p->packet_send(leave_room_p);
    log_ret("leave packet send");
    leave_room_p->destroy(leave_room_p);
    leave_room_m->destroy(leave_room_m);    
    
    return 0;
}

static void handle_message(message_t *message)
{
    /* TODO; forward the message */
}

static void handle_list_rooms(message_t *message)
{
    log_ret("handle list_rooms");
    int fd = message->get_fd(message);
    message_t *list_rooms_m = message_create();
    list_rooms_m->set_type(list_rooms_m, LIST_ROOMS_SUCCESS);
    list_rooms_m->set_fd(list_rooms_m, fd);

    /* packet send in list_rooms */
    online_rooms->list_rooms(online_rooms, list_rooms_m);

    list_rooms_m->destroy(list_rooms_m);
}

static void handle_list_users(message_t *message)
{
    
}

static void handle_unknow_type(message_t *message)
{

}

static void handle_echo(message_t *message)
{
    packet_t *packet_echo = message->get_packet(message);
    packet_echo->packet_send(packet_echo);
    packet_echo->destroy(packet_echo);
}

static void process_message(message_t *message)
{
    printf ("version: %c\n", message->get_version(message));

    char *type = message_type_array[message->get_type(message)];
    printf ("type   : %s\n", type);
    printf ("data   : %s\n", message->get_data(message));
    printf ("fd     : %d\n", message->get_fd(message));
    
    /* dirty work start */
    switch(message->get_type(message)) 
    {
        case JOIN:
        {
            handle_join(message);
            break;
        }
        case LOGIN:
        {
            /* handle_echo(message); */
            handle_login(message);
            break;
        }
        case LOGOUT:
        {
            handle_logout(message);
            break;
        }
        case JOIN_ROOM:
        {
            handle_join_room(message);            
            break;
        }
        case LEAVE_ROOM:
        {
            handle_leave_room(message);
            break;
        }
        case MESSAGE:
        {
            handle_message(message);
            break;
        }
        case LIST_ROOMS:
        {
            handle_list_rooms(message);
            break;
        }
        case LIST_USERS:
        {
            handle_list_users(message);
            break;
        }
        case ECHO:
        {
            handle_echo(message);
            break;
        }
        default:
            handle_unknow_type(message);
    }
}

static void process_packet(char *data, int count, int fd){
    /* write the buffer to standard output */
    fprintf (stdout, "%s\n", data);

    /* construct the packet */
    packet_t * packet = packet_create();
    packet->set_fd(packet, fd);
    packet->set_size(packet, count);
    packet->set_payload(packet, data);

    message_t *message = message_create_from_packet(packet);
    packet->destroy(packet);
    
    process_message(message);

    message->destroy(message);
}

static void *listener_func()
{
    struct epoll_event event;
    struct epoll_event *events;

    int server_fd = -1;
    if ((server_fd = create_and_bind(conf_port)) == -1){
        log_quit("listener_func: create_and_bind failed");
    }

    if (make_socket_non_blocking(server_fd) == -1){
        log_quit("listener_func: make_socket_non_blocking failed");        
    }

    if (listen(server_fd, SOMAXCONN) == -1){
        log_quit("listener_func: make_socket_non_blocking failed");
    }

    int epoll_fd = -1;
    if ((epoll_fd = epoll_create1(0))== -1){
        log_quit("listener_func: epoll_create failed");
    }

    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1){
        log_quit("listener_func: epoll_ctl failed");
    }

    /* buffer where events are returned */
    events = calloc(MAXEVENTS, sizeof event);

    /* the event loop */
    while (1){
        int available_num = epoll_wait (epoll_fd, events, MAXEVENTS, -1);
        int i = 0;
        for (i = 0; i < available_num; i++){
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))){
                /* an error has occured on this fd, or the socket is not
                   ready for reading (why were we notified then?) */
                log_ret("epoll error");
                close (events[i].data.fd);
                continue;
            }
            else if (server_fd == events[i].data.fd){
                /* a notification on the listening socket, which
                   means one or more incoming connections. */
                while (1){
                    struct sockaddr in_addr;
                    
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
                    socklen_t in_len = sizeof(in_addr);
                    int new_fd = accept(server_fd, &in_addr, &in_len);
                    if (new_fd == -1){
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                            /* processed all incoming connections. */
                        }else{
                            log_ret ("accept error");
                        }
                        break;                        
                    }

                    int ret = getnameinfo(&in_addr, in_len,
                                     hbuf, sizeof hbuf,
                                     sbuf, sizeof sbuf,
                                     NI_NUMERICHOST | NI_NUMERICSERV);
                    if (ret == 0){
                        log_ret("accepted connection on descriptor %d "
                               "(host=%s, port=%s)", new_fd, hbuf, sbuf);
                    }

                    /* make the incoming socket non-blocking and add it to the
                       list of fds to monitor. */
                    if (make_socket_non_blocking(new_fd) == -1){
                        log_quit("listener_func: "
                                 "make_socket_non_blocking failed");
                    }
                    int optval;
                    ret = setsockopt(new_fd, IPPROTO_TCP, TCP_NODELAY,
                               (char *)&optval, sizeof(int));
                    log_ret("set sockopt %d", ret);
                    event.data.fd = new_fd;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1){
                        log_quit("epoll ctl add");
                    }
                } /* end while */
                continue;
            } /* end if new connection */
            else{
                /* we have data on the fd waiting to be read.
                   we must read whatever data is available
                   completely, as we are running in edge-triggered mode
                   and won't get a notification again for the same
                   data. */
                int done = 0;
                int current_fd = events[i].data.fd;
                while (1){

                    char packet[1024] = {'\0'};
                    ssize_t count = read(current_fd, packet, sizeof(packet));
                    if (count == -1){
                        if (errno != EAGAIN){
                            log_ret("read error");
                            done = 1;
                        }else{
                            /* we have read all data. */
                            break;
                        }
                    }
                    else if (count == 0){
                        /* the remote has closed the connection. */
                        done = 1;
                        break;
                    }
                    /* process every packet */
                    process_packet(packet, count, current_fd);
                }

                if (done){
                    log_ret("closed connection on descriptor %d\n",
                            events[i].data.fd);
                    
                    handle_logout_closefd(events[i].data.fd);
                    /* closing the descriptor will make epoll remove it
                       from the set of descriptors which are monitored. */
                    close(events[i].data.fd);
                }
            }  /* end data */
        }  /* end for statement */
    }   /* end while */

    free (events);
    close (server_fd);
}


void daemon_init()
{
    load_conf(CONF_FILE);
    
    /* TODO: check return value */
    online_users = online_users_create();
    online_rooms = online_rooms_create();    
    db = database_create();
    db->open_db(db, DB_FILE);
    
    online_rooms->load_rooms(online_rooms, db);
    
    /* TODO: load online rooms from database */

    static pthread_t listener;
    /* TODO: use thread pool */
    if (pthread_create(&listener, NULL, listener_func, NULL) != 0){
        exit(EXIT_FAILURE);
    }

    pthread_join(listener, NULL);

    /* clean up */
    online_users->destroy(online_users);
    online_rooms->destroy(online_rooms);
    db->close_db(db);
    db->destroy(db);
}
