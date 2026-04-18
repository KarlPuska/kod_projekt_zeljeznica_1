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

#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

#include <Servo.h>

// --- Pin definitions ---
const int ssPins[]    = {2, 4};
const int resetPins[] = {29, 28};
const int numReaders  = 2;

// --- Driver & reader setup ---
MFRC522DriverPinSimple ss_pins[numReaders] = { ssPins[0], ssPins[1] };
MFRC522DriverSPI drivers[numReaders] = { ss_pins[0], ss_pins[1] };
MFRC522 mfrc522[numReaders] = { drivers[0], drivers[1] };

String currentUIDs[numReaders] = {"", ""};

const int LED_napred = 23;
const int LED_iza = 22;

Servo myservo;
const int servo = 5;
int pos = 90;
// pos-max: 120; pos-min: 90

const String correctIDs[][2] = {
  {"62eeca06", "left"},
  {"e6531106", "forward-right"},
  {"7a95c906", "forward-left"},
  {"553b1506", "right"},
  {"126ec906", "forward-left"},
  {"57df1106", "left"}
};
int numGroups = sizeof(correctIDs) / sizeof(correctIDs[0]);
int broj_grupe = 0;

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  Serial.println(F("Serial communication started"));
  #endif

  // Pull all SS pins HIGH before SPI starts to prevent bus conflicts
  for (int i = 0; i < numReaders; i++) {
    pinMode(ssPins[i], OUTPUT);
    digitalWrite(ssPins[i], HIGH);
  }

  SPI.begin();
  for (int i = 0; i < numReaders; i++) {
    pinMode(resetPins[i], OUTPUT);
    digitalWrite(resetPins[i], LOW);
    delay(50);
    digitalWrite(resetPins[i], HIGH);
    delay(100);

    mfrc522[i].PCD_Init();
    delay(200);

    #ifdef DEBUG
    Serial.print(F("Reader #"));
    Serial.print(i);
    Serial.print(F(" initialised on SS pin "));
    Serial.print(ssPins[i]);
    Serial.print(F(". Antenna strength: "));
    Serial.print(mfrc522[i].PCD_GetAntennaGain());
    Serial.print(F(". Version: "));
    MFRC522Debug::PCD_DumpVersionToSerial(mfrc522[i], Serial);
    #endif
  }

  pinMode(LED_napred, OUTPUT);
  digitalWrite(LED_napred, HIGH);
  pinMode(LED_iza, OUTPUT);
  digitalWrite(LED_iza, HIGH);

  myservo.attach(servo);
  myservo.write(pos);  

  Serial.println(F("--- END SETUP ---"));
}

void loop() {

  for (int i = 0; i < numReaders; i++) {
    String readRFID = "";

    if (mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial())
    {
      readRFID = getUID(i);
      delay(20);
    }

    if(readRFID != currentUIDs[i])
    {
      currentUIDs[i] = readRFID;
    }

    delay(150);
  }

  for (int i = 0; i < numReaders; i++) {
    #ifdef DEBUG
    Serial.print(F("Reader #"));
    Serial.print(i+1);
    Serial.print(F(" detected card ID: "));
    Serial.println(currentUIDs[i]);
    #endif
  }

  Serial.println(F("---"));
  Serial.println("");

  if (broj_grupe < numGroups)
  {
    if (correctIDs[broj_grupe][1] == "right" && currentUIDs[0] == correctIDs[broj_grupe][0])
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
    }
    else if (correctIDs[broj_grupe][1] == "left" && currentUIDs[0] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_lijevo();
      digitalWrite(LED_napred, LOW);
      while (true)
      {
        if(mfrc522[1].PICC_IsNewCardPresent() && mfrc522[1].PICC_ReadCardSerial())
        {
          currentUIDs[1] = getUID(1);

          Serial.print(F("Reader #"));
          Serial.print(String(2));
          Serial.print(F(" on Pin #"));
          Serial.print(String(ssPins[1]));
          Serial.print(F(" detected tag: "));
          Serial.print({currentUIDs[1]});
          Serial.println("");

          
          digitalWrite(LED_iza, HIGH);
          break;
        }
	  }
	    digitalWrite(LED_napred, HIGH);
	  idi_lijevo();
    }
    else if (correctIDs[broj_grupe][1] == "forward-left" && currentUIDs[1] == correctIDs[broj_grupe][0])
    {
      broj_grupe++;
      idi_lijevo();
      digitalWrite(LED_iza, LOW);
      while (true)
      {
        if(mfrc522[0].PICC_IsNewCardPresent() && mfrc522[0].PICC_ReadCardSerial())
        {
          currentUIDs[0] = getUID(0);

          Serial.print(F("Reader #"));
          Serial.print(String(1));
          Serial.print(F(" on Pin #"));
          Serial.print(String(ssPins[0]));
          Serial.print(F(" detected tag: "));
          Serial.print({currentUIDs[0]});
          Serial.println("");

          
          digitalWrite(LED_iza, HIGH);
          break;
        }
      }
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
          currentUIDs[0] = getUID(0);

          Serial.print(F("Reader #"));
          Serial.print(String(1));
          Serial.print(F(" on Pin #"));
          Serial.print(String(ssPins[0]));
          Serial.print(F(" detected tag: "));
          Serial.print({currentUIDs[0]});
          Serial.println("");

          
          digitalWrite(LED_iza, HIGH);
          break;
        }
      }
    }
  }

  delay(1000);
}

String getUID(int readerIndex) {
  String uid = "";
  for (byte j = 0; j < mfrc522[readerIndex].uid.size; j++) {
    if (mfrc522[readerIndex].uid.uidByte[j] < 0x10) uid += "0";
    uid += String(mfrc522[readerIndex].uid.uidByte[j], HEX);
  }
  return uid;
}

void idi_desno() {
  pos = 90;
  myservo.write(pos);
}

void idi_lijevo() {
  pos = 120;
  myservo.write(pos);
}
