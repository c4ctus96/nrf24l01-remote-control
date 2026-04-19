#include <SPI.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display TWI address
#define OLED_ADDR   0x3C
// Reset pin not used but required for library
#define RESET_PIN   -1

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT - 1);

int ailerons, engine, pitch, yaw, tailR, tailL;

int j1x, j1y, j2x, j2y;

int t[2] = {0, 0};

#define J1X_PIN  A0
#define J1Y_PIN  A1
#define J2X_PIN  A2
#define J2Y_PIN  A3

char textToSend[32] = {0};

RF24 radio(7, 8); // CE, CSN

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableAckPayload();
  radio.setAutoAck(false);
  radio.setRetries(0, 0);
  radio.stopListening();

  // Initialize with the I2C addr 0x3C (for the 128x64)
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
}

void loop() {
  j1x = analogRead(J1X_PIN);
  j1y = analogRead(J1Y_PIN);
  j2x = analogRead(J2X_PIN);
  j2y = analogRead(J2Y_PIN);

  ailerons = map(j1x, 0, 1023, 80, 30);
  engine  = map(j1y, 0, 1023, 0, 180);

  yaw   = map(j2x, 0, 1023, -90, 90);
  pitch = map(j2y, 0, 1023,  90, -90);

  tailR = pitch - yaw;
  tailL = pitch + yaw;

  tailR = map(tailR, -90, 90, 0, 180);
  tailL = map(tailL, -90, 90, 180, 0);

  tailR = constrain(tailR, 0, 180);
  tailL = constrain(tailL, 0, 180);

  snprintf(textToSend, sizeof(textToSend), "%d;%d;%d;%d", ailerons, engine, tailL, tailR);

  radio.write(&textToSend, sizeof(textToSend));

  if (radio.isAckPayloadAvailable())
  {
    unsigned long ackPayload;//[32] = "";
    radio.read(&ackPayload, sizeof(ackPayload));
    Serial.println(ackPayload);
    t[1] = t[0];
    t[0] = int(ackPayload);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(2, 5);
    display.print("dt:");
    display.setCursor(40, 5);
    display.print(t[0] - t[1]);
    display.display();
  }
  else
  {
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.fillScreen(WHITE);
    display.setTextSize(2);
    display.setCursor(12, 9);
    display.print("NO SIGNAL");
    display.display();
  }
}
