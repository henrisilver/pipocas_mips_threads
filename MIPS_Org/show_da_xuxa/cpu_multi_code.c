/*
 Universidade de Sao Paulo - USP
 Instituto de Ciencias Matematicas e de Computacao – ICMC 
 Departamento de Ciencias de Computacao – SCC
 SSC610 Organizacao de Computadores Digitais I
 2o Trabalho - Simulador de uma CPU MIPS Multiciclo de 32 bits

 Integrantes do grupo:                              Numero USP:
 
 ---> Adriano Belfort de Sousa                  -   7960706
 ---> Gabriel de Salles Ramos                   -   7126782
 ---> Henrique de Almeida Machado da Silveira   -   7961089
 ---> Rodrigo Almeida Bergamo Ferrari           -   8006421
 
 Engenharia de Computacao
 Data: 08/11/2013
 Professor: Paulo Sergio Souza
 
 */

#include <stdarg.h>

// Usadas na tabela de despacho 1:         OpCode      Saida (prox. estado)
#define RFormat1 0x06                   // 00 0000     0110
#define JUMP1    0x09                   // 00 0010     1001
#define BEQ1     0x08                   // 00 0100     1000
#define Mem1     0x02                   // 10 0011     0010
// 10 1011     0010

// Usadas na tabela de despacho 2:         OpCode      Saida
#define LW2      0x03                   // 10 0011     0011
#define SW2      0x05                   // 10 1011     0101


#define TRASH1 0x0babaca0
#define TRASH2 0xbabababe
#define FOUR 0x00000004

#define BITS_PER_BYTE 8


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

/****** Funcoes da ULA
/* Retorna a soma dos dois inteiros de entrada e o overflow
 add (int *result_op, int a, int b, char *overflow)
 args de entrada: int a, int b
 args de saida: int *result_op, char *overflow */
void add(int *, int, int, char *);

/* Retorna a subtracao de dois inteiros
 sub (int *result_op, int a, int b, char *overflow)
 args de entrada: int a, int b
 args de saida: int *result_op, char *overflow */
void sub(int *, int, int, char *);

/* Retorna a and de dois inteiros
 and(int *result_op, int a, int b)
 args de entrada: int a, int b
 args de saida: int *result_op */
void and(int *, int, int);

/* Retorna a or de dois inteiros
 or(int *result_op, int a, int b)
 args de entrada: int a, int b
 args de saida: int *result_op */
void or(int *, int, int);

/* Retorna a set_on_less_than entre dois inteiros
 slt(int *result_op, int a, int b)
 args de entrada: int a, int b
 args de saida: int *result_op */
void slt(int *, int, int);

/* Retorna a soma completa entre os bits 'a' e 'b' e o carry_in
 soma_completa(char *result_op, char a, char b, char c_in, char *c_out)
 args de entrada: char a, char b, char c_in
 args de saida: char *result_op, char *c_out */
void somador_completo (char *, char, char, char *, char);

/* Multiplexador Arbitrario
 void Mux (int *output, char control, ...);
 args de entrada: char control, ... (quantidade indeterminada de argumentos, todos inteiros)
 args de saida: *output */
void Mux(int *, char, ...);

/* Sign_extend
 void sign_extend(int *MDRnew, short int MDR)
 args de entrada: int MDR
 args de saida: int *MDRnew */
void sign_extend(int *, short int);

/* Calculo do endereco de Jump
 void Jump_adress_calculation (int *Jump_Adress, int endereco_jump, int PC)
 args de entrada: int endereco_jump, int PC
 args de saida: int *Jump_Adress */
void Jump_adress_calculation (int *, int, int);

/* Determinacao dos sinais de controle da ula
 void ALU_control_calc(char *ALU_control, char ALUOp, char cfuncao)
 args de entrada: char ALUOp, char cfuncao
 args de saida: char *ALU_control */
void ALU_control_calc(char *, char, char);

/* Funcao relativa a tabela de despacho 1
 void Dispatch_ROM_1 (char *output, char cop)
 args de entrada: char cop
 args de saida: char *output */
