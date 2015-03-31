/* Caso de teste de brinde, executa fibonacci usando nosso processador (suave)
 * http://pt.wikipedia.org/wiki/N%C3%BAmero_de_Fibonacci
 * Ver topico "Abordagem Iterativa"
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
                             //                                             ## fat(n) iterativo
    memoria[0] = 0x8db00000; // 1000 1101 1011 0000 0000 0000 0000 0000         lw $s0, 0($t5)       # carrega n da memoria
    memoria[1] = 0x11300005; // 0001 0001 0011 0000 0000 0000 0000 0101   loop: beq $t1, $s0, fim    # se der a cond. sai do loop
    memoria[2] = 0x01025020; // 0000 0001 0000 0010 0101 0000 0010 0000         add $t2, $t0, $v0    # t2 = t0 + v0)
    memoria[3] = 0x00404025; // 0000 0000 0100 0000 0100 0000 0010 0101         or $t0, $v0, $zero   # t0 = v0
    memoria[4] = 0x01431024; // 0000 0001 0100 0011 0001 0000 0010 0100         and $v0, $t2, $v1    # v0 = t2
    memoria[5] = 0x012b4820; // 0000 0001 0010 1011 0100 1000 0010 0000         add $t1, $t1, $t3    # t1 = t1 + 1
    memoria[6] = 0x08000001; // 0000 1000 0000 0000 0000 0000 0000 0001         j loop (palavra 1)   # volta pro loop
    memoria[7] = 0xad820000; // 1010 1101 1000 0010 0000 0000 0000 0000   fim:  sw $v0, 0($t4)       # salva o resultado na memoria
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
    
    while(loop)
    {
        // aqui comeca um novo ciclo
        
        // abaixo estao as unidades funcionais que executarao em todos os ciclos
        // os sinais de controle em sc impedirao/permitirao que a execucao seja, de fato, efetivada
        UnidadeControle(IR, &sc);
        Busca_Instrucao(sc, PC, ALUOUT, IR, &PCnew, &IRnew, &MDRnew);
        Decodifica_BuscaRegistrador(sc, IR, PC, A, B, &Anew, &Bnew, &ALUOUTnew);
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
        // aqui termina um ciclo
    } // fim do while(loop)
    
    // impressao da memoria para verificar se a implementacao esta correta
    {
       int ii;
       for (ii = 20; ii < 22; ii++) {
           printf("memoria[%d]=%ld \n", ii, memoria[ii]);
       }
       printf("Nr de ciclos executados =%d \n", nr_ciclos);
    }
    
    exit(0);
} // fim de main
