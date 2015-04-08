#include <stdio.h>
/*  O Mux precisa do sinal de controle para decidir qual dado usará.
  Consequentemente, o Mux precisa de só um buffer de entrada.  
*/

//////////////// VARIAVEIS GLOBAIS: BUFFER DE MUXES /////////
int mux_IorD_buffer;
int mux_PCSource_buffer;
int mux_RegDst_buffer;
int mux_MemtoReg_buffer;
int mux_ALUSrcA_buffer;
int mux_ALUSrcB_buffer;
//////////////// FIM VARIAVEIS GLOBAIS: BUFFER DE MUXES /////////


//// TEMOS QUE PENSAR EM UM JEITO DE ENCERRAR AS THREADS QUANDO O
//// PROGRAMA NA MEMORIA CHEGA AO FIM. OU SEJA: TEMOS QUE INCLUIR
//// WHILES NAS THREADS PARA QUE ELAS CONTINUEM EXECUTANDO ATE QUE
//// UMA CONDICAO DE PARADA SEJA VERIFICADA. PODEMOS FAZER ISSO USANDO
//// UMA VARIAVEK GLOBAL QUE COMECA SENDO VERDADEIRA E VIRA FALSA QUANDO
//// UMA INSTRUCAO NULA EH ENCONTRADA NA MEMORIA, INDICANDO O FIM DO 
//// PROGRAMA. ESSA VARIAVEL EH CONSTANTEMENTE CHECADA PELAS THREADS

///////// VARIAVEIS GLOBAIS: COLOCAR NA MAIN //////////
// -> sc ja esta na main
// int pc
// aluout
// mdr
// IR
// int A
// int B
// -> alu_result
// -> jump_address
// FOUR = 0x00000004
// BEQ_Address
// memory_content_read
// read_data_1
// read_data_2

//////////////////threads faltando////////////////
// shift left2 (as duas [em um deles deve ter concatenacao com 4 bits de PC])
// sign extend
// ALU
// OK--------> A (deve ter uma thread para escrever aqui)
// OK--------> B (deve ter uma thread para escrever aqui)
// AND e OR
// OK--------> MDR (deve ter uma thread para escrever aqui)
// kinda ok -> PC (deve ter o sinal de controle de E_OU)
// OK--------> Memory
// Banco de Registradores
// OK--------> IR
// ALUout
// ALUcontrol

/////////////////////////////////////////////////////

enum positions {
    RegDst_POS = 0,
    RegWrite_POS,
    ALUSrcA_POS,
    ALUSrcB0_POS,
    ALUSrcB1_POS,
    ALUOp0_POS,
    ALUOp1_POS,
    PCSource0_POS,
    PCSource1_POS,
    PCWriteCond_POS,
    PCWrite_POS,
    IorD_POS,
    MemRead_POS,
    MemWrite_POS,
    MemtoReg_POS,
    IRWrite_POS
};

/*	**** A seguinte estrutura terá de ser aplicada nas threads que esperarão pelo sinal da unidade de controle:

	pthread_mutex_lock(&mutex)
	while(!condition) {
		pthread_cond_wait(&condition_t,&mutex) >> Imediatamente antes de dormir, libera o mutex!  <<
	}
	>> Realiza tarefas necessários depois de ter obtido o sinal verdadeiro de condição <<
	>> É implicito que quando sair do loop while, o mutex terá sido relocado <<

	pthread_mutex_unlock(&mutex)





	**** A seguinte estrutura deverá ser aplicada na unidade de controle:

	pthread_mutex_lock(&mutex)
	>> Realiza alguma coisa que poderá liberar a condição necessária para outras acordarem  <<

	pthread_cond_signal()
	pthread_mutex_unlock (&mutex)

	// Do código acima é importante notar que mesmo a unidade de controle tendo liberado a flag da
	// variável de condição, ele ainda não liberou o mutex. Logo em seguida libera.
	
 */

void banco_registradores ()
{
  int read_register_1, read_register_2, write_register, write_data;
    read_register_1 = ((separa_rs & IR) >> 21) & 0x0000003f;
    read_register_2 = ((separa_rt & IR) >> 16) & 0x0000003f;
    read_data_1 = reg[read_register_1];
    read_data_2 = reg[read_register_2];
    write_register = mux_RegDst_buffer;
  write_data = mux_MemtoReg_buffer;
  
    if ((sc & separa_RegWrite) == ativa_RegWrite)
      reg[write_register] = write_data;
}



