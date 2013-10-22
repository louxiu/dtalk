/**
 * @file   dtalk-s.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Mon Oct 21 20:07:34 2013
 * 
 * @brief  the main file of dtalk server
 * 
 * 
 */
#define _POSIX_SOURCE 
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "debug.h"


/* TODO: generate from config.in */
#define DTALK_S_DIR ""
/* TODO: use /var/dtalk instead */
#define DTALK_S_PIDDIR "/var/run/"
static char *cmd_default = DTALK_S_DIR "dtalk";
static char *pid_file_default = DTALK_S_PIDDIR "dtalk.pid";

/* TODO: additional info */
char *daemon_name = "dtalk-s";
char *cmd = NULL;
char *dtalk_s_pid_file = DTALK_S_PIDDIR "dtalk.pid";

static char *config_file = NULL;

static int check_pid(const char *pid_file)
{
	struct stat stb;
	FILE *pidfile;
	if (stat(pid_file, &stb) == 0){
		pidfile = fopen(pid_file, "r");
		if (pidfile){
			char buf[64];
			pid_t pid = 0;
			memset(buf, 0, sizeof(buf));
			if (fread(buf, 1, sizeof(buf), pidfile)){
				buf[sizeof(buf) - 1] = '\0';
				pid = atoi(buf);
			}
			fclose(pidfile);
			if (pid && kill(pid, 0) == 0){
                /* process is running */
				return 1;
			}
		}
		fprintf(stderr, "removing pidfile '%s', process not running", pid_file);
		unlink(pid_file);
	}
	return 0;
}

/*
 * TODO: generate from config.h.in
 * */
#define VERSION "0.0.1"

debug_type debugType = STDERR;

typedef enum run_mode{
    DAEMON,
    DEBUG
}run_mode;

static run_mode runMode = DEBUG;

#define no_argument		    0
#define required_argument	1
#define optional_argument	2


/**
 * print command line usage and exit
 */
static void usage(const char *msg)
{
	if (msg != NULL && *msg != '\0'){
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

static void run_as_daemon(run_mode type){
    /* fork if we're not debugging stuff */
	if (type == DAEMON){
		debugType = SYSLOG;

		switch (fork()){
			case 0:
			{
				int fnull = open("/dev/null", O_RDWR);
				if (fnull >= 0){
					dup2(fnull, STDIN_FILENO);
					dup2(fnull, STDOUT_FILENO);
					dup2(fnull, STDERR_FILENO);
					close(fnull);
				}

				setsid();
				log_ret("dtalk start");
			}
			break;
			case -1:
				log_ret("can't fork: %s", strerror(errno));
				break;
			default:
				exit(EXIT_SUCCESS);
		} /* end switch */
        
        /* save pid file in /var/run/dtalk[.daemon_name].pid */
        FILE *fd = fopen(dtalk_s_pid_file, "w");
        if (fd){
            fprintf(fd, "%u\n", getpid());
            fclose(fd);
        } else {
            log_quit("can not create pid file");
        }
	} /* end if */
    else{
		debugType = DEBUG;
    }
}

int main(int argc, char *argv[])
{
    parse_option(argc, argv);

    if (check_pid(dtalk_s_pid_file)){
        fprintf (stderr,
                 "%s is already running (%s exists) -- skipping daemon start",
                 daemon_name, dtalk_s_pid_file);
    }

    /* from here, use debug interface */
    run_as_daemon(runMode);
    
    log_ret ("dtalk-s run success\n");
    return 0;
}
