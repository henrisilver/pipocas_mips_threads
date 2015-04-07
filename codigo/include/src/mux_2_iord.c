/*
	File with implementation of Mux_2_IorD routine.
	It receives a control parameter from control unit to choose
	between PC's or ALUOut's adresses content to send to the main memory.
*/
#ifndef _MUX_2_IORD_
#define _MUX_2_IORD_

#include <pthread.h>
#include "mascara.h"

#define PC 0

/* Extern global variables  */
extern int cpu_clock;
extern int pc_value;
extern int aluout_value
extern c_sign cs;

extern pthread_mutex_t control_sign;
extern pthread_cond_t control_sign_wait;

/* This file's global variable  */
int mux_IorD_buffer;

void mux_2_IorD(void *not_used){
	int last_clock = 10;

	while(ir){
		if (last_clock != cpu_clock){

			pthread_mutex_lock(&control_sign);
			if(!cs.isUpdated)
				while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
                        pthread_mutex_unlock(&control_sign);

			if(( (separa_IorD & cs.value) >> IorD_POS) & 0x01 == PC){
      				mux_IorD_buffer = pc_value;
			}
			else mux_IorD_buffer = aluout_value;

			last_clock = cpu_clock;
			pthread_barrier_wait(&current_cycle);
		}
		else pthread_yield();
	}
	pthread_exit(0);
}
#endif
