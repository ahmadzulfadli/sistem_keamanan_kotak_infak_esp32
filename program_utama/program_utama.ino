#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//------------------------------------lcd keypad------------------------------------

LiquidCrystal_I2C lcd(0x27, 16, 2);// pin lcd yang di gunakan


SoftwareSerial mySerial(2, 3); //Inisialisasi serial RX TX modul fingerprint
SoftwareSerial espcam(9,10);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
    Serial.begin(9600);
    //-----------------------------------------espcam--------------------------
    espcam.begin(9600);
    //-----------------------------------------fingerprint--------------------------
    Serial.println("nnAdafruit finger detect test");
    // set the data rate for the sensor serial port
    finger.begin(57600);
    if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    } else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) {
            delay(1);
        }
    }
    finger.getTemplateCount();
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
    Serial.println("Waiting for valid finger...");

    lcd.begin();
    lcd.clear();

    //-----------------------------------------lcd--------------------------
    lcd.begin();
    lcd.setCursor(1, 0);
    lcd.print("TEKNIK ELEKTRO");

    lcd.setCursor(4, 1);
    lcd.print("RESTEC");
    delay(3000);

    lcd.clear();
}

void loop() {
    getFingerprintID();
    delay(50);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("Input PIN: ");
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
        lcd.print("BERHASIL");
        lcd.setCursor(0, 1);
        lcd.print("TERIDENTIFIKASI");
        lcd.setCursor(20, 0);
        espcam.write("photo");
        lcd.print("AutoLock ");
        for (int i = 5; i > 0; i--) {
            lcd.setCursor(30, 0);
            lcd.print(i);
            delay(1000);
        }
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
        delay(500);
        lcd.setCursor(20, 0);
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

    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);

    return finger.fingerID;
}
//-----------------------------------------------------------------------------

