#ifndef _PC_SHIFT_LEFT_
#define _PC_SHIFT_LEFT_

#include <pthread.h>
#include "mascara.h"

#define ALUOut 0

extern int pc_var;
extern int cpu_clock;
extern int ir_var;

int PC_shift_left_buffer;

void PC_shift_left()
{
    while(valid_instruction)
    {
		if (last_clock != cpu_clock)
		{
            PC_shift_left_buffer = (ir_var & 0x03ffffff) << 2;      //0000...00
            pc_temp = (0x10000000 & pc_var);                        //pego somente os 4 bits mais significativos
            PC_shift_left_buffer = (pc_temp | PC_shift_left_buffer)
        }
    }
}

#endif
