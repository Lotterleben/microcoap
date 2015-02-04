#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <strings.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "coap.h"
#include "debug.h"

#define BUFSZ 4096

int main(int argc, char **argv)
{
    PDEBUG("Starting microcoap client...\n");
    
    uint8_t snd_buf[BUFSZ];
    int sock, serv_port, errcode;
    in_addr_t serv_ip;
    struct sockaddr_in servaddr;
    uint8_t msg = 23;
    size_t req_pkt_sz;

    sock = socket(AF_INET,SOCK_DGRAM,0);
    
    if (argc != 3) {
        printf("usage:  microcoap-client <Server IP> <Server port>\n");
        return 1;
    }

    serv_ip = inet_addr(argv[1]);
    serv_port = atoi(argv[2]);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = serv_ip;
    servaddr.sin_port = htons(serv_port);

    printf("creating example request...\n");
    // TODO

    // cobble together CoAP packet
    coap_header_t req_hdr = {
        .ver = 1,
        .t = COAP_TYPE_NONCON,
        .tkl = 0,                  /* microcoap can't generate tokens anyway */
        .code = COAP_RSPCODE_GET,  /* Not sure if this is correct... */
        .id = {13,37}              /*let's see if this works :D */
    };

    coap_buffer_t payload = {
        .p = &msg,
        .len = sizeof(msg)
    };

    coap_packet_t req_pkt = {
        .hdr = req_hdr,
        .tok = (coap_buffer_t) {}, /* No token */
        .numopts = 0,
        .opts = {},
        .payload = payload
    };

    req_pkt_sz = sizeof(req_pkt);

    // try to  write packet to send buffer
    if (0 != (errcode = coap_build(snd_buf, &req_pkt_sz, &req_pkt))) {
        printf("Error building packet! Error code: %i\nAborting. \n", errcode);
        return 1;
    }

    printf("sending example request...\n");
#ifdef DEBUG
    coap_dumpPacket(&req_pkt);
#endif

    sendto(sock, snd_buf, sizeof(snd_buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Waiting for an answer... (TODO)\n");
    // TODO

    return 0;
}