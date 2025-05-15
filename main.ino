#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define LCD_ADDR 0x27
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

const int potPin    = A0;
const int buttonPin = 2;
const int servoPin  = 9;  // Servo motor sinyal pini

Servo myServo;

int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

int correctAnswers = 0;
int currentQuestion = 0;
int correctResult = 0;
bool waitingForAnswer = true;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buttonPin, INPUT_PULLUP);
  myServo.attach(servoPin);
  myServo.write(0);  // Başlangıç konumu
  randomSeed(analogRead(A1));
  generateQuestion();
}

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    static int buttonState = HIGH;
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW && waitingForAnswer) {
        int potValue = analogRead(potPin);
        int answer = map(potValue, 0, 1023, 0, 100);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Senin cevab: ");
        lcd.setCursor(0, 1);
        lcd.print(answer);
        delay(1000);

        if (answer == correctResult) {
          correctAnswers++;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Dogru!");
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Yanlis!");
          lcd.setCursor(0, 1);
          lcd.print("Cevap: ");
          lcd.print(correctResult);
        }

        delay(1500);

        currentQuestion++;
        if (currentQuestion < 3) {
          generateQuestion();
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          if (correctAnswers == 3) {
            lcd.print("Tebrikler!");
            // Servo motoru döndür
            myServo.write(90);
            delay(3000);
            myServo.write(0);
          } else {
            lcd.print("Tekrar deneyin");
          }
          correctAnswers = 0;
          currentQuestion = 0;
          delay(3000);
          generateQuestion();
        }
      }
    }
  }

  lastButtonState = reading;

  if (waitingForAnswer) {
    int potValue = analogRead(potPin);
    int displayValue = map(potValue, 0, 1023, 0, 100);

    lcd.setCursor(0, 1);
    lcd.print("Secim: ");
    if (displayValue < 10) lcd.print(' ');
    lcd.print(displayValue);
    lcd.print("   ");
  }
}

void generateQuestion() {
  waitingForAnswer = true;
  int a, b, op;
  bool valid = false;

  while (!valid) {
    a = random(1, 101);
    b = random(1, 101);
    op = random(0, 4);

    switch (op) {
      case 0:
        correctResult = a + b;
        valid = (correctResult >= 0 && correctResult <= 100);
        break;
      case 1:
        correctResult = a - b;
        valid = (correctResult >= 0 && correctResult <= 100);
        break;
      case 2:
        correctResult = a * b;
        valid = (correctResult >= 0 && correctResult <= 100);
        break;
      case 3:
        if (b == 0) break;
        if (a % b != 0) break;
        correctResult = a / b;
        valid = (correctResult >= 0 && correctResult <= 100);
        break;
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  switch (op) {
    case 0:
      lcd.print(a); lcd.print("+"); lcd.print(b); lcd.print("=?");
      break;
    case 1:
      lcd.print(a); lcd.print("-"); lcd.print(b); lcd.print("=?");
      break;
    case 2:
      lcd.print(a); lcd.print("*"); lcd.print(b); lcd.print("=?");
      break;
    case 3:
      lcd.print(a); lcd.print("/"); lcd.print(b); lcd.print("=?");
      break;
  }
}
