#ifndef _PC_SHIFT_LEFT_
#define _PC_SHIFT_LEFT_

#include <pthread.h>
#include "mascara.h"

extern int pc_var;
extern int cpu_clock;
extern int ir_var;

int pc_shift_left_buffer;

void PC_shift_left()
{
    
    int last_clock = 10;
    
    while(ir_value)
    {
	if (last_clock != cpu_clock)
	{
            pc_shift_left_buffer = (ir_var & 0x03ffffff) << 2;      //0000...00
            pc_temp = (0x10000000 & pc_var);                        //pego somente os 4 bits mais significativos
            pc_shift_left_buffer = (pc_temp | pc_shift_left_buffer)
        }
        else pthread_yield();
    }
    pthread_exit(0);

}

#endif
