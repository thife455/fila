/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino     ESP8266 
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro   Wi-Fi
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin         Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST         D3
 * SPI SS      SDA(SS)      10            53        D10        10               10          D8
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16          D7
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14          D6
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15          D5
 */

#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

ESP8266WebServer server;

int table1 = 1; //variável para mesa 1
char* ssid = "AP 24"; //Nome do wi-fi
char* password = "vero4467"; //senha do wi-fi

#define SS_PIN 15 //GPIO do pino D8
#define RST_PIN 0 //GPIO do pino D3
 
MFRC522 rfid(SS_PIN, RST_PIN); // Cria a classe RFID

MFRC522::MIFARE_Key key; //Key dos dispositivos RFID que seguem o padrão MIFARE
 
byte nuidPICC[4]; //Inicia o vetor que vai armazenar o ID da tag

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password); //Conecta Wemos ao wi-fi
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
    }
    Serial.println("");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); //Imprime IP do ESP8266


  
  SPI.begin(); // Inicia o protocolo SPI
  rfid.PCD_Init(); // Inicia o leitor RFID (MFRC522) 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
 
void loop() {

  // Procura tags
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verifica se o ID foi lido
  if ( ! rfid.PICC_ReadCardSerial())
    return;


  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Checa se a tag segue o padrão MIFARE
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  //nuidPICC guardaria as tags que já foram lida. Como, para o propósito desse programa, não importa se uma tag está sendo lida pela primeira ou segunda vez, o nuidPICC nunca é preenchido com o ID das tags lidas
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));
  
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

  //ID da tag referente à mesa 1
    if (rfid.uid.uidByte[0] == 0x51 && 
     rfid.uid.uidByte[1] == 0x64 &&
     rfid.uid.uidByte[2] == 0x2b &&
     rfid.uid.uidByte[3] == 0xd9) {
      if(WiFi.status()== WL_CONNECTED){
        server.on("/", [](){
          Serial.println("Serving /data");
          server.sendHeader("Access-Control-Allow-Origin", "*");
          if(table1 == 1)
            server.send(200, "application/json", "{\"Mesa_1\":\"Ocupada\"}");
          if(table1 == 0)
            server.send(200, "application/json", "{\"Mesa_1\":\"Livre\"}");
          table1 = table1?0:1;
          }
        );
        server.begin();
      } 
    }
  }

  // Interrompe a procura de tags pelo leitor
  rfid.PICC_HaltA();

  // Interrompe a criptografia no PCD
  rfid.PCD_StopCrypto1();

  //Executa as funções determinadas anteriormente (server.on) e lida com os trâmites do http
  server.handleClient();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
