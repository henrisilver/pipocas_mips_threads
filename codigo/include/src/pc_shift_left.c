/* implementa o shift left que pega 26 bits de IR e os multiplica por 4 
 * este valor sera concatenado com os 4 bits mais significativos de pc*/ 

#ifndef _PC_SHIFT_LEFT_
#define _PC_SHIFT_LEFT_

#include <pthread.h>
#include "mascara.h"

void pc_shift_left(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	int pc_temp;

    	while(1)
    	{
            	pthread_mutex_lock(&control_sign);
            	if(!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
				}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
						pthread_exit(0);
				}

            	pc_shift_left_buffer.value = (ir & 0x03ffffff) << 2;//implementa shift left 2
            	pc_temp = (0xf0000000 & pc);//separa os 4 bits mais significativos de pc
            	pc_shift_left_buffer.value = (pc_temp | pc_shift_left_buffer.value);//concatena
            
            	pthread_mutex_lock(&pc_shift_left_result);
            	pc_shift_left_buffer.isUpdated = 1;
            	pthread_cond_signal(&pc_shift_left_execution_wait);
            	pthread_mutex_unlock(&pc_shift_left_result);


            	pthread_barrier_wait(&current_cycle);
            	pc_shift_left_buffer.isUpdated = 0;
            	pthread_barrier_wait(&update_registers);
    	}
}
#endif
