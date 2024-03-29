/*
 * event headers
 */
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

/*
 * event timer headers
 */
#include <sys/timerfd.h>
#include <time.h>
#include <inttypes.h>
#include <stdint.h>

#include <event.h>
#include <list.h>

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

static void event_handler(struct event *e)
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

/*
 * Event timer implementation
 */
static void timer_event_node_iter_cb(struct node *n, void *cbd)
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

static void event_timer_1s_handler(void *data)
{
    struct timer_ctx *ctx = (struct timer_ctx *)data;
    uint64_t res;

    read(ctx->fd, &res, sizeof(res));

    list_iter(&ctx->event_list, timer_event_node_iter_cb, NULL);
}

struct timer_ctx *event_timer_init(struct event_ctx *event_ctx)
{
    static struct timer_ctx ctx = {
        .fd = -1,
        .active_timers = 0,
        .event_ctx = NULL,
        .timer_event = {-1, NULL, NULL},
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

    list_init(&ctx.event_list);

    event_create(fd, event_timer_1s_handler, &ctx, &ctx.timer_event);

    r = event_add(event_ctx, &ctx.timer_event);
    if (r < 0) {
        close(fd);
        return NULL;
    }

    ctx.fd = fd;
    ctx.event_ctx = event_ctx;

    return &ctx;
}

static void timer_event_free(struct timer_event *e)
{
    free(e);
}

static void timer_event_node_free(void *p)
{
    struct timer_event *e = (struct timer_event *)p;
    struct timer_ctx *ctx = e->tmr_ctx;

    timer_event_free(e);
    if (ctx->active_timers > 0)
        ctx->active_timers--;
}

static int timer_event_node_is_match_all(struct node *n, void *p)
{
    return 1;
}

static void timer_event_delete_all(struct timer_ctx *ctx)
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

static struct timer_event *
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

int event_timer_add(uint32_t *id, struct timer_ctx *ctx, uint32_t intvl, void (*cb)(void *cbd), void *cbd)
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

static int timer_event_node_is_match(struct node *n, void *p)
{
    struct timer_event *e = (struct timer_event *)n->data;
    uint32_t *id = (uint32_t *)p;

    if (e->id == *id) {
        return 1;
    }

    return 0;
}

int event_timer_delete(struct timer_ctx *ctx, uint32_t timer_id)
{
    list_node_delete(&ctx->event_list,
            timer_event_node_is_match, &timer_id,
            timer_event_node_free);
    return 0;
}
