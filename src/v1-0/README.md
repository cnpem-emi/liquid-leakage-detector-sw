# Sensor de Vazamento - _SIRIUS_

Esta versão utiliza compartilhamento de memória para comunicação com as PRUs e device tree overlay para configuração dos pinos.


## Biblioteca de interface

Na pasta /src/libraries, executar os arquivos `library_build.sh` e `overlay_build.sh` para a instalação da biblioteca na BBB.


## IOC EPICS

IOC desenvolvido para mapear os sensores de vazamento em variáveis EPICS e comandar o monitoramento.
Ao iniciar, executar o arquivo `overlay.sh` presente na pasta /src. Executar apenas uma vez após ligar o sistema.
Para rodar o IOC, basta executar o arquivo `SV-IOC.py`