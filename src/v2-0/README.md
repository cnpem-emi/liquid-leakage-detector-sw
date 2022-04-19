# Sensor de Vazamento - SIRIUS (Remoteproc)

Esta versão incorpora o padrão Remoteproc, subistituindo o compartilhamento de memória pelo protocolo RPMsg para comunicação com as PRUs. Compatível com Debian 10.
Contém:
- C library
- Python library
- PRU Firmware (ASM/C)
- Pin config script

## Python

### Instalação
```sh
cd library/Python && python3 setup.py install
```

### Uso
```python
from LLDet import pulsar_prus
pulsar_prus() # Returna distância(m) até o vazamento, 0 se nada for detectado.
```

## C
```c
#include <stdio.h>
#include <stdint.h>
#include "reflexao.h"

int main(void)
{
    float distancia[2];
    pulsar_prus(distancia);

    for(int i = 0; i < 2; i++) printf("Distância canal %d: %f\n", i, distancia[i]);
    return 0;
}
```

## PRU firmware
1. Instale o [PRU support package](https://git.ti.com/cgit/pru-software-support-package/pru-software-support-package/tree?h=master)
2. Compile
```
make
```
3. Execute `config-pin.sh`