void Dispatch_ROM_1 (char *, char);

/* Funcao relativa a tabela de despacho 2
 void Dispatch_ROM_2 (char *output, char cop)
 args de entrada: char cop
 args de saida: char *output */
void Dispatch_ROM_2 (char *, char);

/* Funcao que realiza o sequenciamento do estado atual
 void Adder (char *output, char S)
 args de entrada: char S
 args de saida: char *output */
void Adder (char *, char);

/* Funcao responsavel pela logica de selecao de endereco para o controle de sequenciamento
 void Adress_Selection_Logic (char *output, char cop, char AddrCtl, char Adder_result)
 args de entrada: char cop, char AddrCtl, char Adder_result
 args de saida: char *output */
void Adress_Selection_Logic (char *, char, char, char);

/* Funcao responsavel pela ROM/PLA que implementa a logica combinacional de determinacao das proximas saidas
 void Microcode_Memory (int *output, char S)
 args de entrada: char cop, char AddrCtl, char Adder_result
 args de saida: char *output */
void Microcode_Memory (int *, char);

/****************************
 IMPLEMENTACAO DAS FUNCOES
 ****************************/

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
        u = ((a << i) >> 31) & 1;	/* Isolamos o bit da vez de a na posicao menos significativa */
        v = ((b << i) >> 31) & 1;	/* Isolamos o bit da vez de b na posicao menos significativa */
        
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
        u = ((a << i) >> 31) & 1;	/* Isolamos o bit da vez de a na posicao menos significativa */
        v = ((b << i) >> 31) & 1;	/* Isolamos o bit da vez de b na posicao menos significativa */
        
        /* Para guardar o resultado, basta fazer uma operacao OR entre o resultado e @u e @v ANDed deslocados
         de volta para a posicao correta, o que setara os bits iguais a 1 no resultado */
        *result_op |= (u | v) << (31 - i);
    }
}


/* ULA deve seguir a especificação dada em sala de aula.
 As funcoes necessarias a ULA devem estar implementadas em ula_code.c, cujo "#include" encontra-se logo abaixo
 int ula (int a, int b, char ula_op, int *result_ula, char *zero, char *overflow)
 args de entrada: int a, int b, char ula_op
 args de saida: int *result_ula, char *zero, char *overflow */
//
// Op. Ar/Log ou calc. end. {
//     Tipo-R  ALU_A = A(regs), ALU_B = B(regs)               e ALU_control = operacao selecionada (pelo cfuncao)
//     LW/SW   ALU_A = A(regs), ALU_B = extsinal[IR[15...0]]  e ALU_control = ativa_soma
//     BEQ     ALU_A = A(regs), ALU_B = B(regs)               e ALU_control = ativa_subtracao (mas nosso interesse esta no zero)
//     J       ALU_A = xxx, ALU_B = xxx (don't care)          e ALU_control = xxx (don't care)
// }
//
// Demais casos                    ALU_A = xxx, ALU_B = xxx   e ALU_control = xxx (don't care)
int ula (int a, int b, char ula_op, int *result_ula, char *zero, char *overflow) {
    
    // Caso que o codigo de operacao indica uma soma
    if (ula_op == ativa_soma)
    {
        add(result_ula, a, b, overflow);
    }
    
    // Caso que o codigo de operacao indica uma subtracao
    else if (ula_op == ativa_subtracao)
    {
        sub(result_ula, a, b, overflow);
    }
    
    // Caso que o codigo de operacao indica uma operacao and
    else if (ula_op == ativa_and)
    {
        and(result_ula, a, b);
    }
    
    // Caso que o codigo de operacao indica uma operacao or
    else if (ula_op == ativa_or)
    {
        or(result_ula, a, b);
    }
    
    // Caso que o codigo de operacao indica uma operacao slt
    else if (ula_op == ativa_slt)
    {
        slt(result_ula, a, b);
    }
    
    // Se o resultado da ula for zero, o retorno *zero eh setado.
    if (*result_ula == 0x00) *zero = 0x01;
    else *zero = 0x00;
    
    // O resultado da operacao da ula tambem eh retornado pela funcao
    return *result_ula;
}

// Realiza a extensao de sinal
void sign_extend(int *myint, short int mysint)
{
    register int i;                   // Variavel de contagem
    
    *myint = mysint & 0x0000ffff;       // Copia do conteudo de MDR (16 bits) para *MDRnew
    mysint = mysint & 0x00004000;      // Eh feita uma mascara para manter apenas o MSB de MDR
    
    // O MSB de MDR eh copiado para todos os bits seguintes de *MDRnew (extensao de sinal)
    for (i = 1; i < 18; i++) {
        *myint = *myint | (mysint << i);
    }
}

// Realiza a multiplexacao entre os sinais de entrada, baseado no controle (2o arg.)
void Mux (int *output, char control, ...)
{
    va_list args;
    va_start (args, control);
    
    if (control == 0x00) *output = va_arg (args, int);
    int a = va_arg(args, int);
    
    if (control == 0x01) *output = va_arg (args, int);
    a = va_arg(args, int);
    
    if (control == 0x02) *output = va_arg (args, int);
    a = va_arg(args, int);
    
    if (control == 0x03) *output = va_arg (args, int);
    
    va_end (args);
}

// Realiza o calculo do endereco de jump, da forma Jump_Adress = PC[31-28]:shift2(IR[25-0])
void Jump_adress_calculation (int *Jump_Adress, int IR, int PC)
{
    *Jump_Adress = PC & separa_4bits_PC;                             // Copia o conteudo de PC[31-28] e zera o restante dos bits
    *Jump_Adress = (*Jump_Adress) | ((IR << 2) & 0x0fffffff);        // Faz o shift de 2 bits e aplica uma mascara nos bits [31-28]                                                                       // e em seguida concatena com PC[31-28] na variavel de retorno.
}

// Recebe o campo de funcao e o sinal de controle ALUOp, e determina qual sera o sinal enviado a ula
void ALU_control_calc(char *ALU_control, char ALUOp, char cfuncao)
{
    if (ALUOp == 0x00)       // UC forca uma soma
        *ALU_control = ativa_soma;
    
    if (ALUOp == 0x01)       // UC forca uma subtracao
        *ALU_control = ativa_subtracao;
    
    if (ALUOp == 0x02)       // UC nao sabe o que fazer
    {
        cfuncao = cfuncao & zera_2bits_cfuncao;         // cfuncao    op. na ula
        if (cfuncao == 0x00)                            // 10 0000       add
            *ALU_control = ativa_soma;                   // 10 0010       sub
        if (cfuncao == 0x02)                            // 10 0100       and
            *ALU_control = ativa_subtracao;              // 10 0101       or
        if (cfuncao == 0x04)                            // 10 1010       slt
            *ALU_control = ativa_and;
        if (cfuncao == 0x05)
            *ALU_control = ativa_or;
        if (cfuncao == 0x0a)
            *ALU_control = ativa_slt;
    }
}

// Agrupamento do sinal de saida: bits [15...0] = sinais de controle
//                                bits [17, 16] = AddrClt
void Microcode_Memory (int *output, char S)
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
}

void Adress_Selection_Logic (char *output, char cop, char AddrCtl, char Adder_result)
{
    char Desp1,
    Desp2;
    
    // Resultado das tabelas de despacho
    Dispatch_ROM_1 (&Desp1, cop);
    Dispatch_ROM_2 (&Desp2, cop);
    
    // Multiplexacao para a definicao do proximo estado:
    // (output, control, 0, Desp1, Desp2, Adder_result)
    Mux ((int*) output, AddrCtl, 0x00, Desp1, Desp2, Adder_result);
    
    // Para o caso do programa ter acabado de comecar, o estado inicial eh setado para o estado 0
    if (cop == 0x3f) *output = 0x00;
}

void Dispatch_ROM_1 (char *output, char cop)
{
    if      ((cop & separa_COp5) == 0x20) *output = Mem1;
    else if ((cop & separa_COp2) == 0x04) *output = BEQ1;
    else if ((cop & separa_COp1) == 0x02) *output = JUMP1;
    else if (cop == 0x00)                 *output = RFormat1;
}

