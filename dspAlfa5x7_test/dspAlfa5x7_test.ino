// display alphabet pattern for control Digit 32x16 dot LED module
// which has 3 BD7851 controllers
// 
// made by Vaphio @ Oct.9, 2026

#include "hankaku_v.h"  // alfa character pattern code

const int SIN1 = 2;
const int SIN2 = 3;
const int SIN3 = 4;
const int CLK = 5;
const int LATCH = 6;
const int ENABL = 7;

int PINS[6] = {SIN1, SIN2, SIN3, CLK, LATCH, ENABL};

const int dotWidth = 16;
byte charCode[dotWidth];   //storage of character pattern

char msg1[] = "AGJKBFLPQ";
char msg2[] = "123456789";
const int msgSize = sizeof(msg1) / sizeof(msg1[0]) -1;
const long codeSize = msgSize * (charWidth+1);
uint16_t dcode[16];
uint16_t ecode[16];
uint8_t strCode1[128];
uint8_t strCode2[128];
uint16_t dspCode[128];

void setup() {
  for (int i=0; i<16; i++) {
    pinMode(PINS[i], OUTPUT);
    digitalWrite(PINS[i], LOW);
  }
  sendSig(ENABL, 1);
  sendSig(LATCH, 1);
  sendSig(LATCH, 0);
  sendSig(ENABL, 0);
  fillCode(dcode, 0x0000, 16);
  fillCode(ecode, 0x0000, 16);
  setDigCode(dcode, ecode);
  getStrCode(msg1, strCode1);
  getStrCode(msg2, strCode2);

  joinCode(strCode1, strCode2, dspCode);
  cutCode(dspCode, 0, dcode);
  cutCode(dspCode, 1, ecode);
  rotCode(dcode);
  rotCode(ecode);
//  setLine(ecode);
  Serial.begin(9600);
//  prnCode(dcode);
//  prnCode(ecode);
//  prnCode(ecode);
}

void loop() {
  setDigCode(dcode, ecode);
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

void getcharCode(uint8_t* code, char alfa) {
  for (int i=0; i<charWidth; i++) {
    code[i] = HANV[alfa-codeOffset][i];
  }
}

void getStrCode(char* msg, uint8_t* code) {
  byte buf[charWidth];
  int offset = 0;
  for (int i=0; i<msgSize; i++) {
    getcharCode(buf, msg[i]);
    for (int j=0; j<charWidth; j++) {
      code[j+offset] = buf[j];
    }
    code[charWidth+offset] = 0x00;
    offset += charWidth +1;
  }
}

void cutCode(uint16_t* baseCode, int index, uint16_t* resCode) {
  for (int i=0; i<16; i++) {
    resCode[i] = baseCode[i + index*16];
  }
}

void joinCode(uint8_t* code1, uint8_t* code2, uint16_t* result) {
//  int len = sizeof(code1) / sizeof(code1[0]);
  for (int i=0; i<codeSize; i++) {
    result[i] = ((uint16_t)code1[i] << 8) | code2[i];
  }
}

void rotCode(uint16_t* code) {      // rotate 90deg of the code
  uint16_t buf[16];
  uint16_t stack;
  for (int i=0; i<16; i++) {
    for (int j=0; j<16; j++) {
      stack = (code[i] >> j) & 0x0001;
      if (stack == 1) {
        bitSet(buf[j], 15-i);
      } else {
        bitClear(buf[j], 15-i);
      }
    }
  }
  for (int i=0; i<16; i++) {
    code[i] = buf[i];
  }
}

void shiftCode(byte* code) {
  byte stack;
  stack = code[0];
  for (int i=0; i<codeSize-1; i++) {
    code[i] = code[i+1];
  }
  code[codeSize-1] = stack;
}
