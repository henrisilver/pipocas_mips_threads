/*
	File with implementation of Mux_2_MemtoReg routine.
	It receives a control parameter from control unit to choose
	between ALUOut's or MDR's content to be sent to the registers bank.
 */
#ifndef _MUX_2_MEMTOREG_
#define _MUX_2_MEMTOREG_

#include <pthread.h>
#include "mascara.h"

#define ALUOut 0

void mux_2_memtoreg(void *not_used){
    int last_clock = 10;
    mux_memtoreg_buffer.isUpdated = 0;
    
    while(ir){
        if (last_clock != cpu_clock){
            pthread_mutex_lock(&control_sign);
            if (!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            pthread_mutex_unlock(&control_sign);
            
            if((( (separa_MemtoReg & cs.value) >> MemtoReg_POS) & 0x01) == ALUOut)
                mux_memtoreg_buffer.value = aluout;
            else mux_memtoreg_buffer.value = mdr;
            last_clock = cpu_clock;
            
            pthread_mutex_lock(&mux_memtoreg_result);
            mux_memtoreg_buffer.isUpdated = 1;
            pthread_cond_signal(&mux_memtoreg_execution_wait);
            pthread_mutex_unlock(&mux_memtoreg_result);
            
            pthread_barrier_wait(&current_cycle);
            mux_memtoreg_buffer.isUpdated = 0;
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif
