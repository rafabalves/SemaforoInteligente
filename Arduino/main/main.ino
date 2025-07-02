#include <MFRC522.h>
#include <SPI.h>
#include <Servo.h>

#define LED_ESQUERDA 5       // LED vermelho da via com RFID e botão
#define LED_DIREITA 2        // LED vermelho da via sem sensores
#define BOTAO_PEDESTRE 4     // Botão de pedestre

#define PINO_RST 9
#define PINO_SDA 10

#define ANGULO_ESQUERDA 0
#define ANGULO_DIREITA 90

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
  tempo_espera = calcularTempoEspera(); // Tempo inicial baseado em veículos aleatórios

  Serial.print("Semáforo iniciado. Tempo de espera via esquerda: ");
  Serial.print(tempo_espera);
  Serial.println("s");
}

void loop() {
  //Variavel quant carros
  /*if (Serial.available() > 0) // Verifica se há dados recebidos pela porta
    vehicles_number = Serial.parseInt();*/
  unsigned long agora = millis();
  int tempo_passado = (agora - last_change_time) / 1000;
  int tempo_restante = tempo_espera - tempo_passado;

  // Contagem regressiva (somente se o valor mudou)
  if (tempo_restante != ultimo_segundo && tempo_restante >= 0) {
    ultimo_segundo = tempo_restante;
    if (via_esquerda_fechada)
      Serial.print("Tempo de espera via esquerda: ");
    else
      Serial.print("Tempo de espera via direita: ");
    Serial.print(tempo_restante);
    Serial.println("s");
  }

  // DETECÇÃO DE VEÍCULO DE SERVIÇO (RFID)
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

  // DETECÇÃO DE PEDRESTRE
  if (!via_esquerda_fechada && digitalRead(BOTAO_PEDESTRE) == HIGH) {
    if (tempo_restante > 100) {
      tempo_espera = 100;
      last_change_time = millis();
      ultimo_segundo = -1;
      Serial.println("Botão de pedestre pressionado. Tempo reduzido para 100s.");
      return; // Evita troca imediata no mesmo loop
    } else {
      Serial.println("Botão de pedestre pressionado, mas já próximo da troca.");
    }
  }

  // VERIFICA SE O TEMPO ESGOTOU PARA TROCAR OS SEMÁFOROS
  if (tempo_passado >= tempo_espera) {
    via_esquerda_fechada = !via_esquerda_fechada; // Alterna o estado
    alternarSemaforos();
    last_change_time = millis();
    tempo_espera = calcularTempoEspera();
    ultimo_segundo = -1;

    if (via_esquerda_fechada)
      Serial.print("Nova espera via esquerda: ");
    else
      Serial.print("Nova espera via direita: ");
    Serial.print(tempo_espera);
    Serial.println("s");
  }
}

// Alterna LEDs e movimenta a câmera
void alternarSemaforos() {
  if (via_esquerda_fechada) {
    digitalWrite(LED_ESQUERDA, HIGH);  // Vermelho = fechado
    digitalWrite(LED_DIREITA, LOW);    // Verde = aberto
    servo.write(ANGULO_ESQUERDA);
  } else {
    digitalWrite(LED_ESQUERDA, LOW);   // Verde = aberto
    digitalWrite(LED_DIREITA, HIGH);   // Vermelho = fechado
    servo.write(ANGULO_DIREITA);
  }

  // Atualiza número de veículos com valor aleatório de 0 a 10
  num_veiculos = random(0, 11);
  Serial.print("Número de veículos na via fechada: ");
  Serial.println(num_veiculos);
}

// Calcula o tempo com base em número de veículos, respeitando o limite de 200s
int calcularTempoEspera() {
  int tempo = 200 - (num_veiculos * 10);
  if (tempo < 0) tempo = 0;
  return tempo;
}
