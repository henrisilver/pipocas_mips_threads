#ifndef _MUX_2_ALUSrcA_
#define _MUX_2_ALUSrcA_

#include <pthread.h>
#include "mascara.h"

#define ALUOut 0

extern int pc;
extern int cpu_clock;
extern int A_value;
extern int pc_value;
int mux_ALUSrcA_buffer;

extern pthread_cond_t cs_ready;

extern pthread_mutex_t control_sign;

void mux_2_ALUSrcA()
{
    int last_clock = 10;

    while(ir_value)
    {
		if (last_clock != cpu_clock)
		{
			pthread_mutex_lock(&control_sign);
			while(pthread_cond_wait(&cs_ready, &control_sign) != 0);//idle loop
	
	    		pthread_mutex_lock(&control_sign);
	
			//seguindo a logica dos outros muxes, tenho o mutex e a uc mandou o sinal verde
			if(( (separa_ALUSrcA & sc) >> ALUSrcA_POS) & 0x01 == 0)//verifico qual a entrada do mux
	    		{
				 mux_ALUSrcA_buffer = pc_value;
	
			}
			else
	            	{//entrada vem de A
	                	mux_ALUSrcA_buffer = A_value;
	        	}
	
	    		last_clock = cpu_clock;


            		pthread_barrier_wait(&current_cycle);
		}

        else pthread_yield();
    }

 pthread_exit(0);
}

#endif
