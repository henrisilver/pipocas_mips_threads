/*
    File with implementation of Mux_2_MemtoReg routine.
    It receives a control parameter from control unit to choose
    between ALUOut's or MDR's content to be sent to the registers bank.
*/
#ifndef _MAIN_MEMORY_
#define _MAIN_MEMORY_

#include <pthread.h>
#include "mascara.h"

extern int *memoria;
extern int cpu_clock;
extern int mdr;
extern c_sign cs;
extern link mux_iord_buffer

extern pthread_mutex_t control_sign;
extern pthread_mutex_t mux_iord_result;
extern pthread_cond_t control_sign_wait;
extern pthread_cond_t mux_iord_execution_wait;


void main_memory(void *not_used){
    int last_clock = 10;

    while(ir){
        if (last_clock != cpu_clock){

            pthread_mutex_lock(&control_sign);
            if (!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);
            pthread_mutex_unlock(&control_sign);

            pthread_mutex_lock(&mux_iord_result);
            if (!mux_iord_buffer.isUpdated)
                while(pthread_cond_wait(&mux_iord_execution_wait, &mux_iord_result) != 0);
            pthread_mutex_unlock(&mux_iord_result);

            if ((separa_MemRead & cs.value) == ativa_MemRead)  
                mem_data = memoria[mux_iord_buffer.value/4];
  
            if ((cs.value & separa_MemWrite) == ativa_MemWrite)
                memoria[mux_iord_buffer.value/4] = b_value;

            last_clock = cpu_clock;

            pthread_barrier_wait(&current_cycle);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif
