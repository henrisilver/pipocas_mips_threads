#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // não sei se vamos precisar

int main (int, char **);
int ula( int, int , char, int *, char *, char *);
void control_unit(int, short int *);
void Busca_Instrucao(short int, int, int, int, int *, int *, int *);
void Decodifica_BuscaRegistrador(short int, int, int, int, int, int *, int *, int *);
void Execucao_CalcEnd_Desvio(short int, int, int, int, int, int, int *, int *);
void EscreveTipoR_AcessaMemoria(short int, int, int, int, int, int *, int *);
void EscreveRefMem(short int, int, int, int);

/*
Threads:
PC: 1 Thread, 1 funcao
Mux 2 entradas: 4 threads, 1 funcao
Mux 3 entradas: 1 thread, 1 funcao
Mux 4 entradas: 1 thread, 1 funcao
memory: 1 thread, 1 funcao
instruction register: 1 thread, 1 funcao
memory data register: 1 thread, 1 funcao
registers bank: 1 thread, 1 funcao
Shift left: 2 threads, 1 funcao
Signal Extend: 1 thread, 1 funcao
ALU: 1 thread, 1 funcao
ALU Control: 1 thread, 1 funcao
A: 1 thread, 1 funcao
B: 1 thread, 1 funcao
Aluout: 1 thread, 1 funcao
AND e OR: 1 thread, 1 funcao
*/

