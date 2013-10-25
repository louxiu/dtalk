/**
 * @file   database.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Fri Oct 25 16:02:06 2013
 * 
 * @brief  the database module
 * 
 * 
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include "message.h"
#include "user.h"
#include "room.h"

#define MAX_TIME_SIZE 128
#define MAX_MESSAGE_SIZE 1024

typedef struct database_log_t{
	message_type  type;
    char          time[MAX_TIME_SIZE];    
	char          username[MAX_USERNAME_SIZE];
	char          message[MAX_MESSAGE_SIZE];
}database_log_t;

typedef struct database_t database_t;

struct database_t {

    int (*open_db)(database_t *database, char *db_name);
    int (*close_db)(database_t *database);

    int (*create_user)(database_t *database, user_t *user);
    int (*check_user)(database_t *database, user_t *user);    
    int (*delete_user)(database_t *database, user_t *user);
    user_t *(*list_users)(database_t *database);
    
    int (*create_room)(database_t *database, room_t *room);
    room_t *(*list_rooms)(database_t *database);
    int (*delete_room)(database_t *database, room_t *room);

    int (*save_log)(database_t *database, database_log_t *log);
    database_log_t *(*list_logs)(database_t *database);
    
    void (*destroy)(database_t *database);

};

database_t *database_create();

#endif /* DATABASE_H_ */
