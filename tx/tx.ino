#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display TWI address
#define OLED_ADDR   0x3C
// Reset pin not used but required for library
#define RESET_PIN   -1

#define SCREEN_WIDTH 128 // Larghezza del display OLED in pixels
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT - 1);

int tailR, tailL, mapped_tailR, mapped_tailL;

int j1x = 0;
int j1y = 0;
int yaw = 0;
int pitch = 0;

int t[1] = {1};

#define J1X_PIN  A0
#define J1Y_PIN  A1
#define J2X_PIN  A2
#define J2Y_PIN  A3

char textToSend[32] = {0}; // Increased the size of the buffer to accommodate 4-digit values

RF24 radio(7, 8); // CE, CSN

void setup() {
  Serial.begin(9600);
  radio.begin();
  //radio.setRetries(15, 15);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableAckPayload();
  radio.setAutoAck(true);
  radio.stopListening();

  // Initialize with the I2C addr 0x3C (for the 128x64)
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
}

void loop() {
  //const char text[] = "Hello, world!";
  //radio.write(&text, sizeof(text));

  j1x = map(analogRead(J1X_PIN), 0, 1023, 80, 30);
  j1y = map(analogRead(J1Y_PIN), 0, 1023, 0, 180);
  
  yaw = map(analogRead(J2X_PIN), 0, 1023, -90, 90);
  pitch = map(analogRead(J2Y_PIN), 0, 1023, 90, -90);

  tailR = pitch - yaw;
  tailL = pitch + yaw;

  mapped_tailL = map(tailL, -90, 90, 180, 0);
  mapped_tailL = constrain(mapped_tailL, 0, 180);

  mapped_tailR = map(tailR, -90, 90, 0, 180);
  mapped_tailR = constrain(mapped_tailR, 0, 180);
  
  // Format the values as fixed-size character arrays
  snprintf(textToSend, sizeof(textToSend), "%d;%d;%d;%d", j1x, j1y, mapped_tailL, mapped_tailR);

  radio.write(&textToSend, sizeof(textToSend));


  if (radio.isAckPayloadAvailable()) {
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


  //delay(1000);
}