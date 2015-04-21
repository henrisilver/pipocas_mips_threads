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

/* implementa o multiplexador de duas entradas com sinal de controle MemtoReg
 * Seleciona entre mdr e aluout 
 * A operacao e funcao de todos os multiplexadores sao similares */

#ifndef _MUX_2_MEMTOREG_
#define _MUX_2_MEMTOREG_

#include <pthread.h>
#include "mascara.h"

#define ALUOut 0

void mux_2_memtoreg(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	while(1){
            	pthread_mutex_lock(&control_sign);
            	if (!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
				}
				pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
						pthread_exit(0);
				}
            
            	if((( (separa_MemtoReg & cs.value) >> MemtoReg_POS) & 0x01) == ALUOut)//verifica o sinal de controle
                	mux_memtoreg_buffer.value = aluout;//escolhe qual entrada deve ser considerada
            	else mux_memtoreg_buffer.value = mdr;
            
            	pthread_mutex_lock(&mux_memtoreg_result);
            	mux_memtoreg_buffer.isUpdated = 1;//muda flag de atualizacao para verdadeira
            	pthread_cond_signal(&mux_memtoreg_execution_wait);//emite um sinal para desbloquear as threads dependentes
            	pthread_mutex_unlock(&mux_memtoreg_result);


            	pthread_barrier_wait(&current_cycle);
            	mux_memtoreg_buffer.isUpdated = 0;
            	pthread_barrier_wait(&update_registers);
    	}
}
#endif
