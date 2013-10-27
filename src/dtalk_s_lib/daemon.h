/**
 * @file   daemon.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Tue Oct 22 14:59:40 2013
 * 
 * @brief  This is the core, the main stuff is handled in daemon.
 * daemon use a thread-pool (called processor). All threads originate
 * from the processor. Work is delegated to the processor by queueing
 * jobs to it.

   1. Tasks kind:
   +--------+
   |listener| listen task(only one), this task keeps running.
   +--------+
       |
       V
   +--------+   
   |receive | receive task, this task is created by listener.
   +--------+
       |
       V
   +--------+   
   |process | process task, this task is created by receiver
   +--------+
       |              |                |             |
       V              V                V             V
   +--------+     +--------+       +--------+
   |  log   |     |login...|       |  send  |       ...
   +--------+     +--------+       +--------+

   process task can create many different kind of tasks.

   2. Task queue:
   +---------------------------------+
   |           tasks queue           |
   +---------------------------------+
   ^                                 ^
   |                                 |
   threads, get tasks.               threads, add tasks.

   3. The processer manage all the threads, in/decrease thread num, etc.

   4. The main thread handle the start, stop, segment detect, etc.
*/   

#include "online_rooms.h"
#include "online_users.h"

extern online_users_t *online_users;
extern online_rooms_t *online_rooms;
    
void daemon_init();
