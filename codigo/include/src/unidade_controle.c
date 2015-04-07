/*
    File with implementation of the control unit routine.
    It receives a control parameter from control unit to choose
    between PC's or ALUOut's adresses content to send to the main memory.
*/
#ifndef _UNIDADE_CONTROLE_
#define _UNIDADE_CONTROLE_

#include <pthread.h>
#include "mascara.h"

#define PC 0
extern int pc,aluout,cpu_clock;

pthread_mutex_t control_sign;
pthread_cond_t cs_ready;        // Should this condition be declared in control unit scope?


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

void control_unit(int IR, short int *sc, int cycle)
{
  int sinal;
    char S;
    char op = ((IR & separa_cop) >> 26) & 0x3f;
  switch(cycle)
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
          /* Dependererá do IR. IFS?
            if(branch) Gera sinais p branch
      else if(add) gera sinais p add
            add,sub,lw,sw
            */
          if(op == op_jump)//jump
              S = 9;
            else if(op == op_beq)//beq
              S = 8;
            else if(op == op_r_type)//r-type
              S = 6;
            else if(op == op_lw || op == op_sw)//lw ou sw
              S = 2;
        break;
      
        case 3:
          /*
            
            */
            if(op == op_r_type)//r-type
              S = 7;
            else if(op == op_lw) //lw ou sw
              S = 3;
            else if(op == op_sw) //lw ou sw
              S = 5;
        break;
      
        case 4: // Somente executara no caso da instrucao lw
          S = 4;
        break;
    }
    gera_sinal_controle (&sinal, S);
    *sc = output_signals & 0x0000ffff;
    // broadcast(sc);
}
#endif