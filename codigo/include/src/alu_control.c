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

ALU_control_calc(&ALU_control, (((separa_ALUOp0 | separa_ALUOp1) & sc) >> ALUOp0_POS) & 0x03, (IR & separa_cfuncao));

// Recebe o campo de funcao e o sinal de controle ALUOp, e determina qual sera o sinal enviado a ula
void ALU_control_calc(char *ALU_control, char ALUOp, char cfuncao)
{
  char alu_op = (((separa_ALUOp0 | separa_ALUOp1) & sc) >> ALUOp0_POS) & 0x03;
  char cfuncao = (IR & separa_cfuncao);
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
}



#endif