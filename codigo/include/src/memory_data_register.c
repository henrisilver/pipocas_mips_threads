/* SSC0640 - Sistemas Operacionais I
* Trabalho 1 - CPU MIPS com pthreads em C
* Data: 22/04/2015
***************************************************************
* Integrantes do Grupo:
* Guilherme Nishina Fortes             - No. USP 7245552
* Henrique de Almeida Machado da Silveira     - No. USP 7961089
* Marcello de Paula Ferreira Costa         - No. USP 7960690
* Sergio Yudi Takeda                 - No. USP 7572996
***************************************************************
* GRUPO 13
*/

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

