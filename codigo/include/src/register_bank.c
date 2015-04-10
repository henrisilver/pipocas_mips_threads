/*      >> Implementation of Instruction's Register routine.
        >> This routine only updates ir global value after all threads have already executed.
        >> Therefore, it updates between barrier that indicates the end of execution and barrier
        indicates the end of updating data.
 */
#ifndef _REGISTER_BANK_
#define _REGISTER_BANK_

#include <pthread.h>
#include "mascara.h"

#define IRWrite 1

void register_bank(void *not_used){
    int last_clock = 10;
    int read_register_1, read_register_2, write_register, write_data;
    
    while(ir){
        if (last_clock != cpu_clock){
            pthread_mutex_lock(&control_sign);
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            pthread_mutex_unlock(&control_sign);
            
            pthread_mutex_lock(&mux_regdst_result);
            if(!mux_regdst_buffer.isUpdated)
                while(pthread_cond_wait(&mux_regdst_execution_wait, &mux_regdst_result) != 0);
            pthread_mutex_unlock(&mux_regdst_result);
            
            pthread_mutex_lock(&mux_memtoreg_result);
            if(!mux_regdst_buffer.isUpdated)
                while(pthread_cond_wait(&mux_memtoreg_execution_wait, &mux_memtoreg_result) != 0);
            pthread_mutex_unlock(&mux_memtoreg_result);
            
            
            last_clock = cpu_clock;
            
            
            read_register_1 = ((separa_rs & ir) >> 21) & 0x0000003f;
            read_register_2 = ((separa_rt & ir) >> 16) & 0x0000003f;
            read_data_1 = reg[read_register_1];
            read_data_2 = reg[read_register_2];
            write_register = mux_regdst_buffer.value;
            write_data = mux_memtoreg_buffer.value;
            
            pthread_barrier_wait(&current_cycle);
            
            if ((sc & separa_RegWrite) == ativa_RegWrite)
                reg[write_register] = write_data;
            
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif

