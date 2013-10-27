/**
 * @file   load_conf.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Sat Oct 26 22:43:00 2013
 * 
 * @brief  
 * 
 * 
 */
#include <stdio.h>
#include <string.h>
#include "load_conf.h"

#define MAX_LINE_SIZE 100

unsigned short conf_port = 0;
char conf_ip[128] = {'\0'};

int load_conf(char *conf_file)
{
	FILE *fp = NULL;

	if ((fp = fopen(conf_file, "r")) == NULL){
		return -1;
	}

	char line_buffer[MAX_LINE_SIZE];
	memset(line_buffer, '\0', MAX_LINE_SIZE);
	while (fgets(line_buffer, MAX_LINE_SIZE - 1, fp) != NULL){
        /* ignore the comments and other non-sense lines */
        if (!isalpha(line_buffer[0])){
			memset(line_buffer, '\0' ,MAX_LINE_SIZE);			
            continue;
        }
		char *needle = NULL;
		if ((needle = strstr(line_buffer, "port")) != NULL){
			if ((needle = strstr(needle, "=")) != NULL){
				sscanf(needle + 1, "%hd", &conf_port);
			}
		}
		if ((needle = strstr(line_buffer, "ip")) != NULL){
			if ((needle = strstr(needle, "=")) != NULL){
				sscanf(needle + 1, "%s", conf_ip);
			}
		}
		memset(line_buffer, '\0' ,MAX_LINE_SIZE);
	}
	return 0;
}

#ifdef CONF_TEST
int main(int argc, char *argv[])
{
	load_conf(argv[1]);
	printf ("%d\n", conf_port);
	printf ("%s\n", conf_ip);	
    return 0;
}
#endif /* CONF_TEST */
