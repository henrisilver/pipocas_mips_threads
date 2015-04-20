/* implementa memory data register. Apenas escreve o dado advindo da memoria a cada ciclo apropriado */

#ifndef _MDR_
#define _MDR_

#include <pthread.h>
#include "mascara.h"

void memory_data_register(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	while(1){
            	pthread_mutex_lock(&control_sign);
            	if(!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){//aumenta robustez
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);
				}


            	pthread_barrier_wait(&current_cycle);            
            	mdr = mem_data;//atualiza a variavel global de mdr
				pthread_barrier_wait(&update_registers);
    	}
}
#endif

