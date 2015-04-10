#ifndef _PC_SHIFT_LEFT_
#define _PC_SHIFT_LEFT_

#include <pthread.h>
#include "mascara.h"

void pc_shift_left(void *not_used)
{
    pc_shift_left_buffer.isUpdated = 0;
    int last_clock = 10;
    int pc_temp;
    
    while(ir)
    {
        if (last_clock != cpu_clock)
        {
            pthread_mutex_lock(&control_sign);
            if(cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            pthread_mutex_unlock(&control_sign);
            
            pc_shift_left_buffer.value = (ir & 0x03ffffff) << 2;      //0000...00
            pc_temp = (0xf0000000 & pc);                        //pego somente os 4 bits mais significativos
            pc_shift_left_buffer.value = (pc_temp | pc_shift_left_buffer.value);
            
            last_clock = cpu_clock;
            
            pthread_mutex_lock(&pc_shift_left_result);
            pc_shift_left_buffer.value = 1;
            pthread_cond_signal(&pc_shift_left_execution_wait);
            pthread_mutex_unlock(&pc_shift_left_result);
            
            pthread_barrier_wait(&current_cycle);
            pc_shift_left_buffer.isUpdated = 0;
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif
