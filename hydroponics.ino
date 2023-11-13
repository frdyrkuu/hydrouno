#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Initialize variables for FLOAT SENSOR
int floatSensorVal_UP;
int floatSensorVal_DOWN;
//=================================================

//Initialize Variables Relay

/*

  Relay are on NC (Normaly Close) state which means the digital write
  must be;

  HIGH = Off
  LOW = ON

*/

const int pump1 = 6;
const int pump2 = 7;

/*Custom Characters*/
byte waterDrop[] = {
  B00100,
  B00100,
  B01110,
  B01010,
  B11011,
  B11101,
  B11111,
  B01110
};

byte partitionLine[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte waterLevel[] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110
};

byte leaf[] = {
  B00100,
  B00000,
  B11111,
  B01110,
  B01110,
  B01110,
  B00100,
  B00000
};

//Controller Class for pH Sensor
class PhSensor {
  public:
    PhSensor(int pin, float calibrationValue) : pin(pin), calibrationValue(calibrationValue) {}

    void begin() {
      pinMode(pin, INPUT);
    }

    float read() {
      unsigned long int avgval = 0;
      int bufferArr[10];
      int temp;

      for (int i = 0; i < 10; i++) {
        bufferArr[i] = analogRead(pin);
        delay(30);
      }

      for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
          if (bufferArr[i] > bufferArr[j]) {
            temp = bufferArr[i];
            bufferArr[i] = bufferArr[j];
            bufferArr[j] = temp;
          }
        }
      }

      for (int i = 2; i < 8; i++) {
        avgval += bufferArr[i];
      }

      float volt = (float)avgval * 5.0 / 1024 / 6;
      float phAct = -5.70 * volt + calibrationValue;

      return phAct;
    }

  private:
    int pin;
    float calibrationValue;
};

PhSensor phSensor(A0, 21.34); // Create an instance of the pH sensor

void setup() {
  Serial.begin(9600);

  //  PinMode Float
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

  // Pinmode Relay
  pinMode(pump1, OUTPUT);
  pinMode(pump2, OUTPUT);

  /*LCD character Setup*/
  lcd.createChar(0, waterDrop);
  lcd.createChar(1, partitionLine);
  lcd.createChar(2, waterLevel);
  lcd.createChar(3, leaf);

  //  LCD
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();


  lcd.setCursor(3, 0);
  lcd.write(byte(3));

  lcd.setCursor(5, 0);
  lcd.print("HydroUNO");
  lcd.setCursor(0, 1);

  lcd.setCursor(0, 1);
  lcd.print("Hydroponics Sys.");

  lcd.print("");
  delay(1000);
  lcd.clear();

  phSensor.begin(); // Initialize the pH sensor
}

