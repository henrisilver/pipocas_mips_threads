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
extern int pc;
extern int aluout;
extern int cpu_clock;
extern short int sc;
extern int mdr;

pthread_mutex_t mem_ir_mux;
pthread_cond_t cs_ready;
pthread_cond_t main_memory_read;

typedef struct link{
	int value;
	int updated;
}link;

link input,output;

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

void mux_2_MemtoReg(){
	int last_clock = 10;

	while(valid_instruction){
		if (last_clock != cpu_clock){
			pthread_mutex_lock(&mem_ir_mux);

			while(pthread_cond_wait(&cs_ready,&mem_ir_mux) != 0);
			if(( (separa_MemtoReg & sc) >> MemtoReg_POS) & 0x01 == ALUOut){
      				output.value = aluout;
			}
			else{
				if (l.updated == 0)
					while (pthread_cond_wait(&main_memory_read,&mem_ir_mux) != 0);
				output.value = mdr;
			}

			pthread_mutex_lock(&mux_regs);
			output.updated = 1;			// Sinaliza que o buffer esta atualizado
			pthread_cond_signal(&mux_selected);
			pthread_mutex_unlock(&mux_regs);
			pthread_mutex_unlock(&mem_ir_mux);

			input.updated = 0;
			last_clock = cpu_clock;

			pthread_barrier_wait(&current_cycle);
		}
		else pthread_yield();
	}
	pthread_exit(0);
}
#endif
