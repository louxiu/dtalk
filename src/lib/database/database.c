/**
 * @file   database.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Fri Oct 25 16:31:58 2013
 * 
 * @brief  implement the database module using sqlite3
 * 
 * 
 */
#include "database.h"
#include <sqlite3.h>
#include "../debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

typedef struct private_database_t private_database_t;

struct private_database_t {
	database_t public;
    sqlite3 *db;
};

/* NOTE: sql buffer length must be large enough. */
#define MAX_SQL_SIZE 1024

#define SQL_CREATE_USER "insert into users(username, password)"  \
                        " values(\'%s\',\'%s\')"
#define SQL_CHECK_USER  "select * from users where username = \'%s\' and"  \
                        " password = \'%s\'"
#define SQL_SAVE_LOG    "insert into log(time, username, type, message)"\
                        " values(\'%s\', \'%s\', %d, \'%s\')"
#define SQL_CREATE_ROOM "insert into rooms(roomname, username, details)"  \
                        " values(\'%s\', \'%s\', \'%s\')"
#define SQL_DELETE_ROOM "delete from rooms where roomname = \'%s\'"
#define SQL_CHECK_USER_NAME "select * from users where username=\'%s\'"
#define SQL_CHECK_ROOM  "select * from rooms where roomname=\'%s\'"

#define SQL_INIT_SIZE 3 
char *sql_init[SQL_INIT_SIZE] = {
    "create table if not exists "\
    "users(username varchar(128) primary key, password vachar(128))",
    
    "create table if not exists "\
    "log(id integer primary key autoincrement, time varchar(128), "\
    "username varchar(128), type integer, message varchar(1024), " \
    "FOREIGN KEY(username) REFERENCES users(username))",
    
    "create table if not exists "\
    "rooms(roomname varchar(128), username varchar(128), "\
    "details varchar(1024), FOREIGN KEY(username) REFERENCES users(username))"
};

static int open_db(private_database_t *this, char *db_name)
{
	if (sqlite3_open(db_name, &this->db) != SQLITE_OK){
		log_ret("can't open database:%s",sqlite3_errmsg(this->db));
		sqlite3_close(this->db);
		return -1;
	} else {
        int i = 0;
        for(i = 0; i < SQL_INIT_SIZE; ++i){
            if(sqlite3_exec(this->db, sql_init[i], NULL, NULL, NULL) !=
               SQLITE_OK){
                log_ret("%s\n",sqlite3_errmsg(this->db));
                sqlite3_close(this->db);
                return -1;
            }
        }
		log_ret("database open success");
    }
	return 0;
}

static int close_db(private_database_t *this)
{
    /* TODO_: check return value */
	sqlite3_close(this->db);
	return 0;
}

static int create_user(private_database_t *this, user_t *user)
{
	char sql_buffer[MAX_SQL_SIZE] = {'\0'};
	sqlite3_stmt *stmt = NULL;

    if (strcmp(user->username, "admin")==0){
       log_ret("forbiden username\n");
        return -1;
    }
    if (strcmp(user->username, "all")==0){
       log_ret("forbiden username\n");
        return -1;
    }
    if (strcmp(user->username, "login")==0){
       log_ret("forbiden username\n");
        return -1;
    }
    /* check wether the user exist or not */
    sprintf(sql_buffer, SQL_CHECK_USER_NAME, user->username);
	int res = sqlite3_prepare(this->db, sql_buffer, strlen(sql_buffer), &stmt, 0);
	if (res != SQLITE_OK){
		printf("prepare error: %s\n", sqlite3_errmsg(this->db));
		return -1;
	}
	
	res = sqlite3_step(stmt);
	if (res == SQLITE_ROW){
		sqlite3_finalize(stmt);
        log_ret("duplicate user");
	    return 0;
    }
    
	memset(sql_buffer, 0, sizeof(sql_buffer));
	sprintf(sql_buffer, SQL_CREATE_USER, user->username, user->password);

	if (sqlite3_exec(this->db, sql_buffer, NULL, NULL, NULL) != SQLITE_OK){
		log_ret("can't insert user:%s", sqlite3_errmsg(this->db));
		sqlite3_close(this->db);
		return -1;
	} else {
		log_ret("insert success");
    }
	return 0;
}

/* -1: error 0: success 1:username error 2:password error */
static int check_user(private_database_t *this, user_t *user)
{
	char sql_buffer[MAX_SQL_SIZE];
    /* check wether the user exists */
    sprintf(sql_buffer, SQL_CHECK_USER_NAME, user->username);
    sqlite3_stmt *stmt = NULL;
	int res = sqlite3_prepare(this->db, sql_buffer, strlen(sql_buffer), &stmt, 0);
	if (res != SQLITE_OK){
		printf("prepare error: %s\n", sqlite3_errmsg(this->db));
		return -1;
	}
	
	res = sqlite3_step(stmt);
	if (res != SQLITE_ROW){
		sqlite3_finalize(stmt);
        log_ret("user does not exist");
	    return 1;
    }	

	memset(sql_buffer, 0, sizeof(sql_buffer));
	sprintf(sql_buffer, SQL_CHECK_USER, user->username, user->password);

    res = sqlite3_prepare(this->db, sql_buffer, strlen(sql_buffer), &stmt, 0);
	if (res != SQLITE_OK){
		log_ret("prepare error: %s", sqlite3_errmsg(this->db));
		return -1;
	}
	
	res = sqlite3_step(stmt);

	if (res == SQLITE_ROW){
        log_ret("login success: %s, %s", user->username, user->password);        
		sqlite3_finalize(stmt);
		return 0;
	}

	sqlite3_finalize(stmt);
    
	return 2;
}

