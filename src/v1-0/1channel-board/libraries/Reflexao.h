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
 * prudata[0] = pulse verify request
 * prudata[1] = ||
 * prudata[2] = || Response
 * prudata[3] = || Time
 * prudata[4] = ||
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
 * --Retorno--
 * Distancia de propagacao (ida) até o problema, em metros.
 * Se 0, não houve reflexao.
*/
float pulsar_PRU();





#ifdef __cplusplus
}
#endif
