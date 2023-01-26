#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

struct event_ctx {
    int fd;
};

struct event {
    int fd;
    void *(*cb)(void *cbd);
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

struct event *event_create(int fd, void *(*cb)(void *cbd), void *cbd, struct event *e)
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

struct timer_event {
    uint32_t id;
    uint32_t intvl;
    uint32_t remaing_intvl;
    void (*cb)(void *cbd);
    void *cbd;
};

#define MAX_TIMER 8
struct timer_ctx {
    int fd;
    /*
     * TODO: maintain timers  in list
     */
    unsigned active_timers;
    struct timer_event events[MAX_TIMER];
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

    return &ctx;
}

void event_timer_destroy(struct timer_ctx *ctx)
{
    close(ctx->fd);
    ctx->fd = -1;
    ctx->active_timers = 0;
}

void timer_event_iter(struct timer_event *e, void *p);
void timer_event_for_each(struct timer_ctx *ctx, void (*cb)(struct timer_event *e, void *cbd),
        void *cbd);
void *event_timer_1s_handler(void *data)
{
    struct timer_ctx *ctx = (struct timer_ctx *)data;
    uint64_t res;

    read(ctx->fd, &res, sizeof(res));

    timer_event_for_each(ctx, timer_event_iter, NULL);
}

int timer_event_add(struct timer_ctx *ctx, uint32_t intvl, void (*cb)(void *cbd), void *cbd)
{
    struct timer_event *e;
    struct timespec tp;
    time_t curr_ts, next_ts;

    if (ctx->fd < 0) {
        return -1;
    }

    if (ctx->active_timers >= MAX_TIMER) {
        return -1;
    }
    e = &ctx->events[ctx->active_timers++];

    e->intvl = intvl;
    e->cb = cb;
    e->cbd = cbd;
    e->remaing_intvl = intvl;
    e->id = ctx->active_timers;
}

int timer_event_delete(struct timer_ctx *ctx, uint32_t timer_id)
{
    /*
     * TODO: implemet this when timer is maintained in list
     */
}

void timer_event_iter(struct timer_event *e, void *p)
{
    if (e->remaing_intvl > 0) {
        e->remaing_intvl--;
    }
    if (e->remaing_intvl == 0) {
        e->remaing_intvl = e->intvl;
        e->cb(e->cbd);
    }
}

void timer_event_for_each(struct timer_ctx *ctx, void (*cb)(struct timer_event *e, void *cbd),
        void *cbd)
{
    int ii;

    for (ii = 0; ii < ctx->active_timers; ii++) {
        cb(&ctx->events[ii], cbd);
    }
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
            printf("x = 0x%x\n", t->x);
        }
        break;
    case EVENT_ID_TEST2:
        {
            struct test *t = (struct test *)data;
            printf("x = 0x%x\n", t->x);
        }
        break;
    case EVENT_ID_TEST3:
        {
            struct test *t = (struct test *)data;
            printf("x = 0x%x\n", t->x);
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
    struct test t5, t10, t3;
    MSG_HDR_DEF(&t5, EVENT_ID_TEST1, sizeof(t5));
    t5.x = 0xdead;
    MSG_HDR_DEF(&t10, EVENT_ID_TEST2, sizeof(t10));
    t10.x = 0xabcd;
    MSG_HDR_DEF(&t3, EVENT_ID_TEST3, sizeof(t3));
    t3.x = 0xffff;

    timer_event_add(tmr_ctx, 5, test_timer, &t5);
    timer_event_add(tmr_ctx, 10, test_timer, &t10);
    timer_event_add(tmr_ctx, 3, test_timer, &t3);

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
