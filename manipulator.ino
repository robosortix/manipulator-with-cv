//Манипулятор на кнопке

#include <Servo.h>

const int pinBtn = 4;
int btnCounter = 0;
int btnState = 0;
int lastBtnState = 0;
bool buttonFlag = false;
String obj_class = "";
String angleStr = "";
int angle, tilt_angle, fin_angle;

Servo servo9; // захват
Servo servo11; // сервопривод наклона
Servo servo13; // сервопривод поворота
Servo motor;

void setup() {
  pinMode(pinBtn, INPUT);
  Serial.begin(9600);
  servo13.attach(13);
  servo13.write(0);
  servo13.detach();
  delay(500);
  servo11.attach(11);
  servo11.write(90);
  //servo11.detach();
}

void loop() {
  delay(100);
  btnState = digitalRead(pinBtn);

  if (btnState != lastBtnState) {
    if (btnState != 1) {

      Serial.println("button event");
      
      angle = serialReadAngle();
      tilt_angle = serialReadAngle() + 60;
      obj_class = serialReadObjectClass();

      Serial.println(obj_class);

      doWork(angle, tilt_angle, obj_class); // манипулятор выполняет полезную работу
    }
    lastBtnState = btnState;
  }
}

int serialReadAngle() {
   String rec = "";
   char symbol;
   do {
      symbol = Serial.read();
      if (symbol != '\xFF') {
        if (symbol == '\x0A') {
          break;
        } else {
          rec += symbol;
        }
      }
    } while (1);
  return rec.toInt();
}

String serialReadObjectClass() {
   String rec = "";
   char symbol;
   do {
      symbol = Serial.read();
      if (symbol != '\xFF') {
        if (symbol == '\x0A') {
          break;
        } else {
          rec += symbol;
        }
      }
    } while (1);
  return rec;
}

void doWork(int angle, int tilt_angle, String cls) {
  servo11.attach(11);
  //servo11.write(100);
  servo13.attach(13);
  servoSmoothRotationM(servo13, 0, angle);
  servo9.attach(9); // захват
  servo9.write(0);
  servoSmoothRotationM(servo11, 90, tilt_angle);
  //servoSmoothRotationM(servo11, 180, tilt_angle); 
  //motor.attach(10);
  //motor.write(1200);
  //delay(2100);
  servo9.write(100);
  //motor.write(1700);
  //delay(3600);
  //motor.detach();
  servoSmoothRotationM(servo11, tilt_angle, 90);
  
  if (cls.indexOf("plastic") != -1) {
    servoSmoothRotationM(servo13, angle, 50);
    delay(1000);
    fin_angle = 150;
    servoSmoothRotationM(servo11, 90, fin_angle);
    delay(1000);
    servo9.write(0); 
    delay(500);
    servo9.detach();
    servoSmoothRotationM(servo13, 50, 0);
    
  } else if (cls.indexOf("metal") != -1) {
    servoSmoothRotationM(servo13, angle, 35);
    delay(1000);
    fin_angle = 130;
    servoSmoothRotationM(servo11, 90, fin_angle);
    delay(1000);
    servo9.write(0); 
    delay(500);
    servo9.detach();
    servoSmoothRotationM(servo13, 35, 0);
    
  } else {
    servoSmoothRotationM(servo13, angle, 10);
    delay(1000);
    fin_angle = 140;
    servoSmoothRotationM(servo11, 90, fin_angle);
    delay(1000);
    servo9.write(0);
    delay(500);
    servo9.detach();
  }
  
  servoSmoothRotationM(servo11, fin_angle, 90);
  //servo11.detach();
  servo13.detach();
}

void servoSmoothRotationM(Servo &s, int angle1, int angle2) {
  // входные параметры функции:
  // s - ссылка на объект класса Servo (передаем в функцию сервопривод, который
  // нужно повернуть на определенный угол) angle1 - начальный угол поворота
  // сервопривода angle2 - конечный угол поворота сервопривода
  int step;
  int i = 0;

  if (angle1 > angle2) {
    step = -1; // шаг поворота = -2 град
    i = angle1;
    do {
      i += step;
      s.write(i); // плавно поворачиваем сервопривод с шагом -1 град.
      delay(25);
    } while (i >= angle2);
  } else {
    step = 1; // шаг поворота = +2 град
    i = angle1;
    do {
      i += step;
      s.write(i); // плавно поворачиваем сервопривод с шагом +1 град.
      delay(25);
    } while (i <= angle2);
  }
}

void servoSmoothRotation(Servo &s, int angle) {
  // входные параметры функции:
  // s - ссылка на объект класса Servo (передаем в функцию сервопривод, который
  // нужно повернуть на определенный угол) angle - угол поворота сервопривод
  int step;
  int i = 0;

  if (angle < 0) { // если угол поворота отрицательный
    step = -1;     // шаг поворота = -2 град
    i = abs(angle);
    do {
      i += step;
      s.write(i); // плавно поворачиваем сервопривод с шагом -1 град.
      delay(25);
    } while (i > 0);

  } else {    // если угол поворота положительный
    step = 1; // шаг поворота = 2 град
    do {
      i += step;
      s.write(i); // плавно поворачиваем сервопривод с шагом +1 град.
      delay(25);
    } while (i <= angle);
  }
}
