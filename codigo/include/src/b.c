/* Implementa a rotina do registrador B
 * Simplesmente espera o dado a ser consumido estar atualizado
 * e o ciclo correto de execucao para pegar o valor de um registrador 
 * esta thread se comporta de maneira similar a thread de A */

#ifndef _B_
#define _B_

#include <pthread.h>
#include "mascara.h"

void b(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	while(1){//loop de execucao
            	pthread_mutex_lock(&control_sign);
            	if(!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);
				}
            
            	pthread_barrier_wait(&current_cycle);
            	b_value = read_data_2;//pega valor lido na memoria
            	pthread_barrier_wait(&update_registers);
    	}
}
#endif

