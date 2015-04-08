#ifndef _SIGN_EXTEND_
#define _SIGN_EXTEND_

#include <pthread.h>
#include "mascara.h"

extern int pc;
extern int cpu_clock;
extern int a_value;
int mux_ALUSrcA_buffer;

// extern pthread_cond_t control_sign_wait;
// extern pthread_mutex_t control_sign;
extern pthread_barrier_t current_cycle;

void mux_2_ALUSrcA(void *not_used)
{
    int last_clock = 10;

    while(ir)
    {
		if (last_clock != cpu_clock)
		{
			// pthread_mutex_lock(&control_sign);
			// if (!cs.Updated)
			// 	while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0); //idle loop
	  		//  pthread_mutex_lock(&control_sign);
	
			//seguindo a logica dos outros muxes, tenho o mutex e a uc mandou o sinal verde
			
	
	    	last_clock = cpu_clock;

            pthread_barrier_wait(&current_cycle);
		}

        else pthread_yield();
    }

 pthread_exit(0);
}

#endif
