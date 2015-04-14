/*      >> Implementation of Instruction's Register routine.
        >> This routine only updates ir global value after all threads have already executed.
        >> Therefore, it updates between barrier that indicates the end of execution and barrier
        indicates the end of updating data.
 */
#ifndef _ALUOUT_
#define _ALUOUT_

#include <pthread.h>
#include "mascara.h"

/* This file's global variable  */
void aluout_register(void *not_used){
    int last_clock = 10;
    
    while(ir){
        if (last_clock != cpu_clock){
            
            // pthread_mutex_lock(&control_sign);
            // if(!cs.isUpdated)
            //         while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            // pthread_mutex_unlock(&control_sign);
            
            last_clock = cpu_clock;
            pthread_barrier_wait(&current_cycle);
            
            aluout = alu_result.value;
            
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif
