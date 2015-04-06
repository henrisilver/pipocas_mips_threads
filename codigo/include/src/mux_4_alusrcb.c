/*
	File with implementation of Mux_2_ALUSrcB routine.
	It receives a control parameter from control unit to choose
	between four possible values to be sent to the second ULA's input.
*/
#ifndef _MUX_4_ALUSRCB_
#define _MUX_4_ALUSRCB_

#include <pthread.h>
#include "mascara.h"

#define ALUOut 0
extern int cpu_clock;
extern int B_value;
extern int sign_extend_value;
extern int shift_left2_value;
extern c_sign;

extern pthread_mutex_t control_sign;
extern pthread_cond_t control_sign_wait;

int mux_MemtoReg_buffer;

void mux_4_ALUSrcB(){
	int last_clock = 10;

	while(ir_value){
		if (last_clock != cpu_clock){
			pthread_mutex_lock(&control_sign);

			if (!cs.isUpdated)
				while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);

			pthread_mutex_unlock(&control_sign);

			if((((separa_ALUSrcB0 | separa_ALUSrcB1) & cs.value) >> ALUSrcB0_POS) & 0x03 == 0)
				mux_ALUSrcB_buffer = B_value;		// B register value
			else if((((separa_ALUSrcB0 | separa_ALUSrcB1) & cs.value) >> ALUSrcB0_POS) & 0x03 == 1)
				mux_ALUSrcB_buffer = 4;			// PC's increment
			else if((((separa_ALUSrcB0 | separa_ALUSrcB1) & cs.value) >> ALUSrcB0_POS) & 0x03 == 2)
				mux_ALUSrcB_buffer = ext_sign_value;	// Imediato
			else mux_ALUSrcB_buffer = shift_left2_value;	// Branch Address

			last_clock = cpu_clock;
			pthread_barrier_wait(&current_cycle);
		}
		else pthread_yield();
	}
	pthread_exit(0);
}
#endif
