/* implementa o program counter 
 * Espera o sinal de controle para execucao
 * Escreve quando or_result.value for verdadeiro
 * pc depende da variavel de controle e dos dados vindos do mux_3_pcsource */

#ifndef _PC_
#define _PC_

#include <pthread.h>
#include "mascara.h"

void program_counter(void *not_used){

        pthread_barrier_wait(&threads_creation);

	while(1){

            	pthread_mutex_lock(&control_sign);
				if(!cs.isUpdated){
						while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);
				}

				pthread_mutex_lock(&or_result_mutex);
            	if(!or_result.isUpdated)
                	while(pthread_cond_wait(&or_result_wait, &or_result_mutex) != 0);
				pthread_mutex_unlock(&or_result_mutex);

            	pthread_mutex_lock(&mux_pcsource_result);
            	if(!mux_pcsource_buffer.isUpdated)
                	while(pthread_cond_wait(&mux_pcsource_execution_wait, &mux_pcsource_result) != 0);
            	pthread_mutex_unlock(&mux_pcsource_result);


            	pthread_barrier_wait(&current_cycle);
				if(or_result.value)
	            	pc = mux_pcsource_buffer.value;
            	pthread_barrier_wait(&update_registers);
	}
}
#endif
