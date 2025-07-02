#include <MFRC522.h>
#include <SPI.h>
#include <Servo.h>

#define LED_ESQUERDA 5    // LED vermelho da via com RFID e botão
#define LED_DIREITA 2     // LED vermelho da via sem sensores
#define BOTAO_PEDESTRE 4  // Botão de pedestre

#define PINO_RST 9        // Pino do módulo RFID
#define PINO_SDA 10

#define ANGULO_ESQUERDA 0
#define ANGULO_DIREITA 90

MFRC522 rfid(PINO_SDA, PINO_RST);
Servo servo;

const String rfidTagUID = "d3 1e a2 a5";

unsigned long last_change_time = 0;
int tempo_espera = 0;
bool via_esquerda_fechada = true; // Começa com a esquerda fechada (LED aceso)
int num_veiculos = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_ESQUERDA, OUTPUT);
  pinMode(LED_DIREITA, OUTPUT);
  pinMode(BOTAO_PEDESTRE, INPUT);
  
  servo.attach(6); // Pino PWM para o servo motor

  // Estado inicial: esquerda fechada, direita aberta
  digitalWrite(LED_ESQUERDA, HIGH);  // Vermelho ligado = fechado
  digitalWrite(LED_DIREITA, LOW);    // Verde (apagado) = aberto
  servo.write(ANGULO_ESQUERDA);      // Câmera aponta para via fechada
  last_change_time = millis();

  tempo_espera = calcularTempoEspera(); // Define tempo inicial
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

  // Exibe contagem regressiva a cada segundo
  static int ultimo_segundo = -1;
  if (tempo_restante != ultimo_segundo && tempo_restante >= 0) {
    ultimo_segundo = tempo_restante;
    if (via_esquerda_fechada)
      Serial.print("Tempo de espera via esquerda: ");
    else
      Serial.print("Tempo de espera via direita: ");
    Serial.print(tempo_restante);
    Serial.println("s");
  }

  // Checa RFID se a via esquerda estiver fechada
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
        Serial.println("Veículo de emergência detectado. Tempo reduzido para 15s.");
      }
    }
    rfid.PICC_HaltA();
  }

  // Checa botão de pedestre se a via direita estiver fechada
  if (!via_esquerda_fechada && digitalRead(BOTAO_PEDESTRE) == HIGH) {
    if (tempo_restante > 20) {
      tempo_espera = 20;
      last_change_time = millis();
      Serial.println("Botão de pedestre pressionado. Tempo reduzido para 20s.");
    }
  }

  // Verifica se é hora de alternar os semáforos
  if (tempo_passado >= tempo_espera) {
    via_esquerda_fechada = !via_esquerda_fechada;
    alternarSemaforos();
    last_change_time = millis();
    tempo_espera = calcularTempoEspera();

    if (via_esquerda_fechada)
      Serial.print("Nova espera via esquerda: ");
    else
      Serial.print("Nova espera via direita: ");
    Serial.print(tempo_espera);
    Serial.println("s");
  }
}

// Alterna LEDs e move a câmera para a via que fechou
void alternarSemaforos() {
  if (via_esquerda_fechada) {
    digitalWrite(LED_ESQUERDA, HIGH);  // Fechado
    digitalWrite(LED_DIREITA, LOW);    // Aberto
    servo.write(ANGULO_ESQUERDA);
  } else {
    digitalWrite(LED_ESQUERDA, LOW);   // Aberto
    digitalWrite(LED_DIREITA, HIGH);   // Fechado
    servo.write(ANGULO_DIREITA);
  }

  // Atualiza número de veículos (aleatório de 0 a 10)
  num_veiculos = random(0, 11);
  Serial.print("Número de veículos na via fechada: ");
  Serial.println(num_veiculos);
}

// Calcula o tempo com base no número de veículos
int calcularTempoEspera() {
  int tempo = 200; // tempo máximo

  // Cada veículo reduz 10s
  tempo -= num_veiculos * 10;
  if (tempo < 0) tempo = 0;

  return tempo;
}