void Dispatch_ROM_2 (char *output, char cop)
{
    if      ((cop & separa_COp3) == 0x00) *output = LW2;
    else if ((cop & separa_COp3) == 0x08) *output = SW2;
}

// Poderia-se usar a funcao add implementada. Mas o somador necessario aqui
// precisa somar apenas 4 bits, enquanto a funcao o faria para 32 bits.
// Esse somador eh o utilizado dentro da UC
void Adder (char *output, char S)
{
    *output = S + 1;
}

// UC principal
// A variavel de estado da UC (char S) eh declarada globalmente
void UnidadeControle(int IR, short int *sc)
{
    static char S;
    
    char cop = ((IR & separa_cop) >> 26) & 0x3f;  // Variavel que contem o campo de operacao
    
    int output_signals = 0x00000000;              // Bits [15...0] = sc; bits[17,16] = AddrCtl
    
    char Adder_result;                            // Variavel que recebera o estado em sequencia ao anterior
    char AddrCtrl;                                 // Controle de sequenciamento
    
    // Condicao de encerramento do programa: IR == 0x00000000
    if (IR == 0x00000000)
    {
        loop = 0;
        return;
    }
    
    // Definicao da saida, considerando o estado atual. Estamos interessados apenas em AddrCtl,
    // para que possamos fazer o controle de selecao do proximo endereco.
    // (output_signals, S)
    Microcode_Memory (&output_signals, S);
    AddrCtrl = (output_signals & separa_AddrCtrl) >> 16;
    
    // Sequenciamento do estado
    Adder (&Adder_result, S);
    
    // Logica de selecao do proximo endereco (e definicao do proximo estado, armazenado na mesma variavel S)
    Adress_Selection_Logic (&S, cop, AddrCtrl, Adder_result);
    
    // Definicao dos proximos sinais de saida
    Microcode_Memory (&output_signals, S);
    
    // Sinais de controle que irao para o restante do processador
    *sc = output_signals & 0x0000ffff;
}

