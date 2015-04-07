/*
    File with implementation of ALU routine.
    It receives a control parameter from the ALU control unit which
    indicates what kind of operation the ALU should perform.
*/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
// FALTA: IMPLEMENTAR MUTEXES, BARREIRAS E VARIAVEIS DE CONDICAO
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
#ifndef _ALU_
#define _ALU_

#include <pthread.h>
#include "mascara.h"

extern int alu_result, mux_ALUSrcA_buffer, mux_ALUSrcB_buffer;
extern char zero, alu_control;
extern char alu_overflow;

void add(int *alu_result, int a, int b, char *overflow);
void sub(int *alu_result, int a, int b, char *overflow);
void and(int *alu_result, int a, int b);
void or(int *alu_result, int a, int b);
void slt(int *alu_result, int a, int b);
void somador_completo (char *result_op, char a, char b, char * c_out, char c_in);

// Op. Ar/Log ou calc. end. {
//     Tipo-R  ALU_A = A(regs), ALU_B = B(regs)               e ALU_control = operacao selecionada (pelo cfuncao)
//     LW/SW   ALU_A = A(regs), ALU_B = extsinal[IR[15...0]]  e ALU_control = ativa_soma
//     BEQ     ALU_A = A(regs), ALU_B = B(regs)               e ALU_control = ativa_subtracao (mas nosso interesse esta no zero)
//     J       ALU_A = xxx, ALU_B = xxx (don't care)          e ALU_control = xxx (don't care)
// }
//
// Demais casos                    ALU_A = xxx, ALU_B = xxx   e ALU_control = xxx (don't care)
// Argumentos da alu no trabalho de org: int a, int b, char alu_op, int *result_ula, char *zero, char *overflow
void alu (void * not_used) {
    
    // Caso que o codigo de operacao indica uma soma
    if (alu_control == ativa_soma)
    {
        add(&alu_result, mux_ALUSrcA_buffer, mux_ALUSrcB_buffer, &alu_overflow);
    }
    
    // Caso que o codigo de operacao indica uma subtracao
    else if (alu_control == ativa_subtracao)
    {
        sub(&alu_result, mux_ALUSrcA_buffer, mux_ALUSrcB_buffer, &alu_overflow);
    }
    
    // Caso que o codigo de operacao indica uma operacao and
    else if (alu_control == ativa_and)
    {
        and(&alu_result, mux_ALUSrcA_buffer, mux_ALUSrcB_buffer);
    }
    
    // Caso que o codigo de operacao indica uma operacao or
    else if (alu_control == ativa_or)
    {
        or(&alu_result, mux_ALUSrcA_buffer, mux_ALUSrcB_buffer);
    }
    
    // Caso que o codigo de operacao indica uma operacao slt
    else if (alu_control == ativa_slt)
    {
        slt(&alu_result, mux_ALUSrcA_buffer, mux_ALUSrcB_buffer);
    }
    
    // Se o resultado da ula for zero, o retorno *zero eh setado.
    if (alu_result == 0x00) zero = 0x01;
    else zero = 0x00;
    
    // O resultado da operacao da ula tambem eh retornado pela funcao so que nao
    // kkk zoeira mano essa funcao é void!!!1 lol
    //return *alu_result;
}

void somador_completo (char *result_op, char a, char b, char * c_out, char c_in)
{
    (*result_op) = ((a ^ b) ^ c_in);                 // Faz o XOR entre as entradas a, b e carry_in
    (*c_out) = ((a & b) | (a & c_in) | (b & c_in));  // Faz os AND's dois a dois entre as entradas e
    // um or entre os resultados, como na implementacao
    // do somador completo.
}

void add(int *result_op, int a, int b, char *overflow)
{
    char result_aux,         // Variavel de resultado para cada iteracao do somador completo
    c_out,              // carry_out do somador
    c_in_aux,
    c_in = 0x00;        // carry_in do somador, setado para zero (por ser uma soma)
    
    int i;                   // variavel de iteracao para a execucao do somador completo
    *result_op = 0x00000000;
    
    
    // O somador completo deve ser executado uma vez pra cada bit do inteiro. Ao todo, 31 vezes.
    for (i = 0; i < BITS_PER_BYTE * sizeof(int); i++)
    {
        // A funcao do somador completo eh chamada com as variaveis 'a' e 'b'. Eh aplicada uma mascara nas
        // variaveis, para que apenas o bit atual da iteracao seja mandado para o somador completo.
        somador_completo(&result_aux, ((a << (31 - i)) >> 31) & 1, ((b << (31 - i)) >> 31) & 1, &c_out, c_in);
        c_in_aux = c_in;  // Atualizacao do carry para a proxima iteracao
        c_in = c_out;
        
        *(result_op) = (*result_op) | ((int) (result_aux << i));   // A posicao correta do resultado recebe o calculo
        // do bit referente a aquela iteracao do somador completo
    }
    
    // Em complemento de 2, acontece overflow se o "vem 1" e o "vai um"
    // do bit mais significativos são diferentes! Ou seja, overflow
    // é o resultado do XOR entre o Cin (armazenado e c_in_aux) e o c_out
    // do bit mais significativo.
    *overflow = c_in_aux ^ c_out;
}

