#ifndef _MUX_2_IORD_
#define _MUX_2_IORD_

#include <pthread.h>
#include "mascara.h"

#define RT 0

/* Extern global variables  */
extern int cpu_clock;
extern int aluout;
extern int pc;
extern c_sign cs;

extern pthread_mutex_t control_sign;
extern pthread_cond_t control_sign_wait;

extern pthread_mutex_t mux_regdst_result;
extern pthread_cond_t mux_regdst_execution_wait;

extern pthread_barrier_t current_cycle;
extern pthread_barrier_t update_registers;

link mux_regdst_buffer;

void mux_2_iord(void *not_used){
	int last_clock = 10;

	while(ir){
		if (last_clock != cpu_clock){
			pthread_mutex_lock(&control_sign);
			if(!cs.isUpdated)
				while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
                        pthread_mutex_unlock(&control_sign);

			if(( (separa_RegDst & cs.value) >> RegDst_POS) & 0x01 == RT){
      				mux_iord_buffer.value = ((IR & separa_rt) >> 16) & 0x0000001f;
			}
			else mux_regdst_buffer.value = ((IR & separa_rd) >> 11) & 0x0000001f;
			last_clock = cpu_clock;

			pthread_mutex_lock(&mux_regdst_result);
			mux_regdst_buffer.isUpdated = 1;
			pthread_cond_broadcast(&mux_regdst_execution_wait);
			pthread_mutex_unlock(&mux_regdst_result);

			pthread_barrier_wait(&current_cycle);
			mux_regdst_buffer.isUpdated = 0;
			pthread_barrier_wait(&update_registers);
		}
		else pthread_yield();
	}
	pthread_exit(0);
}
#endif
