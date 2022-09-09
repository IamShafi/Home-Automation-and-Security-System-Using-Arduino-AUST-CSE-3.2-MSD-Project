#include<Servo.h>
#include<LiquidCrystal_I2C.h>
#include<Wire.h>
#include<Keypad.h>
#include<string.h>
#include<SimpleDHT.h>
#include<dht.h>

#define dhtPin A0
#define dhtType 11
int ledPin = 12;

const byte ROWS = 4;
const byte COLS = 4;

/*
  Sensors --------Pin no
  Motion   ---------12
  Buzzer ---------11
  Servo ----------10
  Gas ------------A2
*/
int x = 0;
int mode = 0; // Buzzer - 1, cal -0
int ans = 0;
int val1;
int val2;
int buzzerPin = 11;
int flag = 0;
int passflag = 0;
int passlen = 4;
int angle = 0;
int doorstate = 0;
int gasA0 = A2;
int sensorThreshold = 300;
int pirPin = 13;
int pirState = 0;

char temp = ' ';
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
char password[4] = {'5', '6', '7', '8'};
char userinput[4];

// 2 dm array = makeKeymap(keys)
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
//Servo motor object
Servo motor;
dht newdht;


/*void redLedOn() {
  digitalWrite(RED_LED, HIGH);
  }

  void redLedOff() {
  digitalWrite(RED_LED, LOW);
  }

  void greenLedOn() {
  digitalWrite(GREEN_LED, HIGH);
  }

  void greenLedOff() {
  digitalWrite(GREEN_LED, LOW);
  }*/

void motionDetect()
{
  int j = 0;
  while (j<5)
  {
    pirState = digitalRead(pirPin);
    if (pirState == HIGH)
    {
      lcd.clear();
      lcd.print("MOTION DETECTED!");
      Serial.println("MOTION DETECTED!");
      delay(1000);
    }
    else
    {
      lcd.clear();
      lcd.print("NO MOTION.");
      Serial.println("NO MOTION.");
      delay(1000);
    }
    j++;
  }
}

void clearLCD()
{
  lcd.clear();
  lcd.print("WELCOME!");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void alarmTone()
{
  for (int i = 0; i < 30; i++)
  {
    tone(buzzerPin, 1000);
    delay(100);
    noTone(buzzerPin);
    delay(100);
  }
}

void gasDetect()
{
  lcd.clear();
  if (gasSensor()) {
    lcd.print("GAS DETECTED!");
    lcd.setCursor(0, 1);
    lcd.print("FIX IT ASAP!");
    Serial.print("GAS DETECTED! FIX IT ASAP!");
    alarmTone();
  }
  else {
    Serial.print("No gas detected.");
    lcd.print("No gas detected.");
  }
}

bool gasSensor() {
  int analogSensor = analogRead(gasA0);

  Serial.println("Pin A0: ");
  Serial.print(analogSensor);
  if (analogSensor > sensorThreshold)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void dhtFunction()
{
  int chk = newdht.read11(dhtPin);

  float tmp = newdht.temperature;
  float hmd = newdht.humidity;
  Serial.print("Temp: ");
  Serial.print(tmp);
  Serial.print("°C, ");
  Serial.print("Humidity: ");
  Serial.print(hmd);
  Serial.print("%");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print((float)tmp);
  lcd.print("°C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print((float)hmd);
  lcd.print("%");
}











void setup()
{
  Serial.begin(9600);
  motor.attach(10);
  motor.write(0);
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin,LOW);
 
 
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  //lcd.backlight();
  lcd.print("WELCOME!");
  delay(2000);
  lcd.clear();

  //MQ or gas sensor  init
  pinMode(gasA0, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}





void LightState(){
  if(Serial.available() > 0){
    char data = Serial.read();
/*
    if(data == 0){
      digitalWrite(ledPin,LOW);
      Serial.println("Light ON!");
    }
    if(data == 1) {
      digitalWrite(ledPin,HIGH);
      Serial.println("Light ON!");
    }
*/

    switch(data){
      case 'a': digitalWrite(ledPin,LOW);
                Serial.println("Light ON!");
                break;
      case 'b': digitalWrite(ledPin,HIGH);
                Serial.println("Light OFF!");
                break;
      default:break;
    }
    Serial.println(data);
  }
  delay(300);
}








void loop()
{
  // Do nothing here...
  char key = kpd.getKey();

  LightState();
  ans = 0;
  if (key && passlen != 0)
  {

    tone(buzzerPin, 1000);
    delay(50);
    noTone(buzzerPin);
    delay(50);
    Serial.println(key);

    if (key == 'C') clearLCD();

    if (passlen == 4) {
      userinput[0] = key;
      lcd.setCursor(0, 1);
      lcd.print(userinput[0]);
    }
    if (passlen == 3) {
      userinput[1] = key;
      lcd.setCursor(1, 1);
      lcd.print(userinput[1]);
    }
    if (passlen == 2) {
      userinput[2] = key;
      lcd.setCursor(2, 1);
      lcd.print(userinput[2]);
    }
    if (passlen == 1) {
      userinput[3] = key;
      lcd.setCursor(3, 1);
      lcd.print(userinput[3]);
    }

    passlen--;

    if (passlen == 0) {
      for (int i = 0; i < 4; i++)
      {
        if (password[i] != userinput[i]) {  
          passflag = 1;
          break;
        }
      }

      if (passflag == 0 ) //if password is correct
      {
        tone(buzzerPin, 3000);
        delay(500);
        noTone(buzzerPin);
        delay(50);
        Serial.println("CORRECT!");
        lcd.setCursor(0, 0);
        lcd.clear();
        lcd.print("CORRECT!");

        digitalWrite(ledPin, HIGH);
        delay(5000);
        digitalWrite(ledPin, LOW);

        //open door
        if (doorstate == 0)
        {
          angle = 90;
          motor.write(angle);
          delay(1500);

          while (true) { ///VERY IMPORTANT LOOP. ONEK JORURI LOOP. BOHAT ZARURI LOOP. SEHR WICHTIGE LOOP. LOOP MUY IMPORTANTE. LOOP TRES IMPORTANTE.
            doorstate = 1;

            dhtFunction();
            delay(1500);
           
            motionDetect();

            gasDetect();
            delay(1500);

           
          }
        }
      }
      else {
        tone(buzzerPin, 1000);
        delay(50);
        noTone(buzzerPin);
        delay(50);

        tone(buzzerPin, 1000);
        delay(50);
        noTone(buzzerPin);
        delay(50);

        tone(buzzerPin, 1000);
        delay(50);
        noTone(buzzerPin);
        delay(50);

        Serial.println("INCORRECT!");
        lcd.setCursor(0, 0);
        lcd.clear();
        lcd.print("INCORRECT!");
        delay(3000);
        lcd.clear();
        lcd.print("Try Again:");
      }
      passlen = 4;
      passflag = 0;
    }
  }
}

void closeDoor()
{
  //close door
  if (doorstate == 1)
  {
    for (angle = 90; angle <= 0; angle--)
    {
      motor.write(angle);
      delay(100);
    }
  }
}
