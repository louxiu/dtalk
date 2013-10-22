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
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
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
static FILE *dtalk_s_fd = NULL;

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


/*
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

/*
 * drop daemon capabilities
 */
static int drop_capabilities()
{
    /* TODO: */
    return 0;
}

/* 
 * stroke the program to background
 */
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
                /* TODO: drop capabilities */
                drop_capabilities();
			}
			break;
			case -1:
				log_ret("can't fork: %s", strerror(errno));
				break;
			default:
                /* dtalk-s wrapper: bye-bye */
				exit(EXIT_SUCCESS);
		} /* end switch */
        
        /* save pid file in /var/run/dtalk[.daemon_name].pid */
        dtalk_s_fd = fopen(dtalk_s_pid_file, "w");
        if (dtalk_s_fd){
            fprintf(dtalk_s_fd, "%u\n", getpid());
            fflush(dtalk_s_fd);
            /* fclose(fd); */
        } else {
            log_quit("can not create pid file");
        }
	} /* end if */
    else{
		debugType = DEBUG;
    }
}

/*
 * Handle fatal signals raised by threads
 */
static void fatal_signal_handler(int signal)
{
    /* TODO: backtrace the caller */
	abort();
}

/*
 * install signal handler for the daemon
 */
static void install_signals_handler()
{
    struct sigaction action;
    /* handle these signals only in epoll() */
	memset(&action, 0, sizeof(action));
	sigemptyset(&action.sa_mask);
    
    /* add handler for SEGV and ILL,
	 * INT, TERM and HUP are handled by sigwait() in run() */
	action.sa_handler = fatal_signal_handler;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGINT);
	sigaddset(&action.sa_mask, SIGTERM);
	sigaddset(&action.sa_mask, SIGHUP);
	sigaction(SIGSEGV, &action, NULL);
	sigaction(SIGILL, &action, NULL);
	sigaction(SIGBUS, &action, NULL);
	action.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &action, NULL);

	pthread_sigmask(SIG_SETMASK, &action.sa_mask, NULL);
}

/*
 * Ignore result of functions tagged with warn_unused_result attributes
 */
#define ignore_result(call) { if(call){}; }

static void unlink_pidfile()
{
	/* because unlinking the PID file may fail, we truncate it to ensure the
	 * daemon can be properly restarted.  one probable cause for this is the
	 * combination of not running as root and the effective user lacking
	 * permissions on the parent dir(s) of the PID file */
	if (dtalk_s_fd)
	{
        /* TODO: warning: implicit declaration */
		ignore_result(ftruncate(fileno(dtalk_s_fd), 0));
		fclose(dtalk_s_fd);
	}
	unlink(dtalk_s_pid_file);
}

/*
 * run the daemon and handle unix signals
 */
static void run()
{
    sigset_t set;

	/* handle SIGINT, SIGHUP ans SIGTERM in this handler */
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGTERM);
	sigprocmask(SIG_BLOCK, &set, NULL);

	while (1){
		int sig;
		int error;

		error = sigwait(&set, &sig);
		if (error){
			log_ret("error %d while waiting for a signal", error);
			return;
		}
		switch (sig){
			case SIGHUP:
            {
				log_ret("signal of type SIGHUP received. Ignored");
				break;
			}
			case SIGINT:
            {
				log_ret("signal of type SIGINT received. Shutting down");
				return;
			}
			case SIGTERM:{
				log_ret("signal of type SIGTERM received. Shutting down");
				return;
			}
			default:
			{
				log_ret("unknown signal %d received. Ignored", sig);
				break;
			}
		} /* end switch */
	} /* end while */
}

int main(int argc, char *argv[])
{
    parse_option(argc, argv);

    if (check_pid(dtalk_s_pid_file)){
        fprintf (stderr,
                 "%s is already running (%s exists) -- skipping daemon start",
                 daemon_name, dtalk_s_pid_file);
        return 1;
    }

    run_as_daemon(runMode);

    /* install signal handler */
    install_signals_handler();

 	/* start daemon (i.e. the threads in the thread-pool) */
    
    /* main thread goes to run loop */
	run();

    /* normal termination, cleanup and exit */
	unlink_pidfile();

    return 0;
}
