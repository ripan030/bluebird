#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

struct event_ctx {
    int fd;
};

struct event {
    int fd;
    void (*cb)(void *cbd);
    void *cbd;
};

struct event_ctx *event_init()
{
    static struct event_ctx ctx = {
        .fd = -1,
    };
    int fd;

    fd = epoll_create1(0);
    if (fd < 0) {
        return NULL;
    }

    ctx.fd = fd;

    return &ctx;
}

void event_destroy(struct event_ctx *ctx)
{
    close(ctx->fd);
    ctx->fd = -1;
}

struct event *event_create(int fd, void (*cb)(void *cbd), void *cbd, struct event *e)
{
    if (e) {
        e->fd = fd;
        e->cb = cb;
        e->cbd = cbd;
    }

    return e;
}

int event_add(struct event_ctx *ctx, struct event *e)
{
    struct epoll_event event;
    int epoll_fd, fd;
    int r;

    epoll_fd = ctx->fd;
    fd = e->fd;

    if (epoll_fd < 0) {
        return -1;
    }

    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = e;

    r = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if (r < 0) {
        return -1;
    }

    return 0;
}

int event_delete(struct event_ctx *ctx, struct event *e)
{
    struct epoll_event event;
    int epoll_fd, fd;
    int r;

    epoll_fd = ctx->fd;
    fd = e->fd;

    if (epoll_fd < 0) {
        return -1;
    }

    event.events = EPOLLIN;
    event.data.ptr = &e;

    r = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event);
    if (r < 0) {
        return -1;
    }

    return -1;
}

void event_handler(struct event *e)
{
    if (e->cb) {
        e->cb(e->cbd);
    }
}

int event_start(struct event_ctx *ctx)
{
    const int maxevents = 8;
    const int timeout_ms = 500;
    struct epoll_event events[maxevents];
    int epoll_fd;
    int nfd;
    int i;

    epoll_fd = ctx->fd;
    if (epoll_fd < 0) {
        return -1;
    }

    /*
     * Consume thread
     */
    while (1) {
        nfd = epoll_wait(epoll_fd, events, maxevents, timeout_ms);
        if (nfd <= 0) {
            continue;
        }
        for (i = 0; i < nfd; i++) {
            event_handler(events[i].data.ptr);
        }
    }

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <sys/timerfd.h>
#include <inttypes.h>
#include <stdint.h>
#include "list.h"

struct timer_ctx;

struct timer_event {
    uint32_t id;
    uint32_t intvl;
    uint32_t remaing_intvl;
    void (*cb)(void *cbd);
    void *cbd;
    struct timer_ctx *tmr_ctx;
};

#define MAX_TIMER 8
struct timer_ctx {
    int fd;
    unsigned active_timers;
    struct node *event_list;
};

struct timer_ctx *event_timer_init()
{
    static struct timer_ctx ctx = {
        .fd = -1,
        .active_timers = 0,
    };
    struct itimerspec ts;
    int fd;
    int r;

    fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd < 0) {
        return NULL;
    }

    ts.it_interval.tv_sec = 1;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = 1;
    ts.it_value.tv_nsec = 0;

    r = timerfd_settime(fd, 0, &ts, NULL);
    if (r < 0) {
        close(fd);
        return NULL;
    }

    ctx.fd = fd;
    list_init(&ctx.event_list);

    return &ctx;
}

void timer_event_free(struct timer_event *e)
{
    free(e);
}

void timer_event_node_free(void *p)
{
    struct timer_event *e = (struct timer_event *)p;
    struct timer_ctx *ctx = e->tmr_ctx;

    timer_event_free(e);
    if (ctx->active_timers > 0)
        ctx->active_timers--;
}

int timer_event_node_is_match_all(struct node *n, void *p)
{
    return 1;
}

void timer_event_delete_all(struct timer_ctx *ctx)
{
    list_node_delete(&ctx->event_list,
            timer_event_node_is_match_all, NULL,
            timer_event_node_free);
}

void event_timer_destroy(struct timer_ctx *ctx)
{
    timer_event_delete_all(ctx);
    close(ctx->fd);
    ctx->fd = -1;
    ctx->active_timers = 0;
}

void timer_event_node_iter_cb(struct node *n, void *cbd)
{
    struct timer_event *e = (struct timer_event *)n->data;

    if (e->remaing_intvl > 0) {
        e->remaing_intvl--;
    }

    if (e->remaing_intvl == 0) {
        e->remaing_intvl = e->intvl;
        e->cb(e->cbd);
    }
}

void event_timer_1s_handler(void *data)
{
    struct timer_ctx *ctx = (struct timer_ctx *)data;
    uint64_t res;

    read(ctx->fd, &res, sizeof(res));

    list_iter(&ctx->event_list, timer_event_node_iter_cb, NULL);
}

