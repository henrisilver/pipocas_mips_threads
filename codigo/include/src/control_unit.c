/* SSC0640 - Sistemas Operacionais I
* Trabalho 1 - CPU MIPS com pthreads em C
* Data: 22/04/2015
***************************************************************
* Integrantes do Grupo:
* Guilherme Nishina Fortes 			- No. USP 7245552
* Henrique de Almeida Machado da Silveira 	- No. USP 7961089
* Marcello de Paula Ferreira Costa 		- No. USP 7960690
* Sergio Yudi Takeda 				- No. USP 7572996
***************************************************************
* GRUPO 13
*/

/*
	>> Arquivo com a implementacao da unidade de controle.
	>> Recebe um parametro de controle para a unidade de controle escolher
	>> entre os endereços de conteudo de pc ou aluout e mandar para a memoria principal.
*/
#ifndef _CONTROL_UNIT_
#define _CONTROL_UNIT_

#include <pthread.h>
#include "mascara.h"

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
}

int isValidInstruction(int instr) {//verifica se a instrucao lida corresponde a uma dentre as descritas na especificacao
    int opcode = ((instr & separa_cop) >> 26) & 0x3f;
    int funct = (char)(instr & separa_cfuncao) & zera_2bits_cfuncao;
    if(instr == 0)
        return 0;
    else if(instr == -1)
        return 1;
    else if((opcode != op_beq) && (opcode != op_jump) && (opcode != op_lw) && (opcode != op_sw) && (opcode != op_r_type))
        return 0;
    else  if((opcode == op_r_type) && (funct != 0x00) && (funct != 0x02) && (funct != 0x04) && (funct != 0x05) && (funct != 0x0a))
        return 0;
    else
        return 1;
}

void control_unit(void *not_used){
    
	pthread_barrier_wait(&threads_creation);

    	int last_clock = 1;
    	int instr;
    	int current_cycle_number = 0, total_cycles = 2;
    	int validInstruction;
    	int sinal;
    	char S = -1;//variavel que sera enviada para a funcao gera_sinal_controle. -1 eh para evitar erros
    	char opcode;

    	while(1){
	    	if(last_clock != cpu_clock){
            
		    	if(current_cycle_number == 1) {
		        	opcode = ((ir & separa_cop) >> 26) & 0x3f;//verifica qual operacao esta sendo realizada
		        	if (opcode == op_beq || opcode == op_jump)
		            		total_cycles = 2;//decide quandos ciclos serao necessarios para completar operacao
		        	else if (opcode == op_lw)
		            		total_cycles = 4;
		        	else total_cycles = 3;
				}

				char op = ((ir & separa_cop) >> 26) & 0x3f;
				
				switch(current_cycle_number){
					case 0:
						S = 0;
								break; 
						case 1:
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
				
				last_clock = cpu_clock;
				
				if(current_cycle_number < total_cycles)
						current_cycle_number++;
				else current_cycle_number = 0;
				
				gera_sinal_controle (&sinal, S);
				cs.value = (short int)(sinal & 0x0000ffff);

				instr = ir;
				validInstruction = isValidInstruction(instr);
				
				if( !validInstruction ){
					loop = 0;
					cs.invalidInstruction = 1;
				}

				pthread_mutex_lock(&control_sign);
				cs.isUpdated = 1;
				pthread_cond_broadcast(&control_sign_wait);
				pthread_mutex_unlock(&control_sign);

				if(!validInstruction ){
						pthread_barrier_wait(&update_registers);
						pthread_exit(0);
				}
					
				pthread_barrier_wait(&current_cycle);
				cs.isUpdated = 0;
				pthread_barrier_wait(&update_registers);

			}//chave de if(last_clock != cpu_clock)
				else pthread_yield();//da yeild para continuar com o andamento do processador; se o tique de clock atual nao necessitar da UC
		}//chave do while(1)
}
#endif
