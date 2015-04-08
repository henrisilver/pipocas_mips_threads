#ifndef _SIGN_EXTEND_
#define _SIGN_EXTEND_

#include <pthread.h>
#include "mascara.h"

extern int cpu_clock;
extern link se;



link shift_left;

// extern pthread_cond_t control_sign_wait;
// extern pthread_mutex_t control_sign;
extern pthread_barrier_t current_cycle;
extern pthread_mutex_t sign_extend_mutex;
extern pthread_cond_t sign_extend_cond;

pthread_mutex_t shift_left_mutex;
pthread_cond_t shift_left_cond;

void shift_left_after_se(void *not_used)
{
	shift_left.isUpdated = 0;
    int last_clock = 10;
    short int imediato;
    int sign_extend_temp
    pthread_mutex_init(&shift_left_mutex, NULL);
    pthread_cond_init(&shift_left_cond, NULL);

    while(ir)
    {
		if (last_clock != cpu_clock)
		{
			pthread_mutex_lock(&control_sign);
			if (!cs.Updated)
				while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0); //idle loop
	  		pthread_mutex_unlock(&control_sign);

			pthread_mutex_lock(&sign_extend_mutex);
			if (!se.isUpdated)
				while(pthread_cond_wait(&sign_extend_cond, &sign_extend_mutex) != 0); //idle loop
	  		pthread_mutex_unlock(&sign_extend_mutex);
	
	    	last_clock = cpu_clock;
	    	pthread_mutex_lock(&shift_left_mutex);
	    	shift_left.value = se.value << 2;
	    	shift_left.isUpdated = 1;
	    	pthread_cond_signal(&shift_left_cond);
	    	pthread_mutex_unlock(&shift_left_mutex);
	    	shift_left.isUpdated = 0;
            pthread_barrier_wait(&current_cycle);
		}

        else pthread_yield();
    }

 pthread_cond_destroy(&shift_left_cond);
 pthread_mutex_destroy(&shift_left_mutex);
 pthread_exit(0);
}

#endif
