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

/* multiplexador de duas entradas com sinal de controle denominado de ALUSrcA que seleciona qual dado entre o registrador A ou
 * o valor de pc que entrara na ula. Usualmente os outros multiplexadores de 2, 3 e 4 entradas serao denominados de maneira similar
 * com mux_"numentradas"_"sinalcontrole" sendo a regra. */

#ifndef _MUX_2_ALUSRCA_
#define _MUX_2_ALUSRCA_

#include <pthread.h>
#include "mascara.h"

#define PC 0

void mux_2_alusrca(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	while(1)
    	{
            	pthread_mutex_lock(&control_sign);
            	if (!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);
				}
            
            	if((( (separa_ALUSrcA & cs.value) >> ALUSrcA_POS) & 0x01) == PC)
                	mux_alusrca_buffer.value = pc;
            	else mux_alusrca_buffer.value = a_value;
            
            	pthread_mutex_lock(&mux_alusrca_result);
            	mux_alusrca_buffer.isUpdated = 1;
            	pthread_cond_signal(&mux_alusrca_execution_wait);
            	pthread_mutex_unlock(&mux_alusrca_result);


            	pthread_barrier_wait(&current_cycle);
            	mux_alusrca_buffer.isUpdated = 0;
            	pthread_barrier_wait(&update_registers);
    	}
}

#endif
