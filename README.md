# CP5 - Computação de Borda com IoT para Monitoramento de Vinhedos

Este projeto faz parte do Check Point 5 de IoT e foca no monitoramento de vinhedos usando dispositivos IoT conectados a um dashboard web dinâmico para análise de dados em tempo real, aproveitando a Computação de Borda e o FIWARE.

## Visão Geral do Projeto

O sistema utiliza um ESP32 conectado a múltiplos sensores para monitoramento de temperatura, umidade e luminosidade, simulando as condições do vinhedo. O ESP32 coleta os dados dos sensores DHT22 e LDR e publica essas informações via MQTT em um broker, permitindo que as leituras sejam monitoradas em tempo real.

O dashboard, desenvolvido em Python, exibe dinamicamente os dados históricos coletados pelos sensores, oferecendo uma visão abrangente das condições do ambiente, facilitando a tomada de decisões.

### Funcionalidades
- **Monitoramento em tempo real** de temperatura, umidade e luminosidade.
- **Publicação automática** dos dados dos sensores a cada segundo.
- **Dashboard web** para análise de dados históricos.
- **Simulação usando Wokwi** para prototipagem da solução.

## Estrutura do Projeto

- **FiwareDeploy_new_v4.drawio**: Diagrama de arquitetura da solução.
- **cp5-Edge.drawio.png**: Representação gráfica da arquitetura do sistema.
- **dashboard.py**: Script Python para o dashboard web dinâmico.
- **wokwi-img.PNG**: Captura de tela da simulação configurada no Wokwi.
- **wokwi.c++**: Código para configuração do ESP32 com os sensores conectados.

 ## Arquitetura
![alt text](cp5-Edge.drawio.png)

## Instalação

### Pré-requisitos
- ESP32
- Sensor DHT22 (temperatura e umidade)
- Sensor LDR (luminosidade)
- Broker MQTT
- Simulador Wokwi

### Passos

1. **Clonar o repositório**:
   ```bash
   git clone https://github.com/seu-repositorio/cp5-Edge.git
   cd cp5-Edge

2. **Executar o dashboard**:
   ```bash
   python3 dashboard.py
   
3. **Simular com o Wokwi**:
   Abra o arquivo wokwi.c++ no simulador Wokwi para simular o ambiente com os sensores.

## Como Funciona

- Sensores: O DHT22 mede temperatura e umidade, enquanto o LDR Monitora a luminosidade. Todos os dados são enviados via MQTT para o broker e, consequentemente, para o FIWARE.

- Dashboard: O dashboard exibe dados em tempo real e tendências históricas dos sensores. Ele se conecta ao Orion Context Broker via FIWARE e exibe gráficos de temperatura, umidade e luminosidade, atualizados dinamicamente.
  
- MQTT: O ESP32 publica os dados dos sensores de temperatura, umidade e luminosidade a cada segundo nos seguintes tópicos MQTT:
- Tópico de temperatura: /TEF/sensor001/t
- Tópico de umidade: /TEF/sensor001/h
- Tópico de luminosidade: /TEF/sensor001/l



## Integrantes
- Arthur Galvão Alves - RM554462
- Eduardo da Silva Lima - RM554804
- Felipe Braunstein e Silva - RM554483
- Felipe do Nascimento Fernandes - RM554598
- Jefferson Junior Alvarez Urbina - RM558497

