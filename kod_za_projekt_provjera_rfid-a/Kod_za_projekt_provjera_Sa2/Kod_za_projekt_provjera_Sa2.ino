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

#define DEBUG
#define MFRC522_SPICLOCK (1500000u)  // 1MHz for long cables
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

const int ssPins[] = {2, 4};
const int resetPins[] = {29, 34};
const int numReaders = 2;

const int LED_napred = 23;
const int LED_iza = 22;

const int servo = 5;
int pos = 90;
// pos-max: 120; pos-min: 90

MFRC522 mfrc522[numReaders];

const String correctIDs[][2] = {
  {"62eeca6", "left"},
  {"e653116", "right"},
  {"7a95c96", "left"},
  {"553b156", "right"},
  {"126ec96", "left"},
  {"57df116", "forward-right"}
};

int numGroups = sizeof(correctIDs) / sizeof(correctIDs[0]);
int broj_grupe = 0;

String currentIDs[numReaders];

void setup() {
  #ifdef DEBUG

  Serial.begin(9600);
  Serial.println(F("Serial communication starded"));
  
  #endif

  SPI.begin();

  for(int i = 0; i < numReaders; i++)
  {
    mfrc522[i].PCD_Init(ssPins[i], resetPins[i]);
    mfrc522[i].PCD_SetAntennaGain(MFRC522::PCD_RxGain::RxGain_max);

    Serial.print(F("Reader #"));
    Serial.print(i);
    Serial.print(F(" initialised on pin "));
    Serial.print(String(ssPins[i]));
    Serial.print(F(". Antenna strenght: "));
    Serial.print(mfrc522[i].PCD_GetAntennaGain());
    Serial.print(F(". Version: "));
    mfrc522[i].PCD_DumpVersionToSerial();

    digitalWrite(resetPins[i], HIGH);
    delay(200);
  }

  Serial.println(F("--- END SETUP ---"));

  pinMode(LED_napred, OUTPUT);
  digitalWrite(LED_napred, HIGH);
  pinMode(LED_iza, OUTPUT);
  digitalWrite(LED_iza, HIGH);

  myservo.attach(servo);
  myservo.write(pos);  
}

void loop() {
  
  for (int i = 0; i < numReaders; i++)
  {
    activateReader(i);
    mfrc522[i].PCD_Init();
    mfrc522[i].PCD_SetAntennaGain(MFRC522::PCD_RxGain::RxGain_max);
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
    deactivateAll();

    delay(200);
  }

  for (int i = 0; i < numReaders; i++)
  {
    Serial.print(F("Reader #"));
    Serial.print(String(i+1));
    Serial.print(F(" on Pin #"));
    Serial.print(String(ssPins[i]));
    Serial.print(F(" detected tag: "));
    Serial.print({currentIDs[i]});
    Serial.println("");
  }
  Serial.println(F("---"));
  Serial.println("");

  while (broj_grupe < numGroups)
  {
    if (correctIDs[broj_grupe][1] == "right" && currentIDs[0] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_desno();
      digitalWrite(LED_napred, LOW);
      while (true)
      {
        delay(4500);
        digitalWrite(LED_napred, HIGH);
        break;
      }
      break;
    }
    else if (correctIDs[broj_grupe][1] == "left" && currentIDs[0] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_lijevo();
      digitalWrite(LED_napred, LOW);
      while (true)
      {
        if(mfrc522[1].PICC_IsNewCardPresent() && mfrc522[1].PICC_ReadCardSerial())
        {
          currentIDs[1] = dump_byte_array(mfrc522[1].uid.uidByte, mfrc522[1].uid.size);

          Serial.print(F("Reader #"));
          Serial.print(String(2));
          Serial.print(F(" on Pin #"));
          Serial.print(String(ssPins[1]));
          Serial.print(F(" detected tag: "));
          Serial.print({currentIDs[1]});
          Serial.println("");

          
          digitalWrite(LED_iza, HIGH);
          break;
        }
	  }
	  
	  digitalWrite(LED_iza, LOW);
	  digitalWrite(LED_napred, HIGH);
	  idi_lijevo();
      break;
    }
    else if (correctIDs[broj_grupe][1] == "forward-left" && currentIDs[1] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_lijevo();
      digitalWrite(LED_iza, LOW);
      while (true)
      {
        if(mfrc522[0].PICC_IsNewCardPresent() && mfrc522[0].PICC_ReadCardSerial())
        {
          currentIDs[0] = dump_byte_array(mfrc522[0].uid.uidByte, mfrc522[0].uid.size);

          Serial.print(F("Reader #"));
          Serial.print(String(1));
          Serial.print(F(" on Pin #"));
          Serial.print(String(ssPins[0]));
          Serial.print(F(" detected tag: "));
          Serial.print({currentIDs[0]});
          Serial.println("");

          
          digitalWrite(LED_iza, HIGH);
          break;
        }
      }
      break;
    }
    else if (correctIDs[broj_grupe][1] == "forward-right")
    {
      broj_grupe++;
      idi_desno();
      digitalWrite(LED_iza, LOW);
      while (true)
      {
        if(mfrc522[0].PICC_IsNewCardPresent() && mfrc522[0].PICC_ReadCardSerial())
        {
          currentIDs[0] = dump_byte_array(mfrc522[0].uid.uidByte, mfrc522[0].uid.size);

          Serial.print(F("Reader #"));
          Serial.print(String(1));
          Serial.print(F(" on Pin #"));
          Serial.print(String(ssPins[0]));
          Serial.print(F(" detected tag: "));
          Serial.print({currentIDs[0]});
          Serial.println("");

          
          digitalWrite(LED_iza, HIGH);
          break;
        }
      }
      break;
    }
    else { break; }
  }

  delay(1000);
}

void activateReader(int index) {
  for (int i = 0; i < numReaders; i++){
    digitalWrite(resetPins[i], LOW); //disable
  }

  digitalWrite(resetPins[index], HIGH); //activate
  delay(50);
}

void deactivateAll() {
  for (int i = 0; i < numReaders; i++) {
    digitalWrite(resetPins[i], LOW);
  }
}

String dump_byte_array(byte *buffer, byte bufferSize) {
    String output = "";
    for (byte i = 0; i < bufferSize; i++) {
        output += String(buffer[i], HEX);
    }
    return output;
}

void idi_desno() {
  pos = 90;
  myservo.write(pos);
}

void idi_lijevo() {
  pos = 120;
  myservo.write(pos);
}