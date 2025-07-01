#include <MFRC522.h>
#include <SPI.h>
#define PINO_RST 9
#define PINO_SDA 10

MFRC522 rfid(PINO_SDA, PINO_RST);

void setup() {
  const int max_waiting_time = 200;//secs
  const int rfid_waiting_time = 60;

  const int right_angle = 0;
  const int left_angle = 90;

  int calculated_max_time;
  int vehicles_number;
  int vehicles_waiting_time;
  int cam_angle;

  bool has_rfid;
  int rfid_waiting_time;

  bool has_pedestrian;
  int pedestrian_waiting_time;

  bool left_tlight;
  int left_counter_time;

  bool right_tlight;
  int right_counter_time;

  Serial.begin(9600);

  SPI.begin();      // Inicia  SPI bus
  rfid.PCD_Init();

  while (!Serial);
}

void loop() {
   //Procura nova tag
  if (!rfid.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Seleciona uma tag
  if (!rfid.PICC_ReadCardSerial()) 
  {
    return;
  }

  //Mostra UID na serial (TESTE RFID)
  Serial.print("UID da tag :");
  String conteudo= "";

  for (byte i = 0; i < rfid.uid.size; i++) 
  {
     conteudo.concat(String(rfid.uid.uidByte[i] < HEX ? " 0" : " ")); 
     conteudo.concat(String(rfid.uid.uidByte[i], HEX)); 
  }
  

  /*if (Serial.available() > 0) // Verifica se há dados recebidos pela porta
    vehicles_number = Serial.parseInt();*/
  vehicles_number = 3;

  if(cam_angle == right_angle){
    
    if (conteudo.substring(1) == "f2 b1 ae 20") //Existe Rfid
    {
      if()
      
    }

  } else if(cam_angle == left_angle){
    if (conteudo.substring(1) == "f2 b1 ae 20") //Existe Rfid
    {
    
    }
  }else{
    print("erro");
  }

}