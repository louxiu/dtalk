#include "dtalk_c_room.h"

int check_if_in(room_handle_t *room_handle, char *room_name)
{
    all_room_t *all_room = room_handle->all_room_list;
    while (all_room != NULL){
        if (strcmp(all_room->room_name, room_name) == 0)
            break;
        all_room = all_room->next;
    }

    if (all_room != NULL)
        return 0;
    else
        return -1;
}

void free_user_list(user_t **userlist)
{
    user_t *curp = *userlist;
    user_t *next;
    while (curp != NULL){
        next = curp->next;
        free(curp);
        curp = next;
    }

    *userlist = NULL;
}

void free_use_room_list(use_room_t **use_room_list)
{
    use_room_t *curp = *use_room_list;
    use_room_t *next;
    while (curp != NULL){
        next = curp->next;
        free_user_list(&(curp->user_list));
        free(curp);
        curp = next;
    }

    *use_room_list = NULL;
}

void free_all_room_list(all_room_t **all_room_list)
{
    all_room_t *curp = *all_room_list;
    all_room_t *next;
    while (curp != NULL){
        next = curp->next;
        free(curp);
        curp = next;
    }

    *all_room_list = NULL;   
}

void room_destory(room_handle_t *room_handle)
{
    free_all_room_list(&(room_handle->all_room_list));
    free_use_room_list(&(room_handle->use_room_list));
}

/* add room into use_room_list */
void room_join(room_handle_t *room_handle, char *room_name)
{

    /* check the use-room */
    use_room_t *useroom = room_handle->use_room_list;
    if (useroom == NULL){
        /* useRoom list is null, malloc a room and put it into useroomLIst */
        useroom = (use_room_t *)malloc(sizeof(use_room_t));
        if (useroom == NULL)
            exit;
        useroom->next = NULL;
        useroom->user_list = NULL;
        sprintf(useroom->room_name, "%s", room_name);
        room_handle->use_room_list = useroom;
    }else {
        while (useroom != NULL){
            if (strcmp(useroom->room_name, room_name) == 0){
                err_print("room is already joined");
                return;
            }
            useroom = useroom->next;
        }

        /* new a room */
        useroom = (use_room_t *)malloc(sizeof(use_room_t));
        if (useroom == NULL)
            exit;
        useroom->next = NULL;
        useroom->user_list = NULL;
        sprintf(useroom->room_name, "%s", room_name);

        /* add into use_room */
        use_room_t *buf = room_handle->use_room_list->next;
        room_handle->use_room_list = useroom;
        useroom->next = buf;
    }
    /* change current chat room */
    room_handle->cur_room = useroom;
}


/* delete room from use_room_list */
void room_logout(room_handle_t *room_handle, char *room_name)
{
    use_room_t *useroom = room_handle->use_room_list;
    if (useroom == NULL){
        return;
    } else {
        use_room_t *next = useroom->next;
        use_room_t *buf;
        if (strcmp(useroom->room_name, room_name) == 0){
            free(useroom);
            room_handle->use_room_list = next;
            return;
        } else {
            while (next != NULL){
                if (strcmp(next->room_name, room_name) == 0){
                    buf = next->next;
                    free(next);
                    useroom->next = buf;
                    return;
                }
                useroom = useroom->next;
                next = next->next;
            }
            return;
        }
    }
}

/* add user into certen room  */
void room_update_add(room_handle_t *room_handle, char *room_name, char *user_name)
{
    use_room_t *useroom = room_handle->use_room_list;
    while (useroom != NULL){
        if (strcmp(useroom->room_name, room_name) == 0){
            break;
        }
        useroom = useroom->next;
    }
    if (useroom == NULL){
        return;
    } else {
        user_t *user = useroom->user_list;
        user_t *pre = NULL;
        while (user != NULL){
            if (strcmp(user->user_name, user_name) == 0){
                break;
            }
            pre = user;
            user = user->next;
        }
        if (user != NULL){
            return;
        }else {
            user = (user_t *)malloc(sizeof(user_t)) ;
            if (user == NULL){
                exit(-1);
            }
            sprintf(user->user_name, "%s", user_name);
            user->next = NULL;
            if (pre == NULL){
                pre = user;
            }else {
                pre->next = user;
            }
        }
    }
}


/* delete user from room in use_room_list */
void room_update_del(room_handle_t *room_handle, 
                    char *room_name, char *user_name)
{
    /* check if in the use_room_list */
    use_room_t *useroom = room_handle->use_room_list;
    while (useroom != NULL){
        if (strcmp(useroom->room_name, room_name) == 0){
            break;
        }
        useroom = useroom->next;
    }
    if (useroom == NULL){   /* no in, return */
        return;
    }else { /* in */
        user_t *user = useroom->user_list;
        user_t *pre = NULL;
        while (user != NULL){
            if (strcmp(user->user_name, user_name) == 0){
                break;
            }
            pre = user;
            user = user->next;
        }
        if (user != NULL){
            pre->next = user->next;
            free(user);
        }else {
            return;
        }
    }
    return;
}

void room_init(room_handle_t *room_handle)
{
    room_handle->use_room_list = NULL;
    room_handle->all_room_list = NULL;
    room_handle->cur_room = NULL;
}

/* delete room from all_room_list */
void room_list_del(room_handle_t *room_handle, char *room_name)
{
    return;
    /* room does not exist */
    if (check_if_in(room_handle, room_name) != 0)
        return;
    /* delete room from allRoomList */
    all_room_t *all_room = room_handle->all_room_list;
    if (all_room == NULL){
        return;
    }else {
        if (strcmp(all_room->room_name, room_name) == 0){
            all_room_t *buf = all_room->next;
            free(all_room);
            room_handle->all_room_list = buf;
            return;
        }
        while (all_room->next != NULL){
            if (strcmp(all_room->next->room_name, room_name) == 0){
                all_room_t *buf = all_room->next->next;
                free(all_room->next);
                all_room ->next = buf;
            }

            all_room = all_room->next;
        }
    }
}

/* add room into all_room_list */
void room_list_add(room_handle_t *room_handle, char *room_name)
{
    return;
    if (check_if_in(room_handle, room_name) == 0)
        return;

    all_room_t *next = room_handle->all_room_list -> next;
    all_room_t *buf = (all_room_t *)malloc(sizeof(all_room_t));
    memset(buf, 0, sizeof(all_room_t));
    buf->next = next;
    room_handle->all_room_list = buf;
}
