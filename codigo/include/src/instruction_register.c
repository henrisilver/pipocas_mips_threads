/* SSC0640 - Sistemas Operacionais I
* Trabalho 1 - CPU MIPS com pthreads em C
* Data: 22/04/2015
***************************************************************
* Integrantes do Grupo:
* Guilherme Nishina Fortes 			- No. USP 7245552
* Henrique de Almeida Machado da Silveira 	- No. USP 7961089
* Marcello de Paula Ferreira Costa 		- No. USP 7960690
* Sergio Yudi Takeda 				- No. USP 7572996
***************************************************************
* GRUPO 13
*/

/* Thread que implementa instruction register
 * Ela espera o ciclo de execucao correto e verifica se ha escrita em IR
 * caso haja, IR eh sobreescrito e a thread espera novamente seu periodo de execucao */

#ifndef _INSTRUCTION_REGISTER_
#define _INSTRUCTION_REGISTER_

#include <pthread.h>
#include "mascara.h"

#define IRWrite 1

void instruction_register(void *not_used){

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
            

            	pthread_barrier_wait(&current_cycle);
            	if((( (separa_IRWrite & cs.value) >> IRWrite_POS) & 0x01) == IRWrite){//verifica se ha escrita no registrador de IR
                	ir = mem_data;//IR recebe o que esta em mem data
            	}
            	pthread_barrier_wait(&update_registers);
    	}
}
#endif

