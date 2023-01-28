#include <stdio.h>
#include <time.h>
#include <event.h>
#include <event_msg.h>

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
                event_timer_delete(_tmr_ctx, _tmr_id);
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
    tmr_ctx = event_timer_init(event_ctx);
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

    event_timer_add(&t3.x, tmr_ctx, 3, test_timer, &t3);
    event_timer_add(&t5.x, tmr_ctx, 5, test_timer, &t5);
    event_timer_add(&t10.x, tmr_ctx, 10, test_timer, &t10);

    _tmr_ctx = tmr_ctx;
    _tmr_id = t10.x;

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
