#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//------------------------------------lcd keypad------------------------------------

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);


SoftwareSerial mySerial(2, 3); //Inisialisasi serial RX TX modul fingerprint
SoftwareSerial espcam(9,10);
int buzz = 4;
int relay = 8;

int val, limS = 6;

bool keamanan = true;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(115200);
  //-----------------------------------------espcam--------------------------
  espcam.begin(9600);

  //-------------------------------------LCD---------------------------------
  lcd.begin();
  lcd.backlight();

  //-----------------------------------------buz,rel,LS--------------------------
  pinMode(buzz, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(limS, INPUT);
  digitalWrite(relay, HIGH);
  //-----------------------------------------fingerprint--------------------------
  Serial.println("nnAdafruit finger detect test");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  lcd.clear();
  lcd.setCursor(0, 0);
  
  lcd.print("Buka Dengan ");
  lcd.setCursor(0, 1);
  lcd.print("Fingerprint");
  
}

void loop() {
  
  getFingerprintID();

  val = digitalRead(limS);
  Serial.println(val);
  if (keamanan == true && val == 0){
    digitalWrite(buzz, HIGH);
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("ADA PENYUSUP");
    espcam.write("photo");
  }else{
    digitalWrite(buzz, LOW);
  }
  delay(100);
    
}

//------------------------------------------------------------
uint8_t getFingerprintID() {
    uint8_t p = finger.getImage();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println("No finger detected");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }

    // OK success!

    p = finger.image2Tz();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }

    // OK converted!---------------------------------------------
    p = finger.fingerFastSearch();
    if (p == FINGERPRINT_OK) { //jika fingerprint terdeteksi
        Serial.println("Found a print match!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PROSES . . .");
        espcam.write("photo");
        delay(5000);
        keamanan = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BERHASIL");
        lcd.setCursor(0, 1);
        lcd.print("TERIDENTIFIKASI");
        digitalWrite(relay, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("AutoLock ");
        for (int i = 9; i > 0; i--) {
            lcd.setCursor(9, 0);
            lcd.print(i);
            delay(3000);
        }
        digitalWrite(relay, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Buka Dengan ");
        lcd.setCursor(0, 1);
        lcd.print("Fingerprint");
        keamanan = true;
        
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_NOTFOUND) { //jika fingerprint salah
        Serial.println("Did not find a match");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TIDAK ");
        lcd.setCursor(0, 1);
        lcd.print("TERIDENTIFIKASI");
        espcam.write("photo");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("COBA LAGI");
        delay(2000);     
        return p;
    } else {
        Serial.println("Unknown error");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("??");
        delay(100);
        return p;
    }
    return finger.fingerID;
}
//-----------------------------------------------------------------------------
