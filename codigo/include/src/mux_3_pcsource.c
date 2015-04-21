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

/* multiplexador que seleciona entre os valores de alu, aluout e o valor de ir multiplicado por 4 e concatenado com os 4 bits mais
 * significativos de pc */

#ifndef _MUX_3_PCSOURCE_
#define _MUX_3_PCSOURCE_

#include <pthread.h>
#include "mascara.h"

#define ALU_RESULT 0
#define ALUOUT 1

void mux_3_pcsource(void *not_used){

        pthread_barrier_wait(&threads_creation);
   
    	while(1){
            	pthread_mutex_lock(&control_sign);
            	if(!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){ 	//aumenta robustez contra entradas invalidas
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);
				}

			char control = (((separa_PCSource0 | separa_PCSource1) & cs.value) >> PCSource0_POS) & 0x03;//pega comando
		
			if(control == ALU_RESULT){ 							//pega resultado diretamente da alu
				pthread_mutex_lock(&alu_result_mutex);			//trava o mutex relacionado ao resultado da alu
				if(!alu_result.isUpdated)						//se nao tiver dado valido...
						while(pthread_cond_wait(&alu_result_wait,&alu_result_mutex) != 0);//...espera a ula executar
				pthread_mutex_unlock(&alu_result_mutex);		//quando cond wait volta, o mutex eh tomado, eh preciso libera-lo
				mux_pcsource_buffer.value = alu_result.value;	//seleciona o resultado da ula e o guarda para transferi-lo
			}
			
			else{
				if (control == ALUOUT){ 					//se o dado advir de aluout
					mux_pcsource_buffer.value = aluout;		//pega o dado. A validade do dado eh garantido pela barreira de current cycle 
				}
			
				else {
					pthread_mutex_lock(&pc_shift_left_result);
					if(!pc_shift_left_buffer.isUpdated)
							while(pthread_cond_wait(&pc_shift_left_execution_wait,&pc_shift_left_result) != 0);
					pthread_mutex_unlock(&pc_shift_left_result);
					mux_pcsource_buffer.value = pc_shift_left_buffer.value;
					}
			}


			pthread_mutex_lock(&mux_pcsource_result);
			mux_pcsource_buffer.isUpdated = 1;
			pthread_cond_signal(&mux_pcsource_execution_wait);
			pthread_mutex_unlock(&mux_pcsource_result);
		
			pthread_barrier_wait(&current_cycle);
			mux_pcsource_buffer.isUpdated = 0;
			pthread_barrier_wait(&update_registers);
    	}
}
#endif
