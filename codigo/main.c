#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // não sei se vamos precisar
#include "mascara.h>"

#define NUM_THREADS_WITH_REGISTERS_TO_UPDATE 8 // 7 threads + 1 (main)

// Abaixo, declarar prototipo de funcoes das threads, seguindo o modelo do exemplo abaixo:
// int ula( int, int , char, int *, char *, char *);
// void control_unit(int, short int *);
// void Busca_Instrucao(short int, int, int, int, int *, int *, int *);
// void Decodifica_BuscaRegistrador(short int, int, int, int, int, int *, int *, int *);
// void Execucao_CalcEnd_Desvio(short int, int, int, int, int, int, int *, int *);
// void EscreveTipoR_AcessaMemoria(short int, int, int, int, int, int *, int *);
// void EscreveRefMem(short int, int, int, int);

/****** VARIAVEIS GLOBAIS ***********/
int pc = 0, aluout, mdr, ir = -1, a_value, b_value, alu_result, jump_address, BEQ_Address, memory_content_read, read_data_1, read_data_2, cpu_clock = 10;
int memoria[MAX], reg[NUMREG];
char zero, alu_overflow, alu_control;
short int sc = 0;

pthread_mutex_t control_sign;
pthread_mutex_t mutex_ciclo;
pthread_mutex_t mutex_IR;


// Falta implementar as threads para as funcoes da ULA e do jump address (a thread do shift left 2 de
// cima deve servir para computar o jump address). Variaveis que dependem dessas threads:
// -> alu_result
// -> jump_address


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
    pthread_t control_unit;
    pthread_t PC;
    pthread_t mux_2_iord;
    pthread_t mux_2_regdst;
    pthread_t mux_2_memtoreg;
    pthread_t mux_2_alusrca;
    pthread_t mux_3_pcsource;
    pthread_t mux_4_alusrcb;
    pthread_t main_memory;
    pthread_t instruction_register;
    pthread_t mdr;
    pthread_t registers_bank;
    pthread_t shift_left_pc;
    pthread_t shift_left_imm;
    pthread_t sign_ext;
    pthread_t alu;
    pthread_t alu_control;
    pthread_t aluout;
    pthread_t A;
    pthread_t B;
    pthread_t and_or;

                             //                                               ## fat(n) iterativo
    memoria[0] = 0x8db00000; // 1000 1101 1011 0000 0000 0000 0000 0000         lw $s0, 0($t5)       # carrega n da memoria
    memoria[1] = 0x11300005; // 0001 0001 0011 0000 0000 0000 0000 0101       loop: beq $t1, $s0, fim    # se der a cond. sai do loop
    memoria[2] = 0x01025020; // 0000 0001 0000 0010 0101 0000 0010 0000         add $t2, $t0, $v0    # t2 = t0 + v0)
    memoria[3] = 0x00404025; // 0000 0000 0100 0000 0100 0000 0010 0101         or $t0, $v0, $zero   # t0 = v0
    memoria[4] = 0x01431024; // 0000 0001 0100 0011 0001 0000 0010 0100         and $v0, $t2, $v1    # v0 = t2
    memoria[5] = 0x012b4820; // 0000 0001 0010 1011 0100 1000 0010 0000         add $t1, $t1, $t3    # t1 = t1 + 1
    memoria[6] = 0x08000001; // 0000 1000 0000 0000 0000 0000 0000 0001         j loop (palavra 1)   # volta pro loop
    memoria[7] = 0xad820000; // 1010 1101 1000 0010 0000 0000 0000 0000       fim:  sw $v0, 0($t4)       # salva o resultado na memoria
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
    
    
     while(ir) //Trocar instrução
     {  
        pthread_barrier_init(&update_registers, NULL, (unsigned int)NUM_THREADS_WITH_REGISTERS_TO_UPDATE);
        cpu_clock = 0; 
        pthread_barrier_wait(&update_registers);
        pthread_barrier_destroy(&update_registers);
       
        pthread_barrier_init(&update_registers, NULL, (unsigned int)NUM_THREADS_WITH_REGISTERS_TO_UPDATE);
        cpu_clock++;
        pthread_barrier_wait(&update_registers);
        pthread_barrier_destroy(&update_registers);
       
       /* Aqui o segundo ciclo já foi executado por completo. Precisamos saber qual o tipo de instrução para continuarmos executando */
     
        char opcode = ((ir & separa_cop) >> 26) & 0x3f;
        if (opcode == op_beq || opcode == op_jump) cycles = 2;
        else if (opcode == op_lw) cycles = 4;
        else cycles = 3; // caso do op_sw e op_r_type
         
        while(cpu_clock < cycles) /*nao*/
        {
          pthread_barrier_init(&update_registers, NULL, (unsigned int)NUM_THREADS_WITH_REGISTERS_TO_UPDATE);
          cpu_clock++;
          pthread_barrier_wait(&update_registers);
          pthread_barrier_destroy(&update_registers);
        }
    }
    // fim do while(loop)

    // impressao da memoria para verificar se a implementacao esta correta
    // {
    //    int ii;
    //    for (ii = 20; ii < 22; ii++)
    //    {
    //        printf("memoria[%d]=%ld \n", ii, memoria[ii]);
    //    }
    // }

  // Criacao de threads
  //    pthread_t prod_handle, cons_handle;

  // /* declarations and initializations */
  //      item_available = 0;

  // sem_init (&mutex, 0 , 1);
  // sem_init(&empty, 0, MAX_QUEUE);
  // sem_init(&full, 0, 0);

  // /* create and join producer and consumer threads */

  // if (pthread_create(&prod_handle, 0, (void *) producer, (void *) 0) != 0) { 
  //  printf("Error creating thread producer! Exiting! \n");
  //  exit(0);
  // }
  // if (pthread_create(&cons_handle, 0, (void *) consumer, (void *) 0) != 0) { 
  //  printf("Error creating thread consumer! Exiting! \n");
  //  exit(0);
  // }

  // printf("\n Thread pai vai esperar filhas.\n\n");
  // fflush(0);

  // pthread_join(prod_handle, 0);
  // pthread_join(cons_handle, 0);

        exit( 0);
} // fim de main