void sub(int *result_op, int a, int b, char *overflow)
{
    char result_aux,   // Toda a funcao de subtracao eh analoga a funcao de soma, a menos de o carry_in
    c_out,        // ser inicializado como 1 e a entrada 'b' ser negada.
    c_in_aux,
    c_in = 0x01;
    
    int i;
    *result_op = 0x00000000;
    
    // Como eh a subtracao entre 'a' e 'b' (result_op = a - b), fazemos a negacao dos bits da entrada b para
    // a chamada do somador completo.
    b = ~b;
    
    for (i = 0; i < BITS_PER_BYTE * sizeof(int); i++)
    {
        somador_completo(&result_aux, ((a << (31 - i)) >> 31) & 1, ((b << (31 - i)) >> 31) & 1, &c_out, c_in);
        c_in_aux = c_in;
        c_in = c_out;
        *(result_op) = (*result_op) | ((int) (result_aux << i));
    }
    
    // Em complemento de 2, acontece overflow se o "vem 1" e o "vai um"
    // do bit mais significativos são diferentes! Ou seja, overflow
    // é o resultado do XOR entre o Cin (armazenado e c_in_aux) e o c_out
    // do bit mais significativo.
    *overflow = c_in_aux ^ c_out;
}

void slt(int *result_op, int a, int b)
{
    //if(a<b)
    //  (*result_op) = 1;
    //else
    //  (*result_op) = 0;
    //
    // A funcao set on less than vai checar se o numero 'a' eh menor que o numero 'b'. Para
    // isso, eh feita a subtracao 'a' - 'b'. O bit mais significativo do resultado dessa
    // subtracao vai indicar o sinal desse resultado: se o bit for 1, o resultado eh negativo
    // e assim 'a' eh menor que 'b'. Caso contrario, se o bit for 0, o resultado eh positivo
    // (ou nulo) e 'a' nao eh menor que 'b'. Logo, fazendo a subtracao e avaliando o bit mais
    // significativo (e levando em conta se ha overflow ou nao na subtracao) eh possivel
    // implementar a funcao set on less than.
    //
    char result_aux,    // vai conter o bit mais significativo do resultado da subtracao
    c_out,              // Representa o carry_out do somador completo
    c_in = 0x01,        // Representa o carry_in. Inicializado com 1 para fazer a subtracao.
    c_in_aux,           // Auxiliar que recebe o valor de c_in antes de que ele seja atualizado
    overflow;           // Indica o overflow
    
    int i;              // Auxiliar para iteracoes
    
    // Como faremos a subtracao entre 'a' e 'b' (result_op = a - b),
    // fazemos a negacao dos bits da entrada b para
    // a chamada do somador completo.
    b = ~b;
    
    // Chama sucessivamente a funcao somador_completo para fazer a diferenca entre 'a' e 'b'.
    // Na ultima iteracao, result_aux vai conter o bit mais significativo do resultado.
    for (i = 0; i < BITS_PER_BYTE * sizeof(int); i++)
    {
        somador_completo(&result_aux, ((a << (31 - i)) >> 31) & 1, ((b << (31 - i)) >> 31) & 1, &c_out, c_in);
        c_in_aux = c_in;    // "Back up" de c_in
        c_in = c_out;       // Atualizacao de c_in
    }
    
    // Ocorre overflow se o bit de "vem 1" e o bit de "vai 1" sao diferentes,
    // ou seja, se c_in_aux != c_out
    overflow = c_in_aux ^ c_out;
    
    // O resultado da operacao deve ser o bit mais significativo da subtracao 'a' - 'b'
    // XORed com o bit de overflow.
    *result_op = (result_aux ^ overflow);
    
}

void and (int *result_op, int a, int b)
{
    /* O mesmo que *result_op = a & b; */
    
    int  i, /* Iterador para os bits */
    u, /* Guardara o isolamento de bits a partir de @a */
    v; /* Guardara o isolamento de bits a partir de @b */
    
    *result_op = 0x00000000;  /* Inicializacao do resultado */
    
    /* A partir daqui, temos que fazer o isolamento de cada bit dos operandos e
     entao a operacao bitwise AND, guardando o resultado em result_op */
    for (i = 0; i < BITS_PER_BYTE * sizeof(int); i++)
    {
        u = ((a << i) >> 31) & 1;   /* Isolamos o bit da vez de a na posicao menos significativa */
        v = ((b << i) >> 31) & 1;   /* Isolamos o bit da vez de b na posicao menos significativa */
        
        /* Para guardar o resultado, basta fazer uma operacao OR entre o resultado e @u e @v ANDed deslocados
         de volta para a posicao correta, o que setara os bits iguais a 1 no resultado */
        *result_op |= (u & v) << (31 - i);
    }
}

void or(int *result_op, int a, int b)
{
    /* O mesmo que *result_op = a | b; */
    
    int  i, /* Iterador para os bits */
    u, /* Guardara o isolamento de bits a partir de @a */
    v; /* Guardara o isolamento de bits a partir de @b */
    
    *result_op = 0x00000000;  /* Inicializacao do resultado */
    
    /* A partir daqui, temos que fazer o isolamento de cada bit dos operandos e
     entao a operacao bitwise AND, guardando o resultado em result_op */
    for (i = 0; i < BITS_PER_BYTE * sizeof(int); i++)
    {
        u = ((a << i) >> 31) & 1;   /* Isolamos o bit da vez de a na posicao menos significativa */
        v = ((b << i) >> 31) & 1;   /* Isolamos o bit da vez de b na posicao menos significativa */
        
        /* Para guardar o resultado, basta fazer uma operacao OR entre o resultado e @u e @v ANDed deslocados
         de volta para a posicao correta, o que setara os bits iguais a 1 no resultado */
        *result_op |= (u | v) << (31 - i);
    }
}

#endif