struct timer_event *
timer_event_new(struct timer_ctx *ctx, uint32_t intvl, void (*cb)(void *cbd), void *cbd)
{
    struct timer_event *e;

    e = malloc(sizeof(struct timer_event));

    if (e) {
        e->intvl = intvl;
        e->cb = cb;
        e->cbd = cbd;
        e->remaing_intvl = intvl;
        e->id = ctx->active_timers;
        e->tmr_ctx = ctx;
    }

    return e;
}

int timer_event_add(uint32_t *id, struct timer_ctx *ctx, uint32_t intvl, void (*cb)(void *cbd), void *cbd)
{
    struct timer_event *e;
    struct node *n;

    if (ctx->fd < 0) {
        return -1;
    }

    e = timer_event_new(ctx, intvl, cb, cbd);
    if (!e) {
        return -1;
    }

    n = list_node_new(e);
    if (!n) {
        timer_event_free(e);
        return -1;
    }

    list_node_add(&ctx->event_list, n);

    ctx->active_timers += 1;

    if (id) {
        *id = e->id;
    }

    return 0;
}

int timer_event_node_is_match(struct node *n, void *p)
{
    struct timer_event *e = (struct timer_event *)n->data;
    uint32_t *id = (uint32_t *)p;

    if (e->id == *id) {
        return 1;
    }

    return 0;
}

int timer_event_delete(struct timer_ctx *ctx, uint32_t timer_id)
{
    list_node_delete(&ctx->event_list,
            timer_event_node_is_match, &timer_id,
            timer_event_node_free);
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Event header
 * @version: header version
 * @id: event id
 * @length: length of msg
 */
struct event_hdr {
    /* 1st field - version */
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

/*
 * Netlink context
 * @nl_sock: netlink socket
 */
struct nl_ctx {
    int nl_sock;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
struct test {
    struct event_hdr hdr;
    uint32_t x;
};

static struct timer_ctx *_tmr_ctx;
static uint32_t _tmr_id;

static void test_timer(void *data)
{
    struct event_hdr *hdr;
    struct timespec tp;

    if (!data) return;

    clock_gettime(CLOCK_MONOTONIC, &tp);

    hdr = (struct event_hdr *)data;

    printf("%s: [%ld] event_id %u length %u\n", __func__, tp.tv_sec, hdr->id, hdr->length);

    switch (hdr->id) {
    case EVENT_ID_TEST1:
        {
            struct test *t = (struct test *)data;
            printf("3s: id = 0x%x\n", t->x);
        }
        break;
    case EVENT_ID_TEST2:
        {
            struct test *t = (struct test *)data;
            printf("5s: id = 0x%x\n", t->x);

        }
        break;
    case EVENT_ID_TEST3:
        {
            struct test *t = (struct test *)data;
            printf("10s: id = 0x%x\n", t->x);
            printf("Deleting 10s timer...\n");
            if (_tmr_id > 0) {
                timer_event_delete(_tmr_ctx, _tmr_id);
            }
        }
        break;
    default:
        printf("Unknown event id %u\n", hdr->id);
    }
}

int main()
{
    struct event_ctx *event_ctx;
    struct timer_ctx *tmr_ctx;
    struct event timer_event;
    int r;

    /*
     * Create event context
     */
    event_ctx = event_init();
    if (!event_ctx) {
        return 1;
    }

    /*
     * Init timer context
     */
    tmr_ctx = event_timer_init();
    if (!tmr_ctx) {
        event_destroy(event_ctx);
        return 1;
    }

    /*
     * Add timers
     */
    struct test t3, t5, t10;
    MSG_HDR_DEF(&t3, EVENT_ID_TEST1, sizeof(t3));
    MSG_HDR_DEF(&t5, EVENT_ID_TEST2, sizeof(t5));
    MSG_HDR_DEF(&t10, EVENT_ID_TEST3, sizeof(t10));

    timer_event_add(&t3.x, tmr_ctx, 3, test_timer, &t3);
    timer_event_add(&t5.x, tmr_ctx, 5, test_timer, &t5);
    timer_event_add(&t10.x, tmr_ctx, 10, test_timer, &t10);

    _tmr_ctx = tmr_ctx;
    _tmr_id = t10.x;

    /*
     * Add timers to event context
     */
    event_create(tmr_ctx->fd, event_timer_1s_handler, tmr_ctx, &timer_event);
    r = event_add(event_ctx, &timer_event);
    if (r < 0) {
        event_timer_destroy(tmr_ctx);
        event_destroy(event_ctx);
        return 1;
    }

    /*
     * Start event loop
     */
    r = event_start(event_ctx);
    if (r < 0) {
        event_timer_destroy(tmr_ctx);
        event_destroy(event_ctx);
        return 1;
    }

    event_timer_destroy(tmr_ctx);
    event_destroy(event_ctx);

    return 0;
}