static int delete_user(private_database_t *this, user_t *user)
{
    /* NOTE: do not support delete user right now */
    return -1;
}

static int create_room(private_database_t *this, room_t *room)
{
    /* TODO_: return error type */
    
    /* The max length has been fixed, no need to check anymore */
    /* if (strlen(room->roomname) > 128 || strlen(room->desc) > 1024) */
    /* { */
    /*     log_ret("roomname too long"); */
    /*     return -1; */
    /* } */

    if (strcmp(room->roomname,"admin")==0){
        log_ret("forbiden roomname");
        return -1;
    }

	char sql_buffer[MAX_SQL_SIZE];
 
	sqlite3_stmt *stmt = NULL;

    sprintf(sql_buffer,
            SQL_CHECK_ROOM, room->roomname);
    int res = sqlite3_prepare(this->db,sql_buffer,strlen(sql_buffer),&stmt,0);

    if (res != SQLITE_OK){
		log_ret("prepare error: %s\n",sqlite3_errmsg(this->db));
		return -1;
	}

	if (sqlite3_step(stmt) == SQLITE_ROW){
		sqlite3_finalize(stmt);
        log_ret("duplicate room");
	    return -1;
    }	
	memset(sql_buffer, 0, sizeof(sql_buffer));

    sprintf(sql_buffer, SQL_CREATE_ROOM,
            room->roomname, room->owner, room->desc);

	res = sqlite3_exec(this->db, sql_buffer, NULL, NULL,NULL);

    if (res != SQLITE_OK){
		log_ret("can't insert room:%s", sqlite3_errmsg(this->db));
		sqlite3_close(this->db);
		return -1;
	} else {
		log_ret("insert success");
    }
	return 0;
}
static user_t *list_users(private_database_t *this)
{
	char sql_buffer[MAX_SQL_SIZE] = {'\0'};

	sprintf(sql_buffer, "select * from users");

    int nrow = 0, ncolumn = 0;
	char **user_list = NULL;
    int res = sqlite3_get_table(this->db, sql_buffer, &user_list, &nrow,
                                &ncolumn, NULL);
	struct user_t *users = NULL;    
	if (res == SQLITE_OK)
	{
        users = (struct user_t*)malloc(nrow*sizeof(struct user_t));
		int index = ncolumn;
        int i = 0;
		for(i = 0; i < nrow; i++)
		{
            /* TODO_: use strncpy instead of strcpy */
			strncpy(users[i].username, user_list[index], strlen(user_list[index]));
            log_ret("list_users: %s", user_list[index]);
			index++;
			strncpy(users[i].password, user_list[index], strlen(user_list[index]));
            log_ret("list_users: %s", user_list[index]);            
			index++;
		}
	}
	sqlite3_free_table(user_list);
	return users;

}

static room_t *list_rooms(private_database_t *this)
{
	char sql_buffer[MAX_SQL_SIZE] = {'\0'};

	sprintf(sql_buffer, "select * from rooms");

    int nrow = 0, ncolumn = 0;
	char **room_list = NULL;
    int res = sqlite3_get_table(this->db, sql_buffer, &room_list, &nrow,
                                &ncolumn, NULL);
	struct room_t *rooms = NULL;    
	if (res == SQLITE_OK)
	{
        rooms = (struct room_t*)malloc(nrow*sizeof(struct room_t));
		int index = ncolumn;
        int i = 0;
		for(i = 0; i < nrow; i++)
		{
            
            /* TODO_: use strncpy instead of strcpy */
			strncpy(rooms[i].roomname, room_list[index], strlen(room_list[index]));
            log_ret("list_rooms: %s", room_list[index]);
			index++;
			strncpy(rooms[i].owner, room_list[index], strlen(room_list[index]));
            log_ret("list_rooms: %s", room_list[index]);            
			index++;
			strncpy(rooms[i].desc, room_list[index], strlen(room_list[index]));
            log_ret("list_rooms: %s", room_list[index]);            
			index++;
		}
	}
	sqlite3_free_table(room_list);
	return rooms;

}