void Busca_Instrucao(short int sc, int PC, int ALUOUT, int IR, int *PCnew, int *IRnew, int *MDRnew)
{
    int Adress,
    J_Adress,
    BEQ_Adress;
    int ALU_A,
    ALU_B,
    ALU_result;
    char zero,
    overflow,
    ALU_control;
    
    
    // Extensao de sinal usada para o calculo especulativo do endereco de beq (mesmo que nao usado nesse ciclo)
    // Extensao de sinal: (output_32bits, input_16bits)
    sign_extend (&BEQ_Adress, IR & separa_imediato);
    
    // Calculo do endereÁo de jump para a entrada no multiplexador que alimenta PC (tambem nao usado nesse ciclo)
    // (endereco_calculado, IR[25...0], PC)
    Jump_adress_calculation (&J_Adress, IR & separa_endereco_jump, PC);
    
    //Instrucoes necessarias para a leitura da memoria na posicao de PC
    {
        // Multiplexacao da entrada do endereco na memoria:(output, controle, PC, ALUOUT)
        Mux (&Adress, ((separa_IorD & sc) >> IorD_POS) & separa_bitmenos_sig, PC, ALUOUT);
        
        // Se a leitura estiver habilitada, estamos no ciclo de busca de instrucao e o conteudo de IR e MDR sao atualizados
        if ((separa_MemRead & sc) == ativa_MemRead)
        {
            // Para acessar a memoria, como o enderecamento de PC eh a byte, e o enderecamento
            // do vetor de tipo int eh a 4 bytes, dividimos o valor do endereco por 4 para acessar
            // o vetor. Todos os acessos a memoria sao feitos semelhantemente.
            *MDRnew = memoria[Adress/4];
            
            // Fazemos tambem a verificacao adicional do sinal IRWrite
            if ((sc & separa_IRWrite) == ativa_IRWrite)
                *IRnew = memoria[Adress/4];
        }
    } // fim da leitura da memoria
    
    // Instrucoes necessarias para o incremento de PC:
    {
        // Execucao da operacao da ULA e afins (multiplexacao de entrada e controle da ULA)
        {
            // Multiplexacao das entradas:
            // (output, controle, PC, A)
            Mux(&ALU_A, ((separa_ALUSrcA & sc) >> ALUSrcA_POS) & separa_bitmenos_sig, PC, TRASH1);
            // (output, controle, B, 0x04, end_beq, end_beq << 2)
            Mux(&ALU_B, (((separa_ALUSrcB0 | separa_ALUSrcB1) & sc) >> ALUSrcB0_POS) & 0x03, TRASH2, FOUR, BEQ_Adress, BEQ_Adress << 2);
            
            // Calculo do sinal controle que sera enviado para a ULA (ALU_control, IR_cfuncao, UC_control)
            ALU_control_calc(&ALU_control, (((separa_ALUOp0 | separa_ALUOp1) & sc) >> ALUOp0_POS) & 0x03, (IR & separa_cfuncao));
            
            // Execucao da instrucao da ULA:
            // No ciclo de busca de instrucao, a UC forca uma soma em ALU_control,
            // e sao escolhidas as entradas 0 (PC) e 1 (4), fazendo o resultado ALU_result = PC + 4;
            ula(ALU_A, ALU_B, ALU_control, &ALU_result, &zero, &overflow);
        } // Fim da execucao da ULA
        
        // Multiplexacao e execucao da escrita em PC
        {
            // O multiplexador opera sempre, independente da escrita ser liberada ou nao.
            // Entradas: (&PC, controle, ALU_result, ALUOUT, end_jump).
            // Obs: PC eh usado como uma variavel local auxiliar. Pc so sera atualizado se *PCnew receber algum valor
            Mux (&PC, (((separa_PCSource0 | separa_PCSource1) & sc) >> PCSource0_POS) & 0x03, ALU_result, ALUOUT, J_Adress);
            
            // Escrita em PC, determinada pelo sinal de controle PCWrite
            if (((separa_PCWrite & sc) == ativa_PCWrite) ||  // porta logica OU
                (((separa_PCWriteCond & sc) == ativa_PCWriteCond) && zero)) // porta logica AND
                *PCnew = PC;
        } // fim das op. p/ escrita em PC
    } // fim das instrucoes p/ incremento de PC
}

// Decodifica Instrucao, Busca Registradores e Calcula Endereco para beq
void Decodifica_BuscaRegistrador(short int sc, int IR, int PC, int A, int B, int *Anew, int *Bnew, int *ALUOUTnew)
{
    int BEQ_Adress;
    int ALU_A,
    ALU_B,
    ALU_result;
    char zero,
    overflow,
    ALU_control;
    
    // Calculo especulativo do endereco de beq (usado nesse ciclo)
    // Extensao de sinal: (output_32bits, input_16bits)
    sign_extend (&BEQ_Adress, IR & separa_imediato);
    
    // Leitura dos Registradores
    {
        // Nao ha nenhum sinal de controle limitando a leitura dos registradores - ela eh sempre feita
        *Anew = reg[ ((separa_rs & IR) >> 21) & 0x0000003f ]; // A = bco_regs[IR[25...21]]
        *Bnew = reg[ ((separa_rt & IR) >> 16) & 0x0000003f ]; // B = bco_regs[IR[20...16]]
        //printf ("%d   %d\n", ((separa_rs & IR) >> 21) & 0x0000003f, ((separa_rt & IR) >> 16) & 0x0000003f);
    } // fim da leitura dos regs
    
    
    // Calculo do endereco especulativo para o beq: Operacoes da ula
    {
        // Multiplexacao das entradas:
        // (output, controle, PC, A)
        Mux(&ALU_A, ((separa_ALUSrcA & sc) >> ALUSrcA_POS) & separa_bitmenos_sig, PC, A);
        // (output, controle, B, 0x04, end_beq, end_beq << 2)
        Mux(&ALU_B, (((separa_ALUSrcB0 | separa_ALUSrcB1) & sc) >> ALUSrcB0_POS) & 0x03, B, FOUR, BEQ_Adress, BEQ_Adress << 2);
        
        // Calculo do sinal controle que sera enviado para a ULA (ALU_control, IR_cfuncao, UC_control)
        ALU_control_calc(&ALU_control, (((separa_ALUOp0 | separa_ALUOp1) & sc) >> ALUOp0_POS) & 0x03, (IR & separa_cfuncao));
        
        // No ciclo de calc. do endereco especulativo para o beq, a UC forca uma soma em ALU_control,
        // e sao escolhidas as entradas 0 (PC) e 3 (extSinal << 2), fazendo o resultado ALU_result = PC + shift2[extsinal[IR[0...15]]];
        // Execucao da instrucao da ULA: (ALU_A, ALU_B, ALU_control, &ALU_result, &zero, &overflow)
        ula(ALU_A, ALU_B, ALU_control, &ALU_result, &zero, &overflow);
        
        // Gravacao da saida da ULA em ALUOUT
        *ALUOUTnew = ALU_result;
    } // fim do calculo de end. p/ o beq
}

