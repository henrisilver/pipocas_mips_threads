/* thread que implementa a memoria 
 * a flag de modificado (isUpdates) eh inicializada como zero na main
 * a memoria eh global */

#ifndef _MAIN_MEMORY_
#define _MAIN_MEMORY_

#include <pthread.h>
#include "mascara.h"

void main_memory(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	while(1){
            	pthread_mutex_lock(&control_sign);
            	if (!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);
				}

            	pthread_mutex_lock(&mux_iord_result);
            	if (!mux_iord_buffer.isUpdated)
                	while(pthread_cond_wait(&mux_iord_execution_wait, &mux_iord_result) != 0);
            	pthread_mutex_unlock(&mux_iord_result);

            	if ((separa_MemRead & cs.value) == ativa_MemRead)//verifica se comando para leitura eh verdadeiro
                	mem_data = memoria[mux_iord_buffer.value/4].value;//a divsao por 4 converte de enderecamento a byte em palavra
            
            	if ((cs.value & separa_MemWrite) == ativa_MemWrite){//verifica se comando para escrita eh verdadeiro
                	memoria[mux_iord_buffer.value/4].value = b_value;//atualiza valor na memoria
                	memoria[mux_iord_buffer.value/4].isUpdated = 1;//atualiza flag indicando pronto para consumo
            	}


            	pthread_barrier_wait(&current_cycle);
				pthread_barrier_wait(&update_registers);
    	}
}
#endif
