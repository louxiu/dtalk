/**
 * @file   utils.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Tue Oct 22 16:07:16 2013
 * 
 * @brief  some useful function and macro
 * 
 * 
 */
#ifndef UTILS_H
#define UTILS_H

/*
 * Macro to allocate a sized type.
 */
#define malloc_thing(thing) ((thing*)malloc(sizeof(thing)))

int make_socket_non_blocking(int fd);
int create_and_bind (int port);

#endif /* UTILS_H */
