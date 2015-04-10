#ifndef _AND_OR_
#define _AND_OR_

#include <pthread.h>
#include "mascara.h"

void and_or(void *not_used){
    int last_clock = 10;
    int and_result;//variavel interna a thread, sera mandada apenas para a porta or
    int or_result_temp;
    or_result.isUpdated = 1;
    
    while(ir){
        if (last_clock != cpu_clock){
            
            last_clock = cpu_clock;
            
            pthread_mutex_lock(&control_sign);
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0);//idle loop. espera o cs atualizar
            pthread_mutex_unlock(&control_sign);//libera o mutex pois cond wait disputa por ele na volta
            
            pthread_mutex_lock(&alu_zero_mutex);
            if(!alu_zero.isUpdated)
                while(pthread_cond_wait(&alu_zero_wait, &alu_zero_mutex) != 0);//idle loop. espera a alu realizar a operacao
            pthread_mutex_unlock(&alu_zero_mutex);
            
            //inicio operacao para a porta and
            if(( (separa_PCWriteCond & cs.value) >> 9) & alu_zero.value)//separa o bit importante e faz a operacao
                and_result = 1;
            
            else
                and_result = 0;
            //fim operacao para porta and
            
            //inicio operacao para porta ou
            if( ((separa_PCWrite & cs.value) >> 10) | and_result ) {
                or_result_temp = 1;
            }
            
            else {
                or_result_temp = 0;
            }
            //fim operacao para porta ou
            
            pthread_mutex_lock(&or_result_mutex);
            or_result.value = or_result_temp;
            or_result.isUpdated = 1;
            pthread_cond_signal(&or_result_wait);//signaliza para pc que ja tem (ou nao)o resultado para escrita
            pthread_mutex_unlock(&or_result_mutex);
            
            pthread_barrier_wait(&current_cycle);
            or_result.isUpdated = 0;
            pthread_barrier_wait(&update_registers);
            
        }
        
        else pthread_yield();
    }
    
    pthread_exit(0);
}

#endif
