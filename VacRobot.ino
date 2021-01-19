#include <AFMotor.h>

#define LN_SENS A1
#define LN_LINE 700
#define LN_STEP 900
#define LED 13
#define LED_RD 9
#define LED_GN 10
#define BTN_RD 9
#define BTN_GN 10
#define SW_L 16
#define SW_R 17
#define BMP_L 18
#define BMP_R 19

AF_DCMotor m_left(3, MOTOR34_64KHZ);
AF_DCMotor m_right(4, MOTOR34_64KHZ);

bool go = false;
int d = RELEASE;
bool ln_detected = false;
unsigned long start = 0;

// Setup things
void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(SW_L, INPUT_PULLUP);
  pinMode(SW_R, INPUT_PULLUP);
  pinMode(BMP_L, INPUT);
  pinMode(BMP_R, INPUT);
  pinMode(LN_SENS, INPUT);

  // Set speed to max. cause currently PWM 
  // does not work with the motors
  m_left.setSpeed(255);
  m_right.setSpeed(255);

  Serial.begin(115200);
  Serial.print("Ready!\n");
}

// Test if robot should run or stop
bool checkGo() {
  // Check buttons
  pinMode(BTN_RD, INPUT);
  pinMode(BTN_GN, INPUT);
  if(digitalRead(BTN_RD) == 0) {
    pinMode(LED_RD, OUTPUT);
    digitalWrite(LED_RD, LOW);
    go = false;
    ln_detected = false;
  } else if(digitalRead(BTN_GN) == 0) {
    pinMode(LED_RD, OUTPUT);
    digitalWrite(LED_RD, HIGH);
    delay(300);
    digitalWrite(LED_RD, LOW);
    delay(300);
    digitalWrite(LED_RD, HIGH);
    delay(300);
    digitalWrite(LED_RD, LOW);
    delay(300);
    go = true;
    d = FORWARD;
    start = millis();
  }

  pinMode(LED_RD, OUTPUT);
  pinMode(LED_GN, OUTPUT);
  digitalWrite(LED_GN, HIGH);

  if(go == false) digitalWrite(LED_RD, LOW);
  else digitalWrite(LED_RD, HIGH);

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
  if (!digitalRead(SW_L) || !digitalRead(SW_R)) {
    m_left.run(RELEASE);
    m_right.run(RELEASE);
    digitalWrite(LED_GN, HIGH);
  } else {
    m_left.run(d);
    m_right.run(d);
    digitalWrite(LED_GN, LOW);
  }

  // Found line?
  if(analogRead(LN_SENS) > LN_LINE && (millis() - start) > 1000) {
    ln_detected = true;
    Serial.println("Line detected!");
    delay(200);
  }

  // Follow line
  if(ln_detected && analogRead(LN_SENS) < LN_LINE) {
    m_left.run(RELEASE);
    delay(10);
    m_left.run(FORWARD);
  }

  // Bumper touched the wall?
  if (!digitalRead(BMP_L) || !digitalRead(BMP_R)) {
    m_left.run(RELEASE);
    m_right.run(RELEASE);
    digitalWrite(LED_RD, LOW);
    delay(200);

    d = BACKWARD;
    m_left.run(d);
    m_right.run(d);
    
    delay(1000);
    d = RELEASE;
    go = false;
  } else {
    digitalWrite(LED_RD, HIGH);
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

    if (digitalRead(SW_L) && digitalRead(SW_R)) {
      m_left.run(d);
      m_right.run(d);
    }

    // Flush buffer
    while (Serial.available())
      Serial.read();
  }

  delay(10);
}
