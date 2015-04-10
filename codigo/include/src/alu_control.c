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

// Recebe o campo de funcao e o sinal de controle ALUOp, e determina qual sera o sinal enviado a ula
void alu_control(void *not_used)
{
    int last_clock = 10;
    char alu_op;
    char alu_control_sign = 0;
    char cfuncao;
    alu_s.isUpdated = 0;
    
    while(ir){
        if (last_clock != cpu_clock){
            
            pthread_mutex_lock(&control_sign);
            if(!cs.isUpdated)
                while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
            
            pthread_mutex_unlock(&control_sign);
            
            alu_op = (((separa_ALUOp0 | separa_ALUOp1) & cs.value) >> ALUOp0_POS) & 0x03;
            cfuncao = (ir & separa_cfuncao);
            if (alu_op == 0x00)       // UC forca uma soma
                alu_control_sign = ativa_soma;
            
            if (alu_op == 0x01)       // UC forca uma subtracao
                alu_control_sign = ativa_subtracao;
            
            if (alu_op == 0x02)       // UC nao sabe o que fazer
            {
                cfuncao = cfuncao & zera_2bits_cfuncao;         // cfuncao       op. na ula
                if (cfuncao == 0x00)                            // 10 0000       add
                    alu_control_sign = ativa_soma;              // 10 0010       sub
                if (cfuncao == 0x02)                            // 10 0100       and
                    alu_control_sign = ativa_subtracao;         // 10 0101       or
                if (cfuncao == 0x04)                            // 10 1010       slt
                    alu_control_sign = ativa_and;
                if (cfuncao == 0x05)
                    alu_control_sign = ativa_or;
                if (cfuncao == 0x0a)
                    alu_control_sign = ativa_slt;
            }
            
            last_clock = cpu_clock;
            pthread_mutex_lock(&alu_sign);          //trava a variavel de alucontrol para passar para alu
            alu_s.value = alu_control_sign;         //atualiza o valor na alucontrol depois da verificacao de qual comando estaria sendo enviado
            alu_s.isUpdated = 1;                      //sinaliza que pode ser lido, a variavel esta atualizada para este ciclo
            pthread_cond_signal(&alu_sign_wait);    //sinaliza para a alu que o dado esta pronto para consumo
            pthread_mutex_unlock(&alu_sign);        //abdica do mutex para a variavel da alucontrol
            pthread_barrier_wait(&current_cycle);   //sinaliza que os deveres desta thread foram concluidos
            
            alu_s.isUpdated = 0;
            
            pthread_barrier_wait(&update_registers);//espera as threads completarem para retornar ao estado 0 da maq. de estados
        }
        else pthread_yield();
    }
    pthread_exit(0);
}

#endif