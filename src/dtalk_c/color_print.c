#include <stdio.h>
#include "color_print.h"

//blackground:40--49          word:30--39
//40: black                        30: black
//41: red                          31: red 
//42: green                        32: green 
//43: yellow                       33: yellow 
//44: blue                         34: blue
//45: purple                       35: purple
//46: dark green                   36: dark green
//47: white                        37: white
//

/*
  none         = "\033[0m"
  black        = "\033[0;30m"
  dark_gray    = "\033[1;30m"
  blue         = "\033[0;34m"
  light_blue   = "\033[1;34m"
  green        = "\033[0;32m"
  light_green -= "\033[1;32m"
  cyan         = "\033[0;36m"
  light_cyan   = "\033[1;36m"
  red          = "\033[0;31m"
  light_red    = "\033[1;31m"
  purple       = "\033[0;35m"
  light_purple = "\033[1;35m"
  brown        = "\033[0;33m"
  yellow       = "\033[1;33m"
  light_gray   = "\033[0;37m"
  white        = "\033[1;37m"
*/

/* start color set */
#define START_RED    " \033[0;31m "
#define START_GREEN  " \033[0;32m "
#define START_YELLOW " \033[0;33m "
#define START_BLUE   " \033[0;34m "
#define START_PURPLE " \033[0;35m "
#define START_WHITE  " \033[0;37m "

/* close color set */
#define CLOSE_COLOR " \033[0m\n"

void err_print(char *info)
{
    printf(START_RED "%s" CLOSE_COLOR, info);
}

void inf_print(char *info)
{
    printf(START_GREEN "%s" CLOSE_COLOR, info);
}

void msg_print(char *info)
{
    printf("%s", info);    
}

void room_print(char *info)
{
    printf(START_YELLOW "%s" CLOSE_COLOR, info);
}

void user_print(char *info)
{
    printf(START_PURPLE "%s" CLOSE_COLOR, info);
}