// Executa TipoR, Calcula endereco para lw/sw e efetiva desvio condicional e incondicional
void Execucao_CalcEnd_Desvio(short int sc, int A, int B, int IR, int PC, int ALUOUT, int *ALUOUTnew, int *PCnew)
{
    int J_Adress,
    BEQ_Adress;
    int ALU_A,
    ALU_B,
    ALU_result;
    char zero,
    overflow,
    ALU_control;
    
    // Calculo especulativo do endereco de beq (ja foi calculado na etapa anterior,
    // e se a instrucao for uma beq, esta salvo em ALUOUT)
    // Extensao de sinal: (output_32bits, input_16bits)
    sign_extend (&BEQ_Adress, IR & separa_imediato);
    
    // Calculo do endereÁo de jump (usado nesse ciclo)
    // (endereco_calculado, IR[25...0], PC)
    Jump_adress_calculation (&J_Adress, IR & separa_endereco_jump, PC);
    
    // Execucao das operacoes que envolvem a ULA
    {
        // Multiplexacao das entradas:
        // (output, controle, PC, A)
        Mux(&ALU_A, ((separa_ALUSrcA & sc) >> ALUSrcA_POS) & separa_bitmenos_sig, PC, A);
        // (output, controle, B, 0x04, end_beq, end_beq << 2)
        Mux(&ALU_B, (((separa_ALUSrcB0 | separa_ALUSrcB1) & sc) >> ALUSrcB0_POS) & 0x03, B, FOUR, BEQ_Adress, BEQ_Adress << 2);
        
        // Calculo do sinal controle que sera enviado para a ULA
        // (&output, ALUOp, cfuncao)
        ALU_control_calc(&ALU_control, (((separa_ALUOp0 | separa_ALUOp1) & sc) >> ALUOp0_POS) & 0x03, (IR & separa_cfuncao));
        
        // Ha 4 possibilidades se estivermos nesse ciclo:
        // Tipo-R              ALUOut = A op B
        // LW/SW               ALUOut = A + extsinal[IR[15...0]]
        // BEQ                 ALUOut = A - B, mas estamos interessados no bit zero da ULA, nesse caso
        // J                   ALUOut = xxx (don't care)
        // Execucao da instrucao da ULA: (ALU_A, ALU_B, ALU_control, &ALU_result, &zero, &overflow)
        ula(ALU_A, ALU_B, ALU_control, &ALU_result, &zero, &overflow);
        
        // Gravacao da saida da ULA em ALUOUT
        *ALUOUTnew = ALU_result;
    } // fim das op. da ula
    
    // Multiplexacao e execucao da escrita em PC
    {
        // Procedimento analogo a busca_instrucao. Mas agora, a escrita sera liberada apenas se o sinal 'zero' e 'PCWriteCond'
        // estiverem ativos (no caso do beq), ou se PCWrite estiver ativo (no caso do jump)
        // (output, controle, ALU_result, ALUOut, end_jump).
        Mux (&PC, (((separa_PCSource0 | separa_PCSource1) & sc) >> PCSource0_POS) & 0x03, ALU_result, ALUOUT, J_Adress);
        
        
        // Condicoes p/ escrita em PC (descritas acima)
        if (((separa_PCWrite & sc) == ativa_PCWrite) || (((separa_PCWriteCond & sc) == ativa_PCWriteCond) && zero))
            *PCnew = PC;
        
    } // fim das op. p/ escrita em PC
}