int main (int argc, char *argv[])
{
    int PCnew = 0, IRnew, MDRnew, Anew, Bnew, ALUOUTnew;
    int PC = 0, IR=-1, MDR, A, B, ALUOUT;   
    short int sc = 0;
    int nr_ciclos = 0;
  
  	pthread_t control_unit;
  	pthread_t PC;
  	pthread_t mux_2_iord;
  	pthread_t mux_2_regdst;
    pthread_t mux_2_memtoreg;
    pthread_t mux_2_alusrca;
  	pthread_t mux_3_pcsource;
  	pthread_t mux_4_alusrcb;
  	pthreat_t main_memory;
  	pthread_t instruction_register;
  	pthread_t mdr;
  	pthreat_t registers_bank;
  	pthread_t shift_left_pc;
  	pthread_t shift_left_imm;
  	pthread_t sign_ext;
  	pthread_t alu;
	pthread_t alu_control;
	pthreat_t aluout;
    pthread_t A;
    pthread_t B;
    pthread_t and_or;
  
                             //                                             	## fat(n) iterativo
    memoria[0] = 0x8db00000; // 1000 1101 1011 0000 0000 0000 0000 0000         lw $s0, 0($t5)       # carrega n da memoria
    memoria[1] = 0x11300005; // 0001 0001 0011 0000 0000 0000 0000 0101   		loop: beq $t1, $s0, fim    # se der a cond. sai do loop
    memoria[2] = 0x01025020; // 0000 0001 0000 0010 0101 0000 0010 0000         add $t2, $t0, $v0    # t2 = t0 + v0)
    memoria[3] = 0x00404025; // 0000 0000 0100 0000 0100 0000 0010 0101         or $t0, $v0, $zero   # t0 = v0
    memoria[4] = 0x01431024; // 0000 0001 0100 0011 0001 0000 0010 0100         and $v0, $t2, $v1    # v0 = t2
    memoria[5] = 0x012b4820; // 0000 0001 0010 1011 0100 1000 0010 0000         add $t1, $t1, $t3    # t1 = t1 + 1
    memoria[6] = 0x08000001; // 0000 1000 0000 0000 0000 0000 0000 0001         j loop (palavra 1)   # volta pro loop
    memoria[7] = 0xad820000; // 1010 1101 1000 0010 0000 0000 0000 0000   		fim:  sw $v0, 0($t4)       # salva o resultado na memoria
    memoria[8] = 0;          // criterio de parada do programa   ciclos: 1+5+(22*(n-1))+3+4+1
    memoria[9] = 0;
    // Dados
    memoria[20] = 10;         // Fibonacci que se deseja calcular
                              // memoria[21] ira receber o resultado
    
    reg[0] = 0;   // $zero
    
    reg[2] = 0;   // $v0
    reg[3] = -1;  // $v1

    reg[8] = 1;   // $t0
    reg[9] = 1;   // $t1
    reg[11] = 1;  // $t3
    reg[12] = 84; // $t4
    reg[13] = 80; // $t5
    
    while(loop) //Trocar instrução
    {
      	control_unit(IR, &sc, 0);
      	Busca_Instrucao(sc, PC, ALUOUT, IR, &PCnew, &IRnew, &MDRnew);
      
      	control_unit(IR, &sc, 1);
      	ciclos = Decodifica_BuscaRegistrador(sc, IR, PC, A, B, &Anew, &Bnew, &ALUOUTnew);
      
      	while(i < ciclos)
        {
            // aqui comeca um novo ciclo
            // abaixo estao as unidades funcionais que executarao em todos os ciclos
            // os sinais de controle em sc impedirao/permitirao que a execucao seja, de fato, efetivada
            control_unit(IR, &sc, ciclo);
            //Busca_Instrucao(sc, PC, ALUOUT, IR, &PCnew, &IRnew, &MDRnew);

            Execucao_CalcEnd_Desvio(sc, A, B, IR, PC, ALUOUT, &ALUOUTnew, &PCnew);
            EscreveTipoR_AcessaMemoria(sc, B, IR, ALUOUT, PC, &MDRnew, &IRnew);
            EscreveRefMem(sc, IR, MDR, ALUOUT);

            // contador que determina quantos ciclos foram executados
            nr_ciclos++;

            // atualizando os registradores temporarios necessarios ao proximo ciclo.
            PC = PCnew;
            IR = IRnew;
            MDR = MDRnew;
            A = Anew;
            B = Bnew;
            ALUOUT = ALUOUTnew;
        }
    } // fim do while(loop)
    
    // impressao da memoria para verificar se a implementacao esta correta
    {
       int ii;
       for (ii = 20; ii < 22; ii++)
       {
           printf("memoria[%d]=%ld \n", ii, memoria[ii]);
       }
       printf("Nr de ciclos executados =%d \n", nr_ciclos);
    }

	// Criacao de threads
 	//    pthread_t prod_handle, cons_handle;

	// /* declarations and initializations */
 	//    	item_available = 0;

	// sem_init (&mutex, 0 , 1);
	// sem_init(&empty, 0, MAX_QUEUE);
	// sem_init(&full, 0, 0);

	// /* create and join producer and consumer threads */

	// if (pthread_create(&prod_handle, 0, (void *) producer, (void *) 0) != 0) { 
	// 	printf("Error creating thread producer! Exiting! \n");
	// 	exit(0);
	// }
	// if (pthread_create(&cons_handle, 0, (void *) consumer, (void *) 0) != 0) { 
	// 	printf("Error creating thread consumer! Exiting! \n");
	// 	exit(0);
	// }
	
	// printf("\n Thread pai vai esperar filhas.\n\n");
	// fflush(0);

	// pthread_join(prod_handle, 0);
	// pthread_join(cons_handle, 0);
    
    exit( 0);
} // fim de main

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
      		if(op == 2)//jump
              S = 9;
            else if(op == 4)//beq
              S = 8;
            else if(op == 0)//r-type
              S = 6;
            else if(op == 35 || op == 43)//lw ou sw
              S = 2;
      	break;
      
      	case 3:
      		/*
            
            */
            if(op == 0)//r-type
              S = 7;
            else if(op == 35) //lw ou sw
              S = 3;
            else if(op == 43) //lw ou sw
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
              
/////////////////////////////////////////////////////////////////////
//////////////// FIM UNIDADE DE CONTROLE ////////////////////////////
/////////////////////////////////////////////////////////////////////