#define LASER_PIN 2
#define MOTOR_PIN 3
#define SENSOR_PIN A0

#define SPEED 3

int64_t nTime = 0;



void setup()
{
  //Serial.begin(57600);

  //Set the correct pins as inputs/outputs
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(LASER_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  while(millis() < 1000) {
    motor(0);
  }

  digitalWrite(LASER_PIN, HIGH);
}

void loop()
{
  motor(SPEED);

  if (analogRead(SENSOR_PIN) > 150)
  {
    //Serial.println(micros());

    nTime = micros() - nTime;
    /*
    while (micros() < micros() + nTime)
    {
      motor(SPEED);
      delayMicroseconds(10);
    }
    */
    //delayMicroseconds(nTime * 3 / 8);
    nTime = micros();

    digitalWrite(13, HIGH);
    delayMicroseconds(500);
    digitalWrite(13, LOW);

    /*
    for (uint8_t i = 0; i < 12; i++)
    {
      motor(SPEED);
      delay((nTime / 6) - 20);

      digitalWrite(LASER_PIN, HIGH);
      delayMicroseconds(500);
      digitalWrite(LASER_PIN, LOW);
    }
    */
  }

  delayMicroseconds(100);
}

// Motor function: the first parameter is the speed the motor should spin at
void motor(int nSpeed)
{
  // Interpret the input parameter
  int nPulse = map(nSpeed, 0, 100, 1000, 2000);
  long nDelayMillis = millis() + 20000;

  // Send data to the motor
  digitalWrite(MOTOR_PIN, HIGH);
  delayMicroseconds(nPulse);
  digitalWrite(MOTOR_PIN, LOW);

  while ((millis() - nDelayMillis) < 0) {
    delayMicroseconds(1);
  }
}
