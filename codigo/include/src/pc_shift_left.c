#ifndef _PC_SHIFT_LEFT_
#define _PC_SHIFT_LEFT_

#include <pthread.h>
#include "mascara.h"

extern int pc_value;
extern int cpu_clock;
extern int ir_value;

extern pthread_mutex_t control_sign;

int pc_shift_left_buffer;

void PC_shift_left()
{
    
    int last_clock = 10;
    
    while(ir)
    {
	if (last_clock != cpu_clock)
	{
            pc_shift_left_buffer = (ir_value & 0x03ffffff) << 2;      //0000...00
            pc_temp = (0xf0000000 & pc_value);                        //pego somente os 4 bits mais significativos
            pc_shift_left_buffer = (pc_temp | pc_shift_left_buffer);
        
	    pthread_barrier_wait(&current_cycle);
	}
        else pthread_yield();
    }
    pthread_exit(0);

}

#endif
