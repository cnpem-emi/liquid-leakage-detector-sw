#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <time.h>
#include "reflexao.h"

#define DEVICE_NAME0 "/dev/rpmsg_pru30"
#define DEVICE_NAME1 "/dev/rpmsg_pru31"
#define MAX_BUFFER_SIZE 512

char readBuf[2][MAX_BUFFER_SIZE];

#define VP				4.1 			// ns/m


void close_PRU();
int init_start_PRU();
int8_t pulsar_PRU(float *Distance_m);


void close_PRU(){
	// ----- Desabilita PRU e fecha mapeamento da shared RAM
	system("echo stop > /sys/class/remoteproc/remoteproc1/state");
	system("echo stop > /sys/class/remoteproc/remoteproc2/state");
}


void init_PRU(){
	// ----- Executar codigo na PRU
	system("echo start > /sys/class/remoteproc/remoteproc1/state");
	system("echo start > /sys/class/remoteproc/remoteproc2/state");
}


int8_t pulsar_PRU(float *Distance_m){
	uint32_t NumberOfInstructions[2];
	int PropagationTime_ns[2];
	uint8_t rd_stat[2];

	struct pollfd pfd[2];

	pfd[0].fd = open(DEVICE_NAME0, O_RDWR);
 	pfd[1].fd = open(DEVICE_NAME1, O_RDWR);

	if (pfd[0].fd < 0) {
 		printf("Failed to open %s\n", DEVICE_NAME0);
 		return -1;
 	}
 	if (pfd[1].fd < 0) {
 		printf("Failed to open %s\n",DEVICE_NAME1);
 		return -1;
 	}

	// ----- Sinaliza inicio
	write(pfd[0].fd, "-", 2);
 	write(pfd[1].fd, "-", 2);


	// ----- Aguarda sinal de finalizacao do ciclo
	int i = 0;
	rd_stat[0]=0;
	while (!rd_stat[0] && i<30){
		rd_stat[0]=read(pfd[0].fd, readBuf[0], MAX_BUFFER_SIZE);
		sleep(0.1);
		i++;
	}
	rd_stat[1]=0;
	i=0;
	while(!rd_stat[1] && i<30){
		rd_stat[1]=read(pfd[1].fd, readBuf[1], MAX_BUFFER_SIZE);
		sleep(0.1);
		i++;
	}


	for(i=0; i<2; i++){
		// ----- Timeout: reinicializa PRUS
		if(!rd_stat[i]){
			if(i){
				system("echo stop > /sys/class/remoteproc/remoteproc2/state");
				system("echo start > /sys/class/remoteproc/remoteproc2/state");
			}
			else{
				system("echo stop > /sys/class/remoteproc/remoteproc1/state");
				system("echo start > /sys/class/remoteproc/remoteproc1/state");
			}
		}

		// ----- Calculo do tempo de propagacao
		NumberOfInstructions[i] = (readBuf[i][3] << 24) | (readBuf[i][2] << 16) | (readBuf[i][1] << 8) | readBuf[i][0];
		PropagationTime_ns[i] = (int) NumberOfInstructions[i]*5;

		// ----- Compensacao de atrasos intrinsecos ao hardware
		if(PropagationTime_ns[i] != 0)
			PropagationTime_ns[i] -= 35;

		// ----- Calculo da distancia ate o problema
		Distance_m[i] = PropagationTime_ns[i]/(2*VP);

		read(pfd[i].fd, readBuf[i], MAX_BUFFER_SIZE);
		close(pfd[i].fd);
	}

	return 0;
}
