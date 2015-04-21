/* main contem os tiques de clock conforme especificado
 * as declaracoes de threads, e mutexes, assim como as barriers e variaveis globais
 * se encontram na main para evitar possiveis conflitos.
 * as threads ao serem invocadas contem uma barreira para esperar todas terminarem de ser criadas
 * eliminando assim acessos a variaveis que para o programa nao existiriam e sinais perdidos de
 * sleep e wake up entre threads 
 * existem ainda variaveis globais alem daquelas descritas na especificacao do trabalho
 * elas foram usadas para ajudar na troca de dados entre cada unidade logica do processador simulado */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mascara.h"

#define NUM_THREADS_TO_UPDATE 22 //usado como condicao de espera para barreiras (sincroniza as threads)
#define NUM_THREADS_TO_EXECUTE 21
#define NUM_THREADS_TO_CREATE 21


/****** VARIAVEIS GLOBAIS ***********/
int pc = 0, aluout, mdr, ir = -1, a_value, b_value, jump_address, BEQ_Address, mem_data, read_data_1, read_data_2, cpu_clock = 1, aluout;
int reg[NUMREG];

char zero, alu_overflow;
int loop = 1;

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

//pthread conds abaixo fazem threads esperarem o resultado sair/ser atualizado para assim poderem consumi-lo corretamente
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
pthread_barrier_t threads_creation;

int main (int argc, char *argv[])
{
    	int i;
    	for(i = 0; i < MEMORY_SIZE; i++)
        	memoria[i].isUpdated = 0;//memoria[i].is updated indica se a memoria foi alterada. 0 nao, 1 sim
    	cs.isUpdated = 0;//is updated indica se o dado esta pronto para consumo. 0 nao, 1 sim.  cs eh a variavel de sinal de controle
		cs.invalidInstruction = 0;//valid instruction verifica se a instrucao recebida eh uma dentre as especificadas do mips
    	alu_zero.isUpdated = 0;
    	alu_result.isUpdated = 0;
    	mux_alusrca_buffer.isUpdated = 0;
    	mux_iord_buffer.isUpdated = 0;
    	mux_alusrca_buffer.isUpdated = 0;
    	mux_alusrcb_buffer.isUpdated = 0;
    	mux_memtoreg_buffer.isUpdated = 0;
    	mux_regdst_buffer.isUpdated = 0;
    	mux_pcsource_buffer.isUpdated = 0;
    	or_result.isUpdated = 0;
    	pc_shift_left_buffer.isUpdated = 0;
    	shift_left.isUpdated = 0;
    	se.isUpdated = 0;
    	alu_s.isUpdated = 0;

		/* Código na memória */
		memoria[0].value = 0x8c480000; // 1000 1100 0100 1000 0000 0000 0000 0000 lw $t0, 0($v0) 5
		memoria[1].value = 0x010c182a; // 0000 0001 0000 1100 0001 1000 00101010 slt $v1, $t0, $t4 4
		memoria[2].value = 0x106d0004; // 0001 0000 0110 1101 0000 0000 0000 0100 beq $v1, $t5, fim(4 palavras abaixo de PC+4) 3
		memoria[3].value = 0x01084020; // 0000 0001 0000 1000 0100 0000 0010 0000 add $t0, $t0, $t0
		memoria[4].value = 0xac480000; // 1010 1100 0100 1000 0000 0000 0000 0000 sw $t0, 0($v0)
		memoria[5].value = 0x004b1020; // 0000 0000 0100 1011 0001 0000 0010 0000 add $v0, $t3, $v0
		memoria[6].value = 0x08000000; // 0000 1000 0000 0000 0000 0000 0000 0000 j inicio (paavra 0) 3
		memoria[7].value = 0; 		// fim (criterio de parada do programa) (27*6)+(5+4+3)+1
		memoria[8].value = 0;
		memoria[9].value = 0;

    	/* Dados */
    	memoria[20].value = 10;
    	memoria[21].value = 12;
    	memoria[22].value = 14;
    	memoria[23].value = 16;
    	memoria[24].value = 18;
    	memoria[25].value = 20;
    	memoria[26].value = -1;

    	reg[2] = 80;
    	reg[11] = 4;
    	reg[12] = 0;
    	reg[13] = 1;
		
		//inicializacao de todos os mutex utilizados
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
		
		//inicializacao de todas as variaveis de condicao utilizadas
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
		
		//inicializacao das barreiras utilizadas
    	pthread_barrier_init(&update_registers, NULL, (unsigned int)NUM_THREADS_TO_UPDATE);//atualizacao dos registradores para leitura/escrita
    	pthread_barrier_init(&current_cycle, NULL, (unsigned int)NUM_THREADS_TO_EXECUTE);//espera o ciclo de execucao
    	pthread_barrier_init(&threads_creation, NULL, (unsigned int)NUM_THREADS_TO_CREATE);//espera as threads serem criadas

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
		
		//inicio da criacao das threads
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
    	//fim da criacao das threads

    	while(loop){//loop de execucao, apenas manda sinais de clock
        	if(cpu_clock == 1){
            		cpu_clock = 0;
        	}
        	else if(cpu_clock == 0){
            		cpu_clock = 1;
        	}
        	pthread_barrier_wait(&update_registers);
    	}
  	
    	printf("Posicões de memória alteradas:\n");
    	for (i = 0; i < MEMORY_SIZE; i++){
        	if(memoria[i].isUpdated == 1)
            		printf("Memória[%d] = %d\n", i, memoria[i].value);
    	}
	    
	    //imprime o conteudo dos registradores
    	printf("\nRegistradores:\n");
		printf("$zero = %d\n", reg[0]);
		printf("$at = %d\n", reg[1]);
		for (i = 2; i < 4; i++){
				printf("$v%d = %d\n", (i-2), reg[i]);
		}
		for (i = 4; i < 8; i++){
				printf("$a%d = %d\n", (i-4), reg[i]);
		}
		for (i = 8; i < 16; i++){
				printf("$t%d = %d\n", (i-8), reg[i]);
		}
		for (i = 16; i < 24; i++){
				printf("$s%d = %d\n", (i-16), reg[i]);
		}
		for (i = 24; i < 26; i++){
				printf("$t%d = %d\n", (i-16), reg[i]);
		}
		for (i = 26; i < 28; i++){
				printf("$k%d = %d\n", (i-26), reg[i]);
		}
		printf("$gp = %d\n", reg[28]);
		printf("$sp = %d\n", reg[29]);
		printf("$fp = %d\n", reg[30]);
		printf("$ra = %d\n", reg[31]);
		//fim da impressao dos registradores
		
    	pthread_barrier_destroy(&current_cycle);
    	pthread_barrier_destroy(&update_registers);
    	pthread_barrier_destroy(&threads_creation);
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
    	exit(0);
}
