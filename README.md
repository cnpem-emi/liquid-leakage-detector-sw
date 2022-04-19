## Sensor de Vazamento - _SIRIUS_
___

_Autores:_

1. Patricia H. Nallin ( _patricia.nallin@lnls.br_ )

2. Carlos Scorzato ( _carlos.scorzato@lnls.br_ )

3. Samuel Fraga ( _samuel.fraga@cnpem.br_ )

_____
_Breve Descrição:_

O projeto `Sensor de Vazamento` está em desenvolvimento e visará detectar vazamentos ao longo das instalações do acelerador Sirius.
Baseia-se no princípio de reflexão de sinais numa linha de transmissão coaxial, cujo descasamento será causado pela presença de líquido no dielétrico do sensor capacitivo.

Os espaçamentos mínimos de indicação de vazamento serão os `berços`, que estão deslocados entre si de, aproximadamente, 5 metros.


_____


### Hardware

_---- em desenvolvimento (Carlos Scorzato)----_

_____



### Software - Beaglebone Black

1. __Biblioteca de interface__

Biblioteca desenvolvida para a operação do sistema, via PRU ( _Programmable Real-Time Unit_ ) da Beaglebone Black.




2. __IOC EPICS__

IOC desenvolvido para mapear os sensores de vazamento em variáveis EPICS e comandar o monitoramento.
Ao iniciar, executar o arquivo `overlay.sh` presente na pasta /src. Executar apenas uma vez após ligar o sistema.
Para rodar o IOC, basta executar o arquivo `SV-IOC.py`
_____




### Supervisório - Control System Studio (CSS)

Supervisório para monitorar vazamento e calibração das posições dos berços.
Desenvolvido na plataforma Control System Studio.
