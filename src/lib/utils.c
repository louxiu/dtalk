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
#include <time.h>
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

int get_current_time(char *date)
{
	time_t epoch_time = time(NULL);

    /* convert to localtime */
    struct tm *local_time = localtime(&epoch_time);

    #define MAX_DATE_SIZE 20
    
    char tmp_buffer[MAX_DATE_SIZE] = {'\0'};
    
	sprintf(date, "%4d-", local_time->tm_year+1900);
	if ((local_time->tm_mon+1) > 9){
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "%2d-", local_time->tm_mon+1);
		strcat(date, tmp_buffer);
	} else {
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "0%d-", local_time->tm_mon+1);
		strcat(date, tmp_buffer);
	}
	if (local_time->tm_mday > 9){
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "%2d ", local_time->tm_mday);
		strcat(date, tmp_buffer);
	} else {
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "0%d ", local_time->tm_mday);
		strcat(date, tmp_buffer);
	}
	if (local_time->tm_hour > 9){
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "%2d:", local_time->tm_hour);
		strcat(date, tmp_buffer);
	} else {
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "0%d:", local_time->tm_hour);
		strcat(date, tmp_buffer);
	}
	if (local_time->tm_min > 9){
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "%2d:", local_time->tm_min);
		strcat(date, tmp_buffer);
	} else {
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "0%d:", local_time->tm_min);
		strcat(date, tmp_buffer);
	}
	if (local_time->tm_sec > 9) {
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "%2d", local_time->tm_sec);
		strcat(date, tmp_buffer);
	} else {
		memset(tmp_buffer, 0, MAX_DATE_SIZE);
		sprintf(tmp_buffer, "0%d", local_time->tm_sec);
		strcat(date, tmp_buffer);
	}
	return 0;
}

unsigned int BKDRHash(char *str, unsigned int mod)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;
 
	while (*str){
		hash = hash * seed + (*str++);
	}
 
	return (hash & 0x7FFFFFFF) % mod;
}
