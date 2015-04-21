/* SSC0640 - Sistemas Operacionais I
* Trabalho 1 - CPU MIPS com pthreads em C
* Data: 22/04/2015
***************************************************************
* Integrantes do Grupo:
* Guilherme Nishina Fortes 			- No. USP 7245552
* Henrique de Almeida Machado da Silveira 	- No. USP 7961089
* Marcello de Paula Ferreira Costa 		- No. USP 7960690
* Sergio Yudi Takeda 				- No. USP 7572996
***************************************************************
* GRUPO 13
*/

Comentários gerais sobre blocos empregados em todas as unidades lógicas do simulador.

// Bloco 1
	pthread_mutex_lock(&control_sign);
	if(!cs.isUpdated){
		while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
	}
	pthread_mutex_unlock(&control_sign);

O bloco acima implementa o comportamento de espera pelo sinal de controle. É sabido que quando a unidade de controle tiver o sinal pronto, ela será responsável por atribuir à variável cs.isUpdated o valor 1, e com essa lógica, todas threads só esperarão pela variável de condição se necessário.

// Bloco 2
	pthread_mutex_lock(&control_sign);			/* Adquire permissão para entrar na região crítica */
	cs.isUpdated = 1;					/* Seta o valor de cs.isUpdated */
	pthread_cond_broadcast(&control_sign_wait);         	/* "Acorda" todas as threads que estão
								"dormindo" nessa variável */
	pthread_mutex_unlock(&control_sign);		/* Abdica o controle da região crítica */

            	O par de blocos exemplificado acima é usado para garantir semântica e sequência de execução de várias unidades da CPU MIPS quando necessário. Por exemplo, um multiplexador que precisa esperar por um resultado proveniente da ULA precisa implementar a mesma lógica. Ou seja, deverá usar o bloco que implementa o comportamento de espera e a unidade responsável pelo envio do sinal (no caso do exemplo seria a ULA) deve implementar o comportamento de envio de sinal. Vale ressaltar que as variáveis pthread_mutex_t e pthread_cond_t serão específicas para cada situação em que o padrão for empregado.






// Bloco 3
Na unidade de controle ocorre a verificação a seguir:
	if( !validInstruction ){
		loop = 0;
		cs.invalidInstruction = 1;
	}
Previamente a essa verificação, a unidade de controle faz a verificação e determina se a instrução contida no registrador IR é válida, ou seja, se é uma das 9 instruções implementadas no simulador em questão (add, sub, and, or, slt, lw, sw, beq, j) ou se o conteúdo de IR é 0x00000000, valor que indica o fim de programa. Caso uma das duas situações mencionadas sejam satisfeitas, o valor de validInstruction será 0. Então, será atribuído o valor 0 à loop para impedir que a thread principal inicie qualquer outro ciclo e o valor 1 será atribuído à cs.invalidInstruction para que esse valor, componente da estrutura do sinal de controle (c_sign), seja usado por outras threads por meio da rotina descrita no Bloco 4.

// Bloco 4
	if(cs.invalidInstruction){
		pthread_barrier_wait(&update_registers);
	pthread_exit(0);
	}

Continuando com o tratamento de erros, cs.invalidInstruction será 1 se houver uma instrução invalida. A thread será interrompida por meio do comando pthread_exit e por consequência o programa será encerrado. Da maneira como está implementado, todas as threads com exceção da main serão fechadas quando uma instrução inválida ou fim de programa estiver em IR. A saída com conteúdo dos registradores e conteúdo das posições de memória alterados serão impressos em qualquer caso. É importante notar que a chamada à pthread_barrier_wait() é necessária pois a barreira já foi alcançada pela função main.


// Bloco 5
	pthread_barrier_wait(&threads_creation);

Espera criação de todas as threads do processador para evitar erros.


// Bloco 6
	pthread_barrier_wait(&current_cycle);  	 
	a_value = read_data_1;         	 
	pthread_barrier_wait(&update_registers);    

O bloco acima usa o funcionamento das barreiras para implementar o comportamento de existir somente uma execução de cada unidade funcional em um ciclo de clock e também para simular a atualização de registradores somente ao final do ciclo, ou melhor, no tique de clock. É importante dizer que a todas as flags x.isUpdated, onde x é uma estrutura qualquer, será atribuído o valor zero. Essa atualização das flags serve justamente para que a lógica empregada nos Bloco 1 e 2 façam algum sentido.
