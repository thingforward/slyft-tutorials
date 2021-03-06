
// Generated by slyft.io
// This code part has been generated on an "as is" basis, without warranties or conditions of any kind.

// endpoint implementation


    #include "Arduino.h"

#include "microcoap/coap.h"
#include "tutorial3_app.h"

#include "config.h"
#include "state.h"


#define COAP_MAKERESPONSE_INTERNALSERVERERROR \
return coap_make_response(scratch, \
  outpkt, (uint8_t*)outbuf, 0, \
  id_hi, id_lo, \
  &inpkt->tok, \
  (coap_responsecode_t)MAKE_RSPCODE(5, 0), COAP_CONTENTTYPE_NONE \
  );

#define COAP_MAKERESPONSE_BADREQUEST \
return coap_make_response(scratch, \
  outpkt, (uint8_t*)outbuf, 0, \
  id_hi, id_lo, \
  &inpkt->tok, \
  COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_NONE \
  );

#define RSPCODE_CLASS(rsp)  (rsp >> 5)
#define RSPCODE_DETAIL(rsp) (rsp & 31)

// define coap handler functions

// Protocol-level handler for GET to /state
int TUT3__tutorial3__handle_get___state(coap_rw_buffer_t *scratch,
    const coap_packet_t *inpkt, coap_packet_t *outpkt,
    uint8_t id_hi, uint8_t id_lo) {

    //Serial.println("TUT3__tutorial3__handle_get___state");

    // processing
    uint8_t outbuf[512];
    mpack_memset(outbuf,0,sizeof(outbuf));
    size_t resp_len = 0;
    coap_responsecode_t resp_code;
    coap_content_type_t  resp_content_type = COAP_CONTENTTYPE_NONE;

    struct TUT3__tutorial3__get___state_req    req;
    struct TUT3__tutorial3__get___state_resp   resp;


    TUT3__state_init(&(resp.data.state_205));


    // forward req,resp structs to application handler
    bool handled = TUT3__tutorial3__get___state(&req, &resp);

    if (handled) {
      // take response code from resp
      resp_code = resp.response_code;
      // form the response body
      if ( RSPCODE_CLASS(resp_code) == 2 && RSPCODE_DETAIL(resp_code) == 05) {
        // handle code 205, schema is state
        resp_len = resp.data.state_205.TUT3__state_serialize(&(resp.data.state_205), outbuf, sizeof(outbuf));
        resp_content_type = COAP_CONTENTTYPE_APPLICATION_OCTECT_STREAM;
      }


      return coap_make_response(scratch,
        outpkt,
        (uint8_t*)outbuf, resp_len,
        id_hi, id_lo,
        &inpkt->tok,
        resp_code, resp_content_type
      );
    } else {
      COAP_MAKERESPONSE_INTERNALSERVERERROR
    }

}

// Protocol-level handler for GET to /config
int TUT3__tutorial3__handle_get___config(coap_rw_buffer_t *scratch,
    const coap_packet_t *inpkt, coap_packet_t *outpkt,
    uint8_t id_hi, uint8_t id_lo) {

    //Serial.println("TUT3__tutorial3__handle_get___config");

    // processing
    uint8_t outbuf[512];
    mpack_memset(outbuf,0,sizeof(outbuf));
    size_t resp_len = 0;
    coap_responsecode_t resp_code;
    coap_content_type_t  resp_content_type = COAP_CONTENTTYPE_NONE;

    struct TUT3__tutorial3__get___config_req    req;
    struct TUT3__tutorial3__get___config_resp   resp;


    TUT3__config_init(&(resp.data.config_205));


    // forward req,resp structs to application handler
    bool handled = TUT3__tutorial3__get___config(&req, &resp);

    if (handled) {
      // take response code from resp
      resp_code = resp.response_code;
      // form the response body
      if ( RSPCODE_CLASS(resp_code) == 2 && RSPCODE_DETAIL(resp_code) == 05) {
        // handle code 205, schema is config
        resp_len = resp.data.config_205.TUT3__config_serialize(&(resp.data.config_205), outbuf, sizeof(outbuf));
        resp_content_type = COAP_CONTENTTYPE_APPLICATION_OCTECT_STREAM;
      }


      return coap_make_response(scratch,
        outpkt,
        (uint8_t*)outbuf, resp_len,
        id_hi, id_lo,
        &inpkt->tok,
        resp_code, resp_content_type
      );
    } else {
      COAP_MAKERESPONSE_INTERNALSERVERERROR
    }

}

// Protocol-level handler for POST to /config
int TUT3__tutorial3__handle_post___config(coap_rw_buffer_t *scratch,
    const coap_packet_t *inpkt, coap_packet_t *outpkt,
    uint8_t id_hi, uint8_t id_lo) {

    //Serial.println("TUT3__tutorial3__handle_post___config");

    // processing
    uint8_t outbuf[512];
    mpack_memset(outbuf,0,sizeof(outbuf));
    size_t resp_len = 0;
    coap_responsecode_t resp_code;
    coap_content_type_t  resp_content_type = COAP_CONTENTTYPE_NONE;

    struct TUT3__tutorial3__post___config_req    req;
    struct TUT3__tutorial3__post___config_resp   resp;

    // handle input body
    TUT3__config_init(&(req.data.config));
    int parse_res = req.data.config.TUT3__config_parse(&(req.data.config),inpkt->payload.p, (uint32_t)inpkt->payload.len);
    if (parse_res != 0) {
        // get out here, unable to parse the request data
        COAP_MAKERESPONSE_BADREQUEST
    };



    // forward req,resp structs to application handler
    bool handled = TUT3__tutorial3__post___config(&req, &resp);

    if (handled) {
      // take response code from resp
      resp_code = resp.response_code;
      // form the response body

      return coap_make_response(scratch,
        outpkt,
        (uint8_t*)outbuf, resp_len,
        id_hi, id_lo,
        &inpkt->tok,
        resp_code, resp_content_type
      );
    } else {
      COAP_MAKERESPONSE_INTERNALSERVERERROR
    }

}


