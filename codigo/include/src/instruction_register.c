/*      >> Implementation of Instruction's Register routine.
        >> This routine only updates ir global value after all threads have already executed.
        >> Therefore, it updates between barrier that indicates the end of execution and barrier
        indicates the end of updating data.
 */
#ifndef _INSTRUCTION_REGISTER_
#define _INSTRUCTION_REGISTER_

#include <pthread.h>
#include "mascara.h"

#define IRWrite 1

/* This file's global variable  */
void instruction_register(void *not_used){
    int last_clock = 10;
    
    while(ir){
        if (last_clock != cpu_clock){
            pthread_mutex_lock(&control_sign);
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            pthread_mutex_unlock(&control_sign);
            last_clock = cpu_clock;
            
            pthread_barrier_wait(&current_cycle);
            if((( (separa_IRWrite & cs.value) >> IRWrite_POS) & 0x01) == IRWrite){
                ir = mem_data;
            }
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif

