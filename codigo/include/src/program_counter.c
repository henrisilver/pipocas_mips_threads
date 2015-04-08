#ifndef _PC_
#define _PC_

extern int and_result;
extern int cpu_clock;
extern int pc;
extern link or_result;
extern link mux_pcsource_buffer;
extern c_sign cs;

extern pthread_barrier_t current_cycle;
extern pthread_barrier_t update_registers;

extern pthread_mutex_t or_result_mutex;
extern ptread_cond_t or_result_wait;

extern pthread_cond_t mux_pcsource_execution_wait;
extern pthread_mutex_t mux_pcsource;

pthread_mutex_t pc_buffer;
ptread_cond_t pc_wait;

void program_counter(void *not_used){
	int last_clock = 10;

	pthread_mutex_init(&pc_buffer, NULL);
	pthread_cond_init(&pc_wait, NULL);

	while(ir){
		if (last_clock != cpu_clock){

            pthread_mutex_lock(&control_sign);//espera o clock
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);//idle loop. espera o cs atualizar
            pthread_mutex_unlock(&control_sign);//libera o mutex pois cond wait disputa por ele na volta

            pthread_mutex_lock(&or_result_mutex);
            if(!or_result.isUpdated)//espera confirmacao de escrita
                while(pthread_cond_wait(&or_result_wait, &or_result_mutex) != 0);//idle loop. espera o and_or
            pthread_mutex_unlock(&or_result_mutex);//libera o mutex pois cond wait disputa por ele na volta

            pthread_mutex_lock(&mux_pcsource);
            if(!mux_pcsource_buffer.isUpdated)//espera dados
                while(pthread_cond_wait(&mux_pcsource_execution_wait, &mux_pcsource) != 0);//idle loop. espera o mux ter um resultado
            pthread_mutex_unlock(&mux_pcsource);//libera o mutex pois cond wait disputa por ele na volta

            last_clock = cpu_clock;
            pthread_barrier_wait(&current_cycle);
            pc = mux_pcsource_buffer.value;

            pthread_barrier_wait(&update_registers);
		}
		else pthread_yield();
	}
	pthread_exit(0);

#endif
