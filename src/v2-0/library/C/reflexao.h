#ifndef CPRU_USERSPACE_H
#define CPRU_USERSPACE_H

#ifdef __cplusplus
extern "C" {
#endif

void start_PRU();
void close_PRU();
int8_t pulsar_PRU(float *Distance_m);

#ifdef __cplusplus
}
#endif

#endif


