#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <unistd.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>


#ifdef __cplusplus
extern "C" {
#endif

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


/* INICIALIZACAO DA PRU
 * --Retorno--
 *  0: Ok, bem inicializada
 * -1: Erro
*/
int init_start_PRU();


/* FINALIZACAO DA PRU
 * -----
*/
void close_PRU();


/* INICIALIZACAO DA PRU
 * Distance_m: vetor de duas posicoes para armazenar a distancia detectada nas duas redes coaxiais
 * --Retorno--
 * Distancia de propagacao (ida) até o problema, em metros.
 * Se 0, não houve reflexao.
*/
void pulsar_PRU(float *Distance_m);





#ifdef __cplusplus
}
#endif
