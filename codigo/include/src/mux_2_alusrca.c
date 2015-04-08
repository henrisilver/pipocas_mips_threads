#ifndef _MUX_2_ALUSRCA_
#define _MUX_2_ALUSRCA_

#include <pthread.h>
#include "mascara.h"

#define PC 0

extern int cpu_clock;
extern int a_value;
extern int pc;
link mux_alusrca_buffer;

extern pthread_cond_t control_sign_wait;
extern pthread_mutex_t control_sign;

void mux_2_alusrca(void *not_used)
{
    	int last_clock = 10;
	mux_alusrca_buffer.isUpdated = 0;

    	while(ir)
    	{
		if (last_clock != cpu_clock)
		{
			pthread_mutex_lock(&control_sign);
			if (!cs.Updated)
				while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);
	    		pthread_mutex_lock(&control_sign);
	
			if(( (separa_ALUSrcA & cs.value) >> ALUSrcA_POS) & 0x01 == PC)
				 mux_alusrca_buffer.value = pc;
			else mux_alusrca_buffer.value = a_value;
	    		last_clock = cpu_clock;

			pthread_mutex_lock(&mux_alusrca_result);
			mux_alusrca_buffer.isUpdated = 1;
			pthread_cond_signal(&mux_alusrca_execution_wait);
			pthread_mutex_unlock(&mux_arca_result);

            		pthread_barrier_wait(&current_cycle);
			mux_alusrca_buffer.isUpdated = 0;
			pthread_barrier_wait(&update_registers);
		}
        	else pthread_yield();
    	}
 	pthread_exit(0);
}

#endif
