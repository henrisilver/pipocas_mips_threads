/* 	>> Implementation of Instruction's Register routine.
	>> This routine only updates ir global value after all threads have already executed.
	>> Therefore, it updates between barrier that indicates the end of execution and barrier
	indicates the end of updating data.
*/
#ifndef _B_
#define _B_

#include <pthread.h>
#include "mascara.h"

extern int cpu_clock;
extern int read_data_2;
int b_value;

extern pthread_barrier_t current_cycle;
extern pthread_barrier_t update_registers;

/* This file's global variable  */
void b(){
        int last_clock = 10;

        while(ir){
                if (last_clock != cpu_clock){
                        last_clock = cpu_clock;

                        pthread_barrier_wait(&current_cycle);
                        b_value = read_data_2;
			pthread_barrier_wait(&update_registers);
                }
                else pthread_yield();
        }
        pthread_exit(0);
}
#endif
