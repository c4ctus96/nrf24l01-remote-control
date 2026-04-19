#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

Servo ailerons;
Servo engine;
Servo tailL;
Servo tailR;

unsigned long lastReceivedTime = 0; // Variable to store the last time data was received

RF24 radio(7, 8); // CE, CSN

void setup() {
  Serial.begin(9600);
  ailerons.attach(3);
  engine.attach(5);
  tailL.attach(6);
  tailR.attach(9);
  radio.begin();
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
  radio.enableAckPayload();
  radio.setAutoAck(false);
  radio.setRetries(0, 0);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char input[32];
    radio.read(&input, sizeof(input));

    Serial.println(input);

    char* ptr = strtok(input, ";");
    int ailerons_cmd = atoi(ptr);

    ptr = strtok(NULL, ";");
    int engine_cmd = atoi(ptr);

    ptr = strtok(NULL, ";");
    int tailL_cmd = atoi(ptr);

    ptr = strtok(NULL, ";");
    int tailR_cmd = atoi(ptr);

    ailerons.write(ailerons_cmd);
    engine.write(engine_cmd);
    tailL.write(tailL_cmd);
    tailR.write(tailR_cmd);

    unsigned long t = millis(); // Get the current millis value
    radio.writeAckPayload(1, &t, sizeof(t)); // Send it as an unsigned long

    lastReceivedTime = millis(); // Update the last received time
  }
  else
  {
    if (millis() - lastReceivedTime > 1000) { // Check if 1 second has passed since the last received time
      // Neutralize engine and servos
      engine.write(55);
      ailerons.write(0);
      tailL.write(90);
      tailR.write(90);
    }
  }
}
