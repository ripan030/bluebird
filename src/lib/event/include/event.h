#ifndef _EVENT_H
#define _EVENT_H

#include <inttypes.h>

#include <list.h>

struct event_ctx {
    int fd;
};

struct event {
    int fd;
    void (*cb)(void *cbd);
    void *cbd;
};

struct timer_ctx;

struct timer_event {
    uint32_t id;
    uint32_t intvl;
    uint32_t remaing_intvl;
    void (*cb)(void *cbd);
    void *cbd;
    struct timer_ctx *tmr_ctx;
};

struct timer_ctx {
    int fd;
    unsigned active_timers;
    struct node *event_list;
    struct event timer_event;
    struct event_ctx *event_ctx;
};

struct event_ctx *event_init();
void event_destroy(struct event_ctx *ctx);
struct event *event_create(int fd, void (*cb)(void *cbd), void *cbd, struct event *e);
int event_add(struct event_ctx *ctx, struct event *e);
int event_delete(struct event_ctx *ctx, struct event *e);
int event_start(struct event_ctx *ctx);

struct timer_ctx *event_timer_init(struct event_ctx *event_ctx);
void event_timer_destroy(struct timer_ctx *ctx);
int event_timer_add(uint32_t *id, struct timer_ctx *ctx, uint32_t intvl, void (*cb)(void *cbd), void *cbd);
int event_timer_delete(struct timer_ctx *ctx, uint32_t timer_id);
#endif
