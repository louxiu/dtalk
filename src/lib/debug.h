
/**
 * @file   debug.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Mon Oct 21 21:04:39 2013
 * 
 * @brief  The debug and log module
 * 
 */

#ifndef DEBUG_H_
#define DEBUG_H_

void log_open (const char *ident, int option, int facility);
void log_msg  (const char *fmt, ...);	
void log_quit (const char *fmt, ...);
void log_ret  (const char *fmt, ...);
void log_sys  (const char *fmt, ...);
void log_close();

typedef enum debug_type{
    STDERR,
    SYSLOG
}debug_type;

extern debug_type debugType; 

#endif /* DEBUG_H_ */

