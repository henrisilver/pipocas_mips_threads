/*
	File with implementation of Mux_2_IorD routine.
	It receives a control parameter from control unit to choose
	between PC's or ALUOut's adresses content to send to the main memory.
*/

#ifndef _MUX_2_IORD_
#define _MUX_2_IORD_

#include <pthread.h>
#include "mascara.h"

#define PC 0

pthread_mutex_t control_sign;
pthread_cond_t cs_ready;		// Should this condition be declared in control unit scope?

int mux_IorD_buffer;			// This must be declared as "extern int mux_IorD_buffer" in the file that will include present file.

/*
	Precisamos pensar em como executar cada ciclo independentemente. Discutimos que deveria ser empregado o uso de barreiras.
	>> pthread_barrier_init(barrier,attr,count);

	Imaginei o seguinte: pthread_barrier_init deve ser chamado da main, esta que dita o clock. Quando é criado uma barreira, as threads que	depende desta devem executar o comando pthread_barrier_wait(barrier), cada thread que executar esse comando aumenta um contador da barreira. Enquanto esse número for menor do que count, as threads não executam.

	Para isso funcionar como controle de clock, quando a thread executar a tarefa, ela libera o mutex ou coisa parecida e chama a função de esperar pela barreira antes de executar pthread_yield();

	Uma pequena dúvida que tenho e gostaria de saber o que acham: Devemos pensar em quais sincronizações entre as threads? Digo, quais precisam executar primeiro? Exceto a unidade de controle que é obvio, temos que pensar em outras sincronias importantes ou é independente da sequência de execução das unidades lógicas? É mais natural pensar que precisamos de outras sincronias, mas isso não estaria tornando a concorrência desnecessária ou algo do tipo? Dorgas, mano.
 
*/

void mux_2_IorD(){
	int last_clock = 10; 		// Local clock to keep track if thread has already executed.

	while(valid_instruction){
		if (!(last_clock == cpu_clock)){
			pthread_mutex_lock(&control_sign);
			// This thread's routine won't go any further whilst mutex isn't locked.

			while(pthread_cond_wait(&cs_ready,&control_sign) != 0);
			// pthread_cond_wait returns 0 upon successful completion.
			// While it doesn't receive the broadcast from control unit, it won't classify as a successful routine.

			// Both the mutex is locked and control unit has successfully broadcasted its sign.
			// From now on, threads associated with MIPS CPU can execute accordingly.
			if(( (separa_IorD & sc) >> IorD_POS) & 0x01 == PC){
      				mux_IorD_buffer = pc;
			}
			else{
				mux_IorD_buffer = aluout;
			}
			last_clock = cpu_clock;
			// Flag that tells which clock was running when the job was completed
	
			pthread_mutex_unlock(&control_sign);
			pthread_barrier_wait(&current_cycle);		// Ilustrando o raciocínio acima de controle de ciclo
			pthread_yield();
			// Unlocks mutex and yields so another thread can do its job
		}
		else pthread_yield();
	}
	pthread_exit(0);
}
#endif
