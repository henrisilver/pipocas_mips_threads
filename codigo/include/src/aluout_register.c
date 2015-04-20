/* thread de aluout. Simplesmente guarda o valor de saida da alu a cada ciclo */

#ifndef _ALUOUT_
#define _ALUOUT_

#include <pthread.h>
#include "mascara.h"

void aluout_register(void *not_used){

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
          

            	pthread_barrier_wait(&current_cycle);//espera o ciclo correto
            	aluout = alu_result.value;//pega valor da ula
            	pthread_barrier_wait(&update_registers);//espera atualizacao dos registradores no outro ciclo de instrucoes
    	}
}
#endif

