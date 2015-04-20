#ifndef _A_
#define _A_

#include <pthread.h>
#include "mascara.h"

/* >> A função abaixo implementa a rotina do registrador A, presente no caminho de dados da CPU MIPS Multicilo */
void a(void *not_used){

	pthread_barrier_wait(&threads_creation);
	/* >> A barreira threads_creation serve unicamente para aguardar até que todas as threads sejam efetivamente
criadas e comecem a executar */

	while(1){
            	pthread_mutex_lock(&control_sign);
		if(!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
		}
		pthread_mutex_unlock(&control_sign);
		/* >> O bloco acima implementa o comportamento de espera pelo sinal de controle.
		   >> É sabido que quando a unidade de controle tiver o sinal pronto, ela será responsável por setar a variável
		cs.isUpdated com o valor 1, e com essa lógica, a thread "a" só espera pela variável de condição se necessário.
		*/

		if(cs.invalidInstruction){
			pthread_barrier_wait(&update_registers);
		    pthread_exit(0);
		}
		/* >> A unidade de controle é responsável por verificar o fim da execução de um programa e também por identificar
		quaisquer operações não executáveis pelo simulador. Uma vez que uma das duas condições acima for alcançada, o
		valor de cs.invalidInstruction será 1 e, portanto, a thread cessará, por conta própria, sua execução.
		   >> a chamada de pthread_barrier_wait serve apenas para sincronizar com a main. No momento em que cada thread
		chega nesse ponto de execução, a main já chamou pthread_barrier_wait para mesma barreira.
		*/

		pthread_barrier_wait(&current_cycle); 		/* Barreira para sincronizar com o fim da execução do ciclo atual */
		a_value = read_data_1;				/* Atualiza o registrador A */
		pthread_barrier_wait(&update_registers); 	/* Barreira para simular o comportamento de atualização de registradores
								somente ao final do ciclo, ou seja, no tique de clock */
	}
}
#endif