// Escreve no Bco de Regs resultado TiporR, Le memoria em lw e escreve na memoria em sw
void EscreveTipoR_AcessaMemoria(short int sc, int B, int IR, int ALUOUT, int PC, int *MDRnew, int *IRnew)
{
    int RegDst,
    Write_data,
    Adress;
    
    // Escrita no banco de registradores (instrucao Tipo-R)
    {
        // Multiplexacao do registrador de escrita: (output, controle, IR[20...16](rt), IR[15...11](rd))
        Mux(&RegDst, ((sc & separa_RegDst) >> RegDst_POS) & separa_bitmenos_sig, ((IR & separa_rt) >> 16) & 0x0000001f, ((IR & separa_rd) >> 11) & 0x0000001f);
        
        // Multiplexacao do conteudo de escrita: (output, controle, ALUOut, MDR)
        Mux (&Write_data, ((sc & separa_MemtoReg) >> MemtoReg_POS) & separa_bitmenos_sig, ALUOUT, TRASH1);
        
        // A escrita no banco de registradores eh controlada pela UC
        if ((sc & separa_RegWrite) == ativa_RegWrite)
            reg[RegDst] = Write_data;
    } // fim da escrita no bco. regs.
    
    // Operacoes na memoria
    {
        // Multiplexacao da entrada do endereco na memoria:(output, controle, PC, ALUOUT)
        Mux (&Adress, ((sc & separa_IorD) >> IorD_POS) & separa_bitmenos_sig, PC, ALUOUT);
        
        // Escrita (instrucao SW)
        {
            // Se a escrita estiver liberada, MEM[ALUOUT] = B
            if ((sc & separa_MemWrite) == ativa_MemWrite)
                memoria[Adress/4] = B;
        } // fim da leitura
        
        // Leitura (instrucao LW)
        {
            // Se a leitura estiver liberada, MDR = MEM[ALUOUT]
            if ((sc & separa_MemRead) == ativa_MemRead)
            {
                *MDRnew = memoria[Adress/4];
                
                // Nesse ciclo, esperamod que IR nao seja atualizado. Verificamos isso atraves do sinal IRWrite
                if ((sc & separa_IRWrite) == ativa_IRWrite)
                    *IRnew = memoria[Adress/4];
            }
            
            
        } // fim da escrita
    } // fim das operacoes na memoria
}

// Escreve no Bco de Regs o resultado da leitura da memoria feita por lw
void EscreveRefMem(short int sc, int IR, int MDR, int ALUOUT)
{
    int Write_data,
    RegDst;
    
    // Escrita no banco de registradores (instrucao LW)
    {
        // Multiplexacao do registrador de escrita: (output, controle, IR[20...16](rt), IR[15...11](rd))
        Mux(&RegDst, ((sc & separa_RegDst) >> RegDst_POS) & separa_bitmenos_sig, ((IR & separa_rt) >> 16) & 0x0000001f, ((IR & separa_rd) >> 11) & 0x0000001f);
        
        // Multiplexacao do conteudo de escrita: (output, controle, ALUOut, MDR)
        Mux (&Write_data, ((sc & separa_MemtoReg) >> MemtoReg_POS) & separa_bitmenos_sig, ALUOUT, MDR);
        
        
        // A escrita no banco de registradores eh controlada pela UC
        if ((separa_RegWrite & sc) == ativa_RegWrite)
            reg[RegDst] = Write_data;
    } // fim da escrita no bco. regs.
}

