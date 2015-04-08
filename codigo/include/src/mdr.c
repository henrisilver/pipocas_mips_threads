/* 	>> Implementation of Instruction's Register routine.
	>> This routine only updates ir global value after all threads have already executed.
	>> Therefore, it updates between barrier that indicates the end of execution and barrier
	indicates the end of updating data.
*/
#ifndef _MDR_
#define _MDR_

#include <pthread.h>
#include "mascara.h"

extern int cpu_clock;
extern int memory_content_read;
extern c_sign cs;
int mdr;

extern pthread_mutex_t control_sign;
extern pthread_cond_t control_sign_wait;

extern pthread_barrier_t current_cycle;
extern pthread_barrier_t update_registers;

/* This file's global variable  */
void mdr(){
        int last_clock = 10;

        while(ir){
                if (last_clock != cpu_clock){

                        pthread_mutex_lock(&control_sign);
                        if(!cs.isUpdated)
                                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
                        pthread_mutex_unlock(&control_sign);

                        last_clock = cpu_clock;
                        pthread_barrier_wait(&current_cycle);

                        mdr = memory_content_read;

			pthread_barrier_wait(&update_registers);
                }
                else pthread_yield();
        }
        pthread_exit(0);
}
#endif

