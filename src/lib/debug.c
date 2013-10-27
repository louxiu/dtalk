/**
 * @file   debug.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Mon Oct 21 21:09:25 2013
 * 
 * @brief  used for log and debug
 * 
 * 
 */

/* TODO: log to specify file */

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"

#define MAXLINE 4096									

debug_type debugType = STDERR;

static void	log_doit(int errnoflag, int priority, const char *fmt, va_list ap);

/*
 * initialize syslog(), if running as daemon.
 */
void log_open(const char *ident, int option, int facility)
{
	if (debugType == SYSLOG){
		openlog(ident, option, facility);
    }
}

/**
 * deinitialize, if running as daemon.
 */
void log_close()
{
	if (debugType == SYSLOG){
		closelog();
	}
}

/*
 * nonfatal error related to a system call.
 * print a message with the system's errno value and return.
 */
void log_ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
}

/*
 * fatal error related to a system call.
 * print a message and terminate.
 */
void log_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/*
 * nonfatal error unrelated to a system call.
 * print a message and return.
 */
void log_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
}

/*
 * fatal error unrelated to a system call.
 * print a message and terminate.
 */
void log_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/*
 * print a message and return to caller.
 * caller specifies "errnoflag" and "priority".
 */
static void log_doit(int errnoflag, int priority, const char *fmt, va_list ap)
{
	int		errno_save = errno;
	char	buf[MAXLINE];

	vsnprintf(buf, MAXLINE, fmt, ap);
	if (errnoflag){
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf), "errno: %s",
                 strerror(errno_save));
    }
	strcat(buf, "\n");
	if (debugType == STDERR){
		fflush(stdout);
		fputs(buf, stderr);
		fflush(stderr);
	} else if(debugType == SYSLOG){
		syslog(priority, buf, strlen(buf));
	}
}

#ifdef DEBUG_TEST
int main(int argc, char *argv[])
{   
    log_open("dstalk", LOG_PID|LOG_CONS, LOG_USER);
    log_ret("Hello world %d\n", 1);

    debugType = SYSLOG;
    log_open("dstalk", LOG_PID|LOG_CONS, LOG_USER);
    log_ret("Hello world %d\n", 1);
    
    return 0;
}
#endif  /* DEBUG_TEST */
