#ifndef _MUX_2_REGDST_
#define _MUX_2_REGDST_

#include <pthread.h>
#include "mascara.h"

#define RT 0

void mux_2_regdst(void *not_used){
    int last_clock = 10;
    mux_regdst_buffer.isUpdated = 0;
    
    while(ir){
        if (last_clock != cpu_clock){
            pthread_mutex_lock(&control_sign);
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            pthread_mutex_unlock(&control_sign);
            
            if((( (separa_RegDst & cs.value) >> RegDst_POS) & 0x01) == RT){
                mux_iord_buffer.value = ((ir & separa_rt) >> 16) & 0x0000001f;
            }
            else mux_regdst_buffer.value = ((ir & separa_rd) >> 11) & 0x0000001f;
            last_clock = cpu_clock;
            
            pthread_mutex_lock(&mux_regdst_result);
            mux_regdst_buffer.isUpdated = 1;
            pthread_cond_broadcast(&mux_regdst_execution_wait);
            pthread_mutex_unlock(&mux_regdst_result);
            
            pthread_barrier_wait(&current_cycle);
            mux_regdst_buffer.isUpdated = 0;
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif
