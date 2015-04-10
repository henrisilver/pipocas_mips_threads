#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mascara.h"


#define NUM_THREADS_WITH_REGISTERS_TO_UPDATE 19 // 7 threads + 1 (main)
#define NUM_OF_THREADS_WHICH_USE_CSVALUE 21 // todas as outras threads


/****** VARIAVEIS GLOBAIS ***********/
int pc = 0, aluout, mdr, ir = -1, a_value, b_value, jump_address, BEQ_Address, mem_data, read_data_1, read_data_2, cpu_clock = 10, aluout;
int reg[NUMREG];

char zero, alu_overflow;
short int sc = 0;

c_sign cs;
connection memoria[MEMORY_SIZE], alu_zero, alu_result, mux_alusrca_buffer, mux_iord_buffer, mux_alusrca_buffer, mux_alusrcb_buffer, mux_memtoreg_buffer, mux_regdst_buffer, mux_pcsource_buffer, or_result, pc_shift_left_buffer, shift_left, se;
alu_signal alu_s;

pthread_mutex_t control_sign;
pthread_mutex_t alu_sign;
pthread_mutex_t or_result_mutex;
pthread_mutex_t mux_iord_result;
pthread_mutex_t mux_alusrca_result;
pthread_mutex_t mux_memtoreg_result;
pthread_mutex_t mux_regdst_result;
pthread_mutex_t mux_pcsource_result;
pthread_mutex_t pc_shift_left_result;
pthread_mutex_t alu_result_mutex;
pthread_mutex_t alu_zero_mutex;
pthread_mutex_t mux_alusrcb_result;
pthread_mutex_t pc_buffer;
pthread_mutex_t shift_left_mutex;
pthread_mutex_t sign_extend_mutex;

pthread_cond_t control_sign_wait;
pthread_cond_t alu_sign_wait;
pthread_cond_t or_result_wait;
pthread_cond_t mux_iord_execution_wait;
pthread_cond_t mux_alusrca_execution_wait;
pthread_cond_t mux_memtoreg_execution_wait;
pthread_cond_t mux_regdst_execution_wait;
pthread_cond_t mux_pcsource_execution_wait;
pthread_cond_t pc_shift_left_execution_wait;
pthread_cond_t alu_result_wait;
pthread_cond_t alu_zero_wait;
pthread_cond_t mux_alusrcb_execution_wait;
pthread_cond_t pc_wait;
pthread_cond_t shift_left_cond;
pthread_cond_t sign_extend_cond;

