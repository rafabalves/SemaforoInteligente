void setup() {
  Serial.begin(9600);  // Inicializa a comunicação serial
  while (!Serial);     // Aguarda a serial estar pronta (em alguns Arduinos)
}

void loop() {
  if (Serial.available() > 0) {   // Verifica se há dados recebidos
    int valor = Serial.parseInt();  // Lê um número inteiro
    Serial.print("Recebido: ");
    Serial.println(valor);
    
    // Você pode usar esse valor como quiser, por exemplo:
    analogWrite(9, valor);  // (apenas como exemplo, se o valor for de 0 a 255)
  }
}