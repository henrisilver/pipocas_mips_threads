/*
	File with implementation of Mux_2_ALUSrcA routine.
	It receives a control parameter from control unit to choose
	between ALUOut's or MDR's content to be sent to the registers bank.
*/
#ifndef _MUX_2_ALUSRCA_
#define _MUX_2_ALUSRCA_

#include <pthread.h>
#include "mascara.h"

#define ALUOut 0
extern int pc;
extern int aluout;
extern int cpu_clock;
extern short int sc;
extern int mdr;

pthread_mutex_t pc_a_mux;
pthread_cond_t cs_ready;
pthread_cond_t main_memory_read;

typedef struct link{
	int value;
	int updated;
}link;

extern link pc_mux;		// Input buffer
extern link a_mux;
link mux_alu;			//Output buffer

/*
	Devido ao não-determinismo no quesito de execução, não consigo garantir que a variável de condição seja suficiente para garantir
a semântica de execução. Ou seja, tome por exemplo uma distribuição onde a memória será executada antes do multiplexador, espera-se que
isso aconteça, certo? Certo. Mas e se acontecer o contrário? Bom, para isso colocamos o pthread_cond_wait. Porém há um problema quando o
esperado acontece, a thread da memória, que executou primeiro, irá executar pthread_cond_signal e já que a thread deste mux não executou
ainda, o sinal será perdido, resultando em um deadlock quando o pthread_cond_wait for executado.

	Uma solução para esse problema, que creio ser boa e funcional, é a apresentada neste código. Foi criada uma estrutura que contém,
além de uma variável para dado, uma variável flag que só será 1 caso a memória já tenha executado, e isso é garantido pelo mutex criado
mem_ir_aux.

	Opiniões, sugestões, soluções? por favor!

	Discussão de um futuro próximo: esse mux além de depender de algumas threads, será dependência de outras, então ainda deverão
ser inseridos alguns comandos pthread_cond_signal().

*/

void mux_2_ALUSrcA(){
	int last_clock = 10;

	while(valid_instruction){
		if (last_clock != cpu_clock){
			pthread_mutex_lock(&pc_a_mux);

			while(pthread_cond_wait(&cs_ready,&pc_a_mux) != 0);
			if(( (separa_ALUSrcA & sc) >> MemtoReg_POS) & 0x01 == PC){
      				mem_mux.value = pc;
			}
			else{
				if (l.updated == 0)
					while (pthread_cond_wait(&main_memory_read,&pc_a_mux) != 0);
				mux_alu.value = a;
			}

			pthread_mutex_lock(&mux_alu);
			mux_alu.updated = 1;			// Sinaliza que o buffer esta atualizado
			pthread_cond_signal(&mux_selected);
			pthread_mutex_unlock(&mux_alu);
			pthread_mutex_unlock(&pc_a_mux);

			mem_mux.updated = 0;
			last_clock = cpu_clock;

			pthread_barrier_wait(&current_cycle);
		}
		else pthread_yield();
	}
	pthread_exit(0);
}
#endif
