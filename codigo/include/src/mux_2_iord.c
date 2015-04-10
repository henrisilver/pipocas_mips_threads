/*
	File with implementation of Mux_2_IorD routine.
	It receives a control parameter from control unit to choose
	between PC's or ALUOut's adresses content to send to the main memory.
 */
#ifndef _MUX_2_IORD_
#define _MUX_2_IORD_

#include <pthread.h>
#include "mascara.h"

#define PC 0

void mux_2_iord(void *not_used){
    int last_clock = 10;
    mux_iord_buffer.isUpdated = 0;
    
    while(ir){
        if (last_clock != cpu_clock){
            pthread_mutex_lock(&control_sign);
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            pthread_mutex_unlock(&control_sign);
            
            if((( (separa_IorD & cs.value) >> IorD_POS) & 0x01) == PC){
                mux_iord_buffer.value = pc;
            }
            else mux_iord_buffer.value = aluout;
            last_clock = cpu_clock;
            
            pthread_mutex_lock(&mux_iord_result);
            mux_iord_buffer.isUpdated = 1;
            pthread_cond_broadcast(&mux_iord_execution_wait);
            pthread_mutex_unlock(&mux_iord_result);
            
            pthread_barrier_wait(&current_cycle);
            mux_iord_buffer.isUpdated = 0;
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif
