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

/* implementa a operacao de extensao de sinal
 * usa como entrada bits de IR */

#ifndef _SIGN_EXTEND_
#define _SIGN_EXTEND_

#include <pthread.h>
#include "mascara.h"

void sign_extend(void *not_used){

        pthread_barrier_wait(&threads_creation);

	int i;
	short int imediato;
	int sign_extend_temp;

	while(1)
	{
            	pthread_mutex_lock(&control_sign);
            	if (!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){//se for uma instrucao invalida
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);//saio da thread
				}

            	imediato = ir & separa_imediato;
            	//seguindo a logica dos outros muxes, tenho o mutex e a uc mandou o sinal verde
            	sign_extend_temp = imediato & 0x0000ffff;       // Copia do conteudo de MDR (16 bits) para *MDRnew
            	imediato = imediato & 0x00008000;      // Eh feita uma mascara para manter apenas o MSB de MDR
            
            	// O MSB de MDR eh copiado para todos os bits seguintes de *MDRnew (extensao de sinal)
            	for (i = 1; i < 17; i++)
                	sign_extend_temp = sign_extend_temp| (imediato << i);

            	se.value = sign_extend_temp;
            
            	pthread_mutex_lock(&sign_extend_mutex);
            	se.isUpdated = 1;
            	pthread_cond_broadcast(&sign_extend_cond);
            	pthread_mutex_unlock(&sign_extend_mutex);
            
            	pthread_barrier_wait(&current_cycle);
            	se.isUpdated = 0;
            	pthread_barrier_wait(&update_registers);
    	}
}
#endif
