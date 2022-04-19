#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>

#define DEVICE_NAME0 "/dev/rpmsg_pru30"
#define DEVICE_NAME1 "/dev/rpmsg_pru31"
#define MAX_BUFFER_SIZE 512

#define VP				4.1 			// ns/m

char readBuf[2][MAX_BUFFER_SIZE];


int8_t pulsar_prus(float *Distance_m) {

	uint32_t NumberOfInstructions[2];
	int PropagationTime_ns[2];
	float Distance_m[2] = {0, 0};
	uint8_t rd_stat, wr_stat[2];

	struct pollfd pfd[2];

	pfd[0].fd = open(DEVICE_NAME0, O_RDWR);
 	pfd[1].fd = open(DEVICE_NAME1, O_RDWR);

	if (pfd[0].fd < 0) {
 		printf("Failed to open %s\n", DEVICE_NAME0);
 		return -1;
 	}
 	if (pfd[1].fd < 0) {
 		printf("Failed to open %s\n", DEVICE_NAME1);
 		return -1;
 	}

	// ----- Sinaliza inicio
	wr_stat[0] = write(pfd[0].fd, "-", 2) == -1;
    if (wr_stat[0]) {
        printf("Failed to write to PRU0");
        return -1;
    }
	wr_stat[1] = write(pfd[1].fd, "-", 2) == -1;
    if (wr_stat[1]) {
        printf("Failed to write to PRU1");
        return -1;
    }


	pfd[0].events = POLLIN;
    pfd[1].events = POLLIN;
    rd_stat = poll(pfd, 2, 3000);



	for(int i=0; i<2; i++){
		if((pfd[i].revents & POLLIN) == 0){
			// timeout, reinicializa pru
			close(pfd[i].fd);
			if(i){
				system("echo stop > /sys/class/remoteproc/remoteproc2/state");
				system("echo start > /sys/class/remoteproc/remoteproc2/state");
			}
			else{
				system("echo stop > /sys/class/remoteproc/remoteproc1/state");
				system("echo start > /sys/class/remoteproc/remoteproc1/state");
			}
		}else{
			read(pfd[i].fd, readBuf[i], MAX_BUFFER_SIZE);
            NumberOfInstructions[i] = (readBuf[i][3] << 24) | (readBuf[i][2] << 16) | (readBuf[i][1] << 8) | readBuf[i][0];
		    PropagationTime_ns[i] = (int) NumberOfInstructions[i]*5;
            PropagationTime_ns[i] -= 35;                //compensacao de atraso de hardware
            Distance_m[i] = PropagationTime_ns[i]/(2*VP);
		}

		close(pfd[i].fd);
	}

	return 0;
}
