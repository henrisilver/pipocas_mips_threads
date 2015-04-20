//unidade que realiza shift left apos o sign extend localizado na parte inferior do diagrama do caminho de dados

#ifndef _SHIFT_LEFT_AFTER_SE_
#define _SHIFT_LEFT_AFTER_SE_

#include <pthread.h>
#include "mascara.h"

void shift_left_after_se(void *not_used){

	pthread_barrier_wait(&threads_creation);//espera criacao de todas as threads do processados, para evitar erros

	while(1)
	{
			pthread_mutex_lock(&control_sign);
			if (!cs.isUpdated){
				while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);
			}
            pthread_mutex_unlock(&control_sign);

			if(cs.invalidInstruction){		//tratamento de erros para evitar entradas invalidas
				pthread_barrier_wait(&update_registers);
				pthread_exit(0);
			}

			pthread_mutex_lock(&sign_extend_mutex);
			if (!se.isUpdated)
				while(pthread_cond_wait(&sign_extend_cond, &sign_extend_mutex) != 0);
			pthread_mutex_unlock(&sign_extend_mutex);

			shift_left.value = se.value << 2;

			pthread_mutex_lock(&shift_left_mutex);
			shift_left.isUpdated = 1;
			pthread_cond_signal(&shift_left_cond);
			pthread_mutex_unlock(&shift_left_mutex);


			pthread_barrier_wait(&current_cycle);
			shift_left.isUpdated = 0;
			pthread_barrier_wait(&update_registers);
	}
}
#endif
