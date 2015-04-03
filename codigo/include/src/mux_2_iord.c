/*
	File with implementation of Mux_2_IorD routine.
	It receives a control parameter from control unit to choose
	between PC's or ALUOut's adresses content to send to the main memory.
*/

#ifndef _MUX_2_IORD_
#define _MUX_2_IORD_

#include <pthread.h>

#define PC 0

pthread_mutex_t control_sign;
pthread_cond_t cs_ready;		// Should this condition be declared in control unit scope?

void mux_2_IorD(){
	int last_clock = 10; 		// Local clock to keep track if thread has already executed

	while(true){
		if (!(last_clock == clock)){
			pthread_mutex_lock(&control_sign);
			// This thread's routine won't go any further whilst mutex isn't locked.

			while(pthread_cond_wait(&cs_ready,&control_sign) != 0)
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
			last_clock = clock;
			// Flag that tells which clock was running when the job was completed
	
			pthread_mutex_unlock(&control_sign)
			pthread_yield();
			// Unlocks mutex and yields so another thread can do its job
		}
		else pthread_yield();
	}
}
#endif

/*


	pthread_mutex_lock(&mutex)
        while(!condition) {
                pthread_cond_signal(&condition_t,&mutex) >> Imediatamente antes de dormir, libera o mutex!  <<
        }
        >> Realiza tarefas necessários depois de ter obtido o sinal verdadeiro de condição <<
        >> É implicito que quando sair do loop while, o mutex terá sido relocado <<

        pthread_mutex_unlock(&mutex)

		pthread_mutex_lock(&mutexIorD);
	      	if (pthread_cond_wait(&ciclo_num, &mutexIorD) == 0){
			received = 1;
		}
		
	      	pthread_mutex_unlock(&mutexIorD);
	}

      	if(controleMUX == 'PC'){
        	pthread_mutex_lock(&mutexPC); //fazer thread dormir se mutex ocupado?
	        pthread_cond_wait( &ciclo_num, &mutexPC );//verificar se estou no ciclo certo para leitura de pc
	        read(PC);//manda memory ler de PC
	        pthread_mutex_unlock(&mutexPC);
      
      	}
      
	else 	if(controleMUX == 'ALUout'){// 1
        
	        	pthread_mutex_lock(&mutexALUout);
		        pthread_cond_wait( &ciclo_num, &mutexALUout );//verificar se estou no ciclo certo para leitura de ALUout (funcao igual a de PC? Daria pra otimizar?)
		        read(ALUout);//manda memory ler de ALUout
		        pthread_mutex_unlock(&mutexALUout);
      		}
}
*/
