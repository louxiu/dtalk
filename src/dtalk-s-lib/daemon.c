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
#include "utils.h"
#include "debug.h"

/* TODO: undeclared (first use in this function) */
#define MAXEVENTS  64
#define NI_MAXHOST 1024
#define NI_MAXSERV 32
/* TODO: make port configurable */
#define SERVER_PORT 5000

void *listener_func()
{
    struct epoll_event event;
    struct epoll_event *events;

    int server_fd = -1;
    if ((server_fd = create_and_bind(SERVER_PORT)) == -1){
        log_quit("listener_func: create_and_bind failed");
    }

    if (make_socket_non_blocking(server_fd) == -1){
        abort ();
    }

    if (listen (server_fd, SOMAXCONN) == -1){
        abort ();
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1){
        abort ();
    }

    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1){
        abort ();
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
                    int new_fd = accept (server_fd, &in_addr, &in_len);
                    if (new_fd == -1){
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                            /* processed all incoming connections. */
                        }else{
                            log_ret ("accept error");
                        }
                        break;                        
                    }

                    int ret = getnameinfo (&in_addr, in_len,
                                     hbuf, sizeof hbuf,
                                     sbuf, sizeof sbuf,
                                     NI_NUMERICHOST | NI_NUMERICSERV);
                    if (ret == 0){
                        log_ret("accepted connection on descriptor %d "
                               "(host=%s, port=%s)\n", new_fd, hbuf, sbuf);
                    }

                    /* Make the incoming socket non-blocking and add it to the
                       list of fds to monitor. */
                    if (make_socket_non_blocking (new_fd) == -1){
                        abort();
                    }

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

                while (1){
                    ssize_t count;
                    char buf[512];

                    count = read(events[i].data.fd, buf, sizeof buf);
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

                    /* write the buffer to standard output */
                    if (write(1, buf, count) == -1){
                        log_ret ("write error");
                    }
                }

                if (done){
                    log_ret("closed connection on descriptor %d\n",
                            events[i].data.fd);

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

static pthread_t listener;
void daemon_init()
{
    if (pthread_create(&listener, NULL, listener_func, NULL) != 0){
        exit(EXIT_FAILURE);
    }
}
