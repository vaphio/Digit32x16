// display alphabet pattern for control Digit 32x16 dot LED module
// which has 3 BD7851 controllers
// display pattern hedder file
// made by Vaphio @ Oct.14, 2026

//#include "hankaku_v.h"  // alfa character pattern code
//#include "aiueo.h"      // pattern file
#include "denshi.h"

const int SIN1 = 2;
const int SIN2 = 3;
const int SIN3 = 4;
const int CLK = 5;
const int LATCH = 6;
const int ENABL = 7;

int PINS[6] = {SIN1, SIN2, SIN3, CLK, LATCH, ENABL};

const int dotWidth = 16;
long dspIndex = 0;
int wait = 1;
uint16_t dcode[16];
uint16_t ecode[16];

void setup() {
  for (int i=0; i<16; i++) {
    pinMode(PINS[i], OUTPUT);
  }
}

void loop() {
  cutCode(dspIndex, ptnCode, dcode);
  cutCode(dspIndex+16, ptnCode, ecode);
  rotCode(dcode);
  rotCode(ecode);
  for (int i=0; i<wait; i++) {
    setDigCode(dcode, ecode);
  }
  dspIndex = (dspIndex+1) % codeWidth;
}

void fillCode(uint16_t* code, uint16_t data, int size) {
  for (int i=0; i<size; i++) {
    code[i] = data;
  }
}

void setLine(uint16_t* code) {
  uint16_t line = 0x0001;
  for (int i=0; i<16; i++) {
    code[i] = line;
    line = line << 1;
  }
}

void fmtCode(uint8_t* orgCode, uint16_t* tgtCode, int offset) {
  uint16_t line = 0x0000;
  fillCode(tgtCode, 0x0000, 16);
  for (int i=0; i<16; i++) {
    line = (orgCode[i] << 8) | (orgCode[i+5]);
    tgtCode[i] = line;
  }
}

void sendSig(int pin, int sig) {
  digitalWrite(pin, sig);
  digitalWrite(CLK, HIGH);
  delay(1);
  digitalWrite(CLK, LOW);
}

void setDigCode(uint16_t* code1, uint16_t* code2) {
  int pin;
  uint16_t sel = 0x0001;
  sendSig(ENABL, 0);
  sendSig(LATCH, 0);
  for (int i=0; i<16; i++) {
    digitalWrite(LATCH, LOW);
    for (int j=0; j<16; j++) {
      digitalWrite(CLK, HIGH);
      digitalWrite(SIN1, (sel>>(15-j)) & 0x0001);
      digitalWrite(SIN2, (code1[i] >> (j)) & 0x0001);
      digitalWrite(SIN3, (code2[i] >> (j)) & 0x0001);
      digitalWrite(CLK, LOW);
    }
    sendSig(LATCH, 1);
    delay(1);
    sel = sel << 1;
  }
}

void prnCode(uint16_t* code) {
  int size = sizeof(code) / sizeof(code[0]);
//  int size = 16;
  for (int i=0; i<size; i++) {
    Serial.println(code[i], BIN);
  }
}

void clearCode(byte* code, int size) {
  for (int i=0; i<size; i++) {
    code[i] = 0x00;
  }
}

void cutCode(int index, uint16_t* orgCode, uint16_t* result) {
  for (int i=0; i<dotWidth; i++) {
    result[i] = orgCode[(i + index) % codeWidth];
  }
}

void rotCode(uint16_t* code) {      // rotate 90deg of the code
  uint16_t buf[dotWidth];
  uint16_t stack;
  for (int i=0; i<dotWidth; i++) {
    for (int j=0; j<dotWidth; j++) {
      stack = (code[i] >> j) & 0x0001;
      if (stack == 1) {
        bitSet(buf[j], 15-i);
      } else {
        bitClear(buf[j], 15-i);
      }
    }
  }
  for (int i=0; i<dotWidth; i++) {
    code[i] = buf[i];
  }
}

void shiftCode(uint16_t* code) {
  uint16_t stack;
  stack = code[0];
  for (int i=0; i<codeWidth-1; i++) {
    code[i] = code[i+1];
  }
  code[codeWidth-1] = stack;
}
