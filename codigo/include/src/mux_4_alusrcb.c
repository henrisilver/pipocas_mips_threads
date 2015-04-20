//multiplexador de 4 entradas com sinal de controle ALUSrcB

#ifndef _MUX_4_ALUSRCB_
#define _MUX_4_ALUSRCB_

#include <pthread.h>
#include "mascara.h"

#define ALUOut 0

void mux_4_alusrcb(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	while(1){
            	pthread_mutex_lock(&control_sign);
            	if (!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);//idle loop
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);
				}
            
            	if(((((separa_ALUSrcB0 | separa_ALUSrcB1) & cs.value) >> ALUSrcB0_POS) & 0x03) == 0)
                	mux_alusrcb_buffer.value = b_value;//pega valor do registrador b
            	else if(((((separa_ALUSrcB0 | separa_ALUSrcB1) & cs.value) >> ALUSrcB0_POS) & 0x03) == 1)
                	mux_alusrcb_buffer.value = 4;//pega o valor de 4 para incrementar pc
            	else if(((((separa_ALUSrcB0 | separa_ALUSrcB1) & cs.value) >> ALUSrcB0_POS) & 0x03) == 2)
            	{
               		pthread_mutex_lock(&sign_extend_mutex);
                	if (!se.isUpdated)
                    		while(pthread_cond_wait(&sign_extend_cond,&sign_extend_mutex) != 0);
                	pthread_mutex_unlock(&sign_extend_mutex);
                	mux_alusrcb_buffer.value = se.value;//pega o valor obtido apos o sign extend, mas antes de shift left
            	}
            	else {
                	pthread_mutex_lock(&shift_left_mutex);
                	if (!shift_left.isUpdated)
                    		while(pthread_cond_wait(&shift_left_cond, &shift_left_mutex) != 0);
                	pthread_mutex_unlock(&shift_left_mutex);
                	mux_alusrcb_buffer.value = shift_left.value;//pega o valor obtido apos o shift left
            	}

            	pthread_mutex_lock(&mux_alusrcb_result);
            	mux_alusrcb_buffer.isUpdated = 1;//flag: indica que o valor pode ser utilizado sem problemas
            	pthread_cond_signal(&mux_alusrcb_execution_wait);
            	pthread_mutex_unlock(&mux_alusrcb_result);

            
            	pthread_barrier_wait(&current_cycle);
            	mux_alusrcb_buffer.isUpdated = 0;//o valor foi consumido por todas as threads que o utilizam
            	pthread_barrier_wait(&update_registers);
    	}
}
#endif
