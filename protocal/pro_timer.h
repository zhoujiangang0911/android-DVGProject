#ifndef _PRO_TIMER_H_
#define _PRO_TIMER_H_
#include "config.h"
#include <time.h>

enum pro_timer_id_ {
    PRO_TIMER_SYN_FRAME,
    PRO_TIMER_LINK_TIMEOUT,
    PRO_TIMER_LERROR_POWERON_FRAME,
    PRO_TIMER_HANDSHAKE_DOWN,
    PRO_TIMER_HANDSHAKE_UP,
    PRO_TIMER_POWERON_FRAME_TIMEOUT,
    PRO_TIMER_MAX,
};

typedef void (*pro_timer_handler)();

int pro_timer_create(timer_t* ptimer, pro_timer_handler handler, int pro_timer_id);
int pro_timer_cancel(timer_t timer);
int pro_timer_is_running(timer_t timer);
#endif
