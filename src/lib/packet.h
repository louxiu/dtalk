/**
 * @file   packet.h
 * @author louxiu <tula.istok@gmail.com>
 * @date   Wed Oct 23 02:35:50 2013
 * 
 * @brief  
 * 
 * 
 */

#ifndef PACKET_H_
#define PACKET_H_

#define MAX_PACKET_SIZE 1024

typedef struct packet_t packet_t;

struct packet_t {
    char *(*get_payload)(packet_t *packet);
    void  (*set_payload)(packet_t *packet, char *payload);

    int   (*get_fd)  (packet_t *packet);    
    void  (*set_fd)  (packet_t *packet, int fd);

    int   (*get_size)(packet_t *packet);    
    void  (*set_size)(packet_t *packet, int size);
    void  (*packet_send) (packet_t *packet);
    void  (*destroy) (packet_t *packet);              

};

packet_t *packet_create(void);

/* packet_t *receive_packet(int fd); */
/* int send_packet(packet_t *this); */

#endif /* PACKET_H_ */
