/*
PINOUT:
RC522 MODULE    Uno/Nano     MEGA
SDA             D10          D2
SCK             D13          D52
MOSI            D11          D51
MISO            D12          D50
IRQ             N/A          N/A
GND             GND          GND
RST             D9           D8
3.3V            3.3V         3.3V
*/
// myservo se spaja na 5V



#define DEBUG

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

Servo myservo;

const int numReaders = 3;
const int ssPins[] = {2, 3, 4};
const int resetPin = 8;

const int LED_crvena_napred = 6;
const int LED_zelena_napred = 7;
const int LED_crvena_iza = 9;
const int LED_zelena_iza = 10;

const int servo = 5;
int pos = 0;

MFRC522 mfrc522[numReaders];

const String correctIDs[][2] = {
  {"19f495", "left"},
  {"b96b75", "forward-right"},
  {"c39b75", "forward-left"},
  {"bac463", "right"},
  {"a7a3c91", "forward-left"},
  {"f11ece1", "forward-right"}
};

int numGroups = sizeof(correctIDs) / sizeof(correctIDs[0]);
int broj_grupe = 0;

String currentIDs[numReaders];
String trenutniID = "";
String sljedeciID = "";

void setup() {
  #ifdef DEBUG

  Serial.begin(9600);
  Serial.println(F("Serial communication starded"));
  
  #endif

  SPI.begin();

  for(int i = 0; i < numReaders; i++)
  {
    mfrc522[i].PCD_Init(ssPins[i], resetPin);
  }

  pinMode(LED_crvena_napred, OUTPUT);
  digitalWrite(LED_crvena_napred, HIGH);
  pinMode(LED_zelena_napred, OUTPUT);
  digitalWrite(LED_zelena_napred, LOW);
  pinMode(LED_crvena_iza, OUTPUT);
  digitalWrite(LED_crvena_iza, HIGH);
  pinMode(LED_zelena_iza, OUTPUT);
  digitalWrite(LED_zelena_iza, LOW);

  myservo.attach(servo);
  myservo.write(0);
  
}

void loop() {
  
  for (int i = 0; i < numReaders; i++)
  {
    mfrc522[i].PCD_Init();
    String readRFID = "";

    if (mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial())
    {
      readRFID = dump_byte_array(mfrc522[i].uid.uidByte, mfrc522[i].uid.size);
    }

    if(readRFID != currentIDs[i])
    {
      currentIDs[i] = readRFID;
    }

    mfrc522[i].PICC_HaltA();
    mfrc522[i].PCD_StopCrypto1();
  }



  while (broj_grupe < numGroups)
  {
    trenutniID = correctIDs[broj_grupe][0];
    sljedeciID = correctIDs[broj_grupe + 1][0];

    if (correctIDs[broj_grupe][1] == "left" && currentIDs[0] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_lijevo();
      pali_zel_gasi_crv_napred();
      while (true)
      {
        if(mfrc522[2].PICC_IsNewCardPresent() && mfrc522[2].PICC_ReadCardSerial())
        {
          pali_crv_gasi_zel_napred();
          break;
        }
      }
      break;
    }
    else if (correctIDs[broj_grupe][1] == "right" && currentIDs[0] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_desno();
      pali_zel_gasi_crv_napred();
      while (true)
      {
        if(mfrc522[1].PICC_IsNewCardPresent() && mfrc522[1].PICC_ReadCardSerial())
        {
          pali_crv_gasi_zel_napred();
          break;
        }
      }
      break;
    }
    else if (correctIDs[broj_grupe][1] == "forward-right" && currentIDs[1] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_desno();
      pali_zel_gasi_crv_iza();
      while (true)
      {
        if(mfrc522[0].PICC_IsNewCardPresent() && mfrc522[0].PICC_ReadCardSerial())
        {
          pali_crv_gasi_zel_iza();
          break;
        }
      }
      break;
    }
    else if (correctIDs[broj_grupe][1] == "forward-left" && currentIDs[2] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_lijevo();
      pali_zel_gasi_crv_iza();
      while (true)
      {
        if(mfrc522[0].PICC_IsNewCardPresent() && mfrc522[0].PICC_ReadCardSerial())
        {
          pali_crv_gasi_zel_iza();
          break;
        }
      }
      break;
    }
    else { break; }
  }

  delay(500);
}

String dump_byte_array(byte *buffer, byte bufferSize) {
    String output = "";
    for (byte i = 0; i < bufferSize; i++) {
        output += String(buffer[i], HEX);
    }
    return output;
}

void pali_zel_gasi_crv_napred() {
  digitalWrite(LED_crvena_napred, LOW);
  digitalWrite(LED_zelena_napred, HIGH);
}

void pali_zel_gasi_crv_iza() {
  digitalWrite(LED_crvena_iza, LOW);
  digitalWrite(LED_zelena_iza, HIGH);
}

void pali_crv_gasi_zel_napred() {
  digitalWrite(LED_crvena_napred, HIGH);
  digitalWrite(LED_zelena_napred, LOW);
}

void pali_crv_gasi_zel_iza() {
  digitalWrite(LED_crvena_iza, HIGH);
  digitalWrite(LED_zelena_iza, LOW);
}

void idi_desno() {
  myservo.write(0);
}

void idi_lijevo() {
  myservo.write(180);
}