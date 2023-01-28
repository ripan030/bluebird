#ifndef _EVENT_MSG_
#define _EVENT_MSG_

#include <inttypes.h>

/*
 * Event header
 * @version: header version
 * @id: event id
 * @length: length of msg
 */
struct event_hdr {
    uint8_t version;
    uint8_t id;
    uint16_t length;
};

/*
 * Macros to SET/GET header fields
 */
#define HDR_MSG_ID_GET(_msg, _id) { \
    do { \
        struct event_hdr *hdr = (struct event_hdr *)(_msg); \
        _id = hdr->id; \
    } while (0); }

#define HDR_MSG_ID_SET(_msg, _id) { \
    do { \
        struct event_hdr *hdr = (struct event_hdr *)(_msg); \
        hdr->id = _id; \
    } while (0); }

#define HDR_MSG_LENGTH_GET(_msg, _length) { \
    do { \
        struct event_hdr *hdr = (struct event_hdr *)(_msg); \
        _length = hdr->length; \
    } while (0); }

#define HDR_MSG_LENGTH_SET(_msg, _length) { \
    do { \
        struct event_hdr *hdr = (struct event_hdr *)(_msg); \
        hdr->length = _length; \
    } while (0); }

#define MSG_HDR_DEF_V1(__msg, __id, __length) { \
    do { \
        struct event_hdr *hdr = (struct event_hdr *)(__msg); \
        hdr->version = 1; \
        hdr->id = __id; \
        hdr->length = __length; \
    } while (0); }

#define MSG_HDR_DEF MSG_HDR_DEF_V1

/*
 * Event id enum
 */
enum event_id {
    EVENT_ID_MAX = 128,
    EVENT_ID_TEST1,
    EVENT_ID_TEST2,
    EVENT_ID_TEST3,
};
#endif
