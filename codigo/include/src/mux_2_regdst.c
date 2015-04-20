/* multiplexador de duas entradas.
 * Seleciona entre os bits 20-16 ou 15-11 de IR */

#ifndef _MUX_2_REGDST_
#define _MUX_2_REGDST_

#include <pthread.h>
#include "mascara.h"

#define RT 0

void mux_2_regdst(void *not_used){

        pthread_barrier_wait(&threads_creation);

   	 while(1){
            	pthread_mutex_lock(&control_sign);
				if(!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
						pthread_exit(0);
				}

            	if((( (separa_RegDst & cs.value) >> RegDst_POS) & 0x01) == RT){//verifica qual entrada devera ser ativada
                	mux_regdst_buffer.value = ((ir & separa_rt) >> 16) & 0x0000001f;
            	}
            	else mux_regdst_buffer.value = ((ir & separa_rd) >> 11) & 0x0000001f;

            	pthread_mutex_lock(&mux_regdst_result);
            	mux_regdst_buffer.isUpdated = 1;
            	pthread_cond_signal(&mux_regdst_execution_wait);
            	pthread_mutex_unlock(&mux_regdst_result);


            	pthread_barrier_wait(&current_cycle);
            	mux_regdst_buffer.isUpdated = 0;
            	pthread_barrier_wait(&update_registers);
    	}
}
#endif
