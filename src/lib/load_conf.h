/**
 * @file   load_conf.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Sat Oct 26 22:36:37 2013
 * 
 * @brief  simple config parser
 * 
 * 
 */

/* TODO: parse username, password */

#ifndef LOAD_CONF_H_
#define LOAD_CONF_H_

extern unsigned short  conf_port;
extern char conf_ip[128];

int load_conf(char *conf_file);

#endif /* LOAD_CONF_H_ */
