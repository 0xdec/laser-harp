void step(int16_t);

#define DIR_PIN 2
#define STEP_PIN 3
#define SLEEP_PIN 4
#define LASER_PIN 5



void setup()
{
	Serial.begin(57600);
	
	//Set the correct pins as outputs
	pinMode(DIR_PIN, OUTPUT);
	pinMode(STEP_PIN, OUTPUT);
	pinMode(SLEEP_PIN, OUTPUT);
	pinMode(LASER_PIN, OUTPUT);

	digitalWrite(SLEEP_PIN, HIGH);
}

void loop()
{
	while(true)
	{
		step(200, 2);
		step(-200, 2);
	}

	for (uint8_t i = 0; i < 12; i++)
	{
		step(2, 1600);
		//delay(2);
		digitalWrite(LASER_PIN, HIGH);
		delay(1);
		digitalWrite(LASER_PIN, LOW);
	}
	for (uint8_t i = 0; i < 12; i++)
	{
		step(-2, 1600);
		//delay(2);
		digitalWrite(LASER_PIN, HIGH);
		delay(1);
		digitalWrite(LASER_PIN, LOW);
	}
}

void step(int16_t nStep, uint32_t nDelay)
{
	Serial.println(map(analogRead(A0), 0, 1023, 1, 1000)); ////////////////

	bool bDirection = (nStep > 0)? 1:0;
	nStep = abs(nStep);

	digitalWrite(DIR_PIN, bDirection); //Set both direction pins correctly
	//delay(2);

	for (uint32_t i = 0; i < nStep; i++)
	{
		digitalWrite(STEP_PIN, HIGH); //Set the step pins if necessary
		delayMicroseconds(2);
		digitalWrite(STEP_PIN, LOW); //Set both step pins low again

		//delay(map(analogRead(A0), 0, 1023, 1, 1000)); ////////////////
		//delayMicroseconds(nDelay);
	}

	//digitalWrite(DIR_PIN, !bDirection); //Set both direction pins correctly

	//digitalWrite(SLEEP_PIN, LOW);
}