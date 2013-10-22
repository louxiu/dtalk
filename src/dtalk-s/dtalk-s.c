/**
 * @file   dtalk-s.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Mon Oct 21 20:07:34 2013
 * 
 * @brief  the main file of dtalk server
 * 
 * 
 */
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "debug.h"

/*
 * TODO: generate from config.h.in
 * */
#define VERSION "0.0.1"

debug_type debugType = STDERR;

typedef enum run_mode{
    DAEMON,
    NORMAL
}run_mode;

static run_mode runMode = NORMAL;

#define no_argument		    0
#define required_argument	1
#define optional_argument	2

/**
 * print command line usage and exit
 */
static void usage(const char *msg)
{
	if (msg != NULL && *msg != '\0')
	{
		fprintf(stderr, "%s\n", msg);
	}
	fprintf(stderr, "Usage: dtalks\n"
            "         [--help]\n"
            "         [--version]\n"
            "         [--daemon]\n"
            );
}

void parse_option(int argc, char *argv[])
{
    struct option long_opts[] = {
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'v' },
        { "daemon", no_argument, NULL, 'd' },
        { 0,0,0,0 }
    };
    int status = 0, opt = EOF;
    while ((opt = getopt_long(argc, argv, ":hvd", long_opts, NULL)) != EOF){
		switch (opt){
			case 'h':
				usage(NULL);
				status = 1;
                break;
			case 'v':
				printf("dtalks %s\n", VERSION);
				status = 1;
                break;
			case 'd':
				runMode = DAEMON;
                break;
			case '?':
				fprintf(stderr, "unknow option: %c\n", optopt);
                usage(NULL);
				status = 1;
                break;
		}
        if(status == 1){
            exit(EXIT_FAILURE);
        }
	} /* end while */
}
int main(int argc, char *argv[])
{
    parse_option(argc, argv);
    
    printf ("dtalk-s run success\n");
    return 0;
}
