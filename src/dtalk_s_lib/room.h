/**
 * @file   room.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Thu Oct 24 21:03:53 2013
 * 
 * @brief  the room class
 * 
 * 
 */

#ifndef ROOM_H_
#define ROOM_H_

#include "message.h"
/* TODO: use kernel list.h */

typedef struct room_t{
    char roomname[MAX_ROOMNAME_SIZE];
    char owner[MAX_USERNAME_SIZE];
    char desc[MAX_DESC_SIZE];
}room_t;

#endif /* ROOM_H_ */
