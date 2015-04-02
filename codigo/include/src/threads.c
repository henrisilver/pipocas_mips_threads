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

void mux_2_IorD (){
  //mutex lock
    if(( (separa_IorD & sc) >> IorD_POS) & 0x01 == 0)
      mux_IorD_buffer = pc;
    else 
      mux_IorD_buffer = aluout;
}
void mux_2_MemtoReg(){
  //mutex lock
    if(( (separa_MemtoReg & sc) >> MemtoReg_POS) & 0x01 == 0)
      mux_MemtoReg_buffer = aluout;
    else 
      mux_MemtoReg_buffer = mdr;
}

void mux_2_RegDst (){
  //mutex lock
    if(( (separa_RegDst & sc) >> RegDst_POS) & 0x01 == 0)
      mux_RegDst_buffer = ((IR & separa_rt) >> 16) & 0x0000001f;
    else 
      mux_RegDst_buffer = ((IR & separa_rd) >> 11) & 0x0000001f;
}

void mux_2_ALUSrcA (){
  //mutex lock
    if(( (separa_ALUSrcA & sc) >> ALUSrcA_POS) & 0x01 == 0)
      mux_ALUSrcA_buffer = pc;
    else 
      mux_ALUSrcA_buffer = A;
}

void mux_3_PCSoure (){
  //mutex lock
    if((((separa_PCSource0 | separa_PCSource1) & sc) >> PCSource0_POS) & 0x03 == 0)
      mux_PCSource_buffer = alu_result;
    else if((((separa_PCSource0 | separa_PCSource1) & sc) >> PCSource0_POS) & 0x03 == 1)
      mux_PCSource_buffer = aluout;
    else
        mux_PCSource_buffer = jump_address;
}

void mux_4_ALUSrcB (){
  //mutex lock
  if((((separa_ALUSrcB0 | separa_ALUSrcB1) & sc) >> ALUSrcB0_POS) & 0x03 == 0)
    mux_ALUSrcB_buffer = B;
  else if((((separa_ALUSrcB0 | separa_ALUSrcB1) & sc) >> ALUSrcB0_POS) & 0x03 == 1)
      mux_ALUSrcB_buffer = FOUR;
    else if((((separa_ALUSrcB0 | separa_ALUSrcB1) & sc) >> ALUSrcB0_POS) & 0x03 == 2)
      mux_ALUSrcB_buffer = BEQ_Adress;
    else
        mux_ALUSrcB_buffer = BEQ_Adress << 2;  
}

void escreve_PC (){
  //mutex lock de PC e PCSource_buffer
  // INCLUIR CHECAGEM DO SINAL PARA VERIFICAR SE ESCREVE NO PC OU NAO
  // VAI SER NECESSARIO CRIAR UM NOVO SINAL RESULTANTE DOS SINAIS DE CONTROLE
  // PASSANDO PELAS PORTAS AND E OR
  // FAZER ISSO APOS IMPLEMENTAR A ULA, POIS O SINAL ZERO EH NECESSARIO
  pc = mux_PCSource_Buffer; 
}

void escreve_MDR (){
  //mutex lock de PC e PCSource_buffer
  mdr = memory_content_read;  
}

void escreve_IR (){
  if ((sc & separa_IRWrite) == ativa_IRWrite)
    IR = memory_content_read; 
}

void escreve_A (){
  //mutex lock
  A = read_data_1;  
}

void escreve_B (){
  //mutex lock
  B = read_data_2;  
}

void memory_access ()
{
    if ((separa_MemRead & sc) == ativa_MemRead)  
    memory_content_read = memoria[mux_IorD_buffer/4];
  
    if ((sc & separa_MemWrite) == ativa_MemWrite)
      memoria[mux_IorD_buffer/4] = B;
}

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



