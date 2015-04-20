/* thread que implementa a unidade de controle exclusiva a ula.
 * Sera responsavel pela definicao da operacao que sera executada pela ula
 * Primeiro verifica se o dado na unidade de controle pode ser consumido
 * entao verifica se a instrucao eh valida. Se for, os bits relevantes sao separados
 * e eh verificado qual comando de operacao sera enviado para a ula.
 * Apos isso, a flag de isUpdated eh mudada para um, sinalizando a finalizacao
 * das tarefas da alu control neste ciclo e sera esperado o consumo dos dados pela ula.
 * Depois a flag eh mudada para zero e um novo ciclo comeca */

#ifndef _ALU_CONTROL_
#define _ALU_CONTROL_

#include <pthread.h>
#include "mascara.h"

void alu_control(void *not_used){

        pthread_barrier_wait(&threads_creation);

    	char alu_op;
    	char alu_control_sign = 0;
    	char cfuncao;
    
    	while(1){//loop de execucao
            	pthread_mutex_lock(&control_sign);
            	if(!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            	}
            	pthread_mutex_unlock(&control_sign);

				if(cs.invalidInstruction){
					pthread_barrier_wait(&update_registers);
					pthread_exit(0);
				}

            	alu_op = (((separa_ALUOp0 | separa_ALUOp1) & cs.value) >> ALUOp0_POS) & 0x03;
            	cfuncao = (ir & separa_cfuncao);

            	if (alu_op == 0x00)       // UC forca uma soma
                	alu_control_sign = ativa_soma;
            
            	if (alu_op == 0x01)       // UC forca uma subtracao
                	alu_control_sign = ativa_subtracao;
            
            	if (alu_op == 0x02){       // UC nao sabe o que fazer
	                cfuncao = cfuncao & zera_2bits_cfuncao;         // cfuncao       op. na ula

        	        if (cfuncao == 0x00)                            // 10 0000       add
				alu_control_sign = ativa_soma;              		// 10 0010       sub
        	        if (cfuncao == 0x02)                            // 10 0100       and
				alu_control_sign = ativa_subtracao;         		// 10 0101       or
        	        if (cfuncao == 0x04)                            // 10 1010       slt
				alu_control_sign = ativa_and;
        	        if (cfuncao == 0x05)
				alu_control_sign = ativa_or;
        	        if (cfuncao == 0x0a)
				alu_control_sign = ativa_slt;
            	}
				
            	alu_s.value = alu_control_sign;
            	pthread_mutex_lock(&alu_sign);
            	alu_s.isUpdated = 1;
            	pthread_cond_signal(&alu_sign_wait);
            	pthread_mutex_unlock(&alu_sign);

            	pthread_barrier_wait(&current_cycle);
            	alu_s.isUpdated = 0;
            	pthread_barrier_wait(&update_registers);
    	}
}

#endif
