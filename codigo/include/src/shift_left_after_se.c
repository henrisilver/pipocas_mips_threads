#ifndef _SHIFT_LEFT_AFTER_SE_
#define _SHIFT_LEFT_AFTER_SE_

#include <pthread.h>
#include "mascara.h"

void shift_left_after_se(void *not_used)
{
    shift_left.isUpdated = 0;
    int last_clock = 10;
    
    while(ir)
    {
        if (last_clock != cpu_clock)
        {
            pthread_mutex_lock(&control_sign);
            if (!cs.isUpdated)
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
    pthread_exit(0);
}

#endif
