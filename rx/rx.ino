#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

Servo ailerons;
Servo engine;
Servo tail1;
Servo tail2;

int angle1, angle2, angle3, angle4;
unsigned long lastReceivedTime = 0; // Variable to store the last time data was received

RF24 radio(7, 8); // CE, CSN

void setup() {
  Serial.begin(9600);
  ailerons.attach(3);
  engine.attach(5);
  tail1.attach(6);
  tail2.attach(9);
  radio.begin();
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
  radio.enableAckPayload();
  radio.setAutoAck(true);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char input[32] = "";
    radio.read(&input, sizeof(input));

    Serial.println(input);

    unsigned long t = millis(); // Get the current millis value
    radio.writeAckPayload(1, &t, sizeof(t)); // Send it as an unsigned long

    char* ptr = strtok(input, ";"); // Split the input string by semicolon

    // Convert the values to integer and write them to the servos
    engine.write(atoi(ptr));
    ptr = strtok(NULL, ";");
    ailerons.write(atoi(ptr));
    ptr = strtok(NULL, ";");
    tail1.write(atoi(ptr));
    ptr = strtok(NULL, ";");
    tail2.write(atoi(ptr));

    lastReceivedTime = millis(); // Update the last received time
  } else {
    // Check if 1 second has passed since the last received time
    if (millis() - lastReceivedTime > 1000) {
      // Set all servos to 0
      engine.write(55);
      ailerons.write(0);
      tail1.write(90);
      tail2.write(90);
    }
  }
}