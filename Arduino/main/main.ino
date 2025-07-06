#include <MFRC522.h>
#include <SPI.h>
#include <Servo.h>

#define LED_ESQUERDA 5       // LED vermelho da via com RFID e botão
#define LED_DIREITA 2        // LED vermelho da via sem sensores
#define BOTAO_PEDESTRE 4     // Botão de pedestre

#define PINO_RST 9
#define PINO_SDA 10

#define ANGULO_ESQUERDA 85
#define ANGULO_DIREITA 185

MFRC522 rfid(PINO_SDA, PINO_RST);
Servo servo;

const String rfidTagUID = "d3 1e a2 a5"; // UID da tag do veículo prioritário

unsigned long last_change_time = 0;
int tempo_espera = 0;
bool via_esquerda_fechada = true; // Começa com a esquerda fechada (LED aceso)
int num_veiculos = 0;

int ultimo_segundo = -1; // Para evitar prints repetidos

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_ESQUERDA, OUTPUT);
  pinMode(LED_DIREITA, OUTPUT);
  pinMode(BOTAO_PEDESTRE, INPUT); // com resistor pull-down externo

  servo.attach(6); // Servo motor no pino PWM 6

  // Estado inicial: esquerda fechada, direita aberta
  digitalWrite(LED_ESQUERDA, HIGH);  // Vermelho ligado = fechado
  digitalWrite(LED_DIREITA, LOW);    // Apagado = aberto
  servo.write(ANGULO_ESQUERDA);      // Câmera olha para via fechada

  last_change_time = millis();
  tempo_espera = calcularTempoEspera(); // Tempo inicial baseado em veículos

  Serial.print("Semáforo iniciado. Tempo de espera via esquerda: ");
  Serial.print(tempo_espera);
  Serial.println("s");
}

void loop() {
  // === LEITURA CONTÍNUA DA SERIAL (contagem de veículos da câmera) ===
  while (Serial.available() > 0) {
    int recebido = Serial.parseInt();
    if (recebido >= 0 && recebido <= 10) {
      num_veiculos = recebido;
      //Serial.print("Número de veículos atualizado pela câmera: ");
      //Serial.println(num_veiculos);
    }
  }

  // === CONTAGEM REGRESSIVA ===
  unsigned long agora = millis();
  int tempo_passado = (agora - last_change_time) / 1000;
  int tempo_restante = tempo_espera - tempo_passado;

  if (tempo_restante != ultimo_segundo && tempo_restante >= 0) {
    ultimo_segundo = tempo_restante;
    if (via_esquerda_fechada)
      Serial.print("Tempo de espera via esquerda: ");
    else
      Serial.print("Tempo de espera via direita: ");
    Serial.print(tempo_restante);
    Serial.println("s");
  }

  // === DETECÇÃO DE VEÍCULO DE EMERGÊNCIA ===
  if (via_esquerda_fechada && rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String conteudo = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      conteudo.concat(String(rfid.uid.uidByte[i], HEX));
      conteudo.concat(" ");
    }
    conteudo.trim();

    if (conteudo == rfidTagUID) {
      if (tempo_restante > 15) {
        tempo_espera = 15;
        last_change_time = millis();
        ultimo_segundo = -1;
        Serial.println("Veículo de emergência detectado. Tempo reduzido para 15s.");
        return; // Evita troca imediata no mesmo loop
      } else {
        Serial.println("Veículo de emergência detectado, mas já próximo da troca.");
      }
    }
    rfid.PICC_HaltA();
  }

  // === DETECÇÃO DE PEDRESTRE ===
  if (!via_esquerda_fechada && digitalRead(BOTAO_PEDESTRE) == HIGH) {
    if (tempo_restante > 10) {
      tempo_espera = 10;
      last_change_time = millis();
      ultimo_segundo = -1;
      Serial.println("Botão de pedestre pressionado. Tempo reduzido para 100s.");
      return;
    } else {
      Serial.println("Botão de pedestre pressionado, mas já próximo da troca.");
    }
  }

  // === VERIFICA SE DEVE TROCAR OS SEMÁFOROS ===
  if (tempo_passado >= tempo_espera) {
    via_esquerda_fechada = !via_esquerda_fechada;
    alternarSemaforos();
    last_change_time = millis();
    tempo_espera = calcularTempoEspera(); // Usa o num_veiculos já atualizado
    ultimo_segundo = -1;

    if (via_esquerda_fechada)
      Serial.print("Nova espera via esquerda: ");
    else
      Serial.print("Nova espera via direita: ");
    Serial.print(tempo_espera);
    Serial.println("s");
  }
}

// === TROCA OS LEDS E MOVE A CÂMERA PARA VIA FECHADA ===
void alternarSemaforos() {
  if (via_esquerda_fechada) {
    digitalWrite(LED_ESQUERDA, HIGH);  // Fecha a esquerda
    digitalWrite(LED_DIREITA, LOW);    // Abre a direita
    Serial.print("Servo em:");
    Serial.println(ANGULO_ESQUERDA);
    servo.write(ANGULO_ESQUERDA);
  } else {
    digitalWrite(LED_ESQUERDA, LOW);   // Abre a esquerda
    digitalWrite(LED_DIREITA, HIGH);   // Fecha a direita
    Serial.print("Servo em:");
    Serial.println(ANGULO_DIREITA);
    servo.write(ANGULO_DIREITA);
  }

  // Aguarda nova leitura válida da câmera via serial
  Serial.println("Aguardando leitura da câmera...");

  unsigned long inicio = millis();

  while ((millis() - inicio) < 2000) {  // Espera até 2 segundos por um valor válido
    if (Serial.available() > 0) {
      int valor = Serial.parseInt();
        num_veiculos = valor;
        Serial.print("Número de veículos na câmera (pós-servo): ");
        Serial.println(num_veiculos);
    }
  }

  Serial.print("Número de veículos na via fechada: ");
  Serial.println(num_veiculos);
}

// === CÁLCULO DO TEMPO COM BASE EM VEÍCULOS ===
int calcularTempoEspera() {
  int tempo = 200 - (num_veiculos * 10);
  if (tempo < 0) tempo = 0;
  return tempo;
}
