#include <stdio.h>
/* 	O Mux precisa do sinal de controle para decidir qual dado usará.
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

///////// VARIAVEIS GLOBAIS: COLOCAR NA MAIN //////////
// -> sc ja esta na main
// pc
// aluout
// mdr
// IR
// A
// B
// -> alu_result
// -> jump_address
// FOUR = 0x00000004
// BEQ_Address

//////////////////threads faltando////////////////
// shift left2 (as duas [em um deles deve ter concatenacao com 4 bits de PC])
// sign extend
// ALU
// A (deve ter uma thread para escrever aqui)
// B (deve ter uma thread para escrever aqui)
// AND e OR
// MDR (deve ter uma thread para escrever aqui)
// PC (deve ter uma thread para escrever aqui)
// Memory
// Banco de Registradores
// IR
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


