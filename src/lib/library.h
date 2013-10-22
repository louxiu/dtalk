
/**
 * @file   library.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Tue Oct 22 15:49:55 2013
 * 
 * @brief  share library
 * 
 * 
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#include "processing/processor.h"

typedef struct library_t library_t;

struct library_t {
    /*
	 * process jobs using a thread pool
	 */
	/* processor_t *processor; */
};

/*
 * initialize library
 *
 */
int library_init();

/*
 * deinitialize library
 */
void library_deinit();

extern library_t *lib;

#endif /* LIBRARY_H */
