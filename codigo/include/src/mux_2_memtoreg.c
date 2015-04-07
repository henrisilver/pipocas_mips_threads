/*
	File with implementation of Mux_2_MemtoReg routine.
	It receives a control parameter from control unit to choose
	between ALUOut's or MDR's content to be sent to the registers bank.
*/
#ifndef _MUX_2_MEMTOREG_
#define _MUX_2_MEMTOREG_

#include <pthread.h>
#include "mascara.h"

#define ALUOut 0
extern int aluout;
extern int cpu_clock;
extern int mdr;
extern c_sign cs;
int mux_MemtoReg_buffer;

extern pthread_mutex_t control_sign;
extern pthread_cond_t control_sign_wait;

/*
	>> Não haverá problema de dependências caso o dado usado seja de algum registrador, pois o dado usado já foi computado no ciclo anterior

	>> Agora é necessário garantir que qualquer computação no ciclo atual seja efetivada somente ao final do cilo, ou seja, depois que as
outras unidades funcionais já usaram o dado do clico anterior

	>> Entendo que o sinal de controle necessita ter uma flag com a informação de que o este sinal já foi atualizado no ciclo atual.
Isso serve para tratar deadlocks.

	>> Estou imaginando o seguinte:

	typedef struct c_sign{
		int short value;		// Inteiro que representa o sinal de controle
		int isUpdated;			// 1 para atualizado e 0 caso contrário
	}c_sign;

*/


/*   */
void mux_2_MemtoReg(void *not_used){
	int last_clock = 10;

	while(ir){
		if (last_clock != cpu_clock){
			pthread_mutex_lock(&control_sign);

			if (!cs.isUpdated)
				while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);

			pthread_mutex_unlock(&control_sign);

			if(( (separa_MemtoReg & cs.value) >> MemtoReg_POS) & 0x01 == ALUOut)
      				mux_MemtoReg_buffer = aluout;
			else mux_MemtoReg_buffer = mdr;

			last_clock = cpu_clock;

			pthread_barrier_wait(&current_cycle);
		}
		else pthread_yield();
	}
	pthread_exit(0);
}
#endif
