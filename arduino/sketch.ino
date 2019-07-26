//#include <FastLED.h>
#include <U8x8lib.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>


#define NUM_LEDS 1
#define DATA_PIN 1
#define RxD 4 // 3 (PB4)
#define TxD 3 // 2 (PB3)

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(2, 0, U8X8_PIN_NONE);
Adafruit_NeoPixel pixels(DATA_PIN, NUM_LEDS, NEO_GRB + NEO_KHZ800);
SoftwareSerial serial(RxD, TxD);

void setup() {
  serial.begin(9600);
  serial.print("AT+NAMEw123456");
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_u);
  pixels.begin();
  pixels.clear();

  u8x8.setPowerSave(0);

  for (int i = 0; i < 20; i++) serial.write((byte) 0);
  serial.flush();
}

void loop() {
  if (serial.available() > 0 && serial.read() == 'W') {
    update(serial);
  }
}

void update(Stream &stream) {
  u8x8.clear();
  // work order
  char text[14] = "W";
  char piece[4];
  for (int j = 0; j < 3; j++) {
    int i = stream.read();
    itoa(i >> 4, piece, 10);
    strcat(text, piece);
    itoa(i & 0x0F, piece, 10);
    strcat(text, piece);
  }
  u8x8.draw2x2String((8 - strlen(text)) / 2, 1, text);

  // led
  int r = stream.read();
  int g = stream.read();
  int b = stream.read();
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.show();

  // date [dddddmmmmmyyyyyy];
  text[0] = 0;
  short buf;
  buf += stream.read() << 8;
  buf += stream.read();
  // y
  itoa((buf & 0x3F) + 2000, piece, 10);
  strcat(text, piece);
  strcat(text, "/");
  buf = buf >> 6;
  // m
  itoa(buf & 0x1F, piece, 10);
  strcat(text, piece);
  strcat(text, "/");
  buf = buf >> 5;
  // d
  itoa(buf & 0x1F, piece, 10);
  strcat(text, piece);
  u8x8.drawString((16 - strlen(text)) / 2, 6, text);

  text[0] = 0;
  for (int j = 0; j < 11; j++) {
    text[j] = stream.read();
  }
  //stream.readBytesUntil('\0', text, 11);
  u8x8.drawString((16 - strlen(text)) / 2, 4, text);
}
