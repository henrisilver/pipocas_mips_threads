Comentarios referente as estruturas do codigo  

		pthread_mutex_lock(&control_sign);
		if(!cs.isUpdated){
                	while(pthread_cond_wait(&control_sign_wait,&control_sign) != 0);
		}
		pthread_mutex_unlock(&control_sign);
		
		   >> O bloco acima implementa o comportamento de espera pelo sinal de controle.
		   >> É sabido que quando a unidade de controle tiver o sinal pronto, ela será responsável por setar a variável 
		   cs.isUpdated com o valor 1, e com essa lógica, todas threads só esperarão pela variável de condição se necessário.


		pthread_barrier_wait(&current_cycle); 		
		a_value = read_data_1;				
		pthread_barrier_wait(&update_registers); 	
		
		  >> O bloco acima implementa a logica de Barreira para sincronizar as threads com o fim da execução do ciclo atual
		  e para simular o comportamento de atualização de registradores somente ao final do ciclo, ou seja, no tique de clock.
