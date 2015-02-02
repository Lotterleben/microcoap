#include <stdbool.h>
#include <string.h>
#include "coap.h"

const uint16_t response_len = 1500;
static char response[response_len] = "";

void get_humidity(void)
{
    strncat(response, "1337", response_len-5);
}

static const coap_endpoint_path_t path_iop = {2, {"plant", "humidity"}};
static int handle_get_humidity(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    PDEBUG("[endpoints]  %s()\n",  __func__);
    get_humidity();
    /* NOTE: COAP_RSPCODE_CONTENT only works in a packet answering a GET. */
    return coap_make_response(scratch, outpkt, (const uint8_t *)response, strlen(response),
                              id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

const coap_endpoint_t endpoints[] =
{
    {COAP_METHOD_GET, handle_get_humidity, &path_iop, "ct=0"},
    {(coap_method_t)0, NULL, NULL, NULL} /* marks the end of the endpoints array */
};