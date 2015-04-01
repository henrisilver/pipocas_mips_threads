/*
	File with implementation of ux_2_IorD routine.
	It receives a control parameter from control unit to choose
	between PC's or ALUOut's adresses content to send to the main memory.
*/
#define PC 0
#define ALUOut 1

void mux_2_IorD(){

	int received = 0;
	while(!received){
		pthread_mutex_lock(&mutexIorD);
	      	if (pthread_cond_wait(&ciclo_num, &mutexIorD) == 0){
			received = 1;
		}
		
	      	pthread_mutex_unlock(&mutexIorD);
	}

      	if(controleMUX == 'PC'){
        	pthread_mutex_lock(&mutexPC); //fazer thread dormir se mutex ocupado?
	        pthread_cond_wait( &ciclo_num, &mutexPC );//verificar se estou no ciclo certo para leitura de pc
	        read(PC);//manda memory ler de PC
	        pthread_mutex_unlock(&mutexPC);
      
      	}
      
	else 	if(controleMUX == 'ALUout'){// 1
        
	        	pthread_mutex_lock(&mutexALUout);
		        pthread_cond_wait( &ciclo_num, &mutexALUout );//verificar se estou no ciclo certo para leitura de ALUout (funcao igual a de PC? Daria pra otimizar?)
		        read(ALUout);//manda memory ler de ALUout
		        pthread_mutex_unlock(&mutexALUout);
      		}
}
