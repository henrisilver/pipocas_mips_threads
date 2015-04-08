/*
	File with implementation of Mux_2_IorD routine.
	It receives a control parameter from control unit to choose
	between PC's or ALUOut's adresses content to send to the main memory.
*/
#ifndef _MUX_2_PCSOURCE__
#define _MUX_2_PCSOURCE_

#include <pthread.h>
#include "mascara.h"

#define ALU_RESULT 0
#define ALUOUT 1

/* Extern global variables  */
extern int cpu_clock;
extern int aluout;
extern int pc;

extern link alu_result;
extern link pc_shift_left_buffer;

extern pthread_mutex_t control_sign;
extern pthread_cond_t control_sign_wait;

pthread_mutex_t pc_shift_left_result;
pthread_cond_t pc_shift_left_execution_wait

pthread_cond_t alu_execution_wait;
pthread_mutex_t alu_value_zero;

extern pthread_cond_t mux_pcsource_execution_wait;
extern pthread_mutex_t mux_pcsource;

extern pthread_barrier_t current_cycle;
extern pthread_barrier_t update_registers;

link mux_pcsource_buffer;

void mux_2_pcsource(void *not_used){
	int last_clock = 10;

	while(ir){
		if (last_clock != cpu_clock){

			pthread_mutex_lock(&control_sign);
			if(!cs.isUpdated)
				while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
                        pthread_mutex_unlock(&control_sign);

			if(( (separa_PCSource & cs.value) >> PCSource_POS) & 0x01 == ALU_RESULT){
				pthread_mutex_lock(&alu_value_zero);
				if(!alu_result.isUpdated)
					while(pthread_cond_wait(&alu_execution_wait,&alu_value) != 0);
				pthread_mutex_unlock(&alu_value_zero);
      				mux_pcsource_buffer.value = alu_result.value;
			}
			else if (( (separa_PCSource & cs.value) >> PCSource_POS ) & 0x01 == ALUOUT){
				mux_pcsource_buffer.value = aluout;
			}
			else {
				pthread_mutex_lock(&pc_shift_left_result);
				if(!pc_shit_left_result.isUpdated)
					while(pthread_cond_wait(&pc_shift_left_execution,&pc_shift_left_result) != 0);
				pthread_mutex_unlock(&pc_shift_left_result.value);
			}
			last_clock = cpu_clock;

			pthread_mutex_lock(&mux_pcsource);
			mux_pcsource_buffer.isUpdated = 1;
			pthread_cond_signal(&mux_pcsource_execution_wait);
			pthread_mutex_unlock(&mux_pcsource);

			pthread_barrier_wait(&current_cycle);
			mux_pcsource_buffer.isUpdated = 0;
			pthread_barrier_wait(&update_registers);
		}
		else pthread_yield();
	}
	pthread_exit(0);
}
#endif