void loop() {

  // CONDITIONS FOR PH SENSOR
  float pHValue = phSensor.read();

  /*
    ===========================================================
    Conditions for floats*/

  if (digitalRead(8) == HIGH) {
    floatSensorVal_UP = 1;

    //    HIGH
    Serial.print("Float UP: ");
    Serial.println(floatSensorVal_UP);

  } else {
    floatSensorVal_UP = 0;

    //    LOW
    Serial.print("Float UP: ");
    Serial.println(floatSensorVal_UP);

  }

  if (digitalRead(9) == HIGH) {
    floatSensorVal_DOWN = 1;

    //  HIGH
    Serial.print("Float DOWN: ");
    Serial.println(floatSensorVal_DOWN);

  } else {
    floatSensorVal_DOWN = 0;

    //    LOW
    Serial.print("Float DOWN: ");
    Serial.println(floatSensorVal_DOWN);

  }

  /*
    ===========================================================
    Conditions for pH readnings*/

  char waterCondition[15];
  char waterLevel[10];

  if (pHValue >= 1 && pHValue <= 5.90) {
    strcpy(waterCondition, "Acidic");

    /*Conditional Statements for the Draining and Refilling the tank based on pH value and Float*/
    /*=========================================*/
    if (strcmp(waterCondition, "Acidic") == 0) {
      /*Condition for float values*/
      if (floatSensorVal_UP == 0 && floatSensorVal_DOWN == 0) {
        strcpy(waterLevel, "HIGH");
        //PUMP 2 will keeps draining
        digitalWrite(pump1, HIGH);
        digitalWrite(pump2, LOW);
      } else if (floatSensorVal_UP == 1 && floatSensorVal_DOWN == 0) {
        strcpy(waterLevel, "LOW");
        digitalWrite(pump1, HIGH);
        digitalWrite(pump2, LOW);
      } else if (floatSensorVal_UP == 1 && floatSensorVal_DOWN == 1) {
        strcpy(waterLevel, "EMPTY");
        digitalWrite(pump1, LOW);
        digitalWrite(pump2, HIGH);
      }
    } else {
      digitalWrite(pump1, HIGH);
      digitalWrite(pump2, HIGH);
    }

  } else if (pHValue >= 6 && pHValue <= 7.80) {
    strcpy(waterCondition, "Neutral");

    /*Conditional Statements for the Draining and Refilling the tank based on pH value and Float*/
    /*=========================================*/
    if (strcmp(waterCondition, "Neutral") == 0) {
      /*Condition for float values*/
      if (floatSensorVal_UP == 0 && floatSensorVal_DOWN == 0) {
        strcpy(waterLevel, "HIGH");
        digitalWrite(pump1, HIGH);
        digitalWrite(pump2, HIGH);
      } else if (floatSensorVal_UP == 1 && floatSensorVal_DOWN == 0) {
        strcpy(waterLevel, "LOW");
        digitalWrite(pump1, LOW);
        digitalWrite(pump2, HIGH);
      } else if (floatSensorVal_DOWN == 1 && floatSensorVal_UP == 1) {
        strcpy(waterLevel, "EMPTY");
        digitalWrite(pump1, LOW);
        digitalWrite(pump2, HIGH);
      }
    } else {
      digitalWrite(pump1, HIGH);
      digitalWrite(pump2, HIGH);
    }

  } else if (pHValue > 7) {
    strcpy(waterCondition, "Basic");

    /*Conditional Statements for the Draining and Refilling the tank based on pH value and Float*/
    /*=========================================*/
    if (strcmp(waterCondition, "Basic") == 0) {
      /*Condition for float values*/
      if (floatSensorVal_UP == 0 && floatSensorVal_DOWN == 0) {
        strcpy(waterLevel, "HIGH");
        digitalWrite(pump1, HIGH);
        digitalWrite(pump2, LOW);
      } else if (floatSensorVal_UP == 1 && floatSensorVal_DOWN == 0) {
        strcpy(waterLevel, "LOW");
        digitalWrite(pump1, LOW);
        digitalWrite(pump2, HIGH);
      } else if (floatSensorVal_UP == 1 && floatSensorVal_DOWN == 1) {
        strcpy(waterLevel, "EMPTY");
        digitalWrite(pump1, LOW);
        digitalWrite(pump2, HIGH);
      }
    } else {
      digitalWrite(pump1, HIGH);
      digitalWrite(pump2, HIGH);
    }
  }



  //  Serial Monitor
  Serial.print("pH Value:");
  Serial.print(pHValue, 2);
  Serial.println(" ");
  Serial.print("Water Condition: ");
  Serial.print(waterCondition);
  Serial.println(" ");
  Serial.print("Water Level: ");
  Serial.print(waterLevel);
  Serial.println(" ");
  Serial.print("Pump Refill: ");
  Serial.println(digitalRead(pump1));
  Serial.print("Pump Drain: ");
  Serial.println(digitalRead(pump2));

  Serial.println("=============================================");


  lcd.setCursor(0, 0);
  lcd.write(byte(1));

  lcd.setCursor(0, 1);
  lcd.write(byte(1));

  lcd.setCursor(1, 0);
  lcd.print("pH Value");

  lcd.setCursor(2, 1);
  lcd.write(byte(0));

  lcd.setCursor(4, 1);
  lcd.print(pHValue, 2);

  lcd.setCursor(10, 0);
  lcd.write(byte(2));

  lcd.setCursor(9, 0);
  lcd.write(byte(1));

  lcd.setCursor(9, 1);
  lcd.write(byte(1));

  lcd.setCursor(12, 0);
  lcd.print("Lvl");

  lcd.setCursor(10, 1);
  lcd.print(waterLevel);


  lcd.setCursor(15, 0);
  lcd.write(byte(1));

  lcd.setCursor(15, 1);
  lcd.write(byte(1));

  delay(1000);
}
