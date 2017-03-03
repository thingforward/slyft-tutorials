// Generated by slyft.io
// This code part has been generated on an "as is" basis, without warranties or conditions of any kind.


    #include "Arduino.h"


#include "stdarg.h"
#include "stdbool.h"
#include "string.h"

#include "udp_microcoap_wrapper.h"
#include "microcoap/coap.h"
#include <udp.h>

//
void _handle_udp_coap_message(udp_microcoap_wrapper *obj) {

    int sz;
    int rc;
    coap_packet_t pkt;
    int i;

    UDP *udp = (UDP *) obj->p_udp;

    if ((sz = udp->parsePacket()) > 0) {
        udp->read(obj->packetbuf, sizeof(obj->packetbuf));

        if (obj->b_debug) {
            for (i = 0; i < sz; i++) {
                Serial.print(obj->packetbuf[i], HEX);
                Serial.print(" ");
            }
            Serial.println("");
        }

        if (0 != (rc = coap_parse(&pkt, obj->packetbuf, sz))) {
            if (obj->b_debug) {
                Serial.print("Bad packet rc=");
                Serial.println(rc, DEC);
            }
        } else {
            if (obj->b_debug) {
                Serial.println("Processing CoAP message");
            }

            size_t rsplen = sizeof(obj->packetbuf);
            coap_packet_t rsppkt;
            coap_handle_req(&obj->scratch_buf, &pkt, &rsppkt);

            memset(obj->packetbuf, 0, sizeof(obj->packetbuf));
            if (0 != (rc = coap_build(obj->packetbuf, &rsplen, &rsppkt))) {
                if (obj->b_debug) {
                    Serial.print("coap_build failed rc=");
                    Serial.println(rc, DEC);
                }
            } else {
                udp->beginPacket(udp->remoteIP(), udp->remotePort());
                uint8_t *buf = obj->packetbuf;
                while (rsplen--)
                    udp->write(*buf++);
                udp->endPacket();
            }
        }
    }
}

void _clear_packet_buf(udp_microcoap_wrapper *obj) {
    memset(obj->packetbuf, 0, sizeof(obj->packetbuf));
}

// Singleton
static udp_microcoap_wrapper udp_microcoap_wrapper_obj;
static udp_microcoap_wrapper_operations udp_microcoap_wrapper_operations_obj;

udp_microcoap_wrapper *udp_microcoap_wrapper_init(void *p_udp) {
    udp_microcoap_wrapper *obj = &udp_microcoap_wrapper_obj;

    udp_microcoap_wrapper_obj.p_udp = p_udp;
    obj->ops = &udp_microcoap_wrapper_operations_obj;
    obj->ops->handle_udp_coap_message = _handle_udp_coap_message;
    obj->ops->clear_packet_buf = _clear_packet_buf;

    return obj;
}

udp_microcoap_wrapper *udp_microcoap_wrapper_get() {
    udp_microcoap_wrapper *obj = &udp_microcoap_wrapper_obj;
    return obj;
}

void udp_microcoap_wrapper_destroy(udp_microcoap_wrapper *p) {
    ;
}