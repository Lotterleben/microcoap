#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include <arpa/inet.h>

#include "coap.h"
#include "coap_ext.h"

#define SERV_PORT 5684
#define CLI_PORT 5683
#define BUFSZ 500

int main(int argc, char **argv)
{
    printf("setting up example server...\n");

    int fd;
    struct sockaddr_in servaddr, cliaddr;
    uint8_t buf[BUFSZ];
    uint8_t scratch_raw[BUFSZ];
    coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};
    char *payload = "xoxods";
    static coap_endpoint_path_t path = {1, {"helloWorld"}}; //TODO: correct path?! //{2, {"node", "humidity"}};
    size_t rsplen = sizeof(buf);

    fd = socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    bind(fd,(struct sockaddr *)&servaddr, sizeof(servaddr));

    // TODO: prepare cliaddr
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    cliaddr.sin_port = htons(CLI_PORT);

    printf("Sending initial PUT to %s on port %hu...\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
    if (0 == coap_ext_build_PUT(buf, &rsplen, payload, &path)) {
        sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
        printf("[main-posix] inital PUT sent.\n");
    }

    while(1)
    {
        int n, rc;
        socklen_t len = sizeof(cliaddr);
        coap_packet_t pkt;

        n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len);
        printf("Received data from %s:\n", inet_ntoa(cliaddr.sin_addr));
        printf(", rsplen: %zd\n", sizeof(buf));

        if (0 != (rc = coap_parse(&pkt, buf, n)))
            printf("Bad packet rc=%d\n", rc);
        else
        {
            coap_packet_t rsppkt;
#ifdef DEBUG
            coap_dumpPacket(&pkt);
#endif
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);

            if (0 != (rc = coap_build(buf, &rsplen, &rsppkt)))
                printf("coap_build failed rc=%d\n", rc);
            else
            {
#ifdef DEBUG
                printf("Sending: ");
                coap_dump(buf, rsplen, true);
                printf(", rsplen: %zd\n", rsplen);
#endif
#ifdef DEBUG
                coap_dumpPacket(&rsppkt);
#endif

                sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
            }
        }
    }
}

