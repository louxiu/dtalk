/**
 * @file   library.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Tue Oct 22 15:55:55 2013
 * 
 * @brief  
 * 
 * 
 */

#include "library.h"
#include "utils.h"
#include <malloc.h>

typedef struct private_library_t private_library_t;

/**
 * private data of library
 */
struct private_library_t {
	/**
	 * public functions
	 */
	library_t public;

#ifdef LEAK_DETECTIVE
    /* TODO: add leak detect module */
#endif /* LEAK_DETECTIVE */
};

/*
 * library instance
 */
library_t *lib;

/*
 * implementation of library_t.destroy
 */
void library_deinit()
{
    /* private_library_t *this = (private_library_t*)lib; */
    /* this->public.processor->destroy(this->public.processor); */
    
	/* threads_deinit(); */

	/* free(this); */
	/* lib = NULL; */
}

/*
 * see header file
 */
int library_init()
{
    /* TODO:
     * the fail case is handled by signal hander right now.
     * use more elegent way.*/
    
    private_library_t *this = malloc_thing(private_library_t);
	lib = &this->public;

    /* threads_init(); */
    /* this->public.processor = processor_create();     */

    
    return 0;
}