pthread_barrier_t update_registers;
pthread_barrier_t current_cycle;


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
    int i, cycles;
    for(i = 0; i < MEMORY_SIZE; i++)
        memoria[i].isUpdated = 0;
    
    //                                               ## fat(n) iterativo
    memoria[0].value = 0x8db00000; // 1000 1101 1011 0000 0000 0000 0000 0000         lw $s0, 0($t5)       # carrega n da memoria
    memoria[1].value = 0x11300005; // 0001 0001 0011 0000 0000 0000 0000 0101       loop: beq $t1, $s0, fim    # se der a cond. sai do loop
    memoria[2].value = 0x01025020; // 0000 0001 0000 0010 0101 0000 0010 0000         add $t2, $t0, $v0    # t2 = t0 + v0)
    memoria[3].value = 0x00404025; // 0000 0000 0100 0000 0100 0000 0010 0101         or $t0, $v0, $zero   # t0 = v0
    memoria[4].value = 0x01431024; // 0000 0001 0100 0011 0001 0000 0010 0100         and $v0, $t2, $v1    # v0 = t2
    memoria[5].value = 0x012b4820; // 0000 0001 0010 1011 0100 1000 0010 0000         add $t1, $t1, $t3    # t1 = t1 + 1
    memoria[6].value = 0x08000001; // 0000 1000 0000 0000 0000 0000 0000 0001         j loop (palavra 1)   # volta pro loop
    memoria[7].value = 0xad820000; // 1010 1101 1000 0010 0000 0000 0000 0000       fim:  sw $v0, 0($t4)       # salva o resultado na memoria
    memoria[8].value = 0;          // criterio de parada do programa   ciclos: 1+5+(22*(n-1))+3+4+1
    memoria[9].value = 0;
    // Dados
    memoria[20].value = 10;         // Fibonacci que se deseja calcular
    // memoria[21] ira receber o resultado
    
    reg[0] = 0;   // $zero
    
    reg[2] = 0;   // $v0
    reg[3] = -1;  // $v1
    
    reg[8] = 1;   // $t0
    reg[9] = 1;   // $t1
    reg[11] = 1;  // $t3
    reg[12] = 84; // $t4
    reg[13] = 80; // $t5
    
    
    pthread_mutex_init(&control_sign, NULL);
    pthread_mutex_init(&alu_sign, NULL);
    pthread_mutex_init(&mux_alusrca_result, NULL);
    pthread_mutex_init(&mux_alusrcb_result, NULL);
    pthread_mutex_init(&mux_iord_result, NULL);
    pthread_mutex_init(&mux_memtoreg_result, NULL);
    pthread_mutex_init(&mux_regdst_result, NULL);
    pthread_mutex_init(&mux_pcsource_result, NULL);
    pthread_mutex_init(&alu_zero_mutex, NULL);
    pthread_mutex_init(&or_result_mutex, NULL);
    pthread_mutex_init(&pc_shift_left_result, NULL);
    pthread_mutex_init(&alu_result_mutex, NULL);
    pthread_mutex_init(&pc_buffer, NULL);
    pthread_mutex_init(&shift_left_mutex, NULL);
    pthread_mutex_init(&sign_extend_mutex, NULL);
    
    pthread_cond_init(&control_sign_wait, NULL);
    pthread_cond_init(&alu_sign_wait, NULL);
    pthread_cond_init(&mux_alusrca_execution_wait, NULL);
    pthread_cond_init(&mux_alusrcb_execution_wait, NULL);
    pthread_cond_init(&mux_iord_execution_wait, NULL);
    pthread_cond_init(&mux_memtoreg_execution_wait, NULL);
    pthread_cond_init(&mux_regdst_execution_wait, NULL);
    pthread_cond_init(&mux_pcsource_execution_wait, NULL);
    pthread_cond_init(&alu_zero_wait, NULL);
    pthread_cond_init(&or_result_wait, NULL);
    pthread_cond_init(&pc_shift_left_execution_wait, NULL);
    pthread_cond_init(&alu_result_wait, NULL);
    pthread_cond_init(&pc_wait, NULL);
    pthread_cond_init(&shift_left_cond, NULL);
    pthread_cond_init(&sign_extend_cond, NULL);
    
    pthread_barrier_init(&update_registers, NULL, (unsigned int)NUM_THREADS_WITH_REGISTERS_TO_UPDATE);
    pthread_barrier_init(&current_cycle,NULL, (unsigned int)NUM_OF_THREADS_WHICH_USE_CSVALUE);

    
    pthread_t control_unit_thread;
    pthread_t program_counter_thread;
    pthread_t mux_2_iord_thread;
    pthread_t mux_2_regdst_thread;
    pthread_t mux_2_memtoreg_thread;
    pthread_t mux_2_alusrca_thread;
    pthread_t mux_3_pcsource_thread;
    pthread_t mux_4_alusrcb_thread;
    pthread_t main_memory_thread;
    pthread_t instruction_register_thread;
    pthread_t memory_data_register_thread;
    pthread_t register_bank_thread;
    pthread_t pc_shift_left_thread;
    pthread_t shift_left_after_se_thread;
    pthread_t sign_extend_thread;
    pthread_t alu_thread;
    pthread_t alu_control_thread;
    pthread_t aluout_register_thread;
    pthread_t a_thread;
    pthread_t b_thread;
    pthread_t and_or_thread;
    
    if (pthread_create(&control_unit_thread, 0, (void *) control_unit, (void *) 0) != 0) {
          printf("Error creating thread! Exiting! \n");
          exit(0);
    }
    if (pthread_create(&a_thread, 0, (void *) a, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&alu_control_thread, 0, (void *) alu_control, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&alu_thread, 0, (void *) alu, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&aluout_register_thread, 0, (void *) aluout_register, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&and_or_thread, 0, (void *) and_or, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&b_thread, 0, (void *) b, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&instruction_register_thread, 0, (void *) instruction_register, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&main_memory_thread, 0, (void *) main_memory, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&memory_data_register_thread, 0, (void *) memory_data_register, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&mux_2_alusrca_thread, 0, (void *) mux_2_alusrca, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&mux_2_iord_thread, 0, (void *) mux_2_iord, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&mux_2_memtoreg_thread, 0, (void *) mux_2_memtoreg, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&mux_2_regdst_thread, 0, (void *) mux_2_regdst, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&mux_3_pcsource_thread, 0, (void *) mux_3_pcsource, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&mux_4_alusrcb_thread, 0, (void *) mux_4_alusrcb, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&pc_shift_left_thread, 0, (void *) pc_shift_left, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&program_counter_thread, 0, (void *) program_counter, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&register_bank_thread, 0, (void *) register_bank, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&shift_left_after_se_thread, 0, (void *) shift_left_after_se, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    if (pthread_create(&sign_extend_thread, 0, (void *) sign_extend, (void *) 0) != 0) {
        printf("Error creating thread! Exiting! \n");
        exit(0);
    }
    
    while(ir) //Trocar instrução
    {
        cpu_clock = 0;
        pthread_barrier_wait(&update_registers);
        
        cpu_clock++;
        pthread_barrier_wait(&update_registers);
        
        /* Aqui o segundo ciclo já foi executado por completo. Precisamos saber qual o tipo de instrução para continuarmos executando */
        
        char opcode = ((ir & separa_cop) >> 26) & 0x3f;
        if (opcode == op_beq || opcode == op_jump) cycles = 2;
        else if (opcode == op_lw) cycles = 4;
        else cycles = 3; // caso do op_sw e op_r_type
        
        while(cpu_clock < cycles) /*nao*/
        {
            cpu_clock++;
            pthread_barrier_wait(&update_registers);
        }
    }
    //fim do while(loop)
    
    // impressao das posicoes de memoria que foram alteradas:
    printf("Posicoes de memoria alteradas:\n");
    for (i = 0; i < MEMORY_SIZE; i++)
    {
        if(memoria[i].isUpdated == 1)
            printf("memoria[%d]=%d \n", i, memoria[i].value);
    }
    
    // impressao do conteudo dos registradores
    printf("\nRegistradores:\n");
    for (i = 0; i < NUMREG; i++)
    {
        printf("reg[%d]=%d \n", i, reg[i]);
    }
    
    
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
    
    pthread_join(control_unit_thread, 0);
    pthread_join(program_counter_thread, 0);
    pthread_join(mux_2_iord_thread, 0);
    pthread_join(mux_2_regdst_thread, 0);
    pthread_join(mux_2_memtoreg_thread, 0);
    pthread_join(mux_2_alusrca_thread, 0);
    pthread_join(mux_3_pcsource_thread, 0);
    pthread_join(mux_4_alusrcb_thread, 0);
    pthread_join(main_memory_thread, 0);
    pthread_join(instruction_register_thread, 0);
    pthread_join(memory_data_register_thread, 0);
    pthread_join(register_bank_thread, 0);
    pthread_join(pc_shift_left_thread, 0);
    pthread_join(shift_left_after_se_thread, 0);
    pthread_join(sign_extend_thread, 0);
    pthread_join(alu_thread, 0);
    pthread_join(alu_control_thread, 0);
    pthread_join(aluout_register_thread, 0);
    pthread_join(a_thread, 0);
    pthread_join(b_thread, 0);
    pthread_join(and_or_thread, 0);
    
    pthread_barrier_destroy(&current_cycle);
    pthread_barrier_destroy(&update_registers);
    
    pthread_mutex_destroy(&control_sign);
    pthread_mutex_destroy(&alu_sign);
    pthread_mutex_destroy(&mux_alusrca_result);
    pthread_mutex_destroy(&mux_alusrca_result);
    pthread_mutex_destroy(&mux_iord_result);
    pthread_mutex_destroy(&mux_memtoreg_result);
    pthread_mutex_destroy(&mux_regdst_result);
    pthread_mutex_destroy(&mux_pcsource_result);
    pthread_mutex_destroy(&alu_zero_mutex);
    pthread_mutex_destroy(&or_result_mutex);
    pthread_mutex_destroy(&pc_shift_left_result);
    pthread_mutex_destroy(&alu_result_mutex);
    pthread_mutex_destroy(&pc_buffer);
    pthread_mutex_destroy(&shift_left_mutex);
    pthread_mutex_destroy(&sign_extend_mutex);
    
    pthread_cond_destroy(&control_sign_wait);
    pthread_cond_destroy(&alu_sign_wait);
    pthread_cond_destroy(&mux_alusrca_execution_wait);
    pthread_cond_destroy(&mux_alusrcb_execution_wait);
    pthread_cond_destroy(&mux_iord_execution_wait);
    pthread_cond_destroy(&mux_memtoreg_execution_wait);
    pthread_cond_destroy(&mux_regdst_execution_wait);
    pthread_cond_destroy(&mux_pcsource_execution_wait);
    pthread_cond_destroy(&alu_zero_wait);
    pthread_cond_destroy(&or_result_wait);
    pthread_cond_destroy(&pc_shift_left_execution_wait);
    pthread_cond_destroy(&alu_result_wait);
    pthread_cond_destroy(&pc_wait);
    pthread_cond_destroy(&shift_left_cond);
    pthread_cond_destroy(&sign_extend_cond);
    
    exit( 0);
} // fim de main





