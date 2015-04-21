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

/* thread para as portas logicas "E" e "OU"
 * Recebe entradas da UC e da ULA
 * faz primeiro uma operacao logica de "E" com Zero e PCWriteCond, guardando o resultado em variavel local
 * entao faz uma operacao logica de "OU" com o resultado obtido e PCWrite, guardado em variavel global or_result.value
 * or_result.isUpdated eh entao mudada para 1 */

#ifndef _AND_OR_
#define _AND_OR_

#include <pthread.h>
#include "mascara.h"

void and_or(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	int and_result;//variaveis locais para auxiliar na operacao e na atribuicao correta do valor a variavel global relacionada a esta thread
    	int or_result_temp;
    
    	while(1){
			
			pthread_mutex_lock(&control_sign);
			if(!cs.isUpdated){
				while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);//idle loop. espera o cs atualizar
			}
			pthread_mutex_unlock(&control_sign);//libera o mutex pois cond wait disputa por ele na volta

			if(cs.invalidInstruction){
				pthread_barrier_wait(&update_registers);
				pthread_exit(0);
			}
            
			pthread_mutex_lock(&alu_zero_mutex);
			if(!alu_zero.isUpdated)
				while(pthread_cond_wait(&alu_zero_wait, &alu_zero_mutex) != 0);//idle loop. espera a alu realizar a operacao
			pthread_mutex_unlock(&alu_zero_mutex);
		
			if(( (separa_PCWriteCond & cs.value) >> 9) & alu_zero.value)//separa o bit importante e faz a operacao
				and_result = 1;
			else
				and_result = 0;

			if( ((separa_PCWrite & cs.value) >> 10) | and_result ) {//usa a mascara para pegar o valor de comando apropriado e
				or_result_temp = 1;									//faz shift left por dez para transformar o numero em 1 ou 0
			}
		
			else {
				or_result_temp = 0;
			}            
			or_result.value = or_result_temp;

			pthread_mutex_lock(&or_result_mutex);
			or_result.isUpdated = 1;
			pthread_cond_signal(&or_result_wait);
			pthread_mutex_unlock(&or_result_mutex);
		
			pthread_barrier_wait(&current_cycle);
			or_result.isUpdated = 0;
			pthread_barrier_wait(&update_registers);
	
		}
}

#endif
