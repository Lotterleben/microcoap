#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>

#include "coap.h"
#include "debug.h"

#define PORT 5683
#define BUFSZ 4096

int make_homemade_request(uint8_t* snd_buf);

int main(int argc, char **argv)
{
    PDEBUG("[main-posix] Starting microcoap example...\n");

    int fd;
    struct sockaddr_in servaddr, cliaddr;
    uint8_t buf[BUFSZ];
    uint8_t scratch_raw[BUFSZ];
    coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

    fd = socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    bind(fd,(struct sockaddr *)&servaddr, sizeof(servaddr));

    while(1)
    {
        int n, rc;
        socklen_t len = sizeof(cliaddr);
        coap_packet_t pkt;

        n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len);
#ifdef DEBUG
        printf("[main-posix] Received packet: ");
        coap_dump(buf, n, true);
        printf("\n");
#endif

        if (0 != (rc = coap_parse(&pkt, buf, n)))
            printf("[main-posix] Bad packet rc=%d\n", rc);
        else
        {
            size_t rsplen = sizeof(buf);
            coap_packet_t rsppkt;
#ifdef DEBUG
            printf("[main-posix] content:\n");
            coap_dumpPacket(&pkt);
#endif
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);

            if (0 != (rc = coap_build(buf, &rsplen, &rsppkt)))
                printf("[main-posix] coap_build failed rc=%d\n", rc);
            else
            {
#ifdef DEBUG
                printf("[main-posix] Sending packet: ");
                coap_dump(buf, rsplen, true);
                printf("\n");
                printf("[main-posix] content:\n");
                coap_dumpPacket(&rsppkt);
#endif
                sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

                printf("[main-posix] Following up with own initial GET:\n");
                
                // clear buffer
                memset(buf, 0, BUFSZ);

                if (0 == make_homemade_request(buf)) {
                    sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
                    printf("[main-posix] homemade inital GET sent.\n");
                }
            }
        }
    }
}

/* organic, local, gluten-free */
int make_homemade_request(uint8_t* snd_buf)
{
    printf("creating example request...\n");
    static char* msg= "hello";
    size_t req_pkt_sz;
    int errcode;

    // cobble together CoAP packet
    coap_header_t req_hdr = {
        .ver = 1,
        .t = COAP_TYPE_NONCON,
        .tkl = 0,                  /* microcoap can't generate tokens anyway */
        .code = COAP_RSPCODE_GET,  /* Not sure if this is correct... */
        .id = {22,22}              /*let's see if this works :D */
    };

    coap_buffer_t payload = {
        .p = (const uint8_t *) msg,
        .len = strlen(msg)
    };

    coap_packet_t req_pkt = {
        .hdr = req_hdr,
        .tok = (coap_buffer_t) {}, /* No token */
        .numopts = 0,
        .opts = {},
        .payload = payload
    };

    req_pkt_sz = sizeof(req_pkt);

#ifdef DEBUG
    printf("[main-posix] content:\n");
    coap_dumpPacket(&req_pkt);
#endif

    // try to  write packet to send buffer
    if (0 != (errcode = coap_build(snd_buf, &req_pkt_sz, &req_pkt))) {
        printf("Error building packet! Error code: %i\nAborting. \n", errcode);
        return 1;
    }
    return 0;
}