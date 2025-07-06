# Codificação do Projeto de Semáforo Inteligente com Visão Computacional e Arduino

## Alunos:

- ### Isabely Toledo de Melo
- ### Rafael Batista Alves
- ### Thaíssa Fernandes Silva

## Descrição do Projeto

Este repositório apresenta um sistema de **semáforo inteligente** que integra visão computacional com um controle físico via Arduino. O sistema detecta veículos em tempo real usando um modelo YOLOv8, conta quantos estão dentro de uma área de interesse e envia essa contagem ao Arduino, que então ajusta o tempo do semáforo (representado por LEDs) de forma dinâmica.

O objetivo é simular um ambiente de tráfego urbano onde o tempo de parada e liberação do semáforo pode ser adaptado de forma automatizada conforme a demanda do fluxo veicular.

## Estrutura de Arquivos

```
├── Arduino/
│   └── libraries/
│       └── MRFC522.zip          # Biblioteca necessária para o código Arduino
│
├── main/
│   └── main.ino                 # Código Arduino que controla os LEDs com base na contagem
│
├── Py/
│   ├── main.py                  # Script Python com detecção de veículos e envio serial
│   └── yolov8n.pt               # Modelo YOLOv8 usado na detecção
│
└── README.md                    # Descrição do projeto
```

- **main.py**: utiliza OpenCV, o modelo YOLOv8 da Ultralytics e comunicação serial para detectar veículos e enviar sua contagem ao Arduino.
- **main.ino**: código que recebe a contagem via porta serial e representa o controle de tráfego.
- **README.md**: este arquivo explicativo com instruções de uso e detalhes do projeto.

## Instruções de Execução

### Requisitos

#### Python

- Python 3.8 ou superior
- Pacotes necessários:
  ```bash
  pip install opencv-python ultralytics pyserial numpy
  ```

#### Arduino

- Placa Arduino Uno (ou similar)
- Projeto montado e estrurado
- Cabo USB e porta COM identificada (ajustar no `main.py`)
- Biblioteca adicional:
  - Instale a biblioteca `MRFC522.zip` manualmente:
    1. Abra a **IDE do Arduino**
    2. Vá em **Sketch → Incluir Biblioteca → Adicionar Biblioteca .ZIP...**
    3. Selecione o arquivo `Arduino/libraries/MRFC522.zip`

### Etapas

1. Faça o upload do código `main.ino` localizado em `main/` para a placa Arduino.
2. Execute o script Python localizado em `Py/`:
   ```bash
   python Py/main.py
   ```
3. O sistema abrirá a câmera e iniciará a detecção. A contagem de veículos será enviada ao Arduino.
4. O Arduino ajustará o tempo de troca de LEDs conforme a quantidade de veículos detectados.

## Explicações Técnicas

### Detecção de Veículos:

- O modelo YOLOv8 (versão `yolov8n.pt`) detecta objetos do tipo: `car`, `bus`, `truck` e `motorcycle`.
- A contagem ocorre apenas para objetos dentro da área definida pela imagem da câmera.

### Comunicação:

- A contagem é enviada via serial para a porta `COM'X'` (pode ser ajustada conforme seu dispositivo).
- O Arduino lê essa informação e determina se o tempo deve ser reduzido pela quantidade de carros.

### Lógica do Arduino:

- Tempo máximo de espera: 200 segundos;
- Redução de 10 segundos por veículo aguardando;
- Veículos de emergência: máximo de 15 segundos de espera;
- Pedestre: se o botão for pressionado enquanto o sinal está aberto, a outra via terá no máximo 100 segundos de espera.

## Considerações Finais

- O projeto utiliza uma abordagem prática e acessível para o controle inteligente de tráfego.
- Possui grande potencial para simulações educacionais e projetos de automação embarcada.
- A lógica pode ser facilmente expandida para mais semáforos ou sensores adicionais.