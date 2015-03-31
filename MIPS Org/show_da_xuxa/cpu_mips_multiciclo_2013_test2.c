/* Caso de teste de brinde, faz umas contas very crazy ai
 * Copyright (c) Ferrari productions 
*/
#include <stdio.h>
#include <stdlib.h>

int main (int, char **);
int ula( int, int , char, int *, char *, char *);
void UnidadeControle(int, short int *);
void Busca_Instrucao(short int, int, int, int, int *, int *, int *);
void Decodifica_BuscaRegistrador(short int, int, int, int, int, int *, int *, int *);
void Execucao_CalcEnd_Desvio(short int, int, int, int, int, int, int *, int *);
void EscreveTipoR_AcessaMemoria(short int, int, int, int, int, int *, int *);
void EscreveRefMem(short int, int, int, int);

#include "mascara.h"

int memoria[MAX];         
int reg[NUMREG];   

char loop = 1;

#include "cpu_multi_code.c"

int main (int argc, char *argv[])
{
    int PCnew = 0, IRnew, MDRnew, Anew, Bnew, ALUOUTnew;
    int PC = 0, IR=-1, MDR, A, B, ALUOUT;
    short int sc = 0;   
    int nr_ciclos = 0;     
    
    
    // n = mem[10]
    //  v0 = 1
    //  para k de 1 ate n faca
    //       se (k == impar) v0 = v0 + k
    //  mem[11] = v0                                                     
    memoria[0] = 0x8db00008; //       lw $s0, 8($t5)       5
    memoria[1] = 0x0110182a; // loop: slt $v1, $t0, $s0    4
    memoria[2] = 0x10600005; //       beq $v1, $zero, fim  3
    memoria[3] = 0x010c4824; //       and $t1, $t0, $t4    4
    memoria[4] = 0x11200001; //       beq $t1, $zero, par  3
    memoria[5] = 0x00481020; //       add $v0, $v0, $t0    4
    memoria[6] = 0x010C4020; // par:  add $t0, $t0, $t4    4
    memoria[7] = 0x08000001; //       j loop               3
    memoria[8] = 0xad62fff4; // fim:  sw $v0, -12($t3)     4
    memoria[9] = 0;          // criterio de parada do programa   
    memoria[9] = 0;          // ciclos: 1+5+([n/2]*25 + ((n/2)-1)*21)+4+3+4+1
                             //    # arredond. pra baixo e pra cima, respec.
    
    // Dados
    memoria[10] = 10;        // memoria[11] ira receber a saida do programa
    
    reg[0] = 0;   // $zero
    
    reg[2] = 1;   // $v0
    
    reg[8] = 1;   // $t0
    reg[11] = 56; // $t3
    reg[12] = 1;  // $t4
    reg[13] = 32; // $t5
    
    while(loop)
    {
        UnidadeControle(IR, &sc);
        Busca_Instrucao(sc, PC, ALUOUT, IR, &PCnew, &IRnew, &MDRnew);
        Decodifica_BuscaRegistrador(sc, IR, PC, A, B, &Anew, &Bnew, &ALUOUTnew);
        Execucao_CalcEnd_Desvio(sc, A, B, IR, PC, ALUOUT, &ALUOUTnew, &PCnew);
        EscreveTipoR_AcessaMemoria(sc, B, IR, ALUOUT, PC, &MDRnew, &IRnew);
        EscreveRefMem(sc, IR, MDR, ALUOUT);
        
        nr_ciclos++;
            
        PC = PCnew;
        IR = IRnew;
        MDR = MDRnew;
        A = Anew;
        B = Bnew;
        ALUOUT = ALUOUTnew;
    }
    
    {
       int ii;
       for (ii = 10; ii < 12; ii++) {
           printf("memoria[%d]=%ld \n", ii, memoria[ii]);
       }
       printf("Nr de ciclos executados =%d \n", nr_ciclos);
    }
    
    exit(0);
}
