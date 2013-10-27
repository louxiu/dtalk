/**
 * @file   user.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Thu Oct 24 21:01:03 2013
 * 
 * @brief  the user class
 * 
 * 
 */

#ifndef USER_H_
#define USER_H_

#include "message.h"

typedef struct user_t{
    /* 0: not authorized, 1: authorized */
    int online;
    int fd;
    char username[MAX_USERNAME_SIZE];
    char password[MAX_PASSWORD_SIZE];
}user_t;

#endif /* USER_H_ */
