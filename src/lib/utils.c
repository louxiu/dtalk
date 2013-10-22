/**
 * @file   utils.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Wed Oct 23 00:34:41 2013
 * 
 * @brief  
 * 
 * 
 */
#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "debug.h"

int make_socket_non_blocking(int fd)
{
    int flags;

    if ((flags = fcntl (fd, F_GETFL, 0)) == -1){
        log_ret("make_socket_non_blocking F_GETFL error");
        return -1;
    }

    flags |= O_NONBLOCK;
    
    if (fcntl(fd, F_SETFL, flags) == -1){
        log_ret("make_socket_non_blocking F_SETFL error");        
        return -1;
    }

    return 0;
}


int create_and_bind (int port)
{
    struct addrinfo hints;

    memset (&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;         /* IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;         /* all interfaces */

    struct addrinfo *result = NULL;
    int addr_info = -1;

    #define PORT_LEN 10
    char port_str[PORT_LEN] = {'\0'};
    snprintf(port_str, PORT_LEN - 1, "%d", port);

    if ((addr_info = getaddrinfo(NULL, port_str, &hints, &result)) != 0){
        log_ret("create_and_bind: getaddrinfo: %s\n", gai_strerror(addr_info));
        return -1;
    }

    int server_fd = -1;
    struct addrinfo *iterator = NULL;
    for (iterator = result; iterator != NULL; iterator = iterator->ai_next){
        server_fd = socket(iterator->ai_family, iterator->ai_socktype,
                     iterator->ai_protocol);

        if (server_fd == -1){continue;};

        if ((bind(server_fd, iterator->ai_addr, iterator->ai_addrlen)) == 0){
            /* bind successfully! */
            break;
        }

        close (server_fd);
    }

    freeaddrinfo(result);
    
    if (iterator == NULL){
        log_ret("create_and_bind: bind failed");
        return -1;
    }

    return server_fd;
}
