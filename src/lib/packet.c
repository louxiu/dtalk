/**
 * @file   packet.c
 * @author louxiu <tula.istok@gmail.com>
 * @date   Wed Oct 23 09:34:16 2013
 * 
 * @brief  
 * 
 * 
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "packet.h"

typedef struct private_packet_t private_packet_t;

struct private_packet_t {
    packet_t public;
    /* where the packet comes from or goto */
    int fd;
    int size;
    char *payload;
};

static int get_fd(private_packet_t *this)
{
    return this->fd;
}
static void set_fd(private_packet_t *this, int fd)
{
    this->fd = fd;
}

static int get_size(private_packet_t *this)
{
    return this->size;
}

static void set_size(private_packet_t *this, int size)
{
    this->size = size;
}

static char *get_payload(private_packet_t *this)
{
    return this->payload;
}

static void set_payload(private_packet_t *this, char *payload)
{
    if (payload == NULL){
        return;
    }
    strncpy(this->payload, payload, MAX_PACKET_SIZE-1);
}

static void packet_send(private_packet_t *this)
{
    int res;
    res = write(this->fd, this->payload, this->size);
    printf("res:%d, content:%s\n", res, this->payload);
}

/* TODO: receive packet function */
static void destroy(private_packet_t *this)
{
    /* assert(this->payload != NULL); */
    free(this->payload);
    free(this);
}
/*
 * documented in header
 */
packet_t *packet_create()
{
	private_packet_t *this = malloc(sizeof(private_packet_t));

    this->public.get_payload = (char* (*)(packet_t *packet))get_payload;
    this->public.set_payload =
        (void (*)(packet_t *packet, char *payload))set_payload;

    this->public.get_size = (int (*)(packet_t *packet))get_size;
    this->public.set_size = (void (*)(packet_t *packet, int size))set_size;

    this->public.get_fd = (int (*)(packet_t *packet))get_fd;
    this->public.set_fd = (void (*)(packet_t *packet, int fd))set_fd;

    this->public.packet_send = (void (*)(packet_t *packet))packet_send;
    this->public.destroy = (void (*)(packet_t *packet))destroy;

    this->payload = malloc(MAX_PACKET_SIZE * sizeof(char));
    this->fd = -1;
    this->size = 0;

    return &this->public;
}

