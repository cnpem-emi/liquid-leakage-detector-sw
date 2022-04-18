

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


#define PRU0		        0
#define PRU1		        1
#define	PULSAR			0xff
#define PRU0_BINARY 		"/usr/bin/Reflexao0.bin"
#define PRU1_BINARY 		"/usr/bin/Reflexao1.bin"
#define VP			4.1 // ns/m


/* PRU SHARED MEMORY (12kB) - MAPPING
 *
 * prudata[0] = pulse verify request - PRU0
 * prudata[1] = ||
 * prudata[2] = || Response
 * prudata[3] = || Time - PRU0
 * prudata[4] = ||
 *
 * prudata[5] = pulse verify request - PRU1
 * prudata[6] = ||
 * prudata[7] = || Response
 * prudata[8] = || Time - PRU1
 * prudata[9] = ||
 *
 */


volatile uint8_t* prudata;

void close_PRU();
int init_start_PRU();
float pulsar_PRU();


void close_PRU(){
	// ----- Desabilita PRU e fecha mapeamento da shared RAM
	prussdrv_pru_disable(PRU_NUM);
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
	prussdrv_exec_program (PRU_NUM, PRU_BINARY);

	return 0;
}


void pulsar_PRU(float *Distance_m){

	uint32_t NumberOfInstructions;
	int i = 0, PRU0_PropagationTime_ns, PRU1_PropagationTime_ns;


	// ----- Sinaliza inicio
	prudata[0] = PULSAR;

	// ----- Aguarda sinal de finalizacao do ciclo - PRU 0
	while((prudata[0]==PULSAR || prudata[5]==PULSAR) && i<30){
		sleep(0.1);
		i++;	
	}

	// ----- Timeout: Limpa os dados da memoria e reinicializa firmware
	if(prudata[0]){
		prudata[0] = 0;
		prudata[1] = 0;
		prudata[2] = 0;
		prudata[3] = 0;
		prudata[4] = 0;
	    prussdrv_exec_program (PRU0, PRU0_BINARY);
	}
	if(prudata[5]){
		prudata[5] = 0;
		prudata[6] = 0;
		prudata[7] = 0;
		prudata[8] = 0;
		prudata[9] = 0;
	    prussdrv_exec_program (PRU1, PRU1_BINARY);
	}

	// ----- Calculo do tempo de propagacao
	NumberOfInstructions = (prudata[4] << 24) + (prudata[3] << 16) + (prudata[2] << 8) + (prudata[1]);
	PRU0_PropagationTime_ns = (int) NumberOfInstructions*5;

	NumberOfInstructions = (prudata[9] << 24) + (prudata[8] << 16) + (prudata[7] << 8) + (prudata[6]);
	PRU1_PropagationTime_ns = (int) NumberOfInstructions*5;

	// ----- Compensacao de atrasos intrinsecos ao hardware
	if(PRU0_PropagationTime_ns != 0)
		PRU0_PropagationTime_ns -= 35;

	if(PRU1_PropagationTime_ns != 0)
		PRU1_PropagationTime_ns -= 35;

	// ----- Calculo da distancia ate o problema
	Distance_m[0] = PRU0_PropagationTime_ns/(2*VP);
	Distance_m[1] = PRU1_PropagationTime_ns/(2*VP);

	return;
}


