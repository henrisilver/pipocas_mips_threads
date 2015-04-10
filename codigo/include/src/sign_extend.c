#ifndef _SIGN_EXTEND_
#define _SIGN_EXTEND_

#include <pthread.h>
#include "mascara.h"

void sign_extend(void *not_used)
{
    se.isUpdated = 0;
    int last_clock = 10;
    int i;
    short int imediato;
    int sign_extend_temp;
    
    while(ir)
    {
        if (last_clock != cpu_clock)
        {
            pthread_mutex_lock(&control_sign);
            if (!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait, &control_sign) != 0); //idle loop
            pthread_mutex_unlock(&control_sign);
            
            
            imediato = ir & separa_imediato;
            //seguindo a logica dos outros muxes, tenho o mutex e a uc mandou o sinal verde
            sign_extend_temp = imediato & 0x0000ffff;       // Copia do conteudo de MDR (16 bits) para *MDRnew
            imediato = imediato & 0x00008000;      // Eh feita uma mascara para manter apenas o MSB de MDR
            
            // O MSB de MDR eh copiado para todos os bits seguintes de *MDRnew (extensao de sinal)
            for (i = 1; i < 17; i++)
                sign_extend_temp = sign_extend_temp| (imediato << i);
            
            
            last_clock = cpu_clock;
            pthread_mutex_lock(&sign_extend_mutex);
            se.value = sign_extend_temp;
            se.isUpdated = 1;
            pthread_cond_broadcast(&sign_extend_cond);
            pthread_mutex_unlock(&sign_extend_mutex);
            
            se.isUpdated = 0;
            pthread_barrier_wait(&current_cycle);
        }
        
        else pthread_yield();
    }
    pthread_exit(0);
}

#endif
