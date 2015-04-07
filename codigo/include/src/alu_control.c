/*
    File with implementation of ALU CONTROL routine.
    It receives a control parameter from the control unit and also analyzes
    the current instruction to define what ALU control signal will be sent to
    the ALU.
*/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
// FALTA: IMPLEMENTAR MUTEXES, BARREIRAS E VARIAVEIS DE CONDICAO
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
#ifndef _ALU_CONTROL_
#define _ALU_CONTROL_

#include <pthread.h>
#include "mascara.h"

extern int ir;
extern pthread_mutex_t alu_control_sign;
extern pthread_cond_t control_sign_wait;

// Recebe o campo de funcao e o sinal de controle ALUOp, e determina qual sera o sinal enviado a ula
void ALU_control_calc(void *not_used)
{
    int last_clock = 10;
    char alu_op;
    char cfuncao;

    while(ir){
        if (last_clock != cpu_clock){

            pthread_mutex_lock(&control_sign);
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            
            pthread_mutex_unlock(&control_sign);
            
            alu_op = (((separa_ALUOp0 | separa_ALUOp1) & sc) >> ALUOp0_POS) & 0x03;
            cfuncao = (ir & separa_cfuncao);
            if (alu_op == 0x00)       // UC forca uma soma
                alu_control = ativa_soma;
    
            if (alu_op == 0x01)       // UC forca uma subtracao
                alu_control = ativa_subtracao;
    
            if (alu_op == 0x02)       // UC nao sabe o que fazer
            {
                cfuncao = cfuncao & zera_2bits_cfuncao;         // cfuncao    op. na ula
                if (cfuncao == 0x00)                            // 10 0000       add
                    alu_control = ativa_soma;                   // 10 0010       sub
                if (cfuncao == 0x02)                            // 10 0100       and
                    alu_control = ativa_subtracao;              // 10 0101       or
                if (cfuncao == 0x04)                            // 10 1010       slt
                    alu_control = ativa_and;
                if (cfuncao == 0x05)
                    alu_control = ativa_or;
                if (cfuncao == 0x0a)
                    alu_control = ativa_slt;
            }

            pthread_barrier_wait(&current_cycle);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}

#endif