static database_log_t *list_logs(private_database_t *this)
{
    log_ret("hello");
	char sql_buffer[MAX_SQL_SIZE] = {'\0'};

	sprintf(sql_buffer, "select * from log");
    int nrow = 0, ncolumn = 0;
	char **log_list = NULL;
    int res = sqlite3_get_table(this->db, sql_buffer, &log_list, &nrow,
                                &ncolumn, NULL);
	struct database_log_t *logs = NULL;    
	if (res == SQLITE_OK)
	{
        logs = (struct database_log_t*)malloc(nrow*sizeof(struct database_log_t));
		int index = ncolumn;
        int i = 0;
		for(i = 0; i < nrow; i++)
		{
            
            /* TODO_: use strncpy instead of strcpy */
			/* strncpy(logs[i].type, log_list[index], strlen(log_list[index])); */
            index++;
			strncpy(logs[i].time, log_list[index], strlen(log_list[index]));
            log_ret("list_logs: %s", log_list[index]);            
			index++;
			strncpy(logs[i].username, log_list[index], strlen(log_list[index]));
            log_ret("list_logs: %s", log_list[index]);            
			index++;
            logs[i].type = atoi(log_list[index]);
            log_ret("list_logs: %d", atoi(log_list[index]));
			index++;
			strncpy(logs[i].message, log_list[index], strlen(log_list[index]));
            log_ret("list_logs: %s", log_list[index]);            
			index++;
		}
	}
	sqlite3_free_table(log_list);
	return logs;

}

static int delete_room(private_database_t *this, room_t *room)
{
    char sql_buffer[MAX_SQL_SIZE];
    sprintf(sql_buffer, SQL_DELETE_ROOM, room->roomname);

    int res = sqlite3_exec(this->db, sql_buffer, NULL, NULL, NULL);
    if (res != SQLITE_OK){
        fprintf(stderr,"can't delete room:%s\n",sqlite3_errmsg(this->db));
        return -1;
    } else {
       log_ret("delete room %s success\n",room->roomname);
    }
    return 0;
}

static int save_log(private_database_t *this, database_log_t *log)
{
	char sql_buffer[MAX_SQL_SIZE] = {'\0'};
	sprintf(sql_buffer, SQL_SAVE_LOG,
            log->time, log->username, log->type, log->message);
    /* log_ret("%s", sql_buffer); */
	char *errmsg = NULL;    
	int res = sqlite3_exec(this->db, sql_buffer, NULL, NULL, &errmsg);

	if (res != SQLITE_OK)
	{
		log_ret("insert log error: %s", sqlite3_errmsg(this->db));
		return -1;
	}

	return 0;
}
    
static void destroy(private_database_t *this)
{
    free(this);
}              

database_t *database_create()
{    
    private_database_t *this = malloc(sizeof(private_database_t));
    this->db = NULL;
    this->public.open_db = (int (*)(database_t *this, char *db_name))open_db;
    this->public.close_db = (int (*)(database_t *this))close_db;

    this->public.create_user = (int (*)(database_t *this, user_t *user))
        create_user;
    
    this->public.check_user = (int (*)(database_t *this, user_t *user))
        check_user;
    
    this->public.delete_user = (int (*)(database_t *this, user_t *user))
        delete_user;

    this->public.create_room = (int (*)(database_t *this, room_t *room))
                                create_room;
    this->public.list_rooms = (room_t *(*)(database_t *this))list_rooms;
    this->public.list_users = (user_t *(*)(database_t *this))list_users;
    this->public.list_logs = (database_log_t *(*)(database_t *this))list_logs;        

    this->public.delete_room = (int (*)(database_t *this, room_t *room))
        delete_room;

    this->public.save_log = (int (*)(database_t *this, database_log_t *log))
        save_log;
    
    this->public.destroy = (void (*)(database_t *this))destroy;

    return &this->public;
}

#ifdef DATABASE_DEBUG
/* gcc -g -Wall -std=c99 database.c ../debug.c ../utils.c -o db.out
 * -lsqlite3 -L/usr/local/sqlite3/lib -I/usr/local/sqlite3/include
 * -I../ -DDATABASE_DEBUG */

#include <assert.h>
#include "../utils.h"

int main()
{
    debugType = STDERR;

    database_t *db = database_create(NULL);

    db->open_db(db, "chat.db");

    db->list_users(db);    
	user_t user;
    
	strcpy(user.username, "lou");
	strcpy(user.password, "123");
    db->create_user(db, &user);
    db->create_user(db, &user);    
    db->check_user(db, &user);    

    strcpy(user.username, "admin");
	strcpy(user.password ,"13");
    db->create_user(db, &user);
    db->list_users(db);
    db->check_user(db, &user);
    
    room_t room;
    strcpy(room.roomname, "room1");
    strcpy(room.owner, "admin");
    strcpy(room.desc, "first room");
    db->create_room(db, &room);
    strcpy(room.roomname, "room2");
    strcpy(room.owner, "admin");
    strcpy(room.desc, "second room");
    db->create_room(db, &room);    
    db->list_rooms(db);

    database_log_t log;
	get_current_time(log.time);
	log.type = LOGIN;
	strcpy(log.username, "lou");
	strcpy(log.message, "denglu");
    db->save_log(db, &log);
    db->list_logs(db);
    
    db->close_db(db);
    db->destroy(db);
	//get_room();
	return 0;
}
#endif /* DATABASE_DEBUG */
