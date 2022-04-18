

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>
#include "Reflexao.h"


#define	PULSAR			0xff
#define PRU0_BINARY 		"/usr/bin/Reflexao0.bin"
#define PRU1_BINARY 		"/usr/bin/Reflexao1.bin"
#define VP			4.1 // ns/m


/* PRU SHARED MEMORY (12kB) - MAPPING
 *
 * prudata[0] = pulse verify request PRU 0
 * prudata[1] = ||
 * prudata[2] = || Response
 * prudata[3] = || Time PRU 0
 * prudata[4] = ||
 *
 * prudata[5] = pulse verify request PRU 1
 * prudata[6] = ||
 * prudata[7] = || Response
 * prudata[8] = || Time PRU 1
 * prudata[9] = ||
 *
 */


volatile uint8_t* prudata;

void close_PRU();
int init_start_PRU();
void pulsar_PRU(float *Distance_m);


void close_PRU(){
	// ----- Desabilita PRU e fecha mapeamento da shared RAM
	prussdrv_pru_disable(0);
	prussdrv_pru_disable(1);
	prussdrv_exit();
}


int init_start_PRU(){

	tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

	// ----- Inicializacao da PRU
	prussdrv_init();

	// ----- Inicializacao da interrupcao PRU
	if (prussdrv_open(PRU_EVTOUT_0))
		return -1; 			// prussdrv_open open failed
	
	prussdrv_pruintc_init(&pruss_intc_initdata);

	// ----- Mapeamento Shared RAM
	prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, (void**)&prudata);
	prudata[0] = 0;
	prudata[5] = 0;

	// ----- Executar codigo na PRU
	prussdrv_exec_program (0, PRU0_BINARY);
	prussdrv_exec_program (1, PRU1_BINARY);

	return 0;
}


void pulsar_PRU(float *Distance_m){

	uint32_t NumberOfInstructions[2];
	int i = 0,PropagationTime_ns[2];

	// ----- Sinaliza inicio
	prudata[0] = PULSAR;
	prudata[5] = PULSAR;

	
	// ----- Aguarda sinal de finalizacao do ciclo
	while(prudata[5]==PULSAR && i<30){
		sleep(0.1);
		i++;	
	}

	i=0;
	while(prudata[0]==PULSAR && i<30){
		sleep(0.1);
		i++;	
	}


	for(i=0; i<2; i++){
		// ----- Timeout: Limpa os dados da memoria e reinicializa firmware
		if(prudata[5*i]){
			prudata[5*i] = 0;
			prudata[(5*i)+1] = 0;
			prudata[(5*i)+2] = 0;
			prudata[(5*i)+3] = 0;
			prudata[(5*i)+4] = 0;
			if(i){
				prussdrv_exec_program (i, PRU1_BINARY);
			}
			else{
				prussdrv_exec_program (i, PRU0_BINARY);
			}
		}

		// ----- Calculo do tempo de propagacao
		NumberOfInstructions[i] = (prudata[(5*i)+4] << 24) + (prudata[(5*i)+3] << 16) + (prudata[(5*i)+2] << 8) + (prudata[(5*i)+1]);
		PropagationTime_ns[i] = (int) NumberOfInstructions[i]*5;

		// ----- Compensacao de atrasos intrinsecos ao hardware
		if(PropagationTime_ns[i] != 0)
			PropagationTime_ns[i] -= 35;

		// ----- Calculo da distancia ate o problema
		Distance_m[i] = PropagationTime_ns[i]/(2*VP);
	}
	
	return;
}


