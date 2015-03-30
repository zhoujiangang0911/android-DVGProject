#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "pro_timer.h"


typedef struct pro_timers_{
    timer_t timer;
    pro_timer_handler handler;
} pro_timers;

int timers_span[PRO_TIMER_MAX] = {300,	500, 10000, 10000, 10000, 5000}; // ms
int timers_type[PRO_TIMER_MAX] = {0, 0, 1, 0, 0, 1}; // 1: perodic timer, 0: one callback timer
pro_timers timers[PRO_TIMER_MAX];

void pro_timer_thread(union sigval v)
{
    printf("pro_timer_thread v:%d\n", v.sival_int);
    if (timers[v.sival_int].handler != NULL){
        timers[v.sival_int].handler();
        if (timers_type[v.sival_int] != 1){
        	timers[v.sival_int].handler = NULL;
	}
    }

    if (timers[v.sival_int].timer != 0 && timers_type[v.sival_int] != 1){
        int re = timer_delete(timers[v.sival_int].timer);
        timers[v.sival_int].timer = 0;
    }
}

int pro_timer_is_running(timer_t timer){
	int running = 0;
        int i = 0;

	if (timer == 0){
		return 0;
        }



        for (i=0; i<PRO_TIMER_MAX; i++){
            if (timers[i].timer == timer){
		running = 1;
          }
        }
     return running;
}

int pro_timer_cancel(timer_t timer){

	if (timer == 0){
		return 1;
        }

	int finded = 0;
        int i = 0;

        for (i=0; i<PRO_TIMER_MAX; i++){
            if (timers[i].timer == timer){
		finded = 1;
            }
        }

        struct itimerspec it_cur;
        if (finded == 1 && timer_gettime(timer, &it_cur) == 0){

            if (it_cur.it_value.tv_sec > 0 || it_cur.it_value.tv_sec > 0){
		struct itimerspec it;
		it.it_interval.tv_sec = 0;
		it.it_interval.tv_nsec = 0;
		it.it_value.tv_sec = 0;
		it.it_value.tv_nsec = 0;
		timer_settime(timer, 0, &it, NULL);            
            }  
        }

        for (i=0; i<PRO_TIMER_MAX; i++){
            if (timers[i].timer == timer){
                timers[i].timer = 0;
                timers[i].handler = NULL;
            }
        }
        return timer_delete(timer);
}

int pro_timer_create(timer_t* ptimer, pro_timer_handler handler, int pro_timer_id){
          
        // cancel the timer, if it is running; 
        pro_timer_cancel(*ptimer);        

        // create timer
	struct sigevent evp;
	memset(&evp, 0, sizeof(struct sigevent));	

	evp.sigev_value.sival_int = pro_timer_id;		
	evp.sigev_notify = SIGEV_THREAD;		
	evp.sigev_notify_function = pro_timer_thread;		

	if (timer_create(CLOCK_REALTIME, &evp, ptimer) == -1)
	{
		perror("fail to timer_create");
		return -1;
	}

	struct itimerspec it;
        if (timers_type[pro_timer_id] == 1){
	    it.it_interval.tv_sec = (timers_span[pro_timer_id] / 1000);
	    it.it_interval.tv_nsec = (timers_span[pro_timer_id]%1000) * 1000000;
        }else{
	    it.it_interval.tv_sec = 0;
	    it.it_interval.tv_nsec = 0;
        }
	it.it_value.tv_sec = (timers_span[pro_timer_id] / 1000);;
	it.it_value.tv_nsec = (timers_span[pro_timer_id]%1000) * 1000000;

	if (timer_settime(*ptimer, 0, &it, NULL) == -1)
	{
		perror("fail to timer_settime");
		return -1;
	}

        timers[pro_timer_id].handler = handler;
        timers[pro_timer_id].timer = *ptimer;

        return 0;
}

