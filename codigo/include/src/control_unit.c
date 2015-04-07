/*
 File with implementation of the control unit routine.
 It receives a control parameter from control unit to choose
 between PC's or ALUOut's adresses content to send to the main memory.
 */
#ifndef _CONTROL_UNIT_
#define _CONTROL_UNIT_

#define NUM_OF_THREADS_WHICH_USE_CSVALUE 11

#include <pthread.h>
#include "mascara.h"

#define PC 0
extern int pc,aluout,cpu_clock;

pthread_mutex_t control_sign;
pthread_cond_t control_sign_wait;        // Should this condition be declared in control unit scope?


/////////////////////////////////////////////////////////////////////
//////////////// UNIDADE DE CONTROLE ////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Agrupamento do sinal de saida: bits [15...0] = sinais de controle
//                                bits [17, 16] = AddrClt
void gera_sinal_controle (int *output, char S)
{
    // Todos os sinais sao resetados por default (don't care = 0)
    *output = 0x00000000;
    
    // Estado 0 - Instruction fetch
    if (S == 0x00)
    {
        *output = *output | ativa_MemRead;
        *output = *output & desativa_ALUSrcA;
        *output = *output & desativa_IorD;
        *output = *output | ativa_IRWrite;
        *output = *output | ativa_ALUSrcB0;
        *output = *output & desativa_ALUSrcB1;
        *output = *output & desativa_ALUOp0;
        *output = *output & desativa_ALUOp1;
        *output = *output | ativa_PCWrite;
        *output = *output & desativa_PCSource0;
        *output = *output & desativa_PCSource1;
        
        // Controle de sequenciamento
        *output = *output | sequencia;
    }
    
    // Estado 1 - Instruction decode/register fetch
    else if (S == 0x01)
    {
        *output = *output & desativa_ALUSrcA;
        *output = *output | ativa_ALUSrcB0;
        *output = *output | ativa_ALUSrcB1;
        *output = *output & desativa_ALUOp0;
        *output = *output & desativa_ALUOp1;
        
        *output = *output | tab_desp1;
    }
    
    // Estado 2 - Memory address computation
    else if (S == 0x02)
    {
        *output = *output | ativa_ALUSrcA;
        *output = *output & desativa_ALUSrcB0;
        *output = *output | ativa_ALUSrcB1;
        *output = *output & desativa_ALUOp0;
        *output = *output & desativa_ALUOp1;
        
        *output = *output | tab_desp2;
    }
    
    // Estado 3 - Memory Acess
    else if (S == 0x03)
    {
        *output = *output | ativa_MemRead;
        *output = *output | ativa_IorD;
        
        *output = *output | sequencia;
    }
    
    // Estado 4 - Write-back step
    else if (S == 0x04)
    {
        *output = *output & desativa_RegDst;
        *output = *output | ativa_RegWrite;
        *output = *output | ativa_MemtoReg;
        
        *output = *output | volta_busca;
    }
    
    // Estado 5 - Memory Acess
    else if (S == 0x05)
    {
        *output = *output | ativa_MemWrite;
        *output = *output | ativa_IorD;
        
        *output = *output | volta_busca;
    }
    
    // Estado 6 - Execution
    else if (S == 0x06)
    {
        *output = *output | ativa_ALUSrcA;
        *output = *output & desativa_ALUSrcB0;
        *output = *output & desativa_ALUSrcB1;
        *output = *output & desativa_ALUOp0;
        *output = *output | ativa_ALUOp1;
        
        *output = *output | sequencia;
    }
    
    // Estado 7 - R-type completion
    else if (S == 0x07)
    {
        *output = *output | ativa_RegDst;
        *output = *output | ativa_RegWrite;
        *output = *output & desativa_MemtoReg;
        
        *output = *output | volta_busca;
    }
    
    // Estado 8 - Branch completion
    else if (S == 0x08)
    {
        *output = *output | ativa_ALUSrcA;
        *output = *output & desativa_ALUSrcB0;
        *output = *output & desativa_ALUSrcB1;
        *output = *output | ativa_ALUOp0;
        *output = *output & desativa_ALUOp1;
        *output = *output | ativa_PCWriteCond;
        *output = *output | ativa_PCSource0;
        *output = *output & desativa_PCSource1;
        
        *output = *output | volta_busca;
    }
    
    // Estado 9 - Jump completion
    else if (S == 0x09)
    {
        *output = *output | ativa_PCWrite;
        *output = *output & desativa_PCSource0;
        *output = *output | ativa_PCSource1;
        *output = *output | volta_busca;
    }
}//fecha void gera_sinal_controle (int *output, char S)

typedef struct c_sign{
    short int value;        // Inteiro que representa o sinal de controle
    int isUpdated;          // 1 para atualizado e 0 caso contrário
}c_sign;

c_sign cs;

void control_unit(void *not_used)
{
    int sinal;
    char S;
    char op = ((ir & separa_cop) >> 26) & 0x3f;
    while(ir)
    {
        if (last_clock != cpu_clock){
            switch(cpu_clock)
            {
                case 0:
                    /*Busca Instrução 1 Estado
                     Gera sinais de controle
                     */
                    S = 0;
                    break;
                    
                case 1:
                    /*Decodifica 2 Estados
                     Gera novos sinais de controle
                     */
                    S = 1;
                    break;
                    
                case 2:
                    if(op == op_jump)                   //jump
                        S = 9;
                    else if(op == op_beq)               //beq
                        S = 8;
                    else if(op == op_r_type)            //r-type
                        S = 6;
                    else if(op == op_lw || op == op_sw) //lw ou sw
                        S = 2;
                    break;
                    
                case 3:
                    if(op == op_r_type)     //r-type
                        S = 7;
                    else if(op == op_lw)    //lw ou sw
                        S = 3;
                    else if(op == op_sw)    //lw ou sw
                        S = 5;
                    break;
                    
                case 4: // Somente executara no caso da instrucao lw
                    S = 4;
                    break;
            }
            gera_sinal_controle (&sinal, S);
            
            pthread_barrier_init(&current_cycle,NULL, (unsigned int)NUM_OF_THREADS_WHICH_USE_CSVALUE);
            pthread_mutex_lock(&control_sign);
            cs.value = (short int)(sinal & 0x0000ffff);
            cs.updated = 1;
            pthread_cond_broadcast(&control_sign_wait);
            pthread_mutex_unlock(&control_sign);
            pthread_barrier_wait(&current_cycle);
            
            cs.updated = 0;
            
            pthread_barrier_wait(&update_registers);
        }
        else pthread_yield();
    }
    pthread_exit(0);
}
#endif