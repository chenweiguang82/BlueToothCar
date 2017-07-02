#include <Servo.h>

const int IN_EN1 = 4;       // 左1
const int IN_EN2 = 2;       // 左2
const int IN_EN3 = 7;       // 右1
const int IN_EN4 = 8;      // 右2
const int IN_ENA = 3;       // 左使能
const int IN_ENB = 5;       // 左使能
const int IN_Servo = 9;     // 舵机
const int IN_HC_Echo = 12;
const int IN_HC_Trig = 13;
const int IN_IR_L = 11; //Left IR sensor
const int IN_IR_R = 10; //Right IR sensor

int current_servo_pos = 90;
int IR_L, IR_R;

Servo myservo;  // create servo object to control a servo 舵机

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(IN_EN1, OUTPUT);
  pinMode(IN_EN2, OUTPUT);
  pinMode(IN_EN3, OUTPUT);
  pinMode(IN_EN4, OUTPUT);
  pinMode(IN_ENA, OUTPUT);
  pinMode(IN_ENA, OUTPUT);
  myservo.attach(IN_Servo);  // attaches the servo on pin 9 to the servo object 绑定舵机端口到9
  pinMode(IN_HC_Echo, INPUT);
  pinMode(IN_HC_Trig, OUTPUT);
  pinMode(IN_IR_L, INPUT);
  pinMode(IN_IR_R, INPUT);
}

unsigned int measure_distance()
{
  unsigned int duration, distance;

  digitalWrite(IN_HC_Trig, LOW);
  delayMicroseconds(2);

  digitalWrite(IN_HC_Trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(IN_HC_Trig, LOW);
  duration = pulseIn(IN_HC_Echo, HIGH);

  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration / 58.2;

  return distance;
}

void rotate_servo(int start_pos, int end_pos)
{
  int pos;
  if (start_pos < end_pos)
  {
    for (pos = start_pos; pos <= end_pos; pos++) // goes from 0 degrees to 180 degrees in steps of 1 degree
    {
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(10);
    }
  }
  else
  {
    for (pos = start_pos; pos >= end_pos; pos--) // goes from 0 degrees to 180 degrees in steps of 1 degree
    {
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(10);
    }
  }

  current_servo_pos = end_pos;

}

void forward(int i)
{
  digitalWrite(IN_EN1, HIGH);
  digitalWrite(IN_EN2, LOW);
  analogWrite(IN_ENA, i);
  digitalWrite(IN_EN3, HIGH);
  digitalWrite(IN_EN4, LOW);
  analogWrite(IN_ENB, i);
}

void backward(int i)
{
  digitalWrite(IN_EN1, LOW);
  digitalWrite(IN_EN2, HIGH);
  analogWrite(IN_ENA, i);
  digitalWrite(IN_EN3, LOW);
  digitalWrite(IN_EN4, HIGH);
  analogWrite(IN_ENB, i);
}

void turn_right(int i)
{
  digitalWrite(IN_EN1, HIGH);
  digitalWrite(IN_EN2, LOW);
  analogWrite(IN_ENA, i);
  digitalWrite(IN_EN3, LOW);
  digitalWrite(IN_EN4, LOW);
  analogWrite(IN_ENB, 0);
}

void spin_right(int i)
{
  digitalWrite(IN_EN1, HIGH);
  digitalWrite(IN_EN2, LOW);
  analogWrite(IN_ENA, i);
  digitalWrite(IN_EN3, LOW);
  digitalWrite(IN_EN4, HIGH);
  analogWrite(IN_ENB, i);
}

void turn_left(int i)
{
  digitalWrite(IN_EN1, LOW);
  digitalWrite(IN_EN2, LOW);
  analogWrite(IN_ENA, 0);
  digitalWrite(IN_EN3, HIGH);
  digitalWrite(IN_EN4, LOW);
  analogWrite(IN_ENB, i);
}

void spin_left(int i)
{
  digitalWrite(IN_EN1, LOW);
  digitalWrite(IN_EN2, HIGH);
  analogWrite(IN_ENA, i);
  digitalWrite(IN_EN3, HIGH);
  digitalWrite(IN_EN4, LOW);
  analogWrite(IN_ENB, i);
}

void stop()
{
  digitalWrite(IN_EN1, LOW);
  digitalWrite(IN_EN2, LOW);
  analogWrite(IN_ENA, 0);
  digitalWrite(IN_EN3, LOW);
  digitalWrite(IN_EN4, LOW);
  analogWrite(IN_ENB, 0);
}

void brake()
{
  digitalWrite(IN_EN1, HIGH);
  digitalWrite(IN_EN2, HIGH);
  analogWrite(IN_ENA, 255);
  digitalWrite(IN_EN3, HIGH);
  digitalWrite(IN_EN4, HIGH);
  analogWrite(IN_ENB, 255);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i;
  char c;
  unsigned int distance, dis_left, dis_right;

  delay(3000);
  rotate_servo(current_servo_pos, 90);

manual:
  while (1)
  {
    while (Serial.available())
    {
      c = Serial.read();
      switch (c)
      {
        case 'F':
          forward(180);
          break;
        case 'B':
          backward(180);
          break;
        case 'L':
          turn_left(180);
          break;
        case 'R':
          turn_right(180);
          break;
        case 'S':
          spin_left(180);
          break;
        case 'O':
          spin_right(180);
          break;
        case 'E':
          goto automobile;
          break;
        default:
          brake();
          stop();
      }
    }
  }

automobile:
  while (1)
  {
    if (Serial.available())
    {
      c=Serial.read();
      if (c=='F' || c=='B' || c=='L' || c=='R' || c=='S' || c=='O') goto manual;
    }

    distance = measure_distance();
    if (distance <= 30 && distance >= 15)
    {
      stop();
      rotate_servo(current_servo_pos, 150);
      dis_left = measure_distance();
      rotate_servo(current_servo_pos, 30);
      dis_right = measure_distance();
      rotate_servo(current_servo_pos, 90);

      if (dis_left <= 30 && dis_right <= 30)
      {
        spin_left(255);
        delay(720);   //255,360 --->90 degree
        stop();
      }
      else if (dis_right > dis_left)
      {
        spin_right(255);
        delay(240);  //255,360 --->90 degree
        stop();
      }
      else
      {
        spin_left(255);
        delay(240);
        stop();
      }
    }
    else if (distance < 15)
    {
      backward(150);
      delay(200);
      stop();
    }
    else
    {
      IR_L = digitalRead(IN_IR_L);
      IR_R = digitalRead(IN_IR_R);
      //      Serial.print("L=");
      //      Serial.println(IR_L);
      //      Serial.print("R=");
      //      Serial.println(IR_R);
      //      delay(1000);
      if (IR_L == 0 && IR_R == 1)
      {
        turn_right(255);
      }
      else if (IR_L == 1 && IR_R == 0)
      {
        turn_left(255);
      }
      else
      {
        forward(150);
      }

    }
  }

}
