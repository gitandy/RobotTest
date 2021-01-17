#include <AFMotor.h>

AF_DCMotor m_left(3, MOTOR34_64KHZ);
AF_DCMotor m_right(4, MOTOR34_64KHZ);

int led = 13;
int led_rd = 9;
int led_gn = 10;
int btn_rd = 9;
int btn_gn = 10;
int sw_l = 16;
int sw_r = 17;
int bmp_l = 18;
int bmp_r = 19;

bool go = false;
int d = RELEASE;

// Setup things
void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  pinMode(sw_l, INPUT_PULLUP);
  pinMode(sw_r, INPUT_PULLUP);
  pinMode(bmp_l, INPUT);
  pinMode(bmp_r, INPUT);

  m_left.setSpeed(255);
  m_right.setSpeed(255);

  Serial.begin(115200);
  Serial.print("Ready!\n");
}

// Test if robot should run or stop
bool checkGo() {
  // Check buttons
  pinMode(btn_rd, INPUT);
  pinMode(btn_gn, INPUT);
  if(digitalRead(btn_rd) == 0) {
    pinMode(led_rd, OUTPUT);
    digitalWrite(led_rd, LOW);
    go = false;
  } else if(digitalRead(btn_gn) == 0) {
    pinMode(led_rd, OUTPUT);
    digitalWrite(led_rd, HIGH);
    delay(300);
    digitalWrite(led_rd, LOW);
    delay(300);
    digitalWrite(led_rd, HIGH);
    delay(300);
    digitalWrite(led_rd, LOW);
    delay(300);
    go = true;
    d = FORWARD;
  }

  pinMode(led_rd, OUTPUT);
  pinMode(led_gn, OUTPUT);
  digitalWrite(led_gn, HIGH);

  if(go == false) digitalWrite(led_rd, LOW);
  else digitalWrite(led_rd, HIGH);

  return go;
}

// Main loop
void loop() {
  if(!checkGo()){
    m_left.run(RELEASE);
    m_right.run(RELEASE);
    delay(100);
    return;
  }

  // Robot on floor?
  if (!digitalRead(sw_l) || !digitalRead(sw_r)) {
    m_left.run(RELEASE);
    m_right.run(RELEASE);
    digitalWrite(led_gn, HIGH);
  } else {
    m_left.run(d);
    m_right.run(d);
    digitalWrite(led_gn, LOW);
  }

  // Bumper touched the wall?
  if (!digitalRead(bmp_l) || !digitalRead(bmp_r)) {
    m_left.run(RELEASE);
    m_right.run(RELEASE);
    digitalWrite(led_rd, LOW);
    delay(200);

    d = BACKWARD;
    m_left.run(d);
    m_right.run(d);
    
    delay(1000);
    d = RELEASE;
  } else {
    digitalWrite(led_rd, HIGH);
  }

  if (Serial.available() > 0) {
    char dt = 0;

    m_left.run(RELEASE);
    m_right.run(RELEASE);
    delay(200);

    dt = Serial.read();
    if (dt == 'b')
      d = BACKWARD;
    else if (dt == 's')
      d = RELEASE;

    if (digitalRead(sw_l) && digitalRead(sw_r)) {
      m_left.run(d);
      m_right.run(d);
    }

    // Flush buffer
    while (Serial.available())
      Serial.read();
  }

  delay(10);
}
