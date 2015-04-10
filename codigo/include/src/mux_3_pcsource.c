/*
	File with implementation of Mux_2_IorD routine.
	It receives a control parameter from control unit to choose
	between PC's or ALUOut's adresses content to send to the main memory.
 */
#ifndef _MUX_3_PCSOURCE_
#define _MUX_3_PCSOURCE_

#include <pthread.h>
#include "mascara.h"

#define ALU_RESULT 0
#define ALUOUT 1

void mux_3_pcsource(void *not_used){
    int last_clock = 10;
    
    
    mux_pcsource_buffer.isUpdated = 0;
    
    while(ir){
        if (last_clock != cpu_clock){
            
            pthread_mutex_lock(&control_sign);
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            pthread_mutex_unlock(&control_sign);
            char control = (((separa_PCSource0 | separa_PCSource1) & sc) >> PCSource0_POS) & 0x03;
            
            if(control == ALU_RESULT){
                pthread_mutex_lock(&alu_result_mutex);
                if(!alu_result.isUpdated)
                    while(pthread_cond_wait(&alu_result_wait,&alu_result_mutex) != 0);
                pthread_mutex_unlock(&alu_result_mutex);
                mux_pcsource_buffer.value = alu_result.value;
            }
            else if (control == ALUOUT){
                mux_pcsource_buffer.value = aluout;
            }
            else {
                pthread_mutex_lock(&pc_shift_left_result);
                if(!pc_shift_left_buffer.isUpdated)
                    while(pthread_cond_wait(&pc_shift_left_execution_wait,&pc_shift_left_result) != 0);
                pthread_mutex_unlock(&pc_shift_left_result);
            }
            last_clock = cpu_clock;
            
            pthread_mutex_lock(&mux_pcsource_result);
            mux_pcsource_buffer.isUpdated = 1;
            pthread_cond_signal(&mux_pcsource_execution_wait);
            pthread_mutex_unlock(&mux_pcsource_result);
            
            pthread_barrier_wait(&current_cycle);
            mux_pcsource_buffer.isUpdated = 0;
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif
