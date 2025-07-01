#include <MFRC522.h>
#include <SPI.h>

#define PINO_RST 9
#define PINO_SDA 10

#define LED_DIREITA 2
#define LED_ESQUERDA 3 //cabo mais curto
#define BOTAO_PEDESTRE 4

MFRC522 rfid(PINO_SDA, PINO_RST);

const String rfidTagUID = "d3 1e a2 a5";
const int max_waiting_time = 200; // Tempo máximo de espera
const int rfid_bonus_time = 60;

const int right_angle = 0;
const int left_angle = 90;

int vehicles_number = 3; // Por enquanto fixo
int cam_angle = right_angle;

bool has_rfid = false;
bool has_pedestrian = false;

int left_counter_time = 0;
int right_counter_time = 0;

unsigned long last_change_time = 0;
int current_duration = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_DIREITA, OUTPUT);
  pinMode(LED_ESQUERDA, OUTPUT);
  pinMode(BOTAO_PEDESTRE, INPUT_PULLUP);

  digitalWrite(LED_DIREITA, HIGH);  // Começa vermelho (parar)
  digitalWrite(LED_ESQUERDA, LOW);  // Começa verde (siga)

  last_change_time = millis();
}

void loop() {
  //Variavel quant carros
  /*if (Serial.available() > 0) // Verifica se há dados recebidos pela porta
    vehicles_number = Serial.parseInt();*/

  // Lê botão de pedestre
  has_pedestrian = digitalRead(BOTAO_PEDESTRE) == LOW;

  // Lê RFID
  has_rfid = false;
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String conteudo = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      conteudo.concat(String(rfid.uid.uidByte[i], HEX));
      conteudo.concat(" ");
    }
    conteudo.trim();

    if (conteudo == rfidTagUID) {
      has_rfid = true;
      Serial.println("Veículo prioritário detectado");
    }
    rfid.PICC_HaltA();
  }

  // Calcula tempo com base nas variáveis
  int base_time = vehicles_number * 10; // Ex: 10s por carro
  if (base_time > max_waiting_time) 
    base_time = max_waiting_time;
  if (has_rfid) 
    base_time += rfid_bonus_time;
  if (has_pedestrian) 
    base_time += 20;

  // Alterna semáforo baseado no tempo
  if (millis() - last_change_time > current_duration * 1000) {
    toggleLights();
    last_change_time = millis();
    current_duration = base_time;

    Serial.print("Tempo do próximo ciclo: ");
    Serial.println(current_duration);
  }
}

void toggleLights() {
  if (digitalRead(LED_DIREITA) == HIGH) {
    digitalWrite(LED_DIREITA, LOW);   // Siga
    digitalWrite(LED_ESQUERDA, HIGH); // Pare
    cam_angle = left_angle;
  } else {
    digitalWrite(LED_DIREITA, HIGH);  // Pare
    digitalWrite(LED_ESQUERDA, LOW);  // Siga
    cam_angle = right_angle;
  }
